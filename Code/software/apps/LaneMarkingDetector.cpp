/*
 * LaneMarkingDetector.cpp
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include "LaneMarkingDetector.h"

// TODO: move MIN_NUM_OF_CONNECTED_COMP_INLIERS to configuration file

#define MIN_NUM_OF_POINTS_IN_ONE_CONNECTED_COMP_FOR_FITLINE	15
#define MIN_NUM_OF_CONNECTED_COMP_INLIERS 15
#define REQUIRED_LINE_INLIER_RATIO 0.8

// TODO: make marking width a configurable parameter

#define LANE_MARKING_WIDTH_WORLD 0.10
#define MIN_LANE_MARKING_WIDTH_IN_PIXEL 0

int leftId; 
int rightId;



LaneMarkingDetector::LaneMarkingDetector()
{

    _pConnectedCompObj = NULL;
    _pCameraGeometryObj = NULL;
    _pConvolutionObj = new Convolution();
    _pRANSAC = new RANSACEstimator();
    _pUtil = new Util();
    _pLane = new Lane();
    _pLine = new Line();
    _pTimer = new Timer();

    _pMatHinv = cvCreateMat(3, 3, CV_32FC1);
    _pMatH = cvCreateMat(3, 3, CV_32FC1);
    _dExpectedLaneWidth = 0;

	

}

LaneMarkingDetector::~LaneMarkingDetector()
{
    if (_pTimer)
        delete _pTimer;
}

CvMat *LaneMarkingDetector::getHinv()
{
    return _pMatHinv;
}

CvMat *LaneMarkingDetector::getH()
{
    return _pMatH;
}

void LaneMarkingDetector::initializeCameraModel(float focalLengthX,
        float focalLengthY, float px, float py, int width, int height,
        float pitch, float roll, float yaw, float tCWx, float tCWy, float tCWz)
{
    _pCameraGeometryObj = new CameraGeometry(focalLengthX, focalLengthY, px, py,
            width, height, pitch, roll, yaw, tCWx, tCWy, tCWz);
    _pMatH = _pCameraGeometryObj->getH();
    _pMatHinv = _pCameraGeometryObj->getHInv();

    calculateLaneMarkingWidths();
}

void LaneMarkingDetector::initializeCameraModelP(double p11, double p12,
        double p13, double p14, double p21, double p22, double p23, double p24,
        double p31, double p32, double p33, double p34, int width, int height)
{
    double P[3][4];

    P[0][0] = p11;
    P[0][1] = p12;
    P[0][2] = p13;
    P[0][3] = p14;
    P[1][0] = p21;
    P[1][1] = p22;
    P[1][2] = p23;
    P[1][3] = p24;
    P[2][0] = p31;
    P[2][1] = p32;
    P[2][2] = p33;
    P[2][3] = p34;

    _pCameraGeometryObj = new CameraGeometry(P, width, height);
    _pMatH = _pCameraGeometryObj->getH();
    _pMatHinv = _pCameraGeometryObj->getHInv();

    calculateLaneMarkingWidths();
}

void LaneMarkingDetector::setIgnorePixels(int left, int right, int top,
        int bottom)
{
    _pConvolutionObj->setIgnorePixels(left, right, top, bottom);
}

void LaneMarkingDetector::setExpectedLaneWidth(double dExpectedLaneWidth)
{
    _dExpectedLaneWidth = dExpectedLaneWidth;
}

void LaneMarkingDetector::calculateLaneMarkingWidths()
{
    _vWidths.clear();
    int imgWidth = _pCameraGeometryObj->getImgWidth();
    int imgHeight = _pCameraGeometryObj->getImgHeight();
    CvMat *imagePoint1 = cvCreateMat(3, 1, CV_32FC1);
    CvMat *imagePoint2 = cvCreateMat(3, 1, CV_32FC1);
    CvMat *groundPoint1 = cvCreateMat(3, 1, CV_32FC1);
    CvMat *groundPoint2 = cvCreateMat(3, 1, CV_32FC1);

    for (int i = 0; i < imgHeight; i++)
    {
        cvmSet(imagePoint1, 0, 0, imgWidth / 2);
        cvmSet(imagePoint1, 1, 0, i);
        cvmSet(imagePoint1, 2, 0, 1);
        cvMatMul(_pMatHinv, imagePoint1, groundPoint1);
        float denominator = cvmGet(groundPoint1, 2, 0);
        cvmSet(groundPoint1, 0, 0, cvmGet(groundPoint1, 0, 0) / denominator);
        cvmSet(groundPoint1, 1, 0, cvmGet(groundPoint1, 1, 0) / denominator);
        cvmSet(groundPoint1, 2, 0, 1.0);
        cvmSet(groundPoint2, 0, 0,
                cvmGet(groundPoint1, 0, 0) + LANE_MARKING_WIDTH_WORLD);
        cvmSet(groundPoint2, 1, 0, cvmGet(groundPoint1, 1, 0));
        cvmSet(groundPoint2, 2, 0, 1.0);
        cvMatMul(_pMatH, groundPoint2, imagePoint2);
        denominator = cvmGet(imagePoint2, 2, 0);
        cvmSet(imagePoint2, 0, 0, cvmGet(imagePoint2, 0, 0) / denominator);
        cvmSet(imagePoint2, 1, 0, cvmGet(imagePoint2, 1, 0) / denominator);
        cvmSet(imagePoint2, 2, 0, 1.0);
        int iWidth = (int) round(
                cvmGet(imagePoint2, 0, 0) - cvmGet(imagePoint1, 0, 0));
        if (iWidth > MIN_LANE_MARKING_WIDTH_IN_PIXEL)
        {
            _vWidths.push_back(iWidth);
        } else
        {
            _vWidths.push_back(0);
        }
    }

    cvReleaseMat(&imagePoint1);
    cvReleaseMat(&imagePoint2);
    cvReleaseMat(&groundPoint1);
    cvReleaseMat(&groundPoint2);

    return;
}

void LaneMarkingDetector::getLinearInlierComponents(
        const vector<vector<cv::Point> > &connectedComponents,
        vector<MarkingComponent> &markingComponents, bool bDebug)
{
    for (unsigned int i = 0; i < connectedComponents.size(); i++)
    {
        if (connectedComponents[i].size() >= MIN_NUM_OF_POINTS_IN_ONE_CONNECTED_COMP_FOR_FITLINE)
        {
            timeval startRansacTime = _pTimer->getCurrentTime();

            std::vector<cv::Point2f> points;
            for (unsigned int j = 0; j < connectedComponents[i].size(); j++)
            {
                points.push_back(
                        cv::Point2f((float) (connectedComponents[i][j].x),
                                (float) (connectedComponents[i][j].y)));
            }
            std::vector<cv::Point2f> inliers;
            _pRANSAC->fitLine(points, inliers);

            if (inliers.size() >= MIN_NUM_OF_CONNECTED_COMP_INLIERS)
            {
                MarkingComponent markingComponent;
                for (unsigned int i = 0; i < inliers.size(); i++)
                {
                    MarkingPoint markingPoint;
                    markingPoint.imagePoint = inliers[i];
                    _pUtil->imagePlaneToGroundPlane(markingPoint.imagePoint,
                            markingPoint.groundPoint, _pMatHinv);
                    markingComponent.push_back(markingPoint);
                }
                markingComponents.push_back(markingComponent);
            }

            timeval finishRansacTime = _pTimer->getCurrentTime();
            if (bDebug)
            {
                double dTimeDiffRansac = _pTimer->getElapsedTimeInMilliSec(
                        startRansacTime, finishRansacTime);
                cout << "RANSAC for comp " << i << ": " << dTimeDiffRansac
                        << " ms" << endl;
                cout << "Points: " << points.size() << " inliers: "
                        << inliers.size() << endl;
            }
        }
    }
}

static void fitGroundLine(Line &line,
        const vector<LaneMarkingDetector::MarkingPoint> component,
        cv::Point2f &minPoint, cv::Point2f &maxPoint)
{
    vector<cv::Point2f> points;
    for (unsigned int i = 0; i < component.size(); i++)
    {
        points.push_back(component[i].groundPoint);
    }
    line.fit(points);
    Line::ExtremePointsF extremePoints = line.get_extreme_pointsF(points);
    boost::tie(minPoint, maxPoint) = extremePoints;
}

static void fitImageLine(Line &line,
        const vector<LaneMarkingDetector::MarkingPoint> component)
{
    vector<cv::Point2f> points;
    for (unsigned int i = 0; i < component.size(); i++)
    {
        points.push_back(component[i].imagePoint);
    }
    line.fit(points);
}

static Line::ExtremePointsF getExtremePointsImage(Line &line,
        const vector<LaneMarkingDetector::MarkingPoint> component)
{
    vector<cv::Point2f> points;
    for (unsigned int i = 0; i < component.size(); i++)
    {
        points.push_back(component[i].imagePoint);
    }
    return line.get_extreme_pointsF(points);
}

void LaneMarkingDetector::drawComponentsImage(cv::Mat &img,
        const vector<MarkingComponent> &components)
{
    Line line;
    cv::Point2f min_point, max_point;
    Util util;

    for (unsigned int i = 0; i < components.size(); i++)
    {
        fitImageLine(line, components[i]);
        Line::ExtremePointsF extreme_points = getExtremePointsImage(line,
                components[i]);
        boost::tie(min_point, max_point) = extreme_points;
        cv::line(img, util.RoundPoint(min_point), util.RoundPoint(max_point),
                cv::Scalar(0, 0, 255), 2, CV_AA, 0);
    }
}

void LaneMarkingDetector::GetCartesianProduct(vector<MarkingComponent> &leftComponents,
					      vector<MarkingComponent> &rightComponents,
					      vector< vector < vector<MarkingComponent> > > &cartesianProduct)
{

        for (unsigned int i = 0; i < leftComponents[i].size(); i++)
	{

//		cout << "Left =  " << leftComponents.size() << endl;
	
//		cout <<  "Right = " << rightComponents.size() << endl;
/*	
	        for (unsigned int j = 0; j < rightComponents[j].size(); j++)
		{
			
		//	std::vector< vector<MarkingComponent> >  vec; 
		//	vec.push_back(leftComponents[i][j]);
		//	vec.push_back(rightComponents[i][j]);
			cout << "I = " << i << " J = " << j << endl;
			//cartesianProduct.push_back(vec);

		
		}
*/
	}

}


