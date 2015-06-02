/*
 * Logger.cpp
 */

#ifdef linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>				// errno.h#else#include <direct.h>             // for getcwd()#endif
#include <iostream>
using namespace std;

#include "Logger.h"
#include "Utilities.h"

int Logger::_iObjCount = 0;
Logger* Logger::_pLogger = NULL;
ErrorHandler* Logger::_pErrorHandler = NULL;
bool Logger::_bErrorDiagListIsSet = false;
bool Logger::_bOutputFolderIsSet = false;
long Logger::_lTimeStampInit = 0;

ofstream Logger::_FileLog;
ofstream Logger::_FileVideoTS;
ofstream* Logger::_pFileCurrent = NULL;
cv::FileStorage Logger::_fsConfigYML;

void Logger::initErrorDiagnosticsList()
{
    if (!Logger::_bErrorDiagListIsSet)
    {
#define LOGGER_OUTPUT_FILE_NOT_OPEN             401
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_OUTPUT_FILE_NOT_OPEN,
                string("Logger : Output file is not open."));
#define LOGGER_INVALID_PATH                     402
        _pErrorHandler->insertErrorDiagnostics((int) LOGGER_INVALID_PATH,
                string(
                        "Logger : Output file could not be opened. Check path."));
#define LOGGER_INVALID_FILE_POINTER             403
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_INVALID_FILE_POINTER,
                string("Logger : Invalid file pointer."));
#define LOGGER_OUTPUT_FOLDER_NOT_SPECIFIED      404
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_OUTPUT_FOLDER_NOT_SPECIFIED,
                string("Logger : Output folder is not specified."));
#define LOGGER_COULD_NOT_CREATE_OUTPUT_FOLDER   405
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_COULD_NOT_CREATE_OUTPUT_FOLDER,
                string("Logger : Could not create output folder."));
#define LOGGER_LOG_FILE_IS_NOT_OPEN_YET         406
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_LOG_FILE_IS_NOT_OPEN_YET,
                string("Logger : Log file is not open yet."));
#define LOGGER_CONFIG_FILE_IS_NOT_OPEN_YET      407
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_CONFIG_FILE_IS_NOT_OPEN_YET,
                string("Logger : Configuration file is not open yet."));

        // Escapable Exceptions
#define LOGGER_NULL_MATRIX                      451
        _pErrorHandler->insertErrorDiagnostics((int) LOGGER_NULL_MATRIX,
                string("*Logger : Null Matrix."));
#define LOGGER_OUTPUT_FOLDER_IS_EMPTY_STRING    452
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_OUTPUT_FOLDER_IS_EMPTY_STRING,
                string(
                        "*Logger : Output folder is not set. So current working directory will be used."));
#define LOGGER_OUTPUT_FOLDER_ALREADY_SPECIFIED  453
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_OUTPUT_FOLDER_ALREADY_SPECIFIED,
                string("*Logger : Output folder has been specified already."));
#define LOGGER_NULL_VECTOR                      454
        _pErrorHandler->insertErrorDiagnostics((int) LOGGER_NULL_VECTOR,
                string("*Logger : Null Vector."));
#define LOGGER_NOT_IMPLEMENTED_FOR_INPUT_MATRIX_TYPE    455
        _pErrorHandler->insertErrorDiagnostics(
                (int) LOGGER_NOT_IMPLEMENTED_FOR_INPUT_MATRIX_TYPE,
                string("*Logger : Not implemented for the input matrix type."));
#define LOGGER_EMPTY_MATRIX                     456
        _pErrorHandler->insertErrorDiagnostics((int) LOGGER_EMPTY_MATRIX,
                string("*Logger : Empty matrix."));
    }
    else
    {
        reinitializeStaticConfigVars();
    }

    Logger::_bErrorDiagListIsSet = true;
}

void Logger::reinitializeStaticConfigVars()
{
    Logger::_lTimeStampInit = Clock::getCurrentTimeStampInSec();
}

// Return the singleton

