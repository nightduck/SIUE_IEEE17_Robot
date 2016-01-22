//
// This is a program to test out a series of submodules
// that might be useful for robotics
//

#define		ON  	1
#define		OFF		0

// We need stdio support

#include  <stdio.h>

// And our robotic library

#include "ROBOTlib.h"

// Need a buffer for UART operations

    unsigned char UARTbuf[80] ;
	int  uart ;

// Start of main program

int main(void) {

// Initialize the UART
// UART fid (uart) is a global variable.

    uart = initUART() ;

// Send a message to PSoC 

//    PSOCprint("Hello world.\n") ;

// Test the sonar module

    test_sonar() ;

// Go out for a test drive

//    test_drive() ;

// Test the servo

//	test_servo() ;

//  Turn LED off on PSOC board
//  Then turn on on for 5 seconds

   	turnLED(OFF) ;
	turnLED(ON) ;
 	pauseSec(5) ;
	turnLED(OFF) ;

// Close the UART connection

    close(uart) ; 
   
	return 0;
}
