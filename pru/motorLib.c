//
// Library of routines which run on PRU0
// for handling the motors
//

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "mem.h"
#include "fix.h"
#include "motorLib.h"

// Pointer to shared memory is a global variable

  extern   shared_memory_t   *mem ;
// ********************************************************
// Wait for interrupt
// ********************************************************
void    waitForInterrupt(void){

}

// *********************************************************
// Routine to halt PRU #1
// Sets halt flag bit in state variable
// *********************************************************
void  haltPRU(void) {
   mem->state = M_HALT ;
   return ;
}

// *********************************************************
// Routine to apply hard brake
// *********************************************************

void hardBrake(void) {
   mem->state = M_HARD_BRAKE ;
   return ;
}

// **********************************************************
// Routine to coast to a stop
// **********************************************************

void coast(void) {
   mem->state = 0 ;
   return ;
}
	
// ************************************************************
// Routine to implement PID loop on a single DC motor
// Using the velocity or differential PID
//
// delta_p = Kp * error
// delta_i = Ki * (error - past_error)
// delta_d = Kd * (error - past_error + 2 * past_past_error)
//
// output = previous_output + (delta_p + delta_i + delta_d)
//
// Errors in Q0 format.
// enc in Q 0 format.
// Kp, Ki, Kd are in Q12 format,
// delta_p, delta_i, and delta_d are in Q12 format
// Output in Q0 format.
//
// **************************************************************

int32_t PID(DCmotor_t * motor, int32_t enc) {
   int32_t   delta_p, delta_i, delta_d ;
   int32_t   scr ;
   int32_t   out ;

// Update past_error and past_past_error
  
   motor->e2 = motor->e1 ;
   motor->e1 = motor->e0 ;

// Compute new error term

   motor->e0 = FSUB(motor->setpoint, enc) ;

// Compute delta_p, delta_i, and delta_d

   delta_p = FMUL(motor->Kp, motor->e0, 0) ;
   scr = FSUB(motor->e0, motor->e1) ;
   delta_i = FMUL(motor->Ki, scr, 0) ;
   scr = FADD(scr, motor->e2 << 1) ;
   delta_d = FMUL(motor->Kd, scr, 0) ;
   scr = FADD(delta_p, delta_i) ;
   scr = FADD(scr, delta_d) ;

// Convert the delta from Q12 to Q0 format

   scr = FCONV(scr, Q12, Q0) ;
   out = FADD(motor->PWMout, scr) ;

// Make sure out is in range

   if (out > motor->PWMmax) {
       out = motor->PWMmax ;
   } else {
       if (out < motor->PWMmin) {
           out = motor->PWMmin ;
       } // end if
   } // end if-then-else

// Save the output and also return it

   motor->PWMout = out ;   
   return out ;
}


// ******************************************************************
// Routine to move.
// *********************************************************************

void move(void) { 
   int        i ;
   DCmotor_t  *motor ;
   int32_t    state ;

// Set the errors to zero

   for (i=0; i<NUM_MOTORS; i++) {
      mem->motor[i].e0 = 0 ;
      mem->motor[i].e1 = 0 ;
      mem->motor[i].e2 = 0 ;
   }

// Look at direction so we can set the state correctly

   state = 0 ;
   if (mem->motorENA[M1]) {
        if (mem->motor[M1].wheelDirection == CW) {
           state |= M1_CW ;
        } else {
           state |= M1_CCW ;
        } // end if-then-else
        if (mem->motor[M1].brakeType == HARD) {
           state |= M_HARD_BRAKE ;
        } // end if
   } // end if

   if (mem->motorENA[M2]) {
        if (mem->motor[M2].wheelDirection == CW) {
           state |= M2_CW ;
        } else {
           state |= M2_CCW ;
        } // end if-then-else
        if (mem->motor[M2].brakeType == HARD) {
           state |= M_HARD_BRAKE ;
        } // end if
   } // end if

   if (mem->motorENA[M3]) {
        if (mem->motor[M3].wheelDirection == CW) {
           state |= M3_CW ;
        } else {
           state |= M3_CCW ;
        } // end if-then-else
        if (mem->motor[M3].brakeType == HARD) {
           state |= M_HARD_BRAKE ;
        } // end if
   } // end if
   if (mem->motorENA[M4]) {
        if (mem->motor[M4].wheelDirection == CW) {
           state |= M4_CW ;
        } else {
           state |= M4_CCW ;
        } // end if-then-else
        if (mem->motor[M4].brakeType == HARD) {
           state |= M_HARD_BRAKE ;
        } // end if
   } // end if
       
   state |= M_RUN ;
   for (i=0; i<NUM_MOTORS; i++) {
      mem->pwm[i] = 0 ;
      mem->enc[i] = 0 ;
   } // end for

// Write state out to shared memory

   mem->state = state ;

// Main loop
 
   int   loop = TRUE ;
   while (loop) {
      waitForInterrupt() ;
      for (i=0; i < NUM_MOTORS; i++) {
         if (mem->motorENA[i]) { 
            motor = &(mem->motor[i]) ;
            mem->pwm[i] = PID(motor, mem->enc[i]) ;
         } // end if
      } // end for
   } // end while
        
   return  ;
}

// *******************************************************************
//  Executes the command
//  Begins by looking at command argumets and setting
//  appropriate values in the moto structure.
// ******************************************************************	
	
void doCommand(int32_t  command_code) {

   switch (command_code) {
       case  NOP:   break ;
//
// FWD command has two arguments
// arg1 is the distance
// arg2 is the velocity
//
       case  FWD:   move() ;
                    break ;

//
// BWD command has two arguments
// arg1 is the distance
// arg2 is the velocity
//

       case  BWD:   move() ;
                    break ;

//
// ROT command has two arguments
// arg1 is the direction (CW or CCW)
// arg2 is the angle 
// arg3 is the velocity
//
       case  ROT:   move() ;
                    break ;

   } // end switch

} // end doCommand()
	
