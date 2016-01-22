//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include <psocapi.h>    // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <string.h>
#include <psocgpioint.h>
//
// User includes
//
#include "LCDlib.h"
#include "utilities.h"
#include "ServoLib.h"
#include "SensorLib.h"
#include "MotorLib.h"

//
// Declare global variables
// Declare globally available functions
//
#include "global.h"

/*
SYS_CLK is 24 MHz
VC1 is 4.8 MHz
VC2 is 480 kHz
VC3 is 38.4 kHz
*/

///////////////////////////////////////////////////////////////////////////

void main(void)
{ 	
	void    GetCommand(void) ;
	void    TestDrive(void) ;
	  
// Initialization
		
	initPSoC() ; 
	LCDclear() ;
	LongWait(2000) ;
	LCDhome() ;
//
// Write today's date out to LCD screen
//
	LCDstr(1, 1, cstrcpy(str, "Run for 8 sec")) ;
	LCDstr(2, 1, cstrcpy(str, "24 in/sec")) ;
	LCDstr(3, 1, cstrcpy(str, "12 in/sec")) ;
	LCDstr(4, 1, cstrcpy(str, "6 in/sec")) ;
	LongWait(4000) ;
//
// "Leadfooting" is allowed!
//  No line following
//  Don't automatically stop for an obstacle.
//  Just drive forward 3 feet.
//  
	TurnLED(ON) ;
	AllowLeadFoot = TRUE ;
	LFmode = FALSE ;
	DistanceThreshold = -1.0 ;
	LCDclear() ;
	LCDhome() ;
	LongWait(2000) ;
	LCDstr(1, 1, cstrcpy(str, "24 in/sec fwd   "))	;
	LongWait(2000) ;
	fwd(192.0, 23.0) ;  
	LongWait(2000) ;
	LCDstr(1, 1, cstrcpy(str, "12 in/sec bwd   "));	
	bwd(96.0, 12.0) ;
	LongWait(2000) ;
	LCDstr(1, 1, cstrcpy(str, "6 in/sec fwd    "));
	fwd(48.0, 6.0) ;	
	TurnLED(OFF) ;
	
//
// Go into infinite loop
// Wait for reset switch to be pressed
//
	while (TRUE) {} ;
	
} // end of main program


// ************************************************************
// Routine which determines if an obstacle is ahead of us ...
// Called by the Drive motor subroutine ....
// Also use to detect a black line 
// We want to stop if all four light sensors are over black!
// ***********************************************************

BOOL	ObstacleAhead(void) {
	float 	distance ;
	BOOL	BlackLine ;

	BlackLine = LFpid.L1 | LFpid.L2 | LFpid.R1 | LFpid.R2 ;
	
	distance = GetDistance(5) ;
	
	if ((distance <= DistanceThreshold)|| BlackLine) return TRUE ;
	else return FALSE ;
		
}	

//***********************************************************
// Take her out for a test drive
// ***********************************************************

void TestDrive(void) {
   float	speed ;
   float	x, y ;

	speed = 16.0 ;
	x = 44.0 ;
	y = 24.0 ;

/*
	rotate(CW, 10.0, 2.0 ) ;
	rotate(CCW, 10.0, 2.0 ) ;	
	rotate(CCW, 10.0, 2.0 ) ;	
	rotate(CW, 10.0, 2.0 ) ;
	LongWait(500) ;
	
	servo(CLAW, 900) ;
	fwd(2.0, 2.0) ;
	LongWait(500) ;
	servo(CLAW, 2500) ;
	LongWait(500) ;
	bwd(2.0, 2.0) ;
*/

	fwd(x, speed) ;
	right() ;
	fwd(y, speed) ;
	right()  ;
	fwd(x, speed) ;
	right() ;
	fwd(y, speed);
	right() ;

	fwd(x, speed) ;
	left() ;
	fwd(y, speed) ;
	left()  ;
	fwd(x, speed) ;
	left() ;
	fwd(y, speed);
	left() ;	
	
	LongWait(500) ;
	
	return ;
}

// *****************************************************
// Routine which sits and waits for a command from master
// controller sent over serial port.  
// The command is interpreted and performed.
// *******************************************************

