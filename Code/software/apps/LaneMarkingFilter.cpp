/*
 * LaneMarkingFilter.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: mdailey
 */

#include <fstream>

#include "LaneMarkingFilter.h"
#include "LaneMarkingDetector.h"

#define STATE_VECTOR_LENGTH 6

#define I_STATE_XPOSITION 0
#define I_STATE_XDOT 1
#define I_STATE_YAW 2
#define I_STATE_LANE_WIDTH 3
#define I_STATE_NEAR_DIST 4
#define I_STATE_NEAR_YAW 5

#define MSMT_VECTOR_LENGTH 6

#define I_MSMT_LEFT_DIST 0
#define I_MSMT_LEFT_YAW 1
#define I_MSMT_RIGHT_DIST 2
#define I_MSMT_RIGHT_YAW 3
#define I_MSMT_NEAR_DIST 4
#define I_MSMT_NEAR_YAW 5

LaneMarkingFilter::LaneMarkingFilter(double dExpectedWidth)
{

    _pMatH = NULL;
    _dExpectedLaneWidth = dExpectedWidth;

    _cUpdatesSinceReset = 0;
    _historyNearest.clear();

    _pUtil = new Util();

    _pRANSAC = new RANSACEstimator();

    _matXhat.create(STATE_VECTOR_LENGTH, 1, CV_32FC1);
    _matPhat.create(STATE_VECTOR_LENGTH, STATE_VECTOR_LENGTH, CV_32FC1);
    this->resetState();

    _matZ.create(MSMT_VECTOR_LENGTH, 1, CV_32FC1);

    this->initStateTransitionModel();
    this->initSensorModel();
}

LaneMarkingFilter::~LaneMarkingFilter()
{
    if (_pUtil)
        delete _pUtil;
}

void LaneMarkingFilter::initStateTransitionModel()
{
    _matF.create(STATE_VECTOR_LENGTH, STATE_VECTOR_LENGTH, CV_32FC1);
    _matQ.create(STATE_VECTOR_LENGTH, STATE_VECTOR_LENGTH, CV_32FC1);

    // State transition function
    //
    // x = x + xdot * delta_t
    // xdot = xdot
    // yaw = yaw
    // lanewidth = lanewidth
    // nearestdist = nearestdist
    // nearestyaw = nearestyaw
    //
    // F = [ 1 delta_t 0 0 0 0
    //       0    1    0 0 0 0
    //       0    0    1 0 0 0
    //       0    0    0 1 0 0
    //       0    0    0 0 1 0
    //       0    0    0 0 0 1 ];

// TODO: calculate DELTA_T based on time stamps. But currently, velocity seems not to be useful
#define DELTA_T 0.0

    _matF.setTo(0);
    _matF.at<float>(I_STATE_XPOSITION, I_STATE_XPOSITION) = 1.0;
    _matF.at<float>(I_STATE_XPOSITION, I_STATE_XDOT) = DELTA_T;
    _matF.at<float>(I_STATE_XDOT, I_STATE_XDOT) = 1.0;
    _matF.at<float>(I_STATE_YAW, I_STATE_YAW) = 1.0;
    _matF.at<float>(I_STATE_LANE_WIDTH, I_STATE_LANE_WIDTH) = 1.0;
    _matF.at<float>(I_STATE_NEAR_DIST, I_STATE_NEAR_DIST) = 1.0;
    _matF.at<float>(I_STATE_NEAR_YAW, I_STATE_NEAR_YAW) = 1.0;

    // State noise per frame
    // xpos: small (e.g. 1cm per frame)
    // xdot: small (e.g. 1 m/s/s or 0.1 m/s/frame)
    // yaw: small (e.g. 1 deg/frame)
    // lane width: bigger (e.g. 10 cm per frame)
    // nearest dist: bigger (e.g. 10 cm per frame)
    // nearest yaw: small (1 degree per frame)

    _matQ.setTo(0);
    _matQ.at<float>(I_STATE_XPOSITION, I_STATE_XPOSITION) = 0.01 * 0.01;
    _matQ.at<float>(I_STATE_XDOT, I_STATE_XDOT) = 0.1 * 0.1;
    _matQ.at<float>(I_STATE_YAW, I_STATE_YAW) = 2.5 / 180 * M_PI * 2.5
            / 180* M_PI;
    _matQ.at<float>(I_STATE_LANE_WIDTH, I_STATE_LANE_WIDTH) = 0.1 * 0.1;
    _matQ.at<float>(I_STATE_NEAR_DIST, I_STATE_NEAR_DIST) = 0.1 * 0.1;
    _matQ.at<float>(I_STATE_NEAR_YAW, I_STATE_NEAR_YAW) = 1.0 / 180 * M_PI * 1.0
            / 180 * M_PI;
}

