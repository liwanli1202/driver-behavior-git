/*
 * ConnectedComponentDetector.cpp
 *
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include <iostream>
using namespace std;

#include "ConnectedComponentDetector.h"

ConnectedComponentDetector::ConnectedComponentDetector()
{
    _pImage = NULL;
    _label = NULL;
    _algorithm = NINE_POINT;
    _pTimer = NULL;
    _aLabelParentMap = NULL;
}

ConnectedComponentDetector::~ConnectedComponentDetector()
{
}

ConnectedComponentDetector::ConnectedComponentDetector(IplImage* img,
        Algorithm algorithm)
{
    _algorithm = algorithm;
    _pImage = cvCreateImage(cvSize(img->width, img->height), img->depth,
            img->nChannels); //cvCloneImage(img);

    // Create label matrix which has the same size with image.
    _label = cvCreateMat(_pImage->height, _pImage->width, CV_8UC1);
    cvSetZero(_label);
    _aLabelParentMap = (int *) calloc(_pImage->height * _pImage->width,
            sizeof(int));
    _pTimer = new Timer();
}

void ConnectedComponentDetector::setImage(IplImage *img)
{
    cvCopyImage(img, _pImage);
    cvSetZero(_label);
    memset(_aLabelParentMap, 0, _pImage->height * _pImage->width);
}

ConnectedComponentDetector::Algorithm ConnectedComponentDetector::getAlgorithm()
{
    return _algorithm;
}

void ConnectedComponentDetector::setLabelElement(int x, int y, int value)
{
    CvScalar scalar;
    scalar.val[0] = value;
    cvSet2D(_label, x, y, scalar);
}

int ConnectedComponentDetector::getLabelElement(int y, int x)
{
    CvScalar scalar;
    scalar = cvGet2D(_label, y, x);
    return scalar.val[0];
}

#define IMG(x,y) (((uchar *)_pImage->imageData)[(x)+(y)*_pImage->widthStep])

std::map<std::string, int> ConnectedComponentDetector::getImgNeighbors(int x,
        int y)
{
    int north, west;
    north = (x > 0) ? IMG(x - 1, y) : 0;
    west = (y > 0) ? IMG(x, y - 1) : 0;

    std::map<std::string, int> neighbours;
    neighbours["north"] = north;
    neighbours["west"] = west;

    return neighbours;
}

std::vector<int> ConnectedComponentDetector::getNeighborsLabel(int y, int x)
{
    if (_algorithm == FOUR_POINT)
    {
        return FourPoint(y, x);
    } else
    {
        return NinePoint(y, x);
    }
}

std::vector<int> ConnectedComponentDetector::FourPoint(int y, int x)
{
    std::vector<int> neighbours(2);
    neighbours[0] = (y > 0) ? getLabelElement(y - 1, x) : 0; //north
    neighbours[1] = (x > 0) ? getLabelElement(y, x - 1) : 0; // west
    return neighbours;
}

std::vector<int> ConnectedComponentDetector::NinePoint(int y, int x)
{
    std::vector<int> neighbours(11);

    neighbours[0] = ((y > 0) && (x > 4)) ? getLabelElement(y - 1, x - 5) : 0;
    neighbours[1] = ((y > 0) && (x > 3)) ? getLabelElement(y - 1, x - 4) : 0;
    neighbours[2] = ((y > 0) && (x > 2)) ? getLabelElement(y - 1, x - 3) : 0;
    neighbours[3] = ((y > 0) && (x > 1)) ? getLabelElement(y - 1, x - 2) : 0;
    neighbours[4] = ((y > 0) && (x > 0)) ? getLabelElement(y - 1, x - 1) : 0;

    neighbours[5] = (y > 0) ? getLabelElement(y - 1, x) : 0;

    neighbours[6] = ((y > 0) && (x < 639)) ? getLabelElement(y - 1, x + 1) : 0;
    neighbours[7] = ((y > 0) && (x < 638)) ? getLabelElement(y - 1, x + 2) : 0;
    neighbours[8] = ((y > 0) && (x < 637)) ? getLabelElement(y - 1, x + 3) : 0;
    neighbours[9] = ((y > 0) && (x < 636)) ? getLabelElement(y - 1, x + 4) : 0;
    neighbours[10] = ((y > 0) && (x < 635)) ? getLabelElement(y - 1, x + 5) : 0;

    return neighbours;
}

void ConnectedComponentDetector::runPass1()
{
    int labelID;
    std::vector<int> neighborLabels;

    assert(_pImage->depth == 8 && _pImage->nChannels == 1);

#define SHOW_CONN_COMPSno
#ifdef SHOW_CONN_COMPS
    cv::Mat imgMat;
    imgMat = cv::Mat::zeros(_pImage->height, _pImage->width, CV_8UC3);
#endif

    int label = 1;
    for (int i = 0; i < _pImage->height; i++)
    {
        for (int j = 0; j < _pImage->width; j++)
        {
            // If the considered pixel has some non-zero value, we will check its neighbors.
            // Otherwise, just ignore it.
            if (IMG(j,i) != 0)
            {
                // get neighbor's label
                neighborLabels = getNeighborsLabel(i, j);
                std::vector<int>::iterator it;
                bool no_label = true;
                // Check whether its neighbors were labeled
                for (it = neighborLabels.begin(); it < neighborLabels.end();
                        ++it)
                {
                    if (*it != 0)
                    {
                        //label already exists at neighbors
                        no_label = false;
                        break;
                    }
                }

                // If all neighbors weren't labeled, these neighbors will be marked with current labelID.
                if (no_label)
                {
                    labelID = label;
                    label++;
                } else
                {
                    // If we found that our neighbors were labeled already, we check for which labelID is the minimum ID.
                    labelID = getMinNeighbours(neighborLabels);
                    // For the neighbor who doesn't have the same labelID with min labelID,
                    for (it = neighborLabels.begin(); it < neighborLabels.end();
                            ++it)
                    {
                        if ((*it != labelID) && (*it != 0))
                        {
                            _union(labelID, *it);
                        }
                    }
                }
                setLabelElement(i, j, labelID);

#ifdef SHOW_CONN_COMPS
                imgMat.at<cv::Vec3b>(i,j)[0] = (float)(labelID*255)/255.0;
                imgMat.at<cv::Vec3b>(i,j)[1] = (float)(labelID*255*4)/255.0;
                imgMat.at<cv::Vec3b>(i,j)[2] = (float)(labelID*255*8)/255.0;
#endif
            }
        }
    }

#ifdef SHOW_CONN_COMPS
    cv::imwrite("conn-comp.png", imgMat);
    cv::imshow("Connected Component",imgMat);
    cvWaitKey(2);
#endif
}

void ConnectedComponentDetector::runPass2()
{
    int i, j;
    cv::Mat imgMat;
    imgMat = cv::Mat::zeros(480, 640, CV_8UC3);
    int labelID;
    for (i = 0; i < _pImage->height; i++)
    {
        for (j = 0; j < _pImage->width; j++)
        {
            if (IMG(j, i) != 0)
            {
                labelID = _find(getLabelElement(i, j));
                setLabelElement(i, j, labelID);
            }
        }
        imgMat.at<cv::Vec3b>(i, j)[0] = (float) (labelID * 255) / 255.0;
        imgMat.at<cv::Vec3b>(i, j)[1] = (float) (labelID * 255 * 4) / 255.0;
        imgMat.at<cv::Vec3b>(i, j)[2] = (float) (labelID * 255 * 8) / 255.0;
    }
    cv::imshow("convFilter2", imgMat);
    cvWaitKey(2);
}

// Returns the minimum label which is not 0, since label starts from 1
int ConnectedComponentDetector::getMinNeighbours(std::vector<int> neighbours)
{
    std::vector<int>::iterator it;
    int min = INT_MAX;
    for (it = neighbours.begin(); it < neighbours.end(); ++it)
    {
        if ((*it < min) && (*it != 0))
        {
            min = *it;
        }
    }
    return min;
}

void ConnectedComponentDetector::_union(int X, int Y)
{
    int j = X;
    int k = Y;
    while (_aLabelParentMap[j] != 0)
    {
        j = _aLabelParentMap[j];
    }
    while (_aLabelParentMap[k] != 0)
    {
        k = _aLabelParentMap[k];
    }
    if (j != k)
    {
        _aLabelParentMap[k] = j;
    }
}

int ConnectedComponentDetector::_find(int X)
{
    int j = X;
    while (_aLabelParentMap[j] != 0)
    {
        j = _aLabelParentMap[j];
    }

    return j;
}

cv::Mat ConnectedComponentDetector::getLabel()
{
    return cv::Mat(_label);
}

void ConnectedComponentDetector::getImagePlaneConnectedComponents(
        std::vector<std::vector<cv::Point> > &connectedComponentsOnImagePlane)
{
    // Construct table which for labeling the points
    std::map<int, std::vector<cv::Point> > label_to_points;
    std::map<int, std::vector<cv::Point> >::iterator it;

    assert(_label->rows == _pImage->height && _label->cols == _pImage->width);
    assert(CV_MAT_DEPTH(_label->type) == CV_8U && _label->step == _label->width);

    uchar *aLabels = _label->data.ptr;

    int label_value;
    runPass1();

    for (int i = 0; i < _label->height; i++)
    {
        for (int j = 0; j < _label->width; j++)
        {
            label_value = aLabels[i * _label->width + j];
            if (label_value != 0)
            {
                it = label_to_points.find(label_value);
                if (it == label_to_points.end())
                {
                    //key not found
                    std::vector<cv::Point> point;
                    // NOTE: Point(x,y). So we have to push back Point(j,i).
                    point.push_back(cv::Point(j, i));
                    label_to_points[label_value] = point;
                } else
                {
                    //key found
                    // NOTE: Point(x,y). So we have to push back Point(j,i).
                    (*it).second.push_back(cv::Point(j, i));
                }
            }
        }
    }

    for (it = label_to_points.begin(); it != label_to_points.end(); ++it)
    {
        std::vector<cv::Point> points = (*it).second;
        connectedComponentsOnImagePlane.push_back(points);
    }
}