void   GetCommand(void) {

	char	* strPtr ;		// Pointer to received string 
	float   dist ;
	float   speed ;
	float   angle ;
	BYTE    ServoCode ;		// Servo code
	int		PulseWidth ;    // Servo pulse width
	int     time ;
	float   value ;
	
	void	TurnLightSensor(int, int) ;
	void    LFmonitor(void) ;
	

	UART_1_IntCntl(UART_1_ENABLE_RX_INT);  	 // Enable RX interrupts
	while (!UART_1_bCmdCheck()) {};          // Wait for command	
		LCDclear() ;
		LCDhome() ;		
    	if(strPtr = UART_1_szGetParam()) { 	// More than delimiter?
//
// Print new command to the LCD module screen and echo out serial port
//
//    	   	LCDstr(1, 1, strPtr) ;	
//			UART_1_PutString(strPtr);
			UART_1_CPutString("\r\n") ;
			
			switch (strPtr[0]) {
//
// Print string
//
			    case 'p' : 	strPtr = UART_1_szGetParam() ;
				            LCDstr(2, 1, strPtr) ;
							UART_1_CPutString("ACK\r\n") ;
						  	break ;
//
// CW rotation
//
				case 'c' :	strPtr = UART_1_szGetParam() ;
				            angle = (float) atoi(strPtr);
							strPtr = UART_1_szGetParam() ;
							speed = (float) atoi(strPtr);
							rotate(CW, angle, speed) ;
							UART_1_CPutString("ACK\r\n") ;
						  	break ;		
//
// CCW rotation
//
				case 'C' :	strPtr = UART_1_szGetParam() ;
				            angle = (float) atoi(strPtr);
							strPtr = UART_1_szGetParam() ;
							speed = (float) atoi(strPtr);
							rotate(CCW, angle, speed) ;
							UART_1_CPutString("ACK\r\n") ;
						  	break ;		
//
// Make right turn
//
				case 'r' : 	right() ;
							UART_1_CPutString("ACK\r\n") ;
							break ;
//
// Make left turn
//
				case 'l' :	left() ;
							UART_1_CPutString("ACK\r\n") ;
				            break ;
//
// Drive forward
// First parameter is distance in tenths of an inch
// Second parameter is speed in tenths of an inch per sec
//
				case 'f' :	strPtr = UART_1_szGetParam() ;
				            dist = (float) atoi(strPtr) ;
							strPtr = UART_1_szGetParam() ;
							speed = (float) atoi(strPtr) ;
							fwd(dist, speed) ;
							UART_1_CPutString("ACK\r\n") ;
							break ;						
//
// Drive backward
// First parameter is distance in tenths of an inch
// Second parameter is speed in tenths of an inch per sec
//
				case 'b' :	strPtr = UART_1_szGetParam() ;
				            dist = (float) atoi(strPtr)  ;
							strPtr = UART_1_szGetParam() ;
							speed = (float) atoi(strPtr) ;
							bwd(dist, speed) ;
							UART_1_CPutString("ACK\r\n") ;	
							break ;			
//
// Servo commands
//
				case 's':	strPtr = UART_1_szGetParam() ;
				            ServoCode = (BYTE) atoi(strPtr) ;
							strPtr = UART_1_szGetParam() ;
							PulseWidth = atoi(strPtr);
				            servo(ServoCode, PulseWidth) ;
							UART_1_CPutString("ACK\r\n") ;
							break ;
//
// Display a compass heading
//
				case 'm' : 	LCDstr(2, 1, cstrcpy(str, "Compass heading:")) ;
						    value = CompassHeading() ;
							LCDfloat(3, 1, value) ;
							UART_1_CPutString("ACK\r\n") ;
							break ;
//
// Display the battery voltage
//
				case 'v' :	LCDstr(2, 1, cstrcpy(str, "Battery (V):")) ;							
							value = GetBatteryVoltage(500) ;
							LCDfloat(3, 1, value) ;
							UART_1_CPutString("ACK\r\n") ;
							break ;
//
// Display the distance to object 
//
				case 'd' :	LCDstr(2, 1, cstrcpy(str, "Distance (in):")) ;
							value = GetDistance(500);
							LCDfloat(3, 1, value) ;
							UART_1_CPutString("ACK\r\n") ;
							break ;							
//
// Turn the light sensor .. specify pulse width (us) and time in ms
//
				case 'T' :	strPtr = UART_1_szGetParam() ;
							PulseWidth = atoi(strPtr) ;
							strPtr = UART_1_szGetParam() ;
							time = atoi(strPtr) ;
							TurnLightSensor(PulseWidth, time) ;
							break ;
//
// Monitor line following sensor
//

				case 'L' :	LFmonitor() ;
							break ;							
//
// That's all!!!!
//

				default:	break ;
				
			} // end switch
		} // end if
//
// Reset the command buffer and get ready for a new command
//
	UART_1_CmdReset() ; 
	return ;
}

// ******************************************
// Routine to monitor line following sensor
// **********************************************

void LFmonitor(void) {

    BYTE   val ;

	LCDclear() ;
	LCDhome() ;
	LCDbacklight(OFF) ;
	LCDstr(1, 1, cstrcpy(str, "Line sensor:")) ;


	while (TRUE) {
		LCDblank(2) ;
		val = LF_L2_Data_ADDR & LF_L2_MASK ;
		if (val == 0) {
	         LCDstr(2, 8, cstrcpy(str, "WHITE")) ;
	    }
		else {
		  	 LCDstr(2, 8, cstrcpy(str, "BLACK")) ;
	    }
		
		val = LF_L1_Data_ADDR & LF_L1_MASK ;
		if (val == 0) {
	         LCDstr(2, 9, cstrcpy(str, "WHITE")) ;
	    }
		else {
		  	 LCDstr(2, 9, cstrcpy(str, "BLACK")) ;
	    }
		
		val = LF_R1_Data_ADDR & LF_R1_MASK ;
		if (val == 0) {
	         LCDstr(2, 10, cstrcpy(str, "WHITE")) ;
	    }
		else {
		  	 LCDstr(2, 10, cstrcpy(str, "BLACK")) ;
	    }
		
		val = LF_R2_Data_ADDR & LF_R2_MASK ;
		if (val == 0) {
	         LCDstr(2, 11, cstrcpy(str, "WHITE")) ;
	    }
		else {
		  	 LCDstr(2, 11, cstrcpy(str, "BLACK")) ;
	    }
		
		LongWait(250) ; //  Wait for 250 ms
	}
	return ;
}

////////////////////////////////////////////////////////////////////////
// Timer interrupt routine 
/////////////////////////////////////////////////////////////////////////

#pragma interrupt_handler Timer16_1_ISR
#pragma nomac

void Timer16_1_ISR() {

	TimesUp = TRUE ;
	return ;
}	
	