void LaneMarkingFilter::initSensorModel()
{
    _matH.create(MSMT_VECTOR_LENGTH, STATE_VECTOR_LENGTH, CV_32FC1);
    _matHc.create(MSMT_VECTOR_LENGTH, 1, CV_32FC1);
    _matRfull.create(MSMT_VECTOR_LENGTH, MSMT_VECTOR_LENGTH, CV_32FC1);
    _matRleft.create(MSMT_VECTOR_LENGTH, MSMT_VECTOR_LENGTH, CV_32FC1);
    _matRright.create(MSMT_VECTOR_LENGTH, MSMT_VECTOR_LENGTH, CV_32FC1);
    _matRnone.create(MSMT_VECTOR_LENGTH, MSMT_VECTOR_LENGTH, CV_32FC1);

    // Sensor model

    // dPerpLeft = laneWidth/2 + xPos
    // yawLeft = -yaw
    // dPerpRight = laneWidth/2 - xPos
    // yawRight = -yaw
    // nearestDist = nearestDist
    // nearestYaw = nearestYaw
    //
    // H = [  1 0  0  0.5 0 0
    //        0 0 -1  0   0 0
    //       -1 0  0  0.5 0 0
    //        0 0 -1  0   0 0
    //        0 0  0  0   1 0
    //        0 0  0  0   0 1 ];
    //
    // Hc = [ 0 ; pi/2 ; 0 ; pi/2 ; 0 ; 0 ];

    _matH.setTo(0);
    _matH.at<float>(I_MSMT_LEFT_DIST, I_STATE_XPOSITION) = 1.0;
    _matH.at<float>(I_MSMT_LEFT_DIST, I_STATE_LANE_WIDTH) = 0.5;
    _matH.at<float>(I_MSMT_LEFT_YAW, I_STATE_YAW) = -1.0;
    _matH.at<float>(I_MSMT_RIGHT_DIST, I_STATE_XPOSITION) = -1.0;
    _matH.at<float>(I_MSMT_RIGHT_DIST, I_STATE_LANE_WIDTH) = 0.5;
    _matH.at<float>(I_MSMT_RIGHT_YAW, I_STATE_YAW) = -1.0;
    _matH.at<float>(I_MSMT_NEAR_DIST, I_STATE_NEAR_DIST) = 1.0;
    _matH.at<float>(I_MSMT_NEAR_YAW, I_STATE_NEAR_YAW) = 1.0;

    _matHc.setTo(0);
    _matHc.at<float>(I_MSMT_LEFT_YAW, 0) = M_PI / 2;
    _matHc.at<float>(I_MSMT_RIGHT_YAW, 0) = M_PI / 2;

    // Sensor noise per frame
    // perp dist: high (e.g. 5 cm)
    // Right now, though, we use a REALLY high error of 50 cm!
    // This gives smooth trajectories with a high lag.
    // yaw: high (e.g. 5 degrees)

#define MSMT_DIST_NOISE (0.5 * 0.5)
#define MSMT_YAW_NOISE (5.0/180 * M_PI * 5.0/180 * M_PI)
#define MSMT_INVALID_NOISE (1e+10)

    _matRfull.setTo(0);
    _matRleft.setTo(0);
    _matRright.setTo(0);
    _matRnone.setTo(0);

    _matRfull.at<float>(I_MSMT_LEFT_DIST, I_MSMT_LEFT_DIST) = MSMT_DIST_NOISE;
    _matRfull.at<float>(I_MSMT_LEFT_YAW, I_MSMT_LEFT_YAW) = MSMT_YAW_NOISE;
    _matRfull.at<float>(I_MSMT_RIGHT_DIST, I_MSMT_RIGHT_DIST) = MSMT_DIST_NOISE;
    _matRfull.at<float>(I_MSMT_RIGHT_YAW, I_MSMT_RIGHT_YAW) = MSMT_YAW_NOISE;
    _matRfull.at<float>(I_MSMT_NEAR_DIST, I_MSMT_NEAR_DIST) = MSMT_DIST_NOISE;
    _matRfull.at<float>(I_MSMT_NEAR_YAW, I_MSMT_NEAR_YAW) = MSMT_YAW_NOISE;

    _matRleft.at<float>(I_MSMT_LEFT_DIST, I_MSMT_LEFT_DIST) = MSMT_DIST_NOISE;
    _matRleft.at<float>(I_MSMT_LEFT_YAW, I_MSMT_LEFT_YAW) = MSMT_YAW_NOISE;
    _matRleft.at<float>(I_MSMT_RIGHT_DIST, I_MSMT_RIGHT_DIST) =
            MSMT_INVALID_NOISE;
    _matRleft.at<float>(I_MSMT_RIGHT_YAW, I_MSMT_RIGHT_YAW) =
            MSMT_INVALID_NOISE;
    _matRleft.at<float>(I_MSMT_NEAR_DIST, I_MSMT_NEAR_DIST) = MSMT_DIST_NOISE;
    _matRleft.at<float>(I_MSMT_NEAR_YAW, I_MSMT_NEAR_YAW) = MSMT_YAW_NOISE;

    _matRright.at<float>(I_MSMT_LEFT_DIST, I_MSMT_LEFT_DIST) =
            MSMT_INVALID_NOISE;
    _matRright.at<float>(I_MSMT_LEFT_YAW, I_MSMT_LEFT_YAW) = MSMT_INVALID_NOISE;
    _matRright.at<float>(I_MSMT_RIGHT_DIST, I_MSMT_RIGHT_DIST) =
            MSMT_DIST_NOISE;
    _matRright.at<float>(I_MSMT_RIGHT_YAW, I_MSMT_RIGHT_YAW) = MSMT_YAW_NOISE;
    _matRright.at<float>(I_MSMT_NEAR_DIST, I_MSMT_NEAR_DIST) = MSMT_DIST_NOISE;
    _matRright.at<float>(I_MSMT_NEAR_YAW, I_MSMT_NEAR_YAW) = MSMT_YAW_NOISE;

    _matRnone.at<float>(I_MSMT_LEFT_DIST, I_MSMT_LEFT_DIST) =
            MSMT_INVALID_NOISE;
    _matRnone.at<float>(I_MSMT_LEFT_YAW, I_MSMT_LEFT_YAW) = MSMT_INVALID_NOISE;
    _matRnone.at<float>(I_MSMT_RIGHT_DIST, I_MSMT_RIGHT_DIST) =
            MSMT_INVALID_NOISE;
    _matRnone.at<float>(I_MSMT_RIGHT_YAW, I_MSMT_RIGHT_YAW) =
            MSMT_INVALID_NOISE;
    _matRnone.at<float>(I_MSMT_NEAR_DIST, I_MSMT_NEAR_DIST) =
            MSMT_INVALID_NOISE;
    _matRnone.at<float>(I_MSMT_NEAR_YAW, I_MSMT_NEAR_YAW) = MSMT_INVALID_NOISE;
}

