/*
 * CameraBasler.cpp
 * Ramesh Marikhu <marikhu@gmail.com> Oct 2011
 */

#include "CameraBasler.h"

#define clip(x) (unsigned char)( (x) < 0 ? 0 : ( (x) > 255 ? 255 : (x) ) )
//#define clip(x) x && 255 // TODO: Check if this is faster, if the function is used.
#define max(x,y) x > y ? x : y
#define min(x,y) x < y ? x : y


ErrorHandler* CameraBasler::_pErrorHandler = NULL;
Logger* CameraBasler::_pLogger = NULL;
bool CameraBasler::_bIsSetErrorDiagnosticsList = false;

teCaptureMode CameraBasler::_eCaptureMode = (teCaptureMode)CAPTURE_MODE;
teImageType CameraBasler::_eImageType = (teImageType)IMAGE_TYPE;
teAcquisitionMode CameraBasler::_eAcquisitionMode = (teAcquisitionMode)ACQUISITION_MODE;
teExposureMode CameraBasler::_eExposureMode = (teExposureMode)EXPOSURE_MODE;
int64 CameraBasler::_iExposureRaw = EXPO_RAW_INIT;
teGainMode CameraBasler::_eGainMode = (teGainMode)GAIN_MODE;
int64 CameraBasler::_iGainRaw = GAIN_RAW_INIT;
double CameraBasler::_dFrameRate = FRAMES_PER_SEC;
int CameraBasler::_iObjCount = 0;
int CameraBasler::_iNumMaxRows = 0;
int CameraBasler::_iNumMaxCols = 0;
int CameraBasler::_iNumRows = 0;
int CameraBasler::_iNumCols = 0;
bool CameraBasler::_bPylonAutoGainControl = PYLON_AUTO_GAIN_CONTROL;
bool CameraBasler::_bCustomAutoGainControl = CUSTOM_AUTO_GAIN_CONTROL;
bool CameraBasler::_bUpdateCustomGain = UPDATE_CUSTOM_GAIN;
bool CameraBasler::_bUpdateCustomExposure = UPDATE_CUSTOM_EXPOSURE;


void CameraBasler::initErrorDiagnosticsList()
{
    if( !CameraBasler::_bIsSetErrorDiagnosticsList )
    {
#define CAMERABASLER_INVALID_IMAGE_TYPE                      2701
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_IMAGE_TYPE,
            string( "CameraBasler : Invalid image type." ) );
#define CAMERABASLER_ERROR_INITIALIZING_CAMERA               2702
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_INITIALIZING_CAMERA,
            string( "CameraBasler : Error occurred when initializing the camera." ) );
#define CAMERABASLER_ERROR_CREATING_TRANSPORT_LAYER          2703
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_CREATING_TRANSPORT_LAYER,
            string( "CameraBasler : Error creating transport layer." ) );
#define CAMERABASLER_NO_CAMERA_PRESENT                       2704
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_NO_CAMERA_PRESENT,
            string( "CameraBasler : No camera present." ) );
#define CAMERABASLER_IMAGE_NOT_SET                           2705
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_IMAGE_NOT_SET,
            string( "CameraBasler : Image is not set yet." ) );
#define CAMERABASLER_INVALID_NULL_IMAGE                      2706
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_NULL_IMAGE,
            string( "CameraBasler : Invalid null image." ) );
#define CAMERABASLER_INVALID_MULTICHANNEL_IMAGE              2707
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_MULTICHANNEL_IMAGE,
            string( "CameraBasler : Invalid multichannel image." ) );
#define CAMERABASLER_INVALID_IMAGE_DEPTH                     2708
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_IMAGE_DEPTH,
            string( "CameraBasler : Invalid image depth." ) );
#define CAMERABASLER_NULL_IMAGE                              2709
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_NULL_IMAGE,
            string( "CameraBasler : Null image. Image is not initialized." ) );
#define CAMERABASLER_ERROR_GRABBING_IMAGE                    2710
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_GRABBING_IMAGE,
            string( "CameraBasler : Error grabbing image from camera." ) );
#define CAMERABASLER_ERROR_CLOSING_CAMERA                    2711
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_CLOSING_CAMERA,
            string( "CameraBasler : Error closing camera." ) );
#define CAMERABASLER_CAMERA_IS_STILL_OPEN                    2712
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_CAMERA_IS_STILL_OPEN,
            string( "CameraBasler : Camera should be closed before exiting capture from camera." ) );
#define CAMERABASLER_INPUT_IMAGE_IS_INCONSISTENT             2713
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INPUT_IMAGE_IS_INCONSISTENT,
            string( "CameraBasler : Input image is inconsistent." ) );
#define CAMERABASLER_NULL_MATRIX                             2714
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_NULL_MATRIX,
            string( "CameraBasler : Null matrix. Matrix is not initialized yet." ) );
#define CAMERABASLER_INVALID_FN_CALL_FOR_THIS_EXPO_MODE      2715
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_FN_CALL_FOR_THIS_EXPO_MODE,
            string( "CameraBasler : Invalid function call for this exposure mode." ) );
#define CAMERABASLER_INVALID_FN_CALL_FOR_THIS_GAIN_MODE      2716
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_FN_CALL_FOR_THIS_GAIN_MODE,
            string( "CameraBasler : Invalid function call for this gain mode." ) );
#define CAMERABASLER_ERROR_SETTING_AUTO_EXPO_ONCE            2717
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_SETTING_AUTO_EXPO_ONCE,
            string( "CameraBasler : Error setting auto exposure once." ) );
#define CAMERABASLER_ERROR_SETTING_AUTO_EXPO_CONT            2718
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_SETTING_AUTO_EXPO_CONT,
            string( "CameraBasler : Error setting auto exposure continuous." ) );
#define CAMERABASLER_ERROR_SETTING_EXPO_RAW                  2719
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_SETTING_EXPO_RAW,
            string( "CameraBasler : Error setting raw exposure." ) );
#define CAMERABASLER_ERROR_SETTING_AUTO_GAIN_ONCE            2720
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_SETTING_AUTO_GAIN_ONCE,
            string( "CameraBasler : Error setting auto gain once." ) );
#define CAMERABASLER_ERROR_SETTING_AUTO_GAIN_CONT            2721
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_SETTING_AUTO_GAIN_CONT,
            string( "CameraBasler : Error setting auto gain continuous." ) );
#define CAMERABASLER_ERROR_SETTING_GAIN_RAW                  2722
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_ERROR_SETTING_GAIN_RAW,
            string( "CameraBasler : Error setting raw gain." ) );
#define CAMERABASLER_INVALID_MAX_ROWS_COLS_SPECIFIED         2723
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_MAX_ROWS_COLS_SPECIFIED,
            string( "CameraBasler : Invalid number of max rows or max cols specified." ) );
#define CAMERABASLER_INVALID_ROWS_COLS_SPECIFIED             2724
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_ROWS_COLS_SPECIFIED,
            string( "CameraBasler : Invalid number of rows or cols specified." ) );
#define CAMERABASLER_MAX_IMAGE_RESOLUTION_NOT_SET_YET        2725
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_MAX_IMAGE_RESOLUTION_NOT_SET_YET,
            string( "CameraBasler : Max image resolution is not set yet." ) );
#define CAMERABASLER_IMAGE_RESOLUTION_NOT_SET_YET            2726
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_IMAGE_RESOLUTION_NOT_SET_YET,
            string( "CameraBasler : Image resolution is not set yet." ) );
#define CAMERABASLER_INVALID_CAPTURE_MODE                   2727
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_CAPTURE_MODE,
            string( "CameraBasler : Invalid capture mode. Try CameraEmulator instead." ) );
#define CAMERABASLER_INVALID_IMAGE                           2728
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_IMAGE,
            string( "CameraBasler : Invalid image." ) );
#define CAMERABASLER_EXPOSURE_OUT_OF_RANGE                   2729
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_EXPOSURE_OUT_OF_RANGE,
            string( "CameraBasler : Exposure value is out of range." ) );
#define CAMERABASLER_GAIN_OUT_OF_RANGE                       2730
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_GAIN_OUT_OF_RANGE,
            string( "CameraBasler : Gain value is out of range." ) );
#define CAMERABASLER_INVALID_PIXEL_FORMAT					 2731
        _pErrorHandler->insertErrorDiagnostics( (int)CAMERABASLER_INVALID_PIXEL_FORMAT,
			string( "CameraBasler : Invalid pixel format." ) );

        // Escapable Exceptions
    }
    CameraBasler::_bIsSetErrorDiagnosticsList = true;
}


CameraBasler::CameraBasler( int iCameraNo )
{
	CameraBasler::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
	_pClock = new Clock();
	_pTimer = new Timer();
	_pUtilities = new Utilities();

	initErrorDiagnosticsList();

    _pCamera = NULL;
    _iCameraNo = iCameraNo;
    _pStreamGrabber = NULL;
    _iNumBuffers = NUM_BUFFERS;
    _bIsGrabComplete = false;
	_iGrabbedFrameNo = 0;

    _pImage = NULL;
    _pImageYCrCb = NULL;
	_pImagePrev = NULL;
    _pImageRectified = NULL;
	_sPixelFormat = string( "" );

    RGBPixel1.b = 0, RGBPixel1.g = 0, RGBPixel1.r = 0;
    RGBPixel2.b = 0, RGBPixel2.g = 0, RGBPixel2.r = 0;

    _lTimeStampOfCaptureInSec = 0;
    _bToggleGainAndExpComputation = true;
}


