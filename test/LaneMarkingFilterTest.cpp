/*
 * LaneMarkingFilterTest.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: mdailey
 */

#include "LaneMarkingFilterTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( LaneMarkingFilterTest );

void
LaneMarkingFilterTest::setUp()
{
	cv::FileStorage fileStorageParamsFile;
	fileStorageParamsFile.open("testFiles/simImage.params", cv::FileStorage::READ);
	double dExpectedLaneWidth = fileStorageParamsFile["expectedLaneWidth"];

	_pLaneMarkingFilter = new LaneMarkingFilter(dExpectedLaneWidth);

	//_pLaneMarkingFilter->setH();
}


void
LaneMarkingFilterTest::tearDown()
{
	delete _pLaneMarkingFilter;
}


void
LaneMarkingFilterTest::testConstructor()
{
	double dPosition;
	double dYaw;
	double dLaneWidth;

	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dPosition, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);

	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dPosition, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.25, dLaneWidth, 1e-6);
}


void
LaneMarkingFilterTest::testUpdate()
{
	LaneMarkingDetector::Boundary boundaryLeft, boundaryRight;

	// No boundaries found: no change in state

	boundaryLeft.likelihood = 0;
	boundaryLeft.dPerp = 2000;
	boundaryLeft.dYaw = M_PI/2;

	boundaryRight.likelihood = 0;
	boundaryRight.dPerp = 1000;
	boundaryRight.dYaw = M_PI/2;

	_pLaneMarkingFilter->resetState();
	_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);

	double dPosition;
	double dYaw;
	double dLaneWidth;

	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dPosition, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.25, dLaneWidth, 1e-6);

	// Left boundary only: if nearer, should pull position toward left, decrease lane width expectation

	boundaryLeft.likelihood = 1;
	boundaryLeft.dPerp = 1;

	_pLaneMarkingFilter->resetState();
	_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);

	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.125, dPosition, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.25, dLaneWidth, 1e-6);

	// Right boundary only: if nearer than expected, should pull position toward right, decrease lane width expectation

	boundaryLeft.likelihood = 0;
	boundaryRight.likelihood = 1;
	boundaryRight.dPerp = 1;

	_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);
	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.05, dPosition, 0.05);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.1, dLaneWidth, 0.1);

	// Both boundaries: if left is closer and right is further than expected, shift left and increase width expectation.
	// With robust estimation, this counter-intuitively gives us a smaller estimate of lane width (1.9 m) because the
	// effect of the outlier measurements is discounted.

	boundaryLeft.likelihood = 1;
	boundaryLeft.dPerp = 0.4;
	boundaryRight.likelihood = 1;
	boundaryRight.dPerp = 2.0;

	_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);
	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.4, dPosition, 0.1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.9, dLaneWidth, 0.1);

	// Both boundaries: if wider than expected and left boundary is further than expected, shift right and widen width

	boundaryLeft.likelihood = 1;
	boundaryLeft.dPerp = 1.2;
	boundaryRight.likelihood = 1;
	boundaryRight.dPerp = 1.4;

	_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);
	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.2, dPosition, 0.1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.4, dLaneWidth, 0.2);
}


void
LaneMarkingFilterTest::testUpdateTooWide()
{
	LaneMarkingDetector::Boundary boundaryLeft, boundaryRight;

	boundaryLeft.likelihood = 1;
	boundaryLeft.dPerp = 1.625;
	boundaryLeft.dYaw = M_PI/2;

	boundaryRight.likelihood = 1;
	boundaryRight.dPerp = 0.625;
	boundaryRight.dYaw = M_PI/2;

	_pLaneMarkingFilter->resetState();
	_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);

	double dPosition;
	double dYaw;
	double dLaneWidth;

	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, dPosition, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.25, dLaneWidth, 1e-6);

	// If we have two new observations too wide apart, we should only pay attention to the highest likelihood,
	// moving us to the right and increasing the width estimate a little

	boundaryLeft.dPerp = 1.925;
	boundaryRight.dPerp = 0.925;
	boundaryRight.likelihood = 0.5;

	_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);
	_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.7, dPosition, 0.1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dYaw, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.35, dLaneWidth, 0.1);
}


