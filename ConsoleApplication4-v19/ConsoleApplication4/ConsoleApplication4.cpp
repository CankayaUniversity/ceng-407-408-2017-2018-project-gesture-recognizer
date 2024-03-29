#include "stdafx.h"
#include "gesture.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "sensor.h"
#include <vector>
#include <map>
#include <fstream>

#ifdef WIN32
#define NOMINMAX
#undef min
#undef max
#endif
#include "json.hpp"

using namespace std;
using namespace nlohmann;
//prototypes
void usage(const char *prog);

void addGesture(int *count,double *calib,int delete_count,string serial_device);
void testGesture(int train_gest_count,int delete_count,double *calib,string serial_device);
boolean deleteGesture();

//bool initApi();
//bool terminateApi();


//main

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[]) {
	std::vector<std::string> args;
	for (int i = 0; i < argc; i++) {
		CString cstring(argv[i]);
		std::string s(CT2CA(cstring.operator LPCWSTR()));
		args.push_back(s);
	}
#else
int main(int argc, char* argv[]) {
	std::vector<std::string> args;
	for (int i = 0; i < argc; i++) {
		args.push_back(std::string(argv[i]));
	}
#endif


	if (argc < 3) {
		printf("ERROR. need more arguments\n");
		usage(args[0].c_str());
		exit(1);
	}

	std::vector<std::string> serial_device_list;
	for (size_t i = 2; i < args.size(); i++) {
		serial_device_list.push_back(args[i]);
	}

	//serial device bilgisini alıyoruz
	string serial_device;
	serial_device = serial_device_list[0];
	//serial_device = "COM5";
	//trainde kaydedilmiş gesture sayısı tespiti
	int train_gest_count;
	int delete_count;
	double calibration[3] = {0};
	ifstream f("count.json");
	if (f.peek() == std::ifstream::traits_type::eof())
	{
		train_gest_count = 0;
		delete_count = 0;
	}
	else {
		json j2;
		f >> j2;
		delete_count = j2["delete"];
		train_gest_count = j2["count"];
		calibration[0] = j2["roll"];
		calibration[1] = j2["pitch"];
		calibration[2] = j2["yaw"];
	}
	f.close();

	string str1("train");
	string str2("test");
	string str3("delete");
	//if (str1.compare(args[1].c_str())==0) { 
	if (str1.compare(args[1].c_str()) == 0) {
		cout << "train mode on \n";
	char c='e';
	do {
		//length of gesture in the euler array ( 0-lastmovement)
		addGesture(&train_gest_count,calibration,delete_count,serial_device);
		cout << "To continue adding new gestures press c.If you are done with training press e.\n";
		cin >> c;
		
	} while (c == 'c');
	



	json j;
	j["roll"]=calibration[0] ;
	j["pitch"]=calibration[1];
	j["yaw"]=calibration[2];
	j["count"] =train_gest_count;
	j["delete"] =delete_count ;
	ofstream f("count.json");
	f << j;
	f.close();

	} //training mode= add gesture =detect&save gesture
	else if (str2.compare(args[1].c_str()) == 0) {
	//else if (str2.compare("test") == 0) {
		cout << "testing mode on \n";
		char c = 'e';
		do {
			testGesture(train_gest_count,delete_count,calibration,serial_device);

			cout << "To continue testing new gestures press c.If you are done with training press e.\n";
			cin >> c;


		} while (c == 'c'); // testing mode =take values from sensor&&create gesture&&compare with the list of gestures(from the training).
	}
	//else if (str3.compare("delete") == 0) {
	else if (str3.compare(args[1].c_str()) == 0) {
		boolean control;
		control=deleteGesture();
		if(control)
		cout << "Your gesture is deleted succesfully";
	}
	else { cout << "Mode name can be \"train\" , \"test\" or \"delete\" \n "; exit(1); }
   //else if () {} //settings= fonksyonlara ulaşım.
	//Sensor.stop_com();
	return 0;
}







//functions

void addGesture(int *train_gest_count,double *calibration, int delete_count,string serial_device){
	try
	{
		gesture G(++(*train_gest_count)+delete_count,calibration,serial_device);	
		G.detect(serial_device);
		if(G.saveGest()!=false) cout<<"Gesture is created and saved succesfully\n";
	}
	catch (exception& e)
	{
		cout << "An error happened when the gesture was created.\n";
	}


}


void testGesture(int train_gest_count,int delete_count,double *calibration,string serial_device) {
	cout << "Please make a gesture\n";
	gesture G(calibration,serial_device);
	G.compare(train_gest_count,delete_count);
}






boolean deleteGesture() {
	int count;
	int delete_count;
	int gest_id;
	json j, j2, j3;
	ifstream f("count.json");
	ifstream f2("test.json");
	if (f.peek() == std::ifstream::traits_type::eof() || f2.peek() == std::ifstream::traits_type::eof())
	{
		cout << "There is no gestures in your files.Please add some gestures first before delete them\n";
		return false;
	}
	f2 >> j2;
	f >> j;
	f2.close();
	f.close();

	count = j["count"];
	delete_count = j["delete"];
	cout << "Enter the id of the gesture you wanna delete\n";
	cin >> gest_id;
	if (gest_id > count || gest_id <= 0) {
		cout << "There is no gesture with that id.\n";
		return false;
	}


	int counter = 0;
	int id;
	for (int i = 1; i <= count; i++)
	{
		string s = "gesture";
		string num = to_string(i);
		s = s + num;
		if (i != gest_id)
		{
			j3[s] = j2[s];

		}
		else
		{
			try
			{
				id = j2[s]["id"];
				counter++;
			}
			catch (...) {
				cout << "This gesture is deleted before \n";
				return false;
			}
		}
	}
	ofstream f3("test.json");
	f3 << j3;
	f3.close();


	if (counter == 1)
	{
		json j4;
		ifstream f4("count.json");
		count--;
		delete_count++;
		f4 >> j4;
		f4.close();
		j4["count"] = count;
		j4["delete"] =delete_count;
		ofstream f5("count.json");
		f5 << j4;
		f5.close();
	}
	return true;
}

void usage(const char *prog) {

	printf("\n usage : %s \"Mode name\"  \"serial port list \"  \n\t ex) $ %s train COM3\n\n", prog, prog);
	printf("\tMode name can be \"train\" or \"test\" or \"delete\"\n"); //testing is explained as recognition mode in the SRS 
																		// There could be a third mode to reach the functions.Ex: Settings
}

//initApi: This function is responsible to initialize the api .
//bool initApi(){}
//terminateApi: This function is responsible to terminate the api.
//bool terminateApi(){}