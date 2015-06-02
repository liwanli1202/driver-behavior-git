/*
 * Camera.cpp
 */

#include "Camera.h"

#include <iostream>
using namespace std;

int Camera::_iObjCount = 0;
ErrorHandler* Camera::_pErrorHandler = NULL;
Logger* Camera::_pLogger = NULL;
bool Camera::_bIsSetErrorDiagnosticsList = false;

void Camera::initErrorDiagnosticsList()
{
    if (!Camera::_bIsSetErrorDiagnosticsList)
    {
#define CAMERA_CAPTUREMODE_NOT_SET					1101
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_CAPTUREMODE_NOT_SET,
                string("Camera : Capture Mode is not set. "));
#define CAMERA_IMAGETYPE_NOT_SET					1102
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_IMAGETYPE_NOT_SET,
                string("Camera : Image Type is not set. "));
#define CAMERA_NULL_IMAGE							1103
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_NULL_IMAGE,
                string("Camera : Image is NULL"));
#define CAMERA_INVALID_FILENAME						1104
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_INVALID_FILENAME,
                string("Camera : Could not load image. Invalid filename."));
#define CAMERA_MATRIX_NOT_SET						1105
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_MATRIX_NOT_SET,
                string("Camera : Matrix is not set."));
#define CAMERA_INCONSISTENT_MATRICES				1106
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERA_INCONSISTENT_MATRICES,
                string("Camera : Matrices are inconsistent in size or type."));
#define CAMERA_IMAGE_FILE_NOT_SET					1107
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_IMAGE_FILE_NOT_SET,
                string("Camera : Image file is not set."));
#define CAMERA_VIDEO_FILE_NOT_SET					1108
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_VIDEO_FILE_NOT_SET,
                string("Camera : Video file is not set."));
#define CAMERA_INVALID_FILE							1109
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_INVALID_FILE,
                string("Camera : Invalid file. Check path of file. Specify -largefile as option if file is larger than 2GB"));
#define CAMERA_INVALID_FOLDER						1110
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_INVALID_FOLDER,
                string("Camera : Invalid folder. Check path of folder."));
#define CAMERA_IMAGE_LIST_FOLDER_NOT_SET			1111
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERA_IMAGE_LIST_FOLDER_NOT_SET,
                string(
                        "Camera : The folder containing the list of images is not set."));
#define CAMERA_RECTIFIED_IMAGE_NOT_COMPUTED			1112
        _pErrorHandler->insertErrorDiagnostics(
                CAMERA_RECTIFIED_IMAGE_NOT_COMPUTED,
                string(
                        "Camera : It is specified not to compute the rectified image."));

        // Escapable Exceptions
#define CAMERA_IMAGE_NOT_SET                        1151
        _pErrorHandler->insertErrorDiagnostics((int)CAMERA_IMAGE_NOT_SET,
                string("*Camera : Image is not set."));
    }
    Camera::_bIsSetErrorDiagnosticsList = true;
}

Camera::Camera()
{
    Camera::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
    initErrorDiagnosticsList();

    _pUtilities = new Utilities();

    _pImage = NULL;
    _bIsSetImage = false;
    _pImageRectified = NULL;
    _bIsSetImageRectified = false;
    _pMatK = cvCreateMat(3, 3, CV_32FC1);
    _pMatD = cvCreateMat(1, 4, CV_32FC1);
    _bIsSetMatK = false;
    _bIsSetMatD = false;

    _iImageWidth = -1;
    _iImageHeight = -1;
    _bIsSetCaptureMode = false;
    _bIsSetImageType = false;
    _iNumFramesToGrab = 0;
    _iGrabbedFrameNo = 0;
    _bIsGrabComplete = false;
    _bStopCapture = false;

    _bComputeRectifiedImage = false;

    _lTimeStampOfCaptureInSec = 0;
#ifdef WIN32
    _tvTimeOfStartCapture.QuadPart = 0;
    _tvTimeOfFrameCapture.QuadPart = 0;
#else
    _tvTimeOfStartCapture.tv_sec = 0;
    _tvTimeOfStartCapture.tv_usec = 0;
    _tvTimeOfFrameCapture.tv_sec = 0;
    _tvTimeOfFrameCapture.tv_usec = 0;
#endif
    _iNumFrameCount = -1;
}

Camera::~Camera()
{
    Camera::_iObjCount--;
    if (_pImage) cvReleaseImage(&_pImage);
    if (_pImageRectified) cvReleaseImage(&_pImageRectified);
    if (_pMatK) cvReleaseMat(&_pMatK);
    if (_pMatD) cvReleaseMat(&_pMatD);

    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;
    if (_pUtilities) delete _pUtilities;
}

string Camera::getOutputFolder()
{
    return string(Logger::getInstance()->getOutputFolder());
}

// == Getters and Setters ==

int Camera::getObjCount()
{
    return Camera::_iObjCount;
}

void Camera::setImage(IplImage *pImage)
{
    _bIsSetImage = false;
    if (!pImage)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_NULL_IMAGE);
    }
    if (_pImage) cvReleaseImage(&_pImage);
    _pImage = cvCloneImage(pImage);
    _bIsSetImage = true;
}

