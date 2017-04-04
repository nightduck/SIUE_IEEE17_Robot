#include <stdio.h>
#include "ir.h"
#include "bbbLib.h"

int frontHandle;
int leftHandle;
int downHandle;

void initIR() {
//    frontHandle = i2c_open(1, FRONTIR);
    leftHandle = i2c_open(1, LEFTIR);
//    downHandle = i2c_open(1, DOWNIR);

    unsigned char buf[2];
    buf[0] = 0x35;
    buf[1] = 0x02;

    i2c_write(leftHandle, buf, 2);	//Set max distance to 128cm   

    buf[0] = 0x3F;
    buf[1] = 0x10;

    i2c_write(leftHandle, buf, 2);	//Turn on filter...thing...idk, rtfm (or ask Sean)

    return;
}

float getDist(unsigned int dev) {
    int handle;
    switch(dev) {
        case FRONTIR:
            handle = frontHandle;
            break;
        case LEFTIR:
            handle = leftHandle;
            break;
        case DOWNIR:
            handle = downHandle;
            break;
        default:
            printf("IR getDist, invalid device");
            return -1.0;
    }

    unsigned char buf[2];
    buf[0] = 0x5E;
    i2c_write_read(handle, dev, buf, 1, dev, buf, 2);

    return ((buf[0]*16 + buf[1])/162.56);
}
