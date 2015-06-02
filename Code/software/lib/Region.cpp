/*
 * Region.cpp
 * PoliceEyes project
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#include "Region.h"

int Region::_iObjCount = 0;
ErrorHandler* Region::_pErrorHandler = NULL;
Logger* Region::_pLogger = NULL;
bool Region::_bIsSetErrorDiagnosticsList = false;

void Region::initErrorDiagnosticsList()
{
    if (!Region::_bIsSetErrorDiagnosticsList)
    {
#define REGION_INVALID_OBJECT						3701
        _pErrorHandler->insertErrorDiagnostics((int)REGION_INVALID_OBJECT,
                string("Region : Invalid object."));

        // Escapable Exceptions
    }
    Region::_bIsSetErrorDiagnosticsList = true;
}

Region::Region()
{
    Region::_iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    _pLogger = Logger::getInstance();
    _pClock = new Clock();
    _pTimer = new Timer();
    _pUtilities = new Utilities();

    initErrorDiagnosticsList();

    _vPts.clear();
}

Region::~Region()
{
    Region::_iObjCount--;
    if (_pClock) delete _pClock;
    if (_pTimer) delete _pTimer;
    if (_pUtilities) delete _pUtilities;
    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;
}

string Region::getOutputFolder()
{
    return string(Logger::getInstance()->getOutputFolder());
}

void Region::addPt(CvPoint pt)
{
    _vPts.push_back(pt);
}

void Region::removeLastEnteredPoint()
{
    if( _vPts.size() > 0) _vPts.pop_back();
}

// == Getters and Setters ==

int Region::getObjCount()
{
    return Region::_iObjCount;
}

vector<CvPoint> Region::getPts()
{
    return _vPts;
}
