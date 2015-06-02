/*
 * CameraAccessor.cpp
 */

#include "CameraAccessor.h"

#include <highgui.h>

int CameraAccessor::_iObjCount = 0;
ErrorHandler* CameraAccessor::_pErrorHandler = NULL;
Logger* CameraAccessor::_pLogger = NULL;
bool CameraAccessor::_bErrorDiagnosticsListIsSet = false;

void CameraAccessor::initErrorDiagnosticsList()
{
    if (!CameraAccessor::_bErrorDiagnosticsListIsSet)
    {
#define CAMERAACCESSOR_INPUT_CHAR_ARRAY_SIZE_IS_LOW		1001
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAACCESSOR_INPUT_CHAR_ARRAY_SIZE_IS_LOW,
                string(
                        "CameraAccessor : Input character array size is of smaller size than required. "));
#define CAMERAACCESSOR_NULL_IMAGE						1002
        _pErrorHandler->insertErrorDiagnostics((int)CAMERAACCESSOR_NULL_IMAGE,
                string("CameraAccessor : Invalid image. "));
#define CAMERAACCESSOR_VIDEOWRITER_ALREADY_SET			1003
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAACCESSOR_VIDEOWRITER_ALREADY_SET,
                string("CameraAccessor : VideoWriter is already set."));
#define CAMERAACCESSOR_VIDEOWRITER_IS_NOT_SET			1004
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAACCESSOR_VIDEOWRITER_IS_NOT_SET,
                string("CameraAccessor : VideoWriter is not set."));
#define CAMERAACCESSOR_START_TIME_NOT_SET               1005
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAACCESSOR_START_TIME_NOT_SET,
                string("CameraAccessor: Start time is not set yet."));
#define CAMERAACCESSOR_TIMESTAMP_FILE_NOT_OPEN          1006
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAACCESSOR_TIMESTAMP_FILE_NOT_OPEN,
                string("CameraAccessor: Timestamp file is not opened."));
#define CAMERAACCESSOR_ACCESSING_PARTICULAR_FRAME_NOT_ALLOWED   1007
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAACCESSOR_ACCESSING_PARTICULAR_FRAME_NOT_ALLOWED,
                string(
                        "CameraAccessor : Accessing particular frame not allowed."));

        // Escapable Exceptions
#define CAMERAACCESSOR_GRABBED_IMAGE_IS_NULL			1051
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAACCESSOR_GRABBED_IMAGE_IS_NULL,
                string("*CameraAccessor : Grabbed image is null."));
#define CAMERAACCESSOR_GRAB_COMPLETE                    1052
        _pErrorHandler->insertErrorDiagnostics(
                (int)CAMERAACCESSOR_GRAB_COMPLETE,
                string("*CameraAccessor : Grab complete."));
    }
    CameraAccessor::_bErrorDiagnosticsListIsSet = true;
}

CameraAccessor::CameraAccessor(string sCameraName)
{
    _sCameraName = sCameraName;
    CameraAccessor::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
    _pClock = new Clock();
    _pTimer = new Timer();
    _pUtilities = new Utilities();
    initErrorDiagnosticsList();

    _sImageName = string("");
    _sText = string("");
    _bSaveVideo = false;
    _iVideoNo = 1; //default
    _sVideoFile = string("");
    _bIsSetVideoWriter = false;
    _pVideoWriter = NULL;

    _iFrameIntvl_usec = (int)(1000000.0f / VIDEO_FRAMERATE);
    _bWriteVideoTimestamp = WRITE_VIDEO_TIMESTAMP;
    _vpairTimestamp.clear();
    _pairTimestamp.first = -1.;
    _pairTimestamp.second = -1.;
    _fAcquiredFrameRate = 0.0f;

    _bCustomResolveFrameDrops = CUSTOM_RESOLVE_FRAME_DROPS;
    _pImagePrev = NULL;
    _pImageColor = NULL;
    _iCurFramePos = 0;
    _iNumDroppedFrames = 0;

    _bIsSetStartTime = false;
    _iCaptureMode = 0; // CAMERA by default
}

