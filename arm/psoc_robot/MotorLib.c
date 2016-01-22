//
// Library of routines to support forward drive motors on Stinger 
//

// Motor 1 is left and 2 is right

#include <m8c.h>        // part specific constants and macros
#include <psocapi.h>    // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <string.h>
#include <psocgpioint.h>

#include "utilities.h"
#include "MotorLib.h"

// Waits specified number of usec

extern  void   	Wait(long) ;
extern  void   	LongWait(long) ;
extern  void    SetAlarm(long) ;
extern  void    ClearAlarm(void) ;
extern  void    TurnLED(BYTE) ;
extern	BOOL	ObstacleAhead(void) ;


// Right and Left wheel records

extern  WheelStruct    RightWheel ;
extern  WheelStruct    LeftWheel ;
extern  BOOL           AllowLeadFoot  ;
//

extern  BOOL  TimesUp ;


//  Line following PID stuff

extern	LFpidStruct   LFpid ;
extern  BOOL		  LFmode ;


// ******************************************************
// Routine to set duty cycles (percent) on right and left wheels
// *******************************************************

BOOL SetDutyCycle(BYTE LeftDuty, BYTE RightDuty) {

	int		RightVal ; 
	int 	LeftVal ;
	
  	if ((RightDuty < 0) || (RightDuty > 80)) {
	    return FALSE ;
   	}
  	if ((LeftDuty < 0) || (LeftDuty > 80)) {
	    return FALSE ;
   	}
	
	RightVal = RightDuty * 24 ;	
	LeftVal = LeftDuty * 24 ;
	
	PWM16_1_WritePulseWidth(LeftVal);
	PWM16_2_WritePulseWidth(RightVal);
	
	return TRUE ;
}

// ***************************************************
// Routine to set PWM values on right and left wheels
// ****************************************************

void SetPWMvalues(int LeftValue, int RightValue) {

    if (RightValue > PWM_MAX) {
	    RightValue = PWM_MAX ;
	}	
	if (RightValue < PWM_MIN) {
	    RightValue = PWM_MIN ;
	}	
	if (LeftValue > PWM_MAX) {
	    LeftValue = PWM_MAX ;
	}
	if (LeftValue < PWM_MIN) {
	    LeftValue = PWM_MIN ;
	}	  
	PWM16_1_WritePulseWidth(LeftValue);
	PWM16_2_WritePulseWidth(RightValue);
	return  ;
}

// ***************************************************************
// Routine to start counting pulses
// This routine intializes the counters to 255 and starts the counters
// Just need to call this routine once
// ****************************************************************

void StartCountingPulses(void) {
    Timer8_1_WritePeriod(255) ;
	Timer8_2_WritePeriod(255) ;
	RightWheel.CurrentCnt = 255 ;
	LeftWheel.CurrentCnt = 255 ;
    Timer8_1_Start() ;
	Timer8_2_Start() ;
	return ;
}

// **********************************************************
// Routine to stop counting pulses
// ***********************************************************
void StopCountingPulses(void) {
    Timer8_1_Stop() ;
	Timer8_2_Stop() ;
	return ;
}

// ***********************************************************
// Routine to ReadDecoderValues
// ************************************************************

void ReadDecoderValues(void) {
	
	RightWheel.PastCnt = RightWheel.CurrentCnt ;
	LeftWheel.PastCnt = LeftWheel.CurrentCnt ;
	
	RightWheel.PastV = RightWheel.CurrentV ;
	LeftWheel.PastV = LeftWheel.CurrentV ;
	
	LeftWheel.CurrentCnt = (int) Timer8_1_bReadTimer() ;
    LeftWheel.CurrentV = LeftWheel.PastCnt - LeftWheel.CurrentCnt ;	
	if(LeftWheel.CurrentV < 0) {
	   LeftWheel.CurrentV += 256 ;
	}
	
    RightWheel.CurrentCnt = (int) Timer8_2_bReadTimer() ;
    RightWheel.CurrentV = RightWheel.PastCnt - RightWheel.CurrentCnt ;
	if(RightWheel.CurrentV < 0) {
	   RightWheel.CurrentV += 256 ;
	}

	RightWheel.Distance +=  RightWheel.CurrentV ;
	LeftWheel.Distance +=   LeftWheel.CurrentV ;
	
	return ;
}

