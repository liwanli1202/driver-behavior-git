/*
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include <cv.h>
#include <highgui.h>
#include <string>
using namespace cv;
using namespace std;
#include "DriverAwareness.h"
#include "DriverBehaviorAnalysis.h"

//These global variables are to display only.
int counter = 0;
bool previous = false; 
int lane_change_count = 0; 

int _lane = 0;
int plane = 0;
bool laneCrossing = false; 
bool laneCrossingPrevious = false; 
std::deque<double> pLane;
std::deque<double> nLane;
int _count = 0; 



int DriverAwareness::_iObjCount = 0;
ErrorHandler* DriverAwareness::_pErrorHandler = NULL;
Logger* DriverAwareness::_pLogger = NULL;
bool DriverAwareness::_bIsSetErrorDiagnosticsList = false;

// Program parameters
string DriverAwareness::_sTimeOfCapture = string("");
int DriverAwareness::_iNumFramesToIgnore = 0;
string DriverAwareness::_sCamera1 = string("");
string DriverAwareness::_sVideo1Path = string("");
int DriverAwareness::_iCamera1No = 0;
int DriverAwareness::_iCaptureMode1 = 0;
int DriverAwareness::_iImageType1 = 0;
float DriverAwareness::_fFPS = 0;
int DriverAwareness::_iNumFramesToGrab = 0;
bool DriverAwareness::_bIsSetSaveVideo = false;
int DriverAwareness::_iCamera1Width = -1;
int DriverAwareness::_iCamera1Height = -1;
bool DriverAwareness::_bIsSetConfigFile = false;
string DriverAwareness::_sConfigFile = "";
bool DriverAwareness::_bIsSetRegionsFromConfigFile = false;
FileStorage DriverAwareness::_fsConfigFileRead;
bool DriverAwareness::_bIsSetParamsFile = false;
string DriverAwareness::_sParamsFile = "";
cv::FileStorage DriverAwareness::_fsParamsFileRead;

// Display image windows
bool DriverAwareness::_bDisableDispWindows = false;
bool DriverAwareness::_bDispImgCapture = false;

// Display graph
bool DriverAwareness::_bDispGraph = false;

// Process frames
bool DriverAwareness::_bProcessFrames = false;

// Mask
bool DriverAwareness::_bEnableMask = false;

void DriverAwareness::initErrorDiagnosticsList()
{
    if (!DriverAwareness::_bIsSetErrorDiagnosticsList)
    {
        //NOTE: Enter the ID XX for your class in ErrorHandler.h so that there is no conflict.
        // You will be required to change 6101 to XX01
#define CLASS_INVALID_OBJECT                        6101
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_INVALID_OBJECT,
                string("Class : Invalid object. "));
#define CLASS_MAX_PROCESSING_AREA_NOT_SET           6102
        _pErrorHandler->insertErrorDiagnostics(
                (int)CLASS_MAX_PROCESSING_AREA_NOT_SET,
                string("Class : Max processing area is not set."));
#define CLASS_NULL_IMAGE                            6103
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_NULL_IMAGE,
                string("Class : Image is null."));
#define CLASS_INCONSISTENT_IMAGES                   6104
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_INCONSISTENT_IMAGES,
                string("Class : Inconsistent images."));
#define CLASS_ROI_NOT_SET_YET                       6105
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_ROI_NOT_SET_YET,
                string("Class : ROI is not set yet."));
#define CLASS_INVALID_ROI                           6106
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_INVALID_ROI,
                string("Class : Invalid ROI. Minimum ROI is 10 x 10 pixels"));
#define CLASS_INVALID_NUM_CHANNELS                  6107
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_INVALID_NUM_CHANNELS,
                string("Class : Image has invalid number of channels."));
#define CLASS_INVALID_CAMERA                       6108
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_INVALID_CAMERA,
                string("InspectionDetectoor : Invalid camera specified"));
#define CLASS_INVALID_VIDEO_WRITER                 6109
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_INVALID_VIDEO_WRITER,
                string("Class : Invalid video writer."));
#define CLASS_INVALID_TIMESTAMP_FILE               6110
        _pErrorHandler->insertErrorDiagnostics(
                (int)CLASS_INVALID_TIMESTAMP_FILE,
                string("Class : Invalid timestamp file."));
#define CLASS_MODULE_NOT_SET                        6111
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_MODULE_NOT_SET,
                string("Class : Module is not set."));
#define CLASS_INVALID_MODULE                        6112
        _pErrorHandler->insertErrorDiagnostics((int)CLASS_INVALID_MODULE,
                string("Class : Module is invalid."));

        // Escapable Exceptions
    }
    DriverAwareness::_bIsSetErrorDiagnosticsList = true;
}

DriverAwareness::DriverAwareness()
{
    DriverAwareness::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
    _pClock = new Clock();
    _pProcessingRegions = new Regions();
    _pTimer = new Timer();
    _pUtilities = new Utilities();
    _pLaneMarkingFilter = NULL;
    _matProjection.create(3, 4, CV_64FC1);

    _pUtil = new Util();
    _pDriverBehaviorAnalysis = new DriverBehaviorAnalysis();

    _pImageProcessGray = NULL;
    _pImage3Channel = NULL;
    _pImageResized = NULL;
    _iImageROIWidth = -1;
    _iImageROIHeight = -1;
    _bIsSetROI = false;

    _pImageProcess = NULL;
    _pImageForDisplay = NULL;
    _pImageForVideo = NULL;
    _pImageMotion = NULL;

    _pImageMaskROI = NULL;
    _pImageToSave = NULL;
    _sImageToSaveTS = string("");

    _pImageMask = NULL;

    _bIsSystemInitialized = false;
    _iFrameNo = 0;
    _iFrameIntvlCount = 0;
    _bIsSetProcessingStartTime = false;

    _lFrameNo = 0;

    _ssOut.str("");
    _sText = string("");

    // Font initialization
    cvInitFont(&_font1, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1, CV_AA);
    cvInitFont(&_font2, CV_FONT_HERSHEY_PLAIN, 3, 3, 0, 2, CV_AA);
    _textColor = CV_RGB(255,0,0);

    _pMemStorage = cvCreateMemStorage(0);

    // Raw time of capture
    _sTimeOfCapture = string("not-set-yet");

    // Saving to video file
    _pVideoWriter = NULL;
    _iVideoFileNo = 1;
    _bSaveFrameToVideo = false;
    _bCreateNewVideoFile = true;
    _iFrameNoInVideo = 0;

    // Terminate program
    _bTerminateProgram = false;

    // Processing framerate: Initialization
    _tvPrev = _pTimer->getCurrentTime();
    _dProcessingFPS = 0.0f;

    // Keep track of time between frames
    _vtvTimeBetweenFrames.clear();
    _bSlowDown = false;

    _cPixelsIgnoreLeft = _cPixelsIgnoreRight = _cPixelsIgnoreTop = _cPixelsIgnoreBottom = 0;

    initErrorDiagnosticsList();
}

DriverAwareness::~DriverAwareness()
{
    DriverAwareness::_iObjCount--;

    // == Write to Log file ==
    // Time of exiting the program
    string sEndTime = _pClock->getCurrentDateTimeString(false);
    _pLogger->writeToFile(FILE_LOG, string("End Time :") + sEndTime);

    // No. of frames grabbed.
    _ssOut.str("");
    _ssOut << _iFrameNo;
    _pLogger->writeToFile(FILE_LOG,
            string("No. of frames grabbed:" + _ssOut.str()));

    if (_pClock) delete _pClock;
    if (_pProcessingRegions) delete _pProcessingRegions;
    if (_pTimer) delete _pTimer;
    if (_pUtilities) delete _pUtilities;
    if (_pLaneMarkingFilter) delete _pLaneMarkingFilter;

    if (_pUtil) delete _pUtil;

    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;

    _pImage3Channel = NULL;
    if (_pImageResized) cvReleaseImage(&_pImageResized);
    if (_pImageProcess) cvReleaseImage(&_pImageProcess);
    if (_pImageForVideo) cvReleaseImage(&_pImageForVideo);
    if (_pImageForDisplay) cvReleaseImage(&_pImageForDisplay);
    if (_pImageMotion) _pImageMotion = NULL;

    if (_pImageMask) cvReleaseImage(&_pImageMask);

    if (_pImageMaskROI) _pImageMaskROI = NULL;
    if (_pImageToSave) cvReleaseImage(&_pImageToSave);

    if (_pVideoWriter) cvReleaseVideoWriter(&_pVideoWriter);
    if (_fsVideoTimestampYML.isOpened()) _fsVideoTimestampYML.release();

    if (_pMemStorage) cvReleaseMemStorage(&_pMemStorage);

    if (_fsConfigFileRead.isOpened()) _fsConfigFileRead.release();
}

string DriverAwareness::getOutputFolder()
{
    return string(Logger::getInstance()->getOutputFolder());
}

void DriverAwareness::startApp(bool bDebug)
{
	if(!_bIsSystemInitialized) initializeSystem(bDebug);

    // Grab image from camera
    CameraAccessor *pCameraAccessor = NULL;
    CameraEmulator *pCameraEmulator = NULL;
    CameraIQEye *pCameraIQEye = NULL;
    CameraUSB *pCameraUSB = NULL;
    int iStreamerID = 0;

    if (_sCamera1.compare(string("emulator")) == 0)
    {
        pCameraEmulator = new CameraEmulator(_sVideo1Path.c_str());
        pCameraEmulator->loadMatFromConfig();
        _pCamera = (Camera*)pCameraEmulator;
        pCameraAccessor = new CameraAccessor(string("Emulator"));
        pCameraAccessor->setCameraSelectionStrategy(_pCamera, _iCaptureMode1,
                (char*)_sVideo1Path.c_str(), _iImageType1, bDebug);
    }
    else if (_sCamera1.compare(string("iqeye")) == 0)
    {
        pCameraIQEye = new CameraIQEye(iStreamerID, _sVideo1Path.c_str());
        pCameraIQEye->setImageResolution(_iCamera1Width, _iCamera1Height,
                bDebug);
        pCameraIQEye->loadMatFromConfig();
        _pCamera = (Camera*)pCameraIQEye;
        pCameraAccessor = new CameraAccessor(string("IQEye"));
        pCameraAccessor->setCameraSelectionStrategy(_pCamera, _iCaptureMode1,
                _iImageType1, bDebug);
    }
    else if (_sCamera1.compare(string("usb")) == 0)
    {
        pCameraUSB = new CameraUSB(_iCamera1No);
        pCameraUSB->loadMatFromConfig();
        _pCamera = (Camera*)pCameraUSB;
        pCameraAccessor = new CameraAccessor(string("USB"));
        pCameraAccessor->setCameraSelectionStrategy(_pCamera, _iCaptureMode1,
                _iImageType1, bDebug);
    }

    _pCamera->setNumFramesToGrab(_iNumFramesToGrab);
    // For Class, all the frames are not necessary to be saved.
    // Only saving frames with motion in Class class.
    //_pCamera->setSaveVideo(_bIsSetSaveVideo);
    pCameraAccessor->initializeCamera(bDebug);
    pCameraAccessor->setSaveVideo(_bIsSetSaveVideo, 1);

    // Start grabbing frames
    IplImage *pImageGrabbed = NULL;
    _iFrameNo = -1;
    _sTimeOfCapture = string("not-set-yet");

    // Enable user to select an initial frame for Bg Subtraction
    bool bEnableSelectingFirstFrame = false;

    // Enable capture as per specified Frames per second.
    // NOTE: Some applications may be better off with low frame rate. Application specific.
    timeval previousTime = _pTimer->getCurrentTime();
    timeval currentTime = _pTimer->getCurrentTime();
    double dNumMsPerCapture = 1000.0 / _fFPS; // In milliseconds
    time_t rawTime;

    // Create LaneMarkingDetector object
    _pLaneMarkingDetector = new LaneMarkingDetector();
    // Initialize camera model, set P, H, HInv
    //_pLaneMarkingDetector->initializeCameraModel(1555.0,1555.0,320.0,240.0,640,480,0,0,0,0,1,0);
    _pLaneMarkingDetector->initializeCameraModelP(
    		_matProjection.at<double>(0,0),  _matProjection.at<double>(0,1), _matProjection.at<double>(0,2), _matProjection.at<double>(0,3),
    		_matProjection.at<double>(1,0),  _matProjection.at<double>(1,1), _matProjection.at<double>(1,2), _matProjection.at<double>(1,3),
    		_matProjection.at<double>(2,0),  _matProjection.at<double>(2,1), _matProjection.at<double>(2,2), _matProjection.at<double>(2,3),
			_iCamera1Width, _iCamera1Height);
    _pLaneMarkingDetector->setIgnorePixels(_cPixelsIgnoreLeft, _cPixelsIgnoreRight, _cPixelsIgnoreTop, _cPixelsIgnoreBottom);
    _pLaneMarkingDetector->setExpectedLaneWidth(_dExpectedLaneWidth);
    _pLaneMarkingFilter = new LaneMarkingFilter(_dExpectedLaneWidth);
	_pLaneMarkingFilter->setH(_pLaneMarkingDetector->getH());

    while (true)
    {
        // Ensure the effect of specified FPS, especially in delaying grabbing.
        currentTime = _pTimer->getCurrentTime();
        double dTimeDiff = _pTimer->getElapsedTimeInMilliSec(previousTime,
                currentTime);
        //cout << "Current time difference = " << dTimeDiff << ". Required interval = " << dNumMsPerCapture  << endl;
        if (dTimeDiff < dNumMsPerCapture)
        {
            usleep(100 * dNumMsPerCapture); //(1/10)th of interval per capture
            //cout << "Skipping capture..." << endl;
            continue;
        }
        previousTime = _pTimer->getCurrentTime();

        // Check if grabbing is complete
        if (_pCamera->getIsGrabComplete() || _pCamera->getStopCapture())
        {
            cout << "Grab complete." << endl;
            setTerminateProgram(true);
            break;
        }

        if (bTerminate) // Either from end of capture or Ctrl+C or kill
        {
            cerr << "Program terminated by user." << endl;
            cerr << "Cleaning up ..." << endl;
            setTerminateProgram(true);
            break;
        }

        if (getTerminateProgram()) break; // Terminated from external process

        // Grab image
        time(&rawTime);
        if (!pCameraAccessor->grabImage(bDebug)) // Keep trying
        {
        	int iStatus = _iFrameNo > 0 ? 0 : 1;
        	cout << "No more frames after " << _iFrameNo << " frames.  Exit with status " << iStatus << "." << endl;
            exit(iStatus);
        }
        _tvFrameCapture = _pCamera->getTimeOfFrameCapture();
        _vtvTimeBetweenFrames.push_back(_tvFrameCapture);
        if (bDebug)
        {
            double dTimeOfCapture = _tvFrameCapture.tv_sec + (double)_tvFrameCapture.tv_usec / 1000000;
            cout << "dTimeOfCapture = " << dTimeOfCapture << endl;
        }

        _currentFrameTimestampInMilliSec = _pTimer->getCurrentTimeInMilliSec();
        if( _iFrameNo == 0)
        {
        	_timestampAtFirstFrameMilliSec = _currentFrameTimestampInMilliSec;
        	_prevFrameTimestampInMilliSec = _currentFrameTimestampInMilliSec;
        }
#if !GET_TIMESTAMP_BASED_ON_FPS
        // Get timestamp in milliseconds based on system clock
        _timestamp = _currentFrameTimestampInMilliSec - _timestampAtFirstFrameMilliSec;
#else
        // Get timestamp in milliseconds based on frame rate
        _timestamp = (long long)_iFrameNo * (1000.0/(float)VIDEO_FRAME_RATE);
#endif
        _iFrameNo++;

        if (_iFrameNo < _iNumFramesToIgnore)
        {
            if (_iFrameNo % 100 == 0)
            {
                cout << "Frame No. " << _iFrameNo << ".." << flush;
                cout << "No. of frames to skip: " << _iNumFramesToIgnore
                        << endl;
            }
            continue;
        }

        // NOTE: Need to "flush" to ensure that the output is displayed.
        //cout << "f," << _iFrameNo << "," << _timestamp << "," << flush << endl;
        _sTimeOfCapture = _pClock->getFormattedDateTimeString(rawTime);

        pImageGrabbed = pCameraAccessor->getImage(bDebug);

        if (_bDispImgCapture)
        {
            _pUtilities->displayImage(pImageGrabbed, string("Grabbed frame"));
            char chKey;
            if (TA_ENABLE_SKIPPING_FRAMES_MANUALLY || TA_ENABLE_PAUSE
                    || TA_ENABLE_SLOWDOWN)
            {
                chKey = waitKey(40);
                if (chKey == 'c' || chKey == 'C') // skip
                {
                    continue;
                }
                else if (chKey == 'p' || chKey == 'P') // pause
                {
                    cout << endl << "Paused. Press 'p' to continue..." << endl;
                    chKey = waitKey(250);
                    while (chKey != 'p' || chKey == 'P')
                    {
                        chKey = waitKey(1000);
                    }
                }
                else if (chKey == 's' || chKey == 'S') // slow
                {
                    _bSlowDown = !_bSlowDown;
                }
                if (_bSlowDown)
                {
                    cout << endl
                            << "Slow down mode. Press 's' to disable slow down..."
                            << endl;
                    chKey = waitKey(1000);
                    if(chKey == 's' || chKey == 'S')
                    {
                        _bSlowDown = !_bSlowDown;
                    }
                }
            }
            cvWaitKey(1);
        }

        bEnableSelectingFirstFrame = TA_ENABLE_SELECTING_FIRST_FRAME;
        if (bEnableSelectingFirstFrame)
        {
            if (!setFrameForInit(pImageGrabbed))
            {
                continue;
            }
        }
        if (_bProcessFrames)
        {
            processFrame(pImageGrabbed, _timestamp, bDebug);
        }

        _prevFrameTimestampInMilliSec = _currentFrameTimestampInMilliSec;
    }

    // Clean up
    pCameraAccessor->closeCamera(bDebug);
    if (pImageGrabbed) pImageGrabbed = NULL;
    if (_pCamera) _pCamera = NULL;
    if (pCameraEmulator) delete pCameraEmulator;
    if (pCameraIQEye) delete pCameraIQEye;
    if (pCameraUSB) delete pCameraUSB;
    if (pCameraAccessor) delete pCameraAccessor;
}

bool DriverAwareness::setFrameForInit(IplImage *pImageGrabbed)
{
    bool bIsSetFrameForInit = false;
    bool bMsgToSelectFrameDisplayed = false;
    string sWindowName = string("Grabbed image");
    char chKey = 0;
    if (!bIsSetFrameForInit)
    {
        if (!bMsgToSelectFrameDisplayed)
        {
            // Enable user to skip frames to find the right frame to define regions
            cout << endl << endl << "Select a frame for initialization." << endl
                    << "Press 'Q' to select frame" << endl;
            bMsgToSelectFrameDisplayed = true;
        }
        cvNamedWindow(sWindowName.c_str(), 0);
        _pUtilities->writeTextOnImage(pImageGrabbed,
                "Select a frame for initialization.", 0);
        _pUtilities->writeTextOnImage(pImageGrabbed,
                "Press 'Q' to select frame.", 1);
        cvShowImage(sWindowName.c_str(), pImageGrabbed);
        //cvSetWindowProperty(sWindowName.c_str(), CV_WND_PROP_FULLSCREEN,
        //        CV_WINDOW_FULLSCREEN);
        if (_sCamera1.compare("emulator") == 0)
        {
            chKey = cvWaitKey(100);
        }
        else
        {
            chKey = cvWaitKey(1000);
        }

        if (chKey == 'q' || chKey == 'Q')
        {
            bIsSetFrameForInit = true;
        }
    }
    return bIsSetFrameForInit;
}

void DriverAwareness::processFrame(const IplImage *pImageC, long long timestamp, bool bDebug)
{
	if(!_bDisableDispWindows)
	{
		cvNamedWindow("Input",1);
		cvShowImage("Input", pImageC);
		cvWaitKey(2);
	}

	assert(pImageC->height == _iCamera1Height);
	assert(pImageC->width == _iCamera1Width);

	// Convert to grayscale

	if (!_pImageProcessGray) // allocate space for the GRAY frame only once
	{
		_pImageProcessGray = cvCreateImage(cvGetSize(pImageC), pImageC->depth, 1);
		_pImageMask = cvCreateImage(cvGetSize(pImageC), pImageC->depth,1);
		if(_bEnableMask)
		{
			createMask(bDebug);
		}
	}
	cvCvtColor(pImageC, _pImageProcessGray, CV_RGB2GRAY); // convert RGB frame to GRAY

	if(_bEnableMask)
	{
		cvAnd(_pImageProcessGray, _pImageMask, _pImageProcessGray);
	}

	// Find lane markings
	timeval startTime = _pTimer->getCurrentTime();
	LaneMarkingDetector::Boundary boundaryLeft, boundaryRight;
	_pLaneMarkingDetector->findLaneBoundaries( pImageC, _pImageProcessGray,
			boundaryLeft, boundaryRight, _bDisableDispWindows, bDebug);
	_pLaneMarkingFilter->updateState(boundaryLeft, boundaryRight, pImageC, _bDisableDispWindows, false);
	double dPosition, dYaw;
        _pLaneMarkingFilter->getCurrentState(dPosition, dYaw);

	timeval finishTime = _pTimer->getCurrentTime();

        // Akila processes for events here

	// Multiply 10.0 to perpendicular distance just for visualization
	if(_bDispGraph) _pUtil->graph(boundaryLeft.dPerp*10.0, boundaryRight.dPerp*10.0);

	double dTimeDiffFindMarkings = _pTimer->getElapsedTimeInMilliSec(startTime, finishTime);
	if(bDebug)
	{
		cout << endl << "Lane boundaries calculated in " << dTimeDiffFindMarkings << " ms" << endl;
	}
	//cout << dPosition << "," << dYaw << endl;

	// Output analysis information to terminal

	// analyze driver behavior

//	plotGraph(dPosition);

	bool val = _pDriverBehaviorAnalysis->detectLaneChange(dPosition, boundaryLeft.dPerp , boundaryRight.dPerp, timestamp ); 	
	_pDriverBehaviorAnalysis->setData( DriverAwareness::getFrameNo(),
					   timestamp,
					   dPosition,
					   _pDriverBehaviorAnalysis->getLaneChangeSpeed(val)) ;

        /*
	   This code snippet is only to display lane changes in the video
	   Can comment out if wanted.
	*/ 	
	Mat mat_img(pImageC);
	if (val == true)
	{
		 putText(mat_img, "LANE CHANGE" , cvPoint(75,200),
                                        FONT_HERSHEY_COMPLEX_SMALL, 3.0, cvScalar(0,0,255), 2, CV_AA);
		 counter++;
		 previous = true;
		 plane = 0; 
		 _lane = 0;

	}
	else if (val == false && previous == true)
	{
		counter = 0;
		previous = false; 
		lane_change_count++;
		_count = 0; 

	}
	_count++;