void LaneMarkingFilter::setH(CvMat *pMatH)
{
    _pMatH = pMatH;
}

void LaneMarkingFilter::setExpectedLaneWidth(double dExpectedLaneWidth)
{
    _dExpectedLaneWidth = dExpectedLaneWidth;
}

void LaneMarkingFilter::resetStateCov(Mat &matP)
{
    matP.setTo(0);
    matP.at<float>(I_STATE_XPOSITION, I_STATE_XPOSITION) = 1.0;
    matP.at<float>(I_STATE_XDOT, I_STATE_XDOT) = 1.0;
    matP.at<float>(I_STATE_YAW, I_STATE_YAW) = 10.0 / 180 * M_PI * 10.0
            / 180*M_PI;
    matP.at<float>(I_STATE_LANE_WIDTH, I_STATE_LANE_WIDTH) = 1.0;
    matP.at<float>(I_STATE_NEAR_DIST, I_STATE_NEAR_DIST) = MSMT_DIST_NOISE;
    matP.at<float>(I_STATE_NEAR_YAW, I_STATE_NEAR_YAW) = MSMT_YAW_NOISE;
}

void LaneMarkingFilter::resetState()
{

    // Initial state assumption

    // X: vehicle is centered in a lane of expected width
    // P: diag(1 meter, 1 meter per second, 10 degrees, 1 meter)^2

    assert(_dExpectedLaneWidth != 0);

    _matXhat.at<float>(I_STATE_XPOSITION, 0) = 0;
    _matXhat.at<float>(I_STATE_XDOT, 0) = 0;
    _matXhat.at<float>(I_STATE_YAW, 0) = 0;
    _matXhat.at<float>(I_STATE_LANE_WIDTH, 0) = _dExpectedLaneWidth;
    _matXhat.at<float>(I_STATE_NEAR_DIST, 0) = 0;
    _matXhat.at<float>(I_STATE_NEAR_YAW, 0) = M_PI / 2;

    resetStateCov(_matPhat);

    _cUpdatesSinceReset = 0;
    _historyNearest.clear();
}

static void drawBoundary(Mat &matImg, double dPerpDist, double dYaw,
        double dDir, CvMat *pMatH, Util *pUtil, Scalar color)
{
    Point2f point1, point2, point3, point4;
    Point2f pointImg1, pointImg2, pointImg3, pointImg4;

    point1.x = dDir * (dPerpDist + 0.05) * cos(dYaw);
    point1.y = dDir * (dPerpDist + 0.05) * sin(dYaw);
    point3.x = dDir * (dPerpDist - 0.05) * cos(dYaw);
    point3.y = dDir * (dPerpDist - 0.05) * sin(dYaw);

    point2.x = point1.x - 100 * sin(dYaw);
    point2.y = point1.y + 100 * cos(dYaw);
    point4.x = point3.x - 100 * sin(dYaw);
    point4.y = point3.y + 100 * cos(dYaw);

    pUtil->groundPlaneToImagePlane(point1, pointImg1, pMatH);
    pUtil->groundPlaneToImagePlane(point2, pointImg2, pMatH);
    pUtil->groundPlaneToImagePlane(point3, pointImg3, pMatH);
    pUtil->groundPlaneToImagePlane(point4, pointImg4, pMatH);

    cv::line(matImg, pointImg1, pointImg2, color);
    cv::line(matImg, pointImg3, pointImg4, color);
}

static void drawBoundaries(Mat &matImg, Mat &matZ, CvMat *pMatH, Util *pUtil,
        Scalar color)
{
    double dPerpLeft = matZ.at<float>(I_MSMT_LEFT_DIST, 0);
    double dYawLeft = matZ.at<float>(I_MSMT_LEFT_YAW, 0);
    double dPerpRight = matZ.at<float>(I_MSMT_RIGHT_DIST, 0);
    double dYawRight = matZ.at<float>(I_MSMT_RIGHT_YAW, 0);

    drawBoundary(matImg, dPerpLeft, dYawLeft, -1, pMatH, pUtil, color);
    drawBoundary(matImg, dPerpRight, dYawRight, 1, pMatH, pUtil, color);
}

static void drawEstimatedBoundaries(Mat &matImg, Mat &matXhat, Mat &matH,
        CvMat *pMatH, Util *pUtil, Scalar color)
{
    Mat matZest = matH * matXhat;
    drawBoundaries(matImg, matZest, pMatH, pUtil, color);
}

void LaneMarkingFilter::saveDataToFile(
        const LaneMarkingDetector::Boundary &boundaryLeft,
        const LaneMarkingDetector::Boundary &boundaryRight)
{
    static ofstream ofileData;
    if (_cUpdatesSinceReset == 0)
    {
        ofileData.open("kalman_data.m", ios::out);
        Mat H(_pMatH);
        ofileData << "H = " << H << ";" << endl;
        ofileData << "expectedLaneWidth = " << _dExpectedLaneWidth << endl;
        ofileData.close();
    }
    ofileData.open("kalman_data.m", ios::app);
    ofileData << "data(" << _cUpdatesSinceReset + 1 << ").dPerpLeft = "
            << boundaryLeft.dPerp << ";" << endl;
    ofileData << "data(" << _cUpdatesSinceReset + 1 << ").dPerpRight = "
            << boundaryRight.dPerp << ";" << endl;
    ofileData << "data(" << _cUpdatesSinceReset + 1 << ").dYawLeft = "
            << boundaryLeft.dYaw << ";" << endl;
    ofileData << "data(" << _cUpdatesSinceReset + 1 << ").dYawRight = "
            << boundaryRight.dYaw << ";" << endl;
    ofileData << "data(" << _cUpdatesSinceReset + 1 << ").likelihoodLeft = "
            << boundaryLeft.likelihood << ";" << endl;
    ofileData << "data(" << _cUpdatesSinceReset + 1 << ").likelihoodRight = "
            << boundaryRight.likelihood << ";" << endl;
    ofileData.close();
}

