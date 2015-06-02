/*
 * Util.cpp
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#include "Util.h"

Util::Util() {
	// TODO Auto-generated constructor stub

}

Util::~Util() {
	// TODO Auto-generated destructor stub
}

void Util::image_to_ground_plane( const std::vector< std::vector<cv::Point> >& image_plane_sequences,
								  std::vector< std::vector<cv::Point2f> >& ground_plane_sequences,
								  CameraGeometry& camera) {
  int i, j;
  for (i = 0; i < static_cast<int>(image_plane_sequences.size()); i++) {
    std::vector<cv::Point2f> sequence;
    for (j = 0; j < static_cast<int>(image_plane_sequences[i].size()); j++) {
      CvPoint2D32f ground_pt = camera.imageToGroundPlane(image_plane_sequences[i][j]);
      sequence.push_back(ground_pt);
    }
    ground_plane_sequences.push_back(sequence);
  }

  return;
}

void Util::imagePlaneToGroundPlane(
		const cv::Point2f &pointImage,
		cv::Point2f &pointGround,
		CvMat* matHInv)
{
	CvMat *matImagePoint = cvCreateMat(3, 1, CV_32FC1);
	cvmSet(matImagePoint, 0, 0, pointImage.x);
	cvmSet(matImagePoint, 1, 0, pointImage.y);
	cvmSet(matImagePoint, 2, 0, 1);
	cvMatMul(matHInv, matImagePoint, matImagePoint);
	float denominator = cvmGet(matImagePoint, 2, 0);
	pointGround.x = cvmGet(matImagePoint, 0, 0) / denominator;
	pointGround.y = cvmGet(matImagePoint, 1, 0) / denominator;
	cvReleaseMat(&matImagePoint);
}

void Util::groundPlaneToImagePlane(
		const cv::Point2f &pointGround,
		cv::Point2f &pointImage,
		CvMat* matH)
{
	CvMat *matPoint = cvCreateMat(3, 1, CV_32FC1);
	cvmSet(matPoint, 0, 0, pointGround.x);
	cvmSet(matPoint, 1, 0, pointGround.y);
	cvmSet(matPoint, 2, 0, 1);
	cvMatMul(matH, matPoint, matPoint);
	float denominator = cvmGet(matPoint, 2, 0);
	pointImage.x = cvmGet(matPoint, 0, 0) / denominator;
	pointImage.y = cvmGet(matPoint, 1, 0) / denominator;
	cvReleaseMat(&matPoint);
}


void Util::imagePlaneToGroundPlane(
		std::vector< std::vector<cv::Point2f> >& image_plane_sequences,
		std::vector< std::vector<cv::Point2f> >& ground_plane_sequences,
		CvMat* matHInv)
{
	CvMat *matImagePoint = cvCreateMat(3, 1, CV_32FC1);
	float denominator;
	for (unsigned int i = 0; i < image_plane_sequences.size(); i++)
	{
		std::vector<cv::Point2f> sequence;
		for (unsigned int j = 0; j < image_plane_sequences[i].size(); j++)
		{
			cvmSet(matImagePoint, 0, 0, image_plane_sequences[i][j].x);
			cvmSet(matImagePoint, 1, 0, image_plane_sequences[i][j].y);
			cvmSet(matImagePoint, 2, 0, 1);
			cvMatMul(matHInv, matImagePoint, matImagePoint);
			denominator = cvmGet(matImagePoint, 2, 0);
			CvPoint2D32f ground_pt = cv::Point2f(cvmGet(matImagePoint, 0, 0) / denominator,
					cvmGet(matImagePoint, 1, 0) / denominator);
			sequence.push_back(ground_pt);
		}
		ground_plane_sequences.push_back(sequence);
	}
	cvReleaseMat(&matImagePoint);
}


void Util::image_to_ground_plane(
    const std::vector< std::vector<cv::Point2f> >& image_plane_sequences,
    std::vector< std::vector<cv::Point2f> >& ground_plane_sequences,
    CameraGeometry* camera)
{

	for (int i = 0; i < static_cast<int>(image_plane_sequences.size()); i++)
	{
		std::vector<cv::Point2f> sequence;
		for (int j = 0; j < static_cast<int>(image_plane_sequences[i].size()); j++)
		{
		  CvPoint2D32f ground_pt = camera->imageToGroundPlane(image_plane_sequences[i][j]);
		  sequence.push_back(ground_pt);
		}
		ground_plane_sequences.push_back(sequence);
	}
}

void
Util::scaleGroundPlaneSequences(
		const std::vector< std::vector <cv::Point2f> >& ground_plane_sequences,
		std::vector< std::vector <cv::Point2f> >& scaled_ground_plane_sequences)
{
	assert(scaled_ground_plane_sequences.size() == 0);
	for (int i = 0; i < static_cast<int>(ground_plane_sequences.size()); i++)
	{
		std::vector<cv::Point2f> sequence;
		for (int j = 0; j < static_cast<int>(ground_plane_sequences[i].size()); j++)
		{
			cv::Point2f pointScaled;
			pointScaled.x = 320 + ground_plane_sequences[i][j].x * 60;
			pointScaled.y = 479 - ground_plane_sequences[i][j].y * 20;
			sequence.push_back(pointScaled);
		}
		scaled_ground_plane_sequences.push_back(sequence);
	}
}


/* Move ground plane sequences back to origin to prepare for back projection
 * to image plane.
 */

