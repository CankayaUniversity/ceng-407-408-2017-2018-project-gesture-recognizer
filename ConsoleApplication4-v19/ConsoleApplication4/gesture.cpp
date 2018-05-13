#include "sensor.h"
#include "gesture.h"
#ifdef WIN32
#define NOMINMAX
#undef min
#undef max
#endif
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
#define Count 15
#define sample 5
gesture::gesture(double calibration[3], string serial_device) {
	sensor Sensor(serial_device);
	int samp_count = 0;
	if (calibration[0] == 0 && calibration[1] == 0 && calibration[2] == 0)
	{
		cout << "Need to calibration.Please make calibration first! You can achieve this by runnig the training mode for the first time !\n";
		exit(0);
	}
	samp_count = Sensor.syn_read_ascii(Sensor.BAUDRATE, euler, acc,calibration) + 1;
	duration = ((double)samp_count) / 100;

	for (int j = 0; j < 3; j++) {
		lin_intra(Count, samp_count, (featureVec + j * Count), euler[j]);
		lin_intra(Count, samp_count, (featureVec + Count * (j + 3)), acc[j]);
	}
	//featureVec[Count*6] = duration;
	norm(featureVec);
}
gesture::gesture(int id, double calibration[3], string serial_device) {
	for  (int i = 0;  i < Count*6;  i++)
	{
		featureVec[i] =0 ;

	}//initiliazing the  feature array
	gest_id = id;
	if(calibration[0]==0 && calibration[1]==0 && calibration[2]==0)
		//calibration deðerlerini al 
	{
		gesture_calib(calibration,serial_device);

	}
	calib[0] = calibration[0];
	calib[1] = calibration[1];
	calib[2] = calibration[2];
}

gesture::gesture(double *feature, float distance, int id) {
	copy(feature,  feature+ (6 * Count), featureVec);
	dist = distance;
	gest_id = id;

}
void gesture::gesture_calib(double calibration[3],string serial_dev) {

	cout << "Please dont move the sensor for the calibration.Enter 4 for the calibration\n";
	string check;
	cin >> check;
	if (check == "4")
	{
		sensor Sensor(serial_dev);
		Sensor.calibration(Sensor.BAUDRATE, calibration);
		
	}
}
bool gesture::saveGest() { 
	try {
		json j;
		ifstream f("gesture.json");
		if (f.peek() != std::ifstream::traits_type::eof())
		{
			f >> j;
		}
		f.close();
		string s = "gesture";
		string num = to_string(gest_id);
		s = s + num;
			
			j[s]["id"] = gest_id;
			j[s]["distance"] = dist;
			j[s]["featureVec"] = vector<double>(featureVec, featureVec + Count*6);
			ofstream f2("gesture.json");
			f2 << j;
			f2.close();
		
		
		return true;
	}
	catch (...) {
		return false;
	}
}

