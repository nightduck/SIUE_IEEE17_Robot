//
// This is the main program on the BBB
// It launches the DSP code on PRU 0 (slave)
// It also launches the I2S and sample period code on PRU 1 (master)
// At the end we start tclsh as a child process.
//

#include   <stdio.h>
#include   <stdlib.h>
#include   <assert.h>
#include   <stdint.h>
#include   <math.h>
#include   "prussdrv.h"
#include   "pruss_intc_mapping.h"

#include   "mio.h"
#include   "child.h"
#include   "bbbLib.h"
#include   "mem.h"
#include   "PRUlib.h"
#include   "robotLib.h"
#include   "navigation.h"
#include   "libIO.h"
#include   "amg.h"
#include   "die.h"
#include   "ir.h"

// Additional includes for 2017 robot

#include   "libIO.h"

// TRUE and FALSE

#define   TRUE   1
#define   FALSE  0

// GUI mode or not

int         GUImode = FALSE ;

// We need a global variable that wil point to the shared memory

shared_memory_t  *shared_memory ;

// Need a variable for debugging

int		    debug = TRUE ;

// Need global structure to hold GUI variables

GUIvars_t   GUIvars ;

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to execute PRU programs 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void execPRUprograms() {

// Initialize the PRUs

   if (debug) printf("Initializing the PRUs.\n") ;
   PRUinit() ;

// Configure PRU 0 based on GUI settings

   if (debug) printf("Configuring PRU 0 with GUI data\n") ;
   configPRU() ;

// Start the PRUs

   if (debug) printf("Start the PRUs ...\n") ;
   PRUstart() ;

   return ;
}

// *****************************************************
// Routine that waits for user to press "green" button
// *****************************************************

void waitForUser(void) {
   int  LEDstate = ON ;
   turnLED(ON) ;
   while (!buttonPress()) {
      delay_ms(250) ;
      if (LEDstate) {
         LEDstate = OFF ;
         turnLED(OFF) ;
      } else {
         LEDstate = ON ;
         turnLED(ON) ;
      }
   } // end while
  turnLED(OFF) ;
  pauseSec(3) ;
  return ;
} // waitForUser() ;

// **********************************
// Main program
// **********************************

int main (void) {  

   FILE    *read_from, *write_to;
   char    str[STR_LEN] ;
   int     exitFlag = FALSE ;
   int     runFlag = FALSE ;    

// Print a welcome statement

   if (debug) printf("\nSIUE Beaglebot Project\n") ;
   if (debug) printf("12-Nov-2016\n\n") ;

// GPIO initialization

   if (debug) printf("Initializing the GPIOs which we will use ...\n") ;
   GPIOinit() ;

// ****************************************************
// Get the GUI string from the robot.config file
// and parse it as usual
// ****************************************************

        loadGuiVarsFromFile(str) ;
        getGUIvars(str) ;

// The string we load may have the exit flag set
// We don' want this ...

        GUIvars.exitFlag = FALSE ;

//Initialize hardware
        if(debug) printf("ROBOTIO init\n");
	initRobotIO();		//Initialize daughter board

// Load, configure, and start the PRUs 

        execPRUprograms() ;

//If red button is held down, clear lights and exit
	if(getPinValue(STOP) == 0) {
                printf("Red button pressed. Performing reset\n");
		cleanup();
		return 0;
        }	

// Wait for user to press green buton
      readyLight();
      waitGO() ;
        
// Now that we're guaranteed to be in starting position, initialize rest of hardware
        if(debug) printf("AMG init\n");
	initAMG();		//Initialize gyroscope
        if(debug) printf("IR init\n");
	initIR();		//Initialize IR sensors
        if(debug) printf("PWM init\n");
	initPWMcontr();   	//Initialize actuator motor controller
	if(debug) printf("Turning on studfinder\n");
        initStudFinder();       //Turn on studfinder and wait for it to calibrate

// Wait for stud finder to calibrate, up to 5 seconds
	int i;
	for(i = 0; i < 50; i++) {
		if(!pollStudFinder()) break;
		delay_ms(100);
        }
        delay_ms(500);

// Perform main operation here
        runRound();
    //	dieReadingSequence();
    //    testRobot();
    //    testIO() ;
	
// Wait for user to press red button
	waitSTOP();
        cleanup();

// User wants to exit so let PRU0 know

   exitFlag = TRUE ;
   shared_memory->exitFlag = TRUE ;
   if (debug) memoryDump() ;
   pauseSec(1) ; 
       
   PRUstop() ;

   return 0;
} // end main

