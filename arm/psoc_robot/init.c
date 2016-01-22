#include <m8c.h>        // part specific constants and macros
#include <psocapi.h>    // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <string.h>
#include <psocgpioint.h>

#include "utilities.h"
#include "MotorLib.h"

extern    BOOL    CurKeyState ;
extern	  BOOL	  LFmode ;

extern    void    ReleaseBrake(void) ;
extern    void    SetDirection(BYTE, BYTE) ;

///////////////////////////////////////////////////////////////////////////////
// PSoC initialization routine
//////////////////////////////////////////////////////////////////////////////

void initPSoC(void) {

// Initialize some global variables

    CurKeyState = KEY_UNPRESSED ;
//
// Added to support the line following routine
//
	LFmode = FALSE ;
//
// Enable our pushbutton switch
//
	SWITCH_Data_ADDR &= ~SWITCH_MASK ;  
//
// Turn the status LED off
//
   	LEDstatus_Data_ADDR &= ~LEDstatus_MASK ;              // A low turns LED off         
//
// Make sure the brake is OFF!
//
    ReleaseBrake() ;
//
// Set direction to forward for both wheels
//
    SetDirection(FWD, FWD) ;
//
// Set PGA gain to 1
// Start the PGA
//
   	PGA_1_SetGain(PGA_1_G1_00) ;
   	PGA_1_Start(PGA_1_HIGHPOWER) ;
//
// Power up the ADC but don't start it
//
   	DelSig_1_Start(DelSig_1_HIGHPOWER) ;
	
//
// Start the UART
//
    UART_1_Start(UART_PARITY_NONE) ;
  
//
// Initialize I2C interface
// 

	I2CHW_1_Start() ;
	I2CHW_1_EnableMstr() ;
	I2CHW_1_EnableInt() ;

// Set up PWMS ... set period to 2400 clocks, 0% duty

// Right wheel (PWMright)

    PWM16_1_WritePeriod(2399);  
    PWM16_1_WritePulseWidth(0);  
    PWM16_1_DisableInt();  

// Left wheel (PWMleft)

    PWM16_2_WritePeriod(2399);  
    PWM16_2_WritePulseWidth(0);  
    PWM16_2_DisableInt();  	
//
// Set up SERVO_1
//
    PWM16_3_WritePeriod(639);  
    PWM16_3_WritePulseWidth(0);  
    PWM16_3_DisableInt();  
	
//
// Set up SERVO_0
//
    PWM16_4_WritePeriod(639);  
    PWM16_4_WritePulseWidth(0);  
    PWM16_4_DisableInt(); 
  
//Make sure general purpose timer interrupt is disabled  

    Timer16_1_DisableInt();  
//
// Timers (8 bit) 1 and 2 will be used by our PID velocity control loops
//
    Timer8_1_WritePeriod(255) ;  
    Timer8_2_WritePeriod(255) ; 
    Timer8_1_DisableInt();  
	Timer8_2_DisableInt();    
  
// Enable the global interrupts 

    M8C_EnableGInt ;  

// Stop left and right timers and light sensor timer
      
    Timer8_1_Stop();    // Right wheel decoder
    Timer8_2_Stop();    // Left wheel decoder
    Timer8_3_Stop();    // Light sensor
	
//
// Start PWM modules
//
	PWM16_1_Start();     // Right drive wheel 
    PWM16_2_Start();     // Left drive wheel
    PWM16_3_Start();     // Servo 1
    PWM16_4_Start();     // Servo 0
	
    return ;
    	
}