CameraBasler::~CameraBasler()
{
	CameraBasler::_iObjCount--;
	if( _pClock ) delete _pClock;
	if( _pTimer ) delete _pTimer;
	if( _pUtilities ) delete _pUtilities;
	if( _pErrorHandler ) _pErrorHandler = NULL;
	if( _pLogger ) _pLogger = NULL;

    if( _pImage ) cvReleaseImage( &_pImage );
    if( _pImageYCrCb ) cvReleaseImage( &_pImageYCrCb );
    if( _pImagePrev ) cvReleaseImage( &_pImagePrev );
    if( _pImageRectified ) cvReleaseImage( &_pImageRectified );

    if( _pCamera ) delete _pCamera;
    if( _pStreamGrabber ) delete _pStreamGrabber;
}


string CameraBasler::getOutputFolder()
{
    return string( Logger::getInstance()->getOutputFolder() );
}


void CameraBasler::loadMatFromConfig()
{
	if( !( _pMatK && _pMatD ) )
	{
		_pErrorHandler->setErrorCode( (int)CAMERABASLER_NULL_MATRIX );
	}
	cvZero( _pMatK );
	CV_MAT_ELEM( *_pMatK, float, 0, 0 ) = BASLER_DEF_FOCAL_X;
	CV_MAT_ELEM( *_pMatK, float, 1, 1 ) = BASLER_DEF_FOCAL_Y;
	CV_MAT_ELEM( *_pMatK, float, 0, 2 ) = BASLER_DEF_CENTER_X;
	CV_MAT_ELEM( *_pMatK, float, 1, 2 ) = BASLER_DEF_CENTER_Y;
	CV_MAT_ELEM( *_pMatK, float, 2, 2 ) = 1.0f;
	_bIsSetMatK = true;

	cvZero( _pMatD );
	CV_MAT_ELEM( *_pMatD, float, 0, 0 ) = BASLER_DIST_COEFFS_K1;
	CV_MAT_ELEM( *_pMatD, float, 0, 1 ) = BASLER_DIST_COEFFS_K2;
	CV_MAT_ELEM( *_pMatD, float, 0, 2 ) = BASLER_DIST_COEFFS_P1;
	CV_MAT_ELEM( *_pMatD, float, 0, 3 ) = BASLER_DIST_COEFFS_P2;
	_bIsSetMatD = true;
}


void CameraBasler::initializeCamera( bool bDebug )
{
	// TODO: Add code to mock Camera initialization when CAPTURE_MODE = IMAGE or IMAGE_LIST or VIDEO
	try
	{
		// Get the transport layer factory
		CTlFactory& TlFactory = CTlFactory::GetInstance();

		// Create the transport layer object needed to enumerate or
		// create a camera object of type Camera_t::DeviceClass()
		ITransportLayer *pTl = TlFactory.CreateTl( Camera_t::DeviceClass() );

		// Exit the application if the specific transport layer is not available
		if ( !pTl )
		{
			//cerr << "Failed to create transport layer!" << endl;
			_pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_CREATING_TRANSPORT_LAYER );
		}

		// Get all attached cameras and exit the application if no camera is found
		DeviceInfoList_t devices;
		if ( pTl->EnumerateDevices(devices) == 0 )
		{
			//cerr << "No camera present!" << endl;
			_pErrorHandler->setErrorCode( (int)CAMERABASLER_NO_CAMERA_PRESENT );
		}

		// Create the camera object of the first available camera.
		// The camera object is used to set and get all available camera features.
		_pCamera = new Camera_t( pTl->CreateDevice( devices[0] ) );

		// Open the camera
		_pCamera->Open();

		// Set the Packet Size, the larger the packet size, the higher the fps
		// GevSCPSPacketSize sets the packet size in bytes for the selected stream channel.
		_pCamera->GevSCPSPacketSize.SetValue( PACKET_SIZE );

		// Get the first stream grabber object of the selected camera
		_pStreamGrabber = new Camera_t::StreamGrabber_t( _pCamera->GetStreamGrabber(0) );

		// Open the stream grabber
		_pStreamGrabber->Open();

		// Set the image format and AOI
        int iWidth = (int)_pCamera->Width.GetMax();
        int iHeight = (int)_pCamera->Height.GetMax();
		if( CUSTOM_SIZE_OF_GRABBED_IMAGE )
		{
            iWidth = BASLER_NUM_COLS;
            iHeight = BASLER_NUM_ROWS;
		}
		if( CameraBasler::_eImageType == MONOCHROME )
		{
			_sPixelFormat = "PixelFormat_Mono8";
			_pCamera->PixelFormat.SetValue( PixelFormat_Mono8 );
			_pImage = cvCreateImage(cvSize(iWidth, iHeight), IPL_DEPTH_8U, 1);
		}
		else if( CameraBasler::_eImageType == BLUE_CHANNEL )
		{
			_sPixelFormat = "PixelFormat_Mono8_As_Blue_Channel";
			_pCamera->PixelFormat.SetValue( PixelFormat_Mono8 );
			_pImage = cvCreateImage(cvSize(iWidth, iHeight), IPL_DEPTH_8U, 3);
		}
		else if( CameraBasler::_eImageType == COLOR )
		{
			_sPixelFormat = "PixelFormat_YUV422Packed";
			_pCamera->PixelFormat.SetValue( PIXEL_FORMAT );
			_pImage = cvCreateImage(cvSize(iWidth, iHeight), IPL_DEPTH_8U, 3);
			_pImageYCrCb = cvCreateImage( cvSize( iWidth, iHeight ), IPL_DEPTH_8U, 3 );
		}
		else
		{
			_pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_IMAGE_TYPE );
		}

		// In order to ensure that the widthStep is fixed to actual size because
		// OpenCV internally updates it to be divisible by 4 for speedup, it is
		// essential to specify widthStep and imageSize explicitly.
		_pImage->widthStep = _pImage->width * _pImage->nChannels;
		_pImage->imageSize = _pImage->widthStep * _pImage->height;
		if( CameraBasler::_eImageType == COLOR )
		{
			_pImageYCrCb->widthStep = _pImageYCrCb->width * _pImageYCrCb->nChannels;
			_pImageYCrCb->imageSize = _pImageYCrCb->widthStep * _pImageYCrCb->height;
		}
		cvSetZero( _pImage );

		// Set camera width, height and offset
		// Obtain max width and height
		if( _iNumMaxRows == 0  || _iNumMaxCols ==0  )
		{
            _iNumMaxRows = (int)_pCamera->Height.GetMax();
            _iNumMaxCols = (int)_pCamera->Width.GetMax();
		}
		else
		{
		    // Verify specified size
            if ( ( _iNumMaxRows != (int)_pCamera->Height.GetMax() ) ||
                 ( _iNumMaxCols != (int)_pCamera->Width.GetMax() ) )
            {
                _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_MAX_ROWS_COLS_SPECIFIED );
            }
		}
		// Obtain specified width and height
        if( CUSTOM_SIZE_OF_GRABBED_IMAGE )
        {
            _iNumRows = BASLER_NUM_ROWS;
            _iNumCols = BASLER_NUM_COLS;
        }
        else
        {
            _iNumRows = _iNumMaxRows;
            _iNumCols = _iNumMaxCols;
        }
        // Verify limits
        if( ( ( _iNumRows + BASLER_OFFSET_Y ) > _iNumMaxRows ) ||
            ( ( _iNumCols + BASLER_OFFSET_X ) > _iNumMaxCols ) ||
            ( ( _iNumRows + BASLER_OFFSET_Y ) < 2 ) ||
            ( ( _iNumCols + BASLER_OFFSET_X ) < 2 ) )
        {
            _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_ROWS_COLS_SPECIFIED );
        }
		_pCamera->Width.SetValue( _iNumCols );
		_pCamera->Height.SetValue( _iNumRows );
        _pCamera->OffsetX.SetValue( BASLER_OFFSET_X );
        _pCamera->OffsetY.SetValue( BASLER_OFFSET_Y );
        cout << "Image Size: " << _iNumCols << " x " << _iNumRows << " : " << endl <<
                "Offset: " << BASLER_OFFSET_X << ", " << BASLER_OFFSET_Y << endl;

		//Disable acquisition start trigger if available
		{
			GenApi::IEnumEntry* acquisitionStart = _pCamera->TriggerSelector.GetEntry( TriggerSelector_AcquisitionStart);
			if ( acquisitionStart && GenApi::IsAvailable( acquisitionStart ) )
			{
				_pCamera->TriggerSelector.SetValue( TriggerSelector_AcquisitionStart );
				_pCamera->TriggerMode.SetValue( TriggerMode_Off );
			}
		}

		//Disable frame start trigger if available
		{
			GenApi::IEnumEntry* frameStart = _pCamera->TriggerSelector.GetEntry( TriggerSelector_FrameStart);
			if ( frameStart && GenApi::IsAvailable( frameStart ) )
			{
				_pCamera->TriggerSelector.SetValue( TriggerSelector_FrameStart );
				_pCamera->TriggerMode.SetValue( TriggerMode_Off );
			}
		}

		//Set acquisition mode
		if( CameraBasler::_eAcquisitionMode == ACQ_CONT )
		{
			_pCamera->AcquisitionMode.SetValue( AcquisitionMode_Continuous );
		}
		else if( CameraBasler::_eAcquisitionMode == ACQ_FRAME )
		{
			_pCamera->AcquisitionMode.SetValue( AcquisitionMode_SingleFrame );
		}

		// Get the image buffer size
		const size_t iImageSize = (size_t)(_pCamera->PayloadSize.GetValue());

		// We won't use image buffers greater than iImageSize
		_pStreamGrabber->MaxBufferSize.SetValue(iImageSize);

		// We won't queue more than iNumBuffers image buffers at a time
		_pStreamGrabber->MaxNumBuffer.SetValue( _iNumBuffers );

		// Allocate all resources for grabbing. Critical parameters like image
		// size now must not be changed until FinishGrab() is called.
		_pStreamGrabber->PrepareGrab();

		// Buffers used for grabbing must be registered at the stream grabber.
		// The registration returns a handle to be used for queuing the buffer.
		for ( int i = 0; i < _iNumBuffers; ++i )
		{
			CGrabBuffer *pGrabBuffer = new CGrabBuffer( iImageSize );
			pGrabBuffer->SetBufferHandle(_pStreamGrabber->RegisterBuffer(
				pGrabBuffer->GetBufferPointer(), iImageSize ) );

			// Put the grab buffer object into the buffer list
			_vBufferList.push_back( pGrabBuffer );
		}

		for (std::vector<CGrabBuffer*>::const_iterator x = _vBufferList.begin(); x != _vBufferList.end(); ++x)
		{
			// Put buffer into the grab queue for grabbing
			_pStreamGrabber->QueueBuffer((*x)->GetBufferHandle(), NULL);
		}

		// Set FrameRate
		_pCamera->AcquisitionFrameRateEnable.SetValue( true );
		_pCamera->AcquisitionFrameRateAbs.SetValue( _dFrameRate );

		// Let the camera acquire images continuously ( Acquisition mode equals Continuous! )
		if( ACQUISITION_MODE == ACQ_CONT )
		{
			// Let the camera acquire images
			_pCamera->AcquisitionStart.Execute();
		}

		if( CameraBasler::_eExposureMode == EXPO_RAW )
		{
			// Fixed Exposure; low values of exposure = faster capture rate
			CameraBasler::_iExposureRaw = EXPO_RAW_INIT;
		}
		activateExposureMode();

		if( CameraBasler::_eGainMode == GAIN_RAW )
		{
			CameraBasler::_iGainRaw = GAIN_RAW_INIT;
		}
		activateGainMode();
	}
	catch ( GenICam::GenericException &e )
	{
		//cerr << "A GenICam exception occurred: " << e.GetDescription() << endl;
		_pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_INITIALIZING_CAMERA, string( e.GetDescription() ) );
	}
}


