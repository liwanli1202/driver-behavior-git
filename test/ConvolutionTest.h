#ifndef CONVOLUTIONTEST_H_
#define CONVOLUTIONTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "Convolution.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class ConvolutionTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ConvolutionTest );
  CPPUNIT_TEST( testRunConvolution1D );
  CPPUNIT_TEST( testRowConvolution );
  CPPUNIT_TEST( testNonMaximaSuppression );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testRunConvolution1D();
  void testRowConvolution();
  void testNonMaximaSuppression();

private:
  Convolution *_pConvolution;
};

#endif
