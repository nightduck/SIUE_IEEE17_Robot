//amg.h	(acceleromter, magnetometer, gyroscope)
//This is for the BNO055

#define DEV	0x29

extern short defaultHeading;	//The angle that is defined as "forward"
extern int handle;		//The handle for the i2c device

int initAMG(); //Returns handle
void setIMU(); //Sets to IMU fusion mode
void setRaw(); //Sets to non-fusion mode
short getOrientation();	//Returns fixed point (decimal is 4 bits) of the heading
void readMag();
void readData();
void readDataCSV();
void printData();
