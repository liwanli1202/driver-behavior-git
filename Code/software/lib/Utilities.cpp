/*
 * Utilities.cpp
 */

#ifndef linux
#pragma warning(disable:4996)			// Disabling specific warnings in MSVC++ 2008 EE
#endif

#include "Utilities.h"

#include <highgui.h>

#include <iostream>
using namespace std;

#ifdef linux
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#endif

int Utilities::_iObjCount = 0;
bool Utilities::_bErrorDiagListIsSet = false;
ErrorHandler* Utilities::_pErrorHandler = NULL;
Logger* Utilities::_pLogger = NULL;

Utilities::Utilities()
{
    _iObjCount++;
    _sText = string("");

#ifdef WIN32
    char *pchCurrentPath = getcwd( _szPathBuffer, _MAX_PATH );
#else
    char cCurrentPath[FILENAME_MAX];
    char *pchCurrentPath = getcwd(_szPathBuffer, sizeof(cCurrentPath));
#endif
    _sCWD = string(pchCurrentPath) + string("/");

    _pLogger = Logger::getInstance();
    _pErrorHandler = ErrorHandler::getInstance();

    _iImageWidth = 320;
    _iImageHeight = 240;
    // The settings below are for 320 x 240 image
    _dHScale = 0.3;
    _dVScale = 0.4;
    _iLineWidth = 1;
    _iNumRowsForText = 12;

    cvInitFont(&_font2, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, 0.6, 0.8, 0,
            2, 8);

    _pMemStorage = cvCreateMemStorage(0);
    _pSeqContours = NULL;
    _pImageContoursTmp = NULL;

    initErrorDiagnosticsList();
}

Utilities::~Utilities()
{
    _iObjCount--;
    if (_pErrorHandler) _pErrorHandler = NULL;
    if (_pLogger) _pLogger = NULL;
    if (_pSeqContours) cvClearSeq(_pSeqContours);
    if (_pImageContoursTmp) cvReleaseImage(&_pImageContoursTmp);
    if (_pMemStorage) cvReleaseMemStorage(&_pMemStorage);
}

void Utilities::initErrorDiagnosticsList()
{
    if (!Utilities::_bErrorDiagListIsSet)
    {
#define UTILITIES_ERROR_READING_FILENAMES                   201
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_ERROR_READING_FILENAMES,
                string("Utilities : Error reading filenames from directory."));
#define UTILITIES_ERROR_CONVERTING_ANSI_TO_UNICODE          202 
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_ERROR_CONVERTING_ANSI_TO_UNICODE,
                string(
                        "Utilities : Error converting ANSI character array to UNICODE."));
#define UTILITIES_ERROR_CONVERTING_UNICODE_TO_ANSI          203
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_ERROR_CONVERTING_UNICODE_TO_ANSI,
                string(
                        "Utilities : Error converting ANSI character array to UNICODE."));
#define UTILITIES_RECURRING_RANDOM_NUMBER					204
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_RECURRING_RANDOM_NUMBER,
                string(
                        "Utilities : Recurring random number. Avoiding infinite looping."));
#define UTILITIES_INSUFFICIENT_NUMBER_OF_POINTS				205
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INSUFFICIENT_NUMBER_OF_POINTS,
                string("Utilities : Insufficient number of points."));
#define UTILITIES_UNABLE_TO_FIT_LINE_TO_MARKERS				206
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_UNABLE_TO_FIT_LINE_TO_MARKERS,
                string("Utilities : Unable to fit line to markers."));
#define UTILITIES_INVALID_NUM_OF_CHANNELS					207
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INVALID_NUM_OF_CHANNELS,
                string("Utilities : Invalid number of channels."));
#define UTILITIES_INVALID_IMAGE_DEPTH						208
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INVALID_IMAGE_DEPTH,
                string("Utilities : Invalid image depth."));
#define UTILITIES_NULL_MATRIX								209
        _pErrorHandler->insertErrorDiagnostics((int)UTILITIES_NULL_MATRIX,
                string("Utilities : Matrix is NULL."));
#define UTILITIES_IMAGE_IS_NULL                             210
        _pErrorHandler->insertErrorDiagnostics((int)UTILITIES_IMAGE_IS_NULL,
                string("Utilities : Image is NULL."));
#define UTILITIES_INVALID_RESIZE_DIMENSIONS                 211
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INVALID_RESIZE_DIMENSIONS,
                string("Utilities : Invalid resize dimensions."));
#define UTILITIES_ROI_NOT_SET                               212
        _pErrorHandler->insertErrorDiagnostics((int)UTILITIES_ROI_NOT_SET,
                string("Utilities : ROI is not set."));
#define UTILITIES_INCONSISTENT_IMAGES                       213
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INCONSISTENT_IMAGES,
                string("Utilities : Images are inconsistent."));
#define UTILITIES_INVALID_BOUNDARY_THICKNESS                214
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INVALID_BOUNDARY_THICKNESS,
                string(
                        "Utilities : Invalid boundary thickness specified. Value should be in range [1,10]"));
#define UTILITIES_ROI_TOO_SMALL                             215
        _pErrorHandler->insertErrorDiagnostics((int)UTILITIES_ROI_TOO_SMALL,
                string("Utilities : ROI is too small."));
#define UTILITIES_INVALID_CHANNEL                           216
        _pErrorHandler->insertErrorDiagnostics((int)UTILITIES_INVALID_CHANNEL,
                string("Utilities : Invalid channel no."));
#define UTILITIES_INVALID_LOCATION_ON_IMAGE                 217
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INVALID_LOCATION_ON_IMAGE,
                string(
                        "Utilities : Invalid location on image. Currently, only top(0) and bottom(1) locations are supported."));
#define UTILITIES_INVALID_NUM_VERTICES_IN_POLYGON           218
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INVALID_NUM_VERTICES_IN_POLYGON,
                string("Utilities : Invalid no. of vertices in polygon."));
#define UTILITIES_INCONSISTENT_SIZE_OF_VECTORS              219
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_INCONSISTENT_SIZE_OF_VECTORS,
                string("Utilities : Inconsistent size of vectors."));
#define UTILITIES_INVALID_DATA_TYPE                         230
        _pErrorHandler->insertErrorDiagnostics((int)UTILITIES_INVALID_DATA_TYPE,
                string("Invalid data type"));

// Escapable Exceptions
#define UTILITIES_ZERO_LENGTH_ANSI_CHAR_ARRAY              251
        _pErrorHandler->insertErrorDiagnostics(
                (int)UTILITIES_ZERO_LENGTH_ANSI_CHAR_ARRAY,
                string(
                        "*Utilities : The input Ansi character array has zero length."));
    }

    Utilities::_bErrorDiagListIsSet = true;
}

#ifdef WIN32
/**
 This function converts Ansi character array to Unicode.
 @param *szAnsi Ansi character array.
 @returns BSTR The LPCWSTR equivalent for the input Ansi character array.
 */
BSTR Utilities::convertAnsiToUnicode( char *szAnsi )
{
    int lenA = lstrlenA( szAnsi );
    int lenW;
    BSTR unicodeForAnsi;

    lenW = ::MultiByteToWideChar( CP_ACP, 0, szAnsi, lenA, 0, 0 );
    if ( lenW > 0 )
    {
        // Check whether conversion was successful
        unicodeForAnsi = ::SysAllocStringLen( 0, lenW );
        ::MultiByteToWideChar( CP_ACP, 0, szAnsi, lenA, unicodeForAnsi, lenW );
    }
    else
    {
        _pErrorHandler->setErrorCode( (int)UTILITIES_ZERO_LENGTH_ANSI_CHAR_ARRAY );
    }
    return unicodeForAnsi;
}

/**
 This function converts Ansi character array to Unicode.
 @param szUnicode Unicode character array, can be BSTR or const TCHAR* as well;
 @returns CHAR* The Ansi character array equivalent for the input Unicode character array.
 */
CHAR* Utilities::convertUnicodeToAnsi( LPCWSTR szUnicode )
{
    if ( szUnicode == NULL )
    {
        _pErrorHandler->setErrorCode( (int)UTILITIES_ERROR_CONVERTING_UNICODE_TO_ANSI );
    }
    int cw = lstrlenW( szUnicode );
    if ( cw == 0 )
    {
        CHAR *psz = new CHAR[1];
        *psz = '\0';
    }
    int cc = WideCharToMultiByte( CP_ACP, 0, szUnicode, cw, NULL, 0, NULL, NULL );
    if( cc == 0 )
    {
        _pErrorHandler->setErrorCode( (int)UTILITIES_ERROR_CONVERTING_UNICODE_TO_ANSI );
    }
    CHAR *psz = new CHAR[ cc+1 ];
    cc = WideCharToMultiByte( CP_ACP, 0, szUnicode, cw, psz, cc, NULL, NULL );
    if (cc==0)
    {
        delete[] psz;
        _pErrorHandler->setErrorCode( (int)UTILITIES_ERROR_CONVERTING_UNICODE_TO_ANSI );
    }
    psz[cc]='\0';
    return psz;
}
#endif

/*
 This function returns a vector of string containing file names specified in *szDir containing
 the wildcard specified in *szPathWithWildcard.

 @param *szDir   The name of the directory where scanning is to be performed. No trailing "/" required, for e.g. C:/foo.
 @param *szPathWithWildcard The absolute or relative path of the file along with wildcard, for e.g. C:/foo/*.txt
 @param bGetAbsolutePath If true, the absolute or relative path of the files is obtained, else only the filename is returned.
 @return std::vector<string> The vector containing the absolute path, relative path or just the name of the file.
 */