void CameraBasler::grabImage( bool bDebug )
{
    try
    {
        if( _iCaptureMode == (int)CAMERA )
        {
            if( _bStopCapture || _bIsGrabComplete ) return;
            if( _iGrabbedFrameNo < _iNumFramesToGrab )
            {
                _bIsGrabComplete = false;
                _bStopCapture = false;
            }
            else
            {
                _bIsGrabComplete = true;
                _bStopCapture = true;
                return;
            }

            _bIsSetImage = false;
            if( CameraBasler::_bCustomAutoGainControl )
            {
                if( _bUpdateCustomGain )
                {
                    if( ( CameraBasler::_iGainRaw >= GAIN_MIN ) &&
                        ( CameraBasler::_iGainRaw <= GAIN_MAX ) )
                    {
                        _pCamera->GainRaw.SetValue( CameraBasler::_iGainRaw );
                    }
                    else
                    {
                        _pErrorHandler->setErrorCode( (int)CAMERABASLER_GAIN_OUT_OF_RANGE );
                    }
                    if( bDebug )
                    {
                        cout << "Custom GainRaw = " << CameraBasler::_iGainRaw << endl;
                    }
                }

                if( _bUpdateCustomExposure )
                {
                    // During capture, I do not want to change the exposure.
                    // Reason: The capture involves grabbing high speed environment. So, increase in
                    // the exposure would result in blur images.
                    // Solution: Keeping the exposure constant, the gain is auto-adjusted.
                    // Problem: When increasing gain, the noise also gets amplified along with the signal.
                    if( ( CameraBasler::_iExposureRaw >= EXPOSURE_MIN ) &&
                        ( CameraBasler::_iExposureRaw <= EXPOSURE_MAX ) )
                    {
                        _pCamera->ExposureTimeRaw.SetValue( CameraBasler::_iExposureRaw );
                    }
                    else
                    {
                        _pErrorHandler->setErrorCode( (int)CAMERABASLER_EXPOSURE_OUT_OF_RANGE );
                    }
                    if( true ) //bDebug
                    {
                        cout << "Custom ExposureRaw = " << CameraBasler::_iExposureRaw << endl;
                    }
                }
            }

            if( CameraBasler::_eAcquisitionMode == ACQ_FRAME )
            {
                // Start the acquisition process after every grabbed frame
                _pCamera->AcquisitionStart.Execute();
            }

            // Wait for the grabbed image with timeout of 3 seconds
            // TODO: while outside of GetWaitObject()?
            GrabResult Result;
            while( !_bIsSetImage )
            {
                if (_pStreamGrabber->GetWaitObject().Wait(3000))
                {
                    // Get the grab result from the grabber's result queue
                    _pStreamGrabber->RetrieveResult( Result );
                    // Result.GetTimeStamp(); // Returns 0, seems it is not supported.
                    if ( Result.Status() == Grabbed )
                    {
                        // Grabbing was successful, process image
                        if( _iGrabbedFrameNo == 0 )
                        {
                            _tvTimeOfStartCapture = _pTimer->getCurrentTime();
                            _tvTimeOfFrameCapture = _tvTimeOfStartCapture;
                        }
                        else
                        {
                            _tvTimeOfFrameCapture = _pTimer->getCurrentTime();
                        }
                        _lTimeStampOfCaptureInSec = _pClock->getCurrentTimeStampInSec();

                        if( bDebug )
                        {
                            cout << "Image #" << _iGrabbedFrameNo << " acquired!" << endl;
                            cout << "Size: " << Result.GetSizeX() << " x " << Result.GetSizeY() << endl;
                            cout << "PayloadSize = " << Result.GetPayloadSize() << endl;
                        }

                        // Get the pointer to the image buffer
                        const unsigned char *pImageBuffer = (unsigned char*) Result.Buffer();
                        if( CameraBasler::_eImageType == MONOCHROME )
                        {
                            int iBufferLoc = 0;
#if 0
                            for( int iRow = 0; iRow < _pImage->height; iRow++ )
                            {
                                for( int iCol = 0; iCol < _pImage->width; iCol++ )
                                {
                                    _pImage->imageData[ iRow * _pImage->widthStep + iCol ] = (unsigned char)pImageBuffer[iBufferLoc++];
                                }
                            }
#else
                            char *pchGray = _pImage->imageData;
                            for( int iRow = 0; iRow < _pImage->height; iRow++ )
                            {
                                for( int iCol = 0; iCol < _pImage->width; iCol++ )
                                {
                                    *pchGray = (unsigned char)pImageBuffer[iBufferLoc++];
                                    pchGray++;
                                }
                            }
#endif
                        }
                        else if( CameraBasler::_eImageType == BLUE_CHANNEL )
                        {
                            char *RGB = _pImage->imageData;
                            int iBufferLoc = 0;
                            for( int iRow = 0; iRow < _pImage->height; iRow++ )
                            {
                                for( int iCol = 0; iCol < _pImage->width; iCol++ )
                                {
                                    *RGB = (unsigned char)pImageBuffer[iBufferLoc++];
                                    RGB +=3;
                                }
                            }
                        }
                        else if( CameraBasler::_eImageType == COLOR )
                        {
                            if( PIXEL_FORMAT == PixelFormat_BayerBG8 )
                            {
                                convertBayerBG2ImageToBGRImage( pImageBuffer, _pImage );
                            }
                            else if( PIXEL_FORMAT == PixelFormat_YUV422Packed )
                            {
#if 1
                                convertYCrCbImageToBGRImage( pImageBuffer, _pImage );
#elif 1
                                convertYUV422PackedImageToBGRImage( pImageBuffer, _pImage );
#else	// Slower than convertYUV422PackedImageToBGRImage
                                convertYUV422PackedImageToBGRImage2( pImageBuffer, _pImage );
#endif
                            }
                            else
                            {
                                _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_PIXEL_FORMAT );
                            }
                        }
                        if( CameraBasler::_bCustomAutoGainControl )
                        {
                            int iExposureRaw = (int)CameraBasler::_iExposureRaw;
                            int iGainRaw = (int)CameraBasler::_iGainRaw;
                            if( CameraBasler::_bUpdateCustomGain && CameraBasler::_bUpdateCustomExposure )
                            {
                                _bToggleGainAndExpComputation = !_bToggleGainAndExpComputation;
                            }
                            else
                            {
                                _bToggleGainAndExpComputation = true; // No need to toggle
                            }
                            getCameraParamsToUpdate( _pImage,
                                iExposureRaw, iGainRaw, (int)CameraBasler::_eImageType,  false );
                            CameraBasler::_iExposureRaw = iExposureRaw;
                            CameraBasler::_iGainRaw = iGainRaw;
                        }
                        _iGrabbedFrameNo++;
                        _bIsSetImage = true;
                    }
                    else if ( Result.Status() == Failed )
                    {
                        // Error handling
                        cerr << "No image acquired!" << endl;
                        cerr << "Error code : 0x" << hex << Result.GetErrorCode() << endl;
                        cerr << "Error description : " << Result.GetErrorDescription() << endl;
                        _bIsSetImage = false;
                    }
                }
                else
                {
                    // Timeout
                    cerr << "Timeout occurred!" << endl;

                    // Get the pending buffer back (You are not allowed to de-register
                    // buffers when they are still queued)
                    _pStreamGrabber->CancelGrab();

                    // Get all buffers back
                    for ( GrabResult r; _pStreamGrabber->RetrieveResult(r); );
                }
                if( !( _bStopCapture || _bIsGrabComplete ) )
                {
                    // Reuse the buffer for grabbing the next image - very essential.
                    if( _iGrabbedFrameNo < _iNumFramesToGrab + _iNumBuffers )
                    {
                        _pStreamGrabber->QueueBuffer( Result.Handle(), NULL );
                    }
                }
                if( CameraBasler::_eAcquisitionMode == ACQ_FRAME )
                {
                    _pCamera->AcquisitionStop.Execute();  // Start the acquisition process after every grabbed frame
                }
            }
        }
        else
        {
            _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_CAPTURE_MODE );
        }
	}
	catch ( GenICam::GenericException &e )
	{
		//cerr << "A GenICam exception occurred: " << e.GetDescription() << endl;
		_pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_GRABBING_IMAGE, string( e.GetDescription() ) );
	}
}


