/*
 * CameraUSB.h
 * PoliceEyes project
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#ifndef CAMERAUSB_H_
#define CAMERAUSB_H_

#include "Camera.h"
#include "CameraCaptureConfig.h"
#include "CameraUSBConfig.h"
#include "Clock.h"
#include "ErrorHandler.h"
#include "Logger.h"
#include "Timer.h"
#include "Utilities.h"


class CameraUSB : public Camera
{
public:
	CameraUSB( int iCameraNo );
	~CameraUSB();

    static void initErrorDiagnosticsList();
    static string getOutputFolder();

    void loadMatFromConfig();

    void initializeCamera( bool bDebug = false );
	void grabImage( bool bDebug = false );
	void closeCamera( bool bDebug = false );
    bool gotoFrameNo(int iFrameNo);

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

    CvCapture *_pCapture;
    int _iCameraNo;
    long _lTimeStampOfCapture;
    vector<string> _vsImageList;
    int _iCurImageNo;
};
#endif //#ifndef CAMERAUSB_H_