#ifdef linux
std::vector<string> Utilities::getFilenamesFromDir(char *szDir,
        char *szPathWithWildcard, bool bGetAbsolutePath)
{
    std::vector<string> vFileNames;

    char szDirnameBuf[256];
    char szBasenameBuf[256];
    char *szBasename, *szDirname;
    string sFileName;

    DIR *pDirectory;
    struct dirent *pDirectoryEntry;

    // NOTE: For some unknown reason, dirname() returns the directory
    // containing the specified directory. So, "." added to the directory name.
    string sDir = string(szDir);
    sDir += ".";

    strncpy(szDirnameBuf, sDir.c_str(), sizeof(szDirnameBuf));
    strncpy(szBasenameBuf, szPathWithWildcard, sizeof(szBasenameBuf));

    szDirname = dirname(szDirnameBuf);
    szBasename = basename(szBasenameBuf);

    pDirectory = opendir(szDirname);
    if (pDirectory)
    {
        while ((pDirectoryEntry = readdir(pDirectory)) != NULL)
        {
            if (fnmatch(szBasename, pDirectoryEntry->d_name, 0) == 0)
            {
                // Create full path of file
                if (bGetAbsolutePath)
                {
                    sFileName = string(szDir);
                    sFileName += pDirectoryEntry->d_name;
                }
                else
                {
                    sFileName = pDirectoryEntry->d_name;
                }
                vFileNames.push_back(sFileName);
            }
        }
    }
    else
    {
        _pErrorHandler->setErrorCode(UTILITIES_ERROR_READING_FILENAMES);
    }
    closedir(pDirectory);
    return vFileNames;
}
#else
std::vector<string> Utilities::getFilenamesFromDir( char *szDir, char *szPathWithWildcard, bool bGetAbsolutePath )
{
    std::vector<string> vFileNames;
    string sFileName;
    WIN32_FIND_DATA info;

#ifdef UNICODE
    BSTR unicodePathWithWildcard = convertAnsiToUnicode( szPathWithWildcard );
    HANDLE h = FindFirstFile( unicodePathWithWildcard, &info );
    ::SysFreeString( unicodePathWithWildcard ); // when done, free the BSTR
#else   // #ifdef UNICODE
    HANDLE h = FindFirstFile( szPathWithWildcard, &info );
#endif

    if ( h == INVALID_HANDLE_VALUE )
    {
        _pErrorHandler->setErrorCode( (int)UTILITIES_ERROR_READING_FILENAMES );
    }
    do
    {
        // Skip Directories
        if (( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0)
        continue;
        CHAR *psz = NULL;
#ifdef UNICODE
        LPCWSTR rect = info.cFileName;
        psz = convertUnicodeToAnsi( rect );
        sFileName += string( psz );
#endif
        if( bGetAbsolutePath )
        {
            // Create full path of file
            sFileName = string( szDir );
#ifdef UNICODE            
            sFileName += string( psz );
#else
            sFileName += string( info.cFileName );
#endif
        }
        else
        {
#ifdef UNICODE
            sFileName = string( psz );
#else
            sFileName = string( info.cFileName );
#endif
        }

        if( psz ) delete[] psz; // This is allocated within Utilities::convertUnicodeToAnsi()

        // Save the filename
        vFileNames.push_back( sFileName );

        // Move to the next file name.
    }
    while( FindNextFile( h, &info ));
    FindClose( h ); // Close dir scan

    return vFileNames;
}
#endif

std::string Utilities::getFilenameWithoutPath(string sFilenameWithPath)
{
    std::string sFilename;
    size_t pos = sFilenameWithPath.find_last_of("/");

    // npos is a static member constant value with the greatest possible value for an element of type size_t.
    // prevent out-of-range values
    if (pos != std::string::npos)
    {
        sFilename.assign(sFilenameWithPath.begin() + pos + 1,
                sFilenameWithPath.end());
    }
    else
    {
        sFilename = sFilenameWithPath;
    }

//	cout << sFilename << endl;
    return sFilename;
}

#ifdef linux
bool Utilities::getIsFileOrFolderPresent(string sPath, bool &bIsFile)
{
    bool bFileOrFolderIsPresent = false;
    bIsFile = false;
    // Check if sPath is a folder
    bool bIsFolder = false;
    DIR *pDir = opendir(sPath.c_str());
    if (pDir != NULL)
    {
        bIsFolder = true;
        bIsFile = false;
        closedir(pDir);
    }
    if (!bIsFolder)
    {
        // Check if sPath is a file
        struct stat buf;
        int iStatus = stat(sPath.c_str(), &buf);
        /* File found */
        if (iStatus == OK)
        {
            bIsFile = true;
        }
    }
    bFileOrFolderIsPresent = bIsFile || bIsFolder;
    return bFileOrFolderIsPresent;
}

#else

bool Utilities::getIsFileOrFolderPresent( string sPath, bool &bIsFile )
{
    DWORD dwReturnValue;
    bool bFileOrFolderIsPresent = false;
#ifdef UNICODE 
    BSTR bstrFile = convertAnsiToUnicode( (char*)sPath.c_str() );
    dwReturnValue = GetFileAttributes( bstrFile );
#else
    dwReturnValue = GetFileAttributes( (LPCSTR)sPath.c_str() );
#endif
    if( dwReturnValue == ((DWORD)-1))		// can else with "else if"
    {
        bFileOrFolderIsPresent = false;
    }
    else if( dwReturnValue != 0xFFFFFFFF )	// can else with "if"
    {
        bFileOrFolderIsPresent = true;
    }
    if( bFileOrFolderIsPresent )
    {
        FILE *fp = fopen( sPath.c_str(), "rb" );
        if( fp != NULL )
        {
            bIsFile = true;
            fclose( fp );
        }
        else
        {
            bIsFile = false;
        }
    }
    return bFileOrFolderIsPresent;
}
#endif



void Utilities::showHistogram(IplImage* img)
{
    IplImage* imgHistogram = 0;
    IplImage* gray = 0;
    CvHistogram* hist;

    // size of the histogram -1D histogram
    int bins = 256;
    int hsize[] = { bins };

    // max and min value of the histogram
    float max_value = 0, min_value = 0;

    // value and normalized value
    float value;
    int normalized;

    //ranges - grayscale 0 to 256
    float xranges[] = { 0, 256 };
    float* ranges[] = { xranges };

    // create an 8 bit single channel image to hold a
    // grayscale version of the original picture
    gray = cvCreateImage(cvGetSize(img), 8, 1);
    cvCvtColor(img, gray, CV_BGR2GRAY);

    // Create 3 windows to show the results
    cvNamedWindow("original", 1);
    cvNamedWindow("gray", 1);
    cvNamedWindow("histogram", 1);

    // planes to obtain the histogram, in this case just one
    IplImage* planes[] = { gray };

    // get the histogram and some info about it
    hist = cvCreateHist(1, hsize, CV_HIST_ARRAY, ranges, 1);
    cvCalcHist(planes, hist, 0, NULL);
    cvGetMinMaxHistValue(hist, &min_value, &max_value);
//	printf( "min: %f, max: %f\n", min_value, max_value );

    // create an 8 bits single channel image to hold the histogram
    // paint it white
    imgHistogram = cvCreateImage(cvSize(bins, 50), 8, 1);
    cvRectangle(imgHistogram, cvPoint(0, 0), cvPoint(256, 50),
            CV_RGB(255, 255, 255), -1);

    // draw the histogram :P
    for (int i = 0; i < bins; i++)
    {
        value = cvQueryHistValue_1D(hist, i);
        normalized = cvRound(value * 50 / max_value);
        //	cout << normalized << endl;
        cvLine(imgHistogram, cvPoint(i, 50), cvPoint(i, 50 - normalized),
                CV_RGB(0, 0, 0));
    }

    // show the image results
    cvShowImage("original", img);
    cvShowImage("gray", gray);
    cvShowImage("histogram", imgHistogram);

    cvWaitKey(0);
}

double Utilities::computeAngle(CvPoint pt1, CvPoint pt2, CvPoint pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2)
            / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

vector<CvPoint2D32f> Utilities::fitLineRANSAC(vector<CvPoint2D32f> vPts,
        CvPoint2D32f &ptP1, CvPoint2D32f &ptP2, double &dAngle, bool bDebug)
{
    // RANSAC Setting
#define MAX_ITERATION						100
#define LINE_ERROR_THRESHOLD				10
#define INLIERS_THRESHOLD					0.8f           /*!< Percentage of points to assert good model */

    int iNoOfPoints = vPts.size();

    if (iNoOfPoints < 3)
    {
        _pErrorHandler->setErrorCode(
                (int)UTILITIES_INSUFFICIENT_NUMBER_OF_POINTS);
    }

    int iMinInlierThreshold = (int)((float)INLIERS_THRESHOLD * iNoOfPoints);
    int iCurIter = 0;
    double bestError = 1.7e+308;
    bool bModelFound = false;
    vector<int> viOutlierIndicesTestFit;
    vector<int> viOutlierIndicesBestFit;

    while (iCurIter < MAX_ITERATION)
    {
        iCurIter++;
        viOutlierIndicesTestFit.clear();

        // Set up variables
        CvPoint2D32f vLinePoints[2];

        int iIndexPt1 = rand() % (iNoOfPoints - 1);
        vLinePoints[0].x = vPts.at(iIndexPt1).x;
        vLinePoints[0].y = vPts.at(iIndexPt1).y;

        int iIndexPt2 = rand() % (iNoOfPoints - 1);
        int iNumTimesRandomize = 0;
        while (iIndexPt2 == iIndexPt1)
        {
            iIndexPt2 = rand() % (iNoOfPoints - 1);
            iNumTimesRandomize++;
            if (iNumTimesRandomize > 1000)
            {
                _pErrorHandler->setErrorCode(
                        (int)UTILITIES_RECURRING_RANDOM_NUMBER);
                break;
            }
        }
        vLinePoints[1].x = vPts.at(iIndexPt2).x;
        vLinePoints[1].y = vPts.at(iIndexPt2).y;
        if (bDebug)
        {
            cout << "iCurIter = " << iCurIter << endl;
            cout << "1: " << iIndexPt1 << " " << vLinePoints[0].x << ", "
                    << vLinePoints[0].y << endl;
            cout << "2: " << iIndexPt2 << " " << vLinePoints[1].x << ", "
                    << vLinePoints[1].y << endl;
        }

        // Find error
        // http://www.allegro.cc/forums/thread/589720
        // int A = x - x1;
        // int B = y - y1;
        // int C = x2 - x1;
        // int D = y2 - y1;
        // int dist = abs(A * D - C * B) / sqrt(C * C + D * D);
        //
        // http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
        // Given line L0: Ax + By + C = 0 and point P1: ( x1, y1 ), the perpendicular distance
        // from P1 to L0 is given by d = | Ax1 + By1 + C | / sqrt( A^2 + B^2 )

        int num_inliers = 0;
        double sum_err = 0;
        float x1 = vLinePoints[0].x;
        float y1 = vLinePoints[0].y;
        float C = vLinePoints[1].x - x1;
        float D = vLinePoints[1].y - y1;
        for (int i = 0; i < iNoOfPoints; i++)
        {
            if (i == iIndexPt1 || i == iIndexPt2) continue;

            float A = vPts.at(i).x - x1;
            float B = vPts.at(i).y - y1;
            float fDistance = abs(A * D - C * B) / sqrt(C * C + D * D);
            if (fDistance < LINE_ERROR_THRESHOLD)
            {
                sum_err += fDistance;
                num_inliers++;
            }
            else
            {
                viOutlierIndicesTestFit.push_back(i);
            }
        }
        if (bDebug)
        {
            cout << "sum_err = " << sum_err << " num_inliers = " << num_inliers
                    << endl;
        }

        // Found better model
        if (num_inliers > iMinInlierThreshold)
        {
            if (sum_err < bestError)
            {
                bModelFound = true;
                bestError = sum_err;
                viOutlierIndicesBestFit.clear();
                if (viOutlierIndicesTestFit.size() > 0)
                {
                    for (unsigned int iOutlier = 0;
                            iOutlier < viOutlierIndicesTestFit.size();
                            iOutlier++)
                    {
                        viOutlierIndicesBestFit.push_back(
                                viOutlierIndicesTestFit[iOutlier]);
                    }
                }
                if (bDebug)
                {
                    cout << "bestError = " << bestError << endl;
                }
                if (vLinePoints[0].y < vLinePoints[1].y)
                {
                    ptP1 = vLinePoints[0];
                    ptP2 = vLinePoints[1];
                }
                else
                {
                    ptP1 = vLinePoints[1];
                    ptP2 = vLinePoints[0];
                }
            }
        }
    }
    vector<CvPoint2D32f> vInlierPts;
    if (bModelFound)
    {
        // Remove the outlier points
        bool bInlier = false;
        for (unsigned int iPt = 0; iPt < vPts.size(); iPt++)
        {
            // Check if it is an inlier
            bInlier = true;
            for (unsigned int iOutlier = 0;
                    iOutlier < viOutlierIndicesBestFit.size(); iOutlier++)
            {
                unsigned int iOutlierLoc = viOutlierIndicesBestFit[iOutlier];
                if (iPt == iOutlierLoc)
                {
                    bInlier = false;
                    break;
                }
            }
            if (bInlier)
            {
                vInlierPts.push_back(vPts[iPt]);
            }
        }

        // NOTE: For better results, given the inlier points, obtain the slope of the line that best
        // fits the inliers. The points PtP1 and ptP2 are two points on this best fit line.
        // Ensure that ptP1.x < ptP2.x
        CvPoint2D32f ptTemp;
        if (ptP1.x > ptP2.x)
        {
            ptTemp.x = ptP1.x;
            ptTemp.y = ptP1.y;
            ptP1.x = ptP2.x;
            ptP1.y = ptP2.y;
            ptP2.x = ptTemp.x;
            ptP2.y = ptTemp.y;
        }
        dAngle = atan2(ptP2.y - ptP1.y, ptP2.x - ptP1.x);
        if (bDebug)
        {
            cout << "ptP1 " << ptP1.x << " " << ptP1.y << endl;
            cout << "ptP2 " << ptP2.x << " " << ptP2.y << endl;
            cout << "dAngle = " << dAngle << "(rad) " << dAngle * 180 / CV_PI
            << "(deg)" << endl;
        }
    }
    else
    {
        _pErrorHandler->setErrorCode(
                (int)UTILITIES_UNABLE_TO_FIT_LINE_TO_MARKERS);
    }
    return vInlierPts;
}

IplImage* Utilities::getRotatedImage(IplImage *pImage, double dAngleRad,
        CvPoint2D32f &ptP1, CvPoint2D32f &ptP2, bool bDebug)
{
    if (pImage->depth != IPL_DEPTH_8U)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_IMAGE_DEPTH);
    }
    if (bDebug)
    {
        cout << "dAngle in radians = " << dAngleRad << endl;
        cout << "dAngle in degrees = " << dAngleRad * 180 / CV_PI << endl;
    }

    // NOTE: Considering the image origin (0,0) as the pivot for rotation
    // This ensures that we do not need translation to move pivot to origin.

    int iWidth = pImage->width;
    int iHeight = pImage->height;

    // Compute the four corners of the image
    CvPoint ptA = cvPoint(0, 0);
    CvPoint ptB = cvPoint(0, iHeight);
    CvPoint ptC = cvPoint(iWidth, iHeight);
    CvPoint ptD = cvPoint(iWidth, 0);

    // Put the points into a matrix
    CvMat *pMatCornerPts = cvCreateMat(3, 4, CV_32FC1);
    CV_MAT_ELEM( *pMatCornerPts, float, 0, 0 ) = (float)ptA.x;
    CV_MAT_ELEM( *pMatCornerPts, float, 1, 0 ) = (float)ptA.y;
    CV_MAT_ELEM( *pMatCornerPts, float, 2, 0 ) = 1.0f;
    CV_MAT_ELEM( *pMatCornerPts, float, 0, 1 ) = (float)ptB.x;
    CV_MAT_ELEM( *pMatCornerPts, float, 1, 1 ) = (float)ptB.y;
    CV_MAT_ELEM( *pMatCornerPts, float, 2, 1 ) = 1.0f;
    CV_MAT_ELEM( *pMatCornerPts, float, 0, 2 ) = (float)ptC.x;
    CV_MAT_ELEM( *pMatCornerPts, float, 1, 2 ) = (float)ptC.y;
    CV_MAT_ELEM( *pMatCornerPts, float, 2, 2 ) = 1.0f;
    CV_MAT_ELEM( *pMatCornerPts, float, 0, 3 ) = (float)ptD.x;
    CV_MAT_ELEM( *pMatCornerPts, float, 1, 3 ) = (float)ptD.y;
    CV_MAT_ELEM( *pMatCornerPts, float, 2, 3 ) = 1.0f;
    if (bDebug) _pLogger->writeToFile(FILE_LOG, "pMatCornerPts", pMatCornerPts);

    // Prepare the rotation matrix
    CvMat *pMatR = cvCreateMat(3, 3, CV_32FC1);
    CV_MAT_ELEM( *pMatR, float, 0, 0 ) = (float)cos(dAngleRad);
    CV_MAT_ELEM( *pMatR, float, 0, 1 ) = (float)(-1 * sin(dAngleRad));
    CV_MAT_ELEM( *pMatR, float, 0, 2 ) = 0.0f;
    CV_MAT_ELEM( *pMatR, float, 1, 0 ) = (float)sin(dAngleRad);
    CV_MAT_ELEM( *pMatR, float, 1, 1 ) = (float)cos(dAngleRad);
    CV_MAT_ELEM( *pMatR, float, 1, 2 ) = 0.0f;
    CV_MAT_ELEM( *pMatR, float, 2, 0 ) = 0.0f;
    CV_MAT_ELEM( *pMatR, float, 2, 1 ) = 0.0f;
    CV_MAT_ELEM( *pMatR, float, 2, 2 ) = 1.0f;
    if (bDebug) _pLogger->writeToFile(FILE_LOG, "pMatR", pMatR);

    // Rotate the corner points
    CvMat *pMatCornerPtsR = cvCreateMat(3, 4, CV_32FC1);
    cvGEMM(pMatR, pMatCornerPts, 1, NULL, 0, pMatCornerPtsR, 0);
    if (bDebug) _pLogger->writeToFile(FILE_LOG, "pMatCornerPtsR",
            pMatCornerPtsR);

    // Obtain bounds for the rotated image.
    float fMinX = 0.0f;
    float fMaxX = 0.0f;
    float fMinY = 0.0f;
    float fMaxY = 0.0f;
    for (int iPt = 0; iPt < pMatCornerPtsR->cols; iPt++)
    {
        float fX = CV_MAT_ELEM( *pMatCornerPtsR, float, 0, iPt );
        float fY = CV_MAT_ELEM( *pMatCornerPtsR, float, 1, iPt );
        if (fMinX > fX) fMinX = fX;
        if (fMaxX < fX) fMaxX = fX;
        if (fMinY > fY) fMinY = fY;
        if (fMaxY < fY) fMaxY = fY;
    }

    // Obtain the four corners of the rotated image.
    IplImage *pImageRotated = cvCreateImage(
            cvSize((cvFloor)(fMaxX - fMinX + 1 + 0.5),
                    (cvFloor)(fMaxY - fMinY + 1 + 0.5)), pImage->depth,
            pImage->nChannels);
    cvSetZero(pImageRotated);

    // Obtain inverse of rotation matrix pMatR
    CvMat *pMatRInv = cvCreateMat(3, 3, CV_32FC1);
    cvInvert(pMatR, pMatRInv, CV_LU);
    if (bDebug) _pLogger->writeToFile(FILE_LOG, "pMatRInv", pMatRInv);

    // The (0,0) of pImageRotated is the point ptOffset
    CvPoint ptOffset = cvPoint((cvFloor)(fMinX + 0.5), (cvFloor)(fMinY + 0.5));

    // For every pixel in pImageRotated, obtain the pixel value from pImage if valid.
    // Since the pixels from pImage, when rotated, may not fall on the integer pixel location
    // in the pImageRotated, the pixels from pImageRotated is considered and is mapped onto the
    // source pixel in pImage. Currently, nearest neighbor pixel is used.
    CvMat *pMatPixLoc = cvCreateMat(3, 1, CV_32FC1);
    CV_MAT_ELEM( *pMatPixLoc, float, 2, 0 ) = 1.0f;
    int iPixLocX = 0;
    int iPixLocY = 0;
    int iPixVal = 0;
    for (int iRow = 0; iRow < pImageRotated->height; iRow++)
    {
        for (int iCol = 0; iCol < pImageRotated->width; iCol++)
        {
            CV_MAT_ELEM( *pMatPixLoc, float, 0, 0 ) =
                    (float)(iCol + ptOffset.x);
            CV_MAT_ELEM( *pMatPixLoc, float, 1, 0 ) =
                    (float)(iRow + ptOffset.y);
            // Get pixel location in pImage
            cvGEMM(pMatRInv, pMatPixLoc, 1, NULL, 0, pMatPixLoc, 0);
            iPixLocX = (cvFloor)(CV_MAT_ELEM( *pMatPixLoc, float, 0, 0 ) + 0.5);
            iPixLocY = (cvFloor)(CV_MAT_ELEM( *pMatPixLoc, float, 1, 0 ) + 0.5);
            if (iPixLocX >= 0 && iPixLocX < pImage->width && iPixLocY >= 0
                    && iPixLocY < pImage->height)
            {
                for (int iChannel = 0; iChannel < pImageRotated->nChannels;
                        iChannel++)
                {
                    // Valid pixel location in pImage
                    iPixVal = (int)(unsigned char)pImage->imageData[iPixLocY
                            * pImage->widthStep + iPixLocX * pImage->nChannels
                            + iChannel];
                    // Set the pixel value into pImageRotated
                    pImageRotated->imageData[iRow * pImageRotated->widthStep
                            + iCol * pImageRotated->nChannels + iChannel] =
                            (unsigned char)iPixVal;
                }
            }
        }
    }

    // Compute rotation of points ptP1 and ptP2
    // Fill points ptP1 and ptP2 into matrix
    CvMat *pMatPtsOnLine = cvCreateMat(3, 2, CV_32FC1);
    CV_MAT_ELEM( *pMatPtsOnLine, float, 0, 0 ) = ptP1.x;
    CV_MAT_ELEM( *pMatPtsOnLine, float, 1, 0 ) = ptP1.y;
    CV_MAT_ELEM( *pMatPtsOnLine, float, 2, 0 ) = 1.0f;
    CV_MAT_ELEM( *pMatPtsOnLine, float, 0, 1 ) = ptP2.x;
    CV_MAT_ELEM( *pMatPtsOnLine, float, 1, 1 ) = ptP2.y;
    CV_MAT_ELEM( *pMatPtsOnLine, float, 2, 1 ) = 1.0f;

    // Rotate the points
    CvMat *pMatPtsOnLineR = cvCreateMat(3, 2, CV_32FC1);
    cvGEMM(pMatR, pMatPtsOnLine, 1, NULL, 0, pMatPtsOnLineR, 0);

    // Subtract the offset from the rotated point
    CV_MAT_ELEM( *pMatPtsOnLineR, float, 0, 0 ) -= ptOffset.x;
    CV_MAT_ELEM( *pMatPtsOnLineR, float, 1, 0 ) -= ptOffset.y;
    CV_MAT_ELEM( *pMatPtsOnLineR, float, 0, 1 ) -= ptOffset.x;
    CV_MAT_ELEM( *pMatPtsOnLineR, float, 1, 1 ) -= ptOffset.y;

    ptP1.x = CV_MAT_ELEM( *pMatPtsOnLineR, float, 0, 0 );
    ptP1.y = CV_MAT_ELEM( *pMatPtsOnLineR, float, 1, 0 );
    ptP2.x = CV_MAT_ELEM( *pMatPtsOnLineR, float, 0, 1 );
    ptP2.y = CV_MAT_ELEM( *pMatPtsOnLineR, float, 1, 1 );

    // Clean up
    cvReleaseMat(&pMatPtsOnLine);
    cvReleaseMat(&pMatPtsOnLineR);
    cvReleaseMat(&pMatPixLoc);
    cvReleaseMat(&pMatR);
    cvReleaseMat(&pMatCornerPts);
    cvReleaseMat(&pMatCornerPtsR);
    cvReleaseMat(&pMatRInv);

    return pImageRotated;
}