void CameraBasler::grabTuningImage( bool bTuningGain, bool bTuningExposure, bool bDebug )
{
    try
    {
		if( CameraBasler::_eAcquisitionMode == ACQ_FRAME )
        {
            _pCamera->AcquisitionStart.Execute();  // Start the acquisition process after every grabbed frame
        }

		// Wait for the grabbed image with timeout of 3 seconds
        if (_pStreamGrabber->GetWaitObject().Wait(3000))
        {
            // Get the grab result from the grabber's result queue
            GrabResult Result;
            _pStreamGrabber->RetrieveResult(Result);

            if ( Result.Status() == Grabbed )
            {
				if( bDebug )
				{
					cout << "Image grabbed." << endl;
					if( bTuningGain ) cout << "Tuning gain..." << endl;
					if( bTuningExposure ) cout << "Tuning exposure ..." << endl;
				}
            }
            else if ( Result.Status() == Failed )
            {
				if( bDebug )
				{
					cout << "No Image Grabbed when tuning gain/exposure..." << endl;
				}
            }

			// Reuse the buffer for grabbing the next image
			_pStreamGrabber->QueueBuffer( Result.Handle(), NULL );
        }
        else
        {
            // Timeout
            cerr << "Timeout occurred! Exiting grabbing of tuning images." << endl;
        }
		if( CameraBasler::_eAcquisitionMode == ACQ_FRAME )
        {
            _pCamera->AcquisitionStop.Execute();  // Start the acquisition process after every grabbed frame
        }
    }
    catch ( GenICam::GenericException &e )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_GRABBING_IMAGE, string( e.GetDescription() ) );
    }
}


void CameraBasler::closeCamera( bool bDebug )
{
	try
	{
	    if( _iCaptureMode == (int)CAMERA )
	    {
            if( !_pCamera->IsOpen() )
            {
                return;
            }
            else
            {
                // Stop acquisition
                if( CameraBasler::_eAcquisitionMode == ACQ_CONT )
                {
                    // Stop the Acquisition process
                    _pCamera->AcquisitionStop.Execute();
                }

                // Get the pending buffer back. ( You are not allowed to de-register
                // buffers when they are still queued )
                _pStreamGrabber->CancelGrab();
                for ( GrabResult r; _pStreamGrabber->RetrieveResult(r); );
                for (std::vector<CGrabBuffer*>::iterator it = _vBufferList.begin(); it != _vBufferList.end(); it++)
                {
                    _pStreamGrabber->DeregisterBuffer((*it)->GetBufferHandle());
                    delete *it;
                    *it = NULL; // caused exception
                }

                _pStreamGrabber->FinishGrab();	// Free all resources used for grabbing
                _pStreamGrabber->Close();		// Close stream grabber
                _pCamera->Close();				// Close camera
            }
	    }
	    else
	    {
            _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_CAPTURE_MODE );
	    }
	}
	catch ( GenICam::GenericException &e )
	{
		//cerr << "A GenICam exception occurred: " << e.GetDescription() << endl;
		_pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_CLOSING_CAMERA, string( e.GetDescription() ) );
	}
}


// == Getters and Setters ==

int CameraBasler::getObjCount()
{
    return CameraBasler::_iObjCount;
}


int CameraBasler::getAcquisitionMode()
{
    return (int)CameraBasler::_eAcquisitionMode;
}


void CameraBasler::setAcquisitionMode( int iAcquisitionMode )
{
    CameraBasler::_eAcquisitionMode = (teAcquisitionMode)iAcquisitionMode;
}


int CameraBasler::getExposureMode()
{
    return (int)CameraBasler::_eExposureMode;
}


void CameraBasler::setExposureMode( int iExposureMode )
{
    CameraBasler::_eExposureMode = (teExposureMode)iExposureMode;
}


int64 CameraBasler::getExposureRaw()
{
    if( CameraBasler::_eExposureMode == EXPO_AUTO_CONT )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_FN_CALL_FOR_THIS_EXPO_MODE );
    }
    return CameraBasler::_iExposureRaw;
}


void CameraBasler::setExposureRaw( int64 iExposureRaw )
{
    if( CameraBasler::_eExposureMode == EXPO_AUTO_CONT )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_FN_CALL_FOR_THIS_EXPO_MODE );
    }
    CameraBasler::_iExposureRaw = iExposureRaw;
}


int CameraBasler::getGainMode()
{
    return (int)CameraBasler::_eGainMode;
}


void CameraBasler::setGainMode( int iGainMode )
{
    CameraBasler::_eGainMode = (teGainMode)iGainMode;
}


int64 CameraBasler::getGainRaw()
{
    if( CameraBasler::_eGainMode == GAIN_AUTO_CONT )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_FN_CALL_FOR_THIS_GAIN_MODE );
    }
    return CameraBasler::_iGainRaw;
}


void CameraBasler::setGainRaw( int64 iGainRaw )
{
    if( CameraBasler::_eGainMode == GAIN_AUTO_CONT )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_FN_CALL_FOR_THIS_GAIN_MODE );
    }
    CameraBasler::_iGainRaw = iGainRaw;
}


double CameraBasler::getFrameRate()
{
    return _dFrameRate;
}


void CameraBasler::setFrameRate( double dFrameRate )
{
    if( dFrameRate > 100 ) dFrameRate = 100;    // Assuming maxFrameRate = 100
    _dFrameRate = dFrameRate;
}


void CameraBasler::getImageResolutionMax( int &iNumRows, int &iNumCols )
{
    if( CameraBasler::_iNumMaxRows == 0 || CameraBasler::_iNumMaxCols == 0 )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_MAX_IMAGE_RESOLUTION_NOT_SET_YET );
    }
    iNumRows = CameraBasler::_iNumMaxRows;
    iNumCols = CameraBasler::_iNumMaxCols;
}


void CameraBasler::setImageResolutionMax( int iNumRows, int iNumCols )
{
    if( iNumRows < 10 || iNumCols < 10 )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_MAX_ROWS_COLS_SPECIFIED );
    }
    CameraBasler::_iNumMaxRows = iNumRows;
    CameraBasler::_iNumMaxCols = iNumCols;
}


void CameraBasler::getImageResolutionCur( int &iNumRows, int &iNumCols )
{
    if( CameraBasler::_iNumRows == 0 || CameraBasler::_iNumCols == 0 )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_IMAGE_RESOLUTION_NOT_SET_YET );
    }
    iNumRows = CameraBasler::_iNumRows;
    iNumCols = CameraBasler::_iNumCols;
}


void CameraBasler::setImageResolutionCur( int iNumRows, int iNumCols )
{
    if( iNumRows < 10 || iNumCols < 10 )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_ROWS_COLS_SPECIFIED );
    }
    CameraBasler::_iNumRows = iNumRows;
    CameraBasler::_iNumCols = iNumCols;
}


void CameraBasler::setPylonAutoGainControl( bool bPylonAutoGainControl )
{
	CameraBasler::_bPylonAutoGainControl = bPylonAutoGainControl;
	if( CameraBasler::_bPylonAutoGainControl )
	{
		CameraBasler::setCustomAutoGainControl( false );
		CameraBasler::setUpdateCustomGain( false );
		CameraBasler::setUpdateCustomExposure( false );

		// Set the gain mode
#if 0
		CameraBasler::setGainMode( (int)GAIN_RAW );
		CameraBasler::setGainRaw( GAIN_RAW_INIT );
		//CameraBasler::setGainMode( (int)GAIN_AUTO_ONCE );
#else
		CameraBasler::setGainMode( (int)GAIN_MODE );
		//CameraBasler::setGainMode( (int)GAIN_AUTO_ONCE );
		//CameraBasler::setGainMode( (int)GAIN_AUTO_CONT );
#endif

		// Set the exposure mode
#if 1
		CameraBasler::setExposureMode( (int)EXPO_RAW );
		CameraBasler::setExposureRaw( EXPO_RAW_INIT );
		//CameraBasler::setExposureMode( (int)EXPO_AUTO_ONCE );
#else
		CameraBasler::setExposureMode( (int)EXPO_AUTO_ONCE );
		//CameraBasler::setExposureMode( (int)EXPO_AUTO_CONT );
#endif
	}
}


bool CameraBasler::getPylonAutoGainControl()
{
	return CameraBasler::_bPylonAutoGainControl;
}


void CameraBasler::setCustomAutoGainControl( bool bCustomAutoGainControl )
{
	CameraBasler::_bCustomAutoGainControl = bCustomAutoGainControl;
	if( CameraBasler::_bCustomAutoGainControl )
	{
		CameraBasler::setPylonAutoGainControl( false );
		CameraBasler::setUpdateCustomGain( true );
		CameraBasler::setUpdateCustomExposure( false );

		// Set the gain mode
#if 1
		CameraBasler::setGainMode( (int)GAIN_AUTO_ONCE );
#else
		CameraBasler::setGainMode( (int)GAIN_RAW );
		CameraBasler::setGainRaw( GAIN_RAW_INIT );
#endif

		// Set the exposure mode
#if 1
		CameraBasler::setExposureMode( (int)EXPO_RAW );
		CameraBasler::setExposureRaw( EXPO_RAW_INIT );
#else
		CameraBasler::setExposureMode( (int)EXPO_AUTO_ONCE );
#endif
	}
}


