/*
 * Clock.cpp
 * PoliceEyes project (2011-2012)
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#include <iostream>
using namespace std;

#include "Clock.h"
#include <string.h>
#include <iostream>
using namespace std;

int Clock::_iObjCount = 0;

Clock::Clock()
{
    _iObjCount++;
    _start = clock();
}

Clock::~Clock()
{
    _iObjCount--;
}

string Clock::getDateTimeString(time_t rawtime, bool bIsStandard,
        bool bWithSpaces)
{
    struct tm *pTime;
    string sDateTime;
    pTime = localtime(&rawtime);
    sDateTime = string(asctime(pTime));
    // Removing the newline character from the end of the string
    sDateTime.replace(sDateTime.end() - 1, sDateTime.end(), "");
    if (!bIsStandard)
    {
        // TODO: Use while loop instead?
        // Replace ":" by "-"
        size_t pos = sDateTime.find(":");
        sDateTime.replace(pos, 1, "-");
        pos = sDateTime.find(":");
        sDateTime.replace(pos, 1, "-");
    }
    if (!bWithSpaces)
    {
        while (true)
        {
            size_t pos = sDateTime.find(" ");
            if (pos > sDateTime.length() || pos < 1) break;
            sDateTime.replace(pos, 1, "_");
        }
    }
    return sDateTime;
}

string Clock::getFormattedDateTimeString(time_t rawtime, bool bWithSpaces)
{
    struct tm *pTime;
    pTime = localtime(&rawtime);
    int iDayOfMonth = pTime->tm_mday;
    int iDayOfWeek = pTime->tm_wday;
    int iMonth = pTime->tm_mon;
    int iYear = 1900 + pTime->tm_year;
    int iHour = pTime->tm_hour;
    int iMin = pTime->tm_min;
    int iSec = pTime->tm_sec;

    string sDateTime = "";
    stringstream _ssOut;
    sDateTime = getDayOfWeekString(iDayOfWeek) + string(" ");
    _ssOut << iDayOfMonth;
    sDateTime += _ssOut.str() + " " + getMonthString(iMonth) + " ";
    _ssOut.str("");
    _ssOut << iYear;
    sDateTime += _ssOut.str() + " ";
    _ssOut.str("");
    _ssOut << iHour;
    if (iHour < 10) sDateTime += "0" + _ssOut.str() + ":";
    else sDateTime += _ssOut.str() + ":";
    _ssOut.str("");
    _ssOut << iMin;
    if (iMin < 10) sDateTime += "0" + _ssOut.str() + ":";
    else sDateTime += _ssOut.str() + ":";
    _ssOut.str("");
    _ssOut << iSec;
    if (iSec < 10) sDateTime += "0" + _ssOut.str();
    else sDateTime += _ssOut.str();
    //cout << iDayOfWeek << " " << iDayOfMonth << "-" << iMonth << "-" << iYear << " "
    //        << iHour << ":" << iMin << ":" << iSec << endl;

    // Replace ":" by "-"
    size_t pos = sDateTime.find(":");
    sDateTime.replace(pos, 1, "-");
    pos = sDateTime.find(":");
    sDateTime.replace(pos, 1, "-");

    if (!bWithSpaces)
    {
        while (true)
        {
            size_t pos = sDateTime.find(" ");
            if (pos > sDateTime.length() || pos < 1) break;
            sDateTime.replace(pos, 1, "_");
        }
    }

    return sDateTime;
}

string Clock::getFormattedDateTimeStringForMySQL(time_t rawtime)
{
    struct tm *pTime;
    pTime = localtime(&rawtime);
    int iDayOfMonth = pTime->tm_mday;
    int iDayOfWeek = pTime->tm_wday;
    int iMonth = pTime->tm_mon;
    int iYear = 1900 + pTime->tm_year;
    int iHour = pTime->tm_hour;
    int iMin = pTime->tm_min;
    int iSec = pTime->tm_sec;

    string sDateTime = "";
    stringstream _ssOut;
    _ssOut.str("");
    _ssOut << iYear;
    sDateTime = _ssOut.str() + "-";
    _ssOut.str("");
    _ssOut << iMonth + 1;
    if (iMonth < 9) sDateTime += "0";
    sDateTime += _ssOut.str() + "-";
    _ssOut.str("");
    _ssOut << iDayOfMonth;
    if (iDayOfMonth < 10) sDateTime += "0";
    sDateTime += _ssOut.str() + " ";
    _ssOut.str("");
    _ssOut << iHour;
    if (iHour < 10) sDateTime += "0";
    sDateTime += _ssOut.str() + ":";
    _ssOut.str("");
    _ssOut << iMin;
    if (iMin < 10) sDateTime += "0";
    sDateTime += _ssOut.str() + ":";
    _ssOut.str("");
    _ssOut << iSec;
    if (iSec < 10) sDateTime += "0";
    sDateTime += _ssOut.str();
    return sDateTime;
}

string Clock::getCurrentDateTimeString(bool bIsStandard, bool bWithSpaces)
{
    time_t rawtime;
    time(&rawtime);
    if (bIsStandard)
    {
        return getDateTimeString(rawtime, bIsStandard, bWithSpaces);
    }
    else
    {
        return getFormattedDateTimeString(rawtime, bWithSpaces);
    }
}

/**
 This function returns the no. of seconds after GMT, January 1, 1970, which is then used as an ID for saving
 results.

 @return long The no. of seconds after GMT, January 1, 1970
 */