IplImage* Utilities::getResizedImage(const IplImage *pImageSrc, int iWidthDst,
        int iHeightDst)
{
    if (!pImageSrc)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_IMAGE_IS_NULL);
    }
    if (iWidthDst <= 0 || iHeightDst <= 0)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_RESIZE_DIMENSIONS);
    }

    // Scale the image to the new dimensions, even if the aspect ratio will be changed.
    IplImage *pImageDst = cvCreateImage(cvSize(iWidthDst, iHeightDst),
            pImageSrc->depth, pImageSrc->nChannels);

    if ((iWidthDst > pImageSrc->width) && (iHeightDst > pImageSrc->height))
    {
        // Make the image larger
        cvResetImageROI((IplImage*)pImageSrc);
        cvResize(pImageSrc, pImageDst, CV_INTER_LINEAR); // CV_INTER_CUBIC or CV_INTER_LINEAR is good for enlarging
    }
    else
    {
        // Make the image smaller
        cvResetImageROI((IplImage*)pImageSrc);
        cvResize(pImageSrc, pImageDst, CV_INTER_AREA); // CV_INTER_AREA is good for shrinking / decimation, but bad at enlarging.
    }
    return pImageDst;
}

CvMat* Utilities::getRotationMatrix(double dAngleRad, bool bDebug)
{
    CvMat *pMatRotation = cvCreateMat(3, 3, CV_32FC1);
    CV_MAT_ELEM( *pMatRotation, float, 0, 0 ) = cos(dAngleRad);
    CV_MAT_ELEM( *pMatRotation, float, 0, 1 ) = -1 * sin(dAngleRad);
    CV_MAT_ELEM( *pMatRotation, float, 0, 2 ) = 0;
    CV_MAT_ELEM( *pMatRotation, float, 1, 0 ) = sin(dAngleRad);
    CV_MAT_ELEM( *pMatRotation, float, 1, 1 ) = cos(dAngleRad);
    CV_MAT_ELEM( *pMatRotation, float, 1, 2 ) = 0;
    CV_MAT_ELEM( *pMatRotation, float, 2, 0 ) = 0;
    CV_MAT_ELEM( *pMatRotation, float, 2, 1 ) = 0;
    CV_MAT_ELEM( *pMatRotation, float, 2, 2 ) = 1;

    if (bDebug)
    {
        cout << "pMatRotation for " << dAngleRad << " radians." << endl;
        for (int iRow = 0; iRow < pMatRotation->rows; iRow++)
        {
            for (int iCol = 0; iCol < pMatRotation->cols; iCol++)
            {
                cout << CV_MAT_ELEM( *pMatRotation, float, iRow, iCol ) << "\t";
            }
            cout << endl;
        }
    }
    return pMatRotation;
}

/*
 This function rotates a point about the specified pivot point.
 */
void Utilities::getRotatedPoint(CvPoint &pointToRotate, double dOri,
        CvPoint pointPivot)
{
    // Translate pivot point to origin.
    pointToRotate.x -= pointPivot.x;
    pointToRotate.y -= pointPivot.y;

    // Rotate the point about origin first
    int iPtOriXr = cvFloor(
            (cos(dOri) * pointToRotate.x - sin(dOri) * pointToRotate.y) + 0.5);
    // In the computation below, -1 is used because the Y-axis of the image coordinates
    // increments downwards from origin in image coordinates.
    int iPtOriYr = -1
            * cvFloor(
                    (sin(dOri) * pointToRotate.x + cos(dOri) * pointToRotate.y)
                            + 0.5);

    // Translate the rotated points with respect to original location of pivot point.
    pointToRotate.x = iPtOriXr + pointPivot.x;
    pointToRotate.y = iPtOriYr + pointPivot.y;
}

/**
 This function returns the transformation matrix, given the rotation and translation matrix.
 @param *pMatR The 3x3 rotation matrix
 @param *pMatT The 1x3 translation matrix
 @param **ppMatTrans The 4x4 transformation matrix that will be returned.
 */
void Utilities::getTransformationMatFromRandT(const CvMat *pMatR,
        const CvMat *pMatT, CvMat **ppMatTrans)
{
    CvMat *pMatTrans = *ppMatTrans;

    CV_MAT_ELEM( *pMatTrans, float, 0 , 0 ) =
            CV_MAT_ELEM( *pMatR, float, 0 , 0 );
    CV_MAT_ELEM( *pMatTrans, float, 0 , 1 ) =
            CV_MAT_ELEM( *pMatR, float, 0 , 1 );
    CV_MAT_ELEM( *pMatTrans, float, 0 , 2 ) =
            CV_MAT_ELEM( *pMatR, float, 0 , 2 );
    CV_MAT_ELEM( *pMatTrans, float, 1 , 0 ) =
            CV_MAT_ELEM( *pMatR, float, 1 , 0 );
    CV_MAT_ELEM( *pMatTrans, float, 1 , 1 ) =
            CV_MAT_ELEM( *pMatR, float, 1 , 1 );
    CV_MAT_ELEM( *pMatTrans, float, 1 , 2 ) =
            CV_MAT_ELEM( *pMatR, float, 1 , 2 );
    CV_MAT_ELEM( *pMatTrans, float, 2 , 0 ) =
            CV_MAT_ELEM( *pMatR, float, 2 , 0 );
    CV_MAT_ELEM( *pMatTrans, float, 2 , 1 ) =
            CV_MAT_ELEM( *pMatR, float, 2 , 1 );
    CV_MAT_ELEM( *pMatTrans, float, 2 , 2 ) =
            CV_MAT_ELEM( *pMatR, float, 2 , 2 );

    CV_MAT_ELEM( *pMatTrans, float, 0 , 3 ) =
            CV_MAT_ELEM( *pMatT, float, 0 , 0 );
    CV_MAT_ELEM( *pMatTrans, float, 1 , 3 ) =
            CV_MAT_ELEM( *pMatT, float, 1 , 0 );
    CV_MAT_ELEM( *pMatTrans, float, 2 , 3 ) =
            CV_MAT_ELEM( *pMatT, float, 2 , 0 );

    CV_MAT_ELEM( *pMatTrans, float, 3 , 0 ) = 0;
    CV_MAT_ELEM( *pMatTrans, float, 3 , 1 ) = 0;
    CV_MAT_ELEM( *pMatTrans, float, 3 , 2 ) = 0;
    CV_MAT_ELEM( *pMatTrans, float, 3 , 3 ) = 1;

    *ppMatTrans = pMatTrans;
}

/**
 This function returns the rotation and translation matrix, given the tranformation matrix.
 @param *pMatTrans The 4x4 translation matrix that is specified.
 @param **ppMatR The 3x3 rotation matrix that will be returned.
 @param **ppMatT The 1x3 translation matrix that will be returned.
 */
void Utilities::getRandTMatFromTransformationMat(const CvMat *pMatTrans,
        CvMat **ppMatR, CvMat **ppMatT)
{
    CvMat *pMatR = *ppMatR;
    CvMat *pMatT = *ppMatT;

    CV_MAT_ELEM( *pMatR, float, 0 , 0 ) =
            CV_MAT_ELEM( *pMatTrans, float, 0 , 0 );
    CV_MAT_ELEM( *pMatR, float, 0 , 1 ) =
            CV_MAT_ELEM( *pMatTrans, float, 0 , 1 );
    CV_MAT_ELEM( *pMatR, float, 0 , 2 ) =
            CV_MAT_ELEM( *pMatTrans, float, 0 , 2 );
    CV_MAT_ELEM( *pMatR, float, 1 , 0 ) =
            CV_MAT_ELEM( *pMatTrans, float, 1 , 0 );
    CV_MAT_ELEM( *pMatR, float, 1 , 1 ) =
            CV_MAT_ELEM( *pMatTrans, float, 1 , 1 );
    CV_MAT_ELEM( *pMatR, float, 1 , 2 ) =
            CV_MAT_ELEM( *pMatTrans, float, 1 , 2 );
    CV_MAT_ELEM( *pMatR, float, 2 , 0 ) =
            CV_MAT_ELEM( *pMatTrans, float, 2 , 0 );
    CV_MAT_ELEM( *pMatR, float, 2 , 1 ) =
            CV_MAT_ELEM( *pMatTrans, float, 2 , 1 );
    CV_MAT_ELEM( *pMatR, float, 2 , 2 ) =
            CV_MAT_ELEM( *pMatTrans, float, 2 , 2 );

    CV_MAT_ELEM( *pMatT, float, 0 , 0 ) =
            CV_MAT_ELEM( *pMatTrans, float, 0 , 3 );
    CV_MAT_ELEM( *pMatT, float, 1 , 0 ) =
            CV_MAT_ELEM( *pMatTrans, float, 1 , 3 );
    CV_MAT_ELEM( *pMatT, float, 2 , 0 ) =
            CV_MAT_ELEM( *pMatTrans, float, 2 , 3 );

    *ppMatR = pMatR;
    *ppMatT = pMatT;
}

/**
 This function scales the values of the matrix by the specified multiplier value.
 @param **ppTMat The input matrix is retuned in the same variable.
 @param dMultiplier The value to multiply the matrix.
 */
void Utilities::getMatrixScalarMultiply(CvMat **ppMat, double dMultiplier)
{
    CvMat *pTMat = *ppMat;
    for (int r = 0; r < pTMat->rows; r++)
        for (int c = 0; c < pTMat->cols; c++)
            CV_MAT_ELEM( *pTMat, float, r, c ) =
                    (float)(CV_MAT_ELEM( *pTMat, float, r, c ) * dMultiplier);
}

void Utilities::displayMatrix(CvMat *pMat, char *pchMatName)
{
    if (!pMat)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_NULL_MATRIX);
    }
    cout << "\nMatrix: " << pchMatName << endl;
    for (int iRow = 0; iRow < pMat->rows; iRow++)
    {
        for (int iCol = 0; iCol < pMat->cols; iCol++)
        {
            cout << CV_MAT_ELEM( *pMat, float, iRow, iCol ) << "\t";
        }
        cout << endl;
    }
}

/**
 The function checks whether the given matrices have the same size and type.
 */
bool Utilities::getIsEqualSizedMatrices(CvMat *pMatA, CvMat *pMatB)
{
    return ((pMatA->rows == pMatB->rows) && (pMatA->cols == pMatB->cols)
            && (pMatA->type == pMatB->type));
}

/**
 The function checks whether the given images have the same size and type.
 */
bool Utilities::getIsEqualSizedImages(IplImage *pImageA, IplImage *pImageB)
{
    return ((pImageA->width == pImageB->width)
            && (pImageA->height == pImageB->height)
            && (pImageA->nChannels == pImageB->nChannels)
            && (pImageA->depth == pImageB->depth));
}

void Utilities::sortVecOfStrings(vector<string> &vString)
{
    string sTemp;
    if (vString.size() > 1)
    {
        // First order by length of the string
        for (unsigned int i = 0; i < vString.size() - 1; i++)
        {
            for (unsigned int j = i + 1; j < vString.size(); j++)
            {
                if (vString.at(j).size() < vString.at(i).size())
                {
                    sTemp = vString.at(i);
                    vString.at(i) = vString.at(j);
                    vString.at(j) = sTemp;
                }
            }
        }
        // Then, order by the string comparison for strings with same size
        for (unsigned int i = 0; i < vString.size() - 1; i++)
        {
            for (unsigned int j = i + 1; j < vString.size(); j++)
            {
                if (vString.at(j) < vString.at(i))
                {
                    if (vString.at(j).size() == vString.at(i).size())
                    {
                        sTemp = vString.at(i);
                        vString.at(i) = vString.at(j);
                        vString.at(j) = sTemp;
                    }
                }
            }
        }
    }
}

CvRect Utilities::getROIFromAreaMask(const IplImage *pImageAreaMask,
        bool bDebug)
{
    IplImage *pImage = NULL;
    if (pImageAreaMask->nChannels != 1)
    {
        pImage = cvCreateImage(cvGetSize(pImageAreaMask), IPL_DEPTH_8U, 1);
        cvConvertImage(pImageAreaMask, pImage);
    }
    else
    {
        pImage = cvCloneImage(pImageAreaMask);
    }
// TODO: It seems like when pImageAreaMask has enabled pixels along the border,
// the resulting ROI is smaller.
// So, need to create pImage with an additional pixel on the border,
// and then update rectROI accordingly to account for this additional border pixel.

// NOTE: cvFindContours edits pImage
    cvFindContours(pImage, _pMemStorage, &_pSeqContours, sizeof(CvContour),
            CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
    if (pImage) cvReleaseImage(&pImage);

    CvRect rectCur;
    CvRect rectROI;
    bool bIsSetROI = false;
    for (CvSeq *pCurContour = _pSeqContours; pCurContour != NULL; pCurContour =
            pCurContour->h_next)
    {
        rectCur = ((CvContour*)pCurContour)->rect;
        if (!bIsSetROI)
        {
            rectROI = rectCur;
            bIsSetROI = true;
        }
        else
        {
            rectROI = cvMaxRect(&rectROI, &rectCur);
        }
    }
    if (!bIsSetROI)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_ROI_NOT_SET);
    }
    else
    {
        if (rectROI.width < 20 || rectROI.height < 20)
        {
            _pErrorHandler->setErrorCode((int)UTILITIES_ROI_TOO_SMALL);
        }
#if 0
        // Ensure that ROI width and height is divisible by 4
        if (rectROI.width % 4 != 0)
        {
            rectROI.width -= rectROI.width % 4;
        }
        if (rectROI.height % 4 != 0)
        {
            rectROI.height -= rectROI.height % 4;
        }
#endif
    }
    if (_pMemStorage) cvClearMemStorage(_pMemStorage);
    return rectROI;
}

void Utilities::getBoundaryOfMask(const IplImage *pImageMask,
        IplImage *pImageMaskBoundary, int iBoundaryThickness, bool bDebug)
{
    if (!pImageMask || !pImageMaskBoundary)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_IMAGE_IS_NULL);
    }
    if (pImageMask->depth != IPL_DEPTH_8U)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_IMAGE_DEPTH);
    }
    if (pImageMask->nChannels != 1)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_NUM_OF_CHANNELS);
    }
    checkImages(pImageMask, pImageMaskBoundary, "getBoundaryOfMask");

// Initialization
    cvSetZero(pImageMaskBoundary);
    IplImage *pImageMaskEroded = cvCreateImage(cvGetSize(pImageMask),
            IPL_DEPTH_8U, 1);
    IplImage *pImageMaskDilated = cvCreateImage(cvGetSize(pImageMask),
            IPL_DEPTH_8U, 1);

