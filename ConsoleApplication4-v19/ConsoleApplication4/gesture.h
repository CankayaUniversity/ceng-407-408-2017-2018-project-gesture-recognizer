#include "stdafx.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include<math.h>
#include <fstream>
#include <string>
#include <stdlib.h>
//using namespace std;

class gesture
{
public:
	gesture(double calibration[3],std::string serial_device);
	gesture(int id, double calibration[3],std::string serial_device);
	
	gesture(double *featureVector,float distance,int id);
	void gesture_calib(double calibration[3],std::string serial_device);
	bool saveGest(); //saveGest is responsible to store the data related with gesture 
	void compare(int train_gest_count,int delete_count); //Compare is responsible to classify the gesture by comparing template and the gesture.It returns the id number of the 
	//gesture if it recognized.The gesture numbers starts with 1.If no match it returns 0.
	void detect(std::string serial_device);// Detect is responsible for creating template.
	//int findmedian(int *samp_count); //finds median of the sample_counts
	float calcDistance(double feature[90]);
	//void intra(int med, int samp_count, double featureVec[][6000], float euler[][6000]);
	int getId() { return gest_id; }
	void lin_intra(int med, int samp_count, double *featureVec, float euler[6000]);
	float pow_sqr(double *dif);
	void norm(double *feature);

private:
	float euler[3][6000];
	float acc[3][6000];
	int gest_id;
	double duration;
	double featureVec[90] ;
	float dist;
	double calib[3];
	struct Candidates {
		int val;
		float dist;
		int id;
	}candidates;
};