void Util::normalize_ground_plane_sequences(
    std::vector< std::vector <cv::Point2f> >& ground_plane_sequences){
  int i, j;
  for (i = 0; i < static_cast<int>(ground_plane_sequences.size()); i++) {
    for (j = 0; j < static_cast<int>(ground_plane_sequences[i].size()); j++) {
      ground_plane_sequences[i][j].x = (ground_plane_sequences[i][j].x - 320.0)/ 60.0;//* 30;
      ground_plane_sequences[i][j].y = (ground_plane_sequences[i][j].y - 479.0)/ 20.0;//* 10;
    }
  }
}


//void Util::draw_ground_points(
//    std::vector<std::vector<cv::Point2f> > ground_plane_sequences) {
//  IplImage* ground_points = cvCreateImage(cvSize(640, 480), 8, 1);
//  cvNamedWindow("ground_points", CV_WINDOW_AUTOSIZE);
//  cvZero(ground_points);
//  int i, j;
//  CvScalar value;
//
//  for (i = 0; i < static_cast<int>(ground_plane_sequences.size()); ++i) {
//    for (j = 0; j < static_cast<int>(ground_plane_sequences[i].size()); ++j) {
//      value.val[0] = 255;
//      if (cvRound(ground_plane_sequences[i][j].x) >= 0 &&
//          cvRound(ground_plane_sequences[i][j].x) < 640 &&
//          cvRound(ground_plane_sequences[i][j].y) >= 0 &&
//          cvRound(ground_plane_sequences[i][j].y) < 480) {
//
//        cvSet2D(ground_points, cvRound(ground_plane_sequences[i][j].y),
//                cvRound(ground_plane_sequences[i][j].x), value);
//      }
//    }
//  }
//  cvShowImage("ground_points", ground_points);
//
//}

