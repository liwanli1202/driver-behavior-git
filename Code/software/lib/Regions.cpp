/*
 * Regions.cpp
 * PoliceEyes project
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#include <cv.h>
#include <highgui.h>

#include "Regions.h"

int Regions::_iObjCount = 0;
ErrorHandler* Regions::_pErrorHandler = NULL;
Logger* Regions::_pLogger = NULL;
bool Regions::_bIsSetErrorDiagnosticsList = false;

void Regions::initErrorDiagnosticsList()
{
    if (!Regions::_bIsSetErrorDiagnosticsList)
    {
#define REGIONS_INVALID_OBJECT			    3801
        _pErrorHandler->insertErrorDiagnostics((int)REGIONS_INVALID_OBJECT,
                string("Regions : Invalid object."));
#define REGIONS_INVALID_NUM_CHANNELS        3802
        _pErrorHandler->insertErrorDiagnostics(
                (int)REGIONS_INVALID_NUM_CHANNELS,
                string("Regions : Invalid number of channels."));
#define REGIONS_IMAGE_NOT_SET               3803
        _pErrorHandler->insertErrorDiagnostics((int)REGIONS_IMAGE_NOT_SET,
                string("Regions : Image is not set."));
#define REGIONS_INSUFFICIENT_POINTS_TO_DEFINE_REGION    3804
        _pErrorHandler->insertErrorDiagnostics(
                (int)REGIONS_INSUFFICIENT_POINTS_TO_DEFINE_REGION,
                string("Regions : Insufficient points to define region."));
#define REGIONS_NO_REGION_DEFINED           3805
        _pErrorHandler->insertErrorDiagnostics((int)REGIONS_NO_REGION_DEFINED,
                string("Regions : No region defined."));
#define REGIONS_INSUFFICIENT_POINTS_TO_DEFINE_LINE    3806
        _pErrorHandler->insertErrorDiagnostics(
                (int)REGIONS_INSUFFICIENT_POINTS_TO_DEFINE_LINE,
                string("Regions : Insufficient points to define line."));
#define REGIONS_INVALID_IMAGE_SIZE          3807
        _pErrorHandler->insertErrorDiagnostics((int)REGIONS_INVALID_IMAGE_SIZE,
                string("Regions : Invalid image size."));

        // Escapable Exceptions
    }
    Regions::_bIsSetErrorDiagnosticsList = true;
}

Regions::Regions()
{
    Regions::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
    _pClock = new Clock();
    _pTimer = new Timer();
    _pUtilities = new Utilities();

    initErrorDiagnosticsList();

    _bEnablePtSelectionForRegion = true;
    _vpRegions.clear();
    _bIsSetImageSelection = false;
    _pImageSelection = NULL;
    _pImageOriginal = NULL;
    _pImageRegionMask = NULL;
    _pImageRegionsMask = NULL;
    _sWindowName = string("User input");

    _bSingleChannelOverlay = true;
    _iChannelToOverlay = 1;
    _colorOverlay = cvScalar(255, 255, 0);
    _colorPtSelection = cvScalar(0, 0, 255);
}

Regions::~Regions()
{
    Regions::_iObjCount--;
    if (_pClock) delete _pClock;
    if (_pTimer) delete _pTimer;
    if (_pUtilities) delete _pUtilities;
    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;
    // TODO: Fix segmentation fault when deleting _pImageOriginal
    if (_pImageOriginal) cvReleaseImage(&_pImageOriginal);
    if (_pImageSelection) cvReleaseImage(&_pImageSelection);
    if (_pImageRegionMask) cvReleaseImage(&_pImageRegionMask);
    if (_pImageRegionsMask) cvReleaseImage(&_pImageRegionsMask);
    if (_fsConfigYMLWrite.isOpened()) _fsConfigYMLWrite.release();
}

string Regions::getOutputFolder()
{
    return string(Logger::getInstance()->getOutputFolder());
}

void Regions::specifyRegionManually(string sTextTop, string sTextBottom)
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    _bEnablePtSelectionForRegion = true; // Disabled within mouseCallbackAddRegion
    Region *pRegion = new Region();
    addRegion(pRegion);
    cout << "Left click to specify the region." << endl
            << "Right click to remove the last defined point." << endl
            << "Middle click to complete specifying the region." << endl
            << "Press 'q' to exit specifying the region" << endl;
    cvNamedWindow(_sWindowName.c_str(), CV_WINDOW_NORMAL);
    //cvSetWindowProperty(_sWindowName.c_str(), CV_WND_PROP_FULLSCREEN,
    //        CV_WINDOW_FULLSCREEN);
    if (sTextTop.length() > 0)
    {
        _pUtilities->writeTextOnImage(_pImageSelection, sTextTop, 0);
    }
    if (sTextBottom.length() > 0)
    {
        _pUtilities->writeTextOnImage(_pImageSelection, sTextBottom, 1);
    }
    cvShowImage(_sWindowName.c_str(), _pImageSelection);
    cvSetMouseCallback(_sWindowName.c_str(), mouseCallbackAddRegion, this);
    while (true)
    {
        char chKey = cvWaitKey(100);
        if (chKey == 'q' || chKey == 'Q') break;
    }
    cout << "Done specifying a region." << endl;
}

void Regions::specifyLPRegionManually(string sTextTop, string sTextBottom)
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    Region *pRegion = new Region();
    addRegion(pRegion);
    cout << sTextTop << endl << sTextBottom << endl;
    cvNamedWindow(_sWindowName.c_str(), 0);
    if (sTextTop.length() > 0)
    {
        _pUtilities->writeTextOnImage(_pImageSelection, sTextTop, 0);
    }
    if (sTextBottom.length() > 0)
    {
        _pUtilities->writeTextOnImage(_pImageSelection, sTextBottom, 1);
    }
    this->getImageSelection();
    cvShowImage(_sWindowName.c_str(), _pImageSelection);
    cvSetMouseCallback(_sWindowName.c_str(), mouseCallbackAddLPRegion, this);
    while (true)
    {
        char chKey = cvWaitKey(100);
        if (chKey == 'q' || chKey == 'Q')
        {
            break;
        }
    }
    void* pvoidWinHandle = cvGetWindowHandle(_sWindowName.c_str());
    if (pvoidWinHandle != 0) cvDestroyWindow(_sWindowName.c_str());
    cout << "Done specifying a LP region." << endl;
}

void Regions::specifyRegionOfCharsManually()
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    Region *pRegion = new Region();
    addRegion(pRegion);
    cvNamedWindow(_sWindowName.c_str(), 0);
    this->getImageSelection();
    cvShowImage(_sWindowName.c_str(), _pImageSelection);
    cvSetMouseCallback(_sWindowName.c_str(), mouseCallbackAddRegionOfChars,
            this);
    while (true)
    {
        char chKey = cvWaitKey(100);
        if (chKey == 'q' || chKey == 'Q')
        {
            break;
        }
    }
    void* pvoidWinHandle = cvGetWindowHandle(_sWindowName.c_str());
    if (pvoidWinHandle != 0) cvDestroyWindow(_sWindowName.c_str());
    cout << "Done specifying the region of characters." << endl;
}

void Regions::specifyCharRegionsManually()
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    Region *pRegion = new Region();
    addRegion(pRegion);
    cvNamedWindow(_sWindowName.c_str(), 0);
    this->getImageSelection();
    cvShowImage(_sWindowName.c_str(), _pImageSelection);
    cvSetMouseCallback(_sWindowName.c_str(), mouseCallbackAddCharRegions, this);
    while (true)
    {
        char chKey = cvWaitKey(100);
        if (chKey == 'q' || chKey == 'Q')
        {
            break;
        }
    }
    void* pvoidWinHandle = cvGetWindowHandle(_sWindowName.c_str());
    if (pvoidWinHandle != 0) cvDestroyWindow(_sWindowName.c_str());
    cout << "Done specifying the characters in the region." << endl;
}

void Regions::specifyRectRegionsUsing2Pts()
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    Region *pRegion = new Region();
    addRegion(pRegion);
    cvNamedWindow(_sWindowName.c_str(), 0);
    this->getImageSelection();
    cvShowImage(_sWindowName.c_str(), _pImageSelection);
    cvSetMouseCallback(_sWindowName.c_str(), mouseCallbackAddRectRegions2Pts,
            this);
    while (true)
    {
        char chKey = cvWaitKey(100);
        if (chKey == 'q' || chKey == 'Q')
        {
            break;
        }
    }
    void* pvoidWinHandle = cvGetWindowHandle(_sWindowName.c_str());
    if (pvoidWinHandle != 0) cvDestroyWindow(_sWindowName.c_str());
    cout << "Done specifying the regions." << endl;
}

void Regions::specifyRectRegionsUsing4Pts()
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    Region *pRegion = new Region();
    addRegion(pRegion);
    cvNamedWindow(_sWindowName.c_str(), 0);
    this->getImageSelection();
    cvShowImage(_sWindowName.c_str(), _pImageSelection);
    cvSetMouseCallback(_sWindowName.c_str(), mouseCallbackAddRectRegions4Pts,
            this);
    while (true)
    {
        char chKey = cvWaitKey(100);
        if (chKey == 'q' || chKey == 'Q')
        {
            break;
        }
    }
    void* pvoidWinHandle = cvGetWindowHandle(_sWindowName.c_str());
    if (pvoidWinHandle != 0) cvDestroyWindow(_sWindowName.c_str());
    cout << "Done specifying the regions." << endl;
}

// TODO : Update code to read from yml file
// Clean up code simply to add the regions specified in the file.
void Regions::specifyRegionFromFile(vector<CvPoint> points)
{
    int x, y;
    Region *pRegions = new Region();
    addRegion(pRegions);
    vector<Region*> vpRegions = this->getRegions();
    int iNumRegions = vpRegions.size();
    Region *pRegion = vpRegions[iNumRegions - 1];
    IplImage *pImageC = this->getImageSelection();
    IplImage *pImageRegionsMask = this->getImageRegionsMask();
    string sWindowName = this->getWindowName();
    bool bSingleChannelOverlay = this->getSingleChannelOverlay();
    int iChannelToOverlay = this->getChannelToOverlay();
    CvScalar colorOverlay = this->getColorOverlay();
    CvScalar colorPtSelection = this->getColorPtSelection();
    Utilities *pUtilities = this->getObjUtilities();
    bool bEnablePtSelectionForRegion = this->getEnablePtSelectionForRegion();
    int iNumPtsInRegion = pRegion->getPts().size();

    for (unsigned int i = 0; i < points.size(); i++)
    {
        x = points[i].x;
        y = points[i].y;

        CvPoint pt = cvPoint(x, y);
        pRegion->addPt(pt);
        cvCircle(pImageC, pt, 0, colorPtSelection, 1);
        cvCircle(pImageC, pt, 3, colorPtSelection, 1);
        cvShowImage(sWindowName.c_str(), pImageC);
        iNumPtsInRegion++;
    }

    if (iNumPtsInRegion > 2)
    {
        CvPoint ptFirst = pRegion->getPts()[0];
        pRegion->addPt(ptFirst);
        IplImage *pImageOverlay = this->getRegionMask(pRegion);
        cvOr(pImageOverlay, pImageRegionsMask, pImageRegionsMask,
                pImageOverlay);
        if (bSingleChannelOverlay)
        {
            pUtilities->overlayImage(pImageOverlay, pImageC, iChannelToOverlay,
                    bSingleChannelOverlay);
        }
        else
        {
            pUtilities->overlayImage(pImageOverlay, pImageC, colorOverlay);
        }
        cvShowImage(sWindowName.c_str(), pImageC);
        this->setEnablePtSelectionForRegion(false);
    }
}

void Regions::specifyLineManually(string sTextTop, string sTextBottom)
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    _bEnablePtSelectionForRegion = true; // Disabled within mouseCallbackAddRegion
    Region *pRegion = new Region();
    addRegion(pRegion);
    cout << "Left click to specify the first point of the line." << endl
            << "Right click to specify the second point of the line." << endl
            << "Press 'q' to exit specifying the line." << endl;
    cvNamedWindow(_sWindowName.c_str(), CV_WINDOW_NORMAL);
    //cvSetWindowProperty(_sWindowName.c_str(), CV_WND_PROP_FULLSCREEN,
    //        CV_WINDOW_FULLSCREEN);
    if (sTextTop.length() > 0)
    {
        _pUtilities->writeTextOnImage(_pImageSelection, sTextTop, 0);
    }
    if (sTextBottom.length() > 0)
    {
        _pUtilities->writeTextOnImage(_pImageSelection, sTextBottom, 1);
    }
    cvShowImage(_sWindowName.c_str(), _pImageSelection);
    cvSetMouseCallback(_sWindowName.c_str(), mouseCallbackAddLine, this);
    while (true)
    {
        char chKey = cvWaitKey(100);
        if (chKey == 'q' || chKey == 'Q') break;
    }
    cout << "Done specifying a line." << endl;
}

void Regions::addRegion(Region *pRegion)
{
    _vpRegions.push_back(pRegion);
}

void Regions::removeLastRegion()
{
    _vpRegions.pop_back();
}

void Regions::setOverlayProperties(bool bSingleChannelOverlay,
        int iChannelToOverlay, CvScalar colorOverlay, CvScalar colorPtSelection)
{
    _bSingleChannelOverlay = bSingleChannelOverlay;
    _iChannelToOverlay = iChannelToOverlay;
    _colorOverlay = colorOverlay;
    _colorPtSelection = colorPtSelection;
}

IplImage* Regions::getRegionMask(Region *pRegion)
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    vector<CvPoint> vPts = pRegion->getPts();
    int iNumPts = vPts.size();
    if (iNumPts < 3)
    {
        _pErrorHandler->setErrorCode(
                (int)REGIONS_INSUFFICIENT_POINTS_TO_DEFINE_REGION);
    }
    CvPoint **ppPts = (CvPoint**)cvAlloc(sizeof(CvPoint *));
    ppPts[0] = (CvPoint*)cvAlloc(sizeof(CvPoint) * (iNumPts));

    for (int iPt = 0; iPt < iNumPts; iPt++)
    {
        ppPts[0][iPt].x = vPts[iPt].x;
        ppPts[0][iPt].y = vPts[iPt].y;
    }
    cvSetZero(_pImageRegionMask);
    cvFillPoly(_pImageRegionMask, ppPts, &iNumPts, 1, CV_RGB(255, 255, 255), 8,
            0);
    cvFree(&ppPts[0]);
    cvFree(&ppPts);
    return _pImageRegionMask;
}

IplImage* Regions::getLineMask(Region *pRegion)
{
    if (!_bIsSetImageSelection)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_IMAGE_NOT_SET);
    }
    vector<CvPoint> vPts = pRegion->getPts();
    int iNumPts = vPts.size();
    if (iNumPts < 2)
    {
        _pErrorHandler->setErrorCode(
                (int)REGIONS_INSUFFICIENT_POINTS_TO_DEFINE_LINE);
    }
    CvPoint **ppPts = (CvPoint**)cvAlloc(sizeof(CvPoint *));
    ppPts[0] = (CvPoint*)cvAlloc(sizeof(CvPoint) * (iNumPts));

    for (int iPt = 0; iPt < iNumPts; iPt++)
    {
        ppPts[0][iPt].x = vPts[iPt].x;
        ppPts[0][iPt].y = vPts[iPt].y;
    }
    cvSetZero(_pImageRegionMask);
    cvFillPoly(_pImageRegionMask, ppPts, &iNumPts, 1, CV_RGB(255, 255, 255), 8,
            0);
    cvFree(&ppPts[0]);
    cvFree(&ppPts);
    return _pImageRegionMask;
}

void Regions::saveToConfigFile(const char *pchRegionType, bool bDebug)
{
    // Convert to matrix and save to yml file
    int iNumRegions = _vpRegions.size();
    if (iNumRegions <= 0)
    {
        _pErrorHandler->setErrorCode((int)REGIONS_NO_REGION_DEFINED);
    }
    string sNode = string("num") + string(pchRegionType);
    string sConfigFile = _pLogger->getConfigFilename();
    if (!_fsConfigYMLWrite.isOpened()) _fsConfigYMLWrite.open(sConfigFile,
            cv::FileStorage::APPEND);
    _fsConfigYMLWrite << sNode << iNumRegions;

    stringstream ssOut;
    for (int iRegion = 0; iRegion < iNumRegions; iRegion++)
    {
        Region *pRegion = _vpRegions[iRegion];
        int iNumPtsInRegion = pRegion->getPts().size();
        if (iNumPtsInRegion < 1) continue;
        vector<CvPoint> ptsInRegion = pRegion->getPts();
        CvMat *pMatRegion = cvCreateMat(iNumPtsInRegion, 2, CV_32SC1);
        for (int iPt = 0; iPt < iNumPtsInRegion; iPt++)
        {
            CV_MAT_ELEM (*pMatRegion, int, iPt, 0) = ptsInRegion[iPt].x;
            CV_MAT_ELEM(*pMatRegion, int, iPt, 1) = ptsInRegion[iPt].y;
        }
        ssOut.str("");
        ssOut << iRegion;
        sNode = string(pchRegionType) + ssOut.str();
        _fsConfigYMLWrite << sNode << pMatRegion;
        if (pMatRegion) cvReleaseMat(&pMatRegion);
    }
    if (_fsConfigYMLWrite.isOpened()) _fsConfigYMLWrite.release();
}

// == Getters and Setters ==

int Regions::getObjCount()
{
    return Regions::_iObjCount;
}

void Regions::setImage(IplImage *pImage, IplImage *pImageProcessingArea)
{
    if (!_pImageSelection)
    {
        _pImageSelection = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 3);
    }
    if (pImage->nChannels == 1)
    {
        cvCvtColor(pImage, _pImageSelection, CV_GRAY2BGR);
    }
    else if (pImage->nChannels == 3)
    {
        cvCopy(pImage, _pImageSelection);
    }
    else
    {
        _pErrorHandler->setErrorCode((int)REGIONS_INVALID_NUM_CHANNELS);
    }
    if (pImageProcessingArea)
    {
        if (pImage->width != pImageProcessingArea->width
                && pImage->height != pImageProcessingArea->height)
        {
            _pErrorHandler->setErrorCode((int)REGIONS_INVALID_IMAGE_SIZE,
                    string(
                            "Specified mask is not the same size as the image."));
        }
    }
    if (_pImageOriginal) cvReleaseImage(&_pImageOriginal);
    _pImageOriginal = cvCloneImage(_pImageSelection);

    // Overlay processing area on the image that has been set to define regions.
    // This is essential to create ground truth data specific to the processing area.
    // This is being used by ImageCropper to define ground truth regions for detection.
    if (pImageProcessingArea)
    {
        _pUtilities->overlayContour(pImageProcessingArea, _pImageOriginal,
                cvScalar(0, 0, 255));
    }

    // Initialization
    if (!_pImageRegionMask)
    {
        _pImageRegionMask = cvCreateImage(cvGetSize(_pImageSelection),
                IPL_DEPTH_8U, 1);
        cvSetZero(_pImageRegionMask);
    }
    if (!_pImageRegionsMask)
    {
        _pImageRegionsMask = cvCreateImage(cvGetSize(_pImageSelection),
                IPL_DEPTH_8U, 1);
        cvSetZero(_pImageRegionsMask);
    }
    _bIsSetImageSelection = true;
}

void Regions::setWindowName(string sWindowName)
{
    _sWindowName = string(sWindowName);
}

string Regions::getWindowName()
{
    return _sWindowName;
}

IplImage* Regions::getImageSelection()
{
    cvCopy(_pImageOriginal, _pImageSelection);
    // Draw the regions and the points of the last region
    vector<Region*> vpRegions = this->getRegions();
    int iNumRegions = vpRegions.size();
    if (iNumRegions == 0)
    {
        cout << "No region defined yet.";
    }
    else if (iNumRegions > 1)
    {
        for (int iRegion = 0; iRegion < iNumRegions - 1; iRegion++)
        {
            Region *pRegion = vpRegions[iRegion];
            vector<CvPoint> vPts = pRegion->getPts();
            for (int iPt = 0; iPt < vPts.size() - 1; iPt++)
            {
                cvLine(_pImageSelection, vPts[iPt], vPts[iPt + 1],
                        cvScalar(0, 255, 0), 2); //Green
            }
            cvLine(_pImageSelection, vPts[0], vPts[vPts.size() - 1],
                    cvScalar(0, 255, 0), 2); //Green
        }
    }
    if (iNumRegions > 0)
    {
        // Display the points
        Region *pRegion = vpRegions[iNumRegions - 1];
        vector<CvPoint> vPts = pRegion->getPts();
        for (int iPt = 0; iPt < vPts.size(); iPt++)
        {
            cvCircle(_pImageSelection, vPts[iPt], 0, cvScalar(0, 0, 255), 1); //Red
            cvCircle(_pImageSelection, vPts[iPt], 5, cvScalar(0, 255, 255), 1); //Yellow
        }
    }
    return _pImageSelection;
}

IplImage* Regions::getImageRegionsMask()
{
    cvSetZero(_pImageRegionsMask);
    for (unsigned int iRegion = 0; iRegion < _vpRegions.size(); iRegion++)
    {
        Region *pRegion = _vpRegions[iRegion];
        IplImage *pImageOverlay = getRegionMask(pRegion);
        cvOr(pImageOverlay, _pImageRegionsMask, _pImageRegionsMask,
                pImageOverlay);
    }
    return _pImageRegionsMask;
}

Utilities* Regions::getObjUtilities()
{
    return _pUtilities;
}

void Regions::setEnablePtSelectionForRegion(bool bEnablePtSelectionForRegion)
{
    _bEnablePtSelectionForRegion = bEnablePtSelectionForRegion;
}

bool Regions::getEnablePtSelectionForRegion()
{
    return _bEnablePtSelectionForRegion;
}

vector<Region*> Regions::getRegions()
{
    return _vpRegions;
}

bool Regions::getSingleChannelOverlay()
{
    return _bSingleChannelOverlay;
}

int Regions::getChannelToOverlay()
{
    return _iChannelToOverlay;
}

CvScalar Regions::getColorOverlay()
{
    return _colorOverlay;
}

CvScalar Regions::getColorPtSelection()
{
    return _colorPtSelection;
}

vector<CvPoint> Regions::getLinePts()
{
	Region *pRegion = _vpRegions[0];
	return pRegion->getPts();
}

// == Mouse events ==
void mouseCallbackAddRegion(int event, int x, int y, int flags, void* param)
{
    Regions *pRegions = (Regions*)param;
    vector<Region*> vpRegions = pRegions->getRegions();
    int iNumRegions = vpRegions.size();
    Region *pRegion = vpRegions[iNumRegions - 1];
    IplImage *pImageC = pRegions->getImageSelection();
    string sWindowName = pRegions->getWindowName();
    bool bSingleChannelOverlay = pRegions->getSingleChannelOverlay();
    int iChannelToOverlay = pRegions->getChannelToOverlay();
    CvScalar colorOverlay = pRegions->getColorOverlay();
    CvScalar colorPtSelection = pRegions->getColorPtSelection();
    Utilities *pUtilities = pRegions->getObjUtilities();
    bool bEnablePtSelectionForRegion =
            pRegions->getEnablePtSelectionForRegion();
    int iNumPtsInRegion = pRegion->getPts().size();
    if (event == CV_EVENT_LBUTTONDOWN && bEnablePtSelectionForRegion)
    {
        cout << "Pt " << (iNumPtsInRegion + 1) << ": (" << x << ", " << y << ")"
                << endl;
        CvPoint pt = cvPoint(x, y);
        pRegion->addPt(pt);
        cvCircle(pImageC, pt, 0, colorPtSelection, 1);
        //cvCircle(pImageC, pt, 1, colorPtSelection, 1);
        cvCircle(pImageC, pt, 3, colorPtSelection, 1);
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_MBUTTONDOWN && bEnablePtSelectionForRegion)
    {
        if (iNumPtsInRegion > 2)
        {
            CvPoint ptFirst = pRegion->getPts()[0];
            pRegion->addPt(ptFirst);
            IplImage *pImageOverlay = pRegions->getRegionMask(pRegion);
            if (bSingleChannelOverlay)
            {
                pUtilities->overlayImage(pImageOverlay, pImageC,
                        iChannelToOverlay, bSingleChannelOverlay);
            }
            else
            {
                pUtilities->overlayImage(pImageOverlay, pImageC, colorOverlay);
            }
            cvShowImage(sWindowName.c_str(), pImageC);
            pRegions->setEnablePtSelectionForRegion(false);
        }
    }
    else if (event == CV_EVENT_RBUTTONDOWN)
    {
        // Remove last entered point
        pRegion->removeLastEnteredPoint();
        cout << "No. of points defined: " << pRegion->getPts().size() << endl;
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_MOUSEMOVE && bEnablePtSelectionForRegion)
    {
        if (iNumPtsInRegion > 0)
        {
            // Draw the line from last point to current mouse point
            // Get points
            vector<CvPoint> vPts = pRegion->getPts();
            int iNumPts = vPts.size();
            for (int iPtNo = 0; iPtNo < iNumPts - 1; iPtNo++)
            {
                cvLine(pImageC, vPts[iPtNo], vPts[iPtNo + 1],
                        cvScalar(0, 0, 255), 2);
            }
            CvPoint pt = cvPoint(x, y);
            cvLine(pImageC, vPts[iNumPts - 1], pt, cvScalar(0, 0, 255), 2);
            cvShowImage(sWindowName.c_str(), pImageC);
        }
    }
}

void mouseCallbackAddLPRegion(int event, int x, int y, int flags, void* param)
{
    Regions *pRegions = (Regions*)param;
    vector<Region*> vpRegions = pRegions->getRegions();
    int iNumRegions = vpRegions.size();
    Region *pRegion = vpRegions[iNumRegions - 1];
    IplImage *pImageC = pRegions->getImageSelection();
    string sWindowName = pRegions->getWindowName();
    cvShowImage(sWindowName.c_str(), pImageC);
    bool bSingleChannelOverlay = pRegions->getSingleChannelOverlay();
    int iChannelToOverlay = pRegions->getChannelToOverlay();
    CvScalar colorOverlay = pRegions->getColorOverlay();
    CvScalar colorPtSelection = pRegions->getColorPtSelection();
    int iNumPtsInRegion = pRegion->getPts().size();
    if (event == CV_EVENT_LBUTTONDOWN && iNumPtsInRegion < 4)
    {
        cout << "Pt " << (iNumPtsInRegion + 1) << ": (" << x << ", " << y << ")"
                << endl;
        CvPoint pt = cvPoint(x, y);
        pRegion->addPt(pt);
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_RBUTTONDOWN)
    {
        // Remove last entered point
        pRegion->removeLastEnteredPoint();
        cout << "No. of points defined: " << pRegion->getPts().size() << endl;
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
}

void mouseCallbackAddRegionOfChars(int event, int x, int y, int flags,
        void* param)
{
    Regions *pRegions = (Regions*)param;
    vector<Region*> vpRegions = pRegions->getRegions();
    int iNumRegions = vpRegions.size();
    Region *pRegion = vpRegions[iNumRegions - 1];
    IplImage *pImageC = pRegions->getImageSelection();
    string sWindowName = pRegions->getWindowName();
    cvShowImage(sWindowName.c_str(), pImageC);
    bool bSingleChannelOverlay = pRegions->getSingleChannelOverlay();
    int iChannelToOverlay = pRegions->getChannelToOverlay();
    CvScalar colorOverlay = pRegions->getColorOverlay();
    CvScalar colorPtSelection = pRegions->getColorPtSelection();
    int iNumPtsInRegion = pRegion->getPts().size();
    if (event == CV_EVENT_LBUTTONDOWN && iNumPtsInRegion < 2)
    {
        cout << "Pt " << (iNumPtsInRegion + 1) << ": (" << x << ", " << y << ")"
                << endl;
        CvPoint pt = cvPoint(x, y);
        pRegion->addPt(pt);
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_RBUTTONDOWN)
    {
        // Remove last entered point
        pRegion->removeLastEnteredPoint();
        cout << "No. of points defined: " << pRegion->getPts().size() << endl;
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
}

void mouseCallbackAddCharRegions(int event, int x, int y, int flags,
        void* param)
{
    Regions *pRegions = (Regions*)param;
    vector<Region*> vpRegions = pRegions->getRegions();
    int iNumRegions = vpRegions.size();
    Region *pRegion = vpRegions[iNumRegions - 1];
    IplImage *pImageC = pRegions->getImageSelection();
    string sWindowName = pRegions->getWindowName();
    cvShowImage(sWindowName.c_str(), pImageC);
    bool bSingleChannelOverlay = pRegions->getSingleChannelOverlay();
    int iChannelToOverlay = pRegions->getChannelToOverlay();
    CvScalar colorOverlay = pRegions->getColorOverlay();
    CvScalar colorPtSelection = pRegions->getColorPtSelection();
    int iNumPtsInRegion = pRegion->getPts().size();
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        cout << "Pt " << (iNumPtsInRegion + 1) << ": (" << x << ", " << y << ")"
                << endl;
        CvPoint pt = cvPoint(x, y);
        pRegion->addPt(pt);
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_RBUTTONDOWN)
    {
        // Remove last entered point
        pRegion->removeLastEnteredPoint();
        cout << "No. of points defined: " << pRegion->getPts().size() << endl;
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
}

void mouseCallbackAddRectRegions2Pts(int event, int x, int y, int flags,
        void* param)
{
    Regions *pRegions = (Regions*)param;
    vector<Region*> vpRegions = pRegions->getRegions();
    int iNumRegions = vpRegions.size();
    Region *pRegion = vpRegions[iNumRegions - 1];
    IplImage *pImageC = pRegions->getImageSelection();
    string sWindowName = pRegions->getWindowName();
    cvShowImage(sWindowName.c_str(), pImageC);
    bool bSingleChannelOverlay = pRegions->getSingleChannelOverlay();
    int iChannelToOverlay = pRegions->getChannelToOverlay();
    CvScalar colorOverlay = pRegions->getColorOverlay();
    CvScalar colorPtSelection = pRegions->getColorPtSelection();
    int iNumPtsInRegion = pRegion->getPts().size();
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        cout << "Pt " << (iNumPtsInRegion + 1) << ": (" << x << ", " << y << ")"
                << endl;
        CvPoint pt = cvPoint(x, y);
        pRegion->addPt(pt);
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_RBUTTONDOWN)
    {
        // Remove last entered point
        pRegion->removeLastEnteredPoint();
        cout << "No. of points defined: " << pRegion->getPts().size() << endl;
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_MOUSEMOVE)
    {
        if (iNumPtsInRegion > 0)
        {
            // Draw the line from last point to current mouse point
            // Get points
            vector<CvPoint> vPts = pRegion->getPts();
            for (int iPtNo = 0; iPtNo < iNumPtsInRegion - 1; iPtNo += 2)
            {
                cvLine(pImageC, vPts[iPtNo], vPts[iPtNo + 1],
                        cvScalar(0, 0, 255), 2);
                // Draw the rectangular ROI in different color
                int iMinX = min(vPts[iPtNo].x, vPts[iPtNo + 1].x);
                int iMaxX = max(vPts[iPtNo].x, vPts[iPtNo + 1].x);
                int iMinY = min(vPts[iPtNo].y, vPts[iPtNo + 1].y);
                int iMaxY = max(vPts[iPtNo].y, vPts[iPtNo + 1].y);
                cv::Point ptTL(iMinX, iMinY);
                cv::Point ptTR(iMaxX, iMinY);
                cv::Point ptBR(iMaxX, iMaxY);
                cv::Point ptBL(iMinX, iMaxY);
                cvLine(pImageC, ptTL, ptTR, cvScalar(0, 255, 0), 2);
                cvLine(pImageC, ptTR, ptBR, cvScalar(0, 255, 0), 2);
                cvLine(pImageC, ptBR, ptBL, cvScalar(0, 255, 0), 2);
                cvLine(pImageC, ptBL, ptTL, cvScalar(0, 255, 0), 2);
            }
            if (iNumPtsInRegion % 2 != 0)
            {
                CvPoint pt = cvPoint(x, y);
                cvLine(pImageC, vPts[iNumPtsInRegion - 1], pt,
                        cvScalar(0, 0, 255), 2);
                // Draw the rectangular ROI in different color
                int iMinX = min(vPts[iNumPtsInRegion - 1].x, pt.x);
                int iMaxX = max(vPts[iNumPtsInRegion - 1].x, pt.x);
                int iMinY = min(vPts[iNumPtsInRegion - 1].y, pt.y);
                int iMaxY = max(vPts[iNumPtsInRegion - 1].y, pt.y);
                cv::Point ptTL(iMinX, iMinY);
                cv::Point ptTR(iMaxX, iMinY);
                cv::Point ptBR(iMaxX, iMaxY);
                cv::Point ptBL(iMinX, iMaxY);
                cvLine(pImageC, ptTL, ptTR, cvScalar(0, 255, 0), 2);
                cvLine(pImageC, ptTR, ptBR, cvScalar(0, 255, 0), 2);
                cvLine(pImageC, ptBR, ptBL, cvScalar(0, 255, 0), 2);
                cvLine(pImageC, ptBL, ptTL, cvScalar(0, 255, 0), 2);
            }
            cvShowImage(sWindowName.c_str(), pImageC);
        }
    }
}

void mouseCallbackAddRectRegions4Pts(int event, int x, int y, int flags,
        void* param)
{
    Regions *pRegions = (Regions*)param;
    vector<Region*> vpRegions = pRegions->getRegions();
    int iNumRegions = vpRegions.size();
    Region *pRegion = vpRegions[iNumRegions - 1];
    IplImage *pImageC = pRegions->getImageSelection();
    string sWindowName = pRegions->getWindowName();
    cvShowImage(sWindowName.c_str(), pImageC);
    bool bSingleChannelOverlay = pRegions->getSingleChannelOverlay();
    int iChannelToOverlay = pRegions->getChannelToOverlay();
    CvScalar colorOverlay = pRegions->getColorOverlay();
    CvScalar colorPtSelection = pRegions->getColorPtSelection();
    int iNumPtsInRegion = pRegion->getPts().size();
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        cout << "Pt " << (iNumPtsInRegion + 1) << ": (" << x << ", " << y << ")"
                << endl;
        CvPoint pt = cvPoint(x, y);
        pRegion->addPt(pt);
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_RBUTTONDOWN)
    {
        // Remove last entered point
        pRegion->removeLastEnteredPoint();
        cout << "No. of points defined: " << pRegion->getPts().size() << endl;
        pImageC = pRegions->getImageSelection();
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_MOUSEMOVE)
    {
        if (iNumPtsInRegion > 0)
        {
            // Draw the line from last point to current mouse point
            // Get points
            vector<CvPoint> vPts = pRegion->getPts();
            for (int iPtNo = 0; iPtNo < iNumPtsInRegion - 1; iPtNo += 4)
            {
                cvLine(pImageC, vPts[iPtNo], vPts[iPtNo + 1],
                        cvScalar(0, 0, 255), 2);
                if ((iPtNo + 1) < (iNumPtsInRegion - 1)) cvLine(pImageC,
                        vPts[iPtNo + 1], vPts[iPtNo + 2], cvScalar(0, 0, 255),
                        2);
                if ((iPtNo + 2) < (iNumPtsInRegion - 1)) cvLine(pImageC,
                        vPts[iPtNo + 2], vPts[iPtNo + 3], cvScalar(0, 0, 255),
                        2);
                if ((iPtNo + 2) < (iNumPtsInRegion - 1)) cvLine(pImageC,
                        vPts[iPtNo + 3], vPts[iPtNo], cvScalar(0, 0, 255), 2);
            }
            if (iNumPtsInRegion % 4 != 0)
            {
                CvPoint pt = cvPoint(x, y);
                cvLine(pImageC, vPts[iNumPtsInRegion - 1], pt,
                        cvScalar(0, 0, 255), 2);
            }
            cvShowImage(sWindowName.c_str(), pImageC);
        }
    }
}

void mouseCallbackAddLine(int event, int x, int y, int flags, void* param)
{
    Regions *pRegions = (Regions*)param;
    vector<Region*> vpRegions = pRegions->getRegions();
    int iNumRegions = vpRegions.size();
    Region *pRegion = vpRegions[iNumRegions - 1];
    IplImage *pImageC = pRegions->getImageSelection();
    string sWindowName = pRegions->getWindowName();
    bool bSingleChannelOverlay = pRegions->getSingleChannelOverlay();
    int iChannelToOverlay = pRegions->getChannelToOverlay();
    CvScalar colorOverlay = pRegions->getColorOverlay();
    CvScalar colorPtSelection = pRegions->getColorPtSelection();
    Utilities *pUtilities = pRegions->getObjUtilities();
    bool bEnablePtSelectionForRegion =
            pRegions->getEnablePtSelectionForRegion();
    int iNumPtsInRegion = pRegion->getPts().size();
    if (event == CV_EVENT_LBUTTONDOWN && bEnablePtSelectionForRegion
            && iNumPtsInRegion < 1)
    {
        cout << "Pt " << (iNumPtsInRegion + 1) << ": (" << x << ", " << y << ")"
                << endl;
        CvPoint pt = cvPoint(x, y);
        pRegion->addPt(pt);
        cvCircle(pImageC, pt, 0, colorPtSelection, 1);
        //cvCircle(pImageC, pt, 1, colorPtSelection, 1);
        cvCircle(pImageC, pt, 3, colorPtSelection, 1);
        cvShowImage(sWindowName.c_str(), pImageC);
    }
    else if (event == CV_EVENT_RBUTTONDOWN && bEnablePtSelectionForRegion)
    {
        if (iNumPtsInRegion == 1)
        {
			cout << "Pt " << (iNumPtsInRegion + 1) << ": (" << x << ", " << y << ")"
                << endl;
            CvPoint pt = cvPoint(x, y);
            pRegion->addPt(pt);
            IplImage *pImageOverlay = pRegions->getLineMask(pRegion);
            if (bSingleChannelOverlay)
            {
                pUtilities->overlayImage(pImageOverlay, pImageC,
                        iChannelToOverlay, bSingleChannelOverlay);
            }
            else
            {
                pUtilities->overlayImage(pImageOverlay, pImageC, colorOverlay);
            }
            cvShowImage(sWindowName.c_str(), pImageC);
            pRegions->setEnablePtSelectionForRegion(false);
        }
    }
    else if (event == CV_EVENT_MOUSEMOVE && bEnablePtSelectionForRegion)
    {
        if (iNumPtsInRegion > 0)
        {
            // Draw the line from last point to current mouse point
            // Get points
            vector<CvPoint> vPts = pRegion->getPts();
            int iNumPts = vPts.size();
            for (int iPtNo = 0; iPtNo < iNumPts - 1; iPtNo++)
            {
                cvLine(pImageC, vPts[iPtNo], vPts[iPtNo + 1],
                        cvScalar(0, 0, 255), 2);
            }
            CvPoint pt = cvPoint(x, y);
            cvLine(pImageC, vPts[iNumPts - 1], pt, cvScalar(0, 0, 255), 2);
            cvShowImage(sWindowName.c_str(), pImageC);
        }
    }
}
