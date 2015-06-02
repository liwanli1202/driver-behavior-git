/*
 * Utilities.h
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <cv.h>

#ifdef linux
#include <dirent.h>
#include <unistd.h>
#include <cerrno>               // errno.h
#else
#include <direct.h>             // for getcwd()
#include <windows.h>
#endif
#include <stdlib.h>             // for _MAX_PATH
#include <string>
#include <vector>
using namespace std;

#include "ErrorHandler.h"
#include "Logger.h"

class Utilities
{
public:
    Utilities();
    ~Utilities();

    void initErrorDiagnosticsList();

#ifdef WIN32
    // Conversion from Ansi to Unicode character set and Vice versa.
    // These functions are necessary when the Character set of the 
    // project is set to "Unicode Character Set" or "Not Set".
    static BSTR convertAnsiToUnicode( char *szAnsi );
    static CHAR* convertUnicodeToAnsi( LPCWSTR szUnicode );
#endif

    /**
     This function obtains the names of the files in the specified directory that
     agrees with the specified wildcard.
     */
    std::vector<string> getFilenamesFromDir(char *szDir,
            char *szPathWithWildcard, bool bGetRelativePath);

    /**
     This function takes the name of the file with full path and returns only the file name.
     */
    std::string getFilenameWithoutPath(string sFilenameWithPath);

    /**
     This function checks whether the specified file/folder is present or not.
     @param sPath The path to check if the file or folder is present.
     @return bool If the file is present, true is returned, else false is returned.
     */
    static bool getIsFileOrFolderPresent(string sPath, bool &bIsFile);

    /**
     This function computes and shows the histogram of the input image in grayscale.
     @param IplImage* A color image to compute the histogram in grayscale
     */
    void showHistogram(IplImage* img);

    /**
     This function finds a cosine of angle between vectors from pt0->pt1 and from pt0->pt2.
     @param pt1 One of the 3 points
     @param pt2 One of the 3 points
     @param pt3 One of the 3 points
     @return angle A cosine of angle between vectors from pt0->pt1 and from pt0->pt2
     */
    static double computeAngle(CvPoint pt1, CvPoint pt2, CvPoint pt0);

    /**
     This function fits the specified set of points to a line.
     @param &vPts Points that need to be fitted onto a line.
     @param &ptP1 The first point of the fitted line.
     @param &ptP2 The last point of the fitted line.
     @param &dAngle The slope of the line in degrees.
     @return vector<CvPoint2D32f> The vector of points is returned after filtering the input vector of points.
     */
    vector<CvPoint2D32f> fitLineRANSAC(vector<CvPoint2D32f> vPts,
            CvPoint2D32f &ptP1, CvPoint2D32f &ptP2, double &dAngle,
            bool bDebug = false);

    /**
     This function rotates the image by the specified angle obtained using the two points specified.
     The rotated points are returned for the specified two points to identify the line used to calculate the angle.
     Currently, the image is rotated about origin.
     Also, the rotation is performed considering the image coordinate system, i.e. top-left point is the origin.
     @param *pImage The image to be rotated.
     @param dAngleRad The angle of rotation in radians.
     @param &ptP1 The specified point will be rotated and returned in this variable.
     @param &ptP2 The specified point will be rotated and returned in this variable.
     @return IplImage* The rotated image is returned.
     */
    IplImage* getRotatedImage(IplImage *pImage, double dAngleRad,
            CvPoint2D32f &ptP1, CvPoint2D32f &ptP2, bool bDebug = false);

    /**
     * This function resizes the image to have NEW_WIDTHxNEW_HEIGHT.
     * @param *pImageSrc The image to be resized.
     */
    IplImage* getResizedImage(const IplImage *pImageSrc, int iWidth,
            int iHeight);

    CvMat* getRotationMatrix(double dAngleRad, bool bDebug = false);
    void getRotatedPoint(CvPoint &pointToRotate, double dOri,
            CvPoint pointPivot = cvPoint(0, 0));
    void getTransformationMatFromRandT(const CvMat *pMatR, const CvMat *pMatT,
            CvMat **ppMatTrans);
    void getRandTMatFromTransformationMat(const CvMat *pMatTrans,
            CvMat **ppMatR, CvMat **ppMatT);

    void getMatrixScalarMultiply(CvMat **ppMat, double dMultiplier);
    void displayMatrix(CvMat *pMat, char *pchMatName);
    bool getIsEqualSizedMatrices(CvMat *pMatA, CvMat *pMatB);
    bool getIsEqualSizedImages(IplImage *pImageA, IplImage *pImageB);

    void sortVecOfStrings(vector<string> &vString);

    CvRect getROIFromAreaMask(const IplImage *pImageAreaMask, bool bDebug =
            false);
    void getBoundaryOfMask(const IplImage *pImageMask,
            IplImage *pImageMaskBoundary, int iBoundaryThickness, bool bDebug =
                    false);

    // Compute Normalized Cross Correlation score for a pixel at location (iCol, iRow)
    bool getNCC(const IplImage *pImageT, const IplImage *pImageI, int iRow,
            int iCol, int iMaskSize, float &fNCC, bool bShadowTest = false,
            bool bDebug = false);
    bool getNCC(const cv::Mat &matTarget, const cv::Mat &matInput,
            cv::Point2f ptTargetRowCol, cv::Point2f ptInputRowCol,
            int iMaskSize, float &fNCC, bool bDebug = false);

    // Compute Normalized Cross Correlation score for two images of same size
    bool getNCC(const IplImage *pImageT, const IplImage *pImageI, float &fNCC,
            bool bDebug = false);
    void getCenterOfGravity(CvSeq *pContour, bool &bValidCG, CvPoint2D32f &ptCG,
            bool bDebug = false);
    void computeMagAndAngleBetweenRGBVectors(int B1, int G1, int R1, int B2,
            int G2, int R2, float &fMag1, float &fMag2, float &fThetaRad,
            bool bDebug = false);

    int getNumExternalContours(const IplImage *pImage, bool bDebug = false);
    int getNumContours(CvSeq *pSeqContours, bool bDebug = false);

    CvSeq* getAlphaShape_ConvexHull(CvSize sizeImage, vector<CvPoint> vPts,
            int &iContourFound, bool bDebug = false);

    void overlayImage(const IplImage *pImageOverlay, IplImage *pImageBaseC,
            int iChannel, bool bSingleChannel = false);
    void overlayImage(const IplImage *pImageOverlay, IplImage *pImageBaseC,
            CvScalar color);
    void overlayImageOnGray(const IplImage *pImageOverlay,
            IplImage *pImageBaseGray, CvScalar color);
    void overlayContour(const IplImage *pImageOverlayBinary,
            IplImage *pImageBaseC, CvScalar color);
    void drawLines(IplImage *pImage, vector<CvPoint> vPts);
    void drawRectangle(IplImage *pImage, cv::Rect rect, CvScalar color, int
            iThickness);
    void convertContourInSeqToPts(CvSeq *pContour, vector<CvPoint*> &vPts);

    void writeTextOnImage(IplImage* pImage, string sText, int iPos = 0);
    void writeTextOnImage(IplImage *pImage, CvPoint pt, string sText);
    void getImageMask(const IplImage *pImage, IplImage *pImageMask,
            int iPixelVal, bool bDebug = false);
    void getImageMask(const IplImage *pImageC, IplImage *pImageMask,
            int iChannel, int iPixelVal, bool bDebug = false);
    void removeShadow(IplImage *pImageFg, const IplImage *pImageShadow,
            bool bDebug = false);
    void analyzeDetectionUsingGroundTruth(const IplImage *pImageGT,
            int &iNumPixGT, const IplImage *pImageD, int &iTP, int &iTN,
            int &iFP, int &iFN, IplImage *pImageOverlayC, bool bDebug = false);
    int getNumActivePixels(const IplImage *pImage);

    // ==================== Watermark ===================================
    // Functions related to watermarking an image using either
    // an image (e.g. logo) or a text added to a blank image.
    void watermarkImage(cv::Mat &img, cv::Mat &wm, float alpha, cv::Mat &dst,
            bool bDebug = false);
    void blurWatermarkImage(cv::Mat &wm, bool bDebug = false);
    void displayMat(cv::Mat mat, bool bDebug = false);
    cv::Mat rotateImage(cv::Mat src, float angleDegrees, float scale,
            bool bDebug = false);
    cv::Mat rotateImage(cv::Mat src, float angleDegrees, bool bDebug = false);
    cv::Mat getImageWithWatermarkText(string s, int iNumLines,
            float fAngleDegrees, float fScale, int iImageWidth,
            int iImageHeight, bool bDebug = false);

    void ensureRectIsInsideImage(const IplImage *pImage, cv::Rect &rect);

    // Transform
    IplImage* getRotatedImageSubPixWithTranslation(IplImage *pImage,
            double dAngleRad, double dScale, float xProbeMeanToImgCenter,
            float yProbeMeanToImgCenter, float xTemplMeanToImgCenter,
            float yTemplMeanToImgCenter, bool bDebug);
    void getUmeyamaTransform(CvMat *pMatTempl, CvMat *pMatProbe, float &fScale,
            float &fRotAngle, float &fTransX, float &fTransY, bool bDebug);
    IplImage* plotCorrPtsOnImage(const IplImage *pImageTempl,
            const IplImage *pImageProbe, vector<CvPoint> vCorrPtsTempl,
            vector<CvPoint> vCorrPtsProbe, bool bDebug = false);

    void checkImages(const IplImage *pImageSrc1, const IplImage *pImageSrc2,
            string sFunctionName);
    bool checkImageSizes(const IplImage *pImage1, const IplImage *pImage2);

    string getImageSizeAsString(IplImage *pImage, string sText);

    // Intersection of lines
    void getIntersectionPt(pair<cv::Point, cv::Point> lineA,
            pair<cv::Point, cv::Point> lineB, bool &bHasIntersectionPt,
            cv::Point2f &ptIntersection, bool bDebug = false);
    float getDistFromPtToPt(cv::Point2f pt1, cv::Point2f pt2);
    float getDistFromPtToLine(pair<cv::Point, cv::Point> line, cv::Point2f pt,
            bool bDebug = false);
    bool getPtLies0nLineSegment(pair<cv::Point, cv::Point> line, cv::Point2f pt,
            bool bDebug = false);

    // Blobs
    void getBlobImage(vector<cv::Rect> vRectBlobs, IplImage **ppImageBlobs,
            bool bDebug = false);

    // Threshold
    void calcThreshold(IplImage *pImage, double percentile, double &threshold,
            double &imageMean, double &imageStdev, double &zscore);

    // Display properties
    void displayLines(cv::Mat mat, vector<pair<CvPoint, CvPoint> > vptLines);
    void displayRects(cv::Mat mat, vector<cv::Rect> &vRect, CvScalar color =
            cv::Scalar(0, 255, 0));
    string displayImage(IplImage *pImage, string sWindowName, bool bFullScreen =
            false);
    string displayMatAsImage(cv::Mat mat, string sWindowName);

    // Rectangle overlap area
    float getRectOverlapWithRect(float minx1, float miny1, float maxx1,
            float maxy1, float minx2, float miny2, float maxx2, float maxy2);
    float getRectOverlapWithRect(cv::Rect rect1, cv::Rect rect2);
    float getRectOverlapWithPolygon(cv::Rect rect1, cv::Mat matVerticesPolygon,
            bool bDebug = false);
    float getAreaOfPolygon(cv::Mat matVerticesPolygon);
    float getAreaOfTriangle(float fAB, float fBC, float fCA);

    string convertIntegerToString(int iVal);
    string convertFloatToString(float fVal);
    int convertStringToInteger(string sText);
    float convertStringToFloat(string sText);
    double convertStringToDouble(string sText);
    bool convertIntegerToBool(int iVal);
    string convertLongToString(long lVal);
    vector<string> getFolderNameFromeDir(string sDir);
    void processEntity(vector<string> &vFolderNames, dirent* entity);

    // Tokenize
    vector<string> tokenizeString(const string source, const char *delimiter =
            " ", bool keepEmpty = false);

    // Create folder
    int createFolder(string sFolder);

    // Getters and Setters
    static int getObjCount();
    string getCurWorkingDir();

private:
    static int _iObjCount;
    static bool _bErrorDiagListIsSet;
    static ErrorHandler *_pErrorHandler;
    static Logger *_pLogger;

    stringstream _ssOut;
    string _sText;

#ifdef WIN32
    char _szPathBuffer[ _MAX_PATH ];
#else
    char _szPathBuffer[FILENAME_MAX];
#endif
    string _sCWD;

    int _iImageWidth;
    int _iImageHeight;
    double _dHScale;
    double _dVScale;
    int _iLineWidth;
    int _iNumRowsForText;
    CvFont _font;
    CvFont _font2;

    CvMemStorage *_pMemStorage;
    CvSeq *_pSeqContours;
    IplImage *_pImageContoursTmp;
};
#endif // UTILITIES_H_
