#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "die.h"

extern int debug;

int initPWMcontr() {
    unsigned char buff[2];

    if(debug) printf("Initializing PWM controller\n");
    
    PWMhandle = i2c_open(1,0x42);

    //POTENTIAL_BUG: If the motor spontaneously turns on, put code here to write a 0 to registers 6-9

    //Read mode register
    buff[0] = 0;
    i2c_write_read(PWMhandle, 0x40, buff, 1, 0x40, &(buff[1]), 1);

    //Turn off bit 4 (the sleep bit)
    buff[1] = buff[1] & 0xEF;
    //DEBUGGING
    i2c_write(PWMhandle, buff, 2);

    //TODO: Recognize errors in I2C write and return non-zero values
    return 0;
}

int liftLid() {
    unsigned char buff[2];
    
    if(debug) printf("Moving arm down\n");

    //Move arm down
    buff[0] = 9;
    buff[1] = 4;
    i2c_write(PWMhandle, buff, 2);

    //Wait a second for it to pick up the lid
    delay_ms(999);
    delay_ms(999);
    delay_ms(999);

    //Hold
    buff[1] = 0;
    i2c_write(PWMhandle, buff, 2);
    delay_ms(500);

    if(debug) printf("Moving arm up\n");

    //Move arm back up
    buff[1] = 8;
    i2c_write(PWMhandle, buff, 2);

    delay_ms(999);
    delay_ms(999);
    delay_ms(500);

    buff[1] = 0;
    i2c_write(PWMhandle, buff, 2);

    //TODO: Recognize erros in I2C write and return non-zero values
    return 0;
}

int dieCount(){
    system("streamer -c /dev/video0 -b 16 -o /root/bot/arm/dice/capture.jpeg");
    delay_ms(999);
    int status = system("/root/bot/arm/dice/dice /root/bot/arm/dice/capture.jpeg");
    return status/256; 
}

struct coordinates findCache() {
    printf("Taking picture\n"); 
    system("streamer -c /dev/video0 -b 16 -o /root/bot/arm/dice/capture.jpeg");
    printf("Calling cache\n"); 
    int status = system("/root/bot/arm/dice/cache /root/bot/arm/dice/capture.jpeg");
 
    struct coordinates coor;
  
    printf("Opening file\n"); 
    FILE * fin = fopen("/root/bot/arm/dice/coordinates.txt","r"); 

    printf("Ascii to int\n"); 
    //We get images that are 320x240. Change these numbers if that's not the case
    char buff[16];
    fgets(buff,16,fin);
    coor.x = atoi(buff);

    fgets(buff,16,fin);
    coor.y = atoi(buff);

    if(debug) printf("Read from file: coor (%d, %d)\n", coor.x, coor.y);   
    
    fclose(fin);

    return coor;
}

void haltDieMotor() {
    initPWMcontr();

    unsigned char buff[2];
    buff[0] = 9;
    buff[1] = 0;
    i2c_write(PWMhandle, buff, 2);
}
