/*
 * Regions.h
 * PoliceEyes project (2011 - 2012)
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#ifndef REGIONS_H_
#define REGIONS_H

#include "Clock.h"
#include "ErrorHandler.h"
#include "Logger.h"
#include "Region.h"
#include "Timer.h"
#include "Utilities.h"

void mouseCallbackAddRegion(int event, int x, int y, int flags, void* param);
void mouseCallbackAddLPRegion(int event, int x, int y, int flags, void* param);
void mouseCallbackAddRegionOfChars(int event, int x, int y, int flags, void* param);
void mouseCallbackAddCharRegions(int event, int x, int y, int flags, void* param);
void mouseCallbackAddRectRegions2Pts(int event, int x, int y, int flags, void* param);
void mouseCallbackAddRectRegions4Pts(int event, int x, int y, int flags, void* param);

void mouseCallbackAddLine(int event, int x, int y, int flags, void* param);

class Regions
{
public:
    Regions();
    ~Regions();

    static void initErrorDiagnosticsList();
    static string getOutputFolder();

    void specifyRegionManually(string sTextTop, string sTextBottom);
    void specifyLPRegionManually(string sTextTop, string sTextBottom);
    void specifyRegionOfCharsManually();
    void specifyCharRegionsManually();
    void specifyRectRegionsUsing2Pts();
    void specifyRectRegionsUsing4Pts();
    void specifyRegionFromFile(vector<CvPoint> points);
    void specifyLineManually(string sTextTop, string sTextBottom);
    void addRegion(Region *pRegion);
    void removeLastRegion();
    void setOverlayProperties(bool bSingleChannelOverlay, int iChannelToOverlay,
            CvScalar colorOverlay, CvScalar colorPtSelection);
    IplImage* getRegionMask(Region *pRegion);
    IplImage* getLineMask(Region *pRegion);

    void saveToConfigFile(const char *pchRegionType, bool bDebug = false);

    // Getters and Setters
    static int getObjCount();
    void setImage(IplImage *pImage, IplImage *pImageProcessingArea = NULL);
    void setWindowName(string sWindowName);
    string getWindowName();
    IplImage* getImageSelection();
    IplImage* getImageRegionsMask();
    Utilities* getObjUtilities();
    void setEnablePtSelectionForRegion(bool bEnablePtSelectionForRegion);
    bool getEnablePtSelectionForRegion();
    vector<Region*> getRegions();
    bool getSingleChannelOverlay();
    int getChannelToOverlay();
    CvScalar getColorOverlay();
    CvScalar getColorPtSelection();
	vector<CvPoint> getLinePts();

private:
    static int _iObjCount;
    static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;
    static bool _bIsSetErrorDiagnosticsList;

    Clock *_pClock;
    Timer *_pTimer;
    Utilities *_pUtilities;

    bool _bEnablePtSelectionForRegion;
    vector<Region*> _vpRegions;
    bool _bIsSetImageSelection;
    IplImage *_pImageOriginal;
    IplImage *_pImageSelection;
    IplImage *_pImageRegionMask;
    IplImage *_pImageRegionsMask;
    string _sWindowName;
    bool _bSingleChannelOverlay;
    int _iChannelToOverlay;
    CvScalar _colorOverlay;
    CvScalar _colorPtSelection;

    cv::FileStorage _fsConfigYMLWrite;
};
#endif //#ifndef REGIONS_H
