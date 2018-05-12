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

static const char* DIVIDER ;  // 100 Hz
string serial_device;
void usage(const char *prog);
void read_serial_devices_from_command_line(std::vector<std::string>& args, std::vector<std::string>& serial_device_list);
float angle_diff(float new_a, float old_a);
int syn_read_ascii(const char* serial_device, int baudrate, float euler[][6000],float acc[][6000],double calib[3]);
void handle_error(const char* error_msg);
void calibration(const char* serial_device,int baudrate,double calib[3]);
};