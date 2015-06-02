/*
 * CameraUSBConfig.h
 * Ramesh Marikhu < marikhu@gmail.com >
 */

#ifndef CAMERAUSBCONFIG_H_
#define CAMERAUSBCONFIG_H_

#define USB_OKER						0
#define USB_LOGITECH					1
#define USB_MOCK_BASLER					2		// For IMAGE_LIST

//#define USE_USB_CAMERA					USB_OKER
//#define USE_USB_CAMERA					USB_LOGITECH
#define USE_USB_CAMERA					USB_MOCK_BASLER


#if USE_USB_CAMERA == USB_OKER
#	define USB_NUM_MAX_ROWS        		640
#	define USB_NUM_MAX_COLS         	480
#	define USB_NUM_ROWS            		640
#	define USB_NUM_COLS            		480
#	define USB_DEF_FOCAL_X         		709.86837f
#	define USB_DEF_FOCAL_Y        		707.82411f
#	define USB_DEF_CENTER_X        		303.0f
#	define USB_DEF_CENTER_Y        		201.0f
#	define USB_DIST_COEFFS_K1      		0.3f	//Values obtained from AR Toolkit has been divided by 1000 for all distortion coefficients
#	define USB_DIST_COEFFS_K2      		0.1945f
#	define USB_DIST_COEFFS_P1      		0.0028f
#	define USB_DIST_COEFFS_P2      		0.001010944f
#elif USE_USB_CAMERA == USB_LOGITECH
#	define USB_NUM_MAX_ROWS        		640
#	define USB_NUM_MAX_COLS         	480
#	define USB_NUM_ROWS            		640
#	define USB_NUM_COLS            		480
#	define USB_DEF_FOCAL_X         		702.1948f
#	define USB_DEF_FOCAL_Y        		704.2145f
#	define USB_DEF_CENTER_X        		309.26496f
#	define USB_DEF_CENTER_Y        		237.9329f
#	define USB_DIST_COEFFS_K1      		0.03568677f	//Values obtained from AR Toolkit has been divided by 1000 for all distortion coefficients
#	define USB_DIST_COEFFS_K2      		-0.8001177f
#	define USB_DIST_COEFFS_P1      		-0.00250854f
#	define USB_DIST_COEFFS_P2      		0.00227859f
#elif  USE_USB_CAMERA == USB_MOCK_BASLER
#	define USB_NUM_MAX_ROWS        		656
#	define USB_NUM_MAX_COLS         	492
#	define USB_NUM_ROWS            		656
#	define USB_NUM_COLS            		492
#	define USB_DEF_FOCAL_X         		0.0f
#	define USB_DEF_FOCAL_Y        		0.0f
#	define USB_DEF_CENTER_X        		0.0f
#	define USB_DEF_CENTER_Y        		0.0f
#	define USB_DIST_COEFFS_K1      		0.0f
#	define USB_DIST_COEFFS_K2      		0.0f
#	define USB_DIST_COEFFS_P1      		0.0f
#	define USB_DIST_COEFFS_P2      		0.0f
#endif
#define USB_EXPO_RAW_INIT				20000
#define USB_GAIN_RAW_INIT				300

#define	USB_CONTRAST_THRESHOLD_MIN		20
#define USB_CONTRAST_THRESHOLD_MAX		30
#define USB_BRIGHTNESS_THRESHOLD_MIN	80
#define USB_BRIGHTNESS_THRESHOLD_MAX	100
#define USB_GAIN_MIN					100
#define USB_GAIN_MAX					1023
#define USB_EXPOSURE_MIN				16
#define USB_EXPOSURE_MAX				1e+006
#define USB_APERTURE_SIZE				2	// Set manually on camera, used for testing.
#define USB_EXPOSURE_INTVL				500
#define USB_GAIN_INTVL					25

#endif //CAMERAUSBCONFIG_H_
