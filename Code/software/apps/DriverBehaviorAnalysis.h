#ifndef DriverBehaviorAnalysis_H
#define DriverBehaviorAnalysis_H

using namespace std;

class DriverBehaviorAnalysis
{

public:
	DriverBehaviorAnalysis();
	double getMean(double data[]);
	double getVariance(double data[]);
	void plotGraph(double dPosition);
	bool detectLaneChange(double dPosition, double left, double right, long long timeStamp);
	double getLaneChangeSpeed(bool isLaneChange);


	void setData(int frameNo, long long timeStamp, double lateralDistance, double laneChangeSpeed);

};

#endif

