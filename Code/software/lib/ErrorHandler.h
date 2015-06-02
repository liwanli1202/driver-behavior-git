/*
 * ErrorHandler.h
 */

#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
using namespace std;

#include "ErrorCodeDefs.h"


class ErrorHandler
{
public:
    class Exception : public std::runtime_error
    {
    public:
        Exception( const char *szMsg, int iErrorCode ) : std::runtime_error( szMsg )
        {
            _iErrorCode = iErrorCode;
            _sErrorMsg = string( szMsg );
        }
        int getErrorCode()
        {
            return _iErrorCode;
        }
        string getErrorMsg()
        {
            return _sErrorMsg;
        }
        ~Exception() throw(){}
    private:
        int _iErrorCode;
        string _sErrorMsg;
    };

    static ErrorHandler* getInstance()
    {
        if( !_pErrorHandler )
            _pErrorHandler = new ErrorHandler;
        return _pErrorHandler;
    }

    void initErrorDiagnosticsList();
    void insertErrorDiagnostics( int iErrorCode, string sErrorString );
    string getErrorString();
    void displayErrorMsg();

    // Getters and Setters
    static int getObjCount();
    void setErrorCode( int iErrorCode, string sAppendedText = string( "" ) );
    int getErrorCode() const;
    int getLenOfErrorString( int iErrorCode );
    string getErrorString( int iErrorCode );

protected:
    ErrorHandler();
    ~ErrorHandler();

private:
    static int _iObjCount;
    static ErrorHandler *_pErrorHandler;
    static bool _bErrorDiagListIsSet;
    int _iErrorCode;
    string _sAppendedText;
    map<int, string> _mapError;
    map<int, string>::iterator _mapIter;
    std::stringstream _sOut;
};
#endif // #ifndef ERRORHANDLER_H_


/**
ErrorHandler            			1XX
Utilities               			2XX
Clock                   			3XX
Logger                  			4XX
Timer								5XX
CoordinateTransformation			6XX
CameraCalibExtrinsic				7XX
IQEye								8XX
UsbOker                             9XX
CameraAccessor						10XX
Camera						        11XX
FrameGrabber						12XX
CameraUSB							13XX
CameraIQEye							14XX
FgAccessor							15XX
BgSubtractor						16XX
BgSubtractorOpenCVFGDStat			17XX
BgSubtractorOpenCVGaussianBG	    18XX
BgSubtractorOpenCVCodebook		    19XX
BgSubtractorNECTECStat				20XX
CameraCalibIntrinsic				21XX
BgSubtractorOpenCVMOG				22XX
BgSubtractorOpenCVMOG2				23XX
BgSubtractorZivkovicGMM				24XX
LaneDetector						25XX
BgSubtractorPoppeGMM				26XX
CameraBasler						27XX
LaneViolationDetector				28XX
BlobExtractor						29XX
ShadowDetector						30XX
CameraEmulator                      31XX
ShapeSelector                       32XX
SpeedEstimator                      33XX
VanishingPointDetector              34XX
FeatureTracker                      35XX
VideoFileCorrector                  36XX
SolidLineDetector                   37XX
PatternDetector                     38XX
BlobTracker                         39XX
FgMaskExtractor                     40XX
BgMaskExtractor                     41XX
InspectionDetector                  42XX
DBFns                               43XX
DBAccessor                          44XX
VideoConcatenator                   45XX
CreateVideoFromImages               46XX
ContainerDetector					47XX
LicensePlateDetector				48XX
SecurityPersonnelDetector			49XX
OCR									50XX
LicensePlateExtractor               51XX
ProbabilityExtractor                52XX
InspectionDetector2                 53XX
MotionDetector                      54XX
SecurityApp                         55XX
BlobTracker2                        56XX
ObjectDetector       				57XX
OCRNN                               58XX
OCRAccessor                         59XX
ImageCropper                        60XX
VTracker                            61XX
GTCreator                           62XX
NoisyImageGenerator                 63XX
ATS                                 64XX
HVC                                 65XX
Delaunay                            66XX
ATSDetection                        67XX
OFTracker                           68XX
VehicleColor						69XX
DetectionAnalyzer                   70XX
Transform                           71XX
PoliceEyes                          72XX
DBConnectorMySQL                    73XX
HVCDB                               74XX
IPC									75xx
VehicleDetector						76xx
*/
