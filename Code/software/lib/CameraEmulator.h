/*
 * CameraEmulator.h
 * PoliceEyes project
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#ifndef CAMERAEMULATOR_H_
#define CAMERAEMULATOR_H_

#include "Camera.h"
#include "CameraCaptureConfig.h"
#include "CameraEmulatorConfig.h"
#include "Clock.h"
#include "ErrorHandler.h"
#include "Logger.h"
#include "Timer.h"
#include "Utilities.h"

class CameraEmulator: public Camera
{
public:
    CameraEmulator(const char *pchInputVideoFile);
    ~CameraEmulator();

    static void initErrorDiagnosticsList();
    static string getOutputFolder();

    void loadMatFromConfig();

    void initializeCamera(bool bDebug = false);
    void grabImage(bool bDebug = false);
    void closeCamera(bool bDebug = false);
    bool gotoFrameNo(int iFrameNo);
    void setPreviewAhead(bool bPreview);

    // Getters and Setters
    static int getObjCount();

private:
    static int _iObjCount;
    static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;
    static bool _bIsSetErrorDiagnosticsList;

    Utilities *_pUtilities;
    Timer *_pTimer;
    Clock *_pClock;

    cv::VideoCapture capture;
    cv::VideoCapture capturePreview;
    bool _bPreviewAhead;
    char _pchInputVideoFile[255];
    long _lTimeStampOfCapture;
    vector<string> _vsImageList;
    int _iCurImageNo;
};
#endif //#ifndef CAMERAEMULATOR_H_
