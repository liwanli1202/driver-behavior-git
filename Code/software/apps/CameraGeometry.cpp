/*
 * CameraGeometry.cpp
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include "CameraGeometry.h"

CameraGeometry::CameraGeometry() {
	_matK      = NULL;
    _matRcw_x  = NULL;
    _matRcw_y  = NULL;
    _matRcw_z  = NULL;
    _matRcw	   = NULL;
    _matKRcw   = NULL;
    _matTcw    = NULL;
    _matP      = NULL;
    _matH      = NULL;
    _matHInv   = NULL;
    _matP      = NULL;
    _imgHeight = 0;
    _imgWidth  = 0;
}

CameraGeometry::~CameraGeometry() {
}


CameraGeometry::CameraGeometry(double P[3][4], int width, int height)
{
	_matK      = NULL;
    _matRcw_x  = NULL;
    _matRcw_y  = NULL;
    _matRcw_z  = NULL;
    _matRcw	   = NULL;
    _matKRcw   = NULL;
    _matTcw    = NULL;
    _matP      = NULL;
    _imgWidth  = width;
    _imgHeight = height;

    _matP = cvCreateMat(3, 4, CV_32FC1);

    for (int i = 0; i < 3; i++)
    {
    	for (int j = 0; j < 4; j++)
    	{
    		cvmSet(_matP, i, j, P[i][j]);
    	}
    }

    _matH = cvCreateMat(3, 3, CV_32FC1);
    _matHInv = cvCreateMat(3, 3, CV_32FC1);

    setH();
    setHInv();
}


CameraGeometry::CameraGeometry(
		float focalLengthX, float focalLengthY, float px, float py,
		int width, int height,
		float pitch, float roll, float yaw,
		float tCWx, float tCWy, float tCWz )
{
	// Sunny's video (movie.avi)
	// X_CW = 0.0  //meters
	// Y_CW = 2.15 //meters
	// Z_CW = 1.0  //meters
	// Jednipat's video
	// X_CW = 0.0  //meters
	// Y_CW = 0.0  //meters
	// Z_CW = 1.0  //meters
    const float X_CW = tCWx;
    const float Y_CW = tCWy;
    const float Z_CW = tCWz;

    _imgWidth = width;
    _imgHeight = height;

    _matK     = cvCreateMat(3, 3, CV_32FC1);
    _matRcw_x = cvCreateMat(3, 3, CV_32FC1);
    _matRcw_y = cvCreateMat(3, 3, CV_32FC1);
    _matRcw_z = cvCreateMat(3, 3, CV_32FC1);
    _matRcw	  = cvCreateMat(3, 3, CV_32FC1);
    _matKRcw  = cvCreateMat(3, 3, CV_32FC1);
    _matTcw   = cvCreateMat(3, 1, CV_32FC1);
    _matP     = cvCreateMat(3, 4, CV_32FC1);
    _matH     = cvCreateMat(3, 3, CV_32FC1);
    _matHInv  = cvCreateMat(3, 3, CV_32FC1);

    /*
     * Construct Rcw and Tcw
     * Camera coordinate sytem
     * ----------> Cz
     * |\
     * | \
     * |  \
     * |   \
     * Cy   Cx
     */

    /*
     * Wz
     * |
     * |
     * |
     * |
     * ----------> Wy
     * \
     *  \
     *   \
     *    \
     *    Wx
     */

    // Rotation matrix that aligns camera coordinate system with world coordinate system
    // NOTE: unit is in degrees. These parameters will be converted to radian again.
    // IMPORTANT: This variable must be changed for each videos. This default parameters
    //            are used with movie.avi which are recorded on Bang Na express way.
    this->setRcw_x(pitch); // Rotate camera coordinate about x-axis (Rotation with right hand's rule)
    this->setRcw_y(roll);  // Rotate camera coordinate about y-axis (Rotation with right hand's rule)
    this->setRcw_z(yaw);   // Rotate camera coordinate about z-axis (Rotation with right hand's rule)
    this->setRcw();

    // Translation matrix that moves the center of camera coordinate system to
    // the origin of world coordinate system
    this->setTcw(X_CW, Y_CW, Z_CW);

    // Camera matrix (Intrinsic camera parameters)
    // Jednipat's camera
    // [	577.100133   	0		301.765368
    // 		0			575.471635	258.032040
    //		0				0			1		]
    this->setK(focalLengthX, focalLengthY, px, py );

    // x = PX
    // P = [KxR | T]
    // where
    // x is a location based on image coordinate system (i.e 2D location)
    // X is a location based on world coordinate system (i.e 3D location)
    this->setP();

    // Get H from P
    this->setH();
    this->setHInv();

}

