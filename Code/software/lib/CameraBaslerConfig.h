/*
 * CameraBaslerConfig.h
 * Ramesh Marikhu <marikhu@gmail.com> Oct 2011
 */

#ifndef CAMERABASLERCONFIG_H_
#define CAMERABASLERCONFIG_H_

#define USE_BASLER_ACE_A640GC			0
#define USE_BASLER_ACE_A1300GC			1

#if 0
#	define USE_BASLER					USE_BASLER_ACE_A1300GC
#	define FRAMES_PER_SEC				30 //60   // can acquire higher fps with cropped image
#else
#	define USE_BASLER					USE_BASLER_ACE_A640GC
#	define FRAMES_PER_SEC				30 //100 //
#endif

#if USE_BASLER == USE_BASLER_ACE_A1300GC
#	define BASLER_NUM_MAX_ROWS        	964        //PylonAppViewer show 1278 x 958
#	define BASLER_NUM_MAX_COLS         	1294
#	define BASLER_NUM_ROWS         		480      //964 //
#	define BASLER_NUM_COLS           	640     //1294 //
#   define BASLER_OFFSET_X              0
#   define BASLER_OFFSET_Y              0
#	define BASLER_DEF_FOCAL_X          	0.0f
#	define BASLER_DEF_FOCAL_Y         	0.0f
#	define BASLER_DEF_CENTER_X         	0.0f
#	define BASLER_DEF_CENTER_Y         	0.0f
#	define BASLER_DIST_COEFFS_K1       	0.0f
#	define BASLER_DIST_COEFFS_K2       	0.0f
#	define BASLER_DIST_COEFFS_P1       	0.0f
#	define BASLER_DIST_COEFFS_P2       	0.0f
#elif USE_BASLER == USE_BASLER_ACE_A640GC
#	define BASLER_NUM_MAX_ROWS         	492
#	define BASLER_NUM_MAX_COLS         	658
#   if 1
#       define BASLER_OFFSET_X              0
#       define BASLER_OFFSET_Y              0
#       define BASLER_NUM_ROWS              480 //492   //
#       define BASLER_NUM_COLS              640 //658   //
#   else
#       define BASLER_OFFSET_X              100
#       define BASLER_OFFSET_Y              100
#       define BASLER_NUM_ROWS              200
#       define BASLER_NUM_COLS              300
#   endif
#	define BASLER_DEF_FOCAL_X          	2936.9143692443340f
#	define BASLER_DEF_FOCAL_Y         	2926.0102223408812f
#	define BASLER_DEF_CENTER_X         	309.01992860567253f
#	define BASLER_DEF_CENTER_Y         	252.31052969957241f
#	define BASLER_DIST_COEFFS_K1       	0.4341734738567579f
#	define BASLER_DIST_COEFFS_K2       	-15.03654214421398f
#	define BASLER_DIST_COEFFS_P1       	0.0404438538475120f
#	define BASLER_DIST_COEFFS_P2       	0.0017918441623388f
#endif

typedef enum teAcquisitionMode{ ACQ_CONT, ACQ_FRAME } teAcquisitionMode;
typedef enum teExposureMode{ EXPO_AUTO_CONT, EXPO_AUTO_ONCE, EXPO_RAW } teExposureMode;
typedef enum teGainMode{ GAIN_AUTO_CONT, GAIN_AUTO_ONCE, GAIN_RAW } teGainMode;

#if 0
#	define PIXEL_FORMAT					PixelFormat_Mono8
#elif 0
#	define PIXEL_FORMAT					PixelFormat_BayerBG8
#else
#	define PIXEL_FORMAT					PixelFormat_YUV422Packed
#endif

#define ACQUISITION_MODE            	ACQ_CONT
#define EXPOSURE_MODE               	EXPO_AUTO_CONT //EXPO_RAW //EXPO_RAW //EXPO_AUTO_ONCE
#define EXPO_RAW_INIT					8000 //20000 //1500 //10000 // Fixed for 30fps, Change aperture to control light
#define GAIN_MODE                   	GAIN_AUTO_CONT //GAIN_RAW //GAIN_AUTO_ONCE //
#define GAIN_RAW_INIT					300 //50 //