CameraAccessor::~CameraAccessor()
{
    CameraAccessor::_iObjCount--;
    if (_pClock) delete _pClock;
    if (_pTimer) delete _pTimer;
    if (_pUtilities) delete _pUtilities;
    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;

    if (_pVideoWriter) cvReleaseVideoWriter(&_pVideoWriter);
    if (_pImagePrev) cvReleaseImage(&_pImagePrev);
    if (_pImageColor) cvReleaseImage(&_pImageColor);

    if (_bWriteVideoTimestamp)
    {
        int iNumFrames = (int)_vpairTimestamp.size();
        cv::Mat matTimestamp = cv::Mat(2, iNumFrames, CV_64FC1);
        for (int iFrame = 0; iFrame < iNumFrames; iFrame++)
        {
            matTimestamp.at<double>(0, iFrame) = _vpairTimestamp[iFrame].first;
            matTimestamp.at<double>(1, iFrame) = _vpairTimestamp[iFrame].second;
        }
        _fsVideoTimestampYML << "matTimestamp" << matTimestamp;
        _vpairTimestamp.clear();
    }

    if (_fsVideoTimestampYML.isOpened()) _fsVideoTimestampYML.release();
}

string CameraAccessor::getOutputFolder()
{
    return string(Logger::getInstance()->getOutputFolder());
}

void CameraAccessor::setCameraSelectionStrategy(Camera *pCamera,
        int iCaptureMode, int iImageType, bool bDebug)
{
    _pCamera = pCamera;
    _pCamera->setCaptureMode(iCaptureMode, bDebug);
    _pCamera->setImageType(iImageType);
    _iCaptureMode = iCaptureMode;
}

void CameraAccessor::setCameraSelectionStrategy(Camera *pCamera,
        int iCaptureMode, char *pchFileOrFolder, int iImageType,
        bool bFileSizeGt2GB, bool bDebug)
{
    _pCamera = pCamera;
    _pCamera->setCaptureMode(iCaptureMode, pchFileOrFolder, bFileSizeGt2GB,
            bDebug);
    _pCamera->setImageType(iImageType);
    _iCaptureMode = iCaptureMode;
}

int CameraAccessor::gotoFrameNo(int iFrameNo)
{
    if (_iCaptureMode != 1) // VIDEO
    {
        _pErrorHandler->setErrorCode(
                (int)CAMERAACCESSOR_ACCESSING_PARTICULAR_FRAME_NOT_ALLOWED,
                "This feature is only for capture mode 1 (VIDEO)");
    }
    return _pCamera->gotoFrameNo(iFrameNo);
}

void CameraAccessor::initializeCamera(bool bDebug)
{
    _pCamera->initializeCamera(bDebug);
}