void Util::drawLines(
    std::vector< std::vector <cv::Point2f> > ground_plane_sequences){
  IplImage* image_lines = cvCreateImage(cvSize(640, 480), 8, 3);
  cvNamedWindow("fitline", CV_WINDOW_AUTOSIZE);
  cvZero(image_lines);
  int i, j;
  float line[4];
  CvPoint left, right;

  for(i = 0; i < static_cast<int>(ground_plane_sequences.size()); i++){
    CvPoint* points = (CvPoint*)malloc(ground_plane_sequences[i].size() * sizeof(points[0]));
    CvMat pointMat = cvMat(1, ground_plane_sequences[i].size(), CV_32SC2, points);
    for(j = 0; j < static_cast<int>(ground_plane_sequences[i].size()); j++){
      points[j].x = cvRound(ground_plane_sequences[i][j].x);
      points[j].y = cvRound(ground_plane_sequences[i][j].y);
    }
    cvFitLine(&pointMat, CV_DIST_L2, 1, 0.001, 0.001, line);
    double a, b, c, d, e, f;
    b = line[1] / line[0];
    a = line[3] - b*line[2];
    d = - 1/b;
    c = points[0].y - d*points[0].x;
    f = d;
    e = points[ground_plane_sequences[i].size() - 1].y -
        f*points[ground_plane_sequences[i].size() - 1].x;
    left.x = (a - c) / (d - b);
    left.y = c + d * left.x;
    right.x = (a - e) / (f - b);
    right.y = e + f * right.x;
    //CvPoint center;
    //center.x = line[2];
    //center.y = line[3];
    cvLine(image_lines, left, right, CV_RGB(255, 0, 0), 1, CV_AA, 0);
    free(points);
  }
  cvSaveImage("fit_line.png", image_lines);
  cvShowImage("fitline", image_lines);
}

cv::Point Util::RoundPoint(cv::Point2f point_f) {
  return cv::Point(cvRound(point_f.x), cvRound(point_f.y) );
}

void Util::ToGroundPlane(
    std::vector< std::vector< std::vector<cv::Point2f> > > lane_marking,
    std::vector< std::vector< std::vector<cv::Point2f> > >& lane_marking_ground,
    CameraGeometry *camera) {
  for (int i = 0; i < static_cast<int>(lane_marking.size()); ++i) {
    std::vector< std::vector<cv::Point2f> > ground_plane_sequence;
    (*this).image_to_ground_plane(lane_marking[i], ground_plane_sequence, camera);
    lane_marking_ground.push_back(ground_plane_sequence);
  }
}

void Util::MergeSequenceToPoints(std::vector< std::vector<cv::Point2f> > sequence,
                                std::vector<cv::Point2f>& points) {
  for (int i = 0; i < static_cast<int>(sequence.size()); ++i) {
    for(int j = 0; j < static_cast<int>(sequence[i].size()); ++j) {
      points.push_back(sequence[i][j]);
    }
  }
}

void Util::MergeLaneMarkingToPoints(
    std::vector< std::vector <std::vector<cv::Point2f> > > lane_marking,
    std::vector<cv::Point2f>& points) {
  for (int i = 0; i < static_cast<int>(lane_marking.size()); ++i) {
    std::vector<cv::Point2f> p;
    (*this).MergeSequenceToPoints(lane_marking[i], p);
    for (int k = 0; k < static_cast<int>(p.size()); ++k) {
      points.push_back(p[k]);
    }
  }
}

void Util::ClosestPointToOrigin(
    std::vector< std::vector<cv::Point2f> > sequence,
    cv::Point2f& point, int& sequence_number) {
  double distance = 0.0;
  double min_distance = std::numeric_limits<double>::max();

  for (int i = 0; i < static_cast<int>(sequence.size()); ++i) {
    for (int j = 0; j < static_cast<int>(sequence[i].size()); ++j) {
      distance = sqrt(pow((sequence[i][j].x - 325), 2) +
                      pow((sequence[i][j].y - 464), 2));
      if (distance < min_distance) {
        min_distance = distance;
        point = sequence[i][j];
        sequence_number = i;
      }
    }
  }
  return;
}

void Util::MostRightPointToOrigin(
    std::vector< std::vector<cv::Point2f> > sequence,
    cv::Point2f& point, int& sequence_number) {
  double distance = 0.0;
  double max_distance = std::numeric_limits<double>::min();

  for (int i = 0; i < static_cast<int>(sequence.size()); ++i)
  {
    for (int j = 0; j < static_cast<int>(sequence[i].size()); ++j)
    {
    	if(sequence[i][j].x > 325)
    	{
		  distance = sqrt(pow((sequence[i][j].x - 325), 2) +
						  pow((sequence[i][j].y - 464), 2));
		  if (distance > max_distance)
		  {
			max_distance = distance;
			point = sequence[i][j];
			sequence_number = i;
		  }
    	}
    }
  }
  return;
}

