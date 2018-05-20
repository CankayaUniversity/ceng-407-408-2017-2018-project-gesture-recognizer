## ceng-407-408-project-gesture-recognizer
ceng-407-408-project-gesture-recognizer created by GitHub Classroom

### Compilation / Installation Guide

To run the application required libraires are given below:
- json.hpp (from https://github.com/nlohmann/json)
- myahrs_pluss.hpp (from https://github.com/withrobot/myAHRS_plus)

This application can run on only Windows.

To work with this application, the user must have myAHRS Plus sensor.

## User Manual

This application uses command line interface.To run this application, the user should open the command line interface.Move to the directory that includes the application or user can enter the directory as path.

Aplication takes tree parameter : 
- Path 
- Method name 
- The Serial Port of the sensor

Path can be the full directory or the name of application if the current directory is the one that has the application.

Method name can be train , test and delete.

The serial port name will be something like COM3, COM4,COM5.User should check the serial port every time he/she connects sensor to the computer.It can be checked from Computer Management >> Device Manager >> Ports >> Usb Serial Device( Port name is written here) in Windows 10.

[![image](https://i.hizliresim.com/8z0XBd.png)](https://hizliresim.com/8z0XBd)