Logger* Logger::getInstance()
{
    if (!_pErrorHandler) _pErrorHandler = ErrorHandler::getInstance();
    if (!Logger::_pLogger)
    {
        Logger::_pLogger = new Logger();
    }
    return Logger::_pLogger;
}

// Return the TimeStamp during initialization of the program.
long Logger::getTimeStampInit()
{
    return Logger::_lTimeStampInit;
}

void Logger::checkIfOutputFolderIsSet()
{
    if (!Logger::_bOutputFolderIsSet)
    {
        //throw ErrorHandler::Exception("Output folder is not set.",
        //        (int)LOGGER_OUTPUT_FOLDER_NOT_SPECIFIED);
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FOLDER_NOT_SPECIFIED); //,_sOutputFolder);
    }
}

void Logger::writeToFile(int iFile, string sText)
{
    checkIfOutputFolderIsSet();

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if ((*_pFileCurrent).is_open())
    {
        *_pFileCurrent << sText;
        *_pFileCurrent << endl;
    }
    else
    {
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FILE_NOT_OPEN);
    }
}

void Logger::writeToFile(int iFile, string sVariable, int iValue)
{
    checkIfOutputFolderIsSet();

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if ((*_pFileCurrent).is_open())
    {
        _ssOut << iValue;
        _sText = sVariable + " = " + _ssOut.str();
        _ssOut.str("");
        *_pFileCurrent << _sText;
        *_pFileCurrent << endl;
    }
    else
    {
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FILE_NOT_OPEN);
    }
}

void Logger::writeToFile(int iFile, string sVariable, long lValue)
{
    checkIfOutputFolderIsSet();

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if ((*_pFileCurrent).is_open())
    {
        _ssOut << lValue;
        _sText = sVariable + " = " + _ssOut.str();
        _ssOut.str("");
        *_pFileCurrent << _sText;
        *_pFileCurrent << endl;
    }
    else
    {
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FILE_NOT_OPEN);
    }
}

void Logger::writeToFile(int iFile, string sVariable, float fValue)
{
    checkIfOutputFolderIsSet();

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if ((*_pFileCurrent).is_open())
    {
        _ssOut << fValue;
        _sText = sVariable + " = " + _ssOut.str();
        _ssOut.str("");
        *_pFileCurrent << _sText;
        *_pFileCurrent << endl;
    }
    else
    {
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FILE_NOT_OPEN);
    }
}

void Logger::writeToFile(int iFile, string sVariable, double dValue)
{
    checkIfOutputFolderIsSet();

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if ((*_pFileCurrent).is_open())
    {
        _ssOut << dValue;
        _sText = sVariable + " = " + _ssOut.str();
        _ssOut.str("");
        *_pFileCurrent << _sText;
        *_pFileCurrent << endl;
    }
    else
    {
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FILE_NOT_OPEN);
    }
}

void Logger::writeToFile(int iFile, string sMatrixName, CvMat *pMat)
{
    checkIfOutputFolderIsSet();

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if (pMat == NULL)
    {
        _sText = "Matrix " + sMatrixName + " is NULL.";
        *_pFileCurrent << _sText << endl;
        *_pFileCurrent << endl;
        _pErrorHandler->setErrorCode((int) LOGGER_NULL_MATRIX);
    }
    else
    {
        if (pMat->rows == 0 || pMat->cols == 0)
        {
            _pErrorHandler->setErrorCode((int) LOGGER_EMPTY_MATRIX);
            return;
        }
        *_pFileCurrent << "Matrix: " << sMatrixName << endl;
        if (CV_MAT_TYPE(pMat->type) == CV_32SC1)
        {
            int iVal = 0;
            for (int r = 0; r < pMat->rows; r++)
            {
                _sText = "";
                for (int c = 0; c < pMat->cols; c++)
                {
                    iVal = CV_MAT_ELEM(*pMat, int, r, c);
                    _ssOut << iVal;
                    _sText += _ssOut.str() + "\t";
                    _ssOut.str("");
                }
                *_pFileCurrent << _sText << endl;
            }
        }
        else if (CV_MAT_TYPE(pMat->type) == CV_32FC1)
        {
            float fVal = 0.0f;
            for (int r = 0; r < pMat->rows; r++)
            {
                _sText = "";
                for (int c = 0; c < pMat->cols; c++)
                {
                    fVal = CV_MAT_ELEM(*pMat, float, r, c);
                    _ssOut << fVal;
                    _sText += _ssOut.str() + "\t";
                    _ssOut.str("");
                }
                *_pFileCurrent << _sText << endl;
            }
        }
        else if (CV_MAT_TYPE(pMat->type) == CV_64FC1)
        {
            double dVal = 0;
            for (int r = 0; r < pMat->rows; r++)
            {
                _sText = "";
                for (int c = 0; c < pMat->cols; c++)
                {
                    dVal = CV_MAT_ELEM(*pMat, double, r, c);
                    _ssOut << dVal;
                    _sText += _ssOut.str() + "\t";
                    _ssOut.str("");
                }
                *_pFileCurrent << _sText << endl;
            }
        }
        else
        {
            _pErrorHandler->setErrorCode(
                    (int) LOGGER_NOT_IMPLEMENTED_FOR_INPUT_MATRIX_TYPE);
            return;
        }
        *_pFileCurrent << endl;
    }
}

