/*
 * Util.h
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef UTIL_H_
#define UTIL_H_
#include "CameraGeometry.h"
#include <cv.h>
#include <highgui.h>
#include <limits>
using namespace std;
using namespace cv;

class Util {
public:
	Util();
	virtual ~Util();

	void image_to_ground_plane(
			const std::vector< std::vector<cv::Point> >& image_plane_sequences,
			std::vector< std::vector<cv::Point2f> >& ground_plane_sequences,
			CameraGeometry &camera);

	void imagePlaneToGroundPlane(
			std::vector< std::vector<cv::Point2f> >& image_plane_sequences,
			std::vector< std::vector<cv::Point2f> >& ground_plane_sequences,
			CvMat* matHInv);

	void imagePlaneToGroundPlane(
			const cv::Point2f &pointImage,
			cv::Point2f &pointGround,
			CvMat* matHInv);

	void groundPlaneToImagePlane(
			const cv::Point2f &pointGround,
			cv::Point2f &pointImage,
			CvMat* matH);

	void image_to_ground_plane(
			const std::vector< std::vector<cv::Point2f> >& image_plane_sequences,
			std::vector< std::vector<cv::Point2f> >& ground_plane_sequences,
			CameraGeometry* camera);

	void scaleGroundPlaneSequences(
	  const std::vector< std::vector <cv::Point2f> > &ground_plane_sequences,
	  std::vector< std::vector <cv::Point2f> >& scaled_ground_plane_sequences);
	void normalize_ground_plane_sequences(
	    std::vector< std::vector <cv::Point2f> >& ground_plane_sequences);
	void drawLines(
	  std::vector< std::vector <cv::Point2f> > ground_plane_sequences);

	cv::Point RoundPoint(cv::Point2f point_f);
	void ToGroundPlane(
	  std::vector< std::vector< std::vector<cv::Point2f> > > lane_marking,
	  std::vector< std::vector< std::vector<cv::Point2f> > >& lane_marking_ground,
	  CameraGeometry *camera);
	void MergeSequenceToPoints(std::vector< std::vector<cv::Point2f> > sequence,
							 std::vector<cv::Point2f>& points);
	void MergeLaneMarkingToPoints(
	  std::vector< std::vector <std::vector<cv::Point2f> > > lane_marking,
	  std::vector<cv::Point2f>& points);
	void MostRightPointToOrigin(std::vector< std::vector<cv::Point2f> > sequence,
							cv::Point2f& point, int& sequence_number);
	void MostLeftPointToOrigin(std::vector< std::vector<cv::Point2f> > sequence,
							cv::Point2f& point, int& sequence_number);
	void ClosestPointToOrigin(std::vector< std::vector<cv::Point2f> > sequence,
							cv::Point2f& point, int& sequence_number);
	cv::Point2f PointWithMaxY(std::vector<cv::Point2f> points);
	void FurthestPointFromOrigin(
	  std::vector< std::vector<cv::Point2f> > sequence,
	  cv::Point2f& point, int& sequence_number);
	void printMat(CvMat* mat);
	void graph(double dis_right, double dis_left);

private:
	std::deque<double> rightQueue;
	std::deque<double> leftQueue;

};

#endif /* UTIL_H_ */