long Clock::getCurrentTimeStampInSec()
{
    time_t rawtime;
    time(&rawtime);
    long lTimeInSeconds = (long)rawtime;
    return lTimeInSeconds;
}

time_t Clock::getRawTime()
{
    time_t rawtime;
    time(&rawtime);
    return rawtime;
}

/*
 This function sets the _start to the time when initStartTime() was invoked.
 */
void Clock::initStartTime()
{
    _start = clock();
}

/**
 This function returns the number of seconds passed since the time the Clock class was initialized.

 @param sFunction The name of the function
 @return double Returns the no. of seconds passed since the starting time.
 */
double Clock::displayTimeElapsedSinceInit(string sFunction)
{
    clock_t end = clock();
    double dCPUTime = (double)(difftime(end, _start) / CLOCKS_PER_SEC);
    if (dCPUTime > MIN_TIME_ELAPSED)
    {
        if (CLOCK_VERBOSE)
        {
            //cout << dCPUTime << " seconds : " << sFunction << endl;
            cout << dCPUTime << " seconds : " << endl;
        }
        else
        {
#if 0
            // NOTE: Need to include "LaneViolationConfig.h"
            string sTimingText = string( "" );
            _ssOut << dCPUTime;
            sTimingText = sFunction + string( " took " ) + _ssOut.str() + string( " seconds." );
            Logger *_pLogger = Logger::getInstance();
            _pLogger->writeToFile( FILE_LOG, sTimingText );
            _pLogger = NULL;
            _ssOut.str( "" );
#endif
        }
    }

    // Specify timing information for main() 
    char chStar = sFunction.at(0);
    if (chStar == '*')
    {
        cout << " *[ " << dCPUTime << " seconds ]" << endl; // sFunction << endl;
    }
    return dCPUTime;
}

/**
 This function returns the current date as string.

 @return char* The current date is converted to string format.
 */
char* Clock::getDateString()
{
#ifdef linux
    time_t timeCurrent = time(NULL);
    struct tm sTime;
    localtime_r(&timeCurrent, &sTime);
    strftime(_chDate, 10, "%F", &sTime);
#else
    _strdate( _chDate );
#endif
    return _chDate;
}

/**
 This function returns the current time as string.

 @return char* The current time is converted to string format.
 */
char* Clock::getTimeString()
{
#ifdef linux
    time_t timeCurrent = time(NULL);
    struct tm sTime;
    localtime_r(&timeCurrent, &sTime);
    strftime(_chTime, 10, "%T", &sTime);
#else
    //_strtime_s( chTime, 10 );      // The posts say that it is better not to use Microsoft's *_s standard functions which are actually non-standard.
    _strtime( _chTime );
#endif
    return _chTime;
}

int Clock::getObjCount()
{
    return Clock::_iObjCount;
}

string Clock::getDayOfWeekString(int iDayOfWeek)
{
    string sDayOfWeek = "";
    switch (iDayOfWeek)
    {
        case 0:
            sDayOfWeek = "Sun";
            break;
        case 1:
            sDayOfWeek = "Mon";
            break;
        case 2:
            sDayOfWeek = "Tue";
            break;
        case 3:
            sDayOfWeek = "Wed";
            break;
        case 4:
            sDayOfWeek = "Thu";
            break;
        case 5:
            sDayOfWeek = "Fri";
            break;
        case 6:
            sDayOfWeek = "Sat";
            break;
        default:
            sDayOfWeek = "Undefined";
            break;
    }
    return sDayOfWeek;
}

string Clock::getMonthString(int iMonth)
{
    string sMonth = "";
    switch (iMonth)
    {
        case 0:
            sMonth = "Jan";
            break;
        case 1:
            sMonth = "Feb";
            break;
        case 2:
            sMonth = "Mar";
            break;
        case 3:
            sMonth = "Apr";
            break;
        case 4:
            sMonth = "May";
            break;
        case 5:
            sMonth = "Jun";
            break;
        case 6:
            sMonth = "Jul";
            break;
        case 7:
            sMonth = "Aug";
            break;
        case 8:
            sMonth = "Sep";
            break;
        case 9:
            sMonth = "Oct";
            break;
        case 10:
            sMonth = "Nov";
            break;
        case 11:
            sMonth = "Dec";
            break;
        default:
            sMonth = "Undefined";
            break;
    }
    return sMonth;
}

void Clock::setStartTime(clock_t start)
{
    _start = start;
}