// ***************************************************************
// Routine to initialize the right and left wheel structures
// ***************************************************************

void   InitWheels(void) {

   RightWheel.CurrentCnt = 255 ;     
   RightWheel.PastCnt = 0 ;         
   RightWheel.SetPoint = 0; 
   RightWheel.OrigSetPoint = 0 ;
   RightWheel.CurrentV = 0 ;       
   RightWheel.PastV = 0 ;           
   RightWheel.e0 = 0 ;              
   RightWheel.e1 = 0 ;               
   RightWheel.e2 = 0 ;             
   RightWheel.PWMvalue = 0 ;        
   RightWheel.Distance = 0 ;    
   RightWheel.TargetDistance = 0 ;  
   RightWheel.Dir = FWD ;                
   RightWheel.pGain = KP ;        
   RightWheel.iGain = KI ;         
   RightWheel.dGain = KD ;          
   RightWheel.Diameter = WHEEL_DIAM_R ;
   RightWheel.TicsPerInch = CNTS_PER_REV / (PI * WHEEL_DIAM_R) ;

   
   LeftWheel.CurrentCnt = 255 ;     
   LeftWheel.PastCnt = 0 ;          
   LeftWheel.SetPoint = 0 ;
   LeftWheel.OrigSetPoint = 0 ; 
   LeftWheel.CurrentV = 0 ;
   LeftWheel.PastV = 0 ;
   LeftWheel.e0 = 0 ;              
   LeftWheel.e1 = 0 ;               
   LeftWheel.e2 = 0 ;               
   LeftWheel.PWMvalue = 0;    
   LeftWheel.Distance = 0 ;
   LeftWheel.TargetDistance = 0 ;
   LeftWheel.Dir = FWD ;                    
   LeftWheel.pGain = KP ;       
   LeftWheel.iGain = KI ;         
   LeftWheel.dGain = KD ;    
   LeftWheel.Diameter = WHEEL_DIAM_L ;
   LeftWheel.TicsPerInch = CNTS_PER_REV / (PI * WHEEL_DIAM_L) ;
   
   return ;
}

// *********************************************************
// Routine to apply brake
// *********************************************************
void ApplyBrake(void) {

    SetPWMvalues(0, 0) ;
    BRAKE_Data_ADDR |= BRAKE_MASK ;	
	return ;
}

// **********************************************************
// Routine to release brake
// ********************************************************
void ReleaseBrake(void){
    BRAKE_Data_ADDR &= ~BRAKE_MASK ;
	return ;
}

// **********************************************************
// Routine to coast to a stop
// **********************************************************

void Coast(void) {
   SetPWMvalues(0, 0) ;
   return ;
}

// *****************************************************
// Routine to set the direction on wheels
// Right and left are reversed
// ******************************************************

void SetDirection(BYTE  LeftDir, BYTE  RightDir) {
    if (LeftDir == FWD) {
       DIRright_Data_ADDR &= ~DIRright_MASK ;
	   RightWheel.Dir = FWD ;
	}
	if (LeftDir == BWD) {
	   DIRright_Data_ADDR |= DIRright_MASK ;
	   RightWheel.Dir = BWD ;
	}
	
    if (RightDir == FWD) {
	   DIRleft_Data_ADDR &= ~DIRleft_MASK ; 
	   LeftWheel.Dir = FWD ;
	}
	if (RightDir == BWD) {
	   DIRleft_Data_ADDR |= DIRleft_MASK ;  
	   LeftWheel.Dir = BWD ;
	}
	return ;
}

// **********************************************************
// PIDupdate routine
// **************************************************************

