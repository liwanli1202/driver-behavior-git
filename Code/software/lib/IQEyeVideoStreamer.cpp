/*
 * IQEyeVideoStreamer.cpp
 * PoliceEyes project
 * Class created on: Oct 27, 2011
 * Jednipat Moonrinta <sir.ivy@gmail.com>
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#include "IQEyeVideoStreamer.h"

int IQEyeVideoStreamer::_iObjCount = 0;
ErrorHandler* IQEyeVideoStreamer::_pErrorHandler = NULL;
Logger* IQEyeVideoStreamer::_pLogger = NULL;
bool IQEyeVideoStreamer::_bIsSetErrorDiagnosticsList = false;

bool IQEyeVideoStreamer::_bImageIsReady[] = { false, false, false };
bool IQEyeVideoStreamer::_bStopStreaming[] = { false, false, false };

Timer* IQEyeVideoStreamer::_pTimer[] = { new Timer(), new Timer(), new Timer() };
timeval IQEyeVideoStreamer::_tvImageAcquisitionTime[] = { 0, 0, 0 };
IplImage* IQEyeVideoStreamer::_pImage[] = { NULL, NULL, NULL };
int IQEyeVideoStreamer::_iImageWidth[] = { -1, -1, -1 };
int IQEyeVideoStreamer::_iImageHeight[] = { -1, -1, -1 };

IQEyeVideoStreamer::IQEyeVideoStreamer(int iStreamerID,
        const char *pchInputVideoStream)
{
    _iStreamerID = iStreamerID;
    strcpy(_pchInputVideoStream[iStreamerID], pchInputVideoStream);
}

IQEyeVideoStreamer::~IQEyeVideoStreamer()
{
    // TODO: Release allocated memory
    IplImage *pImage = _pImage[_iStreamerID];
    if (pImage) cvReleaseImage(&pImage);
}

void IQEyeVideoStreamer::setImageResolution(int iStreamerID, int iWidth,
        int iHeight)
{
    IQEyeVideoStreamer::_iImageWidth[iStreamerID] = iWidth;
    IQEyeVideoStreamer::_iImageHeight[iStreamerID] = iHeight;
}

void IQEyeVideoStreamer::initiateLibVLC(int iStreamerID)
{
    _pContext[iStreamerID] = (struct ctx*)malloc(
            sizeof(*_pContext[iStreamerID]));
    int iImageWidth = _iImageWidth[iStreamerID];
    int iImageHeight = _iImageHeight[iStreamerID];
    _pContext[iStreamerID]->pImage = cvCreateImage(
            cvSize(iImageWidth, iImageHeight), IPL_DEPTH_8U, 4);
    _pContext[iStreamerID]->puchPixels =
            (unsigned char*)_pContext[iStreamerID]->pImage->imageData;
    _pContext[iStreamerID]->iStreamerID = iStreamerID;

    // Load the VLC engine
    string sStreamString = string(_pchInputVideoStream[iStreamerID]);
    std::size_t foundRTSP = sStreamString.find(string("rtsp"));
    cout << endl;
    if (foundRTSP!=std::string::npos)
    {
        cout << "RTSP stream: H264" << endl;
        const char * const vlc_args[] = { "--rtsp-tcp"};
        _pLibvlc_instance[iStreamerID] = libvlc_new(
                sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    }
    else
    {
        cout << "HTTP stream: MJPEG" << endl;
        _pLibvlc_instance[iStreamerID] = libvlc_new(0,NULL);
    }

    // Create a new item
    // Need to be able to choose the appropriate stream if multiple IQEye cameras are used
    _pLibvlc_media[iStreamerID] = libvlc_media_new_location(
            _pLibvlc_instance[iStreamerID], _pchInputVideoStream[iStreamerID]);

    // Create a media player playing environment
    _pLibvlc_media_player[iStreamerID] = libvlc_media_player_new_from_media(
            _pLibvlc_media[iStreamerID]);
}

void IQEyeVideoStreamer::startVideoStreaming(int iStreamerID)
{
    libvlc_video_set_callbacks(_pLibvlc_media_player[iStreamerID], lock, unlock,
            display, _pContext[iStreamerID]);
    int iImageWidth = _iImageWidth[iStreamerID];
    int iImageHeight = _iImageHeight[iStreamerID];
    libvlc_video_set_format(_pLibvlc_media_player[iStreamerID], "RV32",
            iImageWidth, iImageHeight, iImageWidth * 4); // Image has 4 channels
    libvlc_media_player_play(_pLibvlc_media_player[iStreamerID]);

    /*
     * NOTE
     *
     * libvlc_video_set_callbacks create a thread that executes lock->display->unlock sequence continuously.
     * Another thread is being executed right after the libvl_media_player_play().
     * This thread is made to execute continuously in mutual exclusion with lock->display->unlock.
     * Here, pthread is used for mutual exclusion (mutex).
     * An infinite loop is usually used for mutex to execute a specific functionality in the loop.
     * Currently, to execute processFrame() from outside of this class, the infinite loop
     * is specified outside this class, in the flow that invokes startVideoStreaming().
     * An example is provided below for multithreading managed by mutex.
     * For this example, pthread_mutex_lock( &mutex ) is specified inside lock() block and
     * pthread_mutex_unlock( &mutex ) is specified inside unlock() block.
     * However, now, the current implementation does not use pthread but instead sets the
     * current image in the display() block. The execution pattern is lock->display->unlock.

     #if 0       // NOTE: This functionality has been moved to the invoking function.
     //NOTE: Ctrl + Shift + F formatting removed the format of the commented lines for the snippet below
     while(true)
     {
     pthread_mutex_lock( &mutex );
     if( _bImageIsReady )
     {
     cout << "Processing started..." << endl;
     _iFrameCount++;
     cout << "_iFrameCount = " << _iFrameCount << endl;
     _bImageIsReady = false;
     cout << "Processing complete." << endl;
     }
     pthread_mutex_unlock( &mutex );
     if( !_bImageIsReady )
     {
     usleep( 100 );
     }
     }
     #endif
     */
}