bool CameraAccessor::grabImage(bool bDebug)
{
    _pCamera->grabImage(bDebug);
    if (_pCamera->getIsGrabComplete())
    {
        _pErrorHandler->setErrorCode((int)CAMERAACCESSOR_GRAB_COMPLETE);
        return false;
    }
    IplImage *pImage = getImage();
    if (!pImage)
    {
        _pErrorHandler->setErrorCode((int)CAMERAACCESSOR_GRABBED_IMAGE_IS_NULL);
        return false;
    }
    if (_bCustomResolveFrameDrops)
    {
        if (!_pImagePrev)
        {
            _pImagePrev = cvCloneImage(pImage);
            _iCurFramePos = 0;
        }
    }
    int iGrabbedFrameNo = _pCamera->getGrabbedFrameNo() - 1; // incremented after grabImage(), so -1
    if (bDebug) cout << "Frame No = " << iGrabbedFrameNo << endl;

    // Get timestamp of start of capture and the current frame
    if (iGrabbedFrameNo == 0)
    {
        _tvStartTime = _pCamera->getTimeOfStartCapture();
        _tvPrevFrameTime = _tvStartTime;
        _tvFrameRateCheck = _tvStartTime;
    }
    _tvFrameTime = _pCamera->getTimeOfFrameCapture();

    double dTimeDiffInUs = _pTimer->getElapsedTimeInMicroSec(_tvFrameRateCheck,
            _tvFrameTime);
    _fAcquiredFrameRate++;
    if (dTimeDiffInUs >= 1000000) // 1 second
    {
        if (bDebug)
        {
            cout << _sCameraName << ": _fAcquiredFrameRate = "
                    << _fAcquiredFrameRate << endl;
            cout << _sCameraName << ": Grabbed Frame No. " << iGrabbedFrameNo
                    << endl;
        }
        // With every passing second, update _tvFrameRateCheck;
        _tvFrameRateCheck = _pTimer->getCurrentTime();
        _fAcquiredFrameRate = 0.0f;
    }

    if (_bSaveVideo)
    {
        if (_pCamera->getImageType() == (int)MONOCHROME)
        {
            if (!_pImageColor) _pImageColor = cvCreateImage(cvGetSize(pImage),
            IPL_DEPTH_8U, 3);
        }
        if (!_bIsSetVideoWriter)
        {
            if (_pVideoWriter)
            {
                _pErrorHandler->setErrorCode(
                        (int)CAMERAACCESSOR_VIDEOWRITER_ALREADY_SET);
            }
            string sDateTime = Clock::getFormattedDateTimeStringForMySQL(
                    Clock::getCurrentTimeStampInSec());
            string sOutputFolder = string(_pLogger->getOutputFolder());
            _ssOut.str("");
            _ssOut << _iVideoNo;
            _sText = sDateTime + string("_") + _sCameraName + string("_")
                    + _ssOut.str();
            _sVideoFile = sOutputFolder + _sText + string(".avi");
            // Create a video file
            _pVideoWriter = cvCreateVideoWriter(_sVideoFile.c_str(),
                    CV_FOURCC('M', 'J', 'P', 'G'), VIDEO_FRAMERATE,
                    cvSize(pImage->width, pImage->height), 1);
            // Create a timestamp file
            if (_bWriteVideoTimestamp)
            {
                string sOutputFile = sOutputFolder + _sText + string(".yml");
                if (!_fsVideoTimestampYML.isOpened())
                {
                    _fsVideoTimestampYML.open(sOutputFile,
                            cv::FileStorage::WRITE);
                }
            }
            _bIsSetVideoWriter = true;
        }
        if (!_pVideoWriter)
        {
            _pErrorHandler->setErrorCode(
                    (int)CAMERAACCESSOR_VIDEOWRITER_IS_NOT_SET);
        }

        if (_bCustomResolveFrameDrops)
        {
            // Get the timestamp of the current frame and compute its position
            // Check with last position
            // Fill in the missing frames
            // Write info about missing frames as skipped
            double dElapsedTime_usec = _pTimer->getElapsedTimeInMicroSec(
                    _tvStartTime, _tvFrameTime);
            if (bDebug) cout << "dElapsedTime_usec: " << dElapsedTime_usec
                    << endl;
            if (bDebug) cout << "_iFrameIntvl_usec: " << _iFrameIntvl_usec
                    << endl;
            _iCurFramePos = (int)((dElapsedTime_usec + (_iFrameIntvl_usec / 2))
                    / _iFrameIntvl_usec);
            if (bDebug) cout << "_iCurFramePos: " << _iCurFramePos << endl;
            dElapsedTime_usec = _pTimer->getElapsedTimeInMicroSec(
                    _tvPrevFrameTime, _tvFrameTime);
            _iNumDroppedFrames = (int)((dElapsedTime_usec
                    + (_iFrameIntvl_usec / 2)) / _iFrameIntvl_usec) - 1;
            // Here, thought _iNumDroppedFrames is the real value, we do not care
            // if it is dropped more than FRAMERATE times
            if (_iNumDroppedFrames > VIDEO_FRAMERATE) _iNumDroppedFrames =
            VIDEO_FRAMERATE - 1;
            if (bDebug) cout << "_iNumDroppedFrames = " << _iNumDroppedFrames
                    << endl;
            // TODO: Specify saving the image to video file in a different thread
            if (_iNumDroppedFrames > 0)
            {
                bool bDroppedFrame = true;
                for (int i = 0; i < _iNumDroppedFrames; i++)
                {
                    _tvPrevFrameTime.tv_usec += _iFrameIntvl_usec;
                    if (_bWriteVideoTimestamp)
                    {
                        writeToVideoTimestampFile(_tvPrevFrameTime,
                                _iCurFramePos - _iNumDroppedFrames + i,
                                bDroppedFrame, bDebug);
                    }
                    Timer *pT = new Timer();
                    pT->start();

                    if (_pCamera->getImageType() == (int)MONOCHROME)
                    {
                        cvWriteFrame(_pVideoWriter, _pImageColor);
                    }
                    else
                    {
                        cvWriteFrame(_pVideoWriter, _pImagePrev);
                    }
                    pT->stop();
                    double dTimeToSaveOneFrame = pT->getElapsedTimeInMicroSec();
                    if (bDebug) cout << "Time to save one frame: "
                            << dTimeToSaveOneFrame << " us." << endl;
                    // In order to ensure that frames from camera is not missed
                    // while replicating previous frame for dropped frames,
                    // we need to quit adding frames and go ahead to save recent
                    // frames from camera.
                    if (dTimeToSaveOneFrame > _iFrameIntvl_usec / 2) break;
                }
            }
            if (_bWriteVideoTimestamp) writeToVideoTimestampFile(_tvFrameTime,
                    _iCurFramePos, false, bDebug);
            if (_pCamera->getImageType() == (int)MONOCHROME)
            {
                cvCvtColor(pImage, _pImageColor, CV_GRAY2BGR);
                cvWriteFrame(_pVideoWriter, _pImageColor);
            }
            else
            {
                cvWriteFrame(_pVideoWriter, pImage);
            }
            _tvPrevFrameTime = _tvFrameTime;
            cvCopyImage(pImage, _pImagePrev);
        }
        else
        {
            if (_pCamera->getImageType() == (int)MONOCHROME)
            {
                cvCvtColor(pImage, _pImageColor, CV_GRAY2BGR);
                cvWriteFrame(_pVideoWriter, _pImageColor);
            }
            else
            {
                // Analyzing time taken to save one frame. In Toshiba L740, it takes 40ms for 1080p
                // and 17 ms for 720p
                //Timer *pT = new Timer();
                //pT->start();
                cvWriteFrame(_pVideoWriter, pImage);
                //pT->stop();
                //cout << "Time to save one frame: "
                //        << pT->getElapsedTimeInMicroSec() << " us." << endl;
                //delete pT;
            }
            if (_bWriteVideoTimestamp)
            {
                bool bDroppedFrame = false; // We do not check if dropped.
                writeToVideoTimestampFile(_tvFrameTime, iGrabbedFrameNo,
                        bDroppedFrame, bDebug);
            }
        }
    }
    return true;
}

