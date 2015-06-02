/*
 * Convolution.h
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include <stdlib.h>
#include <highgui.h>
#include <cv.h>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

#include "Assert.hh"
#include "Timer.h"

class Convolution {

public:

	Convolution();
	virtual ~Convolution();

	void convolve1DNaive(
			const uchar *aPixelRow, int cRowCols,
			const float *kernel, int cKernelCols,
			float *out,
			uchar *aPixelRowPadded,
			bool bDebug);

	void convolve1DIntegral(
			const uchar *aPixelIn, int cRowCols, int iLaneMarkingWidth,
			int *aRowIntegral, float *aConvolutionOut, bool bDebug);

	void kernel1D(int width, float **kernel);

	void localMaximaSuppression(
			const float* image_row, int image_row_size,
			float *local_maxima);

	void threshold(
			const float *out, int out_size,
			float *normalized, int normalized_size,
			int n, int lane_width);

	void runConvolution1D(
			cv::Mat& img,
			vector<float *> & kernels,
			vector<int> widths);

	void setIgnorePixels(int left, int right, int top, int bottom);

private:

	Timer *_pTimer;
	int _cPixelsIgnoreLeft;
	int _cPixelsIgnoreRight;
	int _cPixelsIgnoreTop;
	int _cPixelsIgnoreBottom;

};

#endif /* CONVOLUTION_H_ */
