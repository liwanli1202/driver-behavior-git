/*
 * CameraEmulatorConfig.h
 * Ramesh Marikhu < marikhu@gmail.com >
 */

#ifndef CAMERAEMULATORCONFIG_H_
#define CAMERAEMULATORCONFIG_H_

#define EMULATE_CAMERA_USB                  0
#define EMULATE_CAMERA_BASLER               1
#define EMULATE_CAMERA_IQEYE                2

#define USE_EMULATOR                        EMULATE_CAMERA_USB
//#define USE_EMULATOR                        EMULATE_CAMERA_BASLER
//#define USE_EMULATOR                        EMULATE_CAMERA_IQEYE

#if USE_EMULATOR == EMULATE_CAMERA_USB
#   define EMULATOR_NUM_MAX_ROWS             480
#   define EMULATOR_NUM_MAX_COLS             640
#   define EMULATOR_NUM_ROWS                 480
#   define EMULATOR_NUM_COLS                 640
#   define EMULATOR_DEF_FOCAL_X              709.86837f
#   define EMULATOR_DEF_FOCAL_Y              707.82411f
#   define EMULATOR_DEF_CENTER_X             303.0f
#   define EMULATOR_DEF_CENTER_Y             201.0f
#   define EMULATOR_DIST_COEFFS_K1           0.3f    //Check: Values obtained from AR Toolkit has been divided by 1000 for all distortion coefficients
#   define EMULATOR_DIST_COEFFS_K2           0.1945f
#   define EMULATOR_DIST_COEFFS_P1           0.0028f
#   define EMULATOR_DIST_COEFFS_P2           0.001010944f
#elif USE_EMULATOR == EMULATE_CAMERA_BASLER
#   define EMULATOR_NUM_MAX_ROWS             492
#   define EMULATOR_NUM_MAX_COLS             656
#   define EMULATOR_NUM_ROWS                 492
#   define EMULATOR_NUM_COLS                 656
#   define EMULATOR_DEF_FOCAL_X              0.0f
#   define EMULATOR_DEF_FOCAL_Y              0.0f
#   define EMULATOR_DEF_CENTER_X             0.0f
#   define EMULATOR_DEF_CENTER_Y             0.0f
#   define EMULATOR_DIST_COEFFS_K1           0.0f
#   define EMULATOR_DIST_COEFFS_K2           0.0f
#   define EMULATOR_DIST_COEFFS_P1           0.0f
#   define EMULATOR_DIST_COEFFS_P2           0.0f
#elif USE_EMULATOR == EMULATE_CAMERA_IQEYE
#   define EMULATOR_NUM_MAX_ROWS             480
#   define EMULATOR_NUM_MAX_COLS             720
#   define EMULATOR_NUM_ROWS                 480
#   define EMULATOR_NUM_COLS                 720
#   define EMULATOR_DEF_FOCAL_X              0.0f
#   define EMULATOR_DEF_FOCAL_Y              0.0f
#   define EMULATOR_DEF_CENTER_X             0.0f
#   define EMULATOR_DEF_CENTER_Y             0.0f
#   define EMULATOR_DIST_COEFFS_K1           0.0f
#   define EMULATOR_DIST_COEFFS_K2           0.0f
#   define EMULATOR_DIST_COEFFS_P1           0.0f
#   define EMULATOR_DIST_COEFFS_P2           0.0f
#endif

#endif // #ifndef CAMERAEMULATORCONFIG_H_