/* 
 	if (dPosition < -0.70)
        {
                _lane++;
		plane = 0;
		nLane.push_back(dPosition);  


        }
	else if (dPosition > 0.70)
	{
		plane++;
		_lane =0;
		pLane.push_back(dPosition); 

	}
        else
        {
                _lane = 0;
		plane= 0;
			pLane.clear();
			nLane.clear();
		laneCrossing = false;
        }

//	if (pLane.size()>0||nLane.size()>0)
//	{ 
		double positiveVari = getVariance(pLane);
		double negativeVari = getVariance(nLane);
		

		cout << "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNn" << negativeVari << " size " << nLane.size() << endl; 
		cout << "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP" << positiveVari << " size " << pLane.size() << endl;	

        	//if ((positiveVari < 0.0030 && pLane.size()>25) || (negativeVari < 0.0030 && nLane.size() > 25 ) )
  
        	if ((pLane.size()>35) || ( nLane.size() > 35 ) )
  
	      	{
                	 laneCrossing = true; 
			//cvWaitKey(0);
			 putText(mat_img, "Lane Crossing" , cvPoint(90,200),
                                        FONT_HERSHEY_COMPLEX_SMALL, 3.0, cvScalar(0,255,255), 2, CV_AA);
			// lane_change_count--;
			laneCrossingPrevious = true; 
		
        	}
	
		if (laneCrossing == false && laneCrossingPrevious == true)
		{
			if (_count < 50 )lane_change_count--; 
			laneCrossingPrevious = false;	
			_count = 0; 

		}
		
//	}	
*/

	putText(mat_img, "Count = "+boost::to_string(lane_change_count) , cvPoint(10,470),
                                        FONT_HERSHEY_COMPLEX_SMALL, 2.0, cvScalar(0,255,0), 2, CV_AA);

        cv::namedWindow("Video", CV_WINDOW_AUTOSIZE);
        cv::imshow("Video", mat_img);


	




	/* end */


}