void LaneMarkingDetector::FindLeftAndRightCandidates(cv::Mat &img,
         const vector<MarkingComponent> &components,
	 vector<MarkingComponent> &leftComponents,
	 vector<MarkingComponent> &rightComponents 
         )
{
    Line line;
    cv::Point2f min_point, max_point;
    Util util;
    MarkingComponents rightComp; 
    MarkingComponents leftComp; 
		
    
    int left =0;
    int right = 0; 	
 
    for (unsigned int i = 0; i < components.size(); i++)
    {
        //draw the line in the image
	fitImageLine(line, components[i]);
        Line::ExtremePointsF extreme_points = getExtremePointsImage(line, components[i]);
        boost::tie(min_point, max_point) = extreme_points;

	MarkingPoint maxPoint;
        maxPoint.imagePoint = max_point;
        _pUtil->imagePlaneToGroundPlane(maxPoint.imagePoint, maxPoint.groundPoint, _pMatHinv);

	MarkingPoint minPoint;
        minPoint.imagePoint = min_point;
        _pUtil->imagePlaneToGroundPlane(minPoint.imagePoint, minPoint.groundPoint, _pMatHinv);

	if(minPoint.groundPoint.x > 0.5 && maxPoint.groundPoint.x >0.5)
	{
                rightComponents.push_back(components[i]);
		rightId++;
		rightComp.components = rightComponents; 
                rightComp.id = rightId; 
        	cv::line(img, util.RoundPoint(min_point), util.RoundPoint(max_point), cv::Scalar(0, 0, 255), 0.5, CV_AA, 0);
		right++;
	}
	else
	{
                leftComponents.push_back(components[i]);
	        leftId++; 	
		leftComp.components = leftComponents;
		leftComp.id = leftId; 
		cv::line(img, util.RoundPoint(min_point), util.RoundPoint(max_point), cv::Scalar(255, 0, 0), 2, CV_AA, 0);
		left++;

	}

    }

    std::vector<MarkingPoint> mcr;

    std::vector<MarkingPoint> mcl;
	
    //adding the empty 	
    mcl.clear();
    mcr.clear(); 
    MarkingPoint empty; 

    mcl.push_back(empty);
    mcr.push_back(empty); 
	
    right++; 
    left++;		

    leftComponents.push_back(mcl);
    rightComponents.push_back(mcr); 			
    		
    leftId++; 	
    leftComp.components = leftComponents;
    leftComp.id = leftId; 

    rightId++;
    rightComp.components = rightComponents; 
    rightComp.id = rightId; 

    cout << "NO OF LEFT COMPONENTS= " << left << endl; 
    cout << "NO OF RIGHT COMPONENTS= "<< right << endl;
    cout << "Left Component Id= "<< leftComp.id << endl;		
    cout << "Right Component Id= "<< rightComp.id << endl;		

}



