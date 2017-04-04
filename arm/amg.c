//amg.c
#include <stdlib.h>
#include "amg.h"
#include "bbbLib.h"

short defaultHeading;	//The default orientation
int handle;		//The handle for the I2C device

int initAMG() {
    unsigned char buff[2];
    handle = i2c_open(1, DEV);

    //Change accelerometer to 2G
    //buff[0] = 0x08;
    //buff[1] = 0x1C;
    //i2c_write(handle, buff, 2);
    
    //Change to configuration mode (in event someone was playing with AMG before program started)
    buff[0] = 0x3D;
    buff[1] = 0x00;
    i2c_write(handle,buff,2);
    usleep(20000);    //Wait at least 19ms

    //Calibrate offset values
    unsigned char calibValues[0x17] = {0x55,0x0C,0,0xF9,0xFF,0xF5,0xFF,0xFB,0xFE,0x09,0x00,0xA8,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xE8,0x03,0xF5,0};
    i2c_write(handle, calibValues, 0x17); 

    //Change to operation mode;
    buff[0] = 0x3D;
    buff[1] = 0x08;  //IMU 
    //buff[1] = 0x09;  //Compass
    //buff[1] = 0x0A;  //M4G
    //buff[1] = 0x0B;  //NDOF_FMC_Off
    //buff[1] = 0x0C;  //NDOF

    i2c_write(handle, buff, 2);
    usleep(8000);	//Wait at least 7ms

    //Change units to mg, dps, degrees, and celsius
    buff[0] = 0x3B;
    buff[1] = 0x81;
    i2c_write(handle, buff, 2);

    usleep(1000000); //Wait a second

    defaultHeading = getOrientation();
    if (defaultHeading == 0) printf("Zero defaultHeading\n");
    return handle;
}

//Sets the device to IMU fusion mode
void setIMU() {
    unsigned char buff[2];
    
    //Change to configuration mode
    buff[0] = 0x3D;
    buff[1] = 0x00;
    i2c_write(handle,buff,2);
    usleep(20000);    //Wait at least 19ms
    
    //Change to IMU mode
    buff[1] = 0x08;
    i2c_write(handle, buff,2);
    usleep(100000);   //Wait at least 7ms
}

//Set the device to read raw data, no fusion mode
void setRaw() {
    unsigned char buff[2];
    
    //Change to configuration mode
    buff[0] = 0x3D;
    buff[1] = 0x00;
    i2c_write(handle,buff,2);
    usleep(20000);    //Wait at least 19ms
    
    //Change to raw data mode
    buff[1] = 0x07;
    i2c_write(handle, buff,2);
    usleep(50000);   //Wait at least 7ms
}

short getOrientation() {
    unsigned char buff[2];
    unsigned char addr = 0x1A;
    while (i2c_write_read(handle, DEV, &addr, 1, DEV, buff, 2) == -1) {
        delay_ms(1);
        printf("ERROR: AMG Read Failed!\n");
    }
    return ((short)buff[1] << 8) + (short)buff[0];
}

int isCalibrated() {
    unsigned char addr = 0x35;
    unsigned char buff;
    
    i2c_write_read(handle, DEV, &addr, 1, DEV, &buff, 1);

    printf("Calibrating...0x%x",buff);
    if (buff>>6 == 3) printf("Done\n"); else printf("\n");

    return ((buff>>6) & 0x03);
}

void readMag() {
    unsigned char addr = 0x0E;
    unsigned char buff[6];
    
    //Print mag
    i2c_write_read(handle, DEV, &addr, 1, DEV, buff, 6);
    printf("Mag\n");
    printf("  x %hd\n",((short)buff[1] << 8) + (short)buff[0]);
    printf("  y %hd\n",((short)buff[3] << 8) + (short)buff[2]);
    printf("  z %hd\n",((short)buff[5] << 8) + (short)buff[4]);
}