void Logger::writeToFile(int iFile, string sVectorName, vector<int> viVector)
{
    checkIfOutputFolderIsSet();

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if (viVector.size() == 0)
    {
        _sText = "Vector " + sVectorName + "'s size is zero.";
        *_pFileCurrent << _sText << endl;
        *_pFileCurrent << endl;
        _pErrorHandler->setErrorCode((int) LOGGER_NULL_VECTOR);
    }
    else
    {
        *_pFileCurrent << "Vector: " << sVectorName << endl;
        for (unsigned int i = 0; i < viVector.size(); i++)
        {
            *_pFileCurrent << viVector[i] << "\t";
        }
        *_pFileCurrent << endl << endl;
    }
}

void Logger::writeToFile(int iFile, string sVectorName,
        vector<vector<int> > vviVector)
{
    checkIfOutputFolderIsSet();

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if (vviVector.size() == 0)
    {
        _sText = "Vector " + sVectorName + "'s size is zero.";
        *_pFileCurrent << _sText << endl;
        *_pFileCurrent << endl;
        _pErrorHandler->setErrorCode((int) LOGGER_NULL_VECTOR);
    }
    else
    {
        *_pFileCurrent << "Vector: " << sVectorName << endl;
        for (unsigned int i = 0; i < vviVector.size(); i++)
        {
            for (unsigned int j = 0; j < vviVector[i].size(); j++)
            {
                *_pFileCurrent << vviVector[i][j] << " ";
            }
            *_pFileCurrent << endl;
        }
        *_pFileCurrent << endl;
    }
}

void Logger::writeLog(int iFile, string sLog)
{
    checkIfOutputFolderIsSet();
    string sTime = string("[") + _pClock->getCurrentDateTimeString(false)
            + string("] ") + sLog + "\n";

    if (iFile == FILE_LOG) _pFileCurrent = &_FileLog;
    else if (iFile == FILE_VIDEO_TS) _pFileCurrent = &_FileVideoTS;
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
    if ((*_pFileCurrent).is_open())
    {
        *_pFileCurrent << sTime;
    }
    else
    {
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FILE_NOT_OPEN);
    }
}

// Getters and Setters

int Logger::getObjCount()
{
    return Logger::_iObjCount;
}

bool Logger::getIsSetLogger()
{
    if (Logger::_pLogger == NULL) return false;
    return true;
}

void Logger::checkLicense()
{
#define JAN_30_2013         1359542466  // Test release on 31st Jan 2013#define NUM_TEST_DAYS       175          // Around mid July, 2013	long lTimeStampInit = Clock::getCurrentTimeStampInSec();    long lNumSeconds = JAN_30_2013 + (60 * 60 * 24 * NUM_TEST_DAYS);
    if (lTimeStampInit > lNumSeconds)
    {
        // Create a file to identify that invalid license has been invoked.
        if (!(_FileLog.is_open())) _FileLog.open("ivl", ios::binary);
        throw ErrorHandler::Exception("Invalid License.", 9999);
    }
    // Check if the ivl file is present.
    bool bIsFile = false;
    bool bIsIVLFilePresent = Utilities::getIsFileOrFolderPresent("ivl",
            bIsFile);
    if (bIsIVLFilePresent && bIsFile)
    {
        throw ErrorHandler::Exception("Invalid License.", 9999);
    }
}