// Boundary constraints
    if (iBoundaryThickness < 1 || iBoundaryThickness > 10)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_BOUNDARY_THICKNESS);
    }

// Morphological operations
    int iNumItersErode = 0;
    int iNumItersDilate = 0;
    iNumItersErode = iBoundaryThickness / 2 + iBoundaryThickness % 2;
    iNumItersDilate = iBoundaryThickness / 2;
    if (iNumItersErode > 0)
    {
        cvErode(pImageMask, pImageMaskEroded, NULL, iNumItersErode);
    }
    if (iNumItersDilate > 0)
    {
        cvDilate(pImageMask, pImageMaskDilated, NULL, iNumItersDilate);
    }
    if (iNumItersDilate == 0)
    {
        cvSub(pImageMask, pImageMaskEroded, pImageMaskBoundary);
    }
    else
    {
        cvSub(pImageMaskDilated, pImageMaskEroded, pImageMaskBoundary);
    }

// Clean up
    if (pImageMaskEroded) cvReleaseImage(&pImageMaskEroded);
    if (pImageMaskDilated) cvReleaseImage(&pImageMaskDilated);
}

#define STD_DEV_THRESHOLD  ( 5.0 * 5.0 )
/*
 * This function computes NCC score for the point (iCol, iRow) considering
 * a mask of size (2*iMaskSize+1) in pImageGray and pImageBg.
 */
bool Utilities::getNCC(const IplImage *pImageT, const IplImage *pImageI,
        int iRow, int iCol, int iMaskSize, float &fNCC, bool bShadowTest,
        bool bDebug)
{
    int iRowLoc = 0, iColLoc = 0;
    int iN = 0;
    int iPixLoc = 0;
    double X = 0, Y = 0, XX = 0, YY = 0, XY = 0;
    int iValT = 0;
    int iValI = 0;

    // TODO: Mean for pImageBg pixels can be computed beforehand
    // TODO: Implement dynamic programming to make computation faster
    for (int iRowStart = -iMaskSize; iRowStart <= iMaskSize; iRowStart++)
    {
        for (int iColStart = -iMaskSize; iColStart <= iMaskSize; iColStart++)
        {
            iRowLoc = iRow + iRowStart;
            iColLoc = iCol + iColStart;
            if (iRowLoc >= 0 && iRowLoc < pImageT->height && iColLoc >= 0
                    && iColLoc < pImageT->width)
            {
                iN++;
                iPixLoc = iRowLoc * pImageT->widthStep + iColLoc;
                iValI = (int)(uchar)pImageI->imageData[iPixLoc];
                iValT = (int)(uchar)pImageT->imageData[iPixLoc];
                X += iValI;
                Y += iValT;
                XX += iValI * iValI;
                YY += iValT * iValT;
                XY += iValI * iValT;
            }
        }
    }
    float fMeanModel = (float)X / (float)iN;
    float fMeanProbe = (float)Y / (float)iN;
    float fCovarXY = (float)XY - fMeanModel * fMeanProbe * (float)iN;
    float fVarModel = (float)XX - fMeanModel * fMeanModel * (float)iN;
    float fVarProbe = (float)YY - fMeanProbe * fMeanProbe * (float)iN;
    float fStdDevModel = sqrt(fVarModel);
    float fStdDevProbe = sqrt(fVarProbe);

    // NCC = (1/(N-1)) * Sum[ ( (f(x,y) - f_mean) * (t(x,y) - t_mean) ) / ( std_dev(f) * std_dev(t) ) ]
    // NCC = Covar(X,Y) / sqrt( Var(X) * Var(Y) )
    // Covar(X,Y) = E(XY) - E(X)*E(Y)
    // Var(X) = Covar(X,X) = E(X^2) - (E(X))^2
    // std_dev(f) = sqrt( (1/(N-1)) * Sum[ (f(x,y) - f_mean)^2 ]
    // std_dev(X) = sqrt(Var(X))
    fNCC = fCovarXY / (fStdDevModel * fStdDevProbe);
    if (bDebug)
    {
        cout << "(" << iCol << "," << iRow << ") fCovarXY: " << fCovarXY
                << " fStdDevModel: " << fStdDevModel << " fStdDevProbe: "
                << fStdDevProbe << " fNCC = " << fNCC << endl;
    }

    if (iN == 0 || fCovarXY <= 0 || fStdDevModel == 0 || fStdDevProbe == 0)
    {
        fNCC = -1;
        return false;
    }

    // TODO: Check if it is necessary to discriminate shadow test
    bShadowTest = false;    // TESTING
    if (bShadowTest)
    {
        // Low variance region: Not reliable to use NCC because the NCC score can be high due to noise
        // If the variance of the template is low, it means it has low contrast, it has smooth region,
        // and we avoid using NCC for shadow detection on these regions.

        // If high NCC, we assume that the pixel in T matches with I, i.e. it is a background pixel
        // Since, it is a shadow test, i.e. earlier the pixel was considered as fg pixel, so it should be a shadow pixel

        // Low variance in T and similar to comparatively slightly higher variance in I ==> Probably shadow

        // TODO: The condition/constraint here needs to be updated as necessary!!!
        //if( fVarProbe / (float)iN < VARIANCE_THRESHOLD &&
        //    fVarModel / (float)iN >= VARIANCE_THRESHOLD &&
        //    fVarModel / (float)iN < VARIANCE_THRESHOLD * 5 )
        if (fVarProbe <= fVarModel) // && fVarModel / fVarProbe < 5 )
        {
            // Pixels that have probability of being shadow pixels
        }
        else
        {
            fNCC = -1;
            return false;
        }

        if (bDebug)
        {
            // Here, we compute only those template regions with low variance in both Probe and Model
            fNCC = fCovarXY / sqrt(fVarModel * fVarProbe);
            cout << "(" << iCol << "," << iRow << ") fCovarXY: " << fCovarXY
                    << " fVarModel: " << fVarModel << " fVarProbe: "
                    << fVarProbe << " fNCC: " << fNCC << endl;
            if (fNCC > 0.9)
            {
                IplImage *pImageDisplay = cvCloneImage(pImageT);
                cvCircle(pImageDisplay, cvPoint(iCol, iRow), 5,
                        cvScalar(255, 255, 255), 1);
                cvNamedWindow("Current point", 0);
                cvShowImage("Current point", pImageDisplay);
                cvWaitKey(1);
                cvReleaseImage(&pImageDisplay);
            }
        }
    }

    // == Additional constraints ==
    // TODO: Recheck on the threshold and also if the steps below are necessary:
    // Penalize the cases when the template and image have large difference in variance

    // If std_dev of image < 25 && std_dev of template > 125, then invalidate computed NCC as it is not reliable
    if ((fVarModel / (float)iN) < STD_DEV_THRESHOLD
            && (fVarProbe / (float)iN) > STD_DEV_THRESHOLD * 5)
    {
        // smooth image region, non-smooth template.
        fNCC = -1;
        return false;
    }

    // If std_dev of template < 25 && std_dev of image > 125, then invalidate computed NCC as it is not reliable
    if ((fVarProbe / (float)iN) < STD_DEV_THRESHOLD
            && (fVarModel / (float)iN) > STD_DEV_THRESHOLD * 5)
    {
        // smooth template, non-smooth image region.
        fNCC = -1;
        return false;
    }

    return true;
}

bool Utilities::getNCC(const cv::Mat &matTarget, const cv::Mat &matInput,
        cv::Point2f ptTargetRowCol, cv::Point2f ptInputRowCol, int iMaskSize,
        float &fNCC, bool bDebug)
{
    int iRowLocT = 0, iColLocT = 0;
    int iRowLocI = 0, iColLocI = 0;
    int iN = 0;
    int iPixLoc = 0;
    double X = 0, Y = 0, XX = 0, YY = 0, XY = 0;
    int iValT = 0;
    int iValI = 0;

    for (int iRowStart = -iMaskSize; iRowStart <= iMaskSize; iRowStart++)
    {
        for (int iColStart = -iMaskSize; iColStart <= iMaskSize; iColStart++)
        {
            iRowLocT = ptTargetRowCol.y + iRowStart;
            iColLocT = ptTargetRowCol.x + iColStart;
            iRowLocI = ptInputRowCol.y + iRowStart;
            iColLocI = ptInputRowCol.x + iColStart;
            bool bValidLocation = true;
            if (iRowLocT < 0 && iRowLocT > matTarget.rows && iColLocT < 0
                    && iColLocT > matTarget.cols)
            {
                bValidLocation = false;
            }
            if (iRowLocI < 0 && iRowLocI > matInput.rows && iColLocI < 0
                    && iColLocI > matInput.cols)
            {
                bValidLocation = false;
            }
            if (bValidLocation)
            {
                iN++;
                //iPixLoc = iRowLoc * pImageT->widthStep + iColLoc;
                iValT = (int)matTarget.at<uchar>(iRowLocT, iColLocT); //(int)(uchar)pImageT->imageData[iPixLoc];
                iValI = (int)matInput.at<uchar>(iRowLocI, iColLocI);
                X += iValI;
                Y += iValT;
                XX += iValI * iValI;
                YY += iValT * iValT;
                XY += iValI * iValT;
            }
        }
    }
    float fMeanModel = (float)X / (float)iN;
    float fMeanProbe = (float)Y / (float)iN;
    float fCovarXY = (float)XY - fMeanModel * fMeanProbe * (float)iN;
    float fVarModel = (float)XX - fMeanModel * fMeanModel * (float)iN;
    float fVarProbe = (float)YY - fMeanProbe * fMeanProbe * (float)iN;
    float fStdDevModel = sqrt(fVarModel);
    float fStdDevProbe = sqrt(fVarProbe);

    // NCC = (1/(N-1)) * Sum[ ( (f(x,y) - f_mean) * (t(x,y) - t_mean) ) / ( std_dev(f) * std_dev(t) ) ]
    // NCC = Covar(X,Y) / sqrt( Var(X) * Var(Y) )
    // Covar(X,Y) = E(XY) - E(X)*E(Y)
    // Var(X) = Covar(X,X) = E(X^2) - (E(X))^2
    // std_dev(f) = sqrt( (1/(N-1)) * Sum[ (f(x,y) - f_mean)^2 ]
    // std_dev(X) = sqrt(Var(X))
    fNCC = fCovarXY / (fStdDevModel * fStdDevProbe);
    if (bDebug)
    {
        cout << "Point target: (" << ptTargetRowCol.x << "," << ptTargetRowCol.y
                << ")  Point input: (" << ptInputRowCol.x << ","
                << ptInputRowCol.y << ")" << endl;
        cout << "fCovarXY: " << fCovarXY << " ,fStdDevModel: " << fStdDevModel
                << " ,fStdDevProbe: " << fStdDevProbe << " ,fNCC = " << fNCC
                << endl;
    }

    if (iN == 0 || fCovarXY <= 0 || fStdDevModel == 0 || fStdDevProbe == 0)
    {
        fNCC = -1;
        return false;
    }

    // == Additional constraints ==
    // TODO: Recheck on the threshold and also if the steps below are necessary:
    // Penalize the cases when the template and image have large difference in variance

    // If std_dev of image < 25 && std_dev of template > 125, then invalidate computed NCC as it is not reliable
    if ((fVarModel / (float)iN) < STD_DEV_THRESHOLD
            && (fVarProbe / (float)iN) > STD_DEV_THRESHOLD * 5)
    {
        // smooth image region, non-smooth template.
        fNCC = -1;
        return false;
    }

    // If std_dev of template < 25 && std_dev of image > 125, then invalidate computed NCC as it is not reliable
    if ((fVarProbe / (float)iN) < STD_DEV_THRESHOLD
            && (fVarModel / (float)iN) > STD_DEV_THRESHOLD * 5)
    {
        // smooth template, non-smooth image region.
        fNCC = -1;
        return false;
    }
    return true;
}

// NCC = (1/(N-1)) * Sum[ ( (f(x,y) - f_mean) * (t(x,y) - t_mean) ) / ( std_dev(f) * std_dev(t) ) ]
// NCC = Covar(X,Y) / sqrt( Var(X) * Var(Y) )
// Covar(X,Y) = E(XY) - E(X)*E(Y)
// Var(X) = Covar(X,X) = E(X^2) - (E(X))^2
// E(X) = sum(xi * pi) where pi is the probability of xi
// std_dev(f) = sqrt( (1/(N-1)) * Sum[ (f(x,y) - f_mean)^2 ]
// std_dev(X) = sqrt(Var(X))
bool Utilities::getNCC(const IplImage *pImageT, const IplImage *pImageI,
        float &fNCC, bool bDebug)
{
    checkImages(pImageT, pImageI, "getNCC");
    bool bValidComputation = true;

    int iN = 0;
    int iPixLoc = 0;
    double X = 0, Y = 0, XX = 0, YY = 0, XY = 0;
    int iValT = 0;
    int iValI = 0;

    for (int iRow = 0; iRow <= pImageT->height; iRow++)
    {
        for (int iCol = 0; iCol <= pImageT->height; iCol++)
        {
            iN++;
            iPixLoc = iRow * pImageT->widthStep + iCol;
            iValI = (int)(uchar)pImageI->imageData[iPixLoc];
            iValT = (int)(uchar)pImageT->imageData[iPixLoc];
            X += iValI;
            Y += iValT;
            XX += iValI * iValI;
            YY += iValT * iValT;
            XY += iValI * iValT;
        }
    }
    float fMeanModel = (float)X / (float)iN;
    float fMeanProbe = (float)Y / (float)iN;
    float fCovarXY = (float)XY - fMeanModel * fMeanProbe * (float)iN;
    float fVarModel = (float)XX - fMeanModel * fMeanModel * (float)iN;
    float fVarProbe = (float)YY - fMeanProbe * fMeanProbe * (float)iN;
    fNCC = fCovarXY / sqrt(fVarModel * fVarProbe);
    if (iN == 0 || fCovarXY <= 0 || fVarModel == 0 || fVarProbe == 0)
    {
        fNCC = -1;
        bValidComputation = false;
    }
    return bValidComputation;
}

void Utilities::getCenterOfGravity(CvSeq *pContour, bool &bValidCG,
        CvPoint2D32f &ptCG, bool bDebug)
{
    CvMoments moments;
    double dM00, dM01, dM10;
    cvMoments(pContour, &moments, 1);
    dM00 = cvGetSpatialMoment(&moments, 0, 0);
    dM10 = cvGetSpatialMoment(&moments, 1, 0);
    dM01 = cvGetSpatialMoment(&moments, 0, 1);
    ptCG.x = -1;
    ptCG.y = -1;
    bValidCG = false;
    if (dM00 > 0)
    {
        bValidCG = true;
        ptCG.x = (float)(dM10 / dM00);
        ptCG.y = (float)(dM01 / dM00);
    }
}

void Utilities::computeMagAndAngleBetweenRGBVectors(int B1, int G1, int R1,
        int B2, int G2, int R2, float &fMag1, float &fMag2, float &fThetaRad,
        bool bDebug)
{
    fMag1 = sqrt(B1 * B1 + G1 * G1 + R1 * R1);
    fMag2 = sqrt(B2 * B2 + G2 * G2 + R2 * R2);
    float fAdotB = B1 * B2 + G1 * G2 + R1 * R2;
    fThetaRad = acos(fAdotB / (fMag1 * fMag2));
}

int Utilities::getNumExternalContours(const IplImage *pImage, bool bDebug)
{
    int iNumContours = 0;
    IplImage *pImageContours = cvCloneImage(pImage);
    CvContourScanner contourScanner = cvStartFindContours(pImageContours,
            _pMemStorage, sizeof(CvContour), CV_RETR_EXTERNAL,
            CV_CHAIN_APPROX_SIMPLE);
    CvSeq *pContour = NULL;

    while ((pContour = cvFindNextContour(contourScanner)) != NULL)
    {
        iNumContours++;
    }
    cvEndFindContours(&contourScanner);
    cvClearMemStorage(_pMemStorage);
    cvReleaseImage(&pImageContours);
    return iNumContours;
}

int Utilities::getNumContours(CvSeq *pSeqContours, bool bDebug)
{
    int iNumContours = 0;
    if (pSeqContours)
    {
        // TODO: Need to include v_next too
        for (CvSeq *pCurContour = pSeqContours; pCurContour != NULL;
                pCurContour = pCurContour->h_next)
        {
            iNumContours++;
        }
    }
    return iNumContours;
}