void LaneMarkingFilter::msmtToStateVector(
        const LaneMarkingDetector::Boundary &boundaryLeft,
        const LaneMarkingDetector::Boundary &boundaryRight,
        const bool bValidLeft, const bool bValidRight, Mat &matX, Mat &matP)
{
    matX.at<float>(I_STATE_XDOT, 0) = 0;
    if (bValidLeft && bValidRight)
    {
        matX.at<float>(I_STATE_XPOSITION, 0) = (boundaryLeft.dPerp
                - boundaryRight.dPerp) / 2.0;
        matX.at<float>(I_STATE_YAW, 0) = (M_PI / 2 - boundaryLeft.dYaw
                + M_PI / 2 - boundaryRight.dYaw) / 2.0;
        matX.at<float>(I_STATE_LANE_WIDTH, 0) = boundaryLeft.dPerp
                + boundaryRight.dPerp;
        if (boundaryLeft.dPerp < boundaryRight.dPerp)
        {
            matX.at<float>(I_STATE_NEAR_DIST, 0) = boundaryLeft.dPerp;
            matX.at<float>(I_STATE_NEAR_YAW, 0) = boundaryLeft.dYaw;
        } else
        {
            matX.at<float>(I_STATE_NEAR_DIST, 0) = boundaryRight.dPerp;
            matX.at<float>(I_STATE_NEAR_YAW, 0) = boundaryRight.dYaw;
        }
    } else if (bValidLeft)
    {
        matX.at<float>(I_STATE_LANE_WIDTH, 0) = _dExpectedLaneWidth;
        double dRightDistAssumed = matX.at<float>(I_STATE_LANE_WIDTH, 0)
                - boundaryLeft.dPerp;
        matX.at<float>(I_STATE_XPOSITION, 0) = (boundaryLeft.dPerp
                - dRightDistAssumed) / 2.0;
        matX.at<float>(I_STATE_YAW, 0) = M_PI / 2 - boundaryLeft.dYaw;
        matX.at<float>(I_STATE_NEAR_DIST, 0) = boundaryLeft.dPerp;
        matX.at<float>(I_STATE_NEAR_YAW, 0) = boundaryLeft.dYaw;
    } else if (bValidRight)
    {
        matX.at<float>(I_STATE_LANE_WIDTH, 0) = _dExpectedLaneWidth;
        double dLeftDistAssumed = matX.at<float>(I_STATE_LANE_WIDTH, 0)
                - boundaryRight.dPerp;
        matX.at<float>(I_STATE_XPOSITION, 0) = (dLeftDistAssumed
                - boundaryRight.dPerp) / 2.0;
        matX.at<float>(I_STATE_YAW, 0) = M_PI / 2 - boundaryRight.dYaw;
        matX.at<float>(I_STATE_NEAR_DIST, 0) = boundaryRight.dPerp;
        matX.at<float>(I_STATE_NEAR_YAW, 0) = boundaryRight.dYaw;
    } else
    {
        matX.at<float>(I_STATE_XPOSITION, 0) = 0;
        matX.at<float>(I_STATE_YAW, 0) = 0;
        matX.at<float>(I_STATE_LANE_WIDTH, 0) = _dExpectedLaneWidth;
        matX.at<float>(I_STATE_NEAR_DIST, 0) = 0;
        matX.at<float>(I_STATE_NEAR_YAW, 0) = M_PI / 2;
    }
    resetStateCov(matP);
}

static bool isBadLaneBoundary(const LaneMarkingDetector::Boundary &boundary,
        double dExpectedLaneWidth)
{
    if (boundary.dPerp
            > 1.1
                    * dExpectedLaneWidth|| fabs(boundary.dYaw - M_PI/2) > 20.0/180*M_PI)
    {
        return true;
    }
    return false;
}

void LaneMarkingFilter::checkBoundaryConsistency(
        const LaneMarkingDetector::Boundary &boundaryLeft,
        const LaneMarkingDetector::Boundary &boundaryRight, bool &bValidLeft,
        bool &bValidRight)
{
    // Check boundaries individually

    bValidLeft = true;
    bValidRight = true;

    if (boundaryLeft.likelihood == 0)
        bValidLeft = false;
    if (boundaryRight.likelihood == 0)
        bValidRight = false;

    if (bValidLeft && isBadLaneBoundary(boundaryLeft, _dExpectedLaneWidth))
        bValidLeft = false;
    if (bValidRight && isBadLaneBoundary(boundaryRight, _dExpectedLaneWidth))
        bValidRight = false;

    if (!bValidLeft || !bValidRight)
        return;

    // Both still valid -- check boundaries together

    double dYawDiff = fabs(boundaryLeft.dYaw - boundaryRight.dYaw);
    assert(boundaryLeft.dPerp >= 0 && boundaryRight.dPerp >= 0);
    double dLaneWidth = boundaryLeft.dPerp + boundaryRight.dPerp;
    double dLaneWidthStdev = _dExpectedLaneWidth * 0.1;
    double dLaneWidthZscore = fabs(
            (dLaneWidth - _dExpectedLaneWidth) / dLaneWidthStdev);
    if (dYawDiff > 10.0 / 180 * M_PI || dLaneWidthZscore > 2.0)
    {
        if (boundaryLeft.likelihood > boundaryRight.likelihood)
        {
            bValidRight = false;
        } else
        {
            bValidLeft = false;
        }
    }
}