void Camera::setImage(char *szImageFile, int iColor)
{
    _bIsSetImage = false;
    if (_pImage) cvReleaseImage(&_pImage);
    if (iColor)
    {
        _pImage = cvLoadImage(szImageFile, CV_LOAD_IMAGE_COLOR);
    }
    else
    {
        _pImage = cvLoadImage(szImageFile, CV_LOAD_IMAGE_GRAYSCALE);
    }
    if (!_pImage)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_INVALID_FILENAME);
    }
    _bIsSetImage = true;
}

IplImage* Camera::getImage() const
{
    if (!_bIsSetImage)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_IMAGE_NOT_SET);
        return NULL;
    }
    return _pImage;
}

bool Camera::getIsSetImage()
{
    return _bIsSetImage;
}

void Camera::setImageRectified(IplImage *pImageRectified)
{
    _bIsSetImageRectified = false;
    if (!pImageRectified)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_NULL_IMAGE);
    }
    if (_pImageRectified) cvReleaseImage(&_pImageRectified);
    _pImageRectified = cvCloneImage(pImageRectified);
    _bIsSetImageRectified = true;
}

void Camera::setImageRectified(char *szImageFile, int iColor)
{
    _bIsSetImageRectified = false;
    if (_pImageRectified) cvReleaseImage(&_pImageRectified);
    if (iColor)
    {
        _pImageRectified = cvLoadImage(szImageFile, CV_LOAD_IMAGE_COLOR);
    }
    else
    {
        _pImageRectified = cvLoadImage(szImageFile, CV_LOAD_IMAGE_GRAYSCALE);
    }
    _bIsSetImageRectified = true;
}

/**
 This function computes the rectified image only if invoked.
 */
IplImage* Camera::getImageRectified()
{
    if (!_bComputeRectifiedImage)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_RECTIFIED_IMAGE_NOT_COMPUTED);
    }
    if (!_bIsSetImage)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_IMAGE_NOT_SET);
        return NULL;
    }
    if (!_bIsSetImageRectified)
    {
        // Compute _pImageRectified if not computed already
        if (!(_bIsSetMatK && _bIsSetMatD))
        {
            _pErrorHandler->setErrorCode((int)CAMERA_MATRIX_NOT_SET);
        }
        if (_pImageRectified) cvReleaseImage(&_pImageRectified);
        _pImageRectified = cvCloneImage(_pImage);
        //cout << "Obtaining undistorted image (rectification)" << endl;
        cvUndistort2(_pImage, _pImageRectified, _pMatK, _pMatD);
        _bIsSetImageRectified = true;
    }
    return _pImageRectified;
}

bool Camera::getIsSetImageRectified()
{
    return _bIsSetImageRectified;
}

void Camera::setMatK(CvMat *pMatK)
{
    if (!_pUtilities->getIsEqualSizedMatrices(_pMatK, pMatK))
    {
        _pErrorHandler->setErrorCode((int)CAMERA_INCONSISTENT_MATRICES);
    }
    for (int iRow = 0; iRow < _pMatK->rows; iRow++)
    {
        for (int iCol = 0; iCol < _pMatK->cols; iCol++)
        {
            CV_MAT_ELEM ( *_pMatK, float, iRow, iCol ) =
                    CV_MAT_ELEM( *pMatK, float, iRow, iCol );
        }
    }
    _bIsSetMatK = true;
}

CvMat* Camera::getMatK()
{
    if (!_bIsSetMatK)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_MATRIX_NOT_SET);
    }
    return _pMatK;
}

bool Camera::getIsSetMatK()
{
    return _bIsSetMatK;
}

void Camera::setMatD(CvMat *pMatD)
{
    if (!_pUtilities->getIsEqualSizedMatrices(_pMatD, pMatD))
    {
        _pErrorHandler->setErrorCode((int)CAMERA_INCONSISTENT_MATRICES);
    }
    for (int iRow = 0; iRow < _pMatD->rows; iRow++)
    {
        for (int iCol = 0; iCol < _pMatD->cols; iCol++)
        {
            CV_MAT_ELEM ( *_pMatD, float, iRow, iCol ) =
                    CV_MAT_ELEM( *pMatD, float, iRow, iCol );
        }
    }
    _bIsSetMatD = true;
}

CvMat* Camera::getMatD()
{
    if (!_bIsSetMatD)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_MATRIX_NOT_SET);
    }
    return _pMatD;
}

bool Camera::getIsSetMatD()
{
    return _bIsSetMatD;
}

void Camera::setImageResolution(int iWidth, int iHeight, bool bDebug)
{
    _iImageWidth = iWidth;
    _iImageHeight = iHeight;
}

void Camera::getImageResolution(int &iWidth, int &iHeight)
{
    iWidth = _iImageWidth;
    iHeight = _iImageHeight;
}

int Camera::getCaptureMode()
{
    if (!_bIsSetCaptureMode)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_CAPTUREMODE_NOT_SET);
    }
    return _iCaptureMode;
}