bool CameraBasler::getCustomAutoGainControl()
{
	return CameraBasler::_bCustomAutoGainControl;
}


void CameraBasler::setUpdateCustomGain( bool bUpdateCustomGain )
{
	CameraBasler::_bUpdateCustomGain = bUpdateCustomGain;
}


bool CameraBasler::getUpdateCustomGain()
{
	return CameraBasler::_bUpdateCustomGain;
}


void CameraBasler::setUpdateCustomExposure( bool bUpdateCustomExposure )
{
	CameraBasler::_bUpdateCustomExposure = bUpdateCustomExposure;
}


bool CameraBasler::getUpdateCustomExposure()
{
	return CameraBasler::_bUpdateCustomExposure;
}


// == Private Functions ==

void CameraBasler::activateExposureMode()
{
	if( CameraBasler::_eExposureMode == EXPO_AUTO_CONT )
	{
		setAutoExposureContinuous();
	}
	else if( CameraBasler::_eExposureMode == EXPO_AUTO_ONCE )
	{
		setAutoExposureOnce( true );
	}
	else if( CameraBasler::_eExposureMode == EXPO_RAW )
	{
		setRawExposure();
	}
}


void CameraBasler::activateGainMode()
{
	if( CameraBasler::_eGainMode == GAIN_AUTO_CONT )
	{
		setAutoGainContinuous();
	}
	else if( CameraBasler::_eGainMode == GAIN_AUTO_ONCE )
	{
		setAutoGainOnce( true );
	}
	else if( CameraBasler::_eGainMode == GAIN_RAW )
	{
		setRawGain();
	}
}


float CameraBasler::getAvgBrightness( IplImage *pImage, int iImageType, bool bDebug )
{
	int iRow = 0;
	int iCol = 0;
	float fAvgBrightness = 0.0f;
	CvScalar scalar;

	// To increase speed of computation of getAvgBrightness and also to ensure that the computation is performed globally (on
	// the whole image), it could be a good idea to skip row and column by some interval.
	int iNumPixConsidered = 0;
	int iIncIntvl = 1;
	if( PARTIAL_COMPUTATION )
	{
		iIncIntvl = SKIP_INTERVAL;
	}

	if( iImageType == COLOR )
	{
		for( iRow = 0; iRow < pImage->height; iRow += iIncIntvl )
		{
			for( iCol = 0; iCol < pImage->width; iCol += iIncIntvl )
			{
				iNumPixConsidered++;
				scalar = cvGet2D( pImage, iRow, iCol );
				fAvgBrightness += (float)( ( scalar.val[0] + scalar.val[1] + scalar.val[2]) / 3 );
			}
		}
	}
	else if( iImageType == BLUE_CHANNEL || ( iImageType == MONOCHROME && _bSaveVideo ) )
	{
		for( iRow = 0; iRow < pImage->height; iRow += iIncIntvl )
		{
			for( iCol = 0; iCol < pImage->width; iCol += iIncIntvl )
			{
				iNumPixConsidered++;
				scalar = cvGet2D( pImage, iRow, iCol );
				fAvgBrightness += (float)scalar.val[0];
			}
		}
	}
	else if( iImageType == MONOCHROME )
	{
		for( iRow = 0; iRow < pImage->height; iRow += iIncIntvl )
		{
			for( iCol = 0; iCol < pImage->width; iCol += iIncIntvl )
			{
				iNumPixConsidered++;
				fAvgBrightness += (float)(int)(unsigned char) pImage->imageData[ iRow * pImage->widthStep + iCol ];
			}
		}
	}
	fAvgBrightness = fAvgBrightness / iNumPixConsidered;
	if( bDebug )
	{
		cout << "fAvgBrightness = " << fAvgBrightness << endl;
	}
	return fAvgBrightness;
}


float CameraBasler::getAvgContrast( IplImage *pImage, int iImageType, float fAvgBrightness, bool bDebug )
{
	int iRow = 0;
	int iCol = 0;
	float fBrightness = 0.0f;
	float fAvgContrast = 0.0f;
	CvScalar scalar;

	// To increase speed of computation of getAvgBrightness and also to ensure that the computation is performed globally (on
	// the whole image), it could be a good idea to skip row and column by some interval.
	int iIncIntvl = 1;
	if( PARTIAL_COMPUTATION )
	{
		iIncIntvl = SKIP_INTERVAL;
	}

	if( iImageType == COLOR )
	{
		for( iRow = 0; iRow < pImage->height; iRow += iIncIntvl )
		{
			for( iCol = 0; iCol < pImage->width; iCol += iIncIntvl )
			{
				scalar = cvGet2D( pImage, iRow, iCol );
				fBrightness = (float)( ( scalar.val[0] + scalar.val[1] + scalar.val[2] ) / 3 );
				fAvgContrast += ( fBrightness - fAvgBrightness ) * ( fBrightness - fAvgBrightness );
			}
		}
	}
	else if( iImageType == BLUE_CHANNEL || ( iImageType == MONOCHROME && _bSaveVideo ) )
	{
		for( iRow = 0; iRow < pImage->height; iRow += iIncIntvl )
		{
			for( iCol = 0; iCol < pImage->width; iCol += iIncIntvl )
			{
				scalar = cvGet2D( pImage, iRow, iCol );
				fBrightness = (float)scalar.val[0];
				fAvgContrast += ( fBrightness - fAvgBrightness ) * ( fBrightness - fAvgBrightness );
			}
		}
	}
	else if( iImageType == MONOCHROME )
	{
		for( iRow = 0; iRow < pImage->height; iRow += iIncIntvl )
		{
			for( iCol = 0; iCol < pImage->width; iCol += iIncIntvl )
			{
				fBrightness = (float)(int)(unsigned char) pImage->imageData[ iRow * pImage->widthStep + iCol ];
				fAvgContrast += ( fBrightness - fAvgBrightness ) * ( fBrightness - fAvgBrightness );
			}
		}
	}
	fAvgContrast = fAvgContrast / ( pImage->width * pImage->height );
	fAvgContrast = (float)sqrt( (double)fAvgContrast );
	if( bDebug )
	{
		cout << "fAvgBrightness = " << fAvgBrightness << endl;
		cout << "fAvgContrast = " << fAvgContrast << endl;
	}
	return fAvgContrast;
}


