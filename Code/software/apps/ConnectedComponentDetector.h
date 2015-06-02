/*
 * ConnectedComponentDetector.h
 *
*
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef CONNECTEDCOMPONENTDETECTOR_H_
#define CONNECTEDCOMPONENTDETECTOR_H_
#include <vector>
#include <iostream>
#include <map>
#include <cv.h>
#include <highgui.h>

#include "Timer.h"

class ConnectedComponentDetector {
public:
	ConnectedComponentDetector();
	virtual ~ConnectedComponentDetector();

	enum Algorithm {FOUR_POINT, NINE_POINT};
	ConnectedComponentDetector(IplImage* img, ConnectedComponentDetector::Algorithm algorithm);

	void setImage(IplImage *pImage);
	void setLabelElement(int x, int y, int value);
	int getLabelElement(int x, int y);
	cv::Mat getLabel();
	cv::Mat* getImg();
	std::map<std::string, int> getImgNeighbors(int x, int y);
	std::vector<int> getNeighborsLabel(int x, int y);
	void runPass1();
	void runPass2();
	Algorithm getAlgorithm();
	void getImagePlaneConnectedComponents(std::vector< std::vector<cv::Point> > &connectedComponentsOnImagePlane);

private:
	IplImage *_pImage;
	CvMat *_label;
	int *_aLabelParentMap;
	Algorithm _algorithm;
	Timer *_pTimer;
	void _union(int M, int X);
	int _find(int X);
	int getMinNeighbours(std::vector<int> neighbours);
	std::vector<int> FourPoint(int x, int y);
	std::vector<int> NinePoint(int x, int y);
};

#endif /* CONNECTEDCOMPONENTDETECTOR_H_ */