void LaneMarkingDetector::drawComponentsGround(cv::Mat &ground_img,
        vector<vector<MarkingPoint> > &components)
{
    for (unsigned int i = 0; i < components.size(); i++)
    {
        for (unsigned int j = 0; j < components[i].size(); j++)
        {
            cv::Point point;
            point.x = 320 + components[i][j].groundPoint.x * 60;
            point.y = 479 - components[i][j].groundPoint.y * 20;

            if (point.y >= 0 && point.y <= ground_img.rows && point.x >= 0
                    && point.x <= ground_img.cols)
            {
                ground_img.at<cv::Vec3b>(point.y, point.x)[0] = 0;
                ground_img.at<cv::Vec3b>(point.y, point.x)[1] = 0;
                ground_img.at<cv::Vec3b>(point.y, point.x)[2] = 255;
            }
        }
    }
}

void LaneMarkingDetector::findLaneBoundaries(const IplImage *pImageOriginal,
        IplImage *pImageC, Boundary &leftBoundary, Boundary &rightBoundary,
        bool bDisableDispWindows, bool bDebug, int leftId, int rightId)
{

    timeval startTime = _pTimer->getCurrentTime();

    // Build kernel for scanning each row of image

    vector<float *> vKernels;
    for (unsigned int i = 0; i < _vWidths.size(); i++)
    {
        float *kernel = NULL;
        _pConvolutionObj->kernel1D(_vWidths[i], &kernel);
        vKernels.push_back(kernel);
        kernel = NULL;
    }

    timeval kernelTime = _pTimer->getCurrentTime();
    if (bDebug)
    {
        double dTimeDiffKernel = _pTimer->getElapsedTimeInMilliSec(startTime,
                kernelTime);
        cout << "Compute kernel time " << dTimeDiffKernel << " ms" << endl;
    }

    // Perform 1D convolution

    cv::Mat matImg(pImageC);

    if (!bDisableDispWindows)
    {
        cv::imshow("processing image", matImg);
        cvWaitKey(2);
    }

    _pConvolutionObj->runConvolution1D(matImg, vKernels, _vWidths);
    timeval convTime = _pTimer->getCurrentTime();
    if (bDebug)
    {
        double dTimeDiffConv = _pTimer->getElapsedTimeInMilliSec(kernelTime,
                convTime);
        cout << "Convolution time " << dTimeDiffConv << " ms" << endl;
    }

    if (!bDisableDispWindows)
    {
        cv::imshow("convolution result", matImg);
        cvWaitKey(2);
    }

    // Get connected components

    if (!_pConnectedCompObj)
    {
        ConnectedComponentDetector::Algorithm algorithm =
                ConnectedComponentDetector::NINE_POINT; //::FOUR_POINT;//
        _pConnectedCompObj = new ConnectedComponentDetector(pImageC, algorithm);
    }

    _pConnectedCompObj->setImage(pImageC);

    std::vector<std::vector<cv::Point> > imagePlaneConnectedComponents;
    _pConnectedCompObj->getImagePlaneConnectedComponents(
            imagePlaneConnectedComponents);

    timeval connCompTime = _pTimer->getCurrentTime();
    if (bDebug)
    {
        double dTimeDiffConnComp = _pTimer->getElapsedTimeInMilliSec(convTime,
                connCompTime);
        cout << "Connected components time " << dTimeDiffConnComp << " ms"
                << endl;
    }

    // Get consistent inlier components

    std::vector<MarkingComponent> markingComponents;
    
    std::vector<MarkingComponent> leftComponents;
    std::vector<MarkingComponent> rightComponents;
    
    
    getLinearInlierComponents(imagePlaneConnectedComponents, markingComponents,
            false);
    //removeInconsistentGroundPlaneComponents(markingComponents);

    timeval inlierTime = _pTimer->getCurrentTime();
    if (bDebug)
    {
        double dTimeDiffConnComp = _pTimer->getElapsedTimeInMilliSec(
                connCompTime, inlierTime);
        cout << "Inlier components time " << dTimeDiffConnComp << " ms" << endl;
    }

    if (!bDisableDispWindows)
    {
        cv::Mat matInlierComps = cv::Mat::zeros(pImageC->height, pImageC->width,
                CV_8UC3);
        //drawComponentsImage(matInlierComps, markingComponents);

	cvWaitKey(0); 
	FindLeftAndRightCandidates(matInlierComps, markingComponents, leftComponents, rightComponents);


        std::vector< vector < vector<MarkingComponent> > >  cartesianProduct;

	GetCartesianProduct(leftComponents, rightComponents, cartesianProduct); 

	//cvWaitKey(0);


	
        cv::imshow("Inlier components", matInlierComps);

        cv::Mat matGroundPlaneImage = cv::Mat::zeros(pImageC->height,
                pImageC->width, CV_8UC3);
        drawComponentsGround(matGroundPlaneImage, markingComponents);
        imshow("Ground plane inlier components", matGroundPlaneImage);

        cvWaitKey(2);
    }

    std::vector<std::vector<MarkingComponent> > groupedMarkingComponents;
    groupLaneMarkings(markingComponents, groupedMarkingComponents);


    if (bDebug)
    {
        Line line;
        cout << "Grouping markings: " << markingComponents.size()
                << " markings placed in " << groupedMarkingComponents.size()
                << " groups." << endl;
        for (unsigned int i = 0; i < markingComponents.size(); i++)
        {
            cout << "Original marking: " << markingComponents[i].size()
                    << " points" << endl;
        }
        for (unsigned int i = 0; i < groupedMarkingComponents.size(); i++)
        {
            cout << "Group " << i << ":" << endl;
            for (unsigned int j = 0; j < groupedMarkingComponents[i].size();
                    j++)
            {
                cv::Point2f pointMin, pointMax;
                cout << "  marking " << j << ": "
                        << groupedMarkingComponents[i][j].size() << " points"
                        << endl;
                fitGroundLine(line, groupedMarkingComponents[i][j], pointMin,
                        pointMax);
                cout << "    points " << pointMin << ", " << pointMax << endl;
            }
        }
    }

    findBestBoundaries(groupedMarkingComponents, leftBoundary, rightBoundary,
            pImageOriginal, bDisableDispWindows, bDebug);

    timeval finishTime = _pTimer->getCurrentTime();
    double dTimeDiffWidths = _pTimer->getElapsedTimeInMilliSec(startTime,
            finishTime);
    if (bDebug)
    {
        cout << "Find lane markings total time: " << dTimeDiffWidths << " ms"
                << endl;
    }

    for (unsigned int i = 0; i < vKernels.size(); i++)
    {
        if (vKernels[i])
            delete vKernels[i];
    }
}