void gesture::detect(string serial_device) {
	sensor Sensor(serial_device);
	double euler_sample_count = 0; 
	float euler[sample][3][6000];
	float accel[sample][3][6000];
	double feature_arr[sample][6 * Count];
	double avg_duration = 0;
	int samp_count[sample];

	cout << "Detect mode is on!!!\n";
	printf("You should enter your gesture at least %d times\n",sample);
	//cout << "You should enter your gesture at least "+sample+" times\n";
	string check;
	for (int i = 0; i < sample; i++)
	{
		cout << "Press 4 to start your gesture\n";
		cin >> check;
		
		if (check=="4")
		{
			//taking sensor values
			euler_sample_count = Sensor.syn_read_ascii(Sensor.BAUDRATE, euler[i], accel[i],calib) + 1;

			printf("\n-----------------------------end-----------------------------------of----------------------------------%dth gesture.\n", i + 1);


			samp_count[i] = euler_sample_count;
			avg_duration += (euler_sample_count / 100); //total duration.

		}
		
	
	}

	//avg duration training modundaki gesturelarda duration da tutuluyor.
	avg_duration = avg_duration / sample; //avg duration.
	duration = avg_duration;//saved


	//int med = findmedian(samp_count);
	//featureVec_sample_count = Count;
	double interpolated[Count*6];
	for (int i = 0; i < sample; i++)
	{
		for (int j = 0; j < 3; j++) {
			lin_intra(Count, samp_count[i], (interpolated + j * Count), euler[i][j]);
			lin_intra(Count, samp_count[i], (interpolated + Count * (j + 3)), accel[i][j]);
		}

		//adding each interpolated array to the feature vector
		for (int i = 0; i < Count * 6; i++)
		{
			featureVec[i] += interpolated[i];
		}
		
		//holds the feature vector of each sample...
		copy(interpolated, interpolated + Count*6, feature_arr[i]);
		//feature_arr[i][Count*6] = ((double)samp_count[i] / 100);
		float m = 0;
		int sign = 0;
		for (int i = 0; i < Count * 3; i++)
		{
			m = featureVec[Count * 3 + i];
			if (m < 0) sign = -1;
			else if (m >= 0) sign = 1;
			m = abs(m);
			featureVec[Count * 3 + i] = sign * log(1 + m);

		}

		norm(feature_arr[i]);

	}
	
	for (int i = 0; i <Count*6; i++)
	{
		featureVec[i] = featureVec[i] / 5;
		
	}
	
	//featureVec[Count*6] =avg_duration ;
	norm(featureVec);

	float max = 0;
	float mean = 0;
	float distance = 0;
	for (int i = 0; i < sample; i++)
	{
		distance = calcDistance(feature_arr[i]);
		if (max < distance) max = distance;
		mean += distance;

	} 
	mean = mean / sample;
	//dist = max + 0.5*mean;
	dist = 1.5*mean;   
	printf("\nThe dist for the gesture is %f\n", dist);


	


}
		


void gesture::lin_intra(int med, int samp_count, double *featureVec, float euler[6000]) {
	double  freq = (double)samp_count / med;
	double d_start;

	double d_finish = 0;
	double d1;
	double d2;
	double value;
	int euler_correspond;
	int k;
	for (int i = 0; i < med; i++)
	{
		//feature vector deki i. elemanýn baþlangýç ve bitiþ hesaplamasý
		d_start = d_finish;
		d_finish = (i + 1)* freq;



		//n1==n2 //d1==0 &&d2==0
		if (freq == 1) { copy(euler, euler + med, featureVec); } //ayný uzunluktalarsa birini diðerine kopyala
		else if (freq < 1) {

			//feature vectorde d1 ve d2 uzunluklarý hesaplamasý
			d1 = ceil(d_start) - d_start;
			d2 = d_finish - floor(d_finish);


			if (d1 > freq || d2 > freq) {//eðer yeni oluþturulan arrayin bir elemaný original arraydaki bir elemanýn tamamen içinde kalýyorsa
				euler_correspond = floor(d_start);
				featureVec[i] = euler[euler_correspond];
			}

			else if (ceil(d_start) == floor(d_finish)) //arada baþka eleman yoksa. (+ ortadaki deðerlerde bu durumu saðlamaktadýr ama onlarýn 
													   //kontrolü önce yapýldýðý için buraya girmiceklerdir)
			{
				if (i == 0) featureVec[0] = euler[0]; //eðer freq<1 ise ilk elemanlarý ayný olucaktýr
				else { //diðer durumlarda bir ortalama deðer hesabý yapýlcaktýr.
					euler_correspond = ceil(d_start);


					value = (d1*euler[euler_correspond - 1] + d2 * euler[euler_correspond]) / (d1 + d2);

					featureVec[i] = value;

				}
			}
		}
		else if (freq > 1) {//freq>>1
			double numerator = 0;
			double denumerator = 0;


			if (i == 0) {//d_start=0 , dolayýsý ile sadece d_finishe kadar elemanlarý direk alýyoruz.
				k = 2;
				numerator += euler[0];
				denumerator++;
				while (d_finish >= k) {
					numerator += euler[k - 1];
					denumerator++;
					k++;
				}
				d1 = (1 + d_finish) - k;
				numerator = numerator + d1 * euler[k - 1];
				denumerator += d1;
				value = numerator / denumerator;
				featureVec[i] = value;
			}

			else {
				//baþlangýç noktasý
				k = ceil(d_start);
				d1 = k - d_start;
				numerator = numerator + d1 * euler[k - 1];
				denumerator += d1;
				k++;
				//ortada kalan elemanlar
				while (d_finish > k) {
					numerator += euler[k - 1];
					denumerator++;
					k++;
				}
				//son eleman
				d2 = (1 + d_finish) - k;
				numerator = numerator + d2 * euler[k - 1];
				denumerator += d2;
				value = numerator / denumerator;
				featureVec[i] = value;









			}



		}


	}


}

	
float gesture::calcDistance(double feature[90]) {

double dif[Count*6]={0};
	float distance =  0 ;
		
			for (int i = 0; i < Count*6; i++)
			{
				dif[i] = abs(feature[i] - featureVec[i]);

			}
			distance=pow_sqr(dif);
		return distance;
	}