// Update gain and exposure values for AutoGainControl.
// These values will be used for the next image.
// Auto adjust brightness -> Exposure
void CameraBasler::getCameraParamsToUpdate( IplImage *pImage, int &iExposureRaw, int &iGainRaw, int iImageType, bool bDebug )
{
    // NEED to alternate gain and exposure update as the change in one affects the other, so need to get new image

	// Ensure that CUSTOM_AUTO_GAIN_CONTROL is set to true
	float fAvgBrightness = getAvgBrightness( pImage, iImageType, bDebug );
	int iGainMultiplier = 0;
	if( CameraBasler::getUpdateCustomExposure() && !_bToggleGainAndExpComputation )
	{
		float fBrightnessDiff = 0.0f;
		float fBrightnessWeight = 0.0f;
		int iBrightnessIntvlToUpdate = 0;
		float fBrightnessMaxDiffInDark = BRIGHTNESS_THRESHOLD_MIN - 0;
		float fBrightnessMaxDiffInBright = 255 - BRIGHTNESS_THRESHOLD_MAX;
		if( fAvgBrightness < BRIGHTNESS_THRESHOLD_MIN )
		{
			if( USE_AUTO_GAIN_WEIGHTS )
			{
				fBrightnessDiff = (float)( BRIGHTNESS_THRESHOLD_MIN - fAvgBrightness );
				fBrightnessWeight = ( fBrightnessDiff / ( BRIGHTNESS_THRESHOLD_MAX - BRIGHTNESS_THRESHOLD_MIN ) ) * ( fBrightnessDiff / fBrightnessMaxDiffInDark );
			}
			else
			{
				fBrightnessWeight = 1.0f;
			}
			iBrightnessIntvlToUpdate = (int)( fBrightnessWeight * EXPOSURE_INTVL );
			if( ( iExposureRaw + iBrightnessIntvlToUpdate ) <= EXPOSURE_MAX )
			{
				iExposureRaw += iBrightnessIntvlToUpdate;
			}
			if( bDebug ) cout << "iBrightnessIntvlToUpdate (++++++++)= " << iBrightnessIntvlToUpdate << endl;
		}
		else if( fAvgBrightness > BRIGHTNESS_THRESHOLD_MAX )
		{
			if( USE_AUTO_GAIN_WEIGHTS )
			{
				fBrightnessDiff = (float)( fAvgBrightness - BRIGHTNESS_THRESHOLD_MAX );
				fBrightnessWeight = ( fBrightnessDiff / ( BRIGHTNESS_THRESHOLD_MAX - BRIGHTNESS_THRESHOLD_MIN ) ) * ( fBrightnessDiff / fBrightnessMaxDiffInBright );
			}
			else
			{
				fBrightnessWeight = 1.0f;
			}
			iBrightnessIntvlToUpdate = (int)( fBrightnessWeight * EXPOSURE_INTVL );
			if( ( iExposureRaw - iBrightnessIntvlToUpdate ) >= EXPOSURE_MIN )
			{
				iExposureRaw -= iBrightnessIntvlToUpdate;
			}
			if( bDebug ) cout << "iBrightnessIntvlToUpdate (--------) = " << iBrightnessIntvlToUpdate << endl;
		}
		if( bDebug ) cout << "iExposureRaw = " << iExposureRaw << endl;
	}
	else
	{
		if( fAvgBrightness < BRIGHTNESS_THRESHOLD_MIN )
		{
			iGainMultiplier = 1;
		}
		else if( fAvgBrightness > BRIGHTNESS_THRESHOLD_MAX )
		{
			iGainMultiplier = -1;
		}
		else
		{
			iGainMultiplier = 0;
		}
	}

	if( CameraBasler::getUpdateCustomGain() && _bToggleGainAndExpComputation )
	{
		if( bDebug ) cout << "iGainMultiplier = " << iGainMultiplier << endl;

		// Auto adjust contrast -> Gain
		float fAvgContrast = getAvgContrast( pImage, (int)IMAGE_TYPE, fAvgBrightness, bDebug );
		float fContrastDiff = 0.0f;
		float fContrastWeight = 0.0f;
		int iContrastIntvlToUpdate = 0;
		float fContrastMaxDiffInDark = CONTRAST_THRESHOLD_MIN - 0;
		float fContrastMaxDiffInBright = 255 - CONTRAST_THRESHOLD_MAX;
		if( fAvgContrast < CONTRAST_THRESHOLD_MIN )
		{
            //cout << "fAvgContrast " << fAvgContrast << " < CONTRAST_THRESHOLD_MIN" << CONTRAST_THRESHOLD_MIN << endl;
			if( USE_AUTO_GAIN_WEIGHTS )
			{
				fContrastDiff = (float)( CONTRAST_THRESHOLD_MIN - fAvgContrast );
				fContrastWeight = ( fContrastDiff / ( CONTRAST_THRESHOLD_MAX - CONTRAST_THRESHOLD_MIN ) ) * ( fContrastDiff / fContrastMaxDiffInDark );
			}
			else
			{
				fContrastWeight = 1.0f;
			}
			if( iGainMultiplier == 0 )
			{
				iContrastIntvlToUpdate = (int)( fContrastWeight * GAIN_INTVL );
			}
			else
			{
				iContrastIntvlToUpdate = (int)( iGainMultiplier * fContrastWeight * GAIN_INTVL );
			}
			if( ( ( iGainRaw + iContrastIntvlToUpdate ) <= GAIN_MAX ) &&
				( ( iGainRaw + iContrastIntvlToUpdate ) >= GAIN_MIN ) )
			{
				iGainRaw += iContrastIntvlToUpdate;
			}
			if( bDebug )
			{
				if( iGainMultiplier == 0 || iGainMultiplier == 1 )
				{
					cout << "iContrastIntvlToUpdate (++++++++) = " << iContrastIntvlToUpdate << endl;
				}
				else
				{
					cout << "iContrastIntvlToUpdate (--------) = " << iContrastIntvlToUpdate << endl;
				}
			}
		}
		else if( fAvgContrast > CONTRAST_THRESHOLD_MAX )
		{
            //cout << "fAvgContrast " << fAvgContrast << " > CONTRAST_THRESHOLD_MAX" << CONTRAST_THRESHOLD_MAX << endl;
			if( USE_AUTO_GAIN_WEIGHTS )
			{
				fContrastDiff = (float)( fAvgContrast - CONTRAST_THRESHOLD_MAX );
				fContrastWeight = ( fContrastDiff / ( CONTRAST_THRESHOLD_MAX - CONTRAST_THRESHOLD_MIN ) ) * ( fContrastDiff / fContrastMaxDiffInBright );
			}
			else
			{
				fContrastWeight = 1.0f;
			}
			if( iGainMultiplier == 0 )
			{
				iContrastIntvlToUpdate = (int)( fContrastWeight * GAIN_INTVL );
				if( ( iGainRaw - iContrastIntvlToUpdate ) > GAIN_MIN )
				{
					iGainRaw -= iContrastIntvlToUpdate;
				}
				if( bDebug ) cout << "iContrastIntvlToUpdate (--------) = " << -1 * iContrastIntvlToUpdate << endl;
			}
			else
			{
				iContrastIntvlToUpdate = (int)( iGainMultiplier * fContrastWeight * GAIN_INTVL );
				if( ( ( iGainRaw + iContrastIntvlToUpdate ) <= GAIN_MAX ) &&
					( ( iGainRaw + iContrastIntvlToUpdate ) >= GAIN_MIN ) )
				{
					iGainRaw += iContrastIntvlToUpdate;
				}
				if( bDebug )
				{
					if( iGainMultiplier == 0 || iGainMultiplier == 1 )
					{
						cout << "iContrastIntvlToUpdate (++++++++) = " << iContrastIntvlToUpdate << endl;
					}
					else
					{
						cout << "iContrastIntvlToUpdate (--------) = " << iContrastIntvlToUpdate << endl;
					}
				}
			}
		}
		if( bDebug ) cout << "iGainRaw = " << iGainRaw << endl;
	}
}


struct CameraBasler::RGBPixel CameraBasler::YUV_to_RGB( int y,int u,int v )
{
    int r,g,b;
    struct RGBPixel rgbPixel = { 0, 0, 0 };

    // u and v are +-0.5
    // y -= 16;
    u -= 128;
    v -= 128;

    // Conversion used for YUV422 to RGB888
    r = (int)( y + 1.370705 * v );
    g = (int)( y - 0.698001 * v - 0.337633 * u );
    b = (int)( y + 1.732446 * u );

    /*
    Alternative approaches
    //http://en.wikipedia.org/wiki/YUV
	r = \mathrm{clamp}(( 298 \times C + 409 \times E + 128) >> 8)
    g = \mathrm{clamp}(( 298 \times C - 100 \times D - 208 \times E + 128) >> 8)
    b = \mathrm{clamp}(( 298 \times C + 516 \times D + 128) >> 8)

    == OR ==

    r = y + 1.402 * v;
    g = y - 0.344 * u - 0.714 * v;
    b = y + 1.772 * u;

    == OR ==

    y -= 16;
    r = 1.164 * y + 1.596 * v;
    g = 1.164 * y - 0.392 * u - 0.813 * v;
    b = 1.164 * y + 2.017 * u;
    */

    // Clamp to 0..1
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    rgbPixel.r = r;
    rgbPixel.g = g;
    rgbPixel.b = b;

    return rgbPixel;
}


void CameraBasler::convertYUV422PackedPixelToRGB24Pixel(unsigned char *_YUV, struct RGBPixel *RGBPixel1, struct RGBPixel *RGBPixel2)
{
   int y1,y2,u,v;

   // Extract _YUV components
   u  = _YUV[0];
   y1 = _YUV[1];
   v  = _YUV[2];
   y2 = _YUV[3];

   // _YUV to rgb
   *RGBPixel1 = YUV_to_RGB(y1,u,v);
   *RGBPixel2 = YUV_to_RGB(y2,u,v);
}


/**
This function checks whether an image has the desired properties or not.
@param pImage The IplImage that will be checked if it conforms the requirements.
*/
void CameraBasler::checkImage( IplImage *pImage )
{
	if ( !pImage )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_NULL_IMAGE );
    }
    if( CameraBasler::_eImageType == MONOCHROME )
    {
	    if ( pImage->nChannels != 1 )
        {
            _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_MULTICHANNEL_IMAGE, string( "ImageType is set to MONOCHROME." ) );
        }
    }
    else if( CameraBasler::_eImageType == COLOR )
    {
	    if ( pImage->nChannels != 3 )
        {
            _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_MULTICHANNEL_IMAGE, string( "ImageType is set to COLOR." ) );
        }
    }
	if ( pImage->depth != IPL_DEPTH_8U )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_INVALID_IMAGE_DEPTH );
    }
}


void CameraBasler::setAutoExposureOnce( bool bDebug )
{
    try
    {
        /*
        NOTE:
        Specifying the Stream Parameters > IP Configuration > Transmission Type
        to "Limited Broadcast ( 255.255.255.255 )" causes the ExposureAuto to be set
        to permanent Off, invalidating the two functionalities Exposure_Continuous
        and Exposure_Once.
        */

        // Set the target value for luminance control. This is always expressed
        // by an 8 bit value regardless of the current pixel format,
        // i.e., 0 -> black, 255 -> white.
        _pCamera->AutoTargetValue.SetValue( 80 );

        if( bDebug )
        {
            // Try ExposureAuto = Once
            cout << "Trying 'ExposureAuto = Once'." << endl;
            cout << "Initial exposure time = ";
            cout << _pCamera->ExposureTimeAbs.GetValue() << " us" << endl;
        }

        // Set the exposure time ranges for luminance control
        _pCamera->AutoExposureTimeAbsLowerLimit.SetValue( EXPOSURE_MIN, false );
        _pCamera->AutoExposureTimeAbsUpperLimit.SetValue( EXPOSURE_MAX, false );
        _pCamera->ExposureAuto.SetValue( ExposureAuto_Once );

        // When the "once" mode of operation is selected,
        // the parameter values are automatically adjusted until the related image property
        // reaches the target value. After the automatic parameter value adjustment is complete, the auto
        // function will automatically be set to "off", and the new parameter value will be applied to the
        // images grabbed after that point.
        int n = 0;
        while ( _pCamera->ExposureAuto.GetValue() != ExposureAuto_Off )
        {
            this->grabTuningImage( false, true, bDebug );
            n++;
			if( n > MAX_FRAMES_PYLON_EXPO_AUTO_ONCE )
			{
				if( bDebug )
				{
					cout << "Maximum number of frames (" << MAX_FRAMES_PYLON_EXPO_AUTO_ONCE << ") for tuning exposure reached." << endl <<
							"Exiting tuning process and using the latest values..." << endl;
				}
				_pCamera->ExposureAuto.SetValue( ExposureAuto_Off );
				break;
			}
        }
        if( bDebug )
        {
            cout << "ExposureAuto went back to 'Off' after " << n << " frames." << endl;
            cout << "Final exposure time = " << _pCamera->ExposureTimeAbs.GetValue() << " us" << endl << endl;
        }
        // Setting the final value to _iExposureRaw
        _iExposureRaw = _pCamera->ExposureTimeRaw.GetValue();
        if( _iExposureRaw > EXPOSURE_MAX )
        {
        	_iExposureRaw = EXPOSURE_MAX;
        	_pCamera->ExposureTimeRaw.SetValue( _iExposureRaw );
        }
        if( _iExposureRaw < EXPOSURE_MIN )
        {
        	_iExposureRaw = EXPOSURE_MIN;
        	_pCamera->ExposureTimeRaw.SetValue( _iExposureRaw );
        }
    }
    catch ( GenICam::GenericException &e )
    {
        //cerr << "A GenICam exception occurred: " << e.GetDescription() << endl;
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_SETTING_AUTO_EXPO_ONCE, string( e.GetDescription() ) );
    }
}


