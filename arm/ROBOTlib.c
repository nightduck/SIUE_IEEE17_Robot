//
// Here is a set of subrotuines useful for the robot
//

// Stdio support

#include    <stdio.h>

// String support

#include    <string.h>

// Support for uart

#include "BBBlib.h"

// We will call things from srf02.h

#include "srf02.h"

// We will call things from servo_driver.h

#include "servo_driver.h"

// Globals

#include "global.h"

int PRUputs(char * str) {
    printf("%s", str) ;
    return 0 ;
}

// Routine to turn LED on board ON or OFF
// 0 is OFF and 1 is ON

void turnLED(int state) {
    return ;
}

// Routine to drive robot forward
// Distance will be in tenths of an inch 
// And so will velcity

int fwd(float distance, float velocity) {
    int dist, vel ;
    char str[80] ;
    dist = (int) (10.0 * distance + 0.5) ;
    vel = (int) (10.0 * velocity + 0.5) ;
    sprintf(str, "f:%d:%d\n", dist, vel) ;
    return 0 ;
}

// Routine to drive robot backward

int bwd(float distance, float velocity) {
    int dist, vel ;
    char str[80] ;
    dist = (int) (10.0 * distance + 0.5) ;
    vel = (int) (10.0 * velocity + 0.5) ;
    sprintf(str, "b:%d:%d\n", dist, vel) ;
    return 0 ;
}

// Routine to drive to make a right turn

int right(void) {
    char str[80] ;
    sprintf(str, "r\n") ;
    return 0 ;    
}

// Routine to drive to make a left turn

int left(void) {
    char str[80] ;
    sprintf(str, "l\n") ; 
    return 0 ;    
}

// Routine to spin robot in CW direction specified # of degrees

int cw(float degrees, float velocity) {
    int  deg ;
    int  vel ;
    char str[80] ;    
    deg = (int) (degrees + 0.5) ;
    vel = (int) (10.0 * velocity + 0.5) ;
    sprintf(str, "c:%d:%d\n", deg, vel) ;
    return 0 ; 
}      

// Routine to spin robot in CCW direction specified # of degrees

int ccw(float degrees, float velocity) {
    int  deg ;
    int  vel ;
    char str[80] ;
    deg = (int) (degrees + 0.5) ;
    vel = (int) (10.0 * velocity + 0.5) ;
    sprintf(str, "C:%d:%d\n", deg, vel) ;
    return 0 ; 
}      

// Routine to take a test drive

int test_drive(void) {
    fwd(24.0, 6.0) ;
    right() ;
    fwd(24.0, 6.0) ;
    left() ;
    fwd(24.0, 6.0) ;
    return 0 ;
}

// Routine to test the sonar unit

int test_sonar() {
    unsigned int  range ;

// Get range from srf02

   range = get_srf02_range() ;

// Print the range

   printf("\nRange ==> %d cm\n\n", range);
   return 0 ;
}

// Routine to test the servo driver

int test_servo(void) {
   int i ;

// Reset the servo driver

   resetServoDriver() ;

// Set rep rate to 50 Hz

   setServoFREQ(50.0) ;

// Turn servo one direction and then back the other

   for (i=150; i<=350; i+=50) {
       setServoPW(0, i) ;
       delay_ms(900) ;
       delay_ms(900) ;
    }
    return 0 ;
}

