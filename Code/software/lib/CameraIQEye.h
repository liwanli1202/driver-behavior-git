/*
 * CameraIQEye.h
 */

#ifndef CAMERAIQEYE_H_
#define CAMERAIQEYE_H_

#include "Camera.h"
#include "CameraCaptureConfig.h"
#include "CameraIQEyeConfig.h"
#include "Clock.h"
#include "ErrorHandler.h"
#include "IQEyeVideoStreamer.h"
#include "Logger.h"
#include "Timer.h"
#include "Utilities.h"

class CameraIQEye : public Camera
{
public:
	CameraIQEye( int iStreamerID, const char *pchInputVideoStream );
	~CameraIQEye();

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

    Clock *_pClock;
    IQEyeVideoStreamer *_pIQEyeVideoStreamer;
    Timer *_pTimer;
    Utilities *_pUtilities;

    int _iStreamerID;
    long _lTimeStampOfCapture;
};
#endif //#ifndef CAMERAIQEYE_H_