void LaneMarkingFilter::boundariesToMsmtVectorAndCov(
        const LaneMarkingDetector::Boundary &boundaryLeft,
        const LaneMarkingDetector::Boundary &boundaryRight,
        const bool bValidLeft, const bool bValidRight, Mat &matZpred)
{
    double dDistLeft = boundaryLeft.dPerp;
    double dYawLeft = boundaryLeft.dYaw;

    double dDistRight = boundaryRight.dPerp;
    double dYawRight = boundaryRight.dYaw;

    if (bValidLeft && bValidRight)
    {
        _matZ.at<float>(I_MSMT_LEFT_DIST, 0) = dDistLeft;
        _matZ.at<float>(I_MSMT_LEFT_YAW, 0) = dYawLeft;
        _matZ.at<float>(I_MSMT_RIGHT_DIST, 0) = dDistRight;
        _matZ.at<float>(I_MSMT_RIGHT_YAW, 0) = dYawRight;
        if (dDistLeft < dDistRight)
        {
            _matZ.at<float>(I_MSMT_NEAR_DIST, 0) = -dDistLeft;
            _matZ.at<float>(I_MSMT_NEAR_YAW, 0) = dYawLeft;
        } else
        {
            _matZ.at<float>(I_MSMT_NEAR_DIST, 0) = dDistRight;
            _matZ.at<float>(I_MSMT_NEAR_YAW, 0) = dYawRight;
        }
        _matR = _matRfull; // Note this does not copy, it points R's data at Rfull's
    } else if (bValidLeft)
    {
        _matZ.at<float>(I_MSMT_LEFT_DIST, 0) = dDistLeft;
        _matZ.at<float>(I_MSMT_LEFT_YAW, 0) = dYawLeft;
        _matZ.at<float>(I_MSMT_RIGHT_DIST, 0) = matZpred.at<float>(
                I_MSMT_RIGHT_DIST, 0);
        _matZ.at<float>(I_MSMT_RIGHT_YAW, 0) = matZpred.at<float>(
                I_MSMT_RIGHT_YAW, 0);
        _matZ.at<float>(I_MSMT_NEAR_DIST, 0) = -dDistLeft;
        _matZ.at<float>(I_MSMT_NEAR_YAW, 0) = dYawLeft;
        _matR = _matRleft;
    } else if (bValidRight)
    {
        _matZ.at<float>(I_MSMT_LEFT_DIST, 0) = matZpred.at<float>(
                I_MSMT_LEFT_DIST, 0);
        _matZ.at<float>(I_MSMT_LEFT_YAW, 0) = matZpred.at<float>(
                I_MSMT_LEFT_YAW, 0);
        _matZ.at<float>(I_MSMT_RIGHT_DIST, 0) = dDistRight;
        _matZ.at<float>(I_MSMT_RIGHT_YAW, 0) = dYawRight;
        _matZ.at<float>(I_MSMT_NEAR_DIST, 0) = dDistRight;
        _matZ.at<float>(I_MSMT_NEAR_YAW, 0) = dYawRight;
        _matR = _matRright;
    } else
    {
        // No information; retain old values

        matZpred.copyTo(_matZ);
        _matR = _matRnone;
    }
}

// Cholesky decomposition of matA.
// matA must be symmetric positive definite or results are undefined.
// Based on C code at http://rosettacode.org/wiki/Cholesky_decomposition

static void cholesky(const Mat &matA, Mat &matL)
{
    assert(matA.rows == matA.cols);
    assert(matA.type() == CV_32FC1);
    matL.setTo(0);

    for (int i = 0; i < matA.rows; i++)
    {
        for (int j = 0; j < (i + 1); j++)
        {
            float s = 0;
            for (int k = 0; k < j; k++)
            {
                s += matL.at<float>(i, k) * matL.at<float>(j, k);
            }
            matL.at<float>(i, j) =
                    (i == j) ?
                            sqrt(matA.at<float>(i, i) - s) :
                            (1.0 / matL.at<float>(j, j)
                                    * (matA.at<float>(i, j) - s));
        }
    }
}

static void influence(const Mat &matResids, const double dDelta, Mat &matPsi)
{
    // Derivative of Huber's score function
    // rho(z) = delta * abs(z) - delta^2/2, |z| >= delta
    //        = z^2/2,                      |z| <  delta

    // For standard Kalman filter, just return the residuals themselves:
    // Psi = resids;

    matResids.copyTo(matPsi);

    // For robust Kalman filter, return the dampened residuals

    // Psi = resids;
    // Psi(resids > delta) = delta;
    // Psi(resids < -delta) = -delta;

    for (int i = 0; i < matPsi.rows; i++)
    {
        if (matPsi.at<float>(i, 0) > dDelta)
            matPsi.at<float>(i, 0) = dDelta;
        if (matPsi.at<float>(i, 0) < -dDelta)
            matPsi.at<float>(i, 0) = -dDelta;
    }
}

static void calcRobustWeights(const Mat &matX, const Mat &matY,
        const Mat &matXhatPrev, Mat &matOmega)
{
    // resid = Y - X * xhat_prev;
    // Psi = influence(resid, 1);
    // Psi(resid==0) = 1;
    // resid(resid==0) = 1;
    // weights = Psi ./ resid;
    // Omega = diag(weights);

    Mat matResid = matY - matX * matXhatPrev;
    Mat matPsi(matResid.rows, 1, CV_32FC1);
    influence(matResid, 1.0, matPsi);
    matOmega.setTo(0);
    for (int i = 0; i < matResid.rows; i++)
    {
        float weight;
        if (matResid.at<float>(i, 0) == 0)
        {
            weight = 1.0;
        } else
        {
            weight = matPsi.at<float>(i, 0) / matResid.at<float>(i, 0);
        }
        matOmega.at<float>(i, i) = weight;
    }
}