void
LaneMarkingFilterTest::testSequence1()
{
	// Compare to Octave simulation

	double dPerpLeft[] = { 1.16516, 1.17669, 1.16348, 1.14929, 1.17489 };
	double dPerpRight[] = { 1.0994, 1.07826, 1.06513, 1.05781, 1.03316 };
	double dYawLeft[] = { 1.5708, 1.5708, 1.5708, 1.5708, 1.5708 };
	double dYawRight[] = { 1.5708, 1.5708, 1.5708, 1.5708, 1.5708 };
	double dLikelihoodLeft[] = { 0.434659, 0.395728, 0.41041, 0.432147, 0.393941 };
	double dLikelihoodRight[] = { 0.0556363, 0.198694, 0.258923, 0.294481, 0.381107 };
	double dPositionFromOctave[] = { 0.032880, 0.047400, 0.048236, 0.047436, 0.053133 };

	_pLaneMarkingFilter->resetState();
	for (int i = 0; i < 5; i++)
	{
		LaneMarkingDetector::Boundary boundaryLeft, boundaryRight;
		boundaryLeft.dPerp = dPerpLeft[i];
		boundaryLeft.dYaw = dYawLeft[i];
		boundaryLeft.likelihood = dLikelihoodLeft[i];
		boundaryRight.dPerp = dPerpRight[i];
		boundaryRight.dYaw = dYawRight[i];
		boundaryRight.likelihood = dLikelihoodRight[i];

		_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);

		double dPosition;
		double dYaw;
		double dLaneWidth;

		_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(dPositionFromOctave[i], dPosition, 1e-6);
		//cerr << "Position " << dPositionFromOctave[i] << " from Octave, " << dPosition << " from C++" << endl;
	}
}

void
LaneMarkingFilterTest::testLaneChange()
{
	double dPerpLeft[] =  { 0.95, 0.85, 0.75, 0.65, 0.55, 0.45, 0.35, 0.25, 0.15, 0.05, 2.2,  2.1,  2.0,  1.9,  1.8,  1.7,  1.6 };
	double dPerpRight[] = { 1.3,  1.4,  1.5,  1.6,  1.7,  1.8,  1.9,  2.0,  2.1,  2.2,  0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65 };
	double dYawLeft[] = { 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708 };
	double dYawRight[] = { 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708 };
	double dLikelihoodLeft[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	double dLikelihoodRight[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	double dPositionFromOctave[] = { -0.175000, -0.263890, -0.316202, -0.367093, -0.417647, -0.468117, -0.518604,
			-0.569164, -0.619834, -0.670642, -0.616183, -0.563152, -0.511635, -0.461731, 0.675000, 0.586110, 0.533798 };

	for (unsigned int i = 0; i < sizeof(dPerpLeft)/sizeof(double); i++)
	{
		LaneMarkingDetector::Boundary boundaryLeft, boundaryRight;
		boundaryLeft.dPerp = dPerpLeft[i];
		boundaryLeft.dYaw = dYawLeft[i];
		boundaryLeft.likelihood = dLikelihoodLeft[i];
		boundaryRight.dPerp = dPerpRight[i];
		boundaryRight.dYaw = dYawRight[i];
		boundaryRight.likelihood = dLikelihoodRight[i];

		_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);

		double dPosition;
		double dYaw;
		double dLaneWidth;

		_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);
		//cerr << "Position " << dPositionFromOctave[i] << " from Octave, " << dPosition << " from C++" << endl;
		CPPUNIT_ASSERT_DOUBLES_EQUAL(dPositionFromOctave[i], dPosition, 1e-6);
	}
}


