/*
 * CameraIQEyeConfig.h
 */

#ifndef CAMERAIQEYECONFIG_H_
#define CAMERAIQEYECONFIG_H_

// Image resolution
#define IQEYE_IMAGE_RES_1080P				0
#define IQEYE_IMAGE_RES_720P				1
#define IQEYE_IMAGE_RES_480P				2
#define IQEYE_IMAGE_RES						IQEYE_IMAGE_RES_1080P
//#define IQEYE_IMAGE_RES						IQEYE_IMAGE_RES_720P
//#define IQEYE_IMAGE_RES						IQEYE_IMAGE_RES_480P

// Manual zoom
#define IQEYE_F3_5							0
#define IQEYE_F9							1
#define IQEYE_F3_5x							2
#define IQEYE_F8_9x							3
//#define IQEYE_ZOOM							IQEYE_F3_5
//#define IQEYE_ZOOM						  IQEYE_F9
#define IQEYE_ZOOM						  IQEYE_F3_5x
//#define IQEYE_ZOOM						  IQEYE_F8_9x

// Intrinsic parameters
#if IQEYE_IMAGE_RES	== IQEYE_IMAGE_RES_1080P
//#	define IQEYE_NUM_ROWS            			1080
//#	define IQEYE_NUM_COLS            			1920
#	define IQEYE_DEF_FOCAL_X         			0
#	define IQEYE_DEF_FOCAL_Y        			0
#	define IQEYE_DEF_CENTER_X        			0
#	define IQEYE_DEF_CENTER_Y        			0
#	define IQEYE_DIST_COEFFS_K1      			0
#	define IQEYE_DIST_COEFFS_K2      			0
#	define IQEYE_DIST_COEFFS_P1      			0
#	define IQEYE_DIST_COEFFS_P2					0
#elif IQEYE_IMAGE_RES	== IQEYE_IMAGE_RES_720P
#	define IQEYE_NUM_ROWS            			720
#	define IQEYE_NUM_COLS            			1280
#	if IQEYE_ZOOM ==  IQEYE_F3_5
#		define IQEYE_DEF_FOCAL_X         			1090.1169f
#		define IQEYE_DEF_FOCAL_Y        			1095.1127f
#		define IQEYE_DEF_CENTER_X        			606.11944f
#		define IQEYE_DEF_CENTER_Y        			333.8626f
#		define IQEYE_DIST_COEFFS_K1      			-0.453061f
#		define IQEYE_DIST_COEFFS_K2      			0.65796f
#		define IQEYE_DIST_COEFFS_P1      			0.00116106f
#		define IQEYE_DIST_COEFFS_P2					-0.001943879f
#	elif IQEYE_ZOOM ==  IQEYE_F9
#		define IQEYE_DEF_FOCAL_X         			2430.20277f
#		define IQEYE_DEF_FOCAL_Y        			1657.07448f
#		define IQEYE_DEF_CENTER_X        			589.86334f
#		define IQEYE_DEF_CENTER_Y        			353.5468f
#		define IQEYE_DIST_COEFFS_K1      			-0.3660932f
#		define IQEYE_DIST_COEFFS_K2      			-0606705f
#		define IQEYE_DIST_COEFFS_P1      			-0.0037755f
#		define IQEYE_DIST_COEFFS_P2					0.001151f
#	elif IQEYE_ZOOM ==  IQEYE_F3_5x
#		define IQEYE_DEF_FOCAL_X         			1105.4457f
#		define IQEYE_DEF_FOCAL_Y        			1110.3506f
#		define IQEYE_DEF_CENTER_X        			602.4329f
#		define IQEYE_DEF_CENTER_Y        			351.1319f
#		define IQEYE_DIST_COEFFS_K1      			-0.4284232f
#		define IQEYE_DIST_COEFFS_K2      			0.4234528f
#		define IQEYE_DIST_COEFFS_P1      			-0.01314683f
#		define IQEYE_DIST_COEFFS_P2					0.000214998f
#	elif IQEYE_ZOOM ==  IQEYE_F8_9x
#		define IQEYE_DEF_FOCAL_X         			2359.48f
#		define IQEYE_DEF_FOCAL_Y        			2376.086f
#		define IQEYE_DEF_CENTER_X        			565.7705f
#		define IQEYE_DEF_CENTER_Y        			410.768f
#		define IQEYE_DIST_COEFFS_K1      			-0.243127f
#		define IQEYE_DIST_COEFFS_K2      			-2.59253f
#		define IQEYE_DIST_COEFFS_P1      			-0.000000158f
#		define IQEYE_DIST_COEFFS_P2					-0.004361314f
#	endif
#elif IQEYE_IMAGE_RES	== IQEYE_IMAGE_RES_480P
#	define IQEYE_NUM_ROWS            			480
#	define IQEYE_NUM_COLS            			720
#	if IQEYE_ZOOM ==  IQEYE_F3_5x
#		define IQEYE_DEF_FOCAL_X         			621.11903f
#		define IQEYE_DEF_FOCAL_Y        			747.71951f
#		define IQEYE_DEF_CENTER_X        			340.63292f
#		define IQEYE_DEF_CENTER_Y        			246.00951f
#		define IQEYE_DIST_COEFFS_K1      			-0.39422f
#		define IQEYE_DIST_COEFFS_K2      			0.21484f
#		define IQEYE_DIST_COEFFS_P1      			-0.00080f
#		define IQEYE_DIST_COEFFS_P2					-0.00046f
#	elif IQEYE_ZOOM ==  IQEYE_F8_9x
#		define IQEYE_DEF_FOCAL_X         			1378.46186f
#		define IQEYE_DEF_FOCAL_Y        			1657.07448f
#		define IQEYE_DEF_CENTER_X        			330.10710f
#		define IQEYE_DEF_CENTER_Y        			238.94784f
#		define IQEYE_DIST_COEFFS_K1      			-0.32987f
#		define IQEYE_DIST_COEFFS_K2      			-0.19265f
#		define IQEYE_DIST_COEFFS_P1      			-0.00223f
#		define IQEYE_DIST_COEFFS_P2					-0.00014f
#	endif
#endif

#define IQEYE_EXPO_RAW_INIT					20000
#define IQEYE_GAIN_RAW_INIT					300

#define	IQEYE_CONTRAST_THRESHOLD_MIN		20
#define IQEYE_CONTRAST_THRESHOLD_MAX		30
#define IQEYE_BRIGHTNESS_THRESHOLD_MIN		80
#define IQEYE_BRIGHTNESS_THRESHOLD_MAX		100
#define IQEYE_GAIN_MIN						100
#define IQEYE_GAIN_MAX						1023
#define IQEYE_EXPOSURE_MIN					16
#define IQEYE_EXPOSURE_MAX					1e+006
#define IQEYE_APERTURE_SIZE					2	// Set manually on camera, used for testing.
#define IQEYE_EXPOSURE_INTVL				500
#define IQEYE_GAIN_INTVL					25

#endif //CAMERAIQEYECONFIG_H_
