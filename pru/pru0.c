//
// This is the motor control code that will run on PRU 0
// 

#include <stdint.h>
#include "pru_cfg.h"
#include "pru_intc.h"
#include "mem.h"
#include "pru0Lib.h"
#include "motorLib.h"
#include "pru0.h"

// Define input and output registers

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Mapping Constant table register to variable */

volatile pruCfg CT_CFG __attribute__((cregister("PRU_CFG", near), peripheral));
volatile far pruIntc CT_INTC __attribute__((cregister("PRU_INTC", far), peripheral));

// Global pointer to memory stucture

shared_memory_t   *mem ;

// Global variables that allow us to handle GPIO

int  *clrGPIO1_reg ;	// GPIO button/LED usage
int  *setGPIO1_reg ;	// GPIO button/LED usage
int  *readGPIO1_reg ;	// GPIO button/LED usage
int  *setGPIO3_reg ;	// DRV pin usage
int  *clrGPIO3_reg ;	// DRV pin usage

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Subroutine to perform PRU initialization
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void initPRU(void) {	
	CT_INTC.SICR = PRU1_PRU0_EVT ; 	
	initGPIO();
    return  ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Subroutine to wait for an interrupt.
// It also clears the interrupt
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void waitForInterrupt(void) {
   while (!(__R31 & HOST0_MASK)) { } ;  // wait for interrupt  
   CT_INTC.SICR = PRU1_PRU0_EVT ;       // clear interrupt
   return ;
}
 
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN PROGRAM STARTS HERE
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void main() {

// Point to 12 kB of shared memory

   mem = (shared_memory_t *) PRU_SHARED_MEM_ADDR ;

// Perform some PRU initialization tasks

   initPRU() ;

// Turn the PRU LED on

   ON_PRU_LED;

// Enable the motor driver signals

//   ENABLE_DRV;

// Keep implementing commands until we are told to exit

   while (mem->exitFlag) {
       switch (mem->command.status) {
           case  IDLE:      break ;

           case  START:     doCommand(mem->command.code) ;
                            break ;

           case  ACTIVE:    break ;

           case  COMPLETED: mem->command.status = IDLE ;
                            break ;

           case  ABORTED:   mem->command.status = IDLE ;
                            break ;                            
       } // end switch
    } // end while

// Disable the motor driver signals

//   DISABLE_DRV;

// Turn the PRU LED off

   OFF_PRU_LED;

//   GPIO1pin(LED_PIN, OFF) ;

   __R31 = 35;    // PRU 0 to ARM interrupt
   __halt();      // halt the PRU

} // end main