CvSeq* Utilities::getAlphaShape_ConvexHull(CvSize sizeImage,
        vector<CvPoint> vPts, int &iContourFound, bool bDebug)
{
    IplImage *pImageAlpha = cvCreateImage(sizeImage, IPL_DEPTH_8U, 1);
    cvSetZero(pImageAlpha);

    if (bDebug)
    {
        for (int iPt = 0; iPt < (int)vPts.size(); iPt++)
        {
            cvCircle(pImageAlpha, vPts[iPt], 1, cvScalar(255, 255, 255), -1);
        }
    }
    for (int iPt = 0; iPt < (int)vPts.size() - 1; iPt++)
    {
        for (int iPt2 = iPt + 1; iPt2 < (int)vPts.size(); iPt2++)
        {
            cvLine(pImageAlpha, vPts[iPt], vPts[iPt2], cvScalar(255, 255, 255),
                    1);
        }
    }
    CvSeq *pSeqContours;
    if (bDebug)
    {
        cvNamedWindow("pImageAlpha", 0);
        cvShowImage("pImageAlpha", pImageAlpha);
    }
// cvFindContours() updates the contents of pImageAlpha
    iContourFound = cvFindContours(pImageAlpha, _pMemStorage, &pSeqContours,
            sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    if (pImageAlpha) cvReleaseImage(&pImageAlpha);
    return pSeqContours;
}

// TODO: Describe the values of iChannel, e.g. what does it mean if it is 0, 1, 2 and 3
void Utilities::overlayImage(const IplImage *pImageOverlay,
        IplImage *pImageBaseC, int iChannel, bool bSingleChannel)
{
    if (!(pImageOverlay && pImageBaseC))
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_IMAGE_IS_NULL);
    }
    if (!(pImageOverlay->nChannels == 1 && pImageBaseC->nChannels == 3))
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_CHANNEL);
    }
    if (!(iChannel >= 0 && iChannel <= 3))
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_NUM_OF_CHANNELS);
    }

    int iChannel1 = 0, iChannel2 = 0, iChannel3 = 0;
    if (iChannel == 0)
    {
        iChannel1 = 0;
        iChannel2 = 1;
        iChannel3 = 2;
    }
    else if (iChannel == 1)
    {
        iChannel1 = 1;
        iChannel2 = 0;
        iChannel3 = 2;
    }
    else if (iChannel == 2)
    {
        iChannel1 = 2;
        iChannel2 = 0;
        iChannel3 = 1;
    }
    else if (iChannel == 3)
    {
        iChannel1 = 0;
        iChannel2 = 1;
        iChannel3 = 2;
    }

    unsigned char *pchBaseData = (unsigned char*)pImageBaseC->imageData;
    int iWidthStep = pImageBaseC->widthStep / sizeof(uchar);
    int iPixVal = 0;
    int iImgLocMultiplier = 0;
    for (int iRow = 0; iRow < pImageOverlay->height; iRow++)
    {
        for (int iCol = 0; iCol < pImageOverlay->width; iCol++)
        {
            iPixVal = (int)(uchar)pImageOverlay->imageData[iRow
                    * pImageOverlay->widthStep + iCol];
            if (iPixVal > 0)
            {
                iImgLocMultiplier = iRow * iWidthStep
                        + iCol * pImageBaseC->nChannels;
                pchBaseData[iImgLocMultiplier + iChannel1] = (uchar)iPixVal;
                if (iChannel == 3)
                {
                    pchBaseData[iImgLocMultiplier + iChannel2] = (uchar)iPixVal;
                    pchBaseData[iImgLocMultiplier + iChannel3] = (uchar)iPixVal;
                }
                else
                {
                    if (!bSingleChannel)
                    {
                        pchBaseData[iImgLocMultiplier + iChannel2] = (uchar)0;
                        pchBaseData[iImgLocMultiplier + iChannel3] = (uchar)0;
                    }
                }
            }
        }
    }
}

void Utilities::overlayImage(const IplImage *pImageOverlay,
        IplImage *pImageBaseC, CvScalar color)
{
    if (!(pImageOverlay && pImageBaseC))
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_IMAGE_IS_NULL);
    }
    if (!(pImageOverlay->nChannels == 1 && pImageBaseC->nChannels == 3))
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_CHANNEL);
    }
    unsigned char *pchBaseData = (unsigned char*)pImageBaseC->imageData;
    int iWidthStep = pImageBaseC->widthStep / sizeof(uchar);
    int iPixVal = 0;
    int iImgLocMultiplier = 0;
    for (int iRow = 0; iRow < pImageOverlay->height; iRow++)
    {
        for (int iCol = 0; iCol < pImageOverlay->width; iCol++)
        {
            iPixVal = (int)(uchar)pImageOverlay->imageData[iRow
                    * pImageOverlay->widthStep + iCol];
            if (iPixVal > 0)
            {
                iImgLocMultiplier = iRow * iWidthStep
                        + iCol * pImageBaseC->nChannels;
                pchBaseData[iImgLocMultiplier + 0] = (uchar)color.val[0];
                pchBaseData[iImgLocMultiplier + 1] = (uchar)color.val[1];
                pchBaseData[iImgLocMultiplier + 2] = (uchar)color.val[2];
            }
        }
    }
}

void Utilities::overlayImageOnGray(const IplImage *pImageOverlay,
        IplImage *pImageBaseGray, CvScalar color)
{
    if (!(pImageOverlay && pImageBaseGray))
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_IMAGE_IS_NULL);
    }
    if (!(pImageOverlay->nChannels == 1 && pImageBaseGray->nChannels == 1))
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_CHANNEL);
    }
    unsigned char *pchBaseData = (unsigned char*)pImageBaseGray->imageData;
    int iWidthStep = pImageBaseGray->widthStep / sizeof(uchar);
    int iPixVal = 0;
    int iImgLocMultiplier = 0;
    for (int iRow = 0; iRow < pImageOverlay->height; iRow++)
    {
        for (int iCol = 0; iCol < pImageOverlay->width; iCol++)
        {
            iImgLocMultiplier = iRow * iWidthStep + iCol;
            iPixVal = (int)(uchar)pImageOverlay->imageData[iImgLocMultiplier];
            if (iPixVal > 0)
            {
                pchBaseData[iImgLocMultiplier] = (uchar)color.val[0];
            }
        }
    }
}

void Utilities::overlayContour(const IplImage *pImageOverlayBinary,
        IplImage *pImageBaseC, CvScalar color)
{
    if (!_pImageContoursTmp)
    {
        _pImageContoursTmp = cvCreateImage(cvGetSize(pImageOverlayBinary),
                IPL_DEPTH_8U, 1);
    }
    else if (!((_pImageContoursTmp->width == pImageOverlayBinary->width)
            && (_pImageContoursTmp->height == pImageOverlayBinary->height)
            && (_pImageContoursTmp->nChannels == pImageOverlayBinary->nChannels)))
    {
        cvReleaseImage(&_pImageContoursTmp);
        _pImageContoursTmp = cvCreateImage(cvGetSize(pImageOverlayBinary),
                IPL_DEPTH_8U, 1);
    }
    cvCopyImage(pImageOverlayBinary, _pImageContoursTmp);
    cvFindContours(_pImageContoursTmp, _pMemStorage, &_pSeqContours,
            sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,
            cvPoint(0, 0));
    cvDrawContours(pImageBaseC, _pSeqContours, color, color, 2, 2);
    if (_pMemStorage) cvClearMemStorage(_pMemStorage);
}

void Utilities::drawLines(IplImage *pImage, vector<CvPoint> vPts)
{
    for (int iPt = 0; iPt < (int)vPts.size() - 1; iPt++)
    {
        cvLine(pImage, vPts[iPt], vPts[iPt + 1], cvScalar(0, 0, 255), 2);
    }
}

void Utilities::drawRectangle(IplImage *pImage, cv::Rect rect, CvScalar color,
        int iThickness)
{
    cvRectangle(pImage, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width,
            rect.y + rect.height), color, iThickness);
}

void Utilities::convertContourInSeqToPts(CvSeq *pContour,
        vector<CvPoint*> &vPts)
{
    for (int iPt = 0; iPt < pContour->total; iPt++)
    {
        CvPoint *pt = (CvPoint*)cvGetSeqElem(pContour, iPt - 1);
        vPts.push_back(pt);
    }
}

void Utilities::writeTextOnImage(IplImage *pImage, string sText, int iPos)
{
    double dWidthRatio = pImage->width / _iImageWidth;
    double dHeightRatio = pImage->height / _iImageHeight;
    double dHScale = _dHScale;
    double dVScale = _dVScale;
    int iVScaleRatio = 1;
    int iLineWidth = _iLineWidth;
    int iNumRowsForText = _iNumRowsForText;
    int iTextLocY = 8;
    int iTextLocX = 2;
    if (iPos == 1)
    {
        iTextLocY = pImage->height - 4;
    }

    if (dWidthRatio > 1 || dHeightRatio > 1)
    {
        dHScale *= dWidthRatio;
        dVScale *= dHeightRatio;
        iLineWidth *= cvCeil(0.5 * dWidthRatio);
        iVScaleRatio = cvCeil(dVScale / _dVScale);
        iNumRowsForText *= iVScaleRatio;
        iTextLocY = (_iNumRowsForText - 3) * iVScaleRatio;
        if (iPos == 1)
        {
            iTextLocY = pImage->height - 4 * iVScaleRatio;
        }
    }
    cvInitFont(&_font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, dHScale,
            dVScale, 0, iLineWidth, 8);
    CvRect rectROI = cvRect(0, 0, pImage->width, iNumRowsForText);
    int iRectPosY = pImage->height - iNumRowsForText;
    if (iPos == 1)
    {
        rectROI = cvRect(0, iRectPosY, pImage->width, iNumRowsForText);
    }
    cvSetImageROI(pImage, rectROI);
    cvSet(pImage, cvScalar(0, 0, 0));
    cvResetImageROI(pImage);

    if (iPos == 0)
    {
        cvPutText(pImage, sText.c_str(), cvPoint(iTextLocX, iTextLocY), &_font,
                cvScalar(255, 255, 255));   // White text
    }
    else if (iPos == 1)
    {
        cvPutText(pImage, sText.c_str(), cvPoint(iTextLocX, iTextLocY), &_font,
                cvScalar(0, 0, 255));   // Red text
    }
    else
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_LOCATION_ON_IMAGE);
    }
}

void Utilities::writeTextOnImage(IplImage *pImage, CvPoint pt, string sText)
{
    cvPutText(pImage, sText.c_str(), pt, &_font2, cvScalar(0, 0, 255));
}

void Utilities::getImageMask(const IplImage *pImage, IplImage *pImageMask,
        int iPixelVal, bool bDebug)
{
    checkImages(pImage, pImageMask, string("getImageMask"));
    cvSetZero(pImageMask);
    int iPixVal = 0;
    int iPixLoc = 0;
    for (int iRow = 0; iRow < pImage->height; iRow++)
    {
        for (int iCol = 0; iCol < pImage->width; iCol++)
        {
            iPixLoc = iRow * pImage->widthStep + iCol;
            iPixVal = (int)(uchar)pImage->imageData[iPixLoc];
            if (iPixVal == iPixelVal)
            {
                pImageMask->imageData[iPixLoc] = (uchar)255;
            }
        }
    }
}

void Utilities::getImageMask(const IplImage *pImageC, IplImage *pImageMask,
        int iChannel, int iPixelVal, bool bDebug)
{
    cvSetZero(pImageMask);
    int iPixLocC = 0;
    uchar *puchData = (uchar*)pImageC->imageData;
    int iWidthStep = pImageC->widthStep / sizeof(uchar);
    int iNumChannels = pImageC->nChannels;
    bool bActivatePixel = false;
    for (int iRow = 0; iRow < pImageC->height; iRow++)
    {
        for (int iCol = 0; iCol < pImageC->width; iCol++)
        {
            bActivatePixel = false;
            iPixLocC = iRow * iWidthStep + iCol * iNumChannels;
            if (iChannel == 0) // Blue
            {
                if ((int)puchData[iPixLocC + 0] == iPixelVal
                        && (int)puchData[iPixLocC + 1] == 0
                        && (int)puchData[iPixLocC + 2] == 0)
                {
                    bActivatePixel = true;
                }
            }
            else if (iChannel == 1) // Green
            {
                if ((int)puchData[iPixLocC + 0] == 0
                        && (int)puchData[iPixLocC + 1] == iPixelVal
                        && (int)puchData[iPixLocC + 2] == 0)
                {
                    bActivatePixel = true;
                }
            }
            else if (iChannel == 2) // Red
            {
                if ((int)puchData[iPixLocC + 0] == 0
                        && (int)puchData[iPixLocC + 1] == 0
                        && (int)puchData[iPixLocC + 2] == iPixelVal)
                {
                    bActivatePixel = true;
                }
            }
            if (bActivatePixel)
            {
                pImageMask->imageData[iRow * pImageMask->widthStep + iCol] =
                        (uchar)255;
            }
        }
    }
}

void Utilities::removeShadow(IplImage *pImageFg, const IplImage *pImageShadow,
        bool bDebug)
{
// TODO: Check if it is the same. Should be same.
#if 0
    cvSub( pImageFg, pImageShadow, pImageFg, pImageShadow );
#else // Simply using cvSub()
    checkImages(pImageFg, pImageShadow, string("removeShadow"));
    int iPixVal = 0;
    int iPixLoc = 0;
    for (int iRow = 0; iRow < pImageShadow->height; iRow++)
    {
        for (int iCol = 0; iCol < pImageShadow->width; iCol++)
        {
            iPixLoc = iRow * pImageShadow->widthStep + iCol;
            iPixVal = (int)(unsigned char)pImageShadow->imageData[iPixLoc];
            if (iPixVal > 0)
            {
                // Disable the pixel in the foreground image
                pImageFg->imageData[iPixLoc] = (unsigned char)0;
            }
        }
    }
#endif
}

void Utilities::analyzeDetectionUsingGroundTruth(const IplImage *pImageGT,
        int &iNumPixGT, const IplImage *pImageD, int &iTP, int &iTN, int &iFP,
        int &iFN, IplImage *pImageOverlayC, bool bDebug)
{
    checkImages(pImageGT, pImageD, string("getDiffFromGroundTruth"));
    iNumPixGT = 0;
    iTP = 0;
    iTN = 0;
    iFP = 0;
    iFN = 0;

    int iPixValGT = 0;
    int iPixValD = 0;
    int iPixLoc = 0;
    for (int iRow = 0; iRow < pImageGT->height; iRow++)
    {
        for (int iCol = 0; iCol < pImageGT->width; iCol++)
        {
            iPixLoc = iRow * pImageGT->widthStep + iCol;
            iPixValGT = (int)(uchar)pImageGT->imageData[iPixLoc];
            iPixValD = (int)(uchar)pImageD->imageData[iPixLoc];
            if (iPixValGT > 0)
            {
                iNumPixGT++;
                if (iPixValD > 0)
                {
                    iTP++; // true positive is good
                }
                else
                {
                    iFN++;
                }
            }
            else
            {
                if (iPixValD > 0)
                {
                    iFP++;
                }
                else
                {
                    iTN++;  // true negative is good
                }
            }
        }
    }

// Update image with overlay
    cvSetZero(pImageOverlayC);
    overlayImage(pImageD, pImageOverlayC, 1, true);
    overlayImage(pImageGT, pImageOverlayC, 2, true);
}

int Utilities::getNumActivePixels(const IplImage *pImage)
{
    int iNumActivePixels = 0;
    if (pImage->depth == IPL_DEPTH_8U)
    {
        if (pImage->nChannels == 1)
        {
            uchar *pchData = (uchar*)pImage->imageData;
            int iWidthStep = pImage->widthStep / sizeof(uchar);
            for (int iRow = 0; iRow < pImage->height; iRow++)
            {
                for (int iCol = 0; iCol < pImage->width; iCol++)
                {
                    if ((int)pchData[iRow * iWidthStep + iCol] > 0) iNumActivePixels++;
                }
            }
        }
        else
        {
            _pErrorHandler->setErrorCode(
                    (int)UTILITIES_INVALID_NUM_OF_CHANNELS);
        }
    }
    else
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_IMAGE_DEPTH);
    }
    return iNumActivePixels;
}