void readData() {
    unsigned char addr = 0x08;
    unsigned char buff[45];

    //Bulk read in 22 bytes for the data dump
    i2c_write_read(handle, DEV, &addr, 1, DEV, buff, 45);
    printf("Acc\n");
    printf("  x %hd\n",((short)buff[1] << 8) + (short)buff[0]);
    printf("  y %hd\n",((short)buff[3] << 8) + (short)buff[2]);
    printf("  z %hd\n",((short)buff[5] << 8) + (short)buff[4]);
    printf("Mag\n");
    printf("  x %hd\n",((short)buff[7] << 8) + (short)buff[6]);
    printf("  y %hd\n",((short)buff[9] << 8) + (short)buff[8]);
    printf("  z %hd\n",((short)buff[11] << 8) + (short)buff[10]);
    printf("Gyro\n");
    printf("  x %hd\n",((short)buff[13] << 8) + (short)buff[12]);
    printf("  y %hd\n",((short)buff[15] << 8) + (short)buff[14]);
    printf("  z %hd\n",((short)buff[17] << 8) + (short)buff[16]);
    printf("Orientation\n");
    printf("  head %hd\n",((short)buff[19] << 8) + (short)buff[18]);
    printf("  roll %hd\n",((short)buff[21] << 8) + (short)buff[20]);
    printf("  pitc %hd\n",((short)buff[23] << 8) + (short)buff[22]);
    printf("Quaternion\n");
    printf("  w %hd\n",((short)buff[25] << 8) + (short)buff[24]);
    printf("  x %hd\n",((short)buff[27] << 8) + (short)buff[26]);
    printf("  y %hd\n",((short)buff[29] << 8) + (short)buff[28]);
    printf("  z %hd\n",((short)buff[31] << 8) + (short)buff[30]);
    printf("Linear Accel\n");
    printf("  x %hd\n",((short)buff[33] << 8) + (short)buff[32]);
    printf("  y %hd\n",((short)buff[35] << 8) + (short)buff[34]);
    printf("  z %hd\n",((short)buff[37] << 8) + (short)buff[36]);   
    printf("Gravity Vector\n");
    printf("  x %hd\n",((short)buff[39] << 8) + (short)buff[38]);
    printf("  y %hd\n",((short)buff[41] << 8) + (short)buff[40]);
    printf("  z %hd\n",((short)buff[43] << 8) + (short)buff[42]);   
    printf("Temperature %hd\n", (short)buff[44]);
}

void readDataCSV() {
    unsigned char addr = 0x08;
    unsigned char buff[45];

    //Bulk read in 22 bytes for the data dump
    i2c_write_read(handle, DEV, &addr, 1, DEV, buff, 45);
    printf("%hd,",((short)buff[1] << 8) + (short)buff[0]);
    printf("%hd,",((short)buff[7] << 8) + (short)buff[6]);
    printf("%hd,",((short)buff[9] << 8) + (short)buff[8]);
    printf("%hd,",((short)buff[11] << 8) + (short)buff[10]);
    printf("%hd,",((short)buff[13] << 8) + (short)buff[12]);
    printf("%hd,",((short)buff[15] << 8) + (short)buff[14]);
    printf("%hd,",((short)buff[17] << 8) + (short)buff[16]);
    printf("%hd,",((short)buff[19] << 8) + (short)buff[18]);
    printf("%hd,",((short)buff[21] << 8) + (short)buff[20]);
    printf("%hd,",((short)buff[23] << 8) + (short)buff[22]);
    printf("%hd,",((short)buff[25] << 8) + (short)buff[24]);
    printf("%hd,",((short)buff[27] << 8) + (short)buff[26]);
    printf("%hd,",((short)buff[29] << 8) + (short)buff[28]);
    printf("%hd,",((short)buff[31] << 8) + (short)buff[30]);
    printf("%hd,",((short)buff[33] << 8) + (short)buff[32]);
    printf("%hd,",((short)buff[35] << 8) + (short)buff[34]);
    printf("%hd,",((short)buff[37] << 8) + (short)buff[36]);   
    printf("%hd,",((short)buff[39] << 8) + (short)buff[38]);
    printf("%hd,",((short)buff[41] << 8) + (short)buff[40]);
    printf("%hd,",((short)buff[43] << 8) + (short)buff[42]);   
    printf("%hd\n", (short)buff[44]);
}

void printData(int max) {
    int i;
    initAMG();

    while(!isCalibrated()) usleep(100000);

    for(i=0; i<max; i++) {
        readData();
        usleep(500000);
    }
}