void
LaneMarkingFilterTest::testNoLaneChange()
{
	// Construct a sequence in which noise makes the further lane marking the closer,
	// forcing the nearest boundary estimate to cross the 0 threshold though vehicle position hasn't yet.

	double dPerpLeft[] =  { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  1.9,  1.9,  1.9,  1.9 };
	double dPerpRight[] = { 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.35, 0.35, 0.35, 0.35  };
	double dYawLeft[] = { 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708 };
	double dYawRight[] = { 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708, 1.5708 };
	double dLikelihoodLeft[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	double dLikelihoodRight[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };
	double dPositionFromOctave[] = { 0.875, 0.875, 0.875, 0.875, 0.875, 0.875, 0.875, 0.875, 0.875,
			0.875, 0.875, 0.875, 0.875, 0.871397, 0.868508, 0.866176, 0.864283 };

	for (unsigned int i = 0; i < sizeof(dPerpLeft)/sizeof(double); i++)
	{
		LaneMarkingDetector::Boundary boundaryLeft, boundaryRight;
		boundaryLeft.dPerp = dPerpLeft[i];
		boundaryLeft.dYaw = dYawLeft[i];
		boundaryLeft.likelihood = dLikelihoodLeft[i];
		boundaryRight.dPerp = dPerpRight[i];
		boundaryRight.dYaw = dYawRight[i];
		boundaryRight.likelihood = dLikelihoodRight[i];

		_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);

		double dPosition;
		double dYaw;
		double dLaneWidth;

		_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);
		//cerr << i+1 << ": position " << dPositionFromOctave[i] << " from Octave, " << dPosition << " from C++" << endl;
		CPPUNIT_ASSERT_DOUBLES_EQUAL(dPositionFromOctave[i], dPosition, 1e-6);
	}
}


void
LaneMarkingFilterTest::testSequence2()
{
	// Compare to Octave simulation

	double dPerpLeft[] =  { 1.15848, 1.17722, 1.15406, 1.15496, 1.20319, 1.20923, 1.20013, 1.16059, 1.15875, 1.14201 };
	double dPerpRight[] = { 1.0994,  1.08092, 1.06513, 1.05781, 1.03316, 1.01151, 1.00295, 2.60277, 1.12939, 1.12942 };
	double dYawLeft[] =   { 1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708 };
	double dYawRight[] =  { 1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708,  1.5708 };
	double dLikelihoodLeft[] = { 0.434801, 0.395702, 0.434781, 0.426551, 0.313579, 0.413566, 0.330578, 0.433766, 0.407736, 0.435114 };
	double dLikelihoodRight[] = { 0.0556363, 0.198412, 0.258923, 0.294481, 0.381107, 0.404906, 0.404145, 0.000778303, 0.00546652, 0.016683 };
	double dPositionFromOctave[] = { 0.029540, 0.046082, 0.045321, 0.046364, 0.055762, 0.064214, 0.069869, 0.068313, 0.060800, 0.054199 };

	_pLaneMarkingFilter->resetState();
	for (unsigned int i = 0; i < sizeof(dPerpLeft)/sizeof(double); i++)
	{
		LaneMarkingDetector::Boundary boundaryLeft, boundaryRight;
		boundaryLeft.dPerp = dPerpLeft[i];
		boundaryLeft.dYaw = dYawLeft[i];
		boundaryLeft.likelihood = dLikelihoodLeft[i];
		boundaryRight.dPerp = dPerpRight[i];
		boundaryRight.dYaw = dYawRight[i];
		boundaryRight.likelihood = dLikelihoodRight[i];

		_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);

		double dPosition;
		double dYaw;
		double dLaneWidth;

		_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(dPositionFromOctave[i], dPosition, 1e-6);
		//cerr << "Position " << dPositionFromOctave[i] << " from Octave, " << dPosition << " from C++" << endl;
	}
}


#include "testFiles/kalman_seq3.cpp"

void
LaneMarkingFilterTest::testSequence3()
{
	_pLaneMarkingFilter->resetState();
	for (unsigned int i = 0; i < sizeof(dPerpLeftSeq3)/sizeof(double); i++)
	{
		LaneMarkingDetector::Boundary boundaryLeft, boundaryRight;
		boundaryLeft.dPerp = dPerpLeftSeq3[i];
		boundaryLeft.dYaw = dYawLeftSeq3[i];
		boundaryLeft.likelihood = dLikelihoodLeftSeq3[i];
		boundaryRight.dPerp = dPerpRightSeq3[i];
		boundaryRight.dYaw = dYawRightSeq3[i];
		boundaryRight.likelihood = dLikelihoodRightSeq3[i];

		_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, NULL, true, false);

		double dPosition;
		double dYaw;
		double dLaneWidth;

		_pLaneMarkingFilter->getCurrentState(dPosition, dYaw, dLaneWidth);
		//cerr << i << ": position " << dPositionFromOctaveSeq3[i] << " from Octave, " << dPosition << " from C++" << endl;
		CPPUNIT_ASSERT_DOUBLES_EQUAL(dPositionFromOctaveSeq3[i], dPosition, 1e-5);
	}
}

