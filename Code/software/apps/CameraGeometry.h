/*
 * CameraGeometry.h
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef CAMERAGEOMETRY_H_
#define CAMERAGEOMETRY_H_

#include <cv.h>
using namespace std;

class CameraGeometry {
public:
	CameraGeometry();
	virtual ~CameraGeometry();

	CameraGeometry(float focalLengthX, float focalLengthY, float px, float py,
	    		int width, int height,
	    		float pitch, float roll, float yaw,
	    		float tCWx, float tCWy, float tCWz);

	CameraGeometry(double P[3][4], int width, int height);

	cv::Point2f imageToGroundPlane(cv::Point pt);
	int calculateWidthInPixels(float Y);
	float calculateY(int current_row);
	void imageToGroundPlane(std::vector<cv::Point> points,
							std::vector<cv::Point2f>& ground_points);
	cv::Point2f GroundPointToImagePoint(cv::Point2f point);
	void ground_points_to_image_points(std::vector<cv::Point2f>& points,
									   std::vector<cv::Point2f>& image_points);

	void setK(float focalLengthX, float focalLengthY, float px, float py);
	CvMat* getK();

	void setRcw_x(float theta);
	CvMat* getRcw_x();
	void setRcw_y(float theta);
	CvMat* getRcw_y();
	void setRcw_z(float theta);
	CvMat* getRcw_z();
	void setRcw();
	CvMat* getRcw();
	void setTcw(float x, float y, float z);
	CvMat* getTcw();
	CvMat* getRTcw();

	void setP();
	CvMat* getP();

	void setH();
	CvMat* getH();
	void setHInv();
	CvMat* getHInv();

	int getImgWidth();
	int getImgHeight();

private:
    CvMat *_matK;
    CvMat *_matRcw_x;
    CvMat *_matRcw_y;
    CvMat *_matRcw_z;
    CvMat *_matRcw;
    CvMat *_matKRcw;
    CvMat *_matTcw;
    CvMat *_matP;
    CvMat *_matH;
    CvMat *_matHInv;
    int _imgWidth;
    int _imgHeight;

    float convertDegreesToRadians(float degrees);
};

#endif /* CAMERAGEOMETRY_H_ */
