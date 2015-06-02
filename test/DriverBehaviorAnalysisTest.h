#ifndef DRIVERBEHAVIORANALYSISTEST_H_
#define DRIVERBEHAVIORANALYSISTEST_H_


#include <cppunit/extensions/HelperMacros.h>

#include "DriverBehaviorAnalysis.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class DriverBehaviorAnalysisTest : public CppUnit::TestFixture
{
          CPPUNIT_TEST_SUITE( DriverBehaviorAnalysisTest );
	  CPPUNIT_TEST( testMean );
          CPPUNIT_TEST( testVariance ); 	
//	  CPPUNIT_TEST( testDetectLaneChange ); 
	  CPPUNIT_TEST( testGetLaneChangeSpeed ); 
//	  CPPUNIT_TEST( testSetData ); 	
          CPPUNIT_TEST_SUITE_END();

public:
          void setUp();
          void tearDown();

	  void testMean(); 
	  void testVariance(); 	
//	  void testDetectLaneChange(); 
	  void testGetLaneChangeSpeed();
//	  void testSetData();  

private:
	  DriverBehaviorAnalysis *_pDriverBehaviorAnalysis; 
};

#endif /* DRIVERBEHAVIORANALYSISTEST_H_  */                                          
