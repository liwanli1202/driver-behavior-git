/*
 * CameraUSB.cpp
 * PoliceEyes project
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#include "CameraUSB.h"

#include <iostream>
using namespace std;

int CameraUSB::_iObjCount = 0;
ErrorHandler* CameraUSB::_pErrorHandler = NULL;
Logger* CameraUSB::_pLogger = NULL;
bool CameraUSB::_bIsSetErrorDiagnosticsList = false;

void CameraUSB::initErrorDiagnosticsList()
{
    if (!CameraUSB::_bIsSetErrorDiagnosticsList)
    {
#define CAMERAUSB_NULL_CAPTURE	                        1301
        _pErrorHandler->insertErrorDiagnostics((int)CAMERAUSB_NULL_CAPTURE,
                string("CameraUSB : Capture object is NULL. "));
#define CAMERAUSB_ERROR_INITIALIZING_CAMERA				1302
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAUSB_ERROR_INITIALIZING_CAMERA,
                string("CameraUSB : Error initializing camera."));
#define CAMERAUSB_ERROR_GRABBING_IMAGE					1303
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAUSB_ERROR_GRABBING_IMAGE,
                string("CameraUSB : Error grabbing image."));
#define CAMERAUSB_ERROR_CLOSING_CAMERA					1304
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAUSB_ERROR_CLOSING_CAMERA,
                string("CameraUSB : Error closing camera."));
#define CAMERAUSB_INVALID_IMAGE_FILE					1305
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAUSB_INVALID_IMAGE_FILE,
                string("CameraUSB : Invalid image file."));
#define CAMERAUSB_NULL_MATRIX							1306
        _pErrorHandler->insertErrorDiagnostics((int)CAMERAUSB_NULL_MATRIX,
                string("CameraUSB : Null matrix."));
#define CAMERAUSB_NO_IMAGE_IN_IMAGE_LIST				1307
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAUSB_NO_IMAGE_IN_IMAGE_LIST,
                string("CameraUSB : No image in image list."));
#define CAMERAUSB_INVALID_CAPTURE_MODE                  1308
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAUSB_INVALID_CAPTURE_MODE,
                string(
                        "CameraUSB : Invalid capture mode. Try CameraEmulator instead."));
#define CAMERAUSB_UNSUPPORTED_METHOD                    1309
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAUSB_UNSUPPORTED_METHOD,
                string("CameraUSB : Invalid method."));
        // Escapable Exceptions
    }
    CameraUSB::_bIsSetErrorDiagnosticsList = true;
}

CameraUSB::CameraUSB(int iCameraNo)
{
    CameraUSB::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
    _pClock = new Clock();
    _pTimer = new Timer();
    _pUtilities = new Utilities();

    _pCapture = NULL;
    _iCameraNo = iCameraNo;
    _lTimeStampOfCapture = 0;
    _vsImageList.clear();
    _iCurImageNo = 0;

    initErrorDiagnosticsList();
}

CameraUSB::~CameraUSB()
{
    CameraUSB::_iObjCount--;
    if (_pClock) delete _pClock;
    if (_pTimer) delete _pTimer;
    if (_pUtilities) delete _pUtilities;
    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;
}

string CameraUSB::getOutputFolder()
{
    return string(Logger::getInstance()->getOutputFolder());
}

void CameraUSB::loadMatFromConfig()
{
    if (!(_pMatK && _pMatD))
    {
        _pErrorHandler->setErrorCode((int)CAMERAUSB_NULL_MATRIX);
    }
    cvZero(_pMatK);
    CV_MAT_ELEM( *_pMatK, float, 0, 0 ) = USB_DEF_FOCAL_X;
    CV_MAT_ELEM( *_pMatK, float, 1, 1 ) = USB_DEF_FOCAL_Y;
    CV_MAT_ELEM( *_pMatK, float, 0, 2 ) = USB_DEF_CENTER_X;
    CV_MAT_ELEM( *_pMatK, float, 1, 2 ) = USB_DEF_CENTER_Y;
    CV_MAT_ELEM( *_pMatK, float, 2, 2 ) = 1.0f;
    _bIsSetMatK = true;

    cvZero(_pMatD);
    CV_MAT_ELEM( *_pMatD, float, 0, 0 ) = USB_DIST_COEFFS_K1;
    CV_MAT_ELEM( *_pMatD, float, 0, 1 ) = USB_DIST_COEFFS_K2;
    CV_MAT_ELEM( *_pMatD, float, 0, 2 ) = USB_DIST_COEFFS_P1;
    CV_MAT_ELEM( *_pMatD, float, 0, 3 ) = USB_DIST_COEFFS_P2;
    _bIsSetMatD = true;
}

void CameraUSB::initializeCamera(bool bDebug)
{
    try
    {
        if (_iCaptureMode == (int)IMAGE || _iCaptureMode == (int)IMAGE_LIST
                || _iCaptureMode == (int)VIDEO)
        {
            _pErrorHandler->setErrorCode((int)CAMERAUSB_INVALID_CAPTURE_MODE);
        }
        else if (_iCaptureMode == (int)CAMERA)
        {
            _pCapture = cvCaptureFromCAM(_iCameraNo);
            if (!_pCapture)
            {
                _pErrorHandler->setErrorCode((int)CAMERAUSB_NULL_CAPTURE);
            }
        }
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode((int)CAMERAUSB_ERROR_INITIALIZING_CAMERA,
                string(e.what()));
    }
}

void CameraUSB::grabImage(bool bDebug)
{
    try
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

        // Grab image
        if (_iCaptureMode == (int)CAMERA)
        {
            _bIsSetImage = false;
            _bIsSetImageRectified = false;
            _pImage = cvQueryFrame(_pCapture);
            if (_iGrabbedFrameNo == 0) _tvTimeOfStartCapture =
                    _pTimer->getCurrentTime();
            _tvTimeOfFrameCapture = _pTimer->getCurrentTime();
            _lTimeStampOfCapture = _pClock->getCurrentTimeStampInSec();
            _bIsSetImage = true;
        }
        else
        {
            _pErrorHandler->setErrorCode((int)CAMERAUSB_INVALID_CAPTURE_MODE);
        }
        _iGrabbedFrameNo++;

#if 0
        int iFrameWidth = (int)cvGetCaptureProperty( _pCapture, CV_CAP_PROP_FRAME_WIDTH ); //3
        int iFrameHeight = (int)cvGetCaptureProperty( _pCapture, CV_CAP_PROP_FRAME_HEIGHT );//4
        int iFPS = (int)cvGetCaptureProperty( _pCapture, CV_CAP_PROP_FPS );//5
        int iFourCC = (int)cvGetCaptureProperty( _pCapture, CV_CAP_PROP_FOURCC );//6
        int iTotalFrames = (int)cvGetCaptureProperty( _pCapture, CV_CAP_PROP_FRAME_COUNT );//7
        cout << "iFrameWidth = " << iFrameWidth << endl <<
        " iFrameHeight = " << iFrameHeight << endl <<
        " iFPS = " << iFPS << endl <<
        " iFourCC = " << iFourCC << endl <<
        " iTotalFrames = " << iTotalFrames << endl;

        double dMilliSec = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_POS_MSEC );//0
        int iFramePos = (int)cvGetCaptureProperty( _pCapture, CV_CAP_PROP_POS_FRAMES );//1
        double dAviRatio = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_POS_AVI_RATIO );//2
        cout << endl << "dMilliSec = " << dMilliSec << endl <<
        " iFramePos = " << iFramePos << endl <<
        " dAviRatio = " << dAviRatio << endl;

        // All the values below are read as 0; probably not set when writing the video.
        int iFormat = (int)cvGetCaptureProperty( _pCapture, CV_CAP_PROP_FORMAT );//8
        int iMode = (int)cvGetCaptureProperty( _pCapture, CV_CAP_PROP_MODE );//9
        double dBrightness = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_BRIGHTNESS );//10
        double dContrast = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_CONTRAST );//11
        double dSaturation = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_SATURATION );//12
        double dHue = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_HUE );//13
        double dGain = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_GAIN );//14
        double dExposure = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_EXPOSURE );//15
        double dConvertRGB = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_CONVERT_RGB );//16
        double dWhiteBalance = -1;//cvGetCaptureProperty( _pCapture, CV_CAP_PROP_WHITE_BALANCE );	//17
        double dRectification = cvGetCaptureProperty( _pCapture, CV_CAP_PROP_RECTIFICATION );//18
        cout << "iFormat = " << iFormat << endl <<
        " iMode = " << iMode << endl <<
        " dBrightness = " << dBrightness << endl;
        cout << "dContrast = " << dContrast << endl <<
        " dSaturation = " << dSaturation << endl <<
        " dHue = " << dHue << endl <<
        " dGain = " << dGain << endl <<
        " dExposure = " << dExposure << endl;
        cout << "dConvertRGB = " << dConvertRGB << endl <<
        " dWhiteBalance = " << dWhiteBalance << endl <<
        " dRectification = " << dRectification << endl;
#endif
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode((int)CAMERAUSB_ERROR_GRABBING_IMAGE,
                string(e.what()));
    }
}

void CameraUSB::closeCamera(bool bDebug)
{
    try
    {
        if (_iCaptureMode == (int)CAMERA)
        {
            if (_pCapture) cvReleaseCapture(&_pCapture);
        }
        else
        {
            _pErrorHandler->setErrorCode((int)CAMERAUSB_INVALID_CAPTURE_MODE);
        }
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode((int)CAMERAUSB_ERROR_CLOSING_CAMERA,
                string(e.what()));
    }
}

bool CameraUSB::gotoFrameNo(int iFrameNo)
{
    _pErrorHandler->setErrorCode((int)CAMERAUSB_UNSUPPORTED_METHOD,
            "Only emulator supports this method.");
    return -1;
}

// == Getters and Setters ==

int CameraUSB::getObjCount()
{
    return CameraUSB::_iObjCount;
}

