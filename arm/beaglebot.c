//
// This is the main program on the BBB
// It launches the DSP code on PRU 0 (slave)
// It also launches the I2S and sample period code on PRU 1 (master)
// At the end we start tclsh as a child process.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include "prussdrv.h"
#include "pruss_intc_mapping.h"

#include "mio.h"
#include "child.h"
#include "BBBlib.h"
#include "mem.h"
#include "gui.h"
#include "PRUlib.h"
#include "miscLib.h"

// Length of string buffer

#define   LEN   100

// TRUE and FALSE

#define   TRUE   1
#define   FALSE  0

// We need a global variable that wil point to the shared memory

shared_memory_t  *shared_memory ;

// Need a variable for debugging

int		debug = TRUE ;

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

// **********************************
// robot test
// We will call routines from ROBOTlib
// **********************************

void testROBOT (void) {
   if (debug) printf("In testROBOT stub\n") ;
   return ;
}

// **********************************
// Main program
// **********************************

int main (void) {  

   FILE    *read_from, *write_to;
   int 	   childpid ;
   char    str[LEN] ;
   int     exitFlag = FALSE ;
   int     runFlag = FALSE ;    

// Print a welcome statement

   printf("\nSIUE Beaglebot Project\n") ;
   printf("12-Jun-2016\n\n") ;

// GPIO initialization

   if (debug) printf("Initializing the GPIOs which we will use ...\n") ;
   GPIOinit() ;
   setPinValue(GPIO_LED_PIN, OFF) ;

// Start the gui

   childpid = start_child("tclsh", &read_from, &write_to);
   fprintf(write_to, "source ./tcl/gui.tcl \n") ;

// Get data from GUI

   while (!exitFlag) {
       if  (fgets(str, LEN, read_from) != NULL) {
          getGUIvars(str) ;
          exitFlag = GUIvars.exitFlag ;
          if (!exitFlag) {
             if (!runFlag) {
                execPRUprograms() ;
                testROBOT() ;
                runFlag = TRUE ;
             } else {;
                PRUstop() ;
                execPRUprograms() ;
                testROBOT() ;
            } // end if then elsef
          } // end if
       } //end if
   } // end while  

// User wants to exit so let PRU0 know

   shared_memory->exitFlag = TRUE ;
   if (debug) memoryDump() ;  
   pauseSec(1) ; // Delay for 1 sec before stopping PRU    
   PRUstop() ;

   return 0;
} // end main

