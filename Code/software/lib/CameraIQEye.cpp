/*
 * CameraIQEye.cpp
 */

#include "CameraIQEye.h"

#include <iostream>
using namespace std;

int CameraIQEye::_iObjCount = 0;
ErrorHandler* CameraIQEye::_pErrorHandler = NULL;
Logger* CameraIQEye::_pLogger = NULL;
bool CameraIQEye::_bIsSetErrorDiagnosticsList = false;

void CameraIQEye::initErrorDiagnosticsList()
{
    if (!CameraIQEye::_bIsSetErrorDiagnosticsList)
    {
#define CAMERAIQEYE_NULL_CAPTURE							1401
        _pErrorHandler->insertErrorDiagnostics((int)CAMERAIQEYE_NULL_CAPTURE,
                string("CameraIQEye : Capture object is NULL. "));
#define CAMERAIQEYE_ERROR_INITIALIZING_CAMERA				1402
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAIQEYE_ERROR_INITIALIZING_CAMERA,
                string("CameraIQEye : Error initializing camera."));
#define CAMERAIQEYE_ERROR_GRABBING_IMAGE					1403
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAIQEYE_ERROR_GRABBING_IMAGE,
                string("CameraIQEye : Error grabbing image."));
#define CAMERAIQEYE_ERROR_CLOSING_CAMERA					1404
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAIQEYE_ERROR_CLOSING_CAMERA,
                string("CameraIQEye : Error closing camera."));
#define CAMERAIQEYE_INVALID_IMAGE_FILE						1405
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAIQEYE_INVALID_IMAGE_FILE,
                string("CameraIQEye : Invalid image file."));
#define CAMERAIQEYE_NULL_MATRIX								1406
        _pErrorHandler->insertErrorDiagnostics((int)CAMERAIQEYE_NULL_MATRIX,
                string("CameraIQEye : Null matrix."));
#define CAMERAIQEYE_INVALID_CAPTURE_MODE                    1407
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAIQEYE_INVALID_CAPTURE_MODE,
                string(
                        "CameraIQEye : Invalid capture mode. Try CameraEmulator instead."));
#define CAMERAIQEYE_INVALID_IMAGE_TYPE                      1408
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAIQEYE_INVALID_IMAGE_TYPE,
                string("CameraIQEye : Invalid image type."));
#define CAMERAIQEYE_UNSUPPORTED_METHOD                          1409
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAIQEYE_UNSUPPORTED_METHOD,
                string("CameraIQEye : Invalid method."));

        // Escapable Exceptions
#define CAMERAIQEYE_GRABBED_IMAGE_IS_NULL					1451
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAIQEYE_GRABBED_IMAGE_IS_NULL,
                string("*CameraIQEye : Grabbed image is null."));
    }
    CameraIQEye::_bIsSetErrorDiagnosticsList = true;
}

CameraIQEye::CameraIQEye(int iStreamerID, const char *pchInputVideoStream)
{
    CameraIQEye::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
    _pClock = new Clock();
    _iStreamerID = iStreamerID;
    _pIQEyeVideoStreamer = new IQEyeVideoStreamer(_iStreamerID,
            pchInputVideoStream);
    _pTimer = new Timer();
    _pUtilities = new Utilities();

    _lTimeStampOfCapture = 0;

    initErrorDiagnosticsList();
}

CameraIQEye::~CameraIQEye()
{
    CameraIQEye::_iObjCount--;
    if (_pIQEyeVideoStreamer) delete _pIQEyeVideoStreamer;
    if (_pClock) delete _pClock;
    if (_pTimer) delete _pTimer;
    if (_pUtilities) delete _pUtilities;
    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;
}

string CameraIQEye::getOutputFolder()
{
    return string(Logger::getInstance()->getOutputFolder());
}

// TODO:Update implementation
// Invalid implementation because now, the calibration needs to be
// linked with image resolution
void CameraIQEye::loadMatFromConfig()
{
    if (!(_pMatK && _pMatD))
    {
        _pErrorHandler->setErrorCode((int)CAMERAIQEYE_NULL_MATRIX);
    }
    cvZero(_pMatK);
    CV_MAT_ELEM( *_pMatK, float, 0, 0 ) = IQEYE_DEF_FOCAL_X;
    CV_MAT_ELEM( *_pMatK, float, 1, 1 ) = IQEYE_DEF_FOCAL_Y;
    CV_MAT_ELEM( *_pMatK, float, 0, 2 ) = IQEYE_DEF_CENTER_X;
    CV_MAT_ELEM( *_pMatK, float, 1, 2 ) = IQEYE_DEF_CENTER_Y;
    CV_MAT_ELEM( *_pMatK, float, 2, 2 ) = 1.0f;
    _bIsSetMatK = true;

    cvZero(_pMatD);
    CV_MAT_ELEM( *_pMatD, float, 0, 0 ) = IQEYE_DIST_COEFFS_K1;
    CV_MAT_ELEM( *_pMatD, float, 0, 1 ) = IQEYE_DIST_COEFFS_K2;
    CV_MAT_ELEM( *_pMatD, float, 0, 2 ) = IQEYE_DIST_COEFFS_P1;
    CV_MAT_ELEM( *_pMatD, float, 0, 3 ) = IQEYE_DIST_COEFFS_P2;
    _bIsSetMatD = true;
}