void Utilities::watermarkImage(cv::Mat &img, cv::Mat &wm, float alpha,
        cv::Mat &dst, bool bDebug)
{
    if (!img.data)
    {
        cout << "Unable to load source image." << endl;
        return;
    }
    if (!wm.data)
    {
        cout << "Unable to load watermark image." << endl;
        return;
    }
    float beta = 1 - alpha;
    if (img.rows != wm.rows || img.cols != wm.cols)
    {
        // NOTE: Resize wm to img before invoking this function to avoid
        // computing the resized image every time
        cv::Mat resized(img.size(), img.type());
        resize(wm, resized, resized.size(), 0, 0, cv::INTER_CUBIC);
        addWeighted(img, alpha, resized, beta, 0.0, dst);
        resized.release();
    }
    else
    {
        addWeighted(img, alpha, wm, beta, 0.0, dst);
    }
}

void Utilities::blurWatermarkImage(cv::Mat &wm, bool bDebug)
{
    cv::GaussianBlur(wm, wm, cvSize(3, 3), 7, 7);
}

void Utilities::displayMat(cv::Mat mat, bool bDebug)
{
    cout << "Matrix:\n" << mat << endl;
}

cv::Mat Utilities::rotateImage(cv::Mat src, float angleDegrees, float scale,
        bool bDebug)
{
    int w = src.cols;
    int h = src.rows;
    CvSize sizeRotated;
    float angleRadians = angleDegrees * CV_PI / 180;
    sizeRotated.width = cvFloor(
            fabs(h * sin(angleRadians)) + fabs(w * cos(angleRadians)) + 0.5);
    sizeRotated.height = cvFloor(
            fabs(h * cos(angleRadians)) + fabs(w * sin(angleRadians)) + 0.5);
    cv::Mat rotated = cv::Mat::zeros(sizeRotated, src.type());
//Point2f center = Point2f(rotated.cols, rotated.rows / 2);
    cv::Point2f center = cv::Point2f(0, 0);
    cv::Mat trf = getRotationMatrix2D(center, angleDegrees, scale);
// In order to find the translation for all angles, need to obtain the
// displacement between the center of the final image and center of the rotated block.
    cv::Mat ptCenterOfFinalImage(2, 1, CV_32F);
    ptCenterOfFinalImage.at<float>(0, 0) = rotated.cols / 2;
    ptCenterOfFinalImage.at<float>(1, 0) = rotated.rows / 2;
    cv::Mat ptCenterOfOrigBlock(2, 1, CV_32F);
    ptCenterOfOrigBlock.at<float>(0, 0) = src.cols / 2;
    ptCenterOfOrigBlock.at<float>(1, 0) = src.rows / 2;
    cv::Mat ptCenterOfRotBlock(2, 1, CV_32F);
    ptCenterOfRotBlock.at<float>(0, 0) = trf.at<double>(0, 0)
            * ptCenterOfOrigBlock.at<float>(0, 0)
            + trf.at<double>(0, 1) * ptCenterOfOrigBlock.at<float>(1, 0);
    ptCenterOfRotBlock.at<float>(1, 0) = trf.at<double>(1, 0)
            * ptCenterOfOrigBlock.at<float>(0, 0)
            + trf.at<double>(1, 1) * ptCenterOfOrigBlock.at<float>(1, 0);
    cv::Mat translation(2, 1, CV_64F);
    translation.at<double>(0, 0) = (double)ptCenterOfFinalImage.at<float>(0, 0)
            - ptCenterOfRotBlock.at<float>(0, 0);
    translation.at<double>(1, 0) = (double)ptCenterOfFinalImage.at<float>(1, 0)
            - ptCenterOfRotBlock.at<float>(1, 0);
    trf.at<double>(0, 2) = translation.at<double>(0, 0);
    trf.at<double>(1, 2) = translation.at<double>(1, 0);
    if (bDebug) displayMat(trf);
    warpAffine(src, rotated, trf, rotated.size());
    if (bDebug)
    {
        cv::namedWindow("rotated", 0);
        imshow("rotated", rotated);
    }
    ptCenterOfOrigBlock.release();
    ptCenterOfRotBlock.release();
    ptCenterOfFinalImage.release();
    translation.release();
    trf.release();
    return rotated;
}

cv::Mat Utilities::rotateImage(cv::Mat src, float angleDegrees, bool bDebug)
{
// Create a map_matrix, where the left 2x2 matrix
// is the transform and the right 2x1 is the origin for rotation.
    float m[6];
    CvMat M = cvMat(2, 3, CV_32F, m);
    int w = src.cols;
    int h = src.rows;
    float angleRadians = angleDegrees * ((float)CV_PI / 180.0f);
    m[0] = (float)(cos(angleRadians));
    m[1] = (float)(sin(angleRadians));
    m[3] = -m[1];
    m[4] = m[0];
    m[2] = w * 0.5f;
    m[5] = h * 0.5f;
// Make a spare image for the result
    CvSize sizeRotated;
    sizeRotated.width = cvFloor(
            fabs(h * sin(angleRadians)) + fabs(w * cos(angleRadians)) + 0.5);
    sizeRotated.height = cvFloor(
            fabs(h * cos(angleRadians)) + fabs(w * sin(angleRadians)) + 0.5);
    cv::Mat rotated = cv::Mat::zeros(sizeRotated, src.type());
    IplImage imgSrc = src;
    IplImage imgRotated = rotated;
// Seems like cvGetQuadrangleSubPix needs IplImage* and does not take &mat. Check!
    cvGetQuadrangleSubPix(&imgSrc, &imgRotated, &M);
    if (bDebug)
    {
        cv::namedWindow("rotated", 0);
        cv::imshow("rotated", rotated);
    }
    return rotated;
}

cv::Mat Utilities::getImageWithWatermarkText(string s, int iNumLines,
        float fAngleDegrees, float fScale, int iImageWidth, int iImageHeight,
        bool bDebug)
{
    if (s.length() > 30)
    {
        cout << "Currently, string of max length 20 is used to "
                << "create the watermark image with text." << endl;
    }
    int iWidthRatio = iImageWidth / 320;
    string sText = s;
    if (iWidthRatio > 1)
    {
        for (int i = 1; i < iWidthRatio; i++)
        {
            sText += s;
        }
    }
    double fontScale = 2;
    int iVerticalDistance = fontScale * 25;
    int iMatWidth = sText.length() * fontScale * 20;
    int iMatHeight = iVerticalDistance * (iNumLines + 1);
    cv::Mat wm = wm.zeros(cvSize(iMatWidth, iMatHeight), CV_8UC3);
//int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
    int fontFace = cv::FONT_HERSHEY_DUPLEX;
    int thickness = 1;
    for (int i = 0; i < iNumLines; i++)
    {
        putText(wm, sText,
                cvPoint(10, iVerticalDistance + i * iVerticalDistance),
                fontFace, fontScale, cvScalar(0, 0, 255), thickness, 8);
    }
    cv::Mat wm_rotated;
#if 0   // using cvQuadrangleSubPix
    wm_rotated = rotateImage(wm, -1*fAngleDegrees, bDebug);
#else   // using warpAffine
    wm_rotated = rotateImage(wm, fAngleDegrees, fScale, bDebug); //+ve values = counter-clockwise
#endif
    if (bDebug)
    {
        cv::namedWindow("wm", 0);
        imshow("wm", wm);
        cv::namedWindow("wm_rotated", 0);
        imshow("wm_rotated", wm_rotated);
    }
    wm.release();

// Resize the matrix to specified output size
    cv::Mat resized(cvSize(iImageWidth, iImageHeight), wm_rotated.type());
    resize(wm_rotated, resized, resized.size(), 0, 0, cv::INTER_CUBIC);
    wm_rotated.release();
    if (bDebug)
    {
        _sText = "watermark_";
        _ssOut.str("");
        _ssOut << iImageWidth;
        _sText += _ssOut.str() + "_";
        _ssOut.str("");
        _ssOut << iImageHeight;
        _sText += _ssOut.str() + ".png";
        IplImage *pImage = new IplImage(resized);
        // TODO: Save somewhere else, maybe in the output folder
        cvSaveImage(_sText.c_str(), pImage);
        pImage = NULL;
    }
    return resized;
}

void Utilities::ensureRectIsInsideImage(const IplImage *pImage, cv::Rect &rect)
{
    if (rect.x < 0) rect.x = 0;
    if (rect.x > pImage->width - 1) rect.x = pImage->width - 1;
    if (rect.y < 0) rect.y = 0;
    if (rect.y > pImage->height - 1) rect.y = pImage->height - 1;
    if (rect.x + rect.width > pImage->width - 1) rect.width = pImage->width
            - rect.x - 1;
    if (rect.y + rect.height > pImage->height - 1) rect.height = pImage->height
            - rect.y - 1;
}

IplImage* Utilities::getRotatedImageSubPixWithTranslation(IplImage *pImage,
        double dAngleRad, double dScale, float xProbeMeanToImgCenter,
        float yProbeMeanToImgCenter, float xTemplMeanToImgCenter,
        float yTemplMeanToImgCenter, bool bDebug)
{
    IplImage *pImageTranslated = cvCloneImage(pImage);
    IplImage *pImageTransRot = cvCloneImage(pImage);
    float fArrM[6];

    CvMat M = cvMat(2, 3, CV_32F, fArrM);
    int iWidth = pImage->width;
    int iHeight = pImage->height;

    fArrM[0] = (float)(cos(-dAngleRad));
    fArrM[1] = (float)(sin(-dAngleRad));
    fArrM[2] = (float)iWidth * 0.5f - 0.5 + xProbeMeanToImgCenter;
    fArrM[3] = -fArrM[1];
    fArrM[4] = fArrM[0];
    fArrM[5] = (float)iHeight * 0.5f - 0.5 + yProbeMeanToImgCenter;

    cvGetQuadrangleSubPix(pImage, pImageTranslated, &M);

    fArrM[0] = (float)(cos(0));
    fArrM[1] = (float)(sin(0));
    fArrM[2] = (float)iWidth * 0.5f - 0.5 - xTemplMeanToImgCenter;
    fArrM[3] = -fArrM[1];
    fArrM[4] = fArrM[0];
    fArrM[5] = (float)iHeight * 0.5f - 0.5 - yTemplMeanToImgCenter;

    cvGetQuadrangleSubPix(pImageTranslated, pImageTransRot, &M);

    if (bDebug)
    {
        cvNamedWindow("pImagee", 1);
        cvShowImage("pImagee", pImage);
        cvNamedWindow("pImageTrans", 1);
        cvShowImage("pImageTrans", pImageTransRot);
        cvWaitKey(0);
    }

    return pImageTransRot;
}
/*
 This function calculates scale, rotation, and translation between probe and template.
 The implementation follows "Least-Squares Estimation of Transformation Parameters Between Two Point Patterns"
 Umeyama's method.

 Assume we have perfect point correspondences.

 @param pMatTempl is the matrix contains point correspondences on template image.
 @param pMatProbe is the matrix contains point correspondences on probe image.
 @param fScale is the scale between template and probe image.
 @param fRotAngle is the angle between template and probe image.
 @param fTransX is translation distance in X-axis between template and probe image.
 @param fTransY is translation distance in Y-axis between template and probe image.
 */
