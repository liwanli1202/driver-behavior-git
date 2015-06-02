#include <deque>
#include <string>
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "DriverBehaviorAnalysis.h"
using namespace cv;
using namespace std;


bool _isLaneChange; 
int frameCount;
int sentinel;  
int size = 40;
std::deque<double> lateralQueue;
std::deque<double> variQueue;
int k = 0;
double data[40];

//new counter to eleminate 2 positives for one lane change
int _counter = 0; 



//for lane change frequency
long long timeDriven;
bool bPrevious = false;
long double numberOfLaneChanges = 0; 
std::deque<long long> timeQueue;

//for stability
double stability;
std::deque<double> stabQueue;


DriverBehaviorAnalysis::DriverBehaviorAnalysis(){}



double DriverBehaviorAnalysis::getMean(double data[])
{
    double sum = 0.0;
    for(int i = 0 ; i < size; i++)
          sum += data[i];
    return sum/size;
}


double DriverBehaviorAnalysis::getVariance(double data[])
{
    double mean = getMean(data);
    double temp = 0;
    for(int i = 0 ; i < size ; i++)
         temp += (mean-data[i])*(mean-data[i]);
    return temp/size;
}

bool DriverBehaviorAnalysis::detectLaneChange(double dPosition, double left , double right, long long timeStamp1)
{
	long double timeStamp = timeStamp1;

	_isLaneChange = false; 

	k++;
        lateralQueue.push_front(dPosition);

        cv::Mat graph(600, 600, CV_8UC3, Scalar(255, 255, 255));
	        cv::Mat graph_v(300, 600, CV_8UC3, Scalar(255, 255, 255));

        //writing on the graph
       // putText(graph, "Lateral Position" , cvPoint(160,30),
         //                               FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,0), 1, CV_AA);


   //     putText(graph, "Variance" , cvPoint(160,580),
     //                                   FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,0), 1, CV_AA);

        //arrays to store the scaled distances
        int scaledValue[40];
        int varianceArray[size];

        //initialize the arrays with 0
        for (int i=0; i<size+1 ; i++)
        {
                scaledValue[i] = 0;
        }

        for (int i=0; i<size+1; i++)
        {
                varianceArray[size]=0;
        }


	 //fill up the arrays with scaled values
        for (int i=0; i<size ; i++)
        {
                scaledValue[i] = lateralQueue[i]*1000/4;
               data[i] = lateralQueue[i];

        }

        variQueue.push_front(getVariance(data));

	for (int i=0; i<size ; i++)
        {
                data[i] = variQueue[i];

        }



        double mean = getMean(data);

        for (int i=0; i<size ; i++)
        {
                varianceArray[i] = variQueue[i]*500;
        }


        //plotting on the graph
        for (int i=0; i<size ; i++)
        {
                int g = scaledValue[i];
     //           line(graph, Point(100+10*i, 160-g),Point(100+10*i, 160 ),Scalar(0,0,255), 2, 8, 0);
        }


	//cout << "mean= " <<  mean << " ,variance= " << variQueue[0] << endl;
	//cout << dPosition <<" left= " << left << " right= " << right << " L+R= " << left+right << endl;  
/*
       ofstream myfile;
       myfile.open ("variance.txt", ios::app);
       myfile << variQueue[0];
       myfile << endl;

	ofstream meanFile;
	meanFile.open("meanFile.txt", ios::app);
	meanFile << mean;
	meanFile << endl;


	ofstream dPosFile;
        dPosFile.open("dPosFile.txt", ios::app);
        dPosFile << dPosition;
        dPosFile << endl;

	ofstream leftFile;
        leftFile.open("leftFile.txt", ios::app);
        leftFile << left;
        leftFile << endl;

	ofstream rightFile;
        rightFile.open("rightFile.txt", ios::app);
        rightFile << right;
        rightFile << endl;

*/
	cout << "counter= " << _counter << endl;
	int v = 0; 
	cout << "Variance= " << variQueue[0] << endl;
        if (variQueue[0] > mean && variQueue[0] > 0.016  && k > size && (_counter > 100||numberOfLaneChanges == 0) )  //dPosition < 0
        {
//		 cout << mean << " , " << variQueue[0] << endl;

//                 putText(graph_v, "Lane Change" , cvPoint(30,200),
  //                                      FONT_HERSHEY_COMPLEX_SMALL, 3.0, cvScalar(0,0,255), 2, CV_AA);

		
	
		 _isLaneChange = true;
		cout << "LANE CHANGE " << dPosition << endl;
           
//		 cv::namedWindow("Visualizing Lateral Distance", CV_WINDOW_AUTOSIZE);
//	        cv::imshow("LANE CHANGE", graph);

		imwrite( "./images/Gray_Image.jpg", graph_v );
 
	//	cvWaitKey(0);
        }
//	cvWaitKey(0);

	if (k==2) cvWaitKey(0);

	if (variQueue[0]<=0.016)
	{
		stability = log(1/variQueue[0]);
		stabQueue.push_front(stability);
	


	}
	while(stabQueue.size()>2000)    // 4000 was used for stability and 10000 18000 is used for avg stability. 
	{
		stabQueue.pop_back();

	}
	double sum = 0;
	
        for (int it = 0; it < stabQueue.size() ; it++ ) {
		sum = sum + stabQueue[it];
	}

	double avg_stability = sum/stabQueue.size();
 
	ofstream stabilityFile;
        stabilityFile.open("stabilityFile.txt", ios::app);
        stabilityFile << avg_stability;
        stabilityFile << endl;



	///////////////////////////////////////////////////////////////////////////////////////////
	if (k>4)// to ignore the first few frames 
	{

		timeDriven = timeDriven + timeStamp; 
		if (_isLaneChange == true)
		{
			bPrevious = true;
		}
		else if (_isLaneChange == false && bPrevious == true)
		{
			bPrevious = false; 
			numberOfLaneChanges++; 
			timeQueue.push_front(timeStamp);
			cout << "in the else if" << endl;
			_counter = 0;			
		}
		_counter++;
		cout << "number of lane changes: " << numberOfLaneChanges << endl;	
		cout << "size of the ququq: "	<< timeQueue.size() <<endl;

		cout << "the first element: " << timeQueue[0] << endl;
		if (timeQueue.size()>0)
			cout << "the last element: "  << timeQueue[timeQueue.size()-1] << endl;
	
		while ((timeQueue.size()>0) && ((timeQueue[timeQueue.size()-1]-timeQueue[0] ) > 2.62974e9)) //2.62974e5 
		{
			timeQueue.pop_back();
			numberOfLaneChanges--;
			cout << "in the while loop" << endl;
		}
	
        	long double laneChangeFrequency = numberOfLaneChanges*1000/(timeStamp*60);
		cout << numberOfLaneChanges*1000 << endl;
		cout << timeStamp*60 << endl;
        	cout << "lane change frequency=   " << laneChangeFrequency << endl;
		cout << numberOfLaneChanges*1000/timeStamp*60 << endl;


		ofstream laneChangeFrequencyFile;
        	laneChangeFrequencyFile.open("laneChangeFrequencyFile.txt", ios::app);
        	laneChangeFrequencyFile << laneChangeFrequency;
        	laneChangeFrequencyFile << endl;


	}

	return _isLaneChange;

}


double DriverBehaviorAnalysis::getLaneChangeSpeed(bool isLaneChange)
{

	if (isLaneChange == false && sentinel > 0)
	{
	//	cout << "frameCount:  " << frameCount << "sentinel: " << sentinel << endl; 
		sentinel = 0;  
		return frameCount;
 
	}
	else if (isLaneChange == false && sentinel == 0)
	{
	//	  cout << "frameCount:  " << frameCount << "sentinel: " << sentinel << endl;

		frameCount = 0; 
		return frameCount; 
	}
	else if (isLaneChange == true)
	{
	//	  cout << "frameCount:  " << frameCount << "sentinel: " << sentinel << endl;

		frameCount = frameCount + 1;
		sentinel++;  
		return 0; 
	}
	
	

}


void DriverBehaviorAnalysis::setData(int frameNo, long long timeStamp, double lateralDistance , double laneChangeSpeed)
{

	ofstream timeStampFile;
        timeStampFile.open("timeStampFile.txt", ios::app);
        timeStampFile << timeStamp;
        timeStampFile << endl;

	cout << "f," << frameNo << "," << timeStamp << "," << lateralDistance << "," << laneChangeSpeed << endl; 

	 
	
}