void Util::MostLeftPointToOrigin(
    std::vector< std::vector<cv::Point2f> > sequence,
    cv::Point2f& point, int& sequence_number) {
  double distance = 0.0;
  double max_distance = std::numeric_limits<double>::min();

  for (int i = 0; i < static_cast<int>(sequence.size()); ++i)
  {
    for (int j = 0; j < static_cast<int>(sequence[i].size()); ++j)
    {
    	if(sequence[i][j].x < 325)
    	{
			distance = sqrt(pow((sequence[i][j].x - 325), 2) +
						  pow((sequence[i][j].y - 464), 2));
			if (distance > max_distance)
			{
				max_distance = distance;
				point = sequence[i][j];
				sequence_number = i;
			}
    	}
    }
  }
  return;
}


cv::Point2f Util::PointWithMaxY(std::vector<cv::Point2f> points) {
  double max_y = 0;
  cv::Point2f point;
  for (int i = 0; i < static_cast<int>(points.size()); ++i) {
    if (points[i].y > max_y) {
      max_y = points[i].y;
      point = points[i];
    }
  }
  return point;
}


void Util::FurthestPointFromOrigin(
    std::vector< std::vector<cv::Point2f> > sequence,
    cv::Point2f& point, int& sequence_number) {
  double max_y = 0.0;
  for (int i = 0; i < static_cast<int>(sequence.size()); ++i) {
    for (int j = 0; j < static_cast<int>(sequence[i].size()); ++j) {
      if (sequence[i][j].y > max_y) {
        max_y = sequence[i][j].y;
        point = sequence[i][j];
        sequence_number = i;
      }
    }
  }
}

void Util::printMat(CvMat* mat)
{
	for(int i=0; i < mat->width; i++)
	{
		for(int j=0; j < mat->height; j++)
		{
			cout << cvmGet(mat, i, j) << " " ;
		}
		cout << endl;
	}

}


void Util::graph(double dis_right, double dis_left)
{
	rightQueue.push_front(dis_right);
	leftQueue.push_front(dis_left);

	cv::Mat graph(400, 600, CV_8UC3, Scalar(255, 255, 255));

	//writing on the graph
	putText(graph, "Lateral distance in real time" , cvPoint(130,30),
					FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,0), 1, CV_AA);
	putText(graph, "Right" , cvPoint(10,120),
					FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,0), 1, CV_AA);
	putText(graph, "Left" , cvPoint(10,320),
					FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,0), 1, CV_AA);

	//arrays to store the scaled distances
	int scaledValueRight[40];
	int scaledValueLeft[40];

	//initialize the arrays with 0
	for (int i=0; i<41 ; i++)
	{
		scaledValueRight[i] = 0;
			scaledValueLeft[i] = 0;
	}

	//fill up the arrays with scaled values
	for (int i=0; i<40 ; i++)
	{
		scaledValueRight[i] = (150/100)*rightQueue[i];
		scaledValueLeft[i] = (150/100)*leftQueue[i];

	}

	//plotting on the graph
	for (int i=0; i<40 ; i++)
	{
		line(graph, Point(10*i+100, 160 - scaledValueRight[i]),Point(10*i+100, 160 ),Scalar(0,0,255), 2, 8, 0);
		line(graph, Point(10*i+100, 350 - scaledValueLeft[i]),Point(10*i+100, 350 ),Scalar(0,255,0), 2, 8, 0);
	}

	//displaying the graph
	cv::namedWindow("Visualizing Lateral Distance", CV_WINDOW_AUTOSIZE);
	cv::imshow("Visualizing Lateral Distance", graph);
}

