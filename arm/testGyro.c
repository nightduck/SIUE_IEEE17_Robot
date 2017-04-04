#include "bbbLib.h"
#include <unistd.h>
#include <stdio.h>
#include "amg.h"

void testGyro(int n) {
    int i;
    int handle = initAMG();
    unsigned char addr = 0x08;
    unsigned char buff[18];

    for(i=0; i < n; i++) {
        //Bulk read in 18 bytes for the acc, mag, gyro
        i2c_write_read(handle, DEV, &addr, 1, DEV, buff, 18);
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
    
        printf("Yaw: %hd\n", getOrientation());
        usleep(500000);
        printf("\n");
    }

}

//void testGyro() {
//    int i;
//    unsigned char buff[128];
//    unsigned char yawAddr = 0x1A;
//    unsigned char rollAddr = 0x1C;
//    unsigned char pitchAddr = 0x1E;
//    unsigned char idAddr = 0x00;
//    unsigned char accXAddr = 0x08; 
//    unsigned char accYAddr = 0x0A;
//    unsigned char accZAddr = 0x0C;
//    unsigned char magXAddr = 0x0E;
//    unsigned char magYAddr = 0x10;
//    unsigned char magZAddr = 0x12;
//    unsigned char powerAddr = 0x3E;
//
//
//    //Initialization
//    int handle = i2c_open(1, DEV);
//
//    //Change accelerometer to 2g
//    buff[0] = 0x08;
//    buff[1] = 0x1C;
//    i2c_write(handle, buff, 2);
//
//    //Change to NDOF mode
//    buff[0] = 0x3D;
//    buff[1] = 0x0C;
//    i2c_write(handle, buff, 2);
//    usleep(7000);	//Wait 7ms
//
//    //Change units to mg, dps, degrees, and celsius
//    buff[0] = 0x3B;
//    buff[1] = 0x81;
//    i2c_write(handle, buff, 2);
//
//    for(i = 0; i < 10; i++) {
//        //Bulk read in 18 bytes for the acc, mag, gyro
//        i2c_write_read(handle, DEV, &accXAddr, 1, DEV, buff, 18);
//        printf("Acc\n");
//        printf("  x %hd\n",((short)buff[1] << 8) + (short)buff[0]);
//        printf("  y %hd\n",((short)buff[3] << 8) + (short)buff[2]);
//        printf("  z %hd\n",((short)buff[5] << 8) + (short)buff[4]);
//        printf("Mag\n");
//        printf("  x %hd\n",((short)buff[7] << 8) + (short)buff[6]);
//        printf("  y %hd\n",((short)buff[9] << 8) + (short)buff[8]);
//        printf("  z %hd\n",((short)buff[11] << 8) + (short)buff[10]);
//        printf("Gyro\n");
//        printf("  x %hd\n",((short)buff[13] << 8) + (short)buff[12]);
//        printf("  y %hd\n",((short)buff[15] << 8) + (short)buff[14]);
//        printf("  z %hd\n",((short)buff[17] << 8) + (short)buff[16]);
//
//        i2c_write_read(handle, DEV, &yawAddr, 1, DEV, buff, 2);
//        printf("Yaw: %hd\n",((short)buff[1] << 8) + (short)buff[0]);
//
//        printf("\n");
//        usleep(500000);
//    }
//
//    printf("Calibrating acc, mag, and gyro\n");
//    //Zero out acc, mag, and gyro
//    i2c_write_read(handle, DEV, &accXAddr, 1, DEV, buff+1, 18); //Read acc, mag, & gyro data)
//    printf("Buffer\n");
//    for(i = 0; i <= 18; i++) printf(" 0x%x\n",buff[i]);
//    buff[0] = 0x55;	//Offset address for acc
//    i2c_write(handle, buff, 7);
//    buff[6] = 0x5B;	//Offset address for mag
//    i2c_write(handle, buff+6, 7);
//    buff[12] = 0x61;	//Offset address for gyro
//    i2c_write(handle, buff+12, 7);
//    while(1) { 
//        usleep(500000);
//        
//        //Bulk read in 18 bytes for the acc, mag, gyro
//        i2c_write_read(handle, DEV, &accXAddr, 1, DEV, buff, 18);
//        printf("Acc\n");
//        printf("  x %hd\n",((short)buff[1] << 8) + (short)buff[0]);
//        printf("  y %hd\n",((short)buff[3] << 8) + (short)buff[2]);
//        printf("  z %hd\n",((short)buff[5] << 8) + (short)buff[4]);
//        printf("Mag\n");
//        printf("  x %hd\n",((short)buff[7] << 8) + (short)buff[6]);
//        printf("  y %hd\n",((short)buff[9] << 8) + (short)buff[8]);
//        printf("  z %hd\n",((short)buff[11] << 8) + (short)buff[10]);
//        printf("Gyro\n");
//        printf("  x %hd\n",((short)buff[13] << 8) + (short)buff[12]);
//        printf("  y %hd\n",((short)buff[15] << 8) + (short)buff[14]);
//        printf("  z %hd\n",((short)buff[17] << 8) + (short)buff[16]);
//
//        i2c_write_read(handle, DEV, &yawAddr, 1, DEV, buff, 2);
//        printf("Yaw: %hd\n",((short)buff[1] << 8) + (short)buff[0]);
//
//        printf("\n");
//    }
//}
