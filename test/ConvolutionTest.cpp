#include <cv.h>
#include <highgui.h>
using namespace cv;

#include "ConvolutionTest.h"
#include "Convolution.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ConvolutionTest);

void ConvolutionTest::setUp()
{
    _pConvolution = new Convolution();
}

void ConvolutionTest::tearDown()
{
    delete _pConvolution;
}

void ConvolutionTest::testNonMaximaSuppression()
{
    float aIn[][8] = { { 20, 0, 10, 20, 30, 10, 0, 1 }, { 20, 0, 10, 30, 30, 10,
            0, 1 }, { 20, 0, 10, 10, 30, 10, 0, 1 },
            { 20, 20, 0, 0, 30, 0, 1, 1 }, { 20, 20, 20, 20, 20, 20, 20, 20 }, {
                    10, 20, 20, 20, 20, 20, 20, 20 }, { 20, 20, 20, 20, 20, 20,
                    20, 10 }, { 30, 20, 20, 20, 20, 20, 20, 10 }, { 10, 20, 20,
                    20, 20, 20, 20, 30 }, };
    float aOut[][8] = { { 20, 0, 0, 0, 30, 0, 0, 1 },
            { 20, 0, 0, 30, 0, 0, 0, 1 }, { 20, 0, 0, 0, 30, 0, 0, 1 }, { 20, 0,
                    0, 0, 30, 0, 1, 0 }, { 0, 0, 0, 20, 0, 0, 0, 0 }, { 0, 0, 0,
                    0, 20, 0, 0, 0 }, { 0, 0, 0, 20, 0, 0, 0, 0 }, { 30, 0, 0,
                    0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 30 }, };
    float aBufferOut[8];
    int cRows = sizeof(aIn) / sizeof(float) / 8;
    for (int i = 0; i < cRows; i++)
    {
        _pConvolution->localMaximaSuppression(aIn[i], 8, aBufferOut);
        for (int j = 0; j < 8; j++)
        {
            CPPUNIT_ASSERT(aBufferOut[j] == aOut[i][j]);
        }
    }
}

void ConvolutionTest::testRowConvolution()
{
    int iLaneMarkingWidth = 5;
    int iKernelWidth = iLaneMarkingWidth * 2 + 1;
    uchar aPixel1[] = { 10, 10, 10, 10, 10, 10, 50, 50, 50, 50, 50, 10, 10, 10,
            10, 10, 10, 10, 10, 10 };
    int cPixel = sizeof(aPixel1);
    float aConvolutionOut[cPixel];
    uchar aPixelPadded[iKernelWidth + iLaneMarkingWidth * 2];
    int aRowIntegral[cPixel + iLaneMarkingWidth * 2];
    float *aKernel = NULL;

    _pConvolution->kernel1D(iLaneMarkingWidth, &aKernel);
    _pConvolution->convolve1DNaive(aPixel1, cPixel, aKernel, iKernelWidth,
            aConvolutionOut, aPixelPadded, false);
    CPPUNIT_ASSERT(aConvolutionOut[0] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[1] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[2] == -160);
    CPPUNIT_ASSERT(aConvolutionOut[6] == 40);
    CPPUNIT_ASSERT(aConvolutionOut[8] == 200);
    CPPUNIT_ASSERT(aConvolutionOut[10] == 40);
    CPPUNIT_ASSERT(aConvolutionOut[14] == -160);
    CPPUNIT_ASSERT(aConvolutionOut[15] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[16] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[cPixel - 1] == 0);

    uchar aPixel2[] = { 10, 10, 10, 10, 10, 10, 50, 50, 50, 50, 50, 30, 30, 30,
            30, 30, 30, 30, 30, 30 };
    assert(sizeof(aPixel1) == sizeof(aPixel2));
    _pConvolution->convolve1DNaive(aPixel2, cPixel, aKernel, iKernelWidth,
            aConvolutionOut, aPixelPadded, false);
    CPPUNIT_ASSERT(aConvolutionOut[0] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[1] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[2] == -160);
    CPPUNIT_ASSERT(aConvolutionOut[8] == 100);

    delete aKernel;

    _pConvolution->convolve1DIntegral(aPixel1, cPixel, iLaneMarkingWidth,
            aRowIntegral, aConvolutionOut, false);
    CPPUNIT_ASSERT(aConvolutionOut[0] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[1] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[2] == -160);
    CPPUNIT_ASSERT(aConvolutionOut[6] == 40);
    CPPUNIT_ASSERT(aConvolutionOut[8] == 200);
    CPPUNIT_ASSERT(aConvolutionOut[10] == 40);
    CPPUNIT_ASSERT(aConvolutionOut[14] == -160);
    CPPUNIT_ASSERT(aConvolutionOut[15] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[16] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[cPixel - 1] == 0);

    _pConvolution->convolve1DIntegral(aPixel2, cPixel, iLaneMarkingWidth,
            aRowIntegral, aConvolutionOut, false);
    CPPUNIT_ASSERT(aConvolutionOut[0] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[1] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[2] == -160);
    CPPUNIT_ASSERT(aConvolutionOut[8] == 100);

    iLaneMarkingWidth = 4;
    iKernelWidth = iLaneMarkingWidth * 2 + 1;
    _pConvolution->kernel1D(iLaneMarkingWidth, &aKernel);
    _pConvolution->convolve1DNaive(aPixel1, cPixel, aKernel, iKernelWidth,
            aConvolutionOut, aPixelPadded, false);
    CPPUNIT_ASSERT(aConvolutionOut[0] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[1] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[2] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[3] == -160);
    CPPUNIT_ASSERT(aConvolutionOut[4] == -120);
    CPPUNIT_ASSERT(aConvolutionOut[5] == -40);
    CPPUNIT_ASSERT(aConvolutionOut[6] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[7] == 80);
    CPPUNIT_ASSERT(aConvolutionOut[8] == 160);
    CPPUNIT_ASSERT(aConvolutionOut[9] == 80);
    CPPUNIT_ASSERT(aConvolutionOut[10] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[14] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[15] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[19] == 0);

    _pConvolution->convolve1DIntegral(aPixel1, cPixel, iLaneMarkingWidth,
            aRowIntegral, aConvolutionOut, true);
    CPPUNIT_ASSERT(aConvolutionOut[0] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[1] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[2] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[3] == -160);
    CPPUNIT_ASSERT(aConvolutionOut[4] == -120);
    CPPUNIT_ASSERT(aConvolutionOut[5] == -40);
    CPPUNIT_ASSERT(aConvolutionOut[6] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[7] == 80);
    CPPUNIT_ASSERT(aConvolutionOut[8] == 160);
    CPPUNIT_ASSERT(aConvolutionOut[9] == 80);
    CPPUNIT_ASSERT(aConvolutionOut[10] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[14] == -80);
    CPPUNIT_ASSERT(aConvolutionOut[15] == 0);
    CPPUNIT_ASSERT(aConvolutionOut[19] == 0);

    delete aKernel;
}