#define SWAP(a,b) {tmp = (a); (a) = (b); (b) = tmp;}

static double overlap(float x11, float x12, float x21, float x22)
{
    float tmp;
    if (x11 > x12)
    {
        SWAP(x12, x11);
    }
    if (x21 > x22)
    {
        SWAP(x21, x22);
    }
    if (x11 > x21)
    {
        SWAP(x11, x21);
        SWAP(x12, x22);
    }
    if (x21 > x12)
        return 0;
    if (x12 > x22)
        return 1.0;
    double overlap1 = (x12 - x21) / (x12 - x11);
    double overlap2 = (x12 - x21) / (x22 - x21);
    return max(overlap1, overlap2);
}

static double lineLineOverlap(cv::Point2f minPoint1, cv::Point2f maxPoint1,
        cv::Point2f minPoint2, cv::Point2f maxPoint2)
{
    double extentX = max(fabs(maxPoint1.x - minPoint1.x),
            fabs(maxPoint2.x - minPoint2.x));
    double extentY = max(fabs(maxPoint1.y - minPoint1.y),
            fabs(maxPoint2.y - minPoint2.y));

    if (extentX > extentY)
    {
        return overlap(minPoint1.x, maxPoint1.x, minPoint2.x, maxPoint2.x);
    } else
    {
        return overlap(minPoint1.y, maxPoint1.y, minPoint2.y, maxPoint2.y);
    }
}

