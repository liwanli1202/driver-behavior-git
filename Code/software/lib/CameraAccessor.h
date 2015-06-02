/*
 * CameraAccessor.h
 */

#ifndef CAMERAACCESSOR_H_
#define CAMERAACCESSOR_H_

#include <cv.h>

#include "Camera.h"
#include "Clock.h"
#include "ErrorHandler.h"
#include "Logger.h"
#include "Timer.h"
#include "Utilities.h"

class CameraAccessor
{
public:
    CameraAccessor(string sCameraName);
    ~CameraAccessor();

    static void initErrorDiagnosticsList();
    static string getOutputFolder();

    void setCameraSelectionStrategy(Camera *pCamera, int iCaptureMode,
            int iImageType, bool bDebug = false);
    void setCameraSelectionStrategy(Camera *pCamera, int iCaptureMode,
            char *pchFileOrFolder, int iImageType, bool bFileSizeGt2GB = false,
            bool bDebug = false);
    int gotoFrameNo(int iFrameNo);

    void initializeCamera(bool bDebug = false);
    bool grabImage(bool bDebug = false);
    void closeCamera(bool bDebug = false);

    void setNumFramesToGrab(int iNumFramesToGrab);
    int getGrabbedFrameNo(bool bDebug = false);
    bool getIsGrabComplete();
    bool getStopCapture();

#ifdef WIN32
    LARGE_INTEGER getTimeOfStartCapture();
    LARGE_INTEGER getTimeOfFrameCapture();
#else
    timeval getTimeOfStartCapture();
    timeval getTimeOfFrameCapture();
#endif
    int getCaptureMode();

    IplImage* getImage(bool bDebug = false);
    void getImageSize(int &iWidth, int &iHeight, bool bDebug = false);
    IplImage* getImageRectified(bool bDebug = false);

    void saveCapturedImage(const char *szFolder, bool bSaveRectifiedImage);
    void getNameOfCapturedImage(char *pchImage, int iLengthOfCharArray);

    void displayImage(const IplImage *pImage, char *szTitle);
    void setWaitKey(int iNumMilliseconds = 0);

#ifdef WIN32
    void writeToVideoTimestampFile( LARGE_INTEGER startTime, int iFrameIntvl_usec );
    void writeToVideoTimestampFile( int iFrameNoInVideo, LARGE_INTEGER frameTime, bool bIsDroppedFrame );
#else
    void writeToVideoTimestampFile(timeval curTime, int iFrameNo,
            bool bDroppedFrame, bool bDebug);
    void writeToVideoTimestampFile(timeval startTime, int iFrameIntvl_usec);
    void writeToVideoTimestampFile(int iFrameNoInVideo, timeval frameTime,
            bool bIsDroppedFrame);
#endif

    // Getters and Setters
    static int getObjCount();
    void setSaveVideo(bool bSaveVideo, int iVideoNo = 1);
    bool getSaveVideo();
    string getVideoFile();
    bool getIsSetVideoWriter();
    void setWriteVideoTimestamp(bool bWriteVideoTimestamp);
    bool getWriteVideoTimestamp();
    void setCustomResolveFrameDrops(bool bCustomResolveFrameDrops);
    bool getCustomResolveFrameDrops();
    string getCameraName();
    int getNumFrameCount();

private:
    static int _iObjCount;
    static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;
    static bool _bErrorDiagnosticsListIsSet;
    string _sCameraName;

    Utilities *_pUtilities;
    Timer *_pTimer;
    Clock *_pClock;
    Camera *_pCamera;

    CvVideoWriter *_pVideoWriter;

    stringstream _ssOut;
    string _sImageName;
    string _sText;
    bool _bSaveVideo;
    int _iVideoNo;
    string _sVideoFile;
    bool _bIsSetVideoWriter;

    int _iFrameIntvl_usec;
    bool _bWriteVideoTimestamp;
    vector<pair<double, double> > _vpairTimestamp;
    pair<double, double> _pairTimestamp;
    timeval _tvStartTime;
    timeval _tvFrameRateCheck;
    timeval _tvFrameTime;
    timeval _tvPrevFrameTime;
    float _fAcquiredFrameRate;

    bool _bCustomResolveFrameDrops;
    IplImage *_pImagePrev; // For filling in frames during custom resolve frame drops
    IplImage *_pImageColor; // For writing color frames to video file.
    int _iCurFramePos;
    int _iNumDroppedFrames;

    bool _bIsSetStartTime;
    cv::FileStorage _fsVideoTimestampYML;

    int _iCaptureMode;
};
#endif //#ifndef CAMERAACCESSOR_H_
