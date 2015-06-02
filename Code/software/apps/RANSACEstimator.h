/*
 * RANSAC.h
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef RANSAC_H_
#define RANSAC_H_
#include <cv.h>
#include <cmath>
#include <ctime>
#include <limits.h>
#include "Line.h"
#include "Random.h"
#include <ext/algorithm>

class RANSACEstimator {
public:
	RANSACEstimator();
	virtual ~RANSACEstimator();

	int fitLine(std::vector<cv::Point2f> vIn, std::vector<cv::Point2f>& vInliers, double dThreshold=1.0);

	double perpendicularDistance(
			const cv::Point2f a,
			const cv::Point2f b,
			const cv::Point2f c,
			cv::Point2f &pointNearest);

	bool isConsistent(cv::Point2f v1, cv::Point2f v2, cv::Point2f p1, cv::Point2f p2);

private:
	void randomShuffle(std::vector<cv::Point2f>& points, int count);
	void RandomShuffle(std::vector< std::vector<cv::Point2f> > sequences,
			std::vector< std::vector<cv::Point2f> >& shuffled_sequences);
	void GetMaybeInliers(std::vector< std::vector<cv::Point2f> > sequences,
					   std::vector< std::vector<cv::Point2f> >& two_sequence);
	double lineLineSimilarity(cv::Point2f v1, cv::Point2f p1, cv::Point2f v2, cv::Point2f p2);
	void RemoveSequenceWithLessThanTwoPoints(std::vector< std::vector<cv::Point2f> >& sequences);
	void GetOtherPoints(std::vector< std::vector<cv::Point2f> > sequences,
					  std::vector< std::vector<cv::Point2f> > inliers,
					  std::vector< std::vector<cv::Point2f> >& other_points);
	bool IsEqual(std::vector<cv::Point2f> points1,
			std::vector<cv::Point2f> points2);
	void MergeTwoSequences(std::vector<cv::Point2f> seq1,
			std::vector<cv::Point2f> seq2, std::vector<cv::Point2f>& out);
	void ClearAndCopy(std::vector< std::vector<cv::Point2f> > to,
					std::vector< std::vector<cv::Point2f> >& from);
};

#endif /* RANSAC_H_ */