void LaneMarkingDetector::checkConsistency(MarkingComponent component1,
        MarkingComponent component2, cv::Point2f normal1, cv::Point2f normal2,
        cv::Point2f point1, cv::Point2f point2, cv::Point2f minPoint1,
        cv::Point2f minPoint2, cv::Point2f maxPoint1, cv::Point2f maxPoint2,
        bool &isConsistent)
{
    isConsistent = false;

    // Quick screening based on position and normal vectors

    if (!_pRANSAC->isConsistent(normal1, normal2, point1, point2))
    {
        return;
    }

    // Quick check based on overlap

    if (lineLineOverlap(minPoint1, maxPoint1, minPoint2, maxPoint2) > 0.1)
    {
        return;
    }

    // Get the image points

    vector<cv::Point2f> pointsImage;
    for (unsigned int i = 0; i < component1.size(); i++)
    {
        pointsImage.push_back(component1[i].imagePoint);
    }
    for (unsigned int i = 0; i < component2.size(); i++)
    {
        pointsImage.push_back(component2[i].imagePoint);
    }
    vector<cv::Point2f> pointsImageInliers;
    _pRANSAC->fitLine(pointsImage, pointsImageInliers);
    if (pointsImageInliers.size()
            >= REQUIRED_LINE_INLIER_RATIO * pointsImage.size() &&
            pointsImageInliers.size() > MIN_NUM_OF_CONNECTED_COMP_INLIERS)
    {
        isConsistent = true;
    }
}

