/*
 * LaneMarkingDetectorTest.cpp
 *
 *  Created on: Dec 9, 2014
 *      Author: mdailey
 */

#include "LaneMarkingDetectorTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(LaneMarkingDetectorTest);

void LaneMarkingDetectorTest::setUp()
{
    _pLaneMarkingDetector = new LaneMarkingDetector();

    cv::FileStorage fileStorageParamsFile;
    fileStorageParamsFile.open("testFiles/simImage.params",
            cv::FileStorage::READ);
    bool bHasProjMatrix =
            (bool) (int) fileStorageParamsFile["hasProjectionMatrixParams"];
    _distLeftGroundTruth =
            (double) fileStorageParamsFile["distLeftGroundTruth"];
    _distRightGroundTruth =
            (double) fileStorageParamsFile["distRightGroundTruth"];
    int iCameraWidth = (int) fileStorageParamsFile["iCameraWidth"];
    int iCameraHeight = (int) fileStorageParamsFile["iCameraHeight"];
    CPPUNIT_ASSERT(bHasProjMatrix);
    Mat matP;
    fileStorageParamsFile["ProjectionMatrix"] >> matP;
    _pLaneMarkingDetector->initializeCameraModelP(matP.at<double>(0, 0),
            matP.at<double>(0, 1), matP.at<double>(0, 2), matP.at<double>(0, 3),
            matP.at<double>(1, 0), matP.at<double>(1, 1), matP.at<double>(1, 2),
            matP.at<double>(1, 3), matP.at<double>(2, 0), matP.at<double>(2, 1),
            matP.at<double>(2, 2), matP.at<double>(2, 3), iCameraWidth,
            iCameraHeight);
    double dExpectedLaneWidth = fileStorageParamsFile["expectedLaneWidth"];
    _pLaneMarkingDetector->setExpectedLaneWidth(dExpectedLaneWidth);

    _pUtil = new Util();
    _pMatHinv = _pLaneMarkingDetector->getHinv();
    _pMatH = _pLaneMarkingDetector->getH();
}

void LaneMarkingDetectorTest::tearDown()
{
    delete _pLaneMarkingDetector;
    delete _pUtil;
    _pMatHinv = NULL;
    _pMatH = NULL;
}

void LaneMarkingDetectorTest::makeMarkingComponents(
        vector<vector<cv::Point2f> > pointComponents,
        vector<LaneMarkingDetector::MarkingComponent> &markingComponents)
{
    for (unsigned int i = 0; i < pointComponents.size(); i++)
    {
        LaneMarkingDetector::MarkingComponent markingComponent;
        for (unsigned int j = 0; j < pointComponents[i].size(); j++)
        {
            LaneMarkingDetector::MarkingPoint markingPoint;
            markingPoint.imagePoint = pointComponents[i][j];
            _pUtil->imagePlaneToGroundPlane(markingPoint.imagePoint,
                    markingPoint.groundPoint, _pMatHinv);
            markingComponent.push_back(markingPoint);
        }
        markingComponents.push_back(markingComponent);
    }
}

void LaneMarkingDetectorTest::testConsistentTripleLaneMarkings()
{
    vector<vector<Point2f> > pointComponents;

    vector<Point2f> component1;
    vector<Point2f> component2;
    vector<Point2f> component3;

    // Create 3 components that are pairwise consistent but not quite consistent
    // when all three are combined

    for (int i = 0; i < 20; i++)
    {
        component1.push_back(Point2f(100.0 + 4.0 / 20.0 * i, 479.0 - i));
        if (i < 19)
            component2.push_back(Point2f(106.0 + 8.0 / 20.0 * i, 449.0 - i));
        if (i < 18)
            component3.push_back(Point2f(112.0 + 4.0 / 20.0 * i, 419.0 - i));
    }
    pointComponents.push_back(component1);
    pointComponents.push_back(component2);
    pointComponents.push_back(component3);

    vector<LaneMarkingDetector::MarkingComponent> markingComponents;
    makeMarkingComponents(pointComponents, markingComponents);

    // Group the components. A single group is expected.

    vector<vector<vector<LaneMarkingDetector::MarkingPoint> > > groupedMarkingComponents;
    _pLaneMarkingDetector->groupLaneMarkings(markingComponents,
            groupedMarkingComponents);

    CPPUNIT_ASSERT(groupedMarkingComponents.size() == 1);
    CPPUNIT_ASSERT(groupedMarkingComponents[0].size() == 3);

    // The smallest group (component3) should be removed during final fitting

    LaneMarkingDetector::Boundary bestLeftBoundary, bestRightBoundary;
    _pLaneMarkingDetector->findBestBoundaries(groupedMarkingComponents,
            bestLeftBoundary, bestRightBoundary, NULL, true, false);

    CPPUNIT_ASSERT(
            bestLeftBoundary.cPoints > 20 && bestLeftBoundary.cPoints <= 39);
}

