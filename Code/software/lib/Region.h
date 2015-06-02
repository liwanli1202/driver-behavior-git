/*
 * Region.h
 * PoliceEyes project (2011 - 2012)
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#ifndef REGION_H_
#define REGION_H_

#include <cv.h>
#include <vector>
using namespace std;

#include "Clock.h"
#include "ErrorHandler.h"
#include "Logger.h"
#include "Timer.h"
#include "Utilities.h"

class Region
{
public:
    Region();
    ~Region();

    static void initErrorDiagnosticsList();
    static string getOutputFolder();

    void addPt(CvPoint pt);
    void removeLastEnteredPoint();

    // Getters and Setters
    static int getObjCount();
    vector<CvPoint> getPts();

private:
    static int _iObjCount;
    static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;
    static bool _bIsSetErrorDiagnosticsList;

    Clock *_pClock;
    Timer *_pTimer;
    Utilities *_pUtilities;

    vector<CvPoint> _vPts;
};
#endif //#ifndef REGION_H_
