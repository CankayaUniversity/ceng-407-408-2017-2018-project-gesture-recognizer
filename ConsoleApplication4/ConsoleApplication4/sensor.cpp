#include "stdafx.h"
#include "sensor.h"
 const int sensor:: BAUDRATE = 115200;
 const char*sensor:: DIVIDER = "1";
//functions

//useage: gives necessary information about programs usage.
void sensor:: usage(const char *prog) {

	printf("\n usage : %s \"Mode name\"  \"serial port list \"  \n\t ex) $ %s train COM3\n\n", prog, prog);
	printf("\tMode name can be \"train\" or \"test\" \n\n"); //testing is explained as recognition mode in the SRS 
															 // There could be a third mode to reach the functions.Ex: Settings
}  

void sensor::handle_error(const char* error_msg)
{
	fprintf(stderr, "ERROR: %s\n", error_msg);
	exit(1);
}

void sensor::read_serial_devices_from_command_line(std::vector<std::string>& args, std::vector<std::string>& serial_device_list)
{
	for (size_t i = 1; i<args.size(); i++) {
		serial_device_list.push_back(args[i]);
	}
}
void sensor::calibration(const char* serial_device,int baudrate, double calibration[3])
{
	MyAhrsPlus sensor;
	SensorData sensor_data;
	if (sensor.start(serial_device, baudrate) == false) {
		handle_error("start() returns false");
	}

	/*
	*  set binary output format
	*   - select Quaternion and IMU data
	*/
	if (sensor.cmd_binary_data_format("QUATERNION, IMU") == false) {
		handle_error("cmd_binary_data_format() returns false");
	}

	/*
	*  set divider
	*   - output rate(Hz) = max_rate/divider
	*/
	if (sensor.cmd_divider(DIVIDER) == false) {
		handle_error("cmd_divider() returns false");
	}

	/*
	*  set transfer mode
	*   - BC : Binary Message & Continuous mode
	*/
	if (sensor.cmd_mode("BC") == false) {
		handle_error("cmd_mode() returns false");
	}

	if (sensor.wait_data() == true) { // waiting for new data
									  // read counter
									  //	sample_count = sensor.get_sample_count();
									  // copy sensor data
		sensor.get_data(sensor_data);

		Quaternion& q = sensor_data.quaternion;

		DirectionCosineMatrix dcm;
		dcm.set(q);

		EulerAngle e = q.to_euler_angle();
		calibration[0] = e.roll;
		calibration[1] = e.pitch;
		calibration[2] = e.yaw;

	}

}
int sensor::syn_read_ascii(const char* serial_device, int baudrate, float euler[][6000],float acc[][6000],double calib[3])
{
	MyAhrsPlus sensor;
	SensorData sensor_data;
	int sample_count = 0;
	int lastMovement = 0;
	int countForStop = 0;


	float mg;
	/*
	* 	start communication with the myAHRS+.
	*/
	if (sensor.start(serial_device, baudrate) == false) {
		handle_error("start() returns false");
	}

	/*
	*  set binary output format
	*   - select Quaternion and IMU data
	*/
	if (sensor.cmd_binary_data_format("QUATERNION, IMU") == false) {
		handle_error("cmd_binary_data_format() returns false");
	}

	/*
	*  set divider
	*   - output rate(Hz) = max_rate/divider
	*/
	if (sensor.cmd_divider(DIVIDER) == false) {
		handle_error("cmd_divider() returns false");
	}

	/*
	*  set transfer mode
	*   - BC : Binary Message & Continuous mode
	*/
	if (sensor.cmd_mode("BC") == false) {
		handle_error("cmd_mode() returns false");
	}
		//Record for a minute
		while (sample_count < 6000) {
			if (countForStop == 300) {
			cout << "Gesture has finished.\n";
				lastMovement = sample_count - 300; //index of the last movement in the euler array. (sampleCount-300+1=array length)
				break;
			}//if the last 300 data is same, the gesture has finished.

			//ivme deðerleri ve euler açýlarý alýndý.Bu if'in içini deðiþtirme!


			if (sensor.wait_data() == true) { // waiting for new data
											  // read counter
											  //	sample_count = sensor.get_sample_count();
											  // copy sensor data
				sensor.get_data(sensor_data);

				Quaternion& q = sensor_data.quaternion;
				ImuData<float>& imu = sensor_data.imu;
				/*
				printf("%04d) Quaternion(xyzw)=%.4f,%.4f,%.4f,%.4f, Accel(xyz)=%.4f,%.4f,%.4f, Gyro(xyz)=%.4f,%.4f,%.4f, Magnet(xyz)=%.2f,%.2f,%.2f\n",
					sample_count,
					q.x, q.y, q.z, q.w,
					imu.ax, imu.ay, imu.az,
					imu.gx, imu.gy, imu.gz,
					imu.mx, imu.my, imu.mz);
					*/
				DirectionCosineMatrix dcm;
				dcm.set(q);

				EulerAngle e = q.to_euler_angle();
				mg = abs(1 - ((imu.ax) *(imu.ax) + (imu.ay) *(imu.ay) + (imu.az) *(imu.az)));
			//	printf(" euler angle from quaternion %s\n", e.to_string().c_str());
				//if (control==0) {

					//detect to first movement.Always your gesture starts at first element of the array. Index of start of movement is 0.
					if (sample_count == 0 && mg < 0.25) continue;
					else if(sample_count==0) cout << "Gesture has started.";
					
					
						acc[0][sample_count] = imu.ax;
						acc[1][sample_count] = imu.ay;
						acc[2][sample_count] = imu.az;


						euler[0][sample_count] = angle_diff(e.roll, calib[0]);
						euler[1][sample_count] = angle_diff(e.pitch, calib[1]);
						euler[2][sample_count] = angle_diff(e.yaw, calib[2]);


						//printf("roll_diff: %f\tpitch_diff: %f\tyaw_diff:%f\n",e.roll, e.pitch, e.yaw);
						//	printf("roll_diff: %f\tpitch_diff: %f\tyaw_diff:%f\n", euler[0][sample_count], euler[1][sample_count], euler[2][sample_count]);
							//printf("accel x:%f, accel y: %f accel z: %f\n",acc[0][sample_count], acc[1][sample_count], acc[2][sample_count]);

					//}


					

				if (sample_count >= 1) {

					
					if (mg < 0.25) {
						//printf("%f %f %f\n", imu.ax, imu.ay, imu.az);
						countForStop++;
					}
					else  countForStop = 0;

				}
				sample_count++;

			}
		}
		if (sample_count == 6000) lastMovement = 5999;
		/*
		* 	stop communication
		*/
		//sensor.stop();
		//printf("Your gesture start with first element of the array and continues until %dth element", lastMovement);
		sensor.stop();
		return lastMovement;

	}


	float sensor::angle_diff(float new_a, float calib) {
		float mutlak, angle_diff_2;
		float angle_diff = new_a - calib;
		if (angle_diff < 0)
			mutlak = angle_diff * (-1);
		else
			mutlak = angle_diff;

		if (mutlak > 360) {
			if (angle_diff< 0)
				angle_diff = angle_diff + 360;

			if (angle_diff> 0)
				angle_diff = angle_diff - 360;
			mutlak = mutlak - 360;

		}

		angle_diff_2 = 360 - mutlak;

		if (angle_diff_2 < mutlak) {
			if (angle_diff < 0)
				angle_diff = angle_diff_2;
			else if (angle_diff > 0)
				angle_diff = -angle_diff_2;
		}



		return angle_diff;
	}