void CameraAccessor::closeCamera(bool bDebug)
{
    _pCamera->closeCamera(bDebug);
    double dTimeElapsedInMs = 0.0;
    int iNumFramesGrabbed = _pCamera->getGrabbedFrameNo();
    float fFramesPerSec = 0.0f;
#if WIN32
    LARGE_INTEGER _tvTimeOfStartCapture = _pCamera->getTimeOfStartCapture();
    LARGE_INTEGER _tvTimeOfFrameCapture = _pCamera->getTimeOfFrameCapture();
#else
    timeval _tvStartCaptureInMs = _pCamera->getTimeOfStartCapture();
    timeval _tvFrameCaptureInMs = _pCamera->getTimeOfFrameCapture();
#endif
    dTimeElapsedInMs = _pTimer->getElapsedTimeInMilliSec(_tvStartCaptureInMs,
            _tvFrameCaptureInMs);
    fFramesPerSec = (float)(iNumFramesGrabbed) / (dTimeElapsedInMs / 1000);
    _ssOut.str("");
    _ssOut << fFramesPerSec;
    string sFrameRate =
            _sCameraName + ": fFramesPerSec = " + _ssOut.str() + " fps."
                    + "\nNote: Ignored frames not taken into account, so the fps may be slightly off."
                    + "\nThe value is incorrect when the processing is paused during system execution.";
    if (bDebug) cout << sFrameRate << endl;
    _pLogger->writeToFile(FILE_LOG,
            string("\nFrame rate\n") + sFrameRate + "\n");
}