void   PIDupdate(void)  {
  float    pTerm, dTerm, iTerm ;


//
// Read the the right and left wheel decoders
// Updates PastCnt and CurrentCnt fields in both wheel structures
//
  ReadDecoderValues() ;

//
// Work on right wheel first
//
  RightWheel.e2 = RightWheel.e1 ;
  RightWheel.e1 = RightWheel.e0 ;
  RightWheel.e0 = RightWheel.SetPoint - RightWheel.CurrentV ;

// Calculate the delta proportional term

  pTerm = RightWheel.pGain * RightWheel.e0 ;   

// Calculate the delta integral term

  iTerm = RightWheel.iGain * (RightWheel.e0 - RightWheel.e1) ;  

// Calculate the delta derivative term

  dTerm = RightWheel.dGain * (RightWheel.e0 - RightWheel.e1 + 2 * RightWheel.e2) ;

// Add the three terms together and then enforce limits

  RightWheel.PWMvalue += (int) (pTerm + dTerm + iTerm + 0.5) ;
  
  if (RightWheel.PWMvalue > PWM_MAX) RightWheel.PWMvalue = PWM_MAX ;
  if (RightWheel.PWMvalue < PWM_MIN) RightWheel.PWMvalue = PWM_MIN ;  
  
//
// Work on left wheel now
//
  LeftWheel.e2 = LeftWheel.e1 ;
  LeftWheel.e1 = LeftWheel.e0 ;
  LeftWheel.e0 = LeftWheel.SetPoint - LeftWheel.CurrentV ;

// Calculate the delta proportional term

  pTerm = LeftWheel.pGain * LeftWheel.e0 ;   

// Calculate the delta integral term

  iTerm = LeftWheel.iGain * (LeftWheel.e0 - LeftWheel.e1) ;  

// Calculate the delta derivative term

  dTerm = LeftWheel.dGain * (LeftWheel.e0 - LeftWheel.e1 + 2 * LeftWheel.e2) ;

// Add the three terms together and then enforce limits

  LeftWheel.PWMvalue += (int) (pTerm + dTerm + iTerm + 0.5) ;

  if (LeftWheel.PWMvalue > PWM_MAX) LeftWheel.PWMvalue = PWM_MAX ;
  if (LeftWheel.PWMvalue < PWM_MIN) LeftWheel.PWMvalue = PWM_MIN ;  
  
// 
// Update the PWM values for both wheels
//
  SetPWMvalues(LeftWheel.PWMvalue, RightWheel.PWMvalue) ;

  return ;
}


// ***************************************************************
// Convert velocity in inches per sec to tics in sampling period
// *****************************************************************

void SetVelocity(float LeftVelocity, float RightVelocity) {
  	RightWheel.SetPoint = 
      (int)(RightVelocity * RightWheel.TicsPerInch 
	        * T/1000.0 + 0.5) ;
   	LeftWheel.SetPoint = 
      (int) (LeftVelocity * LeftWheel.TicsPerInch * T/1000.0 + 0.5) ; 
//
// Added to support the line following routine
//
	RightWheel.OrigSetPoint = RightWheel.SetPoint ;
	LeftWheel.OrigSetPoint = LeftWheel.SetPoint ;
   return ;
}

// ****************************************************************
// Convert distance in inches to tics
// *****************************************************************

void SetDistance(float LeftDistance, float RightDistance) {
    RightWheel.TargetDistance = 
	    (int) (RightDistance * RightWheel.TicsPerInch + 0.5) ;
    LeftWheel.TargetDistance = 
	    (int) (LeftDistance * LeftWheel.TicsPerInch + 0.5) ;
    return ;
}

// *****************************************************************
// Routine to convert Tics to inches
// *****************************************************************

float TicsToInches(int Tics) {
    float	Dist ;
	
	Dist = (2.0 * Tics) / 
	               (RightWheel.TicsPerInch 
				   + LeftWheel.TicsPerInch) ;
	return	Dist ;
}

// ******************************************************************
// Routine to drive forward/backward a given distance and at a
// given velocity
// Maximum distance is 740 inches (62 feet)
// Maximum velocity is 26 in /sec
// *********************************************************************

