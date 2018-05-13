#include <string>
#include <stdio.h>
#include <stdlib.h>
//#include <vector>
//#include <map>

#include "myahrs_plus.hpp"
using namespace WithRobot;
using namespace std;


class sensor{
public:

 static const int BAUDRATE ;
static const char* DIVIDER ;// 100 Hz
sensor(string serial);

float angle_diff(float new_a, float old_a);
int syn_read_ascii(int baudrate, float euler[][6000],float acc[][6000],double calib[3]);
void handle_error(const char* error_msg);
void calibration(int baudrate,double calib[3]);
private:
	string serial_device;
};