void CameraIQEye::initializeCamera(bool bDebug)
{
    try
    {
        if (_iCaptureMode == (int)CAMERA)
        {
            if (_iImageType == (int)MONOCHROME)
            {
                _pImage = cvCreateImage(cvSize(_iImageWidth, _iImageHeight),
                IPL_DEPTH_8U, 1);
            }
            else if (_iImageType == (int)COLOR)
            {
                _pImage = cvCreateImage(cvSize(_iImageWidth, _iImageHeight),
                IPL_DEPTH_8U, 3);
            }
            else
            {
                _pErrorHandler->setErrorCode(
                        (int)CAMERAIQEYE_INVALID_IMAGE_TYPE);
            }
            _pIQEyeVideoStreamer->setImageResolution(_iStreamerID, _iImageWidth,
                    _iImageHeight);
            _pIQEyeVideoStreamer->initiateLibVLC(_iStreamerID);
            _pIQEyeVideoStreamer->startVideoStreaming(_iStreamerID);
        }
        else
        {
            _pErrorHandler->setErrorCode((int)CAMERAIQEYE_INVALID_CAPTURE_MODE);
        }
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode((int)CAMERAIQEYE_ERROR_INITIALIZING_CAMERA,
                string(e.what()));
    }
}

void CameraIQEye::grabImage(bool bDebug)
{
    try
    {
        // Grab image
        if (_iCaptureMode == (int)CAMERA)
        {
            if (_bStopCapture || _bIsGrabComplete) return;
            if (_iNumFramesToGrab == -1 || _iGrabbedFrameNo < _iNumFramesToGrab)
            {
                _bIsGrabComplete = false;
                _bStopCapture = false;
            }
            else
            {
                _bIsGrabComplete = true;
                _bStopCapture = true;
                return;
            }

            _bIsSetImage = false;
            _bIsSetImageRectified = false;
            _pIQEyeVideoStreamer->setImageIsReady(_iStreamerID, false);
            while (!_pIQEyeVideoStreamer->getImageIsReady(_iStreamerID))
            {
                //cout << "Image is not ready. Sleeping 1 ms" << endl;
                usleep(1000); //10us
            }
            //cout << "Image is ready." << endl;
            IplImage *pImage = _pIQEyeVideoStreamer->getImage(_iStreamerID); // 4 channels in this image
            cvConvertImage(pImage, _pImage, 0); // 4 channels -> 3 or 1 channel(s)
            pImage = NULL;
            if (!_pImage)
            {
                _pErrorHandler->setErrorCode(
                        (int)CAMERAIQEYE_GRABBED_IMAGE_IS_NULL);
                return;
            }
            if (_iGrabbedFrameNo == 0)
            {
                _tvTimeOfStartCapture =
                        _pIQEyeVideoStreamer->getImageAcquisitionTime(
                                _iStreamerID);
                _tvTimeOfFrameCapture = _tvTimeOfStartCapture;
            }
            else
            {
                _tvTimeOfFrameCapture =
                        _pIQEyeVideoStreamer->getImageAcquisitionTime(
                                _iStreamerID);
            }
            _lTimeStampOfCapture = _pClock->getCurrentTimeStampInSec();
            _bIsSetImage = true;
        }
        else
        {
            _pErrorHandler->setErrorCode((int)CAMERAIQEYE_INVALID_CAPTURE_MODE);
        }
        _iGrabbedFrameNo++;
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode((int)CAMERAIQEYE_ERROR_GRABBING_IMAGE,
                string(e.what()));
    }
}

void CameraIQEye::closeCamera(bool bDebug)
{
    try
    {
        if (_iCaptureMode == (int)CAMERA)
        {
            _pIQEyeVideoStreamer->setStopStreaming(_iStreamerID, true);
            _pIQEyeVideoStreamer->stopVideoStreaming(_iStreamerID);
        }
        else
        {
            _pErrorHandler->setErrorCode((int)CAMERAIQEYE_INVALID_CAPTURE_MODE);
        }
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode((int)CAMERAIQEYE_ERROR_CLOSING_CAMERA,
                string(e.what()));
    }
}

bool CameraIQEye::gotoFrameNo(int iFrameNo)
{
    _pErrorHandler->setErrorCode((int)CAMERAIQEYE_UNSUPPORTED_METHOD,
            "Only emulator supports this method.");
    return -1;
}

// == Getters and Setters ==

int CameraIQEye::getObjCount()
{
    return CameraIQEye::_iObjCount;
}