// Project specific output sub-folders are created.
void Logger::setOutputFolder(char *pchProjectName, char *szOutputFolder)
{
#if 0
    checkLicense();
#endif
    if (!Logger::_bOutputFolderIsSet)
    {
        _sOutputFolder = string(szOutputFolder);
        long lTimeStampInit = Clock::getCurrentTimeStampInSec();
        Logger::_lTimeStampInit = lTimeStampInit;
#if 0
        _sDateTime = _pClock->getFormattedDateTimeString(
                (time_t)(Logger::_lTimeStampInit));
#else
        _sDateTime = _pClock->getFormattedDateTimeStringForMySQL(
                (time_t)(Logger::_lTimeStampInit));
#endif

        // Close previous references to files if present in subsequent runs of the program
        // This is necessary if using applications in which the static objects stay alive in memory
        // even after aborting the application.
        closeFile(FILE_LOG);
        closeFile(FILE_VIDEO_TS);

        if (_sOutputFolder.compare(string("")) == 0)
        {
            _pErrorHandler->setErrorCode(
                    (int) LOGGER_OUTPUT_FOLDER_IS_EMPTY_STRING);
            Utilities *pUtilities = new Utilities();
            _sOutputFolder = pUtilities->getCurWorkingDir(); // Currently, not allowing szOutputFolder = ""
            delete pUtilities;
        }
        else
        {
#if CREATE_SUB_DIR_FOR_OUTPUT
            // Specify output folder for the current application as per current time.
            // Create the directory if not already present
            _sOutputFolder += _sDateTime + string("/");
            _sOutputFolderName = _sDateTime;
#endif
            // Create output folder
            if (createFolder(_sOutputFolder) != 0)
            {
                cerr
                        << "Please make sure that the specified folder is present: "
                        << szOutputFolder << endl << endl;
                exit(0);
            }

            // == Create files as necessary ==
            cout << "Creating project files." << endl;
            string sProjectName = string(pchProjectName);
            createLogFile(sProjectName.c_str());
            //createVideoTSFile(sProjectName.c_str());
            createConfigFile(sProjectName.c_str());
            cout << "Done creating project files." << endl;
        }
    }
    else
    {
        _pErrorHandler->setErrorCode(
                (int) LOGGER_OUTPUT_FOLDER_ALREADY_SPECIFIED);
    }
}

