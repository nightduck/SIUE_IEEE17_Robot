//
// Library of routines which run on PRU0
// for handling the motors
//

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "mem.h"
#include "pru0Lib.h"
#include "motorLib.h"
#include "pru0.h"
#include "neopixel.h"

// Pointer to shared memory is a global variable

  extern   shared_memory_t   *mem ;


// *****************************************************
//
// gle added the following on 25 Nov 2016 to support NeoPixelDisplay
//
// *****************************************************

void NeoPixelDisplay(void) {

   int32_t  state ;

// Change command status to ACIVE

   mem->command.status = ACTIVE ;

// Change from motor to neo-pixel mode

   mem->mode = NEO_MODE ;

// Set the run bit
       
   state = 0 ;
   state |= M_RUN ;
   mem->state = state ;

// PRU 1 will de-assert the RUN bit before returning
// Wait for an interrupt from PRU 1

   waitForInterrupt() ;

// Set back to motor MODE

   mem->mode = MOTOR_MODE ;
   state = 0 ;
   mem->state = state ;

// We can return

   return ;

} // end NeoPixelDisplay() 

	