void DriverAwareness::createVideoFile(int iVideoFileNo, IplImage *pImage)
{
    string sOutputFolder = getOutputFolder();
    string sDateTime = Clock::getFormattedDateTimeStringForMySQL(
            Clock::getCurrentTimeStampInSec());

    // Create new video file
    string sVideoFile = sOutputFolder + sDateTime + string(".avi");
    if (_pVideoWriter)
    {
        cvReleaseVideoWriter(&_pVideoWriter);
    }
    _pVideoWriter = cvCreateVideoWriter(sVideoFile.c_str(),
            CV_FOURCC('D', 'I', 'V', 'X'), TA_FRAMERATE_IN_VIDEO_CLIP,
            cvGetSize(pImage), true);

    // Create a corresponding timestamp file
    string sTimestampFile = sOutputFolder + sDateTime + string(".yml");
    if (_fsVideoTimestampYML.isOpened())
    {
        _fsVideoTimestampYML.release();
    }
    _fsVideoTimestampYML.open(sTimestampFile, FileStorage::WRITE);
}

// TODO: Need to create a player to play the video with the timestamp, so
// that if the time between two frames is large, this needs to be simulated.
void DriverAwareness::saveFrameToVideo(IplImage *pImage, int iFrameNo)
{
    _iFrameNoInVideo++;
    cout << "Saving frame " << iFrameNo << " to video frame "
            << _iFrameNoInVideo << endl;
    _tvTimeOfFrameCapture = _pCamera->getTimeOfFrameCapture();

    if (_pVideoWriter)
    {
        cvWriteFrame(_pVideoWriter, pImage);
    }
    else
    {
        _pErrorHandler->setErrorCode((int)CLASS_INVALID_VIDEO_WRITER);
    }
    if (_fsVideoTimestampYML.isOpened())
    {
        writeToVideoTimestampFile(_tvTimeOfFrameCapture, _iFrameNoInVideo);
    }
    else
    {
        _pErrorHandler->setErrorCode((int)CLASS_INVALID_TIMESTAMP_FILE);
    }
}

void DriverAwareness::writeToVideoTimestampFile(timeval curTime, int iFrameNo,
        bool bDebug)
{
    _ssOut.str("");
    _ssOut << iFrameNo;
    string sFrameNo = string("f") + _ssOut.str();

    time_t tFrameTime;
    time(&tFrameTime);
    string sTime1 = _pClock->getFormattedDateTimeString(tFrameTime);
    double dTime1 = (double)(long)tFrameTime;
    double dSec = curTime.tv_sec;
    double dMicroSec = curTime.tv_usec;

    _fsVideoTimestampYML << sFrameNo << "{";
    _fsVideoTimestampYML << "sTime1" << sTime1;
    _fsVideoTimestampYML << "dTime1" << dTime1;
    _fsVideoTimestampYML << "dSec" << dSec;
    _fsVideoTimestampYML << "dMicroSec" << dMicroSec;
    _fsVideoTimestampYML << "}";
    if (bDebug)
    {
        cout << "sTime1: " << sTime1 << endl;
        cout << "dTime1: " << dTime1 << endl;
        cout << "dSec: " << dSec << endl;
        cout << "dMicroSec: " << dMicroSec << endl;
    }
}

void DriverAwareness::specifyProcessingRegions(bool bDebug)
{
    // == Specify Processing area(s) ==
    bool bEnableMultipleProcessingAreas = TA_ENABLE_MULTIPLE_PROCESSING_AREAS;
    _pProcessingRegions->setImage(_pImageResized);
    String sWindowName = string("Processing Area");
    int iNumProcessingAreas = 1;
    if (bEnableMultipleProcessingAreas)
    {
        cout << endl << "No. of processing areas:";
        cin >> iNumProcessingAreas;
        if (iNumProcessingAreas < 0)
        {
            cerr << "Invalid number of processing areas specified." << endl
                    << "Setting it to 1." << endl;
            iNumProcessingAreas = 1;
        }
    }

    for (int iCount = 0; iCount < iNumProcessingAreas; iCount++)
    {
        _ssOut.str("");
        _ssOut << (iCount + 1);
        sWindowName = string("Processing Area ") + _ssOut.str();
        _pProcessingRegions->setWindowName(sWindowName);
        _pProcessingRegions->setOverlayProperties(true, 1,
                cvScalar(255, 255, 0), cvScalar(0, 255, 0));
        _pProcessingRegions->specifyRegionManually("Specify Processing area",
                "Left click: Set point, Right click: Close region, 'Q': Exit");
        void* pvoidWinHandle = cvGetWindowHandle(sWindowName.c_str());
        if (pvoidWinHandle != 0) cvDestroyWindow(sWindowName.c_str());
    }
    _pImageMaskROI = cvCloneImage(_pProcessingRegions->getImageRegionsMask());
}