void LaneMarkingDetector::groupLaneMarkings(
        const std::vector<MarkingComponent> markingComponents,
        std::vector<std::vector<MarkingComponent> >& groupedMarkingComponents)
{
    if (markingComponents.size() == 0)
        return;

    // Fit lines to each component

    cv::Point2f normals[markingComponents.size()];
    cv::Point2f points[markingComponents.size()];
    cv::Point2f minPoints[markingComponents.size()];
    cv::Point2f maxPoints[markingComponents.size()];

    for (unsigned int i = 0; i < markingComponents.size(); i++)
    {
        Line line;
        fitGroundLine(line, markingComponents[i], minPoints[i], maxPoints[i]);
        normals[i] = line.getDir();
        points[i] = line.getPoint();
    }

    // Find each consistent pair of markings

    bool isConsistent[markingComponents.size()][markingComponents.size()];
    assert(markingComponents.size() > 0);
    for (unsigned int i = 0; i < markingComponents.size() - 1; i++)
    {
        isConsistent[i][i] = true;
        for (unsigned int j = i + 1; j < markingComponents.size(); j++)
        {
            bool isPairConsistent = false;
            checkConsistency(markingComponents[i], markingComponents[j],
                    normals[i], normals[j], points[i], points[j], minPoints[i],
                    minPoints[j], maxPoints[i], maxPoints[j], isPairConsistent);

            if (isPairConsistent)
            {
                isConsistent[i][j] = isConsistent[j][i] = true;
            } else
            {
                isConsistent[i][j] = isConsistent[j][i] = false;
            }
        }
    }
    isConsistent[markingComponents.size() - 1][markingComponents.size() - 1] =
            true;

    // Repeatedly pull out the largest consistent group until no markings are left

    bool isGrouped[markingComponents.size()];
    for (unsigned int i = 0; i < markingComponents.size(); i++)
        isGrouped[i] = false;
    int cGrouped = 0;
    while (true)
    {
        int iBest = -1;
        int cConsistentBest = 0;
        for (unsigned int i = 0; i < markingComponents.size(); i++)
        {
            if (isGrouped[i])
            {
                continue;
            }
            int cConsistent = 0;
            for (unsigned int j = 0; j < markingComponents.size(); j++)
            {
                if (isGrouped[j])
                    continue;
                if (isConsistent[i][j])
                    cConsistent++;
            }
            if (cConsistent > cConsistentBest)
            {
                iBest = i;
                cConsistentBest = cConsistent;
            }
        }
        if (iBest == -1)
            break;
        std::vector<std::vector<MarkingPoint> > markingComponentGroup;
        for (unsigned int i = 0; i < markingComponents.size(); i++)
        {
            if (!isGrouped[i] && isConsistent[iBest][i])
            {
                markingComponentGroup.push_back(markingComponents[i]);
                isGrouped[i] = true;
            }
        }
        assert((int )markingComponentGroup.size() == cConsistentBest);
        groupedMarkingComponents.push_back(markingComponentGroup);
        cGrouped += markingComponentGroup.size();
    }
    assert(cGrouped == (int )markingComponents.size());
}

// What makes a boundary a good one?  It should start close to the vehicle, it should have
// a lot of points, and it should be close to the vehicle to the left or right.  We formulate
// this as a simple likelihood function where the features are assumed independent, the
// nearest point's longitudinal distance is assumed exponential, the number of points is
// Gaussian to some number then uniform, and the lateral distance is Gaussian.

