//
// Library of routines to support the servos
//

#include <m8c.h>        // part specific constants and macros
#include <psocapi.h>    // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <string.h>
#include <psocgpioint.h>

#include "ServoLib.h"
#include "utilities.h"

// Timer interrupt variables

extern   BOOL	TimesUp ;	

//
// Long wait routine

extern  BOOL LongWait(unsigned int ) ;
//
// Routine to control a servo motor
// Servo #0 is controlled by PWM16_4
// Servo #1 is controlled by PWM16_3
//
BOOL servo(BYTE MotorCode, int PulseWidth) {
    int		CountValue ;
    float	temp ;
	
    if ((PulseWidth < 0) || (PulseWidth >2500)){
	  	return FALSE ;
	}
    if ((MotorCode < 0) || (MotorCode > 1)) {
	  	return FALSE ;
	}	
	temp = PulseWidth / 31.25 + 0.5 ;
	CountValue = (int) temp ;
	
	switch (MotorCode) {
	    case 0:  	PWM16_4_WritePulseWidth(CountValue) ; 
	            	break ;
		case 1:  	PWM16_3_WritePulseWidth(CountValue) ;
					break ;
	}

	return  TRUE ;
}

//
// Routine to turn light sensor
// Specify the pulse width in used and the time in ms
// Pulse width of 0 or 1600 stops the turret.
// CW rotation if > 1600
// CCW rotation if < 1600
//
void TurnLightSensor(int PulseWidth, int time) {
   BOOL   servo(BYTE, int) ;
   
   servo(ROT_LIGHT, PulseWidth) ;
   LongWait(time) ;
   servo(ROT_LIGHT, 0) ;
   
   return ;
}
	