// NOTE: In this function, if the specified height is greater than the
// actual height of the original image, the original image will be used directly.
void DriverAwareness::resizeInputImage(IplImage **ppImageResized,
        int iHeightOfResizedImage, bool bSingleChannel, bool bDebug)
{
    IplImage *pImage = *ppImageResized;
    Timer *pTimer = NULL;
    if (bDebug)
    {
        pTimer = new Timer();
        pTimer->start();
    }
    bool bSameSize = false;
    int iWidthOfResizedImage = _pImage3Channel->width; // Initialization
    if (iHeightOfResizedImage < _pImage3Channel->height)
    {
        double dHeightRatio = (double)iHeightOfResizedImage
                / _pImage3Channel->height;
        iWidthOfResizedImage = cvCeil(dHeightRatio * _pImage3Channel->width);
    }
    else
    {
        // Just take the original image in this case to avoid increasing the
        // scale of the image
        iHeightOfResizedImage = _pImage3Channel->height;
        bSameSize = true;
    }
    if (!pImage)
    {
        if (bSameSize)
        {
            pImage = cvCloneImage(_pImage3Channel);
        }
        else
        {
            pImage = cvCreateImage(
                    cvSize(iWidthOfResizedImage, iHeightOfResizedImage),
                    IPL_DEPTH_8U, 3);
            cvResize(_pImage3Channel, pImage, CV_INTER_CUBIC);
        }
    }
    else
    {
        if (bSameSize)
        {
            cvCopyImage(_pImage3Channel, pImage);
        }
        else
        {
            cvResize(_pImage3Channel, pImage, CV_INTER_CUBIC);
        }
    }
    if (bSingleChannel)
    {
        IplImage *pImageGray = cvCreateImage(
                cvSize(pImage->width, pImage->height), pImage->depth, 1);
        cvConvertImage(pImage, pImageGray, CV_BGR2GRAY);
        *ppImageResized = pImageGray;
        if (pImage) cvReleaseImage(&pImage);
    }
    else
    {
        *ppImageResized = pImage;
    }
    if (bDebug)
    {
        cout << "Elapsed time for resizing image: "
                << pTimer->getElapsedTimeInMilliSec() << " ms" << endl;
        if (pTimer) delete pTimer;
    }
}