void CameraAccessor::setNumFramesToGrab(int iNumFramesToGrab)
{
    _pCamera->setNumFramesToGrab(iNumFramesToGrab);
}

int CameraAccessor::getGrabbedFrameNo(bool bDebug)
{
    return _pCamera->getGrabbedFrameNo();
}

bool CameraAccessor::getIsGrabComplete()
{
    return _pCamera->getIsGrabComplete();
}

bool CameraAccessor::getStopCapture()
{
    return _pCamera->getStopCapture();
}

#ifdef WIN32

LARGE_INTEGER CameraAccessor::getTimeOfStartCapture()
{
    return _pCamera->getTimeOfStartCapture();
}

LARGE_INTEGER CameraAccessor::getTimeOfFrameCapture()
{
    return _pCamera->getTimeOfFrameCapture();
}

#else

timeval CameraAccessor::getTimeOfStartCapture()
{
    return _pCamera->getTimeOfStartCapture();
}

timeval CameraAccessor::getTimeOfFrameCapture()
{
    return _pCamera->getTimeOfFrameCapture();
}
#endif

int CameraAccessor::getCaptureMode()
{
    return _iCaptureMode;
}

IplImage* CameraAccessor::getImage(bool bDebug)
{
    return _pCamera->getImage();
}

void CameraAccessor::getImageSize(int &iWidth, int &iHeight, bool bDebug)
{
    IplImage *pImage = getImage(bDebug);
    iWidth = pImage->width;
    iHeight = pImage->height;
    pImage = NULL;
}

IplImage* CameraAccessor::getImageRectified(bool bDebug)
{
    return _pCamera->getImageRectified();
}

/**
 This function saves the rectified images to the specified folder.
 @param *szFolder The absolute or relative path of the folder to which the captured images are to be saved to.
 @param bSaveRectifiedImage If true, only rectified image is saved, else captured image is saved.
 */
void CameraAccessor::saveCapturedImage(const char *szFolder,
        bool bSaveRectifiedImage)
{
    IplImage *pImage = NULL;
    if (bSaveRectifiedImage)
    {
        pImage = _pCamera->getImageRectified();
    }
    else
    {
        pImage = _pCamera->getImage();
    }
    if (!pImage)
    {
        _pErrorHandler->setErrorCode((int)CAMERAACCESSOR_NULL_IMAGE);
    }
    long lTimeStampOfCaptureInSec = _pCamera->getTimeStampOfCaptureInSec();
    _ssOut.str("");
    _ssOut << lTimeStampOfCaptureInSec;
    _sImageName = getOutputFolder() + _ssOut.str() + string(".png");
    cvSaveImage(_sImageName.c_str(), pImage);
    pImage = NULL;
}

/**
 This function returns the name of the captured image that has been saved.
 @param *pchImage The name of the captured image that was saved.
 @param iLengthOfCharArray The length of the input character array
 */