static void robustKalmanUpdate(Mat &matXhat, Mat &matPhat, const Mat &matZ,
        const Mat &matF, const Mat &matQ, const Mat &matH, const Mat &matHc,
        const Mat &matR)
{
    int cStates = matXhat.rows;
    int cMsmts = matZ.rows;

    //cout << "Input to update: matPhat" << endl << matPhat << endl << "matXhat" << endl << matXhat << endl;

    // xpred = F * xhat
    // Ppred = F * Phat * F' + Q

    Mat matXpred = matF * matXhat;
    Mat matPpred = matF * matPhat * matF.t() + matQ;

    //cout << "matPpred: " << endl << matPpred << endl;

    // SST = [ Ppred , zeros(ns, nm) ; zeros(nm, ns), R ];
    // S = chol(SST)';
    // Sinv = inv(S);

    Mat matSSt(cStates + cMsmts, cStates + cMsmts, CV_32FC1);
    matSSt.setTo(0);
    Mat matPdest = matSSt(Rect(0, 0, cStates, cStates));
    matPpred.copyTo(matPdest);
    Mat matRdest = matSSt(Rect(cStates, cStates, cMsmts, cMsmts));
    matR.copyTo(matRdest);
    Mat matS(matSSt.rows, matSSt.cols, CV_32FC1);
    cholesky(matSSt, matS);
    Mat matSinv = matS.inv();

    // X = Sinv * [ eye(ns) ; H ];

    Mat matIH(cStates + cMsmts, cStates, CV_32FC1);
    matIH.setTo(0);
    for (int i = 0; i < cStates; i++)
        matIH.at<float>(i, i) = 1.0;
    Mat matHdest = matIH(cv::Rect(0, cStates, cStates, cMsmts));
    matH.copyTo(matHdest);
    assert(
            matIH.at<float>(cStates + cMsmts - 1, cStates - 1)
                    == matH.at<float>(cMsmts - 1, cStates - 1));
    Mat matX = matSinv * matIH;

    //cout << "matX:" << endl << matX << endl;
    //cout << "matSinv:" << endl << matSinv << endl;

    // Y = Sinv * [ xpred ; z-Hc ];

    Mat matXpredZ(cStates + cMsmts, 1, CV_32FC1);
    for (int i = 0; i < cStates; i++)
    {
        matXpredZ.at<float>(i, 0) = matXpred.at<float>(i, 0);
    }
    for (int i = 0; i < cMsmts; i++)
    {
        matXpredZ.at<float>(cStates + i, 0) = matZ.at<float>(i, 0)
                - matHc.at<float>(i, 0);
    }
    Mat matY = matSinv * matXpredZ;

    //cout << "matXpredZ:" << endl << matXpredZ << endl;
    //cout << "matY:" << endl << matY << endl;

    // Omega = calc_robust_weights(X, Y, xhat);

    Mat matOmega(cStates + cMsmts, cStates + cMsmts, CV_32FC1);
    calcRobustWeights(matX, matY, matXhat, matOmega);

    //cout << "matOmega:" << endl << matOmega << endl;

    // Phat = inv(X' * Omega * X);
    // xhat = Phat * X' * Omega * Y;

    matPhat = (matX.t() * matOmega * matX).inv();
    matXhat = matPhat * matX.t() * matOmega * matY;

    //cout << "matZ:" << endl << matZ << endl << "Omega:" << endl << matOmega << endl;
    //cout << "Output of update: matXhat" << endl << matXhat << endl << "matPhat" << endl << matPhat << endl;
}

#define TEST_EQUALITY_WITH_ORDINARY_KALMANno
#ifdef TEST_EQUALITY_WITH_ORDINARY_KALMAN
static void
ordinaryKalmanUpdate(
        Mat &matXhat,
        Mat &matPhat,
        const Mat &matZ,
        const Mat &matF,
        const Mat &matQ,
        const Mat &matH,
        const Mat &matHc,
        const Mat &matR)
{
    Mat matXpred = matF * matXhat;
    Mat matZpred = matH * matXpred + matHc;
    Mat matPpred = matF * matPhat * matF.t() + matQ;
    Mat matZresid = matZ - matZpred;
    Mat matSresid = matH * matPpred * matH.t() + matR;
    Mat matK = matPpred * matH.t() * matSresid.inv();
    matXhat = matXpred + matK * matZresid;

    // Compute Phat with the Joseph form to avoid numerical instability

    Mat matI = Mat::eye(STATE_VECTOR_LENGTH, STATE_VECTOR_LENGTH, CV_32FC1);
    Mat matIKH = matI - matK * matH;
    matPhat = matIKH * matPpred * matIKH.t() + matK * matR * matK.t();
}
#endif