void CameraGeometry::setK(float focalLengthX, float focalLengthY, float px, float py){


    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 0, 0)) = focalLengthX;// 577.100133;//focal_length;
    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 0, 1)) = 0;
    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 0, 2)) = px;//301.765368;//p_x;
    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 1, 0)) = 0;
    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 1, 1)) = focalLengthY;//575.471635;//focal_length;
    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 1, 2)) = py;//258.032040;//p_y;
    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 2, 0)) = 0;
    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 2, 1)) = 0;
    *((float*)CV_MAT_ELEM_PTR(*(this->_matK), 2, 2)) = 1;

    // [	577.100133   	0		301.765368
	// 		0			575.471635	258.032040
	//		0				0			1		]
}

CvMat* CameraGeometry::getK(){
    return _matK;
}

void CameraGeometry::setRcw_x(float theta){
/*
Rx = 1        0               0
     0        cos(theta)      sin(theta)
     0        -sin(theta)    cos(theta)
*/
	theta = convertDegreesToRadians(theta); // theta in radians
//	cvmSet(this->Rcw, 0, 0, 1);
//	cvmSet(this->Rcw, 0, 1, 0);
//	cvmSet(this->Rcw, 0, 2, 0);
//	cvmSet(this->Rcw, 1, 0, 0);
//	cvmSet(this->Rcw, 1, 1, 0);
//	cvmSet(this->Rcw, 1, 2, 1);
//	cvmSet(this->Rcw, 2, 0, 0);
//	cvmSet(this->Rcw, 2, 1, -1);
//	cvmSet(this->Rcw, 2, 2, 0);

	// Updated by Jednipat
	cvmSet(this->_matRcw_x, 0, 0, 1);
	cvmSet(this->_matRcw_x, 0, 1, 0);
	cvmSet(this->_matRcw_x, 0, 2, 0);
	cvmSet(this->_matRcw_x, 1, 0, 0);
	cvmSet(this->_matRcw_x, 1, 1, cos(theta));
	cvmSet(this->_matRcw_x, 1, 2, sin(theta));
	cvmSet(this->_matRcw_x, 2, 0, 0);
	cvmSet(this->_matRcw_x, 2, 1, -sin(theta));
	cvmSet(this->_matRcw_x, 2, 2, cos(theta));
#ifdef DEBUG_MAT
	cout << "=============Rcw_x===============" << endl;
	for(int i=0; i < _matRcw_x->height; i++)
	{
		for(int j=0; j < _matRcw_x->width; j++)
		{
			cout << cvmGet(_matRcw_x,i,j) << "\t" ;
		}
		cout << endl;
	}
#endif

  return;
}

CvMat* CameraGeometry::getRcw_x(){
  return _matRcw_x;
}

void CameraGeometry::setRcw_y(float theta){
/*
Ry = cos(theta)  0   -sin(theta)
     0           1     0
     sin(theta)  0   cos(theta)
*/
    theta = convertDegreesToRadians(theta); // theta in radians
    cvmSet(this->_matRcw_y, 0, 0, cos(theta));
    cvmSet(this->_matRcw_y, 0, 1, 0);
    cvmSet(this->_matRcw_y, 0, 2, -sin(theta));
    cvmSet(this->_matRcw_y, 1, 0, 0);
    cvmSet(this->_matRcw_y, 1, 1, 1);
    cvmSet(this->_matRcw_y, 1, 2, 0);
    cvmSet(this->_matRcw_y, 2, 0, sin(theta));
    cvmSet(this->_matRcw_y, 2, 1, 0);
    cvmSet(this->_matRcw_y, 2, 2, cos(theta));
#ifdef DEBUG_MAT
    cout << "=============Rcw_y===============" << endl;
	for(int i=0; i < _matRcw_y->height; i++)
	{
		for(int j=0; j < _matRcw_y->width; j++)
		{
			cout << cvmGet(_matRcw_y,i,j) << "\t" ;
		}
		cout << endl;
	}
#endif
    return;
}

CvMat* CameraGeometry::getRcw_y(){
    return _matRcw_y;
}