// Specify _pImageMaskROI
void DriverAwareness::initializeSystem(bool bDebug)
{
    if (!_bIsSystemInitialized)
    {
        cout << endl << "Initializing system ..." << endl;

        bool bSetParamsFromFile = false;
		if (_bIsSetParamsFile)
		{
			// Read the parameters from the sParamsFile
			bSetParamsFromFile = (bool)(int)_fsParamsFileRead["hasProjectionMatrixParams"];
			cout << "Params File is set" << endl;
			if (bSetParamsFromFile)
			{
				cout << "has projection matrix" << endl;
				_fsParamsFileRead["ProjectionMatrix"] >> _matProjection;
				if(bDebug)
				{
					for(int i=0; i < _matProjection.rows; i++)
					{
						for(int j=0; j < _matProjection.cols; j++)
						{
							cout << _matProjection.at<double>(i,j) << endl;
						}
					}
				}
			}
			_fsParamsFileRead["estimatedRoll"] >> _dRoll;
			_fsParamsFileRead["estimatedPitch"] >> _dPitch;
			_fsParamsFileRead["estimatedYaw"] >> _dYaw;

			_fsParamsFileRead["ignorePixelsLeft"] >> _cPixelsIgnoreLeft;
			_fsParamsFileRead["ignorePixelsRight"] >> _cPixelsIgnoreRight;
			_fsParamsFileRead["ignorePixelsTop"] >> _cPixelsIgnoreTop;
			_fsParamsFileRead["ignorePixelsBottom"] >> _cPixelsIgnoreBottom;

			_fsParamsFileRead["expectedLaneWidth"] >> _dExpectedLaneWidth;

			if(bDebug)
			{
				cout << "Estimated Roll: " << _dRoll << endl;
				cout << "Estimated Pitch: " << _dPitch << endl;
				cout << "Estimated Yaw: " << _dYaw << endl;
				cout << "Pixels to ignore on left: " << _cPixelsIgnoreLeft << endl;
				cout << "Pixels to ignore on right: " << _cPixelsIgnoreRight << endl;
				cout << "Pixels to ignore on top: " << _cPixelsIgnoreTop << endl;
				cout << "Pixels to ignore on bottom: " << _cPixelsIgnoreBottom << endl;
			}
		}


		if (_bEnableMask)
		{
			// Read the parameters for creating mask
			bSetParamsFromFile = (bool)(int)_fsParamsFileRead["hasMaskMatrixParams"];
			if (bSetParamsFromFile)
			{
				_fsParamsFileRead["MaskMatrix"] >> _matMask;
				if(bDebug)
				{
					for(int i=0; i < _matMask.rows; i++)
					{
						for(int j=0; j < _matMask.cols; j++)
						{
							cout << _matMask.at<int>(i,j) << endl;
						}
					}
				}

			}
	
		}
        // Set system as initialized.
        _bIsSystemInitialized = true;
        cout << "System initialized." << endl;
    }
}


