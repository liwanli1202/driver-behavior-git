/*
 * Line.h
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef LINE_H_
#define LINE_H_
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <boost/tuple/tuple.hpp>

class Line {

public:
	Line();
	virtual ~Line();
	typedef boost::tuple<cv::Point, cv::Point> ExtremePoints;
	typedef boost::tuple<cv::Point2f, cv::Point2f> ExtremePointsF;
	void fit(std::vector<cv::Point2f> points);
	double getSlope();
	void setDir(double V_x, double V_y);
	void setPoint(double P_x, double P_y);
	cv::Point2f getDir();
	cv::Point2f getPoint();
	ExtremePoints get_extreme_points(std::vector<cv::Point2f> points);
	ExtremePointsF get_extreme_pointsF(std::vector<cv::Point2f> points);
	void DrawLines(std::vector< std::vector<cv::Point2f> > points);

private:
	cv::Point2f pointDir;
	cv::Point2f point;
	std::vector<double> vPointsProjected;
	void calculate_projected_points(std::vector<cv::Point2f> points);
	std::vector<double> get_projected_points();
	double get_projected_PxPy();
	cv::Point2f get_max_point();
	cv::Point2f get_min_point();
	cv::Point RoundPoint(cv::Point2f point_f);
};

#endif /* LINE_H_ */
