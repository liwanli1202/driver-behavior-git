/*
 * ErrorHandler.cpp
 */
#include <iostream>
using namespace std;

#include "Clock.h"
#include "ErrorHandler.h"
#include "Logger.h"

int ErrorHandler::_iObjCount = 0;
ErrorHandler* ErrorHandler::_pErrorHandler = NULL;
bool ErrorHandler::_bErrorDiagListIsSet = false;

/**
 For any new ErrorCode and ErrorString pair, insert it here.
 Note: If the string starts with '*', it should mean that the error should not terminate the program.
 */
void ErrorHandler::initErrorDiagnosticsList()
{
    if (!ErrorHandler::_bErrorDiagListIsSet)
    {
        insertErrorDiagnostics((int)OK, string("OK"));
        insertErrorDiagnostics((int)NOT_IMPLEMENTED_YET,
                string("Not Implemented Yet."));
        insertErrorDiagnostics((int)OBJECT_IS_NULL, string("Object is Null."));
        insertErrorDiagnostics((int)OBJECT_NOT_FOUND,
                string("Object was not found."));
        insertErrorDiagnostics((int)INVALID_FILE, string("Invalid file."));
        insertErrorDiagnostics((int)INADEQUATE_STRING_LENGTH,
                string(
                        "The length of the input string is shorter than required."));
        insertErrorDiagnostics((int)NULL_IMAGE, string("Null image."));
        insertErrorDiagnostics((int)IMAGE_WITH_INVALID_CHANNEL,
                string("Image with invalid number of channels."));

#define ERRORCODE_UNINITIALIZED_ERRORCODE       101
        insertErrorDiagnostics((int)ERRORCODE_UNINITIALIZED_ERRORCODE,
                string("ErrorHandler : Need to set ErrorCode first."));
#define ERRORCODE_INVALID_ERRORCODE             102
        insertErrorDiagnostics((int)ERRORCODE_INVALID_ERRORCODE,
                string("ErrorHandler : Invalid ErrorCode."));
#define ERRORCODE_DUPLICATE_ERRORCODE           103    
        insertErrorDiagnostics((int)ERRORCODE_DUPLICATE_ERRORCODE,
                string(
                        "ErrorHandler : Duplicate ErrorCode is being inserted."));
#define ERRORCODE_INVALID_WARNING_ERROR_MESSAGE 104
        insertErrorDiagnostics((int)ERRORCODE_INVALID_WARNING_ERROR_MESSAGE,
                string("ErrorHandler : Invalid warning/error message."));
    }

    ErrorHandler::_bErrorDiagListIsSet = true;
}

/** ctor */
ErrorHandler::ErrorHandler()
{
    _iObjCount++;
    _iErrorCode = ERRORCODE_UNINITIALIZED_ERRORCODE;
    _sAppendedText = string("");
    initErrorDiagnosticsList();
}

/** dtor */
ErrorHandler::~ErrorHandler()
{
#if 0
    // Displaying ErrorCode and ErrorString
    int iCount = 0;
    for ( std::map< int, std::string>::const_iterator iter = _mapError.begin();
            iter != _mapError.end(); ++iter )
    {
        iCount++;
        cout << iCount << ". iter->first = " << iter->first << "\titer->second = " << iter->second << endl;
    }
#endif
    _iObjCount--;
    _mapError.clear();
}

/**
 This function inserts an Error Diagnostic into the global map variable g_Map.

 @param iErrorCode The ErrorCode.
 @param sErrorString The string associated with the specified ErrorCode.
 */
void ErrorHandler::insertErrorDiagnostics(int iErrorCode, string sErrorString)
{
    //cout << iErrorCode << " : " << sErrorString << endl;
    _mapIter = _mapError.find(iErrorCode);
    if (_mapIter != _mapError.end())
    {
        _sOut.str("");
        _sOut << iErrorCode << " ErrorString: " << sErrorString << endl;
        setErrorCode((int)ERRORCODE_DUPLICATE_ERRORCODE, _sOut.str());
        _sOut.str("");
    }
    else
    {
        //cout << "iErrorCode = " << iErrorCode << endl;
        _mapError.insert(
                std::map<int, std::string>::value_type(iErrorCode,
                        sErrorString));
    }
}

