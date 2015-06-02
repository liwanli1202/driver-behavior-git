/*
 * RANSAC.cpp
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include <cv.h>
#include "RANSACEstimator.h"

RANSACEstimator::RANSACEstimator() {
}

RANSACEstimator::~RANSACEstimator() {
}


int RANSACEstimator::fitLine(
		std::vector<cv::Point2f> vIn,
		std::vector<cv::Point2f> &vInliers,
		double dThreshold)
{
	int iMaxConsensusSetSize = 0;
	int N = 100;
	int cIter = 0;
	double e; // ratio of inliers in a data set

	while (cIter < N)
	{
		std::vector<cv::Point2f> pointsConsensus;
		std::vector<cv::Point2f> pointsSample;

		// Shuffle the first two elements of the array

		this->randomShuffle(vIn, 2);

		// Instantiate the model from the first two points

		pointsSample.push_back(vIn[0]);
		pointsSample.push_back(vIn[1]);
		pointsConsensus.push_back(vIn[0]);
		pointsConsensus.push_back(vIn[1]);

		// Count inliers

		for(unsigned int i = 2; i < vIn.size(); i++)
		{
			cv::Point2f pointNearest;
			double dist = this->perpendicularDistance(pointsSample[0], pointsSample[1], vIn[i], pointNearest);
			if (fabs(dist) < dThreshold)
			{
				pointsConsensus.push_back(vIn[i]);
			}
		}

		// Save model if best so far

		if ((int)pointsConsensus.size() > iMaxConsensusSetSize)
		{
			iMaxConsensusSetSize = pointsConsensus.size();
			vInliers.clear();
			vInliers.swap(pointsConsensus);
			e = (double)iMaxConsensusSetSize / vIn.size();

			// Prob of inlier is e
			// Prob of 2 inliers is e^2
			// Prob of bad sample is 1-e^2
			// Prob of N bad samples is (1-e*e)^N
			// Prob of at least one good sample in N samples is 1 - (1-e^2)^N
			// Want N such that 1 - (1-e^2)^N > 0.99
			// (1-e^2)^N < 0.01
			// N log(1-e^2) < log(0.01)
			// N < log(0.01) / log(1-e^2)

#define DESIRED_PROB_FINDING_MODEL 0.999
#define MIN_ITERS 10

			int newN = (int)ceil(log(1.0 - DESIRED_PROB_FINDING_MODEL) / log(1.0 - e * e));
			if (newN < N) N = newN;
			if (N < MIN_ITERS) N = MIN_ITERS;
		}
		cIter++;
	}
	return cIter;
}


void RANSACEstimator::randomShuffle(std::vector<cv::Point2f>& points, int count) {
	// Move count random points to the beginning of the list
	for (int i = 0; i < count; i++)
	{
		int index = rand() % points.size();
		cv::Point2f point = points[i];
		points[i] = points[index];
		points[index] = point;
	}
  return;
}


double RANSACEstimator::perpendicularDistance(
		const cv::Point2f a,
		const cv::Point2f b,
		const cv::Point2f c,
		cv::Point2f &pointNearest)
{

	// Get unit direction vector

	cv::Point2f pointDir(b - a);
	double mag = sqrt(pointDir.x * pointDir.x + pointDir.y * pointDir.y);
	pointDir.x /= mag;
	pointDir.y /= mag;
	cv::Point2f pointNormal(-pointDir.y, pointDir.x);
	cv::Point2f pointRelative(c - a);
	double dotProdNormal = pointRelative.x * pointNormal.x + pointRelative.y * pointNormal.y;
	double dotProdLine = pointRelative.x * pointDir.x + pointRelative.y * pointDir.y;
	pointNearest = cv::Point2f(a + pointDir * dotProdLine);
	return fabs(dotProdNormal);
}


void RANSACEstimator::ClearAndCopy(std::vector< std::vector<cv::Point2f> > to,
                          std::vector< std::vector<cv::Point2f> >& from) {
  from.clear();
  for (int i = 0; i < static_cast<int>(to.size()); ++i) {
    from.push_back(to[i]);
  }
}
void RANSACEstimator::GetOtherPoints(
    std::vector< std::vector<cv::Point2f> > sequences,
    std::vector< std::vector<cv::Point2f> > inliers,
    std::vector< std::vector<cv::Point2f> >& other_points) {
  other_points.resize(0);
  for (int i = 0; i < static_cast<int>(sequences.size()); ++i) {
    int number_of_not_equal = 0;
    for (int j = 0; j < static_cast<int>(inliers.size()); ++j) {
      if ((*this).IsEqual(sequences[i], inliers[j]) == false) {
        number_of_not_equal = number_of_not_equal + 1;
      }
    }
    if (number_of_not_equal == static_cast<int>(inliers.size())) {
      other_points.push_back(sequences[i]);
    }
  }
  return;
}

bool RANSACEstimator::IsEqual(std::vector<cv::Point2f> points1,
                     std::vector<cv::Point2f> points2) {
  if (static_cast<int>(points1.size()) != static_cast<int>(points2.size())) {
    return false;
  }
  for (int i = 0; i < static_cast<int>(points1.size()); ++i) {
    if ((points1[i].x != points2[i].x) || (points1[i].y != points2[i].y)) {
      return false;
    }
  }
  return true;
}

void RANSACEstimator::MergeTwoSequences(std::vector<cv::Point2f> seq1,
                               std::vector<cv::Point2f> seq2,
                               std::vector<cv::Point2f>& out) {
  out.resize(0);
  for (int i = 0; i < static_cast<int>(seq1.size()); ++i) {
    out.push_back(seq1[i]);
  }
  for (int i = 0; i < static_cast<int>(seq2.size()); ++i) {
    out.push_back(seq2[i]);
  }
}

void RANSACEstimator::GetMaybeInliers(
		std::vector< std::vector<cv::Point2f> > sequences,
		std::vector< std::vector<cv::Point2f> >& two_sequence)
{
	two_sequence.clear();
	Line line1, line2;

	for (unsigned int i = 0; i < sequences.size(); i++)
	{
		line1.fit(sequences[i]);
		cv::Point2f v1 = line1.getDir();
		cv::Point2f p1 = line1.getPoint();
		for (unsigned int j = 0; j < sequences.size(); j++)
		{
			if (i != j)
			{
				line2.fit(sequences[j]);
				cv::Point2f v2 = line2.getDir();
				cv::Point2f p2 = line2.getPoint();
				if (this->isConsistent(v1, v2, p1, p2))
				{
					two_sequence.push_back(sequences[i]);
					two_sequence.push_back(sequences[j]);
					return;
				}
			}
		}
	}
}

bool RANSACEstimator::isConsistent(cv::Point2f v1, cv::Point2f v2, cv::Point2f p1, cv::Point2f p2) {
	if (fabs(v1.dot(v2)) > 0.95 &&
			this->lineLineSimilarity(v1, p1, v2, p2) > 0.985)
	{
		return true;
	} else {
		return false;
	}
}


double RANSACEstimator::lineLineSimilarity(
		cv::Point2f v1_, cv::Point2f p1_,
		cv::Point2f v2_, cv::Point2f p2_)
{
	cv::Mat v1 = (cv::Mat_<double>(2, 1) << v1_.x, v1_.y);
	cv::Mat v2 = (cv::Mat_<double>(2, 1) << v2_.x, v2_.y);
	cv::Mat p1 = (cv::Mat_<double>(2, 1) << p1_.x, p1_.y);
	cv::Mat p2 = (cv::Mat_<double>(2, 1) << p2_.x, p2_.y);
	cv::Mat pDiff = (p2 - p1) / cv::norm(p2 - p1);
	return MIN(fabs(v1.dot(pDiff)), fabs(v2.dot(pDiff)));
}

void RANSACEstimator::RemoveSequenceWithLessThanTwoPoints(
    std::vector< std::vector<cv::Point2f> >& sequences) {
  std::vector< std::vector<cv::Point2f> > temp(sequences);
  sequences.resize(0);
  for (int i = 0; i < static_cast<int>(temp.size()); ++i) {
    if (static_cast<int>(temp[i].size()) > 1) {
      sequences.push_back(temp[i]);
    }
  }
}
