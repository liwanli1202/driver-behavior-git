/*
 * Lane.h
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef LANE_H_
#define LANE_H_
#include "Line.h"
#include "Util.h"
class Lane {
public:
	Lane();
	virtual ~Lane();

	void DrawLanes(
		std::vector< std::vector< std::vector<cv::Point2f> > > lane_marking,
	    std::string window_name);
	void DrawLanes(
	    std::vector< std::vector< std::vector<cv::Point2f> > > lane_marking,
	    cv::Mat img);
};

#endif /* LANE_H_ */