void Camera::setCaptureMode(int iCaptureMode, bool bDebug)
{
    _bIsSetCaptureMode = false;
    _iCaptureMode = iCaptureMode;
    _bIsSetCaptureMode = true;
}

void Camera::setCaptureMode(int iCaptureMode, char *pchFileOrFolder,
        bool bFileSizeGt2GB, bool bDebug)
{
    _bIsSetCaptureMode = false;
    _iCaptureMode = iCaptureMode;
    if (_iCaptureMode == (int)IMAGE)
    {
        if (pchFileOrFolder == NULL)
        {
            _pErrorHandler->setErrorCode((int)CAMERA_IMAGE_FILE_NOT_SET);
        }
        else
        {
            // Check if it is a valid file.
            bool bIsFile = false;
            _pUtilities->getIsFileOrFolderPresent(string(pchFileOrFolder),
                    bIsFile);
            if (!bIsFile)
            {
                _pErrorHandler->setErrorCode((int)CAMERA_INVALID_FILE,
                        string(pchFileOrFolder));
            }
            strcpy(_chArrFile, pchFileOrFolder);
        }
    }
    else if (_iCaptureMode == (int)IMAGE_LIST)
    {
        if (pchFileOrFolder == NULL)
        {
            _pErrorHandler->setErrorCode((int)CAMERA_IMAGE_LIST_FOLDER_NOT_SET);
        }
        else
        {
            // Check if it is a valid file.
            bool bIsFolder = false;
            bool bIsFile = false;
            bIsFolder = _pUtilities->getIsFileOrFolderPresent(
                    string(pchFileOrFolder), bIsFile);
            if (!bIsFolder)
            {
                _pErrorHandler->setErrorCode((int)CAMERA_INVALID_FOLDER,
                        string(pchFileOrFolder));
            }
            strcpy(_chArrFolder, pchFileOrFolder);
        }
    }
    else if (_iCaptureMode == (int)VIDEO)
    {
        if (pchFileOrFolder == NULL)
        {
            _pErrorHandler->setErrorCode((int)CAMERA_VIDEO_FILE_NOT_SET);
        }
        else
        {
            // Check if it is a valid file.
            // NOTE: Sometimes, if the file is of large size, it is considered
            // as an invalid file. e.g. the original TalingChan_1944p.avi is 3.4 GB
            // and was not considered as a file.
            // NOTE: This code can be disabled if the file to be read is large.
            // 2^31 is the largest integer -> 2147483648, which is 2GB. So, I think
            // anything larger than 2GB will be considered as not a file
            // http://pubs.opengroup.org/onlinepubs/7908799/xsh/stat.html
            if (!bFileSizeGt2GB)
            {
                bool bIsFile = false;
                bool bIsFileOrFolderPresent =
                        _pUtilities->getIsFileOrFolderPresent(
                                string(pchFileOrFolder), bIsFile);
                if (!bIsFile)
                {
                    _pErrorHandler->setErrorCode((int)CAMERA_INVALID_FILE,
                            string(pchFileOrFolder));
                }
            }
            strcpy(_chArrFile, pchFileOrFolder);
        }
    }
    _bIsSetCaptureMode = true;
}

bool Camera::getIsSetCaptureMode()
{
    return _bIsSetCaptureMode;
}

int Camera::getImageType()
{
    if (!_bIsSetImageType)
    {
        _pErrorHandler->setErrorCode((int)CAMERA_IMAGETYPE_NOT_SET);
    }
    return _iImageType;
}

void Camera::setImageType(int iImageType)
{
    _iImageType = iImageType;
    _bIsSetImageType = true;
}

bool Camera::getIsSetImageType()
{
    return _bIsSetImageType;
}

void Camera::setNumFramesToGrab(int iNumFramesToGrab)
{
    _iNumFramesToGrab = iNumFramesToGrab;
}

int Camera::getNumFramesToGrab()
{
    return _iNumFramesToGrab;
}

int Camera::getGrabbedFrameNo()
{
    return _iGrabbedFrameNo;
}

bool Camera::getIsGrabComplete()
{
    return _bIsGrabComplete;
}

bool Camera::getStopCapture()
{
    return _bStopCapture;
}

void Camera::setComputeRectifiedImage(bool bComputeRectifiedImage)
{
    _bComputeRectifiedImage = bComputeRectifiedImage;
}

bool Camera::getComputeRectifiedImage()
{
    return _bComputeRectifiedImage;
}

long Camera::getTimeStampOfCaptureInSec()
{
    return _lTimeStampOfCaptureInSec;
}

#ifdef WIN32

LARGE_INTEGER Camera::getTimeOfStartCapture()
{
    return _tvTimeOfStartCapture;
}

LARGE_INTEGER Camera::getTimeOfFrameCapture()
{
    return _tvTimeOfFrameCapture;
}

#else

timeval Camera::getTimeOfStartCapture()
{
    return _tvTimeOfStartCapture;
}

timeval Camera::getTimeOfFrameCapture()
{
    return _tvTimeOfFrameCapture;
}
#endif

int Camera::getNumFrameCount()
{
    return _iNumFrameCount;
}