void CameraBasler::setAutoExposureContinuous( bool bDebug )
{
    try
    {
        // Set the target value for luminance control. This is always expressed
        // by an 8 bit value regardless of the current pixel format,
        // i.e., 0 -> black, 255 -> white.
        _pCamera->AutoTargetValue.SetValue( 80 );

        if( bDebug )
        {
            cout << "ExposureAuto 'GainAuto = Continuous'." << endl;
            cout << "Initial exposure time = ";
            cout << _pCamera->ExposureTimeAbs.GetValue() << " us" << endl;
        }
        _pCamera->AutoExposureTimeAbsLowerLimit.SetValue( EXPOSURE_MIN, false );
        _pCamera->AutoExposureTimeAbsUpperLimit.SetValue( EXPOSURE_MAX, false );
        _pCamera->ExposureMode.SetValue( ExposureMode_Timed );
        _pCamera->ExposureAuto.SetValue( ExposureAuto_Continuous );

        if( bDebug )
        {
            // When "continuous" mode is selected, the parameter value is adjusted repeatedly while images are acquired.
            // Depending on the current frame rate, the automatic adjustments will usually be carried out for
            // every or every other image, unless the cameras microcontroller is kept busy by other tasks.
            // The repeated automatic adjustment will proceed until the "once" mode of operation is used or
            // until the auto function is set to "off", in which case the parameter value resulting from the latest
            // automatic adjustment will operate unless the value is manually adjusted.
#if 0 // For demonstration purposes, we will use only 20 images
			for ( int n = 0; n < 20; n++ )
            {
                this->grabTuningImage( false, true, bDebug );
            }
            _pCamera->ExposureAuto.SetValue( ExposureAuto_Off ); // Switch Exposure auto off!
            cout << "Final exposure time = " << _pCamera->ExposureTimeAbs.GetValue() << " us" << endl << endl;
#endif
        }
    }
    catch ( GenICam::GenericException &e )
    {
        //cerr << "A GenICam exception occurred: " << e.GetDescription() << endl;
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_SETTING_AUTO_EXPO_CONT, string( e.GetDescription() ) );
    }
}


/**
CHECK: Error states that the node is not writable.
*/
void CameraBasler::setRawExposure( bool bDebug )
{
    try
    {
        _pCamera->ExposureMode.SetValue( ExposureMode_Timed );
        _pCamera->ExposureAuto.SetValue( ExposureAuto_Off );
        if( ( CameraBasler::_iExposureRaw >= EXPOSURE_MIN ) &&
            ( CameraBasler::_iExposureRaw <= EXPOSURE_MAX ) )
        {
            _pCamera->ExposureTimeRaw.SetValue( CameraBasler::_iExposureRaw );
        }
        else
        {
            _pErrorHandler->setErrorCode( (int)CAMERABASLER_EXPOSURE_OUT_OF_RANGE );
        }
    }
    catch ( GenICam::GenericException &e )
    {
        //cerr << "A GenICam exception occurred: " << e.GetDescription() << endl;
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_SETTING_EXPO_RAW, string( e.GetDescription() ) );
    }
}


void CameraBasler::setAutoGainOnce( bool bDebug )
{
    try
    {
        // Set the the target value for luminance control. This is always expressed
        // by an 8 bit value regardless of the current pixel format,
        // i.e., 0 -> black, 255 -> white.
        _pCamera->AutoTargetValue.SetValue( 80 );

        if( bDebug )
        {
            // We are going to try GainAuto = Once
            cout << "Trying 'GainAuto = Once'." << endl;
            cout << "Initial Gain = " << _pCamera->GainRaw.GetValue() << endl;
        }

        // Set the gain ranges for luminance control
        _pCamera->AutoGainRawLowerLimit.SetValue( GAIN_MIN, false );
        _pCamera->AutoGainRawUpperLimit.SetValue( GAIN_MAX, false );
        _pCamera->GainAuto.SetValue( GainAuto_Once );

        // When the "once" mode of operation is selected,
        // the parameter values are automatically adjusted until the related image property
        // reaches the target value. After the automatic parameter value adjustment is complete, the auto
        // function will automatically be set to "off", and the new parameter value will be applied to the
        // images grabbed after that point.
        int n = 0;
        while( _pCamera->GainAuto.GetValue() != GainAuto_Off )
        {
            this->grabTuningImage( true, false, bDebug );
            n++;
			if( n > MAX_FRAMES_PYLON_GAIN_AUTO_ONCE )
			{
				if( bDebug )
				{
					cout << "Maximum number of frames (" << MAX_FRAMES_PYLON_GAIN_AUTO_ONCE << ") for tuning gain reached." << endl <<
							"Exiting tuning process and using the latest values..." << endl;
				}
				_pCamera->GainAuto.SetValue( GainAuto_Off );
				break;
			}
        }

        // Assigning final gain raw value to _iGainRaw
		_iGainRaw = _pCamera->GainRaw.GetValue();
		if( _iGainRaw > GAIN_MAX )
		{
			_iGainRaw = GAIN_MAX;
			_pCamera->GainRaw.SetValue( _iGainRaw );
		}
		if( _iGainRaw < GAIN_MIN)
		{
			_iGainRaw = GAIN_MIN;
			_pCamera->GainRaw.SetValue( _iGainRaw );
		}
        if( bDebug )
        {
            cout << "GainAuto went back to 'Off' after " << n << " frames." << endl;
            cout << "Final Gain = " << _pCamera->GainRaw.GetValue() << endl << endl;
        }
	}
    catch ( GenICam::GenericException &e )
    {
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_SETTING_AUTO_GAIN_ONCE, string( e.GetDescription() ) );
    }
}


void CameraBasler::setAutoGainContinuous( bool bDebug )
{
    try
    {
        // Set the target value for luminance control. This is always expressed
        // by an 8 bit value regardless of the current pixel format,
        // i.e., 0 -> black, 255 -> white.
        _pCamera->AutoTargetValue.SetValue(80);

        if( bDebug )
        {
            // We are trying GainAuto = Continuous
            cout << "Trying 'GainAuto = Continuous'." << endl;
            cout << "Initial Gain = " << _pCamera->GainRaw.GetValue() << endl;
        }

        // Set the gain ranges for luminance control
#if 0	// Actual default max values
        _pCamera->AutoGainRawLowerLimit.SetValue( _pCamera->GainRaw.GetMin() );
        _pCamera->AutoGainRawUpperLimit.SetValue( _pCamera->GainRaw.GetMax() );
#else
        _pCamera->AutoGainRawLowerLimit.SetValue( GAIN_MIN, false );
        _pCamera->AutoGainRawUpperLimit.SetValue( GAIN_MAX, false );
#endif
        _pCamera->GainAuto.SetValue( GainAuto_Continuous );

        if( bDebug )
        {
            // When "continuous" mode is selected, the parameter value is adjusted repeatedly while images are acquired.
            // Depending on the current frame rate, the automatic adjustments will usually be carried out for
            // every or every other image unless the cameras micro controller is kept busy by other tasks.
            // The repeated automatic adjustment will proceed until the "once" mode of operation is used or
            // until the auto function is set to "off", in which case the parameter value resulting from the latest
            // automatic adjustment will operate unless the value is manually adjusted.
#if 0 // For demonstration purposes, we will grab "only" 20 images
            for (int n = 0; n < 20; n++)
            {
                this->grabImage();
            }
            _pCamera->GainAuto.SetValue( GainAuto_Off ); // Switch GainAuto off
            cout << "Final Gain = " << _pCamera->GainRaw.GetValue() << endl << endl;
#endif
        }
    }
    catch ( GenICam::GenericException &e )
    {
        //cerr << "A GenICam exception occurred: " << e.GetDescription() << endl;
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_SETTING_AUTO_GAIN_CONT, string( e.GetDescription() ) );
    }
}


void CameraBasler::setRawGain( bool bDebug )
{
    try
    {
        _pCamera->GainAuto.SetValue( GainAuto_Off );
        if( ( CameraBasler::_iGainRaw >= GAIN_MIN ) &&
            ( CameraBasler::_iGainRaw <= GAIN_MAX ) )
        {
            _pCamera->GainRaw.SetValue( CameraBasler::_iGainRaw );
        }
        else
        {
            _pErrorHandler->setErrorCode( (int)CAMERABASLER_GAIN_OUT_OF_RANGE );
        }
    }
    catch ( GenICam::GenericException &e )
    {
        //cerr << "A GenICam exception occurred: " << e.GetDescription() << endl;
        _pErrorHandler->setErrorCode( (int)CAMERABASLER_ERROR_SETTING_GAIN_RAW, string( e.GetDescription() ) );
    }
}


