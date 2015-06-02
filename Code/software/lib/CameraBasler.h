/*
 * CameraBasler.h
 * Ramesh Marikhu <marikhu@gmail.com> Oct 2011
 */

#ifndef CAMERABASLER_H_
#define CAMERABASLER_H_

#include "Camera.h"
#include "CameraCaptureConfig.h"
#include "CameraBaslerConfig.h"
#include "CGrabBuffer.h"
#include "Clock.h"
#include "ErrorHandler.h"
#include "Logger.h"
#include "Timer.h"
#include "Utilities.h"

// Settings to use Basler GigE cameras
#ifndef USE_GIGE
#define USE_GIGE
#endif

#include <pylon/gige/BaslerGigECamera.h>
#include <pylon/PylonIncludes.h>
typedef Pylon::CBaslerGigECamera Camera_t;
using namespace Basler_GigECameraParams;
using namespace Basler_GigEStreamParams;
using namespace GenApi;
using namespace Pylon;


class CameraBasler : public Camera
{
public:
    struct RGBPixel
    {
        int r, g, b;
    };

    CameraBasler( int iCameraNo );
	~CameraBasler();

    static void initErrorDiagnosticsList();
    static string getOutputFolder();

    void loadMatFromConfig();

    void initializeCamera( bool bDebug = false );
	void grabImage( bool bDebug = false );
	void grabTuningImage( bool bTuningGain, bool bTuningExposure, bool bDebug = false );
	void closeCamera( bool bDebug = false );

	// Getters and Setters
	static int getObjCount();
	static int getAcquisitionMode();
	static void setAcquisitionMode( int iAcquisitionMode );
	static int getExposureMode();
	static void setExposureMode( int iExposureMode );
	static int64 getExposureRaw();
	static void setExposureRaw( int64 iExposureRaw );
	static int getGainMode();
	static void setGainMode( int iGainMode );
	static int64 getGainRaw();
	static void setGainRaw( int64 iGainRaw );
	static double getFrameRate();
	static void setFrameRate( double dFrameRate );
	static void getImageResolutionMax( int &iNumRows, int &iNumCols );
	static void setImageResolutionMax( int iNumRows, int iNumCols );
	static void getImageResolutionCur( int &iNumRows, int &iNumCols );
	static void setImageResolutionCur( int iNumRows, int iNumCols );
	static void setPylonAutoGainControl( bool bPylonAutoGainControl );
	static bool getPylonAutoGainControl();
	static void setCustomAutoGainControl( bool bCustomAutoGainControl );
	static bool getCustomAutoGainControl();
	static void setUpdateCustomGain( bool bUpdateCustomGain );
	static bool getUpdateCustomGain();
	static void setUpdateCustomExposure( bool bUpdateCustomExposure );
	static bool getUpdateCustomExposure();

	Camera_t* getCamera() const;

private:
	static int _iObjCount;
	static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;
    static bool _bIsSetErrorDiagnosticsList;

    static teCaptureMode _eCaptureMode;
    static teImageType _eImageType;
    static teAcquisitionMode _eAcquisitionMode;
    static teExposureMode _eExposureMode;
    static teGainMode _eGainMode;
    static int64 _iExposureRaw;
	static int64 _iGainRaw;
	static double _dFrameRate;
	static int _iNumMaxRows;
	static int _iNumMaxCols;
	static int _iNumRows;
	static int _iNumCols;
	static bool _bPylonAutoGainControl;
	static bool _bCustomAutoGainControl;
	static bool _bUpdateCustomGain;
	static bool _bUpdateCustomExposure;

    Clock *_pClock;
    Timer *_pTimer;
    Utilities *_pUtilities;

    vector<string> _vsImageList;
    int _iCurImageNo;

    Pylon::PylonAutoInitTerm autoInitTerm;
	unsigned char *_pBuffers[ NUM_BUFFERS ];
	Camera_t *_pCamera;
    int _iCameraNo;
	Camera_t::StreamGrabber_t *_pStreamGrabber;
	Pylon::StreamBufferHandle _hBuffer[ NUM_BUFFERS ];
	std::vector<CGrabBuffer*> _vBufferList;
	GrabResult _grabResult;
    int _iNumBuffers;

    unsigned char _YUV[4];
    struct RGBPixel RGBPixel1;
    struct RGBPixel RGBPixel2;

    char _szImage[200];
	char _szDynamicString[200];

	IplImage *_pImagePrev;
    IplImage *_pImageYCrCb;
	string _sPixelFormat;
	stringstream _ssOut;

	bool _bToggleGainAndExpComputation;

	// Private functions

    void activateExposureMode();
    void activateGainMode();
	float getAvgBrightness( IplImage *pImage, int iImageType, bool bDebug = false );
	float getAvgContrast( IplImage *pImage, int iImageType, float fAvgBrightness, bool bDebug = false );
	void getCameraParamsToUpdate( IplImage *pImage, int &iExposureRaw, int &iGainRaw, int iImageType, bool bDebug = false );

    struct RGBPixel YUV_to_RGB( int y,int u,int v );
    void convertYUV422PackedPixelToRGB24Pixel( unsigned char *yuv, struct RGBPixel *rgb1, struct RGBPixel *rgb2 );
    void checkImage( IplImage *pImage );
    void prepareMatCameraAndDistCoeffs();
    void setAutoExposureOnce( bool bDebug = false );
    void setAutoExposureContinuous( bool bDebug = false );
    void setRawExposure( bool bDebug = false );
    void setAutoGainOnce( bool bDebug = false );
    void setAutoGainContinuous( bool bDebug = false );
    void setRawGain( bool bDebug = false );
    void convertYUV422PackedImageToBGRImage( const unsigned char *YUV, IplImage *pImageBGR, bool bDebug = false );
    void convertYUV422PackedImageToBGRImage2( const unsigned char *YUV, IplImage *pImageBGR, bool bDebug = false );
    void convertBayerBG2ImageToBGRImage( const unsigned char *YUV, IplImage *pImageBGR, bool bDebug = false );
    void convertYCrCbImageToBGRImage( const unsigned char *YUV, IplImage *pImageBGR, bool bDebug = false );
};
#endif //#ifndef CAMERABASLER_H_
