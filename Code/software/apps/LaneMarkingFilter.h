/*
 * LaneMarkingFilter.h
 *
 *  Created on: Dec 17, 2014
 *      Author: mdailey
 */

#ifndef LANEMARKINGFILTER_H_
#define LANEMARKINGFILTER_H_

#include "LaneMarkingDetector.h"

#include <cv.h>

using namespace cv;

class LaneMarkingFilter {

public:

	LaneMarkingFilter(double dExpectedWidth);

	virtual ~LaneMarkingFilter();

	// State processing

	void updateState(
			const LaneMarkingDetector::Boundary &boundaryLeft,
			const LaneMarkingDetector::Boundary &boundaryRight,
			const IplImage *pImage,
			bool bDisableDisplayWindows,
			bool bDebug);

	void getCurrentState(double &dPosition, double &dYaw);
	void getCurrentState(double &dPosition, double &dYaw, double &dLaneWidth);

	void resetState();

	// Camera properties

	void setH(CvMat *pMatH);
	void setExpectedLaneWidth(double dExpectedLaneWidth);

private:

	class HistoryItem
	{
	public:
		double dPosition;
		double dNearestDist;
		double dNearestDistZ;
	};

	// state: x (lateral position), yaw, yawdot, lane width
	// msrmt: xleft (perpendicular distance), yawleft, xright (perpendicular distance), yawright

	Mat _matXhat;
	Mat _matPhat;

	Mat _matF;
	Mat _matQ;

	Mat _matH;
	Mat _matHc;

	Mat _matRfull;
	Mat _matRleft;
	Mat _matRright;
	Mat _matRnone;

	Mat _matR;
	Mat _matZ;

	int _cUpdatesSinceReset;

	Util *_pUtil;
	CvMat *_pMatH;
	double _dExpectedLaneWidth;

	vector<HistoryItem> _historyNearest;
	RANSACEstimator *_pRANSAC;

	void eliminateInconsistentBoundaryMeasurements(
			const LaneMarkingDetector::Boundary &boundaryLeft,
			const LaneMarkingDetector::Boundary &boundaryRight,
			const Mat &matZpred, const Mat &matXpred,
			bool &bIgnoreLeft, bool &bIgnoreRight);

	void saveDataToFile(
			const LaneMarkingDetector::Boundary &boundaryLeft,
			const LaneMarkingDetector::Boundary &boundaryRight);

	void msmtToStateVector(
			const LaneMarkingDetector::Boundary &boundaryLeft,
			const LaneMarkingDetector::Boundary &boundaryRight,
			const bool bValidLeft, const bool bValidRight,
			Mat &matX, Mat &matP);

	void boundariesToMsmtVectorAndCov(
			const LaneMarkingDetector::Boundary &boundaryLeft,
			const LaneMarkingDetector::Boundary &boundaryRight,
			const bool bValidLeft,
			const bool bValidRight,
			Mat &matZpred);

	void checkBoundaryConsistency(
			const LaneMarkingDetector::Boundary &boundaryLeft,
			const LaneMarkingDetector::Boundary &boundaryRight,
			bool &bValidLeft, bool &bValidRight);

	void resetStateCov(Mat &mat);
	bool possiblelaneChange(bool bDebug);

	void updateNearestHistory();
	bool prepareNearestUpdate();
	void initStateTransitionModel();
	void initSensorModel();
};

#endif /* LANEMARKINGFILTER_H_ */
