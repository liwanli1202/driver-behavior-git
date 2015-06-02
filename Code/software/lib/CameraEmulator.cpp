/*
 * CameraEmulator.cpp
 * PoliceEyes project
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#include "CameraEmulator.h"

#include <iostream>
using namespace std;

int CameraEmulator::_iObjCount = 0;
ErrorHandler* CameraEmulator::_pErrorHandler = NULL;
Logger* CameraEmulator::_pLogger = NULL;
bool CameraEmulator::_bIsSetErrorDiagnosticsList = false;

void CameraEmulator::initErrorDiagnosticsList()
{
    if (!CameraEmulator::_bIsSetErrorDiagnosticsList)
    {
#define CAMERAEMULATOR_NULL_CAPTURE                         3101
        _pErrorHandler->insertErrorDiagnostics((int)CAMERAEMULATOR_NULL_CAPTURE,
                string("CameraEmulator : Capture object is NULL. "));
#define CAMERAEMULATOR_ERROR_INITIALIZING_CAMERA            3102
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAEMULATOR_ERROR_INITIALIZING_CAMERA,
                string("CameraEmulator : Error initializing camera."));
#define CAMERAEMULATOR_ERROR_GRABBING_IMAGE                 3103
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAEMULATOR_ERROR_GRABBING_IMAGE,
                string("CameraEmulator : Error grabbing image."));
#define CAMERAEMULATOR_ERROR_CLOSING_CAMERA                 3104
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAEMULATOR_ERROR_CLOSING_CAMERA,
                string("CameraEmulator : Error closing camera."));
#define CAMERAEMULATOR_INVALID_IMAGE_FILE                   3105
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAEMULATOR_INVALID_IMAGE_FILE,
                string("CameraEmulator : Invalid image file."));
#define CAMERAEMULATOR_NULL_MATRIX                          3106
        _pErrorHandler->insertErrorDiagnostics((int)CAMERAEMULATOR_NULL_MATRIX,
                string("CameraEmulator : Null matrix."));
#define CAMERAEMULATOR_NO_IMAGE_IN_IMAGE_LIST               3107
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAEMULATOR_NO_IMAGE_IN_IMAGE_LIST,
                string("CameraEmulator : No image in image list."));
#define CAMERAEMULATOR_INVALID_CAPTURE_MODE                 3108
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAEMULATOR_INVALID_CAPTURE_MODE,
                string("CameraEmulator : Invalid capture mode."));
#define CAMERAEMULATOR_ACCESSING_PARTICULAR_FRAME_NOT_ALLOWED   3109
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAEMULATOR_ACCESSING_PARTICULAR_FRAME_NOT_ALLOWED,
                string(
                        "CameraEmulator : Accessing particular frame not allowed."));
        // Escapable Exceptions
    }
    CameraEmulator::_bIsSetErrorDiagnosticsList = true;
}

CameraEmulator::CameraEmulator(const char *pchInputVideoFile)
{
    CameraEmulator::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
    _pClock = new Clock();
    _pTimer = new Timer();
    _pUtilities = new Utilities();

    strcpy(_pchInputVideoFile, pchInputVideoFile);
    _lTimeStampOfCapture = 0;
    _vsImageList.clear();
    _iCurImageNo = 0;

    initErrorDiagnosticsList();
}

CameraEmulator::~CameraEmulator()
{
    CameraEmulator::_iObjCount--;
    _vsImageList.clear();
    if (_pClock) delete _pClock;
    if (_pTimer) delete _pTimer;
    if (_pUtilities) delete _pUtilities;
    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;
    if (_pImage) _pImage = NULL;
}

string CameraEmulator::getOutputFolder()
{
    return string(Logger::getInstance()->getOutputFolder());
}

void CameraEmulator::loadMatFromConfig()
{
    if (!(_pMatK && _pMatD))
    {
        _pErrorHandler->setErrorCode((int)CAMERAEMULATOR_NULL_MATRIX);
    }
    cvZero(_pMatK);
    CV_MAT_ELEM( *_pMatK, float, 0, 0 ) = EMULATOR_DEF_FOCAL_X;
    CV_MAT_ELEM( *_pMatK, float, 1, 1 ) = EMULATOR_DEF_FOCAL_Y;
    CV_MAT_ELEM( *_pMatK, float, 0, 2 ) = EMULATOR_DEF_CENTER_X;
    CV_MAT_ELEM( *_pMatK, float, 1, 2 ) = EMULATOR_DEF_CENTER_Y;
    CV_MAT_ELEM( *_pMatK, float, 2, 2 ) = 1.0f;
    _bIsSetMatK = true;

    cvZero(_pMatD);
    CV_MAT_ELEM( *_pMatD, float, 0, 0 ) = EMULATOR_DIST_COEFFS_K1;
    CV_MAT_ELEM( *_pMatD, float, 0, 1 ) = EMULATOR_DIST_COEFFS_K2;
    CV_MAT_ELEM( *_pMatD, float, 0, 2 ) = EMULATOR_DIST_COEFFS_P1;
    CV_MAT_ELEM( *_pMatD, float, 0, 3 ) = EMULATOR_DIST_COEFFS_P2;
    _bIsSetMatD = true;
}

void CameraEmulator::initializeCamera(bool bDebug)
{
    try
    {
        if (_iCaptureMode == (int)IMAGE)
        {
            if (_iImageType == (int)MONOCHROME)
            {
                _pImage = cvLoadImage(_chArrFile, CV_LOAD_IMAGE_GRAYSCALE);
            }
            else if (_iImageType == (int)COLOR)
            {
                _pImage = cvLoadImage(_chArrFile, CV_LOAD_IMAGE_COLOR);
            }
            if (!_pImage)
            {
                _pErrorHandler->setErrorCode(
                        (int)CAMERAEMULATOR_INVALID_IMAGE_FILE);
            }
            _bIsSetImage = true;
        }
        else if (_iCaptureMode == (int)IMAGE_LIST)
        {
            // Enable accessing multiple wildcards for images in image list
            string sInputImageWildcards = string(INPUT_IMAGE_WILDCARD);
            vector<string> vsImageExts = _pUtilities->tokenizeString(
                    sInputImageWildcards, "|", false);

            _vsImageList.clear();
            for (unsigned int iExtNo = 0; iExtNo < vsImageExts.size(); iExtNo++)
            {
                string sImageExt = vsImageExts[iExtNo];
                string sPathWithWildcard = string(_chArrFolder) + sImageExt;
                vector<string> vsImageList = _pUtilities->getFilenamesFromDir(
                        _chArrFolder, (char*)sPathWithWildcard.c_str(), true);
                if (vsImageList.size() < 1)
                {
                    continue;
                }
                _pUtilities->sortVecOfStrings(vsImageList);
                if (bDebug)
                {
                    cout << "List of files in " << sPathWithWildcard << endl;
                    for (unsigned int iFile = 0; iFile < vsImageList.size();
                            iFile++)
                    {
                        cout << "[" << (iFile + 1) << "] " << vsImageList[iFile]
                                << endl;
                    }
                }

                //Copy the list to private instance variable
                for (unsigned int iFile = 0; iFile < vsImageList.size();
                        iFile++)
                {
                    _vsImageList.push_back(vsImageList[iFile]);
                }
                if (bDebug)
                {
                    for (unsigned int iFile = 0; iFile < _vsImageList.size();

                    iFile++)
                    {
                        cout << "[" << (iFile + 1) << "] "
                                << _vsImageList[iFile] << endl;
                    }
                }
            }
            if (_vsImageList.size() < 1)
            {
                _pErrorHandler->setErrorCode(
                        (int)CAMERAEMULATOR_NO_IMAGE_IN_IMAGE_LIST);
            }
        }
        else if (_iCaptureMode == (int)VIDEO)
        {
            capture.open(_pchInputVideoFile);
            if (!capture.isOpened())
            {
                _pErrorHandler->setErrorCode((int)CAMERAEMULATOR_NULL_CAPTURE);
            }
            if (_bPreviewAhead) capturePreview.open(_pchInputVideoFile);
            _iNumFrameCount = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);
        }
        else
        {
            cout << "Capture Mode: " << _iCaptureMode << endl;
            string sCaptureMode = _pUtilities->convertIntegerToString(
                    _iCaptureMode);
            _pErrorHandler->setErrorCode(
                    (int)CAMERAEMULATOR_INVALID_CAPTURE_MODE, sCaptureMode);
        }
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode(
                (int)CAMERAEMULATOR_ERROR_INITIALIZING_CAMERA,
                string(e.what()));
    }
}

void CameraEmulator::grabImage(bool bDebug)
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
        if (_iCaptureMode == (int)IMAGE)
        {
            // Do nothing. A single image is used, which is already set.
        }
        else if (_iCaptureMode == (int)IMAGE_LIST)
        {
            _bIsSetImage = false;
            if (_iCurImageNo < (int)_vsImageList.size())
            {
                if (bDebug)
                {
                    cout << "CurImageNo: " << _iCurImageNo << " Path: "
                            << _vsImageList[_iCurImageNo] << endl;
                }
                if (_iImageType == (int)MONOCHROME)
                {
                    if (_pImage) cvReleaseImage(&_pImage);
                    _pImage = cvLoadImage(_vsImageList[_iCurImageNo].c_str(),
                            CV_LOAD_IMAGE_GRAYSCALE);
                }
                else if (_iImageType == (int)COLOR)
                {
                    if (_pImage) cvReleaseImage(&_pImage);
                    _pImage = cvLoadImage(_vsImageList[_iCurImageNo].c_str(),
                            CV_LOAD_IMAGE_COLOR);
                }
            }
            if (_iCurImageNo == (int)_vsImageList.size())
            {
                _bIsGrabComplete = true;
            }
            _iCurImageNo++;
            _bIsSetImage = true;
        }
        else if (_iCaptureMode == (int)VIDEO)
        {
            _bIsSetImage = false;
            _bIsSetImageRectified = false;

            if(capture.grab())
            {
            	capture.retrieve(_matImage,0);
            }else{
            	_bIsGrabComplete = false;
            	_bIsSetImage = false;
            	return;
            }

            _img = _matImage;
            _pImage = &_img;
            if (!_pImage)
            {
                // End of video reached.
                _bIsGrabComplete = true;
                _bIsSetImage = false;
            }
            else
            {
                _bIsSetImage = true;
            }
            if (_bPreviewAhead)
            {
                int iFrameNo = _iGrabbedFrameNo + iNumFramesAhead;
                if (iFrameNo < capturePreview.get(CV_CAP_PROP_FRAME_COUNT))
                {
                    capturePreview.set(CV_CAP_PROP_POS_FRAMES, iFrameNo);
                    capturePreview >> _matImagePreview;
                    _imgPreview = _matImagePreview;
                    _pImagePreview = &_imgPreview;
                    _pUtilities->displayImage(_pImagePreview, "Preview", false);
                }
            }
        }
        else
        {
            _pErrorHandler->setErrorCode(
                    (int)CAMERAEMULATOR_INVALID_CAPTURE_MODE);
        }
        if (_iGrabbedFrameNo == 0) _tvTimeOfStartCapture =
                _pTimer->getCurrentTime();
        _tvTimeOfFrameCapture = _pTimer->getCurrentTime();
        _lTimeStampOfCapture = _pClock->getCurrentTimeStampInSec();
        _iGrabbedFrameNo++;
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode((int)CAMERAEMULATOR_ERROR_GRABBING_IMAGE,
                string(e.what()));
    }
}

void CameraEmulator::closeCamera(bool bDebug)
{
    try
    {
        if (_iCaptureMode == (int)IMAGE || _iCaptureMode == (int)IMAGE_LIST)
        {
            // Do nothing
        }
        else if (_iCaptureMode == (int)VIDEO)
        {
            if (capture.isOpened()) capture.release();
            if (capturePreview.isOpened()) capturePreview.release();
        }
        else
        {
            _pErrorHandler->setErrorCode(
                    (int)CAMERAEMULATOR_INVALID_CAPTURE_MODE);
        }
    }
    catch (ErrorHandler::Exception &e)
    {
        _pErrorHandler->setErrorCode((int)CAMERAEMULATOR_ERROR_CLOSING_CAMERA,
                string(e.what()));
    }
}

bool CameraEmulator::gotoFrameNo(int iFrameNo)
{
    if (_iCaptureMode != (int)VIDEO)
    {
        _pErrorHandler->setErrorCode(
                (int)CAMERAEMULATOR_ACCESSING_PARTICULAR_FRAME_NOT_ALLOWED,
                "This feature is only allowed for capture mode = 1 (VIDEO)");
    }
    bool bIsSetFrame = capture.set(CV_CAP_PROP_POS_FRAMES, iFrameNo);
    return bIsSetFrame;
}

void CameraEmulator::setPreviewAhead(bool bPreviewAhead)
{
    _bPreviewAhead = bPreviewAhead;
}

// == Getters and Setters ==

int CameraEmulator::getObjCount()
{
    return CameraEmulator::_iObjCount;
}