int Logger::createFolder(string sFolder)
{
    int iErrorCode = 0;
#ifdef WIN32
    iErrorCode = mkdir( sFolder.c_str() );
    if(iErrorCode == 0 ) return 0;
    else if( iErrorCode == -1 )
#else
    iErrorCode = mkdir(sFolder.c_str(), 0777);
    if (iErrorCode == 0) return 0;
    else if (iErrorCode == -1)
#endif
    {
        if (DISPLAY_MSG_IN_CONSOLE)
        {
            cerr << "Unable not create specified output folder: " << sFolder
                    << endl;
            std::cerr << "\tError No.: " << errno << endl;
            std::cerr << "\tError String: " << strerror(errno) << endl;
        }
#if 0 // == This is just for information. ==		// If the folder could not be created, then the error will be specified        // in errno
        // To know the actual meaning of errno, install errno using
        //     sudo apt-get install errno
        // Then, in the terminal, you can use
        //      errno 2     (for error no. 2)
        // OR
        //      errno ..    (for the list of error numbers and their meanings)

        marikhu@marikhu-ubuntu:~/work/ATS/software/apps$ errno ..
        _ASM_GENERIC_ERRNO_BASE_H
        EPERM 1 /* Operation not permitted */
        ENOENT 2 /* No such file or directory */
        ESRCH 3 /* No such process */
        EINTR 4 /* Interrupted system call */
        EIO 5 /* I/O error */
        ENXIO 6 /* No such device or address */
        E2BIG 7 /* Argument list too long */
        ENOEXEC 8 /* Exec format error */
        EBADF 9 /* Bad file number */
        ECHILD 10 /* No child processes */
        EAGAIN 11 /* Try again */
        ENOMEM 12 /* Out of memory */
        EACCES 13 /* Permission denied */
        EFAULT 14 /* Bad address */
        ENOTBLK 15 /* Block device required */
        EBUSY 16 /* Device or resource busy */
        EEXIST 17 /* File exists */
        EXDEV 18 /* Cross-device link */
        ENODEV 19 /* No such device */
        ENOTDIR 20 /* Not a directory */
        EISDIR 21 /* Is a directory */
        EINVAL 22 /* Invalid argument */
        ENFILE 23 /* File table overflow */
        EMFILE 24 /* Too many open files */
        ENOTTY 25 /* Not a typewriter */
        ETXTBSY 26 /* Text file busy */
        EFBIG 27 /* File too large */
        ENOSPC 28 /* No space left on device */
        ESPIPE 29 /* Illegal seek */
        EROFS 30 /* Read-only file system */
        EMLINK 31 /* Too many links */
        EPIPE 32 /* Broken pipe */
        EDOM 33 /* Math argument out of domain of func */
        ERANGE 34 /* Math result not representable */
        _ASM_GENERIC_ERRNO_H
        EDEADLK 35 /* Resource deadlock would occur */
        ENAMETOOLONG 36 /* File name too long */
        ENOLCK 37 /* No record locks available */
        ENOSYS 38 /* Function not implemented */
        ENOTEMPTY 39 /* Directory not empty */
        ELOOP 40 /* Too many symbolic links encountered */
        EWOULDBLOCK EAGAIN /* Operation would block */
        ENOMSG 42 /* No message of desired type */
        EIDRM 43 /* Identifier removed */
        ECHRNG 44 /* Channel number out of range */
        EL2NSYNC 45 /* Level 2 not synchronized */
        EL3HLT 46 /* Level 3 halted */
        EL3RST 47 /* Level 3 reset */
        ELNRNG 48 /* Link number out of range */
        EUNATCH 49 /* Protocol driver not attached */
        ENOCSI 50 /* No CSI structure available */
        EL2HLT 51 /* Level 2 halted */
        EBADE 52 /* Invalid exchange */
        EBADR 53 /* Invalid request descriptor */
        EXFULL 54 /* Exchange full */
        ENOANO 55 /* No anode */
        EBADRQC 56 /* Invalid request code */
        EBADSLT 57 /* Invalid slot */
        EDEADLOCK EDEADLK
        EBFONT 59 /* Bad font file format */
        ENOSTR 60 /* Device not a stream */
        ENODATA 61 /* No data available */
        ETIME 62 /* Timer expired */
        ENOSR 63 /* Out of streams resources */
        ENONET 64 /* Machine is not on the network */
        ENOPKG 65 /* Package not installed */
        EREMOTE 66 /* Object is remote */
        ENOLINK 67 /* Link has been severed */
        EADV 68 /* Advertise error */
        ESRMNT 69 /* Srmount error */
        ECOMM 70 /* Communication error on send */
        EPROTO 71 /* Protocol error */
        EMULTIHOP 72 /* Multihop attempted */
        EDOTDOT 73 /* RFS specific error */
        EBADMSG 74 /* Not a data message */
        EOVERFLOW 75 /* Value too large for defined data type */
        ENOTUNIQ 76 /* Name not unique on network */
        EBADFD 77 /* File descriptor in bad state */
        EREMCHG 78 /* Remote address changed */
        ELIBACC 79 /* Can not access a needed shared library */
        ELIBBAD 80 /* Accessing a corrupted shared library */
        ELIBSCN 81 /* .lib section in a.out corrupted */
        ELIBMAX 82 /* Attempting to link in too many shared libraries */
        ELIBEXEC 83 /* Cannot exec a shared library directly */
        EILSEQ 84 /* Illegal byte sequence */
        ERESTART 85 /* Interrupted system call should be restarted */
        ESTRPIPE 86 /* Streams pipe error */
        EUSERS 87 /* Too many users */
        ENOTSOCK 88 /* Socket operation on non-socket */
        EDESTADDRREQ 89 /* Destination address required */
        EMSGSIZE 90 /* Message too long */
        EPROTOTYPE 91 /* Protocol wrong type for socket */
        ENOPROTOOPT 92 /* Protocol not available */
        EPROTONOSUPPORT 93 /* Protocol not supported */
        ESOCKTNOSUPPORT 94 /* Socket type not supported */
        EOPNOTSUPP 95 /* Operation not supported on transport endpoint */
        EPFNOSUPPORT 96 /* Protocol family not supported */
        EAFNOSUPPORT 97 /* Address family not supported by protocol */
        EADDRINUSE 98 /* Address already in use */
        EADDRNOTAVAIL 99 /* Cannot assign requested address */
        ENETDOWN 100 /* Network is down */
        ENETUNREACH 101 /* Network is unreachable */
        ENETRESET 102 /* Network dropped connection because of reset */
        ECONNABORTED 103 /* Software caused connection abort */
        ECONNRESET 104 /* Connection reset by peer */
        ENOBUFS 105 /* No buffer space available */
        EISCONN 106 /* Transport endpoint is already connected */
        ENOTCONN 107 /* Transport endpoint is not connected */
        ESHUTDOWN 108 /* Cannot send after transport endpoint shutdown */
        ETOOMANYREFS 109 /* Too many references: cannot splice */
        ETIMEDOUT 110 /* Connection timed out */
        ECONNREFUSED 111 /* Connection refused */
        EHOSTDOWN 112 /* Host is down */
        EHOSTUNREACH 113 /* No route to host */
        EALREADY 114 /* Operation already in progress */
        EINPROGRESS 115 /* Operation now in progress */
        ESTALE 116 /* Stale NFS file handle */
        EUCLEAN 117 /* Structure needs cleaning */
        ENOTNAM 118 /* Not a XENIX named type file */
        ENAVAIL 119 /* No XENIX semaphores available */
        EISNAM 120 /* Is a named type file */
        EREMOTEIO 121 /* Remote I/O error */
        EDQUOT 122 /* Quota exceeded */
        ENOMEDIUM 123 /* No medium found */
        EMEDIUMTYPE 124 /* Wrong medium type */
        ECANCELED 125 /* Operation Canceled */
        ENOKEY 126 /* Required key not available */
        EKEYEXPIRED 127 /* Key has expired */
        EKEYREVOKED 128 /* Key has been revoked */
        EKEYREJECTED 129 /* Key was rejected by service */
        EOWNERDEAD 130 /* Owner died */
        ENOTRECOVERABLE 131 /* State not recoverable */
        ERFKILL 132 /* Operation not possible due to RF-kill */
        EHWPOISON 133 /* Memory page has hardware error */
#endif
    }
    return errno;
}