void CameraGeometry::setRcw_z(float theta){
/*
Rz = cos(theta)  sin(theta)  0
     -sin(theta) cos(theta)  0
        0           0        1
*/
    theta = convertDegreesToRadians(theta); // theta in radians
    cvmSet(this->_matRcw_z, 0, 0, cos(theta));
    cvmSet(this->_matRcw_z, 0, 1, sin(theta));
    cvmSet(this->_matRcw_z, 0, 2, 0);
    cvmSet(this->_matRcw_z, 1, 0, -sin(theta));
    cvmSet(this->_matRcw_z, 1, 1, cos(theta));
    cvmSet(this->_matRcw_z, 1, 2, 0);
    cvmSet(this->_matRcw_z, 2, 0, 0);
    cvmSet(this->_matRcw_z, 2, 1, 0);
    cvmSet(this->_matRcw_z, 2, 2, 1);
#ifdef DEBUG_MAT
    cout << "=============Rcw_z===============" << endl;
	for(int i=0; i < _matRcw_z->height; i++)
	{
		for(int j=0; j < _matRcw_z->width; j++)
		{
			cout << cvmGet(_matRcw_z,i,j) << "\t" ;
		}
		cout << endl;
	}
#endif
    return;
}

CvMat* CameraGeometry::getRcw_z(){
    return _matRcw_z;
}

void CameraGeometry::setRcw()
{
	CvMat* tempRcw_x_y = cvCreateMat(3, 3, CV_32FC1);
	cvMatMul(_matRcw_x, _matRcw_y, tempRcw_x_y);
	cvMatMul(tempRcw_x_y, _matRcw_z, _matRcw);

}

CvMat* CameraGeometry::getRcw()
{
	return _matRcw;
}


void CameraGeometry::setTcw(float x, float y, float z){
/*
-- World Co-ordinate system --
	Z
    |
    |
    |
    |_ _ _ _ Y
     \
      \
       \
        \ X
*/
    cvmSet(this->_matTcw,0,0, x);
    cvmSet(this->_matTcw,1,0, y);
    cvmSet(this->_matTcw,2,0, z);

    return;
}

CvMat* CameraGeometry::getTcw(){
    return _matTcw;
}

void CameraGeometry::setP()
{
	// KxR
	cvMatMul(_matK, _matRcw, _matKRcw);
#ifdef DEBUG_MAT
	cout << "=============K===============" << endl;
	for(int i=0; i < _matK->height; i++)
	{
		for(int j=0; j < _matK->width; j++)
		{
			cout << cvmGet(_matK,i,j) << "\t" ;
		}
		cout << endl;
	}
	cout << "=============KxR===============" << endl;
	for(int i=0; i < _matKRcw->height; i++)
	{
		for(int j=0; j < _matKRcw->width; j++)
		{
			cout << cvmGet(_matKRcw,i,j) << "\t" ;
		}
		cout << endl;
	}
#endif
	// Copy KR to P
	for(int i=0; i < _matK->width; i++)
	{
		for(int j=0; j < _matK->height; j++)
		{
			cvmSet(_matP, i, j, cvmGet(_matKRcw,i,j));
		}
	}

	// P = [KxR|T]
	cvmSet(_matP, 0, 3, cvmGet(_matTcw,0,0));
	cvmSet(_matP, 1, 3, cvmGet(_matTcw,1,0));
	cvmSet(_matP, 2, 3, cvmGet(_matTcw,2,0));
#ifdef DEBUG_MAT
	cout << "=============P===============" << endl;
	for(int i=0; i < _matP->height; i++)
	{
		for(int j=0; j < _matP->width; j++)
		{
			cout << cvmGet(_matP,i,j) << "\t" ;
		}
		cout << endl;
	}
#endif
}

CvMat* CameraGeometry::getP()
{
	return _matP;
}