void LaneMarkingDetectorTest::testConsistentLaneMarkings()
{
    vector<vector<Point2f> > pointComponents;

    vector<Point2f> component1;
    vector<Point2f> component2;
    vector<Point2f> component3;
    vector<Point2f> component4;
    for (int i = 0; i < 20; i++)
    {
        component1.push_back(Point2f(20.0 + i, 30.0 + i * 0.20));
        component2.push_back(Point2f(60.0 + i, 39.0 + i * 0.18));
        component3.push_back(Point2f(20.0 + i, 100.0 - i * 0.20));
        component4.push_back(Point2f(60.0 + i, 95.0 - i * 0.40));
    }
    pointComponents.push_back(component1);
    pointComponents.push_back(component2);
    pointComponents.push_back(component3);
    pointComponents.push_back(component4);

    vector<LaneMarkingDetector::MarkingComponent> markingComponents;
    makeMarkingComponents(pointComponents, markingComponents);

    vector<vector<vector<LaneMarkingDetector::MarkingPoint> > > groupedMarkingComponents;
    _pLaneMarkingDetector->groupLaneMarkings(markingComponents,
            groupedMarkingComponents);

    CPPUNIT_ASSERT(groupedMarkingComponents.size() == 3);
    CPPUNIT_ASSERT(groupedMarkingComponents[0].size() == 2);
    CPPUNIT_ASSERT(groupedMarkingComponents[1].size() == 1);
    CPPUNIT_ASSERT(groupedMarkingComponents[2].size() == 1);
}

void LaneMarkingDetectorTest::testGroupLaneMarkings()
{
    vector<vector<LaneMarkingDetector::MarkingPoint> > components;

    for (float x = -8; x < 8.1; x += 3.0)
    {
        vector<LaneMarkingDetector::MarkingPoint> markingComponent1;
        vector<LaneMarkingDetector::MarkingPoint> markingComponent2;
        for (int i = 0; i < 20; i++)
        {
            float x1 = x + i * 0.10;
            float y1 = 2.0 + i * 0.10;
            float x2 = x + 5 + i * 0.10;
            float y2 = 2.0 + 5 + i * 0.10;
            LaneMarkingDetector::MarkingPoint point1, point2;
            point1.groundPoint = Point2f(x1, y1);
            point2.groundPoint = Point2f(x2, y2);
            _pUtil->groundPlaneToImagePlane(point1.groundPoint,
                    point1.imagePoint, _pMatH);
            _pUtil->groundPlaneToImagePlane(point2.groundPoint,
                    point2.imagePoint, _pMatH);
            markingComponent1.push_back(point1);
            markingComponent2.push_back(point2);
        }
        components.push_back(markingComponent1);
        components.push_back(markingComponent2);
    }

    vector<vector<vector<LaneMarkingDetector::MarkingPoint> > > groupedMarkingComponents;
    _pLaneMarkingDetector->groupLaneMarkings(components,
            groupedMarkingComponents);

    // Total number of markings should be preserved, and each group should contain 2 markings

    if (groupedMarkingComponents.size() != components.size() / 2)
    {
        cerr << "Got " << groupedMarkingComponents.size()
                << " groups (expected " << components.size() / 2 << ")" << endl;
    }
    CPPUNIT_ASSERT(groupedMarkingComponents.size() == components.size() / 2);
    Line line;
    for (unsigned int i = 0; i < groupedMarkingComponents.size(); i++)
    {
        CPPUNIT_ASSERT(groupedMarkingComponents[i].size() == 2);
    }
}

static void prepareForDetection(const char *sFileNameImage,
        LaneMarkingDetector *pLaneMarkingDetector, IplImage **ppImage,
        IplImage **ppImageProcess)
{
    *ppImage = cvLoadImage(sFileNameImage);
    CPPUNIT_ASSERT(*ppImage != NULL);
    *ppImageProcess = cvCreateImage(cvGetSize(*ppImage), (*ppImage)->depth, 1);
    cvCvtColor(*ppImage, *ppImageProcess, CV_RGB2GRAY);
}

void LaneMarkingDetectorTest::testFindLaneMarkingNoise1()
{
    IplImage *pImage, *pImageProcess;

    prepareForDetection("testFiles/simImage-noise1.png", _pLaneMarkingDetector,
            &pImage, &pImageProcess);

    LaneMarkingDetector::Boundary leftBoundary, rightBoundary;
    _pLaneMarkingDetector->findLaneBoundaries(pImage, pImageProcess,
            leftBoundary, rightBoundary, true, false);
    cout << "Left boundary estimate: " << leftBoundary.pointNearest.x
            << " ground truth (-)" << _distLeftGroundTruth << endl;
    cout << "Right boundary estimate: " << rightBoundary.pointNearest.x
            << " ground truth (+)" << _distRightGroundTruth << endl;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_distLeftGroundTruth, leftBoundary.dPerp,
            0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_distRightGroundTruth, rightBoundary.dPerp,
            0.01);

    cvReleaseImage(&pImage);
    cvReleaseImage(&pImageProcess);
}

void LaneMarkingDetectorTest::testFindLaneMarking()
{
    IplImage *pImage, *pImageProcess;

    prepareForDetection("testFiles/simImage.png", _pLaneMarkingDetector,
            &pImage, &pImageProcess);

    LaneMarkingDetector::Boundary leftBoundary, rightBoundary;
    _pLaneMarkingDetector->findLaneBoundaries(pImage, pImageProcess,
            leftBoundary, rightBoundary, true, true);
    cout << "Left boundary estimate: " << leftBoundary.pointNearest.x
            << " ground truth (-)" << _distLeftGroundTruth << endl;
    cout << "Right boundary estimate: " << rightBoundary.pointNearest.x
            << " ground truth (+)" << _distRightGroundTruth << endl;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_distLeftGroundTruth, leftBoundary.dPerp,
            0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_distRightGroundTruth, rightBoundary.dPerp,
            0.01);

    cvReleaseImage(&pImage);
    cvReleaseImage(&pImageProcess);
}