/** YUV to RGB Conversion
 * B = 1.164(Y - 16)                  + 2.018(U - 128)
 * G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
 * R = 1.164(Y - 16) + 1.596(V - 128)
 *
 * Values have to be clamped to keep them in the [0-255] range.
 * Rumour has it that the valid range is actually a subset of [0-255] (fourcc.org mentions an RGB range
 * of [16-235] mentioned) but clamping the values into [0-255] seems to produce acceptable results.
 * @param YUV unsigned char array that contains the pixels, 4 pixels in 8 byte macro pixel, line after
 *            line
 * @param RGB where the RGB output will be written to, will have pixel after pixel, 3 bytes per pixel
 *            (thus this is a 24bit RGB with one byte per color) line by line.
 * @param width Width of the image contained in the YUV buffer
 * @param height Height of the image contained in the YUV buffer
 */
void CameraBasler::convertYUV422PackedImageToBGRImage(const unsigned char *YUV, IplImage *pImageBGR, bool bDebug )
{
	// Note: Shifting is faster than multiplication and division
	register int y0, y1, u, v;
	register unsigned int iBufferLoc = 0;
	char *pchBGR = pImageBGR->imageData;
#if 0	// Faster, Works for CAMERA2, does NOT work for CAMERA1 due to pImageRGB->widthStep
	//cout << "pImageRGB->widthStep = " << pImageRGB->widthStep << endl;
	int iNumRGBPix = 0;
	int iWidth = pImageBGR->width;
	int iHeight = pImageBGR->height;
	while ( iNumRGBPix < (iWidth * iHeight)/2 )
	{
		u  = YUV[iBufferLoc++] - 128;
		y0 = YUV[iBufferLoc++] -  16;
		v  = YUV[iBufferLoc++] - 128;
		y1 = YUV[iBufferLoc++] -  16;

		// Set red, green and blue bytes for pixel 0
		*pchBGR++ = clip( (76284 * y0 + 132252 * u             ) >> 16 );
		*pchBGR++ = clip( (76284 * y0 -  25625 * u - 53281 * v ) >> 16 );
		*pchBGR++ = clip( (76284 * y0 + 104595 * v             ) >> 16 );

		// Set red, green and blue bytes for pixel 1
		*pchBGR++ = clip( (76284 * y1 + 132252 * u             ) >> 16 );
		*pchBGR++ = clip( (76284 * y1 -  25625 * u - 53281 * v ) >> 16 );
		*pchBGR++ = clip( (76284 * y1 + 104595 * v             ) >> 16 );
		iNumRGBPix++;
	}
#elif 1	// Works for CAMERA1 and CAMERA2, uses _pImage->widthStep
	int iImageLoc = 0;
    for( int iRow = 0; iRow < pImageBGR->height; iRow++ )
    {
    	for( int iCol = 0; iCol < pImageBGR->width; iCol+=2 )
    	{
    		iImageLoc = iRow * pImageBGR->widthStep + (iCol) * pImageBGR->nChannels;
    		// Get 4 bytes from YUV array to obtain 2 RGB pixels
    		u  = YUV[iBufferLoc++] - 128;
    		y0 = YUV[iBufferLoc++] -  16;
    		v  = YUV[iBufferLoc++] - 128;
    		y1 = YUV[iBufferLoc++] -  16;

    		// Set red, green and blue bytes for pixel 0
    		pchBGR[ iImageLoc++ ] = clip( (76284 * y0 + 132252 * u             ) >> 16 );
    		pchBGR[ iImageLoc++ ] = clip( (76284 * y0 -  25625 * u - 53281 * v ) >> 16 );
    		pchBGR[ iImageLoc++ ] = clip( (76284 * y0 + 104595 * v             ) >> 16 );

    		// Set red, green and blue bytes for pixel 1
    		pchBGR[ iImageLoc++ ] = clip( (76284 * y1 + 132252 * u             ) >> 16 );
    		pchBGR[ iImageLoc++ ] = clip( (76284 * y1 -  25625 * u - 53281 * v ) >> 16 );
    		pchBGR[ iImageLoc ] = clip( (76284 * y1 + 104595 * v             ) >> 16 );
    	}
    }
#endif
}


void CameraBasler::convertYUV422PackedImageToBGRImage2(const unsigned char *YUV, IplImage *pImageBGR, bool bDebug )
{
	register unsigned int iBufferLoc = 0;
	for( int iRow = 0; iRow < _pImage->height; iRow++ )
	{
		for( int iCol = 0; iCol < _pImage->width; iCol+=2 )
		{
			// Get 4 bytes from YUV array to obtain 2 RGB pixels
			_YUV[0] = YUV[iBufferLoc++];
			_YUV[1] = YUV[iBufferLoc++];
			_YUV[2] = YUV[iBufferLoc++];
			_YUV[3] = YUV[iBufferLoc++];

			convertYUV422PackedPixelToRGB24Pixel( _YUV, &RGBPixel1, &RGBPixel2 );

			pImageBGR->imageData[ iRow * pImageBGR->widthStep + iCol * pImageBGR->nChannels + 0 ] = RGBPixel1.b;
			pImageBGR->imageData[ iRow * pImageBGR->widthStep + iCol * pImageBGR->nChannels + 1 ] = RGBPixel1.g;
			pImageBGR->imageData[ iRow * pImageBGR->widthStep + iCol * pImageBGR->nChannels + 2 ] = RGBPixel1.r;
			pImageBGR->imageData[ iRow * pImageBGR->widthStep + (iCol+1) * pImageBGR->nChannels + 0 ] = RGBPixel2.b;
			pImageBGR->imageData[ iRow * pImageBGR->widthStep + (iCol+1) * pImageBGR->nChannels + 1 ] = RGBPixel2.g;
			pImageBGR->imageData[ iRow * pImageBGR->widthStep + (iCol+1) * pImageBGR->nChannels + 2 ] = RGBPixel2.r;
		}
	}
}


void CameraBasler::convertBayerBG2ImageToBGRImage( const unsigned char *YUV, IplImage *pImageBGR, bool bDebug )
{
	// Pixel_Format = BayerBG2 - 8 bit per pixel
	IplImage *pImage = cvCreateImage( cvGetSize( pImageBGR ), IPL_DEPTH_8U, 1 );
	pImage->imageData = (char*)YUV;
	pImage->width = pImageBGR->width;
	pImage->height = pImageBGR->height;
	pImage->widthStep = pImage->width * pImage->nChannels;
	pImage->imageSize = pImage->widthStep * pImage->height;
	cvCvtColor( pImage, pImageBGR, CV_BayerBG2BGR);

	// Clean up
	pImage->imageData = NULL;
	cvReleaseImage( &pImage );
}


/**
Note: YCrCb = YUV444. So, in order to convert from YUV422 to RGB24 image,
one way is to convert from YUV422 to YCrCb and then use CV_YCrCb2BGR
to convert from YCrCb (=YUV444) to BGR image.
*/
void CameraBasler::convertYCrCbImageToBGRImage( const unsigned char *YUV, IplImage *pImageBGR, bool bDebug )
{
    // ~10 ms for YUV422 to YCrCb and ~23 ms for cvCvtColor

	// To convert 4:2:0 or 4:2:2 YUV to RGB, convert the YUV data to
	// 4:4:4 YUV, and then convert from 4:4:4 YUV (=YCrCb) to RGB.
	register unsigned int iBufferLoc = 0;
	register unsigned int iImageLoc = 0;
	if( !_pImageYCrCb )
	{
		_pErrorHandler->setErrorCode( (int) CAMERABASLER_INVALID_NULL_IMAGE );
	}
	//cvSetZero( _pImageYCrCb );
	unsigned char *pchYCrCb = (unsigned char*)_pImageYCrCb->imageData;
	int iChannels = _pImageYCrCb->nChannels;
	int iWidthStep = _pImageYCrCb->widthStep;
    for( int iRow = 0; iRow < _pImageYCrCb->height; iRow++ )
    {
    	iImageLoc = iRow * iWidthStep;
    	for( int iCol = 0; iCol < _pImageYCrCb->width; iCol+=2 )
    	{
			// Get 4 bytes from YUV array to obtain 2 RGB pixels
    		// YUV422Packed has byte order U0 Y0 V0 Y1 U1 Y2 V1 Y3
			pchYCrCb[ iImageLoc ] = YUV[ iBufferLoc + 1 ];
			pchYCrCb[ iImageLoc + 1 ] = YUV[ iBufferLoc + 2 ];
			pchYCrCb[ iImageLoc + 2 ] = YUV[ iBufferLoc ];
			pchYCrCb[ iImageLoc + 3 ] = YUV[ iBufferLoc + 3 ];
			pchYCrCb[ iImageLoc + 4 ] = YUV[ iBufferLoc + 2 ];
			pchYCrCb[ iImageLoc + 5 ] = YUV[ iBufferLoc ];
			iBufferLoc += 4;
    		iImageLoc += 2 * iChannels;
    	}
    }
    cvCvtColor( _pImageYCrCb, pImageBGR, CV_YCrCb2BGR );

#if 0
	cvNamedWindow( "Test1", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Test1", _pImageYCrCb );
	cvNamedWindow( "Test2", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Test2", pImageBGR );
	cvWaitKey( 0 );
#endif
}


