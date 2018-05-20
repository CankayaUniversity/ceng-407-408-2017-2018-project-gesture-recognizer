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

__Example__
 $ C:\Users\bengi\OneDrive\Belgeler\GitHub\ceng-407-408-project-gesture-recognizer\ConsoleApplication4-v19\x64\Debug\ConsoleApplication4.exe train COM3
 
 According to the mode that user enter before run the application, application will make different operations.If the application is run for the first time, in the train mode it will make an calibration and user will record train gestures.In test mode user made an gesture and system will compare if its exist between the recorded gesture.Delete mode requires the id number of the gesture which is wanted to be deleted.If user enters and invalid number, program gives error.If the gesture with the id number exist, it will be deleted.
