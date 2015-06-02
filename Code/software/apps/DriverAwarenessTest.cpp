/*
 *  DriverAwarenessTest.cpp
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */
#include <unistd.h>
#include <signal.h>
#include <iostream>
using namespace std;

#include <cv.h>
#include <highgui.h>
using namespace cv;

#include <boost/thread.hpp>

#include "CameraCaptureConfig.h"
#include "DriverAwareness.h"
#include "DriverAwarenessConfig.h"
#include "ErrorHandler.h"
#include "Logger.h"

#define OUTPUT_FOLDER           "../output/"
#define PROJECT_FOLDER          "DriverAwarenessTest_App"

bool volatile bTerminate = false;
void sigterm_handler(int i)
{
    cout << "Program terminated by user." << endl;
    bTerminate = true;
}

bool bDebug = TA_DEBUG_MODE;
Utilities *pUtilities = NULL;

int main(int argc, char* argv[])
{
    cout << "\n\n" << endl;
    cout << "===================================" << endl;
    cout << "=      Driver Awareness Test      =" << endl;
    cout << "= --------------------------------=" << endl;
    cout << "=     CSIM/ISE, AIT, Thailand     =" << endl;
    cout << "===================================" << endl;

    // SIGTERM handler to terminate the program properly.
    bTerminate = false;
    struct sigaction sa;
    sa.sa_handler = &sigterm_handler;
    sigaction(SIGTERM, &sa, NULL);    // kill signal handler
    signal(SIGINT, sigterm_handler);  // Ctrl+C handler

    Logger *pLoggerTest = Logger::getInstance();
    ErrorHandler *pErrorHandlerTest = ErrorHandler::getInstance();
    pUtilities = new Utilities();

    // Check if the output folder is present or not. If not, create it.
    bool bIsFileOrFolderPresent = false;
    bool bIsFile = false;
    string sMainOutputFolder = string(OUTPUT_FOLDER);
    bIsFileOrFolderPresent = pUtilities->getIsFileOrFolderPresent(
            sMainOutputFolder, bIsFile);
    if (bIsFileOrFolderPresent)
    {
        if (bIsFile)
        {
            pErrorHandlerTest->setErrorCode((int)ERROR_IN_MAIN_FUNCTION,
                    "Specified output folder is actually a file.");
        }
    }
    else
    {
        cout << "Specified output folder is not present." << endl
                << "Creating the specified output folder " << sMainOutputFolder
                << endl;
        pLoggerTest->createFolder(sMainOutputFolder);
    }

    // Set output folder
    pLoggerTest->setOutputFolder((char*)string(PROJECT_FOLDER).c_str(),
            (char*)string(OUTPUT_FOLDER).c_str());
    string sOutputFolder = pLoggerTest->getOutputFolder();
    cout << "System output folder: " << sMainOutputFolder << endl;
    cout << "Current output folder: " << sOutputFolder << endl << endl;

    // == Initialization ==
    string sCamera1 = string("");
    string sVideo1Path = string("");
    int iCamera1No = -1;
    int iCaptureMode1 = (int)CAPTURE_MODE;
    // TODO: Need to save the config about image resolution, e.g. 480p, 720p, 1080p etc
    int iCamera1Width = -1;
    int iCamera1Height = -1;
    float fFPS = 30.0f;
    int iImageType = (int)IMAGE_TYPE;
    int iNumFramesToGrab = NUM_FRAMES_TO_GRAB;
    int iNumFramesToSkip = 0;
    bool bProcessFrames = false;
    bool bSaveVideo = false;
    bool bSetConfigFile = false;
    string sConfigFile = string("");
    bool bSetRegionsFromConfigFile = false;
    bool bSetParamsFile = false;
	string sParamsFile = string("");

      // Display image windows
    bool bDispImgCapture = false;
    bool bDisableDispWindows = false;

    // Display graph
    bool bDispGraph = false;

    // Enable mask to filter rearview mirror
    bool bEnableMask = false;

    // == Parse arguments passed to the program ==
    cout << "Obtaining inputs from arguments passed to the program..." << endl;
    int iArgsCount = 1;

    string sArguments = string("Arguments passed to the program:\n");
    while (iArgsCount < argc)
    {
        sArguments += string(argv[iArgsCount]) + string(" ");
        iArgsCount++;
    }
    pLoggerTest->writeToFile(FILE_LOG, sArguments);

    // Parse the arguments effectively for increased flexibility
    bool bValidArgs = true;
    iArgsCount = 1;
    while (iArgsCount < argc)
    {
        string sCommand = string(argv[iArgsCount]);
        if (sCommand.compare("-c1") == 0)
        {
            if ((iArgsCount + 1) >= argc)
            {
                bValidArgs = false;
                break;
            }
            sCamera1 = string(argv[iArgsCount + 1]);
            if (!(sCamera1.compare(string("emulator")) == 0
                    || sCamera1.compare(string("iqeye")) == 0
                    || sCamera1.compare(string("usb")) == 0))
            {
                bValidArgs = false;
                cout << "Camera must be set to one of the followings:" << endl
                        << "{ emulator, iqeye, usb }" << endl;
                break;
            }
            iArgsCount++;
        }
        else if (sCommand.compare("-p1") == 0)
        {
            sVideo1Path = string(argv[iArgsCount + 1]);
            if (sVideo1Path.length() < 5)
            {
                bValidArgs = false;
                cout
                        << "Check the specified path of the video file or video stream."
                        << endl;
                break;
            }
            iArgsCount++;
        }
        else if (sCommand.compare("-n1") == 0)
        {
            iCamera1No = pUtilities->convertStringToInteger(
                    string(argv[iArgsCount + 1]));
            if (iCamera1No < 0 || iCamera1No > 10)
            {
                bValidArgs = false;
                cout << "Camera No. should be specified in the range [0 10]"
                        << endl;
                break;
            }
            iArgsCount++;
        }
        else if (sCommand.compare("-cm1") == 0)
        {
            iCaptureMode1 = pUtilities->convertStringToInteger(
                    string(argv[iArgsCount + 1]));
            if (!(iCaptureMode1 == (int)VIDEO
                    || iCaptureMode1 == (int)IMAGE_LIST
                    || iCaptureMode1 == (int)IMAGE))
            {
                bValidArgs = false;
                cout
                        << "Capture mode should be set for emulator only, from the set {video = 1, image_list = 2, image = 3}"
                        << endl;
                break;
            }
            iArgsCount++;
        }
        else if (sCommand.compare("-w1") == 0)
        {
            iCamera1Width = pUtilities->convertStringToInteger(
                    string(argv[iArgsCount + 1]));
            if (iCamera1Width < 320 || iCamera1Width > 2592)
            {
                bValidArgs = false;
                cout << "Width of the image should be in the range [320, 2592]"
                        << endl;
                break;
            }
            iArgsCount++;
        }
        else if (sCommand.compare("-h1") == 0)
        {
            iCamera1Height = pUtilities->convertStringToInteger(
                    string(argv[iArgsCount + 1]));
            if (iCamera1Height < 240 || iCamera1Height > 1944)
            {
                bValidArgs = false;
                cout << "Height of the image should be in the range [240, 1944]"
                        << endl;
                break;
            }
            iArgsCount++;
        }
        else if (sCommand.compare("-fps") == 0)
        {
            fFPS = pUtilities->convertStringToFloat(
                    string(argv[iArgsCount + 1]));
            if (fFPS < 0.1 || fFPS > 100.0)
            {
                bValidArgs = false;
                cout << "FPS should be in the range [0.1, 100.0]" << endl;
                break;
            }
            iArgsCount++;
        }
#if 0
        else if( sCommand.compare( "-i" ) == 0 )
        {
            iImageType = pUtilities->convertStringToInteger( string( argv[iArgsCount+1]) );
            if( !( iImageType == MONOCHROME || iImageType == COLOR ) )
            {
                bValidArgs = false;
                cout << "Image type should be from the set {monochrome, color}" << endl;
                break;
            }
            iArgsCount++;
        }
#endif
        else if (sCommand.compare("-numframestoskip") == 0)
        {
            iNumFramesToSkip = pUtilities->convertStringToInteger(
                    string(argv[iArgsCount + 1]));
            iArgsCount++;
        }
        else if (sCommand.compare("-nf") == 0)
        {
            iNumFramesToGrab = pUtilities->convertStringToInteger(
                    string(argv[iArgsCount + 1]));
            iArgsCount++;
        }
        else if (sCommand.compare("-process") == 0)
        {
            bProcessFrames = true;
        }
        else if (sCommand.compare("-savevideo") == 0)
        {
            bSaveVideo = true;
        }
        else if (sCommand.compare("-c") == 0)
        {
            sConfigFile = string(argv[iArgsCount + 1]);
            // Check if the specified file is a valid file
            bool bIsFile = false;
            bool bIsFileOrFolder = pUtilities->getIsFileOrFolderPresent(
                    sConfigFile, bIsFile);
            if (!bIsFileOrFolder || !bIsFile)
            {
                bValidArgs = false;
                cout << "Invalid configuration file" << endl;
                break;
            }
            bSetConfigFile = true;
            iArgsCount++;
        }
        else if (sCommand.compare("-setregionsfromconfigfile") == 0)
        {
            bSetRegionsFromConfigFile = true;
        }
        else if (sCommand.compare("-dispimgcapture") == 0)
        {
            bDispImgCapture = true;
        }
        else if (sCommand.compare("-dispGraph") == 0 )
        {
        	bDispGraph = true;
        }
        else if (sCommand.compare("-d") == 0)
        {
            bDebug = true;
        }
        else if (sCommand.compare("-paramsfile") == 0)
		{
			sParamsFile = string(argv[iArgsCount + 1]);
			// Check if the specified file is a valid file
			bool bIsFile = false;
			bool bIsFileOrFolder = pUtilities->getIsFileOrFolderPresent(sParamsFile, bIsFile);
			if (!bIsFileOrFolder || !bIsFile)
			{
				bValidArgs = false;
				cout << "Invalid parameter file" << endl;
				break;
			}
			bSetParamsFile = true;
			iArgsCount++;
		}
        else if (sCommand.compare("-disableDispWindow")==0)
        {
        	bDisableDispWindows = true;
        }else if (sCommand.compare("-enableMask")==0)
        {
        	bEnableMask = true;
        }
        else
        {
            cout << "Invalid option: " << sCommand << endl;
            bValidArgs = false;
            break;
        }
        iArgsCount++;
    }

    // == Additional checks on the inputs ==
    if (bSetRegionsFromConfigFile && !bSetConfigFile)
    {
        bValidArgs = false;
        cout
                << "Setting the regions from config file requires setting the config file."
                << endl;
    }

    if (bSetParamsFile)
	{
		DriverAwareness::setParamsFile((char*)string(sParamsFile).c_str());
	}

    if (sCamera1.compare(string("")) == 0)
    {
        bValidArgs = false;
        cout << "Please specify necessary information for camera 1." << endl;
    }

    if (sCamera1.compare(string("usb")) == 0)
    {
        if (iCamera1No < 0 || iCamera1No > 10)
        {
            bValidArgs = false;
            cout << "Please provide a valid camera number for camera 1."
                    << endl;
        }
    }

    if (sCamera1.compare(string("emulator")) == 0
            || sCamera1.compare(string("iqeye")) == 0)
    {
        if (sVideo1Path.length() < 5)
        {
            bValidArgs = false;
            cout
                    << "Please provide a valid path for video file or video stream of camera 1."
                    << endl;
        }
    }

    if (sCamera1.compare(string("emulator")) == 0)
    {
        if (iCaptureMode1 == -1)
        {
            bValidArgs = false;
            cout << "Please specify the capture mode for camera 1: emulator."
                    << endl
                    << "Capture mode should be set from the set {video = 1, image_list = 2, image = 3} "
                    << endl;
        }
    }

    if (sCamera1.length() > 0)
    {
        if (iCamera1Width == -1)
        {
            bValidArgs = false;
            cout << "Please specify width of the image for camera 1." << endl;
        }
        if (iCamera1Height == -1)
        {
            bValidArgs = false;
            cout << "Please specify height of the image for camera 1." << endl;
        }
    }

    if (argc < 3 || !bValidArgs)
    {
        cout << "Input command: " << endl;
        iArgsCount = 1;
        while (iArgsCount < argc)
        {
            cout << string(argv[iArgsCount++]) << " ";
        }
        cout << "\n\n<Usage>" << endl
                << "(1) ./ClassTest -c1 iqeye w1 640 h1 480 -n1 0 " << endl
                << "\t-p1 http://10.0.6.2/now.jpg?snap=spush&pragma=motion"
                << endl << "\t-c1 iqeye: Sets camera 1 to iqeye" << endl
                << "\t-n1 0: The camera no. of camera 1 is set to 0" << endl
                << "(2) ./ClassTest -c1 usb w1 640 h1 480 -n1 0 " << endl
                << "(3) ./ClassTest -c1 iqeye w1 640 h1 480 -p1 rtsp://192.168.0.202/now.mp4"
                << endl
                << "\t-p1 rtsp://192.168.0.202/now.mp4: Sets the path of the "
                << endl
                << "\t\t video stream of first camera to specified string"
                << endl
                << "(4) ./ClassTest -c1 emulator w1 640 h1 480 -cm1 1 -p1 ../../videos/Hua-Hin_01.avi"
                << endl << "\t-c1 emulator: Sets camera 1 to emulator." << endl
                << "\t-cm1 1: Sets capture mode for camera 1 to VIDEO" << endl
                << "\t-p1 ../../videos/Hua-Hin_01.avi: Sets the path of the video file."
                << endl << endl
                << "(5) ./ClassTest -c1 emulator w1 640 h1 480 -cm1 2 -p1 img_list_folder"
                << endl
                << "\t-cm1 2: Sets capture mode for camera 1 to IMAGE_LIST_FOLDER."
                << endl
                << "\t-p1 img_list_folder: Sets the image list folder to specified directory."
                << endl << endl
                << "(6) ./ClassTest -c1 emulator w1 640 h1 480 -cm1 3 -p1 image_file"
                << endl << "\t-cm1 2: Sets capture mode for camera 1 to IMAGE."
                << endl
                << "\t-p1 image: Sets the image to specified image file."
                << endl << endl << "(7) ./ClassTest -c1 emulator w1 640 h1 480 "
                << "--cm1 1 p1 ../../videos/Hua-Hin_01.avi -process "
                << " -savevideo -c ../ROI.config -setregionsfromconfigfile"
                << endl << "\t-process : Process images" << endl
                << "\t-savevideo : Save video using images captured from camera 1"
                << endl << "\t-c config_file : Specify config file" << endl
                << "\t-setregionsfromconfigfile : Set region from config file"
                << endl << endl << "--- Additional options for debug mode ---"
                << endl
                << "\tUse -d option to enable debug mode (if not using CGI)"
                << endl;
        return 0;
    }

    // == Set parameters to Class class ==
    DriverAwareness::setNumFramesToGrab(iNumFramesToGrab);
    DriverAwareness::setNumFramesToIgnore(iNumFramesToSkip);
    DriverAwareness::setSaveVideo(bSaveVideo);
    DriverAwareness::setCamera1Resolution(iCamera1Width, iCamera1Height);
    if (bSetConfigFile)
    {
        DriverAwareness::setConfigFile((char*)string(sConfigFile).c_str());
        DriverAwareness::setRegionsFromConfigFile(bSetRegionsFromConfigFile);
    }

    // Parameters for the first camera
    DriverAwareness::setCaptureMode1(iCaptureMode1);
    DriverAwareness::setImageType1(iImageType);
    DriverAwareness::setCamera1(sCamera1);
    DriverAwareness::setCamera1No(iCamera1No);
    DriverAwareness::setVideo1Path(sVideo1Path);
    DriverAwareness::setCamera1Resolution(iCamera1Width, iCamera1Height);
    DriverAwareness::setFPS(fFPS);

    // Displaying parameters in console
    cout << "Parameters specified by the user" << endl;
    cout << "No. of frames to grab: " << iNumFramesToGrab << endl;
    cout << "Camera 1: " << sCamera1 << endl;
    cout << "Capture mode for camera 1: " << pchArrCaptureMode[iCaptureMode1]
            << endl;
    cout << "Image type: " << pchArrImageType[iImageType] << endl;
    cout << "Camera No.: " << iCamera1No << endl;
    cout << "Video Path: " << sVideo1Path << endl;
    cout << "Image resolution: " << iCamera1Width << " x " << iCamera1Height
            << endl;
    cout << "Process frames: " << pchArrBoolean[bProcessFrames] << endl;
    cout << "Save video: " << pchArrBoolean[bSaveVideo] << endl;
    cout << "Frame rate of capture: " << fFPS << " fps" << endl;
    if (bSetConfigFile)
    {
        cout << "Config file: " << sConfigFile << endl;
        cout << "Set regions from config file: "
                << pchArrBoolean[bSetRegionsFromConfigFile] << endl;
    }
    cout << endl;

    // Parameters to display image windows
    DriverAwareness::setDisableDispWindows(bDisableDispWindows);
    DriverAwareness::setDispImgCapture(bDispImgCapture);
    // Parameter to process frame
    DriverAwareness::setProcessFrames(bProcessFrames);
    // Parameter to enable mask to ignore rear view mirror
    DriverAwareness::setEnableMask(bEnableMask);


    // Parameter to display graph
    DriverAwareness::setDispGraph(bDispGraph);

    cout << "Input arguments ................................ [ OK ]" << endl;
    if (ENABLE_PAUSE)
    {
        cout << "Press ENTER to continue..." << endl;
        cin.get();
    }

    // Start the application
    DriverAwareness *pClass = new DriverAwareness();
    pClass->startApp(bDebug);

    if (pClass) delete pClass;
    if (pUtilities) delete pUtilities;
    pLoggerTest = NULL;
    pErrorHandlerTest = NULL;
    cout << "Program terminated." << endl;
    kill(getpid(), SIGKILL);
    return 0;
}