float move(BYTE LeftDir, BYTE RightDir, float Distance, float Velocity) {
    int    	TargetInTics ;
	long   	Time ;
	int    	AveDist ;
	float	AveDistInches ;
	float  	CurV ;
	BYTE   	State ;
	float   tmp ;
	int		SlowDownDist ;
	float   Delta_V ;
	
// Declare some routines that we need for line following

	void 	LFroutine(void) ;
	void  	LFpidInit(void) ; 	
//
// Make sure we don't do something stupid
//
	if (Distance > MAX_DIST) Distance = MAX_DIST ;
	if (Velocity > MAX_SPEED) Velocity = MAX_SPEED ;
	if (Distance < MIN_DIST) Distance = MIN_DIST ;
	if (Velocity < MIN_SPEED) Velocity = MIN_SPEED ;
//
// Delta V for accel and de_accel should be percentage of desired
// velocity
//
	Delta_V = DELTA_V_PCNT * Velocity ;
//
// Send appropriate control to H-Bridge circuit for direction
// that we want to go

    if((LeftDir == FWD) && (RightDir == FWD)) {
		SetDirection(FWD, FWD) ;
	}
    if((LeftDir == FWD) && (RightDir == BWD)) {
		SetDirection(FWD, BWD) ;
	}
    if((LeftDir == BWD) && (RightDir == FWD)) {
		SetDirection(BWD, FWD) ;
	}
    if((LeftDir == BWD) && (RightDir == BWD)) {
		SetDirection(BWD, BWD) ;
	}

//
// Initialize wheel records
// Compute velocity in tics per sampling periods
// Compute distance to be traveled in tics rather than inches
//
	InitWheels() ;
    SetDistance(Distance, Distance) ; 
	if (AllowLeadFoot) CurV = Velocity ;
	else CurV = Delta_V ; 
	SetVelocity(CurV, CurV) ;  
    TargetInTics = (RightWheel.TargetDistance +
				   LeftWheel.TargetDistance ) / 2 ;
//
//  Compute where we need to de-accelerate
//
    if (Velocity > MIN_SPEED) {
	 	tmp = REL_DIST * TargetInTics + 0.5 ;
		SlowDownDist = (int) tmp ;
	}
	
// Don't forget to release the brake!

	ReleaseBrake() ;
//
// Here we go!!!!
//
    State = ACCEL ;
	StartCountingPulses() ;
	Time = 1000 * T ;      // T is sampling period in msec ... convert to usec
	AveDist = -1 ;
//
// Added to support the line-following routine
// Initializes the line following PID structure
//
	if (LFmode) LFpidInit() ;      // ******** LINE FOLLOW MODE ************
//
	while (AveDist < TargetInTics){
	    TurnLED(ON) ;
	   	SetAlarm(Time) ; 

		PIDupdate() ;
//
// Compute average distance traveled thus far
//
		
	  	AveDist = (RightWheel.Distance + LeftWheel.Distance) / 2 ;
		
//
// Added to support the line following routine
//
		if (LFmode) LFroutine() ;   // ******** LINE FOLLOW MODE ******* //
//
// Check for an obstacle ahead
//
		if (ObstacleAhead()) {
			StopCountingPulses() ;
			ApplyBrake() ;
		    return TicsToInches(AveDist) ;
		}
//
// Decide if we should be maintaining constant velocity or 
// accelerating or de-accelerating
//
		if (!AllowLeadFoot) {
        	switch (State) {
	   			case  ACCEL:	CurV += Delta_V ; 
	   						if (CurV >= Velocity) {
							   CurV = Velocity ;
							   State = UNIFORM ;
							}
							else State = ACCEL ;
	   						SetVelocity(CurV, CurV) ;
							break ;
				case  UNIFORM:	if (AveDist > SlowDownDist) {
			                    State = DE_ACCEL ;
							}
							else State = UNIFORM ;
							break ;
				case  DE_ACCEL:	CurV -= Delta_V ;
			                if (CurV < MIN_SPEED) {
							    CurV = MIN_SPEED ;
							}
							State = DE_ACCEL ;
							SetVelocity(CurV, CurV) ;
							break ;
				default:	break ;
			} // end switch
		} // end if	
//
// Wait for timer to go off ... be patient :)
//
  		TurnLED(OFF) ; 
	    while(!TimesUp) ;   // Wait for alarm to go off
		ClearAlarm() ;
	}
	StopCountingPulses() ;
	ApplyBrake() ;
	return TicsToInches(AveDist) ;
}
	
// ********************************************************************
// Routine to rotate a specified direction, a given number of degrees
// and at a stated velocity.
// During rotation one wheel is driven forward and the other backward.
// **********************************************************************

void rotate(BYTE Direction, float Degrees, float Velocity) {
    int    TargetInTics ;
	float  Distance ;
	float  Theta ;

	Theta = DEG2RAD * Degrees ;              
            
	
	AllowLeadFoot = TRUE ;
    switch (Direction) {
        case CW :  Distance = RADIUS_CW * Theta ;  
				   move(FWD, BWD, Distance, Velocity) ;
	               break ;
				   
	    case CCW : Distance = RADIUS_CCW * Theta ; 
				   move(BWD, FWD, Distance, Velocity) ;
	               break ;
				   
		default  : break ;
	}
	AllowLeadFoot = FALSE ;
	return ;
}
	
