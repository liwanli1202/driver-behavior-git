/*
 * Lane.cpp
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include "Lane.h"

Lane::Lane() {
	// TODO Auto-generated constructor stub

}

Lane::~Lane() {
	// TODO Auto-generated destructor stub
}

void Lane::DrawLanes( std::vector< std::vector< std::vector<cv::Point2f> > > lane_marking, std::string window_name)
{
	cv::Mat img;
	img = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	Line line;
	cv::Point2f min_point, max_point;
	Util util;
	for (int i = 0; i < static_cast<int>(lane_marking.size()); ++i)
	{
		for (int j = 0; j < static_cast<int>(lane_marking[i].size()); ++j)
		{
			line.fit(lane_marking[i][j]);
			Line::ExtremePoints extreme_points = line.get_extreme_points(lane_marking[i][j]);
			boost::tie(min_point, max_point) = extreme_points;
			cv::line(img, util.RoundPoint(min_point), util.RoundPoint(max_point), cv::Scalar(0, 0,255), 2, CV_AA, 0);
		}
	}
	std::vector<int> params;
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(3); //3 is default
	//cv::imwrite("ground.png", img, params);

  //cv::imshow(window_name, img);
}

void Lane::DrawLanes(
    std::vector< std::vector< std::vector<cv::Point2f> > > lane_marking,
    cv::Mat img) {
  Line line;
  cv::Point2f min_point, max_point;
  Util util;
  for (int i = 0; i < static_cast<int>(lane_marking.size()); ++i) {
    for (int j = 0; j < static_cast<int>(lane_marking[i].size()); ++j) {
      line.fit(lane_marking[i][j]);
      Line::ExtremePoints extreme_points =
          line.get_extreme_points(lane_marking[i][j]);
      boost::tie(min_point, max_point) = extreme_points;
      cv::line(img, util.RoundPoint(min_point), util.RoundPoint(max_point),
               cv::Scalar(0, 0,255), 2, CV_AA, 0);
    }
  }
}