void Utilities::getUmeyamaTransform(CvMat *pMatTempl, CvMat *pMatProbe,
        float &fScale, float &fRotAngle, float &fTransX, float &fTransY,
        bool bDebug)
{
    CvMat *pMatCovar = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatModelVar = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatProbeVar = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatD = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatU = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatV = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatS = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatR = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatUS = cvCreateMat(2, 2, CV_32FC1);
    CvMat *pMatDS = cvCreateMat(2, 2, CV_32FC1);

// Calculate mean model and probe points
    double xProbeMean = 0, yProbeMean = 0, xTemplMean = 0, yTemplMean = 0;
    int N = 0;

    for (int i = 0; i < pMatTempl->cols; i++)
    {
        double xProbe = cvmGet(pMatProbe, 0, i);
        double yProbe = cvmGet(pMatProbe, 1, i);
        double xTempl = cvmGet(pMatTempl, 0, i);
        double yTempl = cvmGet(pMatTempl, 1, i);
        xProbeMean += xProbe;
        yProbeMean += yProbe;
        xTemplMean += xTempl;
        yTemplMean += yTempl;
        N++;
    }
    if (N == 0)
    {
        //_pErrorHandler->setErrorCode( (int)MODEL_INCONSISTENT_INLIER_VECTOR );
    }
    xProbeMean /= N;
    yProbeMean /= N;
    xTemplMean /= N;
    yTemplMean /= N;

// Calculate probe variance, model variance, and probe-model covariance

    float fRMSDProbe = 0.0;
    float fRMSDModel = 0.0;
    cvZero(pMatCovar);
    cvZero(pMatModelVar);
    cvZero(pMatProbeVar);
    float fDistProbeMax = 0.0;
    for (int i = 0; i < pMatTempl->cols; i++)
    {
        float xProbe = cvmGet(pMatProbe, 0, i) - xProbeMean;
        float yProbe = cvmGet(pMatProbe, 1, i) - yProbeMean;
        float xTempl = cvmGet(pMatTempl, 0, i) - xTemplMean;
        float yTempl = cvmGet(pMatTempl, 1, i) - yTemplMean;
        float fDist = (float)(xProbe * xProbe + yProbe * yProbe);
        fRMSDProbe += fDist;
        if (fDist > fDistProbeMax) fDistProbeMax = fDist;
        fRMSDModel += (float)(xTempl * xTempl + yTempl * yTempl);

        cvmSet(pMatCovar, 0, 0, cvmGet(pMatCovar, 0, 0) + xTempl * xProbe);
        cvmSet(pMatCovar, 0, 1, cvmGet(pMatCovar, 0, 1) + xTempl * yProbe);
        cvmSet(pMatCovar, 1, 0, cvmGet(pMatCovar, 1, 0) + yTempl * xProbe);
        cvmSet(pMatCovar, 1, 1, cvmGet(pMatCovar, 1, 1) + yTempl * yProbe);

        cvmSet(pMatModelVar, 0, 0,
                cvmGet(pMatModelVar, 0, 0) + xTempl * xTempl);
        cvmSet(pMatModelVar, 0, 1,
                cvmGet(pMatModelVar, 0, 1) + xTempl * yTempl);
        cvmSet(pMatModelVar, 1, 0,
                cvmGet(pMatModelVar, 1, 0) + yTempl * xTempl);
        cvmSet(pMatModelVar, 1, 1,
                cvmGet(pMatModelVar, 1, 1) + yTempl * yTempl);

        cvmSet(pMatProbeVar, 0, 0,
                cvmGet(pMatProbeVar, 0, 0) + xProbe * xProbe);
        cvmSet(pMatProbeVar, 0, 1,
                cvmGet(pMatProbeVar, 0, 1) + xProbe * yProbe);
        cvmSet(pMatProbeVar, 1, 0,
                cvmGet(pMatProbeVar, 1, 0) + yProbe * xProbe);
        cvmSet(pMatProbeVar, 1, 1,
                cvmGet(pMatProbeVar, 1, 1) + yProbe * yProbe);
    }
    cvScale(pMatCovar, pMatCovar, 1.0 / N);
    cvScale(pMatModelVar, pMatModelVar, 1.0 / N);
    cvScale(pMatProbeVar, pMatProbeVar, 1.0 / N);
    fRMSDProbe /= N;
    fRMSDModel /= N;

    fRMSDProbe = sqrt(fRMSDProbe);
    fRMSDModel = sqrt(fRMSDModel);

    // Get rotation matrix
    cvSVD(pMatCovar, pMatD, pMatU, pMatV, CV_SVD_V_T);
    cvSetIdentity(pMatS);
    float fDet = (float)(cvDet(pMatCovar));
    if (fDet < 0) cvmSet(pMatS, 1, 1, -1);
    cvMatMul(pMatU, pMatS, pMatUS);
    cvMatMul(pMatUS, pMatV, pMatR);
    cvMatMul(pMatD, pMatS, pMatDS);
    CvScalar trace = cvTrace(pMatDS);

    if (trace.val[0] == 0)
    {
        //_pErrorHandler->setErrorCode( (int)MODEL_BAD_MATCH_SET );
    }

    float angleRot = atan2(-cvmGet(pMatR, 0, 1), cvmGet(pMatR, 0, 0));
    float scale = 1.0 / fRMSDProbe * fRMSDModel;
    float translationX = xTemplMean
            - scale
                    * (cvmGet(pMatR, 0, 0) * xProbeMean
                            + cvmGet(pMatR, 0, 1) * yProbeMean);
    float translationY = yTemplMean
            - scale
                    * (cvmGet(pMatR, 1, 0) * xProbeMean
                            + cvmGet(pMatR, 1, 1) * yProbeMean);
#if 0
    cout << "getUmeyamaTransform::scale : " << scale << endl;
    cout << "getUmeyamaTransform::angleRotation : " << angleRot << " radians" << endl;
    cout << "getUmeyamaTransform::translation x : " << translationX << endl;
    cout << "getUmeyamaTransform::translation y : " << translationY << endl;
#endif

    fScale = scale;
    fRotAngle = angleRot;
    fTransX = translationX;
    fTransY = translationY;

    // Fill out the transform structure
#if 0
    Descriptor::Transform transformNew;
    transformNew.angleRot = atan2( -cvmGet( pMatR, 0, 1 ), cvmGet( pMatR, 0, 0 ));
    transformNew.scale = 1.0 / fRMSDProbe * trace.val[0];
    transformNew.xModel = xModelMean - transformNew.scale * ( cvmGet( pMatR, 0, 0 ) * xProbeMean + cvmGet( pMatR, 0, 1 ) * yProbeMean );
    transformNew.yModel = yModelMean - transformNew.scale * ( cvmGet( pMatR, 1, 0 ) * xProbeMean + cvmGet( pMatR, 1, 1 ) * yProbeMean );
    transformNew.xProbe = 0;
    transformNew.yProbe = 0;
#endif

    // Calculate residuals
#if 0
    for ( unsigned int i = 0; i < pMatTempl->cols; i++ )
    {
        double x = ( cvmGet(pMatProbe,0,i) /*- transform.xProbe*/) * scale;
        double y = ( cvmGet(pMatProbe,1,i) /*- transform.yProbe*/) * scale;
        double d = sqrt( x * x + y * y );
        double angle = atan2( y, x ) + angleRot;
        double xhat = translationX + d * cos( angle );
        double yhat = translationY + d * sin( angle );
        cout << "[" << i << "]" <<"xhat : " << xhat << " yhat : " << yhat << endl;
        cout << "(" << i << ")" <<"x    : " << cvmGet(pMatTempl,0,i) << " y    : " << cvmGet(pMatTempl,1,i) << endl;
        cout << "(" << i << ")" <<"x    : " << cvmGet(pMatProbe,0,i) << " y    : " << cvmGet(pMatProbe,1,i) << endl;
    }
#endif

#if 0
    // Calculate residuals
    if ( bDebug )
    {
        for ( unsigned int iMatch = 0; iMatch < matchInitial.vFeatureMatches.size(); iMatch++ )
        {
            if ( !vbInliers[iMatch] ) continue;
            int iFeatureProbe = matchInitial.vFeatureMatches[iMatch].iFeatureProbe;
            int iFeatureModel = matchInitial.vFeatureMatches[iMatch].iFeatureModel;
            double xProbe = vpDescriptors[iFeatureProbe]->getX();
            double yProbe = vpDescriptors[iFeatureProbe]->getY();
            double xModel = _vDescriptors[iFeatureModel].getX();
            double yModel = _vDescriptors[iFeatureModel].getY();

            double xModelHat, yModelHat;
            probeToModel( xProbe, yProbe, transformNew, xModelHat, yModelHat );

            double xErr = xModelHat - xModel;
            double yErr = yModelHat - yModel;

            cout << "Point error is " << sqrt( xErr * xErr + yErr * yErr ) << endl;
        }
    }
#endif

    cvSVD(pMatModelVar, pMatD);
    float fAspectModel = (float)(cvmGet(pMatD, 1, 1) / cvmGet(pMatD, 0, 0));
    cvSVD(pMatProbeVar, pMatD);
    float fAspectProbe = (float)(cvmGet(pMatD, 1, 1) / cvmGet(pMatD, 0, 0));

    fRMSDModel = sqrt(fRMSDModel);
    fRMSDProbe = sqrt(fRMSDProbe);
    fDistProbeMax = sqrt(fDistProbeMax);

    if (bDebug)
    {
        cout << "Model aspect ratio " << fAspectModel << " probe aspect ratio "
                << fAspectProbe << endl;
        cout << "Model var " << fRMSDModel << " probe var " << fRMSDProbe
                << endl;
    }
    cvReleaseMat(&pMatCovar);
    cvReleaseMat(&pMatModelVar);
    cvReleaseMat(&pMatProbeVar);
    cvReleaseMat(&pMatD);
    cvReleaseMat(&pMatU);
    cvReleaseMat(&pMatV);
    cvReleaseMat(&pMatS);
    cvReleaseMat(&pMatR);
    cvReleaseMat(&pMatUS);
    cvReleaseMat(&pMatDS);
}

IplImage* Utilities::plotCorrPtsOnImage(const IplImage *pImageTempl,
        const IplImage *pImageProbe, vector<CvPoint> vCorrPtsTempl,
        vector<CvPoint> vCorrPtsProbe, bool bDebug)
{
    if (vCorrPtsTempl.size() != vCorrPtsProbe.size())
    {
        _pErrorHandler->setErrorCode(
                (int)UTILITIES_INCONSISTENT_SIZE_OF_VECTORS);
    }

    IplImage *pImageTempl3Ch = cvCreateImage(
            cvSize(pImageTempl->width, pImageTempl->height), IPL_DEPTH_8U, 3);
    IplImage *pImageProbe3Ch = cvCreateImage(
            cvSize(pImageProbe->width, pImageProbe->height), IPL_DEPTH_8U, 3);
    if (pImageTempl->nChannels == 1 || pImageProbe->nChannels == 1)
    {
        cvCvtColor(pImageTempl, pImageTempl3Ch, CV_GRAY2BGR);
        cvCvtColor(pImageProbe, pImageProbe3Ch, CV_GRAY2BGR);
    }
    else
    {
        cvCopyImage(pImageTempl, pImageTempl3Ch);
        cvCopyImage(pImageProbe, pImageProbe3Ch);
    }

    int iWidthCorr = pImageTempl3Ch->width + pImageProbe3Ch->width;
    int iHeightCorr = max(pImageTempl3Ch->height, pImageProbe3Ch->height);
    IplImage *pImageWithCorrPts = cvCreateImage(cvSize(iWidthCorr, iHeightCorr),
            IPL_DEPTH_8U, 3);

    CvRect rectROI;
    rectROI.x = 0;
    rectROI.y = 0;
    rectROI.width = pImageTempl3Ch->width;
    rectROI.height = pImageTempl3Ch->height;
    cvSetImageROI(pImageWithCorrPts, rectROI);
    cvCopyImage( pImageTempl3Ch, pImageWithCorrPts);
    cvResetImageROI(pImageWithCorrPts);
    rectROI.x = pImageTempl3Ch->width;
    rectROI.y = 0;
    rectROI.width = pImageProbe3Ch->width;
    rectROI.height = pImageProbe3Ch->height;
    cvSetImageROI(pImageWithCorrPts, rectROI);
    cvCopyImage( pImageProbe3Ch, pImageWithCorrPts);
    cvResetImageROI(pImageWithCorrPts);
    CvPoint ptTempl;
    CvPoint ptProbe;
    for (unsigned int iPt = 0; iPt < vCorrPtsTempl.size(); iPt++)
    {
        ptTempl = vCorrPtsTempl[iPt];
        ptProbe = vCorrPtsProbe[iPt];
        ptProbe.x += pImageTempl3Ch->width;
        cvLine(pImageWithCorrPts, ptTempl, ptProbe,
                cvScalar(255 * rand(), 255 * rand(), 255 * rand()), 1);
    }
    if (bDebug)
    {
        cvNamedWindow("pImageWithCorrPts", 0);
        cvMoveWindow("pImageWithCorrPts", 350, 0);
        cvShowImage("pImageWithCorrPts", pImageWithCorrPts);
        cvWaitKey(0);
    }
    return pImageWithCorrPts;
}

/*
 This function ensures that the two images have the same size, depth and number of channels.
 Also, the depth should be IPL_DEPTH_8U and the no. of channels should be 1.
 */
void Utilities::checkImages(const IplImage *pImage1, const IplImage *pImage2,
        string sFunctionName)
{
    if (!pImage1 || !pImage2)
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_IMAGE_IS_NULL);
    }
    if (pImage1->width != pImage2->width || pImage1->height != pImage2->height
            || pImage1->depth != pImage2->depth
            || pImage1->nChannels != pImage2->nChannels)
    {
        //cout << " width1 = " << pImage1->width << " width2 = " << pImage2->width << endl;
        //cout << " height1 = " << pImage1->height << " height2 = " << pImage2->height << endl;
        //cout << " depth1 = " << pImage1->depth << " depth2 = " << pImage2->depth << endl;
        //cout << " nChannels1 = " << pImage1->nChannels << " nChannels2= " << pImage2->nChannels << endl;
        _pErrorHandler->setErrorCode((int)UTILITIES_INCONSISTENT_IMAGES,
                sFunctionName);
    }
}

bool Utilities::checkImageSizes(const IplImage *pImage1,
        const IplImage *pImage2)
{
    if (pImage1->width != pImage2->width || pImage1->height != pImage2->height
            || pImage1->depth != pImage2->depth
            || pImage1->nChannels != pImage2->nChannels)
    {
        return false;
    }
    return true;
}

string Utilities::getImageSizeAsString(IplImage *pImage, string sText)
{
    string sImageSize = string("");
    int iWidth = pImage->width;
    int iHeight = pImage->height;
    int iDepth = pImage->depth;
    int iNumChannels = pImage->nChannels;
    _ssOut.str("");
    _ssOut << iWidth;
    sImageSize += _ssOut.str() + string("x");
    _ssOut.str("");
    _ssOut << iHeight;
    sImageSize += _ssOut.str() + string("-");
    _ssOut.str("");
    _ssOut << iNumChannels;
    sImageSize += _ssOut.str() + string("ch-");
    _ssOut.str("");
    _ssOut << iDepth;
    sImageSize += _ssOut.str() + string("bit-") + sText;
    return sImageSize;
}

// Compute slope m1, m2. Slope = tan(theta)
// If (m1 = m2), the lines are parallel. So, no intersecting point.
// Compute y-intercept c1, c2
// Solving y_i = m1*x_i+c1 and y_i = m2*x_i+c2, we get x_i, and then y_i
// Compute x_i = (c2-c1)/(m1-m2)
// Compute y_i = m1*x_i + c1 OR y_i = m2*x_i + c2
void Utilities::getIntersectionPt(pair<cv::Point, cv::Point> lineA,
        pair<cv::Point, cv::Point> lineB, bool &bHasIntersectionPt,
        cv::Point2f &ptIntersection, bool bDebug)
{
    cv::Point pt1 = lineA.first;
    cv::Point pt2 = lineA.second;
    cv::Point pt3 = lineB.first;
    cv::Point pt4 = lineB.second;
    bHasIntersectionPt = false;
    ptIntersection.x = -1;
    ptIntersection.y = -1;

    // Compute slope and analyze
    float m1 = float(pt2.y - pt1.y) / float(pt2.x - pt1.x);
    float m2 = float(pt4.y - pt3.y) / float(pt4.x - pt3.x);
    if (bDebug) cout << "Slope: " << m1 << " " << m2 << endl;
    if (fabs(m1 - m2) < 0.01)
    {
        // Parallel lines
        bHasIntersectionPt = false;
        return;
    }
    else
    {
        bHasIntersectionPt = true;
    }

    // Compute y-intercept
    float c1 = pt1.y - m1 * pt1.x;
    float c2 = pt3.y - m2 * pt3.x;
    if (bDebug) cout << "Intercept: " << c1 << " " << c2 << endl;

    // Compute intersection point
    float x_i = (c2 - c1) / (m1 - m2);
    float y_i = m1 * x_i + c1;
    if (bDebug) cout << "Intersection: " << x_i << " " << y_i << endl;
    ptIntersection.x = x_i;
    ptIntersection.y = y_i;

    // Ensure that the intersection point lies within the violation line segment
    if (lineB.first.x < lineB.second.x)
    {
        if (x_i >= lineB.first.x && x_i <= lineB.second.x)
        {
            bHasIntersectionPt = true;
        }
        else bHasIntersectionPt = false;
    }
    else
    {
        if (x_i <= lineB.first.x && x_i >= lineB.second.x)
        {
            bHasIntersectionPt = true;
        }
        else bHasIntersectionPt = false;
    }

    if (lineB.first.y < lineB.second.y)
    {
        if (y_i >= lineB.first.y && y_i <= lineB.second.y)
        {
            bHasIntersectionPt = true;
        }
        else bHasIntersectionPt = false;
    }
    else
    {
        if (y_i <= lineB.first.y && y_i >= lineB.second.y)
        {
            bHasIntersectionPt = true;
        }
        else bHasIntersectionPt = false;
    }
}

float Utilities::getDistFromPtToPt(cv::Point2f pt1, cv::Point2f pt2)
{
    float fDist = -1.0f;
    fDist = sqrt(
            (pt2.x - pt1.x) * (pt2.x - pt1.x)
                    + (pt2.y - pt1.y) * (pt2.y - pt1.y));
    return fDist;
}

// Check if the perpendicular distance from the point to the line segment is < 1 pixel.
// http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
// Given line L0: Ax + By + C = 0 and point P1: ( x1, y1 ), the perpendicular distance
// from P1 to L0 is given by d = | Ax1 + By1 + C | / sqrt( A^2 + B^2 )
// Given, y = mx + c, y = -(A/B)x - (C/B). Consider B = 1
float Utilities::getDistFromPtToLine(pair<cv::Point, cv::Point> line,
        cv::Point2f pt, bool bDebug)
{
    float fDistance = -1.0f;
    float x1 = line.first.x;
    float y1 = line.first.y;
    float C = line.second.x - x1;
    float D = line.second.y - y1;
    float A = pt.x - x1;
    float B = pt.y - y1;
    fDistance = abs(A * D - C * B) / sqrt(C * C + D * D);
    return fDistance;
}

// Get length of the line pt1pt2
// Get distance of pt1 to pti and pt2 to pti. The sum of distance should be
// the length of the line.
bool Utilities::getPtLies0nLineSegment(pair<cv::Point, cv::Point> line,
        cv::Point2f pt, bool bDebug)
{
    cv::Point pt1 = line.first;
    cv::Point pt2 = line.second;
    float fDistPt1Pt2 = sqrt(
            (float)(pt2.x - pt1.x) * (pt2.x - pt1.x)
                    + (float)(pt2.y - pt1.y) * (pt2.y - pt1.y));
    float fDistPt1Pt = sqrt(
            (float)(pt.x - pt1.x) * (pt.x - pt1.x)
                    + (float)(pt.y - pt1.y) * (pt.y - pt1.y));
    float fDistPtPt2 = sqrt(
            (float)(pt2.x - pt.x) * (pt2.x - pt.x)
                    + (float)(pt2.y - pt.y) * (pt2.y - pt.y));
    float fEps = 1.0f;
    if (bDebug)
    {
        cout << "fDistPt1Pt2: " << fDistPt1Pt2 << endl;
        cout << "fDistPt1Pt: " << fDistPt1Pt << endl;
        cout << "fDistPtPt2: " << fDistPtPt2 << endl;
    }
    if (fabs(fDistPt1Pt2 - (fDistPt1Pt + fDistPtPt2)) <= fEps)
    {
        return true;
    }
    return false;
}