void CameraGeometry::setH()
{
	cvmSet(_matH,0,0,cvmGet(_matP,0,0));cvmSet(_matH,0,1,cvmGet(_matP,0,1));cvmSet(_matH,0,2,cvmGet(_matP,0,3));
	cvmSet(_matH,1,0,cvmGet(_matP,1,0));cvmSet(_matH,1,1,cvmGet(_matP,1,1));cvmSet(_matH,1,2,cvmGet(_matP,1,3));
	cvmSet(_matH,2,0,cvmGet(_matP,2,0));cvmSet(_matH,2,1,cvmGet(_matP,2,1));cvmSet(_matH,2,2,cvmGet(_matP,2,3));

//	cvmSet(_matH,0,0,771.57);cvmSet(_matH,0,1,306.61);cvmSet(_matH,0,2,242.57);
//	cvmSet(_matH,1,0,-36.778);cvmSet(_matH,1,1,-10.448);cvmSet(_matH,1,2,1482.9);
//	cvmSet(_matH,2,0,-0.01692);cvmSet(_matH,2,1,0.95185);cvmSet(_matH,2,2,0.55835);
#ifdef DEBUG_MAT
	cout << "=============H===============" << endl;
	for(int i=0; i < _matH->height; i++)
	{
		for(int j=0; j < _matH->width; j++)
		{
			cout << cvmGet(_matH,i,j) << "\t" ;
		}
		cout << endl;
	}
#endif
}

CvMat* CameraGeometry::getH()
{
	return _matH;
}

void CameraGeometry::setHInv()
{
	double retVal = cvInvert(_matH, _matHInv, CV_LU);
#ifdef DEBUG_MAT
	cout << "=============Hinv===============" << endl;
	for(int i=0; i < _matHInv->height; i++)
	{
		for(int j=0; j < _matHInv->width; j++)
		{
			cout << cvmGet(_matHInv,i,j) << "\t" ;
		}
		cout << endl;
	}
#endif

	if(retVal>0)
	{
//		cout << "H is invertible matrix." << endl;
	}else{
		cout << "H is not invertible matrix." << endl;
		exit(0);
	}
}

CvMat* CameraGeometry::getHInv()
{
	this->setHInv();
	return _matHInv;
}

int CameraGeometry::getImgHeight()
{
	return _imgHeight;
}

int CameraGeometry::getImgWidth()
{
	return _imgWidth;
}

/*
 * Construct rotation matrix which aligns camera coordinate system with
 * world coordinate system.
 *
 * */

CvMat* CameraGeometry::getRTcw(){
	CvMat* Rcw_x_Times_Rcw_y = cvCreateMat(3, 3, CV_32FC1);
	CvMat* Rcw = cvCreateMat(3, 3, CV_32FC1);
	cvMatMul(_matRcw_x, _matRcw_y, Rcw_x_Times_Rcw_y);
	cvMatMul(Rcw_x_Times_Rcw_y, _matRcw_z, Rcw);

	CvMat* RTcw = cvCreateMat(3, 4, CV_32FC1);
	//copy _matRcw to RTcw
	int row, col;
	for(row = 0; row < Rcw->rows; row++){
		for(col = 0; col < Rcw->cols; col++){
		  cvmSet(RTcw, row, col, cvmGet(Rcw, row, col));
		}
	}
	//copy Tcw to RTcw
	for(row = 0; row < _matTcw->rows; row++){
		cvmSet(RTcw, row, 3, cvmGet(_matTcw, row, 0));
	}
	//last row of RTcw = [0 0 0 1]
//	cvmSet(RTcw, 3, 0, 0);
//	cvmSet(RTcw, 3, 1, 0);
//	cvmSet(RTcw, 3, 2, 0);
//	cvmSet(RTcw, 3, 3, 1);

	return RTcw;
}

#define PI 3.14159265

float CameraGeometry::convertDegreesToRadians(float degrees){
	return (degrees * PI /180.0);
}


cv::Point2f CameraGeometry::imageToGroundPlane(cv::Point pt){
//
//        	| p_11 p_12 p_14 |
//   	H = | p_21 p_22 p_24 |
//        	| p_31 p_32 p_34 |
//
//


  CvMat *image_point = cvCreateMat(3, 1, CV_32FC1);
  cvmSet(image_point, 0, 0, pt.x);
  cvmSet(image_point, 1, 0, pt.y);
  cvmSet(image_point, 2, 0, 1);

  CvMat *ground_point = cvCreateMat(3, 1, CV_32FC1);
  cvMatMul(_matHInv, image_point, ground_point);

  return cv::Point2f(cvmGet(ground_point, 0, 0) / cvmGet(ground_point, 2, 0),
                     cvmGet(ground_point, 1, 0) / cvmGet(ground_point, 2, 0));
}

void CameraGeometry::imageToGroundPlane(std::vector<cv::Point> points,
                                std::vector<cv::Point2f>& ground_points) {
  for(int i = 0; i < static_cast<int>(points.size()); ++i) {
    cv::Point2f ground_point((*this).imageToGroundPlane(points[i]));
    ground_points.push_back(ground_point);
  }
  return;
}