void CameraAccessor::getNameOfCapturedImage(char *pchImage,
        int iLengthOfCharArray)
{
    if ((int)(_sImageName.length()) > iLengthOfCharArray)
    {
        _pErrorHandler->setErrorCode(
                (int)CAMERAACCESSOR_INPUT_CHAR_ARRAY_SIZE_IS_LOW);
    }
    strcpy(pchImage, _sImageName.c_str());
}

#ifdef WIN32

void CameraAccessor::writeToVideoTimestampFile( LARGE_INTEGER startTime, int iFrameIntvl_usec )
{
    int iFrameNoInVideo = 1;
    _dStartTime_usec = startTime.QuadPart * (1000000.0 / frequency.QuadPart);
    _iFrameIntvl_usec = iFrameIntvl_usec;
    _bIsSetStartTime = true;
    writeToVideoTimestampFile( iFrameNoInVideo, startTime, false );
}

void CameraAccessor::writeToVideoTimestampFile( int iFrameNoInVideo, LARGE_INTEGER frameTime, bool bIsDroppedFrame )
{
    if(!_fsVideoTimestampYML.isOpened())
    {
        _pErrorHandler->setErrorCode((int)CAMERAACCESSOR_TIMESTAMP_FILE_NOT_OPEN);
    }
    if( !_bIsSetStartTime )
    {
        _pErrorHandler->setErrorCode( (int)CAMERAACCESSOR_START_TIME_NOT_SET );
    }
    double dFrameTime_usec = frameTime.QuadPart * (1000000.0 / frequency.QuadPart);
    double dElapsedTime_usec = dFrameTime_usec - _dStartTime_usec;
    double dActualFrameNo = dElapsedTime_usec / _iFrameIntvl_usec;
    string sFrameNoInVideo = string( "" );
    _ssOut.str( "" );
    _ssOut << iFrameNoInVideo;
    sFrameNoInVideo = string( "frame" ) + _ssOut.str();
    _fsVideoTimestampYML << sFrameNoInVideo << "{";
    _fsVideoTimestampYML << "iFrameNoInVideo" << iFrameNoInVideo;
    _fsVideoTimestampYML << "bIsDroppedFrame" << bIsDroppedFrame;
    _fsVideoTimestampYML << "dFrameTime_usec" << dFrameTime_usec;
    _fsVideoTimestampYML << "dElapsedTime_usec" << dElapsedTime_usec;
    _fsVideoTimestampYML << "dActualFrameNo" << dActualFrameNo;
    _fsVideoTimestampYML << "}";
}

#else

void CameraAccessor::writeToVideoTimestampFile(timeval curTime, int iFrameNo,
        bool bDroppedFrame, bool bDebug)
{
    if (!_fsVideoTimestampYML.isOpened())
    {
        _pErrorHandler->setErrorCode(
                (int)CAMERAACCESSOR_TIMESTAMP_FILE_NOT_OPEN);
    }
    _ssOut.str("");
    _ssOut << iFrameNo;
    string sFrameNo = string("f") + _ssOut.str();

    time_t tFrameTime;
    time(&tFrameTime);
    string sTime1 = _pClock->getFormattedDateTimeString(tFrameTime);
    double dTime1 = (double)(long)tFrameTime;
    double dSec = curTime.tv_sec;
    double dMicroSec = curTime.tv_usec;

    // Save timestamp into _vpairTimestamp
    _pairTimestamp.first = dSec;
    _pairTimestamp.second = dMicroSec;
    _vpairTimestamp.push_back(_pairTimestamp);

    // Save timestamp into YML file
    _fsVideoTimestampYML << sFrameNo << "{";
    _fsVideoTimestampYML << "sTime1" << sTime1;
    _fsVideoTimestampYML << "dTime1" << dTime1;
    _fsVideoTimestampYML << "dSec" << dSec;
    _fsVideoTimestampYML << "dMicroSec" << dMicroSec;
    if (bDroppedFrame) _fsVideoTimestampYML << "bDroppedFrame" << 1;
    else _fsVideoTimestampYML << "bDroppedFrame" << 0;
    _fsVideoTimestampYML << "}";
    if (bDebug)
    {
        cout << "FrameNo: " << sFrameNo << endl;
        cout << "sTime1: " << sTime1 << endl;
        cout << "dTime1: " << dTime1 << endl;
        cout << "dSec: " << dSec << endl;
        cout << "dMicroSec: " << dMicroSec << endl;
        if (bDroppedFrame) cout << "bDroppedFrame: true" << endl;
        else cout << "bDroppedFrame: false" << endl;
    }
}