bool LaneMarkingFilter::prepareNearestUpdate()
{
    // We only want to track the nearest lane boundary when we're not near the center of the lane
    // (otherwise the nearest boundary would bounce back and forth between the left and right boundary with noise).
    // Also, when the observation is really noisy right at the beginning, we should reset.

    if (fabs(_matXhat.at<float>(I_STATE_XPOSITION, 0))
            < _dExpectedLaneWidth * 0.088888)
    {
        // Too close to center
        _historyNearest.clear();
        return false;
    }

    // If no update yet or we're just starting and observe a lot of noise, reset state from the observation

    int cNearestUpdates = _historyNearest.size();
    if (cNearestUpdates == 0
            || (cNearestUpdates < 3
                    && fabs(
                            _matZ.at<float>(I_MSMT_NEAR_DIST)
                                    - _historyNearest[cNearestUpdates - 1].dNearestDistZ)
                            > 1.0))
    {
        _matXhat.at<float>(I_STATE_NEAR_DIST, 0) = _matZ.at<float>(
                I_MSMT_NEAR_DIST, 0);
        _matXhat.at<float>(I_STATE_NEAR_YAW, 0) = _matZ.at<float>(
                I_MSMT_NEAR_YAW, 0);
        Mat matRdest = _matPhat(
                Rect(I_STATE_NEAR_DIST, I_STATE_NEAR_DIST, 2, 2));
        Mat matRsrc = _matRfull(
                Rect(I_STATE_NEAR_DIST, I_STATE_NEAR_DIST, 2, 2));
        matRsrc.copyTo(matRdest);
        _historyNearest.clear();
    }
    return true;
}

#define MIN_HISTORY_LENGTH_FOR_LANE_CHANGE 7

bool LaneMarkingFilter::possiblelaneChange(bool bDebug)
{
    int cUpdatesNearest = _historyNearest.size();

    if (bDebug)
    {
        cerr << "Lane changed? " << _historyNearest.size()
                << " measurements available" << endl;
    }

    if (cUpdatesNearest < MIN_HISTORY_LENGTH_FOR_LANE_CHANGE)
        return false;

    double dNearestDist = _historyNearest[cUpdatesNearest - 1].dNearestDist;
    double dNearestDistPrev = _historyNearest[cUpdatesNearest - 2].dNearestDist;

    if (bDebug)
    {
        cerr << "Nearest distances " << dNearestDistPrev << ", " << dNearestDist
                << endl;
    }

    if (dNearestDistPrev <= 0 && dNearestDist <= 0)
        return false;
    if (dNearestDistPrev >= 0 && dNearestDist >= 0)
        return false;

    // Possible lane change

    int cObs = _historyNearest.size();
    vector<Point2f> vPointXpositionEst;
    vector<Point2f> vPointNearestEst;
    vector<Point2f> vPointNearestZ;

    for (int i = 0; i < cObs; i++)
    {
        vPointXpositionEst.push_back(Point2f(i, _historyNearest[i].dPosition));
        vPointNearestEst.push_back(Point2f(i, _historyNearest[i].dNearestDist));
        vPointNearestZ.push_back(Point2f(i, _historyNearest[i].dNearestDistZ));
    }

#define RANSAC_LINE_DIST_THRESHOLD 0.1
#define MIN_NEAREST_INLIERS (0.5 * cObs)
#define MIN_XPOS_INLIERS (0.4 * cObs)

    vector<Point2f> vInliers;
    _pRANSAC->fitLine(vPointNearestEst, vInliers, RANSAC_LINE_DIST_THRESHOLD);
    if (vInliers.size() <= MIN_NEAREST_INLIERS)
        return false;
    Line line;
    line.fit(vInliers);
    double dSlopeNearestEst = line.getSlope();

    _pRANSAC->fitLine(vPointNearestZ, vInliers, RANSAC_LINE_DIST_THRESHOLD);
    if (vInliers.size() <= MIN_NEAREST_INLIERS)
        return false;
    line.fit(vInliers);
    double dSlopeNearestZ = line.getSlope();

    if (bDebug)
    {
        cerr << "Nearest estimate slope " << dSlopeNearestEst << endl;
        cerr << "Nearest measured slope " << dSlopeNearestZ << endl;
    }

    // If slopes are 0 or in opposite directions return false

    if (dSlopeNearestEst * dSlopeNearestZ <= 0)
        return false;

    // Check XpositionEst list also -- first trim wrong-direction elements from end of list

    while (vPointXpositionEst.size() > MIN_XPOS_INLIERS
            && (vPointXpositionEst[vPointXpositionEst.size() - 1].y
                    - vPointXpositionEst[vPointXpositionEst.size() - 2].y)
                    * dSlopeNearestZ >= 0)
    {
        vPointXpositionEst.erase(vPointXpositionEst.end());
    }
    if (vPointXpositionEst.size() <= MIN_XPOS_INLIERS)
        return false;
    _pRANSAC->fitLine(vPointXpositionEst, vInliers, RANSAC_LINE_DIST_THRESHOLD);
    if (vInliers.size() <= MIN_XPOS_INLIERS)
        return false;
    line.fit(vInliers);
    double dSlopeXpositionEst = line.getSlope();

    if (bDebug)
    {
        cerr << "Vehicle position estimate slope " << dSlopeXpositionEst
                << endl;
    }

    // If xpos slope is opposite direction of nearest boundary slope return true else false

    if (dSlopeNearestZ * dSlopeXpositionEst >= 0)
        return false;

    if (bDebug)
    {
        cerr << "Lane change!" << endl;
    }

    return true;
}

// Keep this small or change history to a linked list
#define NEAREST_HISTORY_SIZE 10

void LaneMarkingFilter::updateNearestHistory()
{
    HistoryItem historyItem;

    historyItem.dPosition = _matXhat.at<float>(I_STATE_XPOSITION, 0);
    historyItem.dNearestDist = _matXhat.at<float>(I_STATE_NEAR_DIST, 0);
    historyItem.dNearestDistZ = _matZ.at<float>(I_MSMT_NEAR_DIST, 0);

    _historyNearest.push_back(historyItem);

    while (_historyNearest.size() > NEAREST_HISTORY_SIZE)
    {
        // Note that if history size is longer than 10 or 20, we'd better use a linked
        // list instead of a vector here.
        _historyNearest.erase(_historyNearest.begin());
    }
}