#define MEAN_MIN_Y 3.0
#define POINT_MIN_LAMBDA (1.0/(MEAN_MIN_Y))
#define GOOD_CPOINTS 80
#define CPOINTS_STDEV_SQ (20*20)

double LaneMarkingDetector::boundaryLikelihood(double minY, int cPoints,
        double dPerp)
{
    // likelihood of pointMin is exponential in y
    double likelihoodPointMin = exp(- POINT_MIN_LAMBDA * minY);
    // likelihood of cPoints is Gaussian up to some value then uniform
    double likelihoodCPoints =
            cPoints >= GOOD_CPOINTS ?
                    1.0 :
                    exp(
                            -(GOOD_CPOINTS - cPoints)
                                    * (GOOD_CPOINTS - cPoints)/ 2 / CPOINTS_STDEV_SQ);
    // likelihood of dPerp is Gaussian
    Assert<InvalidParameterException>(_dExpectedLaneWidth > 0);
    double meanDperp = _dExpectedLaneWidth / 2;
    double stdevDperp = _dExpectedLaneWidth / 2;
    double likelihoodDPerp = exp(
            -(dPerp - meanDperp) * (dPerp - meanDperp) / 2 / stdevDperp
                    / stdevDperp);

    return likelihoodPointMin * likelihoodCPoints * likelihoodDPerp;
}

static double normalizeYaw(double yaw)
{
    while (yaw < 0)
        yaw += M_PI;
    while (yaw > M_PI)
        yaw -= M_PI;
    return yaw;
}

// Find the best left and right lane boundaries from the given set of grouped candidate
// lane markings based on distance to the vehicle, nearness to the vehicle, number of
// image points comprising the boundary, etc.
//
// Note that this method does not yet treat the left and right boundaries as a pair,
// which would probably be better.

