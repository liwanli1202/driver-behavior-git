/*
 * Camera.h
 * Class responsible for selecting camera for grabbing images.
 * Camera is an Abstract Class, Interface to Concrete subclasses
 * Generic functions related to all cameras are specified here
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <vector>
using namespace std;

#include <cv.h>
#include <highgui.h>

#include "CameraCaptureConfig.h"
#include "ErrorHandler.h"
#include "Logger.h"
#include "Utilities.h"

class Camera
{
public:
    Camera();
    virtual ~Camera();

    static void initErrorDiagnosticsList();
    static string getOutputFolder();

    virtual void initializeCamera(bool bDebug = false) = 0; // = 0 makes it a pure virtual function
    virtual void grabImage(bool bDebug = false) = 0;
    virtual void closeCamera(bool bDebug = false) = 0;
    virtual bool gotoFrameNo(int iFrameNo) = 0;

    // Getters and Setters
    static int getObjCount();
    void setImage(IplImage *pImage);
    void setImage(char *szImageFile, int iColor);
    IplImage* getImage() const;
    bool getIsSetImage();
    void setImageRectified(IplImage *pImageRectified);
    void setImageRectified(char *szImageFile, int iColor);
    IplImage* getImageRectified();
    bool getIsSetImageRectified();
    void setMatK(CvMat *pMatK);
    CvMat* getMatK();
    bool getIsSetMatK();
    void setMatD(CvMat *pMatD);
    CvMat* getMatD();
    bool getIsSetMatD();

    void setImageResolution(int iWidth, int iHeight, bool bDebug = false);
    void getImageResolution(int &iWidth, int &iHeight);
    int getCaptureMode();
    void setCaptureMode(int iCaptureMode, bool bDebug = false);
    void setCaptureMode(int iCaptureMode, char *pchFileOrFolder,
            bool bFileSizeGt2GB = false, bool bDebug = false);
    bool getIsSetCaptureMode();
    int getImageType();
    void setImageType(int iImageType);
    bool getIsSetImageType();

    void setNumFramesToGrab(int iNumFramesToGrab);
    int getNumFramesToGrab();
    int getGrabbedFrameNo();
    bool getIsGrabComplete();
    bool getStopCapture();
    void setComputeRectifiedImage(bool bComputeRectifiedImage);
    bool getComputeRectifiedImage();

    long getTimeStampOfCaptureInSec();
#ifdef WIN32
    LARGE_INTEGER getTimeOfStartCapture();
    LARGE_INTEGER getTimeOfFrameCapture();
#else
    timeval getTimeOfStartCapture();
    timeval getTimeOfFrameCapture();
#endif

    int getNumFrameCount();

protected:
    Utilities *_pUtilities;
    IplImage _img;
    cv::Mat _matImage;
    IplImage *_pImage;
    IplImage _imgPreview;
    cv::Mat _matImagePreview;
    IplImage *_pImagePreview;
    bool _bIsSetImage;
    IplImage *_pImageRectified;
    bool _bIsSetImageRectified;
    CvMat *_pMatK;
    bool _bIsSetMatK;
    CvMat *_pMatD;
    bool _bIsSetMatD;

    int _iImageWidth;
    int _iImageHeight;
    int _iCaptureMode;
    bool _bIsSetCaptureMode;
    int _iImageType;
    bool _bIsSetImageType;
    char _chArrFile[255];
    char _chArrFolder[255];

    int _iNumFramesToGrab;
    int _iGrabbedFrameNo;

    bool _bComputeRectifiedImage;
    int _iNumFrameCount;

    bool _bIsGrabComplete;
    bool _bStopCapture;
    // Timing information
    long _lTimeStampOfCaptureInSec;
#ifdef WIN32
    LARGE_INTEGER _tvTimeOfStartCapture;
    LARGE_INTEGER _tvTimeOfFrameCapture;
#else
    timeval _tvTimeOfStartCapture;
    timeval _tvTimeOfFrameCapture;
#endif

private:
    static int _iObjCount;
    static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;
    static bool _bIsSetErrorDiagnosticsList;
};

#endif //#ifndef CAMERA_H_