string ErrorHandler::getErrorString()
{
    return getErrorString(_iErrorCode);
}

void ErrorHandler::displayErrorMsg()
{
    string sErrorString = getErrorString();
    // Write the sErrorString into log file, both the errors and the warnings
    Logger::writeToFile(FILE_LOG, sErrorString);
    if (sErrorString.find(string("WARNING")) != (size_t)-1)
    {
        // This is an escapable exception
        if (DISPLAY_MSG_IN_CONSOLE && DISPLAY_WARNING_MESSAGES)
        {
            cerr << sErrorString << endl;
        }
    }
    else if (sErrorString.find(string("ERROR")) != (size_t)-1)
    {
        // This is an error
        if (DISPLAY_MSG_IN_CONSOLE) cerr << sErrorString << endl;
        throw Exception(sErrorString.c_str(), _iErrorCode);
    }
    else
    {
        // The sErrorString must have "WARNING" or "ERROR" string as per current implementation
        Logger::writeToFile(FILE_LOG, string("Invalid warning/error string"));
        throw Exception(string("Invalid warning/error string").c_str(),
                (int)ERRORCODE_INVALID_WARNING_ERROR_MESSAGE);
    }
}

// Getters and Setters

int ErrorHandler::getObjCount()
{
    return ErrorHandler::_iObjCount;
}

void ErrorHandler::setErrorCode(int iErrorCode, string sAppendedText)
{
    _iErrorCode = iErrorCode;
    _sAppendedText = sAppendedText;
    if (_iErrorCode != OK)
    {
        displayErrorMsg();
    }
}

int ErrorHandler::getErrorCode() const
{
    return _iErrorCode;
}

int ErrorHandler::getLenOfErrorString(int iErrorCode)
{
    string sErrorString = getErrorString(iErrorCode);
    if (sErrorString.find(string("WARNING")) != (size_t)-1)
    {
        return 0;
    }
    else if (sErrorString.find(string("ERROR")) != (size_t)-1)
    {
        return strlen(sErrorString.c_str());
    }
    else
    {
        // The sErrorString must have "WARNING" or "ERROR" string as per current implementation
        throw Exception(string("Invalid warning/error string").c_str(),
                (int)ERRORCODE_INVALID_WARNING_ERROR_MESSAGE);
        return -1;
    }
}

string ErrorHandler::getErrorString(int iErrorCode)
{
    string sErrorMsg = string("");
    string sErrorString = string("");
    // Concatenate Date/Time, _iErrorCode and _iErrorString
    Clock *pClock = new Clock();
    string sDateTime = pClock->getCurrentDateTimeString(true);
    _sOut.str("");
    _sOut << iErrorCode;

    _mapIter = _mapError.find(iErrorCode);
    if (_mapIter == _mapError.end())
    {
        sErrorMsg = string("<ERROR: ") + _sOut.str() + string("> ");
        string sErrStr = getErrorString(ERRORCODE_INVALID_ERRORCODE);
        sErrorMsg += sErrStr + _sAppendedText;
    }
    else
    {
        sErrorString = _mapIter->second;
        if (sErrorString.substr(0, 1) == "*")
        {
            sErrorMsg = string("<WARNING: ") + _sOut.str() + string("> ")
                    + sDateTime + string(" <") + sErrorString + string(">")
                    + _sAppendedText;
            //if( DISPLAY_MSG_IN_CONSOLE ) cerr << sErrorMsg << endl;        
        }
        else
        {
            sErrorMsg = string("<ERROR: ") + _sOut.str() + string("> ")
                    + sDateTime + string(" <") + sErrorString + string(">")
                    + _sAppendedText;
        }
    }
    _sOut.str("");
    return sErrorMsg;
}