void Logger::createLogFile(const char *pchProjectName)
{
#if CREATE_FILE_WITH_TIMESTAMP
    _sOutputFileName = _sOutputFolder + string(pchProjectName) + string("_")
            + _sDateTime + ".log";
#else
    _sOutputFileName = _sOutputFolder + string(pchProjectName) + string(".log");
#endif
    _sLogFile = _sOutputFileName;
    if (!_FileLog.is_open()) _FileLog.open(_sOutputFileName.c_str(), ios::app);

    if (!_FileLog.is_open())
    {
        _pErrorHandler->setErrorCode((int) LOGGER_INVALID_PATH);
    }
    else
    {
        Logger::_bOutputFolderIsSet = true;
        this->writeToFile(FILE_LOG, string("Start Time: ") + _sDateTime);
    }
}

void Logger::createVideoTSFile(const char *pchProjectName)
{
#if CREATE_FILE_WITH_TIMESTAMP
    _sOutputFileName = _sOutputFolder + string(pchProjectName) + string("_")
            + _sDateTime + ".videots";
#else
    _sOutputFileName = _sOutputFolder + string(pchProjectName) + string(".videots");
#endif
    _sVideoTSFile = _sOutputFileName;
    if (!_FileVideoTS.is_open()) _FileVideoTS.open(_sOutputFileName.c_str(),
            ios::app);
    if (!_FileVideoTS.is_open())
    {
        _pErrorHandler->setErrorCode((int) LOGGER_INVALID_PATH);
    }
    else
    {
        _FileVideoTS << _sDateTime;
    }
}

