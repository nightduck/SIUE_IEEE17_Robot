#include <m8c.h>        // part specific constants and macros
#include <psocapi.h>    // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <string.h>
#include <psocgpioint.h>
//
// User includes
//
#include "utilities.h"

/*
SYS_CLK is 24 MHz
VC1 is 4.8 MHz
VC2 is 480 kHz
VC3 is 38.4 kHz
*/


//
// Library of utility routines
//

// ***********************************************

// Check for switch closure variables
//
extern    BOOL    PrevKeyState ;
extern    BOOL    CurKeyState ;


//////////////////////////////////////////////////////////////////////
//
// Check on status of pushbutton switch
//
//////////////////////////////////////////////////////////////////////

BOOL ScanForKeyPress(void) 
{
   SWITCH_Data_ADDR &= ~SWITCH_MASK ;  
   PrevKeyState = CurKeyState ;
   
   if (SWITCH_Data_ADDR & SWITCH_MASK) CurKeyState = KEY_PRESSED ;
   else CurKeyState = KEY_UNPRESSED ;
   
   if ((CurKeyState == KEY_PRESSED) && (PrevKeyState == KEY_UNPRESSED)) return(TRUE) ;
   else return(FALSE) ; 
}

// ************************************************

extern    BOOL  TimesUp ;	

////////////////////////////////////////////////////////////////////////
//
// Wait for x usec
// Timer16_1 is clocked from VC2 which is 480 kHz => 2.083 usec
// Can wait for times up to 135 ms or 135,000 usec
//
//
////////////////////////////////////////////////////////////////////////

void Wait(long delay) {	
    unsigned   int    time ;
	float      tmp ;
	
	tmp = ((float) delay / 2.083) + 0.5;			  
	time = (unsigned int) (tmp);		  

    Timer16_1_WritePeriod(time) ;		// Set up timer period
	Timer16_1_EnableInt(); 				// Enable interrupts
	TimesUp = FALSE ;
	Timer16_1_Start() ;				    // Start the timer
	while (!TimesUp) {} ;				// Wait for interrupt
	Timer16_1_DisableInt(); 			// Disable interrupts
	Timer16_1_Stop() ;					// Stop the timer

	return ;
}

///////////////////////////////////////////////////////////////////////////
//
// Routine can be be used to implement waits as long as 60 sec
// The routine is not terribly accurate but certainly reasonable.
//
// Accepts time in ms to wait.
// Must be between 1 ms and 30 sec
//
///////////////////////////////////////////////////////////////////////////

BOOL LongWait(unsigned int delay) {
   unsigned   int    i ;

   if ( (delay < 1) || (delay > 30000)) return FALSE ;
   for (i = 1; i <= delay; i++) {
      Wait(1000) ;   // Wait for 1 msec
   }
   return TRUE ;
}

//
// Routine to set an alarm to go off in x usec
//

void SetAlarm(long delay) {
    unsigned   int    time ;
	float      tmp ;
	
	tmp = ((float) delay / 2.083) + 0.5;			  
	time = (unsigned int) (tmp);		 

    Timer16_1_WritePeriod(time) ;		// Set up timer period
	Timer16_1_EnableInt(); 				// Enable interrupts

	TimesUp = FALSE ;
	Timer16_1_Start() ;				    // Start the timer
	return ;
}

void ClearAlarm(void) {

   	Timer16_1_DisableInt(); 			// Disable interrupts
	Timer16_1_Stop() ;					// Stop the timer
	return ;
}

//
// Routine to control the red LED on the board
//
void  TurnLED(BYTE state) {
    switch (state) {
      	case  ON :  	LEDstatus_Data_ADDR |= LEDstatus_MASK ; 
	                	break ;
		case  OFF:  	LEDstatus_Data_ADDR &= ~LEDstatus_MASK ;
		            	break ;
		case  TOGGLE:	LEDstatus_Data_ADDR ^= LEDstatus_MASK ;
	 					break ;
		default:		break ;
	}
    return ;
}
  
   