//
// Assembly language code to run on pru 1
//
// We will 
// - flash led 
// - until button pressed in pru0 then stop
//
//
// TO DO: implement backwards and forewards on the motor controlers
//
//
// GPIO2[16] > LED for user feedback
// GPIO1[15] > Switch for user input
// r31.t1 => wheel encoder M1 => P8-46 (input) 
// r31.t2 => wheel encoder M4 => P8-43 (input) 
// r31.t3 => wheel encoder M2 => P8-44 (input)
// r31.t4 => wheel encoder M3 => P8-43 (input)
// r30.t5  => PWM output 1-1 => P8-42
// r30.t9  => PWM output 1-2 => P8-29 -Keep low for now
// r30.t6  => PWM output 2-1 => P8-39
// r30.t10 => PWM output 2-2 => P8-28 -Keep low for now
// r30.t7  => PWM output 3-1 => P8-40
// r30.t11 => PWM output 3-2 => P8-30 -Keep low for now
// r30.t8  => PWM output 4-1 => P8-27
// r30.t0  => PWM output 4-2 => P8-45 -Keep low for now
//
// r0 => stores index i
//
// r1 => counts rising transitions on wheel encoder 1 input
// r2 => counts rising transitions on wheel encoder 2 input
// r3 => counts rising transitions on wheel encoder 3 input
// r4 => counts rising transitions on wheel encoder 4 input
//
// r5 => stores the old encoder inputs
// r6 => stores the new encoder inputs
// r12 => stores the encoder ticks edge finder value
// These wil get read in at start of sample period from fro PRU 0
//
// r7 => stores duration high (1 - 8190) for PWM 1
// r8 => stores duration high (1 - 8190) for PWM 2
// r9 => stores duration high (1 - 8190) for PWM 3
// r10 => stores duration high (1 - 8190) for PWM 4
//
// r11 => stores index j
// r16 => Used for GPIO_LED
// r15 => Used for GPIO_BUTTON
// r17 => Used for GPIO1_READ
// r18 => Used for GPIO2_SET
// r19 => Used for GPIO2_CLEAR
// r27 => Used to hold the address value of shared memory 0x00010000 (0x4a310000)
// r29 => used for NOP
//
//
// Pseudo code
//
// START: 
// Turn LED OFF i.e. clr r30.t0
// Clr r1 through 29
// Send interrupt to PRU 0
// Wait for return interrupt from PRU 0
// LOOP: 
// Toggle LED  (GPIO2[16])
// Read in PWM high periods from PRU 0 into r7 - r10
// Turn all 4 PWM outputs ON (r30.t8, r30.t7, r30.t6, r30.t5)
// for (i = 4096, i != 0, i--) {  // use r0 for i
//		read decoder inputs into r6
//		if (posedge on wheel encoder 1) incr r1
//		if (posedge on wheel encoder 4) incr r2
//		if (posedge on wheel encoder 2) incr r3
//		if (posedge on wheel encoder 3) incr r4
//		if (r7  == 0) clr r30.t5   else r7--
//		if (r8  == 0) clr r30.t6   else r8--
//		if (r9  == 0) clr r30.t7   else r9--
//		if (r10 == 0) clr r30.t8   else r10--
//		for (j = DELAY, j != 0, j--) { } // stall
//		r5 = r6 
//		Send interrupt to PRU 0
// }  // Loop should take exactly 50 ms to execute
// if (button press) goto LOOP else HALT
//		

// Don't think we need this but maybe at some point
//Here is how you enable the OCP master port
// Enable the OCP master port -- allows transfer of data to Linux userspace
//
//	LBCO    r0, C4, 4, 4     // load SYSCFG reg into r0 (use c4 const addr)
//	CLR     r0, r0, 4        // clear bit 4 (STANDBY_INIT)
//	SBCO    r0, C4, 4, 4     // store the modified r0 back at the load addr


#include        "./pru1.h"
.origin		0
.entrypoint	START


// Number of times to loop (i.e. 12-bit PWM)






// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//START:			
START:		        
                        zero	&r1, 116				// Clear r1-r29 (29 * 4 = 116 bytes)
                        GPIO_SETUP                                      // Setup Linux space GPIO pointers in registers
// Grab the value we need for our delay loop            
// Host would have written it into location 0 of data memory
// r28 will contain the value
			zero	&r29, 4					// Point at location 0 in data memory
			lbbo	r28, r29, 0, 4				// unsigned int is 4 bytes long

// Use r27 as a pointer to the PRU shared data RAM ($0001_0000)
// That is how we will transfer results to and from PRU 0
			ldi		r27, 1
			lsl		r27, r27, 16

MAIN:			//LED_ON                                         	// toggle bit GPIO2_6 (LED)                       


// Send interrput to PRU 0 to let it know about start of sample period!!!

			ldi		r31, PRU1_PRU0_INTERRUPT + 16
			
			
// Zero r1-r4
			
			zero		&r1, 16			// Zero the wheel encoder counters

// Need to save the wheel encoder input values from past sample period
                       mov		r5, r6			// Copy r6 (new) wheel encoder values to r5 (old)
                       
// Read in R7 - R10 (PWM high times) from PRU0
// For time being just set the r7-r10 registers
// Values will come from first four word locations in shared memory


