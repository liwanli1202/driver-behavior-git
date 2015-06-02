/*
 * LaneMarkingDetectorTest.h
 *
 *  Created on: Dec 9, 2014
 *      Author: mdailey
 */

#ifndef LANEMARKINGFILTERTEST_H_
#define LANEMARKINGFILTERTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "LaneMarkingFilter.h"

class LaneMarkingFilterTest : public CppUnit::TestFixture
{
	  CPPUNIT_TEST_SUITE( LaneMarkingFilterTest );
	  CPPUNIT_TEST( testConstructor );
	  CPPUNIT_TEST( testUpdate );
	  CPPUNIT_TEST( testUpdateTooWide );
	  CPPUNIT_TEST( testSequence1 );
	  CPPUNIT_TEST( testSequence2 );
	  CPPUNIT_TEST( testSequence3 );
	  CPPUNIT_TEST( testLaneChange );
	  CPPUNIT_TEST( testNoLaneChange );
	  CPPUNIT_TEST_SUITE_END();

public:
	  void setUp();
	  void tearDown();

	  void testConstructor();
	  void testUpdate();
	  void testUpdateTooWide();
	  void testSequence1();
	  void testSequence2();
	  void testSequence3();
	  void testLaneChange();
	  void testNoLaneChange();

private:

	  LaneMarkingFilter *_pLaneMarkingFilter;
};

#endif /* LANEMARKINGFILTERTEST_H_ */
