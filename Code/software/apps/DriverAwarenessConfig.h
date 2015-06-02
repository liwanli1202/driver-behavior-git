/*
 * ClassConfig.h
 * Ramesh Marikhu < marikhu@gmail.com >
 */

#ifndef CLASSCONFIG_H_
#define CLASSCONFIG_H_

#define GET_TIMESTAMP_BASED_ON_FPS				false //true//
#define VIDEO_FRAME_RATE						30 //fps

#define TA_DEBUG_MODE                           false

// NOTE: Height is fixed but width will vary as per aspect ratio
// If the size of the image is less than resized value, then the actual image
// will be used.
#define TA_RESIZE_IMAGE							false //true
#define TA_ROI_IMAGE_HEIGHT_RESIZED          	240
#define TA_VIDEO_IMAGE_HEIGHT_RESIZED        	480

// No. of frames to save should be dynamic, per the scenario at hand
#define TA_SAVE_FRAMES_AS_PER_CRITERIA          true //false //
#define TA_FRAMERATE_IN_VIDEO_CLIP              20

// Increasing this value could just miss the subsequent violations
// Also, it is essential that it is 1, so that one of the frames will have good
// snapshot of the license plate
#define TA_NUM_FRAMES_SKIP_INTERVAL             0       //1

// Write timestamp on image // Accurate to 1 sec
#define TA_WRITE_TIMESTAMP_ON_HD_IMAGE          1

// Detection and Processing areas (Set to false by default - Restricts to 1 violation area and 1 processing area)
#define TA_ENABLE_MULTIPLE_PROCESSING_AREAS     false

// Blob Filtering
#define TA_MIN_FILTER_PERIMETER                 2        // 320 x 240
#define TA_MAX_FILTER_PERIMETER                 20
#define TA_MIN_FILTER_AREA                      5
#define TA_MAX_FILTER_AREA                      60

// Enable selecting first frame
#define TA_ENABLE_SELECTING_FIRST_FRAME         false   // SET FALSE
#define TA_ENABLE_SKIPPING_FRAMES_MANUALLY      false
#define TA_ENABLE_PAUSE                         true
#define TA_ENABLE_SLOWDOWN                      true

#define TA_DEPLOY                               false    //true //
#if TA_DEPLOY
#   ifdef TA_DISPLAY_IMAGES
#       undef TA_DISPLAY_IMAGES
#       define TA_DISPLAY_IMAGES                false
#   endif
#   ifdef TA_ENABLE_PAUSE
#       undef TA_ENABLE_PAUSE
#       define TA_ENABLE_PAUSE                  false
#   endif
#   ifdef TA_ENABLE_SKIPPING_FRAMES_MANUALLY
#       undef TA_ENABLE_SKIPPING_FRAMES_MANUALLY
#       define TA_ENABLE_SKIPPING_FRAMES_MANUALLY   false
#   endif
#endif

#endif // #ifndef CLASSCONFIG_H_
