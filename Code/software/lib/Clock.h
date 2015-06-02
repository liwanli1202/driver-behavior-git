/*
 * Clock.h
 * PoliceEyes project (2011-2012)
 * Ramesh Marikhu <marikhu@gmail.com>
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#ifndef linux
#pragma warning(disable:4996)			// Disabling specific warnings in MSVC++ 2008 EE
#endif

#include <ctime>
#include <string>
#include <sstream>
using namespace std;

#define MIN_TIME_ELAPSED            0.0 //0.0001      // 100 microseconds
#define CLOCK_VERBOSE               false

class Clock
{
public:
	Clock();
	~Clock();

    static string getDateTimeString( time_t rawtime, bool bIsStandard = true, bool bWithSpaces = false );
    static string getFormattedDateTimeString( time_t rawtime, bool bWithSpaces = true );
    static string getFormattedDateTimeStringForMySQL( time_t rawtime );
    static string getCurrentDateTimeString( bool bIsStandard = false, bool bWithSpaces = false );
	static long getCurrentTimeStampInSec();
	static time_t getRawTime();

    void initStartTime();
	double displayTimeElapsedSinceInit( string sFunction );
	char* getDateString();
	char* getTimeString();

    // Getters and setters
    static int getObjCount();
    static string getDayOfWeekString(int iDayOfWeek);
    static string getMonthString(int iMonth);
    void setStartTime( clock_t start );

private:
    static int _iObjCount;
    clock_t _start;

    char _chDate[10];
    char _chTime[10];
};
#endif // CLOCK_H_