void CameraAccessor::writeToVideoTimestampFile(timeval startTime,
        int iFrameIntvl_usec)
{
    int iFrameNoInVideo = 0;
    _tvStartTime = startTime;
    _bIsSetStartTime = true;
    _iFrameIntvl_usec = iFrameIntvl_usec;
    writeToVideoTimestampFile(iFrameNoInVideo, startTime, false);
}

void CameraAccessor::writeToVideoTimestampFile(int iFrameNoInVideo,
        timeval frameTime, bool bIsDroppedFrame)
{
    if (!_fsVideoTimestampYML.isOpened())
    {
        _pErrorHandler->setErrorCode(
                (int)CAMERAACCESSOR_TIMESTAMP_FILE_NOT_OPEN);
    }
    if (!_bIsSetStartTime)
    {
        _pErrorHandler->setErrorCode((int)CAMERAACCESSOR_START_TIME_NOT_SET);
    }
    double dElapsedTime_usec = _pTimer->getElapsedTimeInMicroSec(_tvStartTime,
            frameTime);
    double dActualFrameNo = dElapsedTime_usec / _iFrameIntvl_usec;
    string sFrameNoInVideo = string("");
    _ssOut.str("");
    _ssOut << iFrameNoInVideo;
    sFrameNoInVideo = string("frame") + _ssOut.str();
    _fsVideoTimestampYML << sFrameNoInVideo << "{";
    _fsVideoTimestampYML << "iFrameNoInVideo" << iFrameNoInVideo;
    _fsVideoTimestampYML << "bIsDroppedFrame" << bIsDroppedFrame;
    _fsVideoTimestampYML << "dElapsedTime_usec" << dElapsedTime_usec;
    //cout << "dActualFrameNo: " << dActualFrameNo << endl;
    _fsVideoTimestampYML << "dActualFrameNo" << dActualFrameNo;
    _fsVideoTimestampYML << "}";
}

#endif

// == Getters and Setters ==

int CameraAccessor::getObjCount()
{
    return CameraAccessor::_iObjCount;
}

void CameraAccessor::setSaveVideo(bool bSaveVideo, int iVideoNo)
{
    if (bSaveVideo && SAVE_VIDEO_AS_GRABBED)
    {
        _bSaveVideo = bSaveVideo;
        // In case of saving using multiple objects of CameraAccesor
        _iVideoNo = iVideoNo;
    }
    else _bSaveVideo = false;
}

bool CameraAccessor::getSaveVideo()
{
    return _bSaveVideo;
}

string CameraAccessor::getVideoFile()
{
    return _sVideoFile;
}

bool CameraAccessor::getIsSetVideoWriter()
{
    return _bIsSetVideoWriter;
}

void CameraAccessor::setWriteVideoTimestamp(bool bWriteVideoTimestamp)
{
    _bWriteVideoTimestamp = bWriteVideoTimestamp;
}

bool CameraAccessor::getWriteVideoTimestamp()
{
    return _bWriteVideoTimestamp;
}

void CameraAccessor::setCustomResolveFrameDrops(bool bCustomResolveFrameDrops)
{
    _bCustomResolveFrameDrops = bCustomResolveFrameDrops;
}

bool CameraAccessor::getCustomResolveFrameDrops()
{
    return _bCustomResolveFrameDrops;
}

string CameraAccessor::getCameraName()
{
    return _sCameraName;
}

int CameraAccessor::getNumFrameCount()
{
    return _pCamera->getNumFrameCount();
}