void Utilities::getBlobImage(vector<cv::Rect> vRectBlobs,
        IplImage **ppImageBlobs, bool bDebug)
{
    IplImage *pImageBlobs = *ppImageBlobs;
    cvSetZero(pImageBlobs);
    for (int i = 0; i < (int)vRectBlobs.size(); i++)
    {
        CvRect r = vRectBlobs[i];
        cvSetImageROI(pImageBlobs, r);
        cvSet(pImageBlobs, cvScalar(255));
        cvResetImageROI(pImageBlobs);
    }
    *ppImageBlobs = pImageBlobs;
}

void Utilities::calcThreshold(IplImage *pImage, double percentile,
        double &threshold, double &imageMean, double &imageStdev,
        double &zscore)
{
    int hist[256];
    int iRow, iCol, iHist;

    imageMean = 0;
    imageStdev = 0;

    for (iHist = 0; iHist < 256; iHist++)
    {
        hist[iHist] = 0;
    }
    for (iRow = 0; iRow < pImage->height; iRow++)
    {
        for (iCol = 0; iCol < pImage->width; iCol++)
        {
            uchar iVal = pImage->imageData[iRow * pImage->widthStep + iCol];
            imageMean += (double)iVal;
            imageStdev += (double)iVal * (double)iVal;
            hist[iVal]++;
        }
    }
    int N = pImage->height * pImage->width;
    imageMean /= N;
    imageStdev = sqrt(imageStdev / N - imageMean);
    int iCount = (int)(pImage->width * pImage->height * percentile);
    int iCountSoFar = 0;
    int iVal;
    for (iVal = 255; iVal > 0 && iCountSoFar < iCount; iVal--)
    {
        iCountSoFar += hist[iVal];
    }
    threshold = (double)iVal;
    zscore = (threshold - imageMean) / imageStdev;
}

void Utilities::displayLines(cv::Mat mat,
        vector<pair<CvPoint, CvPoint> > vptLines)
{
    for (int iLineNo = 0; iLineNo < (int)vptLines.size(); iLineNo++)
    {
        pair<CvPoint, CvPoint> pairPts = vptLines[iLineNo];

        cv::line(mat, pairPts.first, pairPts.second, cv::Scalar(0, 255, 0), 3,
                CV_AA);
    }
}

void Utilities::displayRects(cv::Mat mat, vector<cv::Rect> &vRect,
        CvScalar color)
{
    for (int iRectNo = 0; iRectNo < (int)vRect.size(); iRectNo++)
    {
        rectangle(mat, vRect[iRectNo], color, 2);
    }
}

string Utilities::displayImage(IplImage *pImage, string sWindowName,
        bool bFullScreen)
{
    string sName = getImageSizeAsString(pImage, sWindowName);
    cvNamedWindow((char*)sName.c_str(), CV_WINDOW_NORMAL);
    if (bFullScreen)
    {
        cvSetWindowProperty((char*)sName.c_str(), CV_WND_PROP_FULLSCREEN,
                CV_WINDOW_FULLSCREEN);
    }
    cvShowImage((char*)sName.c_str(), pImage);
    cvWaitKey(1);
    return sName;
}

string Utilities::displayMatAsImage(cv::Mat mat, string sWindowName)
{
    IplImage img = mat;
    string sName = getImageSizeAsString(&img, sWindowName);
    cv::namedWindow((char*)sName.c_str(), 0);
    cv::imshow((char*)sName.c_str(), mat);
    cv::waitKey(1);
    return sName;
}

// Compute overlap of two rectangles. Returns % of overlap
// (minx1, miny1) is the coordinate of the top left of the rectangle #1
// (maxx1, maxy1) is the coordinate of the bottom right of the rectangle #1
// (minx2, miny2) is the coordinate of the top left of the rectangle #2
// (maxx2, maxy2) is the coordinate of the bottom right of the rectangle #2
float Utilities::getRectOverlapWithRect(float minx1, float miny1, float maxx1,
        float maxy1, float minx2, float miny2, float maxx2, float maxy2)
{
//cout << minx1 << " " << miny1 << " " << maxx1 << " " << maxy1 << endl;
//cout << minx2 << " " << miny2 << " " << maxx2 << " " << maxy2 << endl;
    float overlap;
    if (minx1 > maxx2) overlap = 0.0f;
    else if (maxx1 < minx2) overlap = 0.0f;
    else if (miny1 > maxy2) overlap = 0.0f;
    else if (maxy1 < miny2) overlap = 0.0f;
    else
    {
        // Calculate overlap area
        int x_overlap = min(maxx2, maxx1) - max(minx2, minx1);
        int y_overlap = min(maxy2, maxy1) - max(miny2, miny1);
        overlap = x_overlap * y_overlap;
    }

    float area1 = (maxx1 - minx1) * (maxy1 - miny1);
    float area2 = (maxx2 - minx2) * (maxy2 - miny2);
//float area_norm = (area1 + area2) / (2.0f * area1);

// convert to a percentage
//return overlap / area_norm;
    return overlap * 100 / ((area1 + area2) / 2);
//return overlap;
}

float Utilities::getRectOverlapWithRect(cv::Rect rect1, cv::Rect rect2)
{
    float minx1 = rect1.x;
    float miny1 = rect1.y;
    float maxx1 = rect1.x + rect1.width;
    float maxy1 = rect1.y + rect1.height;
    float minx2 = rect2.x;
    float miny2 = rect2.y;
    float maxx2 = rect2.x + rect2.width;
    float maxy2 = rect2.y + rect2.height;
    //cout << minx1 << " " << miny1 << " " << maxx1 << " " << maxy1 << endl;
    //cout << minx2 << " " << miny2 << " " << maxx2 << " " << maxy2 << endl;

    float fOverlapArea;
    if (minx1 > maxx2) fOverlapArea = 0.0f;
    else if (maxx1 < minx2) fOverlapArea = 0.0f;
    else if (miny1 > maxy2) fOverlapArea = 0.0f;
    else if (maxy1 < miny2) fOverlapArea = 0.0f;
    else
    {
        // Calculate overlap area
        int x_overlap = min(maxx2, maxx1) - max(minx2, minx1);
        int y_overlap = min(maxy2, maxy1) - max(miny2, miny1);
        fOverlapArea = x_overlap * y_overlap;
    }

    float fArea1 = (maxx1 - minx1) * (maxy1 - miny1);
    float fArea2 = (maxx2 - minx2) * (maxy2 - miny2);
    float fAvgArea = (fArea1 + fArea2) / 2;
    float fOverlapPercent = fOverlapArea * 100 / fAvgArea; // Percentage
    return fOverlapPercent;
}

// Compute area overlap by checking if the point
// in actual rectangle lies inside the polygon defined by the
// user as a ground truth region using 4 points.
// NOTE: This region is assumed to be convex.

// For every point in rectDetection, check if it lies in or on the irregular polygon
// defined by N points where N >=3
float Utilities::getRectOverlapWithPolygon(cv::Rect rect1,
        cv::Mat matVerticesPolygon, bool bDebug)
{
    if (matVerticesPolygon.rows < 3)
    {
        _pErrorHandler->setErrorCode(
                (int)UTILITIES_INVALID_NUM_VERTICES_IN_POLYGON,
                "Min no. of vertices = 3");
    }
    vector<cv::Point> contour;
    for (int i = 0; i < 4; i++)
    {
        int x = (int)matVerticesPolygon.at<int>(i, 0);
        int y = (int)matVerticesPolygon.at<int>(i, 1);
        cv::Point pt(x, y);
        contour.push_back(pt);
    }

    int x0 = (int)matVerticesPolygon.at<int>(0, 0);
    int y0 = (int)matVerticesPolygon.at<int>(0, 1);
    cv::Point pt(x0, y0);
    contour.push_back(pt);
    int iNumOverlappingPts = 0;
    for (int x = rect1.x; x < rect1.x + rect1.width; x++)
    {
        for (int y = rect1.y; y < rect1.y + rect1.height; y++)
        {
            cv::Point2f pt(x, y);
            double dist = pointPolygonTest(contour, pt, false);
            // When measureDist == false, the return value is >0 (inside),
            // <0 (outside) and =0 (on edge), respectively.
            // When measureDist == true, it is a signed distance between the point
            // and the nearest contour edge
            if (dist >= 0)
            {
                // The point lies inside the contour
                iNumOverlappingPts++;
            }
        }
    }

    int iArea1 = rect1.width * rect1.height;
    cout << "Detection rect: " << rect1.x << " " << rect1.y << " "
            << rect1.width << " " << rect1.height << endl;
    float fArea2 = getAreaOfPolygon(matVerticesPolygon);
    float fAvgArea = (iArea1 + fArea2) / 2;
    float fOverlapPercent = (iNumOverlappingPts * 100) / fAvgArea;
    if (bDebug)
    {
        cout << "\nRect1: " << endl;
        cout << "Pt(0) x=" << rect1.x << "  y=" << rect1.y << endl;
        cout << "pt(1) x=" << rect1.x + rect1.width << "  y=" << rect1.y
                << endl;
        cout << "pt(2) x=" << rect1.x + rect1.width << "  y="
                << rect1.y + rect1.height << endl;
        cout << "pt(3) x=" << rect1.x << "  y=" << rect1.y + rect1.height
                << endl;
        cout << "Rect2: " << endl;
        for (int i = 0; i < 4; i++)
        {
            int x = (int)matVerticesPolygon.at<int>(i, 0);
            int y = (int)matVerticesPolygon.at<int>(i, 1);
            cout << "Pt(" << i << ")  x=" << x << " y=" << y << endl;
        }
    }
    return fOverlapPercent;
}

//http://www.mathopenref.com/coordpolygonarea2.html
//function polygonArea(X, Y, numPoints)
//{
//  area = 0;         // Accumulates area in the loop
//  j = numPoints-1;  // The last vertex is the 'previous' one to the first
//
//  for (i=0; i<numPoints; i++)
//    { area = area +  (X[j]+X[i]) * (Y[j]-Y[i]);
//      j = i;  //j is previous vertex to i
//    }
//  return area/2;
//}

// Compute area of irregular polygon
// http://www.wikihow.com/Sample/Area-of-an-Irregular-Polygon
// Area_of_irregular_polygon = 1/2 *[x1y2+x2y3+...+xny1 - (x2y1+x3y2+...+x1yn)]
float Utilities::getAreaOfPolygon(cv::Mat matVerticesPolygon)
{
    if (matVerticesPolygon.rows < 4)
    {
        _pErrorHandler->setErrorCode(
                (int)UTILITIES_INVALID_NUM_VERTICES_IN_POLYGON,
                "Min no. of vertices = 4");
    }
    // enum { CV_8U=0, CV_8S=1, CV_16U=2, CV_16S=3, CV_32S=4, CV_32F=5, CV_64F=6 };
    if (!(matVerticesPolygon.type() == CV_32S
            || matVerticesPolygon.type() == CV_32F))
    {
        _pErrorHandler->setErrorCode((int)UTILITIES_INVALID_DATA_TYPE,
                "Expecting CV_32S (integer) or CV_32F (float)");
    }
    float fAreaPolygon = 0.0f;
    int iNumPts = matVerticesPolygon.rows;
    if (matVerticesPolygon.type() == CV_32S)
    {
        int j = iNumPts - 1;
        for (int i = 0; i < iNumPts; i++)
        {
            fAreaPolygon += (matVerticesPolygon.at<int>(j, 0)
                    + matVerticesPolygon.at<int>(i, 0))
                    * (matVerticesPolygon.at<int>(j, 1)
                            - matVerticesPolygon.at<int>(i, 1));
            j = i;
        }
        fAreaPolygon /= 2;
    }
    else if (matVerticesPolygon.type() == CV_32F)
    {
        int j = iNumPts - 1;
        for (int i = 0; i < iNumPts; i++)
        {
            fAreaPolygon += (matVerticesPolygon.at<int>(j, 0)
                    + matVerticesPolygon.at<int>(i, 0))
                    * (matVerticesPolygon.at<int>(j, 1)
                            - matVerticesPolygon.at<int>(i, 1));
            j = i;
        }
        fAreaPolygon /= 2;
    }

    if (fAreaPolygon < 0) fAreaPolygon *= -1;
    if (fAreaPolygon < 0)
    {
        cout << "Area is negative: " << fAreaPolygon << endl;
        for (int i = 0; i < iNumPts; i++)
        {
            if (matVerticesPolygon.type() == CV_32S)
            {
                cout << matVerticesPolygon.at<int>(i, 0) << " "
                        << matVerticesPolygon.at<int>(i, 1) << endl;
            }
            else if (matVerticesPolygon.type() == CV_32F)
            {
                cout << matVerticesPolygon.at<float>(i, 0) << " "
                        << matVerticesPolygon.at<float>(i, 1) << endl;
            }
        }
        cin.get();
    }
    return fAreaPolygon;
}

// Area of the triangle given 3 points. Using the points, the side lengths can
// be computed, and then use Heron's formula to find the area of the irregular
// triangle.
// http://www.mathsisfun.com/geometry/herons-formula.html
// Given side lengths a, b and c,
// Compute area as follows:
// Area = sqrt(s(s-a)*(s-b)*(s-c))
// where s = (a+b+c)/2
//
// For other cases check out
// http://www.mathsisfun.com/algebra/trig-area-triangle-without-right-angle.html
float Utilities::getAreaOfTriangle(float a, float b, float c)
{
    float s = (a + b + c) / 2;
    float fArea = sqrt(s * (s - a) * (s - b) * (s - c));
    return fArea;
}

string Utilities::convertIntegerToString(int iVal)
{
    _ssOut.str("");
    _ssOut << iVal;
    return _ssOut.str();
}

string Utilities::convertFloatToString(float fVal)
{
    _ssOut << std::fixed << std::setprecision(3);
    _ssOut.str("");
    _ssOut << fVal;
    return _ssOut.str();
}

// Alternatively iVal = atoi( sText.c_str() );
int Utilities::convertStringToInteger(string sText)
{
    int iVal = 0;
    std::istringstream buf(sText);
    buf >> iVal;
    return iVal;
}

float Utilities::convertStringToFloat(string sText)
{
    float fVal = 0;
    std::istringstream buf(sText);
    buf >> fVal;
    return fVal;
}

double Utilities::convertStringToDouble(string sText)
{
    double dVal = 0;
    std::istringstream buf(sText);
    buf >> dVal;
    return dVal;
}

bool Utilities::convertIntegerToBool(int iVal)
{
    if (iVal == 0) return true;
    return false;
}

string Utilities::convertLongToString(long lVal)
{
    _ssOut.str("");
    _ssOut << lVal;
    return _ssOut.str();
}

vector<string> Utilities::getFolderNameFromeDir(string sDir)
{
    vector<string> vFolderNames;

    DIR *dir = opendir(sDir.c_str());
    if (dir == NULL)
    {
        cout << "Could't open this directory (" << sDir.c_str() << ")" << endl;
    }

    struct dirent *entity = readdir(dir);
    while (entity != NULL)
    {
        processEntity(vFolderNames, entity);
        entity = readdir(dir);
    }
    closedir(dir);

    return vFolderNames;
}

void Utilities::processEntity(vector<string> &vFolderNames, dirent* entity)
{
    if (entity->d_type == DT_DIR)
    {
        if (entity->d_name[0] == '.')
        {
            return;
        }
        string sFolder = string(entity->d_name);
        vFolderNames.push_back(sFolder);
    }
}

vector<string> Utilities::tokenizeString(const string source,
        const char *delimiter, bool keepEmpty)
{
    vector<string> results;

    size_t prev = 0;
    size_t next = 0;

    while ((next = source.find_first_of(delimiter, prev)) != string::npos)
    {
        if (keepEmpty || (next - prev != 0))
        {
            results.push_back(source.substr(prev, next - prev));
        }
        prev = next + 1;
    }

    if (prev < source.size())
    {
        results.push_back(source.substr(prev));
    }

    return results;
}

int Utilities::createFolder(string sFolder)
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
        cerr << "Unable not create specified output folder: " << sFolder
                << endl;
        std::cerr << "\tError No.: " << errno << endl;
        std::cerr << "\tError String: " << strerror(errno) << endl;
    }
}

// == Getters and Setters ==

int Utilities::getObjCount()
{
    return Utilities::_iObjCount;
}

string Utilities::getCurWorkingDir()
{
    return _sCWD;
}
