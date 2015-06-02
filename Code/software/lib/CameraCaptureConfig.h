/*
 * CameraCaptureConfig.h
 */

#ifndef CAMERACAPTURECONFIG_H_
#define CAMERACAPTURECONFIG_H_

typedef enum teCaptureMode
{
    CAMERA, VIDEO, IMAGE_LIST, IMAGE
} teCaptureMode;
static const int iNumFramesAhead = 100;
static const char *pchArrCaptureMode[] = { "CAMERA", "VIDEO", "IMAGE_LIST",
        "IMAGE" };
typedef enum teImageType
{
    MONOCHROME, COLOR, BLUE_CHANNEL
} teImageType;
static const char *pchArrImageType[] = { "MONOCHROME", "COLOR", "BLUE_CHANNEL" };
static const char *pchArrBoolean[] = { "NO", "YES" };
#define CAPTURE_MODE                        CAMERA
#define CAPTURE_MODE2                       CAMERA
#define IMAGE_TYPE                          COLOR //MONOCHROME //
#define INPUT_IMAGE_WILDCARD                "*.jpg|*.bmp|*.png"
typedef enum teCamera
{
    CAMERA_EMULATOR,
    CAMERA_USB,
    CAMERA_IQEYE,
    CAMERA_LILIN,
    CAMERA_BASLER,
    CAMERA_RTSP_STREAM
} teCamera;
static const char *pchArrCamera[] = { "CAMERA_EMULATOR", "CAMERA_USB",
        "CAMERA_IQEYE", "CAMERA_LILIN", "CAMERA_BASLER", "CAMERA_RTSP_STREAM" };

#define CUSTOM_SIZE_OF_GRABBED_IMAGE        true //false	// does not work for Basler if different size specified, need to check why
#define NUM_FRAMES_TO_GRAB				    -1 //    4320000 // Max: 40 hours with 30fps
// Save video
#define VIDEO_FRAMERATE                     30  //10 for lesser frames per second
#define SAVE_IMAGE_EVERY_SEC                0
#define SAVE_VIDEO_AS_GRABBED               false

// Custom_resolve_frame_drops :
// TODO: Do not add dropped frames during capture. Record the frame no. of the dropped frames in text file
// to be added later. This way, lesser frames will be dropped. Will need to update the saved video using the dropped frames file later.
#define WRITE_DROPPED_FRAMES                0       // One way of resolving frame drops in a saved video file
// Custom_resolve_frame_drops
#define WRITE_VIDEO_TIMESTAMP               1       // One way of resolving frame drops in a saved video file
// NOTE: When multi-threading, set WRITE_VIDEO_TIMESTAMP to 0 because
// there will be resource conflict when the same file is being
// used by two or more camera threads
#define CUSTOM_RESOLVE_FRAME_DROPS          0       // Currently, this is feasible only if NUM_BUFFERS = 1
#endif //CAMERACAPTURECONFIG_H_