void ConvolutionTest::testRunConvolution1D()
{
    IplImage *pImage = cvLoadImage("testFiles/NCA_day-022.jpeg");
    vector<float *> kernels;
    vector<int> widths;

    CPPUNIT_ASSERT(pImage->height == 480);
    CPPUNIT_ASSERT(pImage->nChannels == 3);

    // Convert to gray scale

    IplImage *pImageGray = cvCreateImage(cvGetSize(pImage), pImage->depth, 1);
    cvCvtColor(pImage, pImageGray, CV_RGB2GRAY);

    // Test with invalid parameters

    Mat matImg(pImageGray);
    CPPUNIT_ASSERT_THROW(
            _pConvolution->runConvolution1D(matImg, kernels, widths),
            InvalidParameterException);

    for (int i = 0; i < pImage->height; i++)
    {
        int iWidth = (pImage->height - i) / 20;
        widths.push_back(iWidth);
        kernels.push_back(NULL);
    }

    CPPUNIT_ASSERT_THROW(
            _pConvolution->runConvolution1D(matImg, kernels, widths),
            InvalidParameterException);

    // Run with good parameters

    for (unsigned int i = 0; i < widths.size(); i++)
    {
        float *kernel = NULL;
        _pConvolution->kernel1D(widths[i], &kernel);
        kernels[i] = kernel;
        kernel = NULL;
    }
    _pConvolution->runConvolution1D(matImg, kernels, widths);

    // Check that result is a binary image

    for (int i = 0; i < matImg.rows; i++)
    {
        for (int j = 0; j < matImg.cols; j++)
        {
            // Ensure resulting image is binary
            CPPUNIT_ASSERT(
                    matImg.at<uchar>(i, j) == 0
                            || matImg.at<uchar>(i, j) == 255);
            // Ensure non-maxima are suppressed
            if (matImg.at<uchar>(i, j) != 0)
            {
                CPPUNIT_ASSERT(j == 0 || matImg.at<uchar>(i, j - 1) == 0);
                CPPUNIT_ASSERT(
                        j == matImg.cols - 1
                                || matImg.at<uchar>(i, j + 1) == 0);
            }
        }
    }
}