void Logger::createConfigFile(const char *pchProjectName)
{
    if (!_fsConfigYML.isOpened())
    {
#if CREATE_FILE_WITH_TIMESTAMP
        _sOutputFileName = _sOutputFolder + string(pchProjectName) + string("_")
                + _sDateTime + ".config";
#else
        _sOutputFileName = _sOutputFolder + string(pchProjectName) + string(".config");
#endif
        _sConfigFile = _sOutputFileName;
        _fsConfigYML.open(_sOutputFileName, cv::FileStorage::WRITE);
    }
    _fsConfigYML << "Date" << _sDateTime;
    _fsConfigYML.release();
}

void Logger::createAndroidAppFile()
{
    //String format: Wed Mar 21 13-59-03 2012
    //To access via http, we need to
    //replace " " with "%20"
    for (int i = 0; i < 1; i++)
    {
        size_t pos = _sDateTime.find(" ");
        _sDateTime.replace(pos, 1, "%20");
    }

    // Write current date and time to text file
    std::ofstream fileForAndroid;
    fileForAndroid.open("./data_for_android_app/current_folder");
    if (fileForAndroid.is_open())
    {
        fileForAndroid << _sDateTime;
        fileForAndroid.flush();
    }
    fileForAndroid.close();
}

bool Logger::getIsSetOutputFolder()
{
    if (Logger::_bOutputFolderIsSet) return true;
    return false;
}

const char* Logger::getOutputFolder()
{
    if (!Logger::_bOutputFolderIsSet)
    {
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FOLDER_NOT_SPECIFIED,
                _sOutputFolder);
    }
    return _sOutputFolder.c_str();
}

const char* Logger::getOutputFolderName()
{
    if (!Logger::_bOutputFolderIsSet)
    {
        _pErrorHandler->setErrorCode((int) LOGGER_OUTPUT_FOLDER_NOT_SPECIFIED);
    }
    return _sOutputFolderName.c_str();
}

const char* Logger::getLogFilename()
{
    return _sLogFile.c_str();
}

const char* Logger::getVideoTSFilename()
{
    return _sVideoTSFile.c_str();
}

const char* Logger::getConfigFilename()
{
    return _sConfigFile.c_str();
}

cv::FileStorage Logger::getConfigYMLFile()
{
    return _fsConfigYML;
}

// ---- Protected functions ----

Logger::Logger()
{
    _iObjCount++;
    _pErrorHandler = ErrorHandler::getInstance();
    initErrorDiagnosticsList();
    _pClock = new Clock();
    _pTimer = new Timer();
}

Logger::~Logger()
{
    _iObjCount--;
    if (_pErrorHandler) _pErrorHandler = NULL;
    closeFile(FILE_LOG);
    closeFile(FILE_VIDEO_TS);
    if (_fsConfigYML.isOpened()) _fsConfigYML.release();

    if (_pClock) delete _pClock;
    if (_pTimer) delete _pTimer;
}

// ---- Private functions ----

void Logger::closeFile(int iFile)
{
    if (iFile == FILE_LOG)
    {
        _pFileCurrent = &_FileLog;
        if ((*_pFileCurrent).is_open()) (*_pFileCurrent).close();
    }
    else if (iFile == FILE_VIDEO_TS)
    {
        _pFileCurrent = &_FileVideoTS;
        if ((*_pFileCurrent).is_open()) (*_pFileCurrent).close();
    }
    else _pErrorHandler->setErrorCode((int) LOGGER_INVALID_FILE_POINTER);
}

