/*
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef CLASS_H_
#define CLASS_H_

#include <cv.h>
#include <ml.h>
#include <opencv2/video/background_segm.hpp>

#include <boost/thread.hpp>
#include <boost/circular_buffer.hpp>

#include <vector>
#include <iostream>
using namespace std;
using namespace cv;

#include "CameraEmulator.h"
#include "Camera.h"
#include "CameraUSB.h"
#include "CameraIQEye.h"
#include "CameraAccessor.h"
#include "CameraCaptureConfig.h"
#include "DriverAwarenessConfig.h"
#include "Clock.h"
#include "ErrorHandler.h"
#include "LaneMarkingDetector.h"
#include "LaneMarkingFilter.h"
#include "Logger.h"
#include "Regions.h"
#include "Timer.h"
#include "Utilities.h"
#include "Util.h"
#include "DriverBehaviorAnalysis.h"

extern bool volatile bTerminate;

class DriverAwareness
{
public:
    DriverAwareness();
    ~DriverAwareness();

    static void initErrorDiagnosticsList();
    static string getOutputFolder();

    void startApp(bool bDebug = false);
    bool setFrameForInit(IplImage *pImageGrabbed);
    void processFrame(const IplImage *pImageC, long long timeStamp, bool bDebug = false);

    void createVideoFile(int iVideoFileNo, IplImage *pImage);
    void saveFrameToVideo(IplImage *pImage, int iFrameNo);
    void writeToVideoTimestampFile(timeval curTime, int iFrameNo, bool bDebug =
            false);

    void specifyProcessingRegions(bool bDebug = false);
    void resizeInputImage(IplImage **ppImageResized, int iHeightOfResizedImage,
            bool bSingleChannel = false, bool bDebug = false);
    void initializeSystem(bool bDebug = false);

    void createMask(bool bDebug);

    // Getters and Setters
    static int getObjCount();
    int getFrameNo();

    // Program parameters
    static string getTimeOfCapture();
    static void setNumFramesToIgnore(int iNumFramesToIgnore,
            bool bDebug = false);
    static int getNumFramesToIgnore();
    static void setCamera1(string sCamera1);
    static void setVideo1Path(string sVideo1Path);
    static void setCamera1No(int iCamera1No);
    static void setCaptureMode1(int iCaptureMode1);
    static void setImageType1(int iImageType1);
    static void setFPS(float fFPS);
    static void setNumFramesToGrab(int iNumFramesToGrab);
    static void setSaveVideo(bool bIsSetSaveVideo);
    static void setCamera1Resolution(int iWidth, int iHeight);
    static void setConfigFile(char *pchConfigFile);
    static void setRegionsFromConfigFile(bool bSetRegionsFromConfigFile);

    // Display image windows
    static void setDisableDispWindows(bool bDisableDispWindows);
    static void setDispImgCapture(bool bDispImgCapture);
    // Process frame
    static void setProcessFrames(bool bProcessFrames);
    // Mask
    static void setEnableMask(bool bEnableMask);

    // Display grapn
    static void setDispGraph(bool bDispGraph);

    void setTerminateProgram(bool bTerminateProgram);
    bool getTerminateProgram();

    static void setParamsFile(char *pchParamsFile);

private:
    static int _iObjCount;
    static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;
    static bool _bIsSetErrorDiagnosticsList;

    LaneMarkingDetector *_pLaneMarkingDetector;
	IplImage *_pImageProcessGray;

    Camera *_pCamera;
    Clock *_pClock;
    Regions *_pProcessingRegions;
    Timer *_pTimer;
    Utilities *_pUtilities;
    LaneMarkingFilter *_pLaneMarkingFilter;

    Util *_pUtil;
    DriverBehaviorAnalysis *_pDriverBehaviorAnalysis;

    IplImage *_pImage3Channel;
    IplImage *_pImageResized;
    int _iImageROIWidth;
    int _iImageROIHeight;
    CvRect _rectROI;
    bool _bIsSetROI;

    IplImage *_pImageProcess;
    IplImage *_pImageForDisplay;
    IplImage *_pImageMotion;
    IplImage *_pImageMaskROI;
    IplImage *_pImageForVideo;
    IplImage *_pImageToSave;
    String _sImageToSaveTS;

    IplImage *_pImageMask;


    bool _bIsSystemInitialized;
    int _iFrameNo;
    int _iFrameIntvlCount;
    bool _bIsSetProcessingStartTime;

    stringstream _ssOut;
    string _sText;

    CvFont _font1;
    CvFont _font2;
    CvScalar _textColor;

    CvMemStorage *_pMemStorage;
    CvSeq *_pSeqBlobs;

    // Program parameters
    static string _sTimeOfCapture;  // Time of capture
    static int _iNumFramesToIgnore;
    static string _sCamera1;
    static string _sVideo1Path;
    static int _iCamera1No;
    static int _iCaptureMode1;
    static int _iImageType1;
    static float _fFPS;
    static int _iNumFramesToGrab;
    static int _iNumFramesToSkip;
    static bool _bIsSetSaveVideo;
    static int _iCamera1Width;
    static int _iCamera1Height;
    static bool _bIsSetConfigFile;
    static string _sConfigFile;
    static bool _bIsSetRegionsFromConfigFile;
    static FileStorage _fsConfigFileRead;

    // Display images
    static bool _bDisableDispWindows;
    static bool _bDispImgCapture;
    // Process frame
    static bool _bProcessFrames;
    // Enable mask
    static bool _bEnableMask;

    // Display graph
    static bool _bDispGraph;

    // Database variables
    int _iCurProgramRunID;

    // Saving to video file
    CvVideoWriter *_pVideoWriter;
    int _iVideoFileNo;
    bool _bSaveFrameToVideo;
    bool _bCreateNewVideoFile;
    FileStorage _fsVideoTimestampYML;
    timeval _tvTimeOfFrameCapture;
    int _iFrameNoInVideo;

    // Processing framerate
    timeval _tvPrev;
    double _dProcessingFPS;
    long _lFrameNo;
	long long _currentFrameTimestampInMilliSec;
	long long _prevFrameTimestampInMilliSec;
	long long _timestampAtFirstFrameMilliSec;
	long long _timestamp;

    // Time of capture
    timeval _tvFrameCapture;
    vector<timeval> _vtvTimeBetweenFrames;
    bool _bSlowDown;

    // Terminate program
    bool _bTerminateProgram;

    // Configuration file
    static bool _bIsSetParamsFile;
	static string _sParamsFile;
	static cv::FileStorage _fsParamsFileRead;
	Mat _matProjection;
	Mat _matMask;
	double _dRoll;  // Degrees
	double _dPitch; // Degrees
	double _dYaw;   // Degrees
	int _cPixelsIgnoreLeft;
	int _cPixelsIgnoreRight;
	int _cPixelsIgnoreTop;
	int _cPixelsIgnoreBottom;
	double _dExpectedLaneWidth;
};
#endif //#ifndef CLASS_H_
