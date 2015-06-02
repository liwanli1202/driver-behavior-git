/*
 * LaneMarkingDetector.h
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef LANEMARKINGDETECTOR_H_
#define LANEMARKINGDETECTOR_H_

#include "CameraGeometry.h"
#include "ConnectedComponentDetector.h"
#include "Convolution.h"
#include "Lane.h"
#include "Line.h"
#include "RANSACEstimator.h"
#include "Util.h"
#include "Timer.h"
#include <cv.h>
#include <queue>
#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace cv;


class LaneMarkingDetector {
public:

	LaneMarkingDetector();
	virtual ~LaneMarkingDetector();

	enum eSide { LEFT, RIGHT };
	class Boundary {
	public:
		double dPerp;
		double dYaw;
		cv::Point2f pointMin;
		cv::Point2f pointMax;
		int cPoints;
		cv::Point2f pointNearest;
		eSide side;
		double likelihood;
	};

	class MarkingPoint {
	public:
		cv::Point2f imagePoint;
		cv::Point2f groundPoint;
		int iComponent;
	};
	typedef std::vector<MarkingPoint> MarkingComponent;

	void initializeCameraModel(
			float focalLengthX, float focalLengthY, float px, float py,
			int width, int height,
			float pitch, float roll, float yaw,
			float tCWx, float tCWy, float tCWz);

	void initializeCameraModelP(
			double p11, double p12, double p13, double p14,
			double p21, double p22, double p23, double p24,
			double p31, double p32, double p33, double p34,
			int width, int height);

	void findLaneBoundaries(
			const IplImage *pImageOriginal, IplImage *pImageC,
			Boundary &leftBoundary, Boundary &rightBoundary,
			bool bDisableDispWindows, bool bDebug);

	void findBestBoundaries(
			std::vector< std::vector<MarkingComponent> > &groupedMarkingComponents,
			Boundary &bestLeftBoundary,
			Boundary &bestRightBoundary,
			const IplImage *pImage,
			bool bDisableDispWindows, bool bDebug);

	void findLaneMarking(
			const IplImage *pImageOriginal, IplImage *pImageC,
			vector<int> vWidthOfLaneMarkingOnEachRowOnImage, bool bDisableDispWindows, bool bDebug);

	void groupLaneMarkings(
			const std::vector<MarkingComponent> markingComponents,
			std::vector< std::vector<MarkingComponent> >& groupedMarkingComponents);

	double getLeftBoundaryDist();
	double getRightBoundaryDist();
	CvMat *getHinv();
	CvMat *getH();
	void setIgnorePixels(int left, int right, int top, int bottom);
	void setExpectedLaneWidth(double dExpectedLaneWidth);

private:

	CameraGeometry *_pCameraGeometryObj;
	ConnectedComponentDetector *_pConnectedCompObj;
	Convolution *_pConvolutionObj;
	RANSACEstimator *_pRANSAC;
	Util *_pUtil;
	Line *_pLine;
	Lane *_pLane;
    Timer *_pTimer;
	CvMat* _pMatHinv;
	CvMat* _pMatH;
	vector<int> _vWidths;
	double _dExpectedLaneWidth;

	typedef std::vector<std::vector<cv::Point2f> > Point2fSequences;

	void calculateLaneMarkingWidths();

	void drawComponentsGround(
			cv::Mat &ground_img,
			vector<MarkingComponent> &ground_plane_sequences);

	void getLinearInlierComponents(
			const vector< vector<cv::Point> > &connectedComponentsOnImagePlane,
			vector<MarkingComponent> &markingComponents,
			bool bDebug);

	void drawComponentsImage(
			cv::Mat &img,
			const vector<MarkingComponent> &components);

        void FindLeftAndRightCandidates(
                        cv::Mat &img,
                        const vector<MarkingComponent> &components,
                        vector<MarkingComponent> &leftComponents,
                        vector<MarkingComponent> &rightComponents);

	void GetCartesianProduct(vector<MarkingComponent> &leftComponents,
                                                      vector<MarkingComponent> &rightComponents,
                                                      vector< vector <vector<MarkingComponent> > > &cartesianProduct);
	

	void removeInconsistentGroundPlaneComponents(
			std::vector<MarkingComponent> &components);

	double boundaryLikelihood(
			double minY,
			int cPoints,
			double dPerp);

	void makeMarkingComponents(
			vector< vector<cv::Point2f> > &imageComponents,
			vector<MarkingComponent> &markingComponents);

	void checkConsistency(
			MarkingComponent component1, MarkingComponent component2,
			cv::Point2f normal1, cv::Point2f normal2,
			cv::Point2f point1, cv::Point2f point2,
			cv::Point2f minPoint1, cv::Point2f minPoint2,
			cv::Point2f maxPoint1, cv::Point2f maxPoint2,
			bool &isConsistent);

};

#endif /* LANEMARKINGDETECTOR_H_ */