cv::Point2f CameraGeometry::GroundPointToImagePoint(cv::Point2f point) {

  CvMat *ground_point = cvCreateMat(3, 1, CV_32FC1);
  cvmSet(ground_point, 0, 0, point.x);
  cvmSet(ground_point, 1, 0, point.y);
  cvmSet(ground_point, 2, 0, 1);

  CvMat *image_point = cvCreateMat(3, 1, CV_32FC1);
  cvMatMul(_matH, ground_point, image_point);

  return cv::Point2f(cvmGet(image_point, 1, 0) / cvmGet(image_point, 2, 0),
                     cvmGet(image_point, 0, 0) / cvmGet(image_point, 2, 0));

}

void CameraGeometry::ground_points_to_image_points(std::vector<cv::Point2f>& points, std::vector<cv::Point2f>& image_points) {
  for (int i = 0; i < static_cast<int>(points.size()); ++i) {
    image_points.push_back((*this).GroundPointToImagePoint(points[i]));
  }
  return;
}

int CameraGeometry::calculateWidthInPixels(float Y) {
    CvMat *P = this->getP();
    float W = 0.10; // Width of road 20cm ~ 0.2m
    float w = 0.0;  // Width of the roads in pixels

    CvMat* tmp = cvCreateMat(1,4, CV_32FC1);

    // Create P_1 (row 1 of matrix P)
    CvMat *P_1 = cvCreateMat(1, 4, CV_32FC1);
    cvGetRow(P, tmp, 0);  // row 0
    cvCopy(tmp, P_1, NULL);

    // Create P_3 (row 3 of matrix P)
    CvMat *P_3 = cvCreateMat(1, 4, CV_32FC1);
    cvGetRow(P, tmp, 2);  // row 2
    cvCopy(tmp, P_3, NULL);

    CvMat* X_1 = cvCreateMat(4, 1, CV_32FC1);
    CvMat* X_2 = cvCreateMat(4, 1, CV_32FC1);
    CvMat* P_1_times_X_1 = cvCreateMat(1, 1, CV_32FC1);
    CvMat* P_3_times_X_1 = cvCreateMat(1, 1, CV_32FC1);
    CvMat* P_1_times_X_2 = cvCreateMat(1, 1, CV_32FC1);
    CvMat* P_3_times_X_2 = cvCreateMat(1, 1, CV_32FC1);

  cvmSet(X_1,0,0,W);
  cvmSet(X_1,1,0,Y);
  cvmSet(X_1,2,0,0.0);
  cvmSet(X_1,3,0,1.0);

  cvmSet(X_2,0,0,0);
  cvmSet(X_2,1,0,Y);
  cvmSet(X_2,2,0,0);
  cvmSet(X_2,3,0,1);

  cvMatMul(P_1,X_1,P_1_times_X_1);
  cvMatMul(P_3,X_1,P_3_times_X_1);
  cvMatMul(P_1,X_2,P_1_times_X_2);
  cvMatMul(P_3,X_2,P_3_times_X_2);

  w = ((cvmGet(P_1_times_X_1,0,0) /
        cvmGet(P_3_times_X_1,0,0)
       )
      -
      (cvmGet(P_1_times_X_2,0,0) /
       cvmGet(P_3_times_X_2,0,0)
      ));


  return round(w);
}

float CameraGeometry::calculateY(int current_row) {
    float Y=0, P_34=0, P_24=0, P_32=0, P_22=0;
    /*
        Y = (v*P34 - P24) / (v*P32 - P22)
        Y = (p24 - y*p34) / (y*p32 - p22) <-- should be this???
    */
    P_34 = cvmGet(_matP, 2, 3);//(*this).getP().at<float>(2,3);
    P_24 = cvmGet(_matP, 1, 3);//(*this).getP().at<float>(1,3);
    P_32 = cvmGet(_matP, 2, 1);//(*this).getP().at<float>(2,1);
    P_22 = cvmGet(_matP, 1, 1);//(*this).getP().at<float>(1,1);

    // Y is negative??? Meaning?
//    Y = (current_row * P_34 - P_24) / (P_22 - current_row * P_32);
    Y = (P_24 - current_row * P_34) / (P_22 - current_row * P_32);

    return Y;
}