void LaneMarkingDetector::findBestBoundaries(
        std::vector<std::vector<MarkingComponent> > &groupedMarkingComponents,
        Boundary &bestLeftBoundary, Boundary &bestRightBoundary,
        const IplImage *pImage, bool bDisableDispWindows, bool bDebug)
{
    if (groupedMarkingComponents.size() == 0)
        return;

    cv::Mat matImg(pImage);

    bestLeftBoundary.likelihood = bestRightBoundary.likelihood = 0;

    // groupedLaneMarkings contains consensus sets of lane markings.
    // Each consensus set contains some number of consistent lane markings.
    // Each lane marking contains a set of points points.

    for (unsigned int i = 0; i < groupedMarkingComponents.size(); i++)
    {
        // Fit a RANSAC line to the points in the entire group.
        // If the fit is not good enough, remove components from
        // the fit until it is good enough.

        bool isFitGoodEnough = false;
        std::vector<cv::Point2f> pointsGroupInliers;
        bool abIncludeComponent[groupedMarkingComponents[i].size()];

        for (unsigned int j = 0; j < groupedMarkingComponents[i].size(); j++)
        {
            abIncludeComponent[j] = true;
        }

        while (!isFitGoodEnough)
        {
            std::vector<cv::Point2f> pointsGroup;
            int cPointsSmallest = INT32_MAX;
            int iIndexSmallest = -1;

            // Pull points from components in the group

            int cIncluded = 0;
            for (unsigned int j = 0; j < groupedMarkingComponents[i].size();
                    j++)
            {
                if (abIncludeComponent[j])
                {
                    cIncluded++;
                    for (unsigned int k = 0;
                            k < groupedMarkingComponents[i][j].size(); k++)
                    {
                        pointsGroup.push_back(
                                groupedMarkingComponents[i][j][k].imagePoint);
                    }
                    if ((int) groupedMarkingComponents[i][j].size()
                            < cPointsSmallest)
                    {
                        cPointsSmallest = groupedMarkingComponents[i][j].size();
                        iIndexSmallest = j;
                    }
                }
            }

            // Fit a line

            pointsGroupInliers.resize(0);
            _pRANSAC->fitLine(pointsGroup, pointsGroupInliers);

            if (bDebug)
            {
                cout << i << ": " << pointsGroupInliers.size() << " inliers."
                        << endl;
            }

            if (cIncluded == 1
                    || pointsGroupInliers.size()
                            >= REQUIRED_LINE_INLIER_RATIO * pointsGroup.size())
            {
                isFitGoodEnough = true;
            } else
            {
                abIncludeComponent[iIndexSmallest] = false;
            }
        }

        _pLine->fit(pointsGroupInliers);

        // Collect data about this boundary

        Line::ExtremePointsF extreme_points = _pLine->get_extreme_pointsF(
                pointsGroupInliers);
        Boundary boundary;
        boundary.cPoints = pointsGroupInliers.size();
        boost::tie(boundary.pointMin, boundary.pointMax) = extreme_points;
        _pUtil->imagePlaneToGroundPlane(boundary.pointMin, boundary.pointMin,
                _pMatHinv);
        _pUtil->imagePlaneToGroundPlane(boundary.pointMax, boundary.pointMax,
                _pMatHinv);
        boundary.dPerp = _pRANSAC->perpendicularDistance(boundary.pointMin,
                boundary.pointMax, cv::Point2f(0, 0), boundary.pointNearest);
        boundary.dYaw = normalizeYaw(
                atan2(boundary.pointMax.y - boundary.pointMin.y,
                        boundary.pointMax.x - boundary.pointMax.x));
        boundary.side = (boundary.pointNearest.x < 0) ? LEFT : RIGHT;
        boundary.likelihood = boundaryLikelihood(
                MIN(boundary.pointMin.y, boundary.pointMax.y), boundary.cPoints,
                boundary.dPerp);

        if (bDebug)
        {
            cout << "Group " << i << ": min " << boundary.pointMin << " max "
                    << boundary.pointMax << " dist " << boundary.dPerp
                    << " cPoints " << boundary.cPoints << " nearest point "
                    << boundary.pointNearest << " likelihood "
                    << boundary.likelihood << endl;
        }

        // Save the best

        if (boundary.side == LEFT
                && boundary.likelihood > bestLeftBoundary.likelihood)
            bestLeftBoundary = boundary;
        if (boundary.side == RIGHT
                && boundary.likelihood > bestRightBoundary.likelihood)
            bestRightBoundary = boundary;
    }

    // Draw on image

    if (!bDisableDispWindows)
    {
        if (bestLeftBoundary.likelihood > 0)
        {
            cv::Point2f point1, point2;
            _pUtil->groundPlaneToImagePlane(bestLeftBoundary.pointMin, point1,
                    _pMatH);
            _pUtil->groundPlaneToImagePlane(bestLeftBoundary.pointMax, point2,
                    _pMatH);
            cv::line(matImg, point1, point2, cv::Scalar(0, 0, 255), 2, CV_AA,
                    0);
        }
        if (bestRightBoundary.likelihood > 0)
        {
            cv::Point2f point1, point2;
            _pUtil->groundPlaneToImagePlane(bestRightBoundary.pointMin, point1,
                    _pMatH);
            _pUtil->groundPlaneToImagePlane(bestRightBoundary.pointMax, point2,
                    _pMatH);
            cv::line(matImg, point1, point2, cv::Scalar(0, 0, 255), 2, CV_AA,
                    0);
        }
        cv::imshow("Road Paint Detector", matImg);
        cvWaitKey(2);
    }
}

#define CUT_OFF_DISTANCE_ON_Y_AXIS 20.0
#define CUT_OFF_DISTANCE_ON_X_AXIS 3.0

void LaneMarkingDetector::removeInconsistentGroundPlaneComponents(
        std::vector<MarkingComponent> &markingComponents)
{
    Point2fSequences filteredGroundPlaneComponents;
    vector<vector<MarkingPoint> > filteredMarkingComponents;
    for (unsigned int i = 0; i < markingComponents.size(); i++)
    {
        bool bFarAway = true;
        for (unsigned int j = 0; j < markingComponents[i].size(); j++)
        {
            if (markingComponents[i][j].groundPoint.y
                    < CUT_OFF_DISTANCE_ON_Y_AXIS
                    && markingComponents[i][j].groundPoint.y
                            > 0&&
                            fabs(markingComponents[i][j].groundPoint.x) < CUT_OFF_DISTANCE_ON_X_AXIS)
            {
                bFarAway = false;
            }
        }
        if (!bFarAway)
        {
            filteredMarkingComponents.push_back(markingComponents[i]);
        }
    }

    markingComponents.clear();
    for (unsigned int i = 0; i < filteredMarkingComponents.size(); i++)
    {
        markingComponents.push_back(filteredMarkingComponents[i]);
    }
}