void LaneMarkingFilter::updateState(
        const LaneMarkingDetector::Boundary &boundaryLeft,
        const LaneMarkingDetector::Boundary &boundaryRight,
        const IplImage *pImage, bool bDisableDisplayWindows, bool bDebug)
{
    Assert<InvalidParameterException>(_dExpectedLaneWidth > 0);
    saveDataToFile(boundaryLeft, boundaryRight);

    bool bValidLeft = true;
    bool bValidRight = true;
    bool bTrackingNearestBoundary = false;

    checkBoundaryConsistency(boundaryLeft, boundaryRight, bValidLeft,
            bValidRight);

    if (_cUpdatesSinceReset == 0)
    {
        msmtToStateVector(boundaryLeft, boundaryRight, bValidLeft, bValidRight,
                _matXhat, _matPhat);
    } else
    {
        Mat matXpred = _matF * _matXhat;
        Mat matZpred = _matH * matXpred + _matHc;
        boundariesToMsmtVectorAndCov(boundaryLeft, boundaryRight, bValidLeft,
                bValidRight, matZpred);

        bTrackingNearestBoundary = prepareNearestUpdate();

        if (bDebug)
        {
            cerr << "Xhat before:" << _matXhat << endl;
            cerr << "Z:" << endl << _matZ << endl;
            cerr << "R:" << endl << _matR << endl;
        }

#ifdef TEST_EQUALITY_WITH_ORDINARY_KALMAN
        Mat matXhatOrd = _matXhat.clone();
        Mat matPhatOrd = _matPhat.clone();
        ordinaryKalmanUpdate(matXhatOrd, matPhatOrd, _matZ, _matF, _matQ, _matH, _matHc, _matR);
#endif

        robustKalmanUpdate(_matXhat, _matPhat, _matZ, _matF, _matQ, _matH,
                _matHc, _matR);

        if (bDebug)
        {
            cerr << "Xhat after:" << endl << _matXhat << endl;
        }

#ifdef TEST_EQUALITY_WITH_ORDINARY_KALMAN
        assert(norm(matXhatOrd - _matXhat) < 1e-6);
        assert(norm(matPhatOrd - _matPhat) < 1e-6);
#endif
    }

    // Update nearest boundary history

    if (bTrackingNearestBoundary)
        updateNearestHistory();

    // If lane changed, reset state from observation vector

    if (possiblelaneChange(bDebug))
    {
        msmtToStateVector(boundaryLeft, boundaryRight, bValidLeft, bValidRight,
                _matXhat, _matPhat);
        _historyNearest.clear();

        if (bDebug)
        {
            cerr << "Xhat after lane change:" << endl << _matXhat << endl;
        }
    }

    //cout << "Output after lane change update: matXhat" << endl << _matXhat << endl << "matPhat" << endl << _matPhat << endl;

    //cerr << "Nearest position z " << _matZ.at<float>(I_MSMT_NEAR_DIST, 0) << endl;
    //cerr << "Nearest position " << _matXhat.at<float>(I_STATE_NEAR_DIST, 0) << endl;

    if (!bDisableDisplayWindows && _pMatH)
    {
        Mat matImg(pImage, true);

        // Draw green lines for accepted boundaries, red for ignored

        if (boundaryLeft.likelihood > 0)
        {
            cv::Point2f point1, point2;
            _pUtil->groundPlaneToImagePlane(boundaryLeft.pointMin, point1,
                    _pMatH);
            _pUtil->groundPlaneToImagePlane(boundaryLeft.pointMax, point2,
                    _pMatH);
            Scalar color = bValidLeft ? Scalar(0, 255, 0) : Scalar(0, 0, 255);
            cv::line(matImg, point1, point2, color, 2, CV_AA, 0);
        }
        if (boundaryRight.likelihood > 0)
        {
            cv::Point2f point1, point2;
            _pUtil->groundPlaneToImagePlane(boundaryRight.pointMin, point1,
                    _pMatH);
            _pUtil->groundPlaneToImagePlane(boundaryRight.pointMax, point2,
                    _pMatH);
            Scalar color = bValidRight ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
            cv::line(matImg, point1, point2, color, 2, CV_AA, 0);
        }

        // Draw predicted and estimated boundaries

        Mat matXsens(STATE_VECTOR_LENGTH, 1, CV_32FC1);
        Mat matP(STATE_VECTOR_LENGTH, STATE_VECTOR_LENGTH, CV_32FC1);
        msmtToStateVector(boundaryLeft, boundaryRight, bValidLeft, bValidRight,
                matXsens, matP);
        drawEstimatedBoundaries(matImg, matXsens, _matH, _pMatH, _pUtil,
                Scalar(0, 0, 255));

        drawEstimatedBoundaries(matImg, _matXhat, _matH, _pMatH, _pUtil,
                Scalar(0, 255, 0));

        cv::imshow("Boundary filter", matImg);
        cvWaitKey(2);
    }

    _cUpdatesSinceReset++;
}

void LaneMarkingFilter::getCurrentState(double &dPosition, double &dYaw)
{
    dPosition = _matXhat.at<float>(I_STATE_XPOSITION, 0);
    dYaw = _matXhat.at<float>(I_STATE_YAW, 0);
}

void LaneMarkingFilter::getCurrentState(double &dPosition, double &dYaw,
        double &dLaneWidth)
{
    dPosition = _matXhat.at<float>(I_STATE_XPOSITION, 0);
    dYaw = _matXhat.at<float>(I_STATE_YAW, 0);
    dLaneWidth = _matXhat.at<float>(I_STATE_LANE_WIDTH, 0);
}