float gesture::pow_sqr(double *dif)
{
	float distance = 0;
	for (int j = 0; j < Count * 6 ; j++) {
		distance += pow(dif[j], 2);
	}
	distance = sqrt(distance);
	return distance;
}
void gesture::norm(double *feature) {
	float distance = pow_sqr(feature);
	for (int i = 0; i <Count*6; i++)
	{
		feature[i] = feature[i] /distance;
	}
}
void gesture::compare(int train_gest_count, int delete_count) {
	float dist_test_gest = 0;



	//test.json  dan train gesturelarýný geri yüklüyoruz..
	vector<gesture> Train_Gest;

	double feature[Count * 6] = { 0 };

	int id;
	float distance;
	float dist_to_train;
	json j;
	ifstream f("gesture.json");
	if (f.peek() == std::ifstream::traits_type::eof())
	{
		cout << "No train mode gestures.To use test mode, you should save the gesture in the train mode/n";
		exit(0);
	}
	else {
		f >> j;
		f.close();
		for (int i = 1; i <= (train_gest_count + delete_count); i++)
		{
			string s = "gesture" + to_string(i);

			try { id = j[s]["id"]; }
			catch (...) {
				continue;
			}
			for (int k = 0; k < 6 * Count; k++)
			{
				feature[k] = (double)j[s]["featureVec"][k];
			}

			distance = j[s]["distance"];
			Train_Gest.push_back(gesture(feature, distance, id));
		}

		struct Candidates *cand = (struct Candidates*)malloc(train_gest_count * sizeof(struct Candidates));
		for (int i = 0; i < train_gest_count; i++)
		{
			cand[i].val = 0;
		}
		int count_of_candidates = 0;
		for (int i = 0; i < train_gest_count; i++)
		{
			gesture g = Train_Gest.at(i);
			dist_to_train = calcDistance(g.featureVec);
			printf("%lf\n", dist_to_train);
			if (g.dist > dist_to_train) {
				cand[i].val = 1; cand[i].dist = dist_to_train; count_of_candidates++;
				cand[i].id = g.getId();
			}
		}
		if (count_of_candidates == 1) {
			cout << "A match is found for your gesture\n";
			for (int i = 0; i < train_gest_count; i++)
			{
				if (cand[i].val == 1)		printf("The id of the gesture is %d and the distance from train feature vector is %f\n", cand[i].id, cand[i].dist);
			}
		}
		else if (count_of_candidates == 0) { cout << "No match for your gesture\n"; }
		else { //at least 2
			struct Candidates min;
			min.dist = 100000;
			for (int i = 0; i < train_gest_count; i++)
			{
				if (cand[i].val == 1)
					/*if (min.dist > cand[i].dist)
					{
						min.dist = cand[i].dist;
						min.id = cand[i].id;
					}*/
					printf("The id of the gesture is %d and the distance from train feature vector is %f\n", cand[i].id, cand[i].dist);
			}

			/*printf("The id of the gesture is %d and the distance from train feature vector is %f\n", min.id, min.dist);*/
		}
	}
}