// == Getters and Setters ==

int DriverAwareness::getObjCount()
{
    return DriverAwareness::_iObjCount;
}

int DriverAwareness::getFrameNo()
{
    return _iFrameNo;
}

// Program parameters

string DriverAwareness::getTimeOfCapture()
{
    return DriverAwareness::_sTimeOfCapture;
}

void DriverAwareness::setNumFramesToIgnore(int iNumFramesToIgnore, bool bDebug)
{
    _iNumFramesToIgnore = iNumFramesToIgnore;
}

int DriverAwareness::getNumFramesToIgnore()
{
    return DriverAwareness::_iNumFramesToIgnore;
}

void DriverAwareness::setCamera1(string sCamera1)
{
    _sCamera1 = sCamera1;
}

void DriverAwareness::setVideo1Path(string sVideo1Path)
{
    _sVideo1Path = sVideo1Path;
}

void DriverAwareness::setCamera1No(int iCamera1No)
{
    _iCamera1No = iCamera1No;
}

void DriverAwareness::setCaptureMode1(int iCaptureMode1)
{
    _iCaptureMode1 = iCaptureMode1;
}

void DriverAwareness::setImageType1(int iImageType1)
{
    _iImageType1 = iImageType1;
}

void DriverAwareness::setFPS(float fFPS)
{
    DriverAwareness::_fFPS = fFPS;
}

