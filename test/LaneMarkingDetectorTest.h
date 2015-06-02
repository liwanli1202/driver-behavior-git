/*
 * LaneMarkingDetectorTest.h
 *
 *  Created on: Dec 9, 2014
 *      Author: mdailey
 */

#ifndef LANEMARKINGDETECTORTEST_H_
#define LANEMARKINGDETECTORTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "LaneMarkingDetector.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class LaneMarkingDetectorTest : public CppUnit::TestFixture
{
	  CPPUNIT_TEST_SUITE( LaneMarkingDetectorTest );
	  CPPUNIT_TEST( testFindLaneMarking );
	  CPPUNIT_TEST( testGroupLaneMarkings );
	  CPPUNIT_TEST( testConsistentLaneMarkings );
	  CPPUNIT_TEST( testConsistentTripleLaneMarkings );
	  CPPUNIT_TEST( testFindLaneMarkingNoise1 );
	  CPPUNIT_TEST_SUITE_END();

public:
	  void setUp();
	  void tearDown();

	  void testFindLaneMarking();
	  void testFindLaneMarkingNoise1();
	  void testGroupLaneMarkings();
	  void testConsistentLaneMarkings();
	  void testConsistentTripleLaneMarkings();

private:
	  LaneMarkingDetector *_pLaneMarkingDetector;
	  double _distLeftGroundTruth;
	  double _distRightGroundTruth;
	  Util *_pUtil;
	  CvMat *_pMatHinv;
	  CvMat *_pMatH;
	  double _dExpectedLaneWidth;

	  void makeMarkingComponents(
			  vector<vector<cv::Point2f> > pointComponents,
			  vector<LaneMarkingDetector::MarkingComponent> &markingComponents);

};

#endif /* LANEMARKINGDETECTORTEST_H_ */