//			mov		r7, #500
//			mov		r8, #4000
//			mov		r9, #8000
//			mov		r10, #12000
			//accessing memory from pru0

			lbbo	        r7, r27, 0, 16		// loading r7-r10
//			or        	r30.b0, r30.b0, #0xe0	// make all primary PWM outputs high
//			or		r30.b1, r30.b1, #0x01	// make all primary PWM outputs high			//Change this to a define value with PWM outputs going high in one instruction
//                      and             r30, r30.b1, #0xF1   // make all secondary PWM outputs low
//                      and             r30, r30.b0, #0xF7

                        mov             r30, 0x000001e0         //Make all the primary PWM outputs high (t5,t6,t7,t8)
                                                                //and ground the secondary (t9,t10,t11,t0)

// We will check the wheel encoders 4096 times each sample period (50 ms)
// So we need to do in about 3.05 us

			mov	r0, #TIMES	 		// set i = 4096
	I_LOOP:		mov	r6.b0, r31.b0			// read wheel encoders
                        //lsr     r6, r6, 1                       // remove bit 0
// We can find rising edges by taking ~r5 & r6

			not	r12.b0, r5.b0			// r12 <= ~r5
			and	r12.b0, r12.b0, r6.b0		// r12 <= r12 & r6

// Wheel encoder #1
 			qbbc	ENC1_0, r12.t1			// If set to 0 jump to ENC1_0
			add	r1, r1, #1			// inc posedge counter
			qba	ENC1_1				// jump over the nop
	ENC1_0:		mov	r29, r29			// nop
                        qba     ENC1_1			
//	ENC1_1:		lsr	r5, r5, 1			// get next bit
        ENC1_1:         mov     r29,r29

// Wheel encoder #4
		
 			qbbc	ENC4_0, r12.t2
			add	r4, r4, #1			// inc posedge counter
			qba	ENC4_1
	ENC4_0:		mov	r29, r29			// nop
                        qba     ENC4_1
//	ENC4_1:		lsr	r5, r5, 1			// get next bit
        ENC4_1:         mov     r29, r29
// Wheel encoder #2

 			qbbc	ENC2_0, r12.t3
			add	r2, r2, #1			// inc posedge counter
			qba	ENC2_1
	ENC2_0:		mov	r29, r29			// nop		
                        qba     ENC2_1
//	ENC2_1:		lsr	r5, r5, 1			// get next bit
        ENC2_1:         mov     r29, r29	
// Wheel encoder #3
		
 			qbbc	ENC3_0, r12.t4
			add	r3, r3, #1			// inc posedge counter
			qba	PWM1_0
	ENC3_0:		mov	r29, r29			// nop
			qba	PWM1_0				// branch to next line (mimics timing for lsr)
                        
                        		
// PWM output #1

	PWM1_0:		qbne	PWM1_1, r7, 0			// Time to low? if yes bring low if not dec counter
			clr	r30.t5				// 	Bring output low
			qba	PWM2_0				// 	Jump to next PWM
	PWM1_1:		sub	r7, r7, 1			// else Dec pwm high counter
			mov	r29, r29			// 	nop (Mimic Jump)

// PWM output #2

	PWM2_0:		qbne	PWM2_1, r8, 0			// Time to go low?
			clr	r30.t6				// Bring output low
			qba	PWM3_0				
	PWM2_1:		sub	r8, r8, 1			// Dec pwm high counter
			mov	r29, r29			// nop

// PWM output #3

	PWM3_0:		qbne	PWM3_1, r9, 0			// Time to go low?
			clr	r30.t7				// Bring output low
			qba	PWM4_0
	PWM3_1:		sub	r9, r9, 1			// Dec pwm high counter
			mov	r29, r29			// nop

// PWM output #4

	PWM4_0:		qbne	PWM4_1, r10, 0			// Time to go low?
			clr	r30.t8				// Bring output low
			qba	TIMEKILL			// Jump to the timeKiller -  done with PWM
	PWM4_1:		sub	r10, r10, 1			// Dec pwm high counter
			mov	r29, r29			// nop

// Kill some time before going back around
// First clear the LED
             

	TIMEKILL:	mov	r11, r28			// store the length of delay in r11 (j)
	J_LOOP:		sub	r11, r11, 1			// Dec r11
			qbne	J_LOOP, r11, 0
// Save the encoder counter values to shared memory
			sbbo	&r1, r27, 16, 16		// save r1-r4 to memory after the 4 PWM values 
// i loop               
			sub	r0,	r0, 1			// Dec i
			qbne	I_LOOP, r0, 0

// If user button pressed, then let ARM know we are halting and then halt!
	//AT THE MOMENT JUST BRANCH UNCONDITIONALLY TO MAIN AND CONTUINE THIS LOOP	
			//Need to add code here to poll the GPIO right now we are just going 
			//branch unconditionally and wait for the kernal to halt the pru
	        	//qbbc	MAIN, r31.t9
                        		
			//ADD CODE HERE TO POLL THE USER SPACE GPIO
                        BUTTON_CHK
                        
                        qbbc   MAIN, GPIO_BUTTON.t15            			
	        	LED_OFF
                        mov 	r31.b0, PRU_R31_VEC_VALID | PRU_EVTOUT_1
			
                        halt	




		
