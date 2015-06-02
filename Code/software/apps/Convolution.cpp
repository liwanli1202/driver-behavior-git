/*
 * Convolution.cpp
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include "Convolution.h"

Convolution::Convolution()
{
    _pTimer = new Timer();
    _cPixelsIgnoreLeft = _cPixelsIgnoreRight = _cPixelsIgnoreTop =
            _cPixelsIgnoreBottom = 0;
}

Convolution::~Convolution()
{
    if (_pTimer)
        delete _pTimer;
}

void Convolution::setIgnorePixels(int left, int right, int top, int bottom)
{
    _cPixelsIgnoreLeft = left;
    _cPixelsIgnoreRight = right;
    _cPixelsIgnoreTop = top;
    _cPixelsIgnoreBottom = bottom;
}

void Convolution::convolve1DNaive(const uchar *aPixelIn, int cRowCols,
        const float *aKernel, int cKernelCols, float *aConvolutionOut,
        uchar *aPixelRowPadded, bool bDebug)
{
    // Pad left and right edge of the image row
    // by copying leftmost and rightmost pixel.
    // If user has specified a number of border
    // pixels to ignore, the first unignored pixel
    // value is used as the pad.

    uchar padLeft = aPixelIn[0 + _cPixelsIgnoreLeft];
    uchar padRight = aPixelIn[cRowCols - 1 - _cPixelsIgnoreRight];
    for (int i = 0; i < cKernelCols / 2; i++)
    {
        aPixelRowPadded[i] = padLeft;
        aPixelRowPadded[cKernelCols / 2 + cRowCols + i] = padRight;
    }
    for (int i = 0; i < _cPixelsIgnoreLeft; i++)
    {
        aPixelRowPadded[cKernelCols / 2 + i] = padLeft;
    }
    for (int i = 0; i < _cPixelsIgnoreRight; i++)
    {
        aPixelRowPadded[cKernelCols / 2 + cRowCols - 1 - i] = padRight;
    }

    // Copy row into padded array

    for (int i = 0 + _cPixelsIgnoreLeft; i < cRowCols - _cPixelsIgnoreRight;
            i++)
    {
        aPixelRowPadded[cKernelCols / 2 + i] = aPixelIn[i];
    }

    if (bDebug)
    {
        cout << "Padded row: ";
        for (int i = 0; i < cKernelCols / 2 + cRowCols + cKernelCols / 2; i++)
        {
            cout << " " << (int) aPixelRowPadded[i];
        }
        cout << endl;
    }

    // Perform convolution

    for (int i = 0; i < cRowCols; i++)
    {
        aConvolutionOut[i] = 0;
        float convolutionLeft = 0;
        float convolutionRight = 0;

        for (int j = i + cKernelCols - 1, k = 0; k < cKernelCols; j--, k++)
        {
            if (k < cKernelCols / 2)
                convolutionRight += aPixelRowPadded[j] * aKernel[k];
            else if (k > cKernelCols / 2)
                convolutionLeft += aPixelRowPadded[j] * aKernel[k];
            else
                aConvolutionOut[i] = aPixelRowPadded[j] * aKernel[k];
        }
        aConvolutionOut[i] += 2 * min(convolutionLeft, convolutionRight);
    }
}

void Convolution::convolve1DIntegral(const uchar *aPixelIn, int cRowCols,
        int iLaneMarkingWidth, int *aRowIntegral, float *aConvolutionOut,
        bool bDebug)
{
    // Get padded row integral

    uchar padLeft = aPixelIn[0 + _cPixelsIgnoreLeft];
    uchar padRight = aPixelIn[cRowCols - 1 - _cPixelsIgnoreRight];
    aRowIntegral[0] = padLeft;
    for (int iCol = 1; iCol < iLaneMarkingWidth + _cPixelsIgnoreLeft; iCol++)
        aRowIntegral[iCol] = aRowIntegral[iCol - 1] + padLeft;
    for (int iCol = iLaneMarkingWidth + _cPixelsIgnoreLeft;
            iCol < iLaneMarkingWidth + cRowCols - _cPixelsIgnoreRight; iCol++)
        aRowIntegral[iCol] = aRowIntegral[iCol - 1]
                + aPixelIn[iCol - iLaneMarkingWidth];
    for (int iCol = iLaneMarkingWidth + cRowCols - _cPixelsIgnoreRight;
            iCol < 2 * iLaneMarkingWidth + cRowCols; iCol++)
        aRowIntegral[iCol] = aRowIntegral[iCol - 1] + padRight;

    if (bDebug)
    {
        cout << "Marking width " << iLaneMarkingWidth
                << " padded row integral: ";
        for (int i = 0; i < iLaneMarkingWidth + cRowCols + iLaneMarkingWidth;
                i++)
        {
            cout << " " << aRowIntegral[i];
        }
        cout << endl;
    }

    if (iLaneMarkingWidth % 2 == 1)
    {
        // Kernel is -1 -1 ... -1 -0.5 1 1 ... 1 1 -0.5 -1 ... -1 -1
        for (int iCol = 0; iCol < cRowCols; iCol++)
        {
            // Left brim of top hat is from iCol to iCol+iLaneMarkingWidth/2-1
            int leftBrim = aRowIntegral[iCol + iLaneMarkingWidth / 2 - 1]
                    - (iCol == 0 ? 0 : aRowIntegral[iCol - 1]);
            // Left shoulder is at iCol+iLaneMarkingWidth/2
            int leftShoulder = aRowIntegral[iCol + iLaneMarkingWidth / 2]
                    - aRowIntegral[iCol + iLaneMarkingWidth / 2 - 1];
            // Hat top is from iCol+iLaneMarkingWidth/2+1 to iCol+iLaneMarkingWidth/2+iLaneMarkingWidth
            int hatTopRight = aRowIntegral[iCol + iLaneMarkingWidth / 2
                    + iLaneMarkingWidth]
                    - aRowIntegral[iCol + iLaneMarkingWidth];
            int hatTopLeft = aRowIntegral[iCol + iLaneMarkingWidth - 1]
                    - aRowIntegral[iCol + iLaneMarkingWidth / 2];
            int hatTopCtr = aRowIntegral[iCol + iLaneMarkingWidth]
                    - aRowIntegral[iCol + iLaneMarkingWidth - 1];
            // Right shoulder is at iCol+iLaneMarkingWidth/2+iLaneMarkingWidth+1
            int rightShoulder = aRowIntegral[iCol + iLaneMarkingWidth / 2
                    + iLaneMarkingWidth + 1]
                    - aRowIntegral[iCol + iLaneMarkingWidth / 2
                            + iLaneMarkingWidth];
            // Right brim of top hat is from iCol+iLaneMarkingWidth/2+iLaneMarkingWidth+2 to
            //                               iCol+iLaneMarkingWidth/2+iLaneMarkingWidth+2+iLaneMarkingWidth/2-1
            int rightBrim = aRowIntegral[iCol + 2 * iLaneMarkingWidth]
                    - aRowIntegral[iCol + iLaneMarkingWidth / 2
                            + iLaneMarkingWidth + 1];

            int leftTotal = -leftBrim - leftShoulder / 2 + hatTopLeft;
            int rightTotal = -rightBrim - rightShoulder / 2 + hatTopRight;

            aConvolutionOut[iCol] = hatTopCtr + min(leftTotal, rightTotal) * 2;

            if (bDebug)
            {
                cout << "Conv result - " << leftBrim << " - "
                        << leftShoulder / 2 << " + " << hatTopLeft << " + "
                        << hatTopCtr << " + " << hatTopRight << " - "
                        << rightShoulder / 2 << " - " << rightBrim << " = "
                        << aConvolutionOut[iCol] << endl;
            }
        }
    } else
    {
        // Kernel is -1 -1 ... -1 0.5 1 1 ... 1 1 0.5 -1 ... -1 -1
        for (int iCol = 0; iCol < cRowCols; iCol++)
        {
            // Left brim of top hat is from iCol to iCol+iLaneMarkingWidth/2-1
            int leftBrim = aRowIntegral[iCol + iLaneMarkingWidth / 2 - 1]
                    - (iCol == 0 ? 0 : aRowIntegral[iCol - 1]);
            // Left shoulder is at iCol+iLaneMarkingWidth/2
            int leftShoulder = aRowIntegral[iCol + iLaneMarkingWidth / 2]
                    - aRowIntegral[iCol + iLaneMarkingWidth / 2 - 1];
            // Hat top is from iCol+iLaneMarkingWidth/2+1 to iCol+iLaneMarkingWidth/2+iLaneMarkingWidth-1
            int hatTopRight = aRowIntegral[iCol + iLaneMarkingWidth / 2
                    + iLaneMarkingWidth - 1]
                    - aRowIntegral[iCol + iLaneMarkingWidth];
            int hatTopCtr = aRowIntegral[iCol + iLaneMarkingWidth]
                    - aRowIntegral[iCol + iLaneMarkingWidth - 1];
            int hatTopLeft = aRowIntegral[iCol + iLaneMarkingWidth - 1]
                    - aRowIntegral[iCol + iLaneMarkingWidth / 2];
            // Right shoulder is at iCol+iLaneMarkingWidth/2+iLaneMarkingWidth
            int rightShoulder = aRowIntegral[iCol + iLaneMarkingWidth / 2
                    + iLaneMarkingWidth]
                    - aRowIntegral[iCol + iLaneMarkingWidth / 2
                            + iLaneMarkingWidth - 1];
            // Right brim of top hat is from iCol+iLaneMarkingWidth/2+iLaneMarkingWidth+1 to
            //                               iCol+iLaneMarkingWidth/2+iLaneMarkingWidth+1+iLaneMarkingWidth/2-1
            int rightBrim = aRowIntegral[iCol + 2 * iLaneMarkingWidth]
                    - aRowIntegral[iCol + iLaneMarkingWidth / 2
                            + iLaneMarkingWidth];

            int leftTotal = -leftBrim + leftShoulder / 2 + hatTopLeft;
            int rightTotal = -rightBrim + rightShoulder / 2 + hatTopRight;

            aConvolutionOut[iCol] = hatTopCtr + min(leftTotal, rightTotal) * 2;

            if (bDebug)
            {
                cout << "Conv result - " << leftBrim << " + "
                        << leftShoulder / 2 << " + " << hatTopLeft << " + "
                        << hatTopCtr << " + " << hatTopRight << " + "
                        << rightShoulder / 2 << " - " << rightBrim << " = "
                        << aConvolutionOut[iCol] << endl;
            }
        }
    }
}

void Convolution::kernel1D(int iLaneMarkWidth, float **paKernel)
{
    if (iLaneMarkWidth == 0)
    {
        *paKernel = NULL;
        return;
    }
    int iKernelWidth = iLaneMarkWidth * 2 + 1;
    *paKernel = new float[iKernelWidth];
    float *aKernel = *paKernel;

    int left_edge, right_edge;

    for (int i = 0; i < iKernelWidth; i++)
    {
        aKernel[i] = 1;
    }

    if (iLaneMarkWidth % 2 == 0)
    {
        // Even width

        left_edge = right_edge = iLaneMarkWidth / 2;

        int i;
        for (i = 0; i < left_edge; i++)
        {
            aKernel[i] = -1;
        }
        aKernel[i] = 0.5;
        i++;

        int j = iKernelWidth - 1; // last element of the kernel
        for (i = 0; i < right_edge; i++, j--)
        {
            aKernel[j] = -1;
        }
        aKernel[j] = 0.5;
    } else
    {
        // odd width

        left_edge = right_edge = iLaneMarkWidth / 2;

        int i;
        for (i = 0; i < left_edge; i++)
        {
            aKernel[i] = -1;
        }
        aKernel[i] = -0.5;

        int j = iKernelWidth - 1; // last element of the kernel
        for (i = 0; i < right_edge; i++, j--)
        {
            aKernel[j] = -1;
        }
        aKernel[j] = -0.5;
    }
}

void Convolution::threshold(const float *out, int out_size, float *normalized,
        int normalized_size, int n, int lane_width)
{
    float max = 255 * lane_width;
    float cut_off = 0.08 * max;

    for (int i = 0; i < n; i++)
    {
        normalized[i] = (out[i] < cut_off) ? 0.0 : out[i];
    }
}

void Convolution::localMaximaSuppression(const float *aPixelRow, int cCols,
        float *aPixelRowLocalMaxima)
{

    for (int i = 0; i < cCols; i++)
    {
        aPixelRowLocalMaxima[i] = 0.0;
    }

    int i = 0;
    while (i < cCols)
    {
        // Skip 0 elements

        if (aPixelRow[i] == 0)
        {
            i++;
            continue;
        }

        // Find the adjacent pixels with same value

        int k = i + 1;
        while (k < cCols && aPixelRow[k] == aPixelRow[i])
            k++;

        if ((i == 0 || aPixelRow[i] > aPixelRow[i - 1])
                && (k == cCols || aPixelRow[i] > aPixelRow[k]))
        {
            // Local maximum

            aPixelRowLocalMaxima[(i + k - 1) / 2] = aPixelRow[(i + k - 1) / 2];
        }

        // Set up for next iteration

        i = k;
    }
}

#define DEBUG_CONV_IMAGESno

void Convolution::runConvolution1D(cv::Mat & matImg, vector<float *> & vKernels,
        vector<int> vLaneMarkingWidths)
{
    float out[matImg.cols];
    float normalized[matImg.cols];
    float local_maxima[matImg.cols];
    double timeDiffConv = 0, timeDiffThresh = 0, timeDiffNonmax = 0;
    int iRowStart = matImg.rows - vLaneMarkingWidths.size();
    int iRowEnd = matImg.rows - 1;

    Assert<InvalidParameterException>(
            matImg.rows == (int) vKernels.size()
                    && matImg.rows == (int) vLaneMarkingWidths.size());
    Assert<InvalidParameterException>(matImg.channels() == 1);

#ifdef DEBUG_CONV_IMAGES
    cout << "Ignoring " << _cPixelsIgnoreLeft << " left " << _cPixelsIgnoreRight << " right " <<
    _cPixelsIgnoreTop << " top and " << _cPixelsIgnoreBottom << " bottom pixels." << endl;
#endif

    if (iRowStart < _cPixelsIgnoreTop)
        iRowStart = _cPixelsIgnoreTop;
    iRowEnd = iRowEnd - _cPixelsIgnoreBottom;

    int iKernelWidthMax = 0;
    for (unsigned int i = 0; i < vLaneMarkingWidths.size(); i++)
    {
        if (vLaneMarkingWidths[i] * 2 + 1 > iKernelWidthMax)
            iKernelWidthMax = vLaneMarkingWidths[i] * 2 + 1;
    }
    uchar aPixelRowPadded[iKernelWidthMax + matImg.cols];
    int aRowIntegral[iKernelWidthMax + matImg.cols];

#ifdef DEBUG_CONV_IMAGES
    cv::imwrite("before-conv.png", matImg);
#endif

    for (int iRow = 0; iRow < matImg.rows; iRow++)
    {
        int iMarkingWidth = vLaneMarkingWidths[iRow];
        uchar *aDataRow = matImg.data + iRow * matImg.cols;

        if (iRow >= iRowStart && iRow <= iRowEnd && iMarkingWidth > 0)
        {
            float *aKernel = vKernels[iRow];
            Assert<InvalidParameterException>(aKernel);
            int iKernelWidth = 2 * iMarkingWidth + 1;
            timeval timeStart = _pTimer->getCurrentTime();

#define WIDTH_THRESHOLD_INTEGRAL_ROW 3

            if (iMarkingWidth < WIDTH_THRESHOLD_INTEGRAL_ROW)
            {
                this->convolve1DNaive(aDataRow, matImg.cols, aKernel,
                        iKernelWidth, out, aPixelRowPadded, false);
            } else
            {
                this->convolve1DIntegral(aDataRow, matImg.cols, iMarkingWidth,
                        aRowIntegral, out, false);
            }
            timeval timeConv = _pTimer->getCurrentTime();
            this->threshold(out, matImg.cols, normalized, matImg.cols,
                    matImg.cols, iMarkingWidth);
            timeval timeThresh = _pTimer->getCurrentTime();
            this->localMaximaSuppression(normalized, matImg.cols, local_maxima);
            for (int iCol = 0; iCol < matImg.cols; iCol++)
            {
                aDataRow[iCol] = (local_maxima[iCol] > 0) ? 255 : 0;
            }
            timeval timeNonmax = _pTimer->getCurrentTime();
            timeDiffConv += _pTimer->getElapsedTimeInMicroSec(timeStart,
                    timeConv);
            timeDiffThresh += _pTimer->getElapsedTimeInMicroSec(timeConv,
                    timeThresh);
            timeDiffNonmax += _pTimer->getElapsedTimeInMicroSec(timeThresh,
                    timeNonmax);
        } else
        {
            for (int iCol = 0; iCol < matImg.cols; iCol++)
            {
                aDataRow[iCol] = 0;
            }
        }
    }

#ifdef DEBUG_CONV_IMAGES
    cout << "Timing: conv " << timeDiffConv << " thresh " << timeDiffThresh << " nonmax " << timeDiffNonmax << endl;
    cv::imwrite("after-conv.png", matImg);
#endif

}