#define PYLON_AUTO_GAIN_CONTROL			1
#if PYLON_AUTO_GAIN_CONTROL
//   NOTE: Set GAIN_MODE = GAIN_AUTO_CONT, EXPO_MODE = EXPO_AUTO_CONT
#	define CUSTOM_AUTO_GAIN_CONTROL		false
#	define UPDATE_CUSTOM_GAIN			false 	// depends on CUSTOM_AUTO_GAIN_CONTROL = true
#	define UPDATE_CUSTOM_EXPOSURE		false 	// depends on CUSTOM_AUTO_GAIN_CONTROL = true
#else
//   NOTE: Set GAIN_MODE = GAIN_RAW, EXPO_MODE = EXPO_RAW
#	define CUSTOM_AUTO_GAIN_CONTROL		true	// Not that effective, scene dependent.
#	define UPDATE_CUSTOM_GAIN			true	// depends on CUSTOM_AUTO_GAIN_CONTROL = true
#	define UPDATE_CUSTOM_EXPOSURE		true	// depends on CUSTOM_AUTO_GAIN_CONTROL = true
#endif
#define MAX_FRAMES_PYLON_GAIN_AUTO_ONCE	200
#define MAX_FRAMES_PYLON_EXPO_AUTO_ONCE	200

// If NUM_BUFFERS > 1, CUSTOM_AUTO_GAIN_CONTROL does not work well because the custom gain is calculated in an image from
// buffer whereas the gain values are written into the camera for the next capture which is written to buffer.
// There may be > 1 frame difference between the current frame acquired from the buffer and the frame written to buffer.
// This causes CUSTOM_AUTO_GAIN_CONTROL computation to be ineffective for the currently grabbed frame.
#define NUM_BUFFERS                 	2//8		// Response is faster with NUM_BUFFERS = 2 than with 8 .
                                                    // CHECK: Buffering is found to be faster

// NOTE: For color images, if the packet size is set to 572 ( by default ), the frame rate is about 5 fps.
// The dropped frames result in error: Failed to grab image:GX status 0xe1000014
// The Basler Pylon documentation states that for better capture, jumbo frames must be enabled.
// The packet size need to be set to 8192, BUT in my X61, the maximum value allowed is 1504 which gives good results.
// To change packet size from /opt/pylon/bin/PylonViewerApp, go to
// Choose User Level > Guru and then browse through Basler acA640-100gc > Transport Layer > Packet Size
#define PACKET_SIZE                     1504    // Tested to be maximum value when run in Ubuntu
//#define PACKET_SIZE                     1500    // When using wifi via ASUS GigE Router, also MTU set to 2250 but no difference
//#define PACKET_SIZE                     1980    // Works: when using LAN connected to ASUS GigE Router, and MTU set to 2250

// CUSTOM_AUTO_GAIN_CONTROL
#define PARTIAL_COMPUTATION  			true	// For speed, computation is performed by skipping row and column intervals
#define SKIP_INTERVAL					5
#define	CONTRAST_THRESHOLD_MIN			20
#define CONTRAST_THRESHOLD_MAX			30
#define BRIGHTNESS_THRESHOLD_MIN		80
#define BRIGHTNESS_THRESHOLD_MAX		100
#define EXPOSURE_INTVL                  500
#define GAIN_INTVL                      25
#define USE_AUTO_GAIN_WEIGHTS           false

// PYLON_AUTO_GAIN_CONTROL and CUSTOM_AUTO_GAIN_CONTROL
#define GAIN_MIN						10		// Actual min = 0.
#define GAIN_MAX						800		// Actual max = 1023. Set specified value as max gain to reduce noise amplification
#define EXPOSURE_MIN					100	// Actual min = 16.
#define EXPOSURE_MAX					33000 	// Actual max = 1e+006. Set specified value as max exposure to ensure 30fps
#define APERTURE_SIZE					2		// Set manually on camera, used for testing.

#endif // ifndef CAMERABASLERCONFIG_H_
