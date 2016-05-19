//
// This is the motor control code that will run on PRU 0
// 
#include <stdint.h>
#include "pru_cfg.h"
#include "pru_intc.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Mapping Constant table register to variable */

volatile pruCfg CT_CFG __attribute__((cregister("PRU_CFG", near), peripheral));
volatile far pruIntc CT_INTC __attribute__((cregister("PRU_INTC", far), peripheral));


//  Defines 

//#define PRU-3
#define PRU0
#define HOST1_MASK		(0x80000000)
#define HOST0_MASK		(0x40000000)
#define PRU0_PRU1_EVT		(16)
#define PRU1_PRU0_EVT		(18)
#define PRU0_ARM_EVT		(34)
#define	PRU0_RAM		(0x00000000)
#define PRU1_RAM		(0x00008000)
#define SHARE_RAM		(0x00010000)


// Bit 15 is P8-11
// Bit 14 is p8-16
// Bit 07 is p9-25
// Bit 05 is p9-27

#define TOGGLE_PRU_LED			(__R30 ^= (0x00008000)) //Bit 15
#define OFF_PRU_LED			(__R30 &= (0xFFFF7FFF))
#define ON_PRU_LED			(__R30 |= (0x00008000))
#define DISABLE_DRV			(__R30 |= (0x00000020)) //Bit 5 Neg logic
#define ENABLE_DRV			(__R30 &= (0xFFFFFFDF)) //Bit 5 Neg logic
#define PRU_SW_VALUE			(__R31  & (0x00004000))	//Bit 14
#define	ACC_IN1_VAL			(__R31  & (0x00000080))	//Bit 7

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Subroutine to perform initialization

void init(void) {

/* Configure GPI and GPO as Mode 0 (Direct Connect) */
	CT_CFG.GPCFG0 = 0x0000;

/* Clear GPO pins */
	//__R30 &= 0xFFFF0000;

/* Clear interrupt event 18 */
	
	CT_INTC.SICR = PRU1_PRU0_EVT ; 	

	return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// The main routine

void main() {

// Used by real-time scheduler

	unsigned int temp, TimeSlot, SliceCounter ;
	TimeSlot = 0 ;
	SliceCounter = 0 ;


// Perform initialization

	init() ;
		
//
// Writing the 4 PWM values into the shared memory block (address is 0x0001_0000)
// PRU 1 will grab them and place the values intr R7 - R10

	int	* p ;
	p = (int *) 0x00010000 ;

// Store the 4 PWM values into shared memory

	*p = 200 ;
	*(p+1) = 400  ;
	*(p+2) = 200 ;
	*(p+3) = 1600 ;

// We will use i to count intertupts
// After seeing specified number we quit
// We will also read wheel encoder values

	volatile int enc1, enc2, enc3, enc4 ;

	int i = 0;
	int flag = 1;
	
// Enable the Motor Driver signals

// Start the loop
	//TOGGLE_PRU_LED;
	while (flag == 1) { 
		
// Wait for the start of the new sample period i.e. interrupt from PRU 1
		if (__R31 & HOST0_MASK) {

// Clear interrupt event 18 
			CT_INTC.SICR = PRU1_PRU0_EVT ;

// Read the wheel encoder counters 

			enc1 = *(p+4) ;
			enc2 = *(p+5) ;
			enc3 = *(p+6) ;
			enc4 = *(p+7) ;
// Count the interrupts
 		
			i += 1 ;
		} 

//		__delay_cycles(5); 	

// Wait for 50 interrupts and then we will quit					
		if (i == 5) flag = 0 ;
		if (i == 5) ENABLE_DRV;
// Where we would call the PID routines

// Store the 4 PWM values into shared memory
		//__asm__ __volatile__(" MOV r30.b0, 0xFF\n") ;
		*p = 2048 ;
		*(p+1) = 400 ;
		*(p+2) = 3072 ;
		*(p+3) = 200 ;
	//	__R30 |= 0xFFFFFFFF;

// Implement simple non-premptive real-time scheduler
/*
			temp = SliceCounter++ ;
			TimeSlot |= temp ^ SliceCounter + 1 ;

// Do every 100 ms

			if (TimeSlot & (1 << 2)) { 
				TimeSlot ^= (1 << 2) ;
			}
		
// Do every 1.6 sec

			if (TimeSlot & (1 << 6)) { 
				TimeSlot ^= (1 << 6) ;
				//TOGGLE_LED;
			}
*/
	}


// Just a test to show that you can use assembly instructions
// directly.  Also writing distintive value into data memory
// location 0 to prove that the C-program on arm can read it!
// Pointing r1 to start of PRU shared memory block
/*
	__asm__ __volatile__(" LDI   r1, 1 \n") ;
	__asm__ __volatile__(" LSL   r1, r1, 16 \n") ;
	__asm__ __volatile__(" LDI   r2, 0xabcd \n") ;
	__asm__ __volatile__(" SBBO  &r2, r1, 0, 4 \n") ;
*/

// Exiting the application - PRU 0 -> ARM interrupt
//
// 
   //TOGGLE_PRU_LED;
   __R31 = 35;
   while(!PRU_SW_VALUE){
	TOGGLE_PRU_LED;
	i = 0;
	i = 1;
	i = 3;
	i = 4;
	i = 5;
   }                      // PRUEVENT_0 on PRU0_R31_VEC_VALID 
   DISABLE_DRV;
   OFF_PRU_LED;
   __halt();                        // halt the PRU

}



