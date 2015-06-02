/*
 * Line.cpp
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include "Line.h"


Line::Line() {
}


void Line::fit(std::vector<cv::Point2f> points)
{
	this->vPointsProjected.resize(0);
	cv::Vec4f line;
	cv::fitLine(cv::Mat(points), line, CV_DIST_L2, 0.001, 0.001, 0.01);
	this->setDir(line[0], line[1]); //V_x, V_y
	this->setPoint(line[2], line[3]); //P_x, P_y
	return;
}


double Line::getSlope()
{
	return (pointDir.y / pointDir.x);
}


void Line::setDir(double V_x, double V_y) {
  this->pointDir.x = V_x;
  this->pointDir.y = V_y;
}


void Line::setPoint(double P_x, double P_y) {
  this->point.x = P_x;
  this->point.y = P_y;
}


cv::Point2f Line::getDir() {
  return pointDir;
}


cv::Point2f Line::getPoint() {
  return point;
}


void Line::calculate_projected_points(std::vector<cv::Point2f> points) {
  for (int i = 0; i < static_cast<int>(points.size()); ++i) {
    (*this).vPointsProjected.push_back(points[i].dot((*this).pointDir));
  }
}

std::vector<double> Line::get_projected_points() {
  return (*this).vPointsProjected;
}

double Line::get_projected_PxPy() {
  return (*this).point.dot((*this).pointDir);
}

cv::Point2f Line::get_max_point() {
  std::sort(vPointsProjected.begin(), vPointsProjected.end());
  double projected_max = vPointsProjected[vPointsProjected.size() - 1];
  double distance_max = projected_max - (*this).get_projected_PxPy();
  cv::Point2f point =  (*this).getPoint() +
      (distance_max * (*this).getDir());
  return point;
}

cv::Point2f Line::get_min_point() {
  std::sort(vPointsProjected.begin(), vPointsProjected.end());
  double projected_min = vPointsProjected[0];
  double distance_min = projected_min - (*this).get_projected_PxPy();
  cv::Point2f point =  (*this).getPoint() +
      (distance_min * (*this).getDir());
  return point;
}

Line::ExtremePoints
Line::get_extreme_points(std::vector<cv::Point2f> points)
{
	this->calculate_projected_points(points);
	//Round off the points
	cv::Point2f pointMin = this->get_min_point();
	cv::Point2f pointMax = this->get_max_point();
	cv::Point min_point(cvRound(pointMin.x), cvRound(pointMin.y));
	cv::Point max_point(cvRound(pointMax.x), cvRound(pointMax.y));
	return boost::make_tuple(min_point, max_point);
}

Line::ExtremePointsF
Line::get_extreme_pointsF(std::vector<cv::Point2f> points) {
	this->calculate_projected_points(points);
	return boost::make_tuple(this->get_min_point(), this->get_max_point());
}

void Line::DrawLines(std::vector< std::vector<cv::Point2f> > points) {
  cv::Mat img;
  img = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
  cv::namedWindow("DrawLine", CV_WINDOW_AUTOSIZE);
  cv::Point2f min_point, max_point;
  Line obj;
  for (int i = 0; i < static_cast<int>(points.size()); ++i) {
    obj.fit(points[i]);
    Line::ExtremePoints extreme_points = obj.get_extreme_points(points[i]);
    boost::tie(min_point, max_point) = extreme_points;
    cv::line(img, (*this).RoundPoint(min_point), (*this).RoundPoint(max_point),
             cv::Scalar(0, 0, 255), 2, CV_AA, 0);
  }
  cv::imshow("DrawLine", img);
}

cv::Point Line::RoundPoint(cv::Point2f point_f) {
  return cv::Point(cvRound(point_f.x), cvRound(point_f.y) );
}

Line::~Line() {
  (*this).vPointsProjected.resize(0);
}