void DriverAwareness::setNumFramesToGrab(int iNumFramesToGrab)
{
    _iNumFramesToGrab = iNumFramesToGrab;
}

void DriverAwareness::setSaveVideo(bool bIsSetSaveVideo)
{
    _bIsSetSaveVideo = bIsSetSaveVideo;
}

void DriverAwareness::setCamera1Resolution(int iWidth, int iHeight)
{
    DriverAwareness::_iCamera1Width = iWidth;
    DriverAwareness::_iCamera1Height = iHeight;
}

void DriverAwareness::setConfigFile(char *pchConfigFile)
{
    _bIsSetConfigFile = true;
    _sConfigFile = string(pchConfigFile);
    if (!_fsConfigFileRead.isOpened())
    {
        _fsConfigFileRead.open(_sConfigFile, FileStorage::READ);
    }
}

void DriverAwareness::setRegionsFromConfigFile(bool bSetRegionsFromConfigFile)
{
    _bIsSetRegionsFromConfigFile = bSetRegionsFromConfigFile;
}

void DriverAwareness::setDisableDispWindows(bool bDisableDispWindows)
{
    DriverAwareness::_bDisableDispWindows = bDisableDispWindows;
}

void DriverAwareness::setDispImgCapture(bool bDispImgCapture)
{
    DriverAwareness::_bDispImgCapture = bDispImgCapture;
}

void DriverAwareness::setDispGraph(bool bDispGraph)
{
    DriverAwareness::_bDispGraph = bDispGraph;
}

void DriverAwareness::setProcessFrames(bool bProcessFrames)
{
    DriverAwareness::_bProcessFrames = bProcessFrames;
}

void DriverAwareness::setEnableMask(bool bEnableMask)
{
	DriverAwareness::_bEnableMask = bEnableMask;
}

// = Program termination by user control =

void DriverAwareness::setTerminateProgram(bool bTerminateProgram)
{
    _bTerminateProgram = bTerminateProgram;
}

bool DriverAwareness::getTerminateProgram()
{
    return _bTerminateProgram;
}

void DriverAwareness::setParamsFile(char *pchParamsFile)
{
    _bIsSetParamsFile = true;
    _sParamsFile = string(pchParamsFile);
    if (!_fsParamsFileRead.isOpened())
    {
        _fsParamsFileRead.open(_sParamsFile, cv::FileStorage::READ);
    }
}

void DriverAwareness::createMask(bool bDebug)
{

	int lineType = 8;
	int numOfPoint = _matMask.cols/2;
	int numOfPolygon = _matMask.rows;
	if(bDebug)
	{
		cout << "Num of point: " << numOfPoint << endl;
		cout << "Num of polygon: " << numOfPolygon << endl;
	}
	CvPoint pointArray[numOfPoint];
	int k = 0; 
	for(int i=0; i < numOfPoint; i++)
	{
		pointArray[i] = cvPoint(_matMask.at<int>(0,k),_matMask.at<int>(0,k+1));
		if(bDebug) cout << "point " << i << ": " << pointArray[i].x << " " << pointArray[i].y << endl;
		k+=2;
	}

	// Number of polygon is one.
	CvPoint *maskPoints[1]={&pointArray[0]};
	int numOfPointInEachPolygon[1] = {numOfPoint};
	if(_pImageMask)
	{
		int numOfContourToBindWithPolygon = 1;
		int lineType = 8;
		int offset = 0;
		cvSet(_pImageMask, CV_RGB(255,255,255));
		cvFillPoly( _pImageMask, maskPoints, numOfPointInEachPolygon,
				    numOfContourToBindWithPolygon, cvScalar(0,0,0), lineType, offset);
	}

	if(bDebug)
	{
		cvNamedWindow("Mask",0);
		cvShowImage("Mask", _pImageMask);
		cvWaitKey(0);

	}

}