// ************************************************************************
// Routine to make a right turn
// ************************************************************************

void right(void) {
	
    AllowLeadFoot = TRUE ;
    LongWait(250) ;           
 	rotate(CW, 90.0, TURN_SPEED) ;
	LongWait(250) ;  
	AllowLeadFoot = FALSE ;
    return ;
}

// ***********************************************************************
// Routine to make a left turn
// ************************************************************************

void left(void) {
    AllowLeadFoot = TRUE ;
    LongWait(250) ;         // Wait 250 msec before turning 
 	rotate(CCW, 90.0, TURN_SPEED) ;
	LongWait(250) ;   
	AllowLeadFoot = FALSE ;
    return ;
}

// ***********************************************************************
// Routine to move forward
// ************************************************************************

float fwd(float Distance, float Velocity){
    float dist ;
	dist = move(FWD, FWD, Distance, Velocity) ;
	return dist ;
}
	
// ************************************************************************
// Routine to move backward
// ***************************************************************************
float bwd(float Distance, float Velocity){
	float dist ;
	dist = move(BWD, BWD, Distance, Velocity) ;
	return dist ;
}	
	
// ***********************************************************************
// Routine to initialize the LFpid structure
// *********************************************************************

void  LFpidInit(void) {
	LFpid.w1 = W1 ;           	
	LFpid.w2 = W2 ;           
	LFpid.L1 = WHITE ;			
	LFpid.L2 = WHITE ;		
	LFpid.R1 = WHITE ;	
	LFpid.R2 = WHITE ;		
	LFpid.Kp = KP_PID ;        
	LFpid.Ki = KI_PID ;	   
	LFpid.Kd = KD_PID;  
	LFpid.error = 0.0;      
	LFpid.prev_error = 0.0 ;   
	LFpid.integral = 0.0 ;     	
	LFpid.derivative = 0.0 ;   	
	LFpid.output = 0.0;    
   	return ;
}
	
//
// Routine to use PID control to implement a line following routine
//
void LFroutine(void) {

	BYTE  scr ;    			// scratchpad register
	int   err ;    			// error value created from sensor data
	int   DeltaSetPoint ;  // Small change in right and left wheel setpoints
	
// Read the 4 optical sensors
// BLACK is equal to 1
// WHITE is equal to 0

    scr = LF_L1_Data_ADDR & 0xff ;   // Read port 0 byte
	
	if (scr & LF_L1_MASK) LFpid.L1 = BLACK ;
	else LFpid.L1 = WHITE ;
	
	if (scr & LF_L2_MASK) LFpid.L2 = BLACK ;
	else LFpid.L2 = WHITE ;
	
	if (scr & LF_R1_MASK) LFpid.R1 = BLACK ;
	else LFpid.R1 = WHITE ;
	
	if (scr & LF_R2_MASK) LFpid.R2 = BLACK ;
	else LFpid.R2 = WHITE ;
	
// Compute the error
// Left sensors being over black give me + error
// Right sensors being over black give me - erro
// Thus, + error means I need to turn right wheel at faster
// rate then the left wheel and vice versa

   	err = LFpid.w1 * LFpid.L1 ;
   	err -= LFpid.w1 * LFpid.R1 ;
   	err += LFpid.w2 * LFpid.L2 ;
   	err -= LFpid.w2 * LFpid.R2 ;
   	LFpid.error = (float) err ;

// Compute output of PID controller

   	LFpid.integral += LFpid.error ;
   	LFpid.derivative = LFpid.error - LFpid.prev_error ;
   	LFpid.output = LFpid.Kp * LFpid.error ;
   	LFpid.output += LFpid.Ki * LFpid.integral ;
   	LFpid.output += LFpid.Kd * LFpid.derivative ;
   	LFpid.prev_error = LFpid.error ;
 
//
// Use the output to adjust the velocity PID loop set point
//
    if (LFpid.output >= 0.0) DeltaSetPoint = (int) (LFpid.output + 0.5) ;
	else DeltaSetPoint = (int) (LFpid.output - 0.5) ;
	
	RightWheel.SetPoint = RightWheel.OrigSetPoint + DeltaSetPoint ;
   	LeftWheel.SetPoint = LeftWheel.OrigSetPoint - DeltaSetPoint ;
	
   return ;
}
	
	
	
	
	
