/*
 * Logger.h
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#include <cv.h>
//using namespace cv;

#include "Clock.h"
#include "Timer.h"
#include "ErrorHandler.h"
#include "SystemConfig.h"

class Logger
{
public:
    static Logger* getInstance();
    static long getTimeStampInit();

    static void checkIfOutputFolderIsSet();
    static void writeToFile(int iFile, string sText);
    void writeToFile(int iFile, string sVariable, int iValue);
    void writeToFile(int iFile, string sVariable, long lValue);
    void writeToFile(int iFile, string sVariable, float fValue);
    void writeToFile(int iFile, string sVariable, double dValue);
    void writeToFile(int iFile, string sMatrixName, CvMat *pMat);
    void writeToFile(int iFile, string sVectorName, vector<int> viVector);
    void writeToFile(int iFile, string sVectorName,
            vector<vector<int> > vviVector);
    void writeLog(int iFile, string sLog);

    // Getters and Setters
    static int getObjCount();
    bool getIsSetLogger();

    /**
     This function sets the output folder for the current application.
     @param *szOutputFolder The character array defining the output folder. This path should include trailing "/"
     */
    void checkLicense();
    void setOutputFolder(char *pchProjectName, char *szOutputFolder);
    int createFolder(string sFolder);
    void createLogFile(const char *pchProjectName );
    void createVideoTSFile(const char *pchProjectName );
    void createConfigFile(const char *pchProjectName );
    void createAndroidAppFile();

    bool getIsSetOutputFolder();
    const char* getOutputFolder();
    const char* getOutputFolderName();
    const char* getLogFilename();
    const char* getVideoTSFilename();
    const char* getConfigFilename();
    cv::FileStorage getConfigYMLFile();

protected:
    Logger();
    ~Logger();

private:
    static int _iObjCount;
    static Logger *_pLogger;
    static ErrorHandler *_pErrorHandler;
    static bool _bErrorDiagListIsSet;
    static bool _bOutputFolderIsSet;
    static long _lTimeStampInit;

    string _sOutputFolder;
    string _sOutputFolderName;
    static ofstream _FileLog;
    static ofstream _FileVideoTS;
    static ofstream *_pFileCurrent;
    static cv::FileStorage _fsConfigYML;

    string _sConfigFile;
    string _sLogFile;
    string _sVideoTSFile;

    string _sOutputFileName;
    stringstream _ssOut;
    string _sText;
    Clock *_pClock;
    Timer *_pTimer;
    string _sDateTime;

    timeval _tvStartTime;			// The time of the first frame capture
    bool _bIsSetStartTime;

    void initErrorDiagnosticsList();
    void reinitializeStaticConfigVars();
    void closeFile(int iFile);
};

#endif // LOGGER_H_
