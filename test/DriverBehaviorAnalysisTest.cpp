#include <cv.h>
#include <highgui.h>
using namespace cv;

#include "DriverBehaviorAnalysisTest.h"
#include "DriverBehaviorAnalysis.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DriverBehaviorAnalysisTest );


void
DriverBehaviorAnalysisTest::setUp()
{
        _pDriverBehaviorAnalysis = new DriverBehaviorAnalysis(); 
}


void
DriverBehaviorAnalysisTest::tearDown()
{
        delete _pDriverBehaviorAnalysis;
}

void
DriverBehaviorAnalysisTest::testMean()
{
	double data[40] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 ,17.0, 18.0, 19.0, 20.0,
			   21.0, 22.0, 23.0, 24.0 , 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0,
			   38.0, 39.0, 40.0 };
	double mean = _pDriverBehaviorAnalysis->getMean(data);
	CPPUNIT_ASSERT(mean == 20.5); 
}

void
DriverBehaviorAnalysisTest::testVariance()
{
	double data[40] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 ,17.0, 18.0, 19.0, 20.0,
                           21.0, 22.0, 23.0, 24.0 , 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0,
                           38.0, 39.0, 40.0 };
	double variance = _pDriverBehaviorAnalysis->getVariance(data); 
	CPPUNIT_ASSERT(variance = 133.25); 

}
/*
void 
DriverBehaviorAnalysisTest::testDetectLaneChange()
{
	double position = 0.009; 
	double left = 2.1; 
	double right = 3.1; 

	bool isLaneChange = _pDriverBehaviorAnalysis->detectLaneChange(position, left, right); 
	CPPUNIT_ASSERT(isLaneChange == false);

}
*/

void 
DriverBehaviorAnalysisTest::testGetLaneChangeSpeed()
{
	bool isLaneChange = false; 
	double speed = _pDriverBehaviorAnalysis->getLaneChangeSpeed(isLaneChange); 

	CPPUNIT_ASSERT(speed == 0); 


}

/*
void
DriverBehaviorAnalysisTest::testSetData()
{
	int frameNo = 1000; 
	double lateralDistance = 0.008;
	bool isLaneChange = false;
	double laneChangeSpeed = 30;
	string output = _pDriverBehaviorAnalysis->setData(frameNo,lateralDistance ,laneChangeSpeed);

	CPPUNIT_ASSERT(output == "f,1000,0.008,30");



}
*/
