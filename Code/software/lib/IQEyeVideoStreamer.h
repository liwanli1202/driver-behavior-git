/*
 * IQEyeVideoStreamer.h
 * PoliceEyes project
 * Class created on: Oct 27, 2011
 * Jednipat Moonrinta <sir.ivy@gmail.com>
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#ifndef IQEYEVIDEOSTREAMER_H_
#define IQEYEVIDEOSTREAMER_H_

#include <cv.h>
#include <highgui.h>
#include <opencv2/video/background_segm.hpp>
//using namespace cv;

#include <vector>
using namespace std;

#include <vlc/vlc.h>

#include "Clock.h"
#include "ErrorHandler.h"
#include "CameraCaptureConfig.h"
#include "CameraIQEyeConfig.h"
#include "Logger.h"
#include "Timer.h"
#include "Utilities.h"

#define MAX_NUM_CAMERAS             3
class IQEyeVideoStreamer
{
public:
    struct ctx
    {
        IplImage *pImage;
        uchar *puchPixels;
        int iStreamerID;
    };

    IQEyeVideoStreamer(int iStreamerID, const char *pchInputVideoStream);
    ~IQEyeVideoStreamer();

    void setImageResolution(int iStreamerID, int iWidth, int iHeight);
    void initiateLibVLC(int iStreamerID);
    void startVideoStreaming(int iStreamerID);
    void stopVideoStreaming(int iStreamerID);

    // Getters and Setters
    void setImageIsReady(int iStreamerID, bool bImageIsReady);
    bool getImageIsReady(int iStreamerID);
    void setStopStreaming(int iStreamerID, bool bStopStreaming);
    ctx* getContext(int iStreamerID);
    timeval getImageAcquisitionTime(int iStreamerID);

    IplImage* getImage(int iStreamerID);

protected:
    static bool _bImageIsReady[MAX_NUM_CAMERAS];
    static bool _bStopStreaming[MAX_NUM_CAMERAS];

    // libvlc variables
    libvlc_instance_t *_pLibvlc_instance[MAX_NUM_CAMERAS];
    libvlc_media_player_t *_pLibvlc_media_player[MAX_NUM_CAMERAS];
    libvlc_media_t *_pLibvlc_media[MAX_NUM_CAMERAS];

    ctx *_pContext[MAX_NUM_CAMERAS];

    // Define callbacks for libvlc
    // NOTE: The lock, display and unlock functions need to be static
    // as is required by libvlc_video_lock_cb, libvlc_video_display_cb
    // and libvlc_video_unlock_cb in libvlc_video_set_callbacks()
    static void* lock(void *data, void **p_pixels);
    static void display(void *data, void *id);
    static void unlock(void *data, void *id, void * const *p_pixels);

private:
    static int _iObjCount;
    static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;
    static bool _bIsSetErrorDiagnosticsList;

    static Timer *_pTimer[MAX_NUM_CAMERAS];
    static timeval _tvImageAcquisitionTime[MAX_NUM_CAMERAS];
    static IplImage *_pImage[MAX_NUM_CAMERAS];
    static int _iImageWidth[MAX_NUM_CAMERAS];
    static int _iImageHeight[MAX_NUM_CAMERAS];

    char _pchInputVideoStream[MAX_NUM_CAMERAS][255];
    int _iStreamerID;
};

#endif /* IQEYEVIDEOSTREAMER_H_ */