void IQEyeVideoStreamer::stopVideoStreaming(int iStreamerID)
{
    // Stop playing
    libvlc_media_player_stop(_pLibvlc_media_player[iStreamerID]);
    libvlc_media_player_release(_pLibvlc_media_player[iStreamerID]); // Free the media_player
    libvlc_release(_pLibvlc_instance[iStreamerID]);
}

// == Getters and Setters ==

void IQEyeVideoStreamer::setImageIsReady(int iStreamerID, bool bImageIsReady)
{
    _bImageIsReady[iStreamerID] = bImageIsReady;
}

bool IQEyeVideoStreamer::getImageIsReady(int iStreamerID)
{
    return _bImageIsReady[iStreamerID];
}

void IQEyeVideoStreamer::setStopStreaming(int iStreamerID, bool bStopStreaming)
{
    _bStopStreaming[iStreamerID] = bStopStreaming;
}

IQEyeVideoStreamer::ctx* IQEyeVideoStreamer::getContext(int iStreamerID)
{
    return _pContext[iStreamerID];
}

timeval IQEyeVideoStreamer::getImageAcquisitionTime(int iStreamerID)
{
    return _tvImageAcquisitionTime[iStreamerID];
}

IplImage* IQEyeVideoStreamer::getImage(int iStreamerID)
{
    return _pImage[iStreamerID];
}

// == Protected functions ==

void* IQEyeVideoStreamer::lock(void *data, void**p_pixels)
{
    // TODO: Check if the buffer is the same as the old buffer
    // Is it that the buffer is locked when tried to be written to,
    // because of which when accessed the second time (when it is free to access),
    // it is still the same content
    struct ctx *ctx = (struct ctx*)data;
    int iStreamerID = ctx->iStreamerID;
    //cout << "iStreamerID = " << iStreamerID << endl;
    _tvImageAcquisitionTime[iStreamerID] =
            _pTimer[iStreamerID]->getCurrentTime();
    //cout << "Locking..." << endl;
    if (!_bStopStreaming[iStreamerID])
    {
        //cout << "IQEyeVideoStreamer: lock()" << endl;
        *p_pixels = ctx->puchPixels;
    }
    return NULL;
}

void IQEyeVideoStreamer::display(void *data, void *id)
{
    //cout << "IQEyeVideoStreamer: display() invoked" << endl;
    struct ctx *ctx = (struct ctx*)data;
    IplImage *pImageDisplay = ctx->pImage;
    int iStreamerID = ctx->iStreamerID;
    if (!_bImageIsReady[iStreamerID])
    {
        //cout << "IQEyeVideoStreamer: display(): Image is not ready for display.
        // So, getting reference to data and copying image." << endl;
        if (!_pImage[iStreamerID])
        {
            _pImage[iStreamerID] = cvCreateImage(cvGetSize(pImageDisplay),
                    pImageDisplay->depth, pImageDisplay->nChannels);
        }
        IplImage *pImage = _pImage[iStreamerID];
        //cout << "pImage: " << pImage->width << " x " << pImage->height << endl;
        //cout << "pImageDisplay: " << pImageDisplay->width << " x " << pImageDisplay->height << endl;
        cvCopyImage(pImageDisplay, pImage);
        pImageDisplay = NULL;
        _bImageIsReady[iStreamerID] = true;
        //cout << "IQEyeVideoStreamer: Image is ready for display." << endl;
    }
    else
    {
        //cout << "IQEyeVideoStreamer: Image is not set." << endl;
    }
#if 0
    cout << "Displaying image from video stream..." << endl;
    struct ctx *ctx = (struct ctx*)data;
    IplImage *pImageDisplay = ctx->pImage;
    cout << "pImageDisplay: " << pImageDisplay->width << " x " << pImageDisplay->height <<
    " x " << pImageDisplay->nChannels << " : " << pImageDisplay->imageSize << endl;
    cvNamedWindow( "image from video stream", CV_WINDOW_AUTOSIZE );
    cvShowImage( "image from video stream", pImageDisplay );
    cvWaitKey( 1 );
    pImageDisplay = NULL;
#endif
    (void)data;
    assert(id == NULL);
}

void IQEyeVideoStreamer::unlock(void *data, void *id, void * const *p_pixels)
{
    //cout << "IQEyeVideoStreamer: unlock()" << endl;
    assert(id == NULL);
}

