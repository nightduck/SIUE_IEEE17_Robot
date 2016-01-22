//
// Assembly language code to run on pru 1
//
// We will 
// - flash led 
// - until button pressed and then exit
//
// r30.t0 => p8-45 (output) => LED for user feedback
// r31.t9 => p8-29 (input) => switch for user input
//
// r31.t1 => wheel encoder 1 => P8-46 (input) 
// r31.t2 => wheel encoder 2 => P8-43 (input) 
// r31.t3 => wheel encoder 3 => P8-44 (input)
// r31.t4 => wheel encoder 4 => P8-41 (input)
//
// r30.t5 => PWM output 1 => P8-42
// r30.t6 => PWM output 2 => P8-39
// r30.t7 => PWM output 3 => P8-40
// r30.t8 => PWM output 4 => P8-27
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
//
// These wil get read in at start of sample period from fro PRU 0
//
// r7 => stores duration high (1 - 8190) for PWM 1
// r8 => stores duration high (1 - 8190) for PWM 2
// r9 => stores duration high (1 - 8190) for PWM 3
// r10 => stores duration high (1 - 8190) for PWM 4
//
// r11 => stores index j
// r29 => used for NOP
//
//
// Pseudo code
//
// START: 
// Turn LED OFF i.e. clr r30.t0
// Clr r1 through 29
// Send interrupt to PRU 0
// LOOP: 
// Toggle LED  (R30.t0)
// Read in PWM high periods from PRU 0 into r7 - r10
// Turn all 4 PWM outputs ON (r30.t8, r30.t7, r30.t6, r30.t5)
// for (i = 8192, i != 0, i--) {  // use r0 for i
//		read decoder inputs into r6
//		if (posedge on wheel encoder 1) incr r1
//		if (poseedge on wheel encoder 2) incr r2
//		if (posedge on wheel encoder 3) incr r3
//		if (posedge on wheel encoder 4) incr r4
//		if (r7 == 0) clr r30.t5 else r7--
//		if (r8 == 0) clr r30.t6	 else r8--
//		if (r9 == 0) clr r30.t7 else r9--
//		if (r10 == 0) clr r30.t8 else r10--
//		for (j = DELAY, j != 0, j--) { } // stall
//		r5 = r6 
//		Send interrupt to PRU 0
// }  // Loop should take exactly 50 ms to execute
// if (button not pressed) goto LOOP else HALT
//		

.origin		0
.entrypoint	START

#define		INS_PER_US			200			// 5 ns per instructions
#define	 	INS_PER_DELAY_LOOP	2			// 2 instructions per delay loop

// 3 usec delay

#define		DELAY				3 * (INS_PER_US / INS_PER_DELAY_LOOP)

// Number of times to loop (i.e. 14-bit PWM)

#define		TIMES				16384

#define		PRU_R31_VEC_VALID	32			// allows notification of program completion
#define		PRU_EVTOUT_1		4			// event number that is sent back

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

START:		clr		r30.t0					// turn LED OFF
			zero	&r1, 116				// Clear r1-r29 (29 * 4 = 116 bytes)

// Grab the value we need for our delay loop
// Host would have written it into location 0 of data memory
// r28 will contain the value

			zero	&r29, 4					// Point at location 0 in data memory
			lbbo	r28, r29, 0, 4			// unsigned int is 4 bytes long

MAIN:		xor		r30, r30, #1			// toggle bit r30.t0 (i.. LED) 

// Send interrput to PRU 0 to let it know about start of sample period!!!

			
// Zero r1-r4
			
			zero	&r1, 16					// Zero the wheel encoder counters

// Need to save the wheel encoder input values from past sample period

			mov		r5, r6					// Copy r6 (new) wheel encoder values to r5 (old)

// Read in R7 - R10 (PWM high times) from PRU0
// For time being just set the r7-r10 registers

			mov		r7, #500
			mov		r8, #4000
			mov		r9, #8000
			mov		r10, #12000

			or		r30.b0, r30.b0, #0xe0		   	// make all PWM outputs high
			or		r30.b1, r30.b1, #0x01		   	// make all PWM outputs high

// We will check the wheel encoders 16,384 times each sample period (50 ms)
// So we need to do in about 3.05 us

			mov		r0, #TIMES		 		// set i = 16384
I_LOOP:		mov		r6.b0, r31.b0			// read wheel encoders

// We can find rising edges by takine ~r5 & r6

			not		r5.b0, r5.b0			// r5 <= ~r5
			and		r5.b0, r5.b0, r6.b0		// r5 <= r5 & r6

// Wheel encoder #1

 			qbbc	ENC1_0, r5.t0
			add		r1, r1, #1				// inc posedge counter
			qba		ENC1_1
ENC1_0:		mov		r29, r29				// nop
			qba		ENC1_1			
ENC1_1:		lsr		r5, r5, 1				// get next bit

// Wheel encoder #2
		
 			qbbc	ENC2_0, r5.t0
			add		r2, r2, #1				// inc posedge counter
			qba		ENC2_1
ENC2_0:		mov		r29, r29				// nop
			qba		ENC2_1			
ENC2_1:		lsr		r5, r5, 1				// get next bit

// Wheel encoder #3

 			qbbc	ENC3_0, r5.t0
			add		r3, r3, #1				// inc posedge counter
			qba		ENC3_1
ENC3_0:		mov		r29, r29				// nop
			qba		ENC3_1			
ENC3_1:		lsr		r5, r5, 1
	
// Wheel encoder #4
		
 			qbbc	ENC4_0, r5.t0
			add		r4, r4, #1				// inc posedge counter
			qba		ENC4_1
ENC4_0:		mov		r29, r29				// nop
			qba		ENC4_1	
		
// PWM output #1

ENC4_1:		qbne	PWM1_0, r7, 0			// Time to low?
			clr		r30.t5					// Bring output low
			qba		PWM1_1
PWM1_0:		sub		r7, r7, 1				// Dec pwm high counter
			mov		r29, r29				// nop

// PWM output #2

PWM1_1:		qbne	PWM2_0, r8, 0			// Time to go low?
			clr		r30.t6					// Bring output low
			qba		PWM2_1					
PWM2_0:		sub		r8, r8, 1				// Dec pwm high counter
			mov		r29, r29				// nop

// PWM output #3

PWM2_1:		qbne	PWM3_0, r9, 0			// Time to go low?
			clr		r30.t7					// Bring output low
			qba		PWM3_1
PWM3_0:		sub		r9, r9, 1				// Dec pwm high counter
			mov		r29, r29				// nop

// PWM output #4

PWM3_1:		qbne	PWM4_0, r10, 0			// Time to go low?
			clr		r30.t8					// Bring output low
			qba		PWM4_1
PWM4_0:		sub		r10, r10, 1				// Dec pwm high counter
			mov		r29, r29				// nop

// Kill some time before going back around

PWM4_1:		mov		r11, r28				// store the length of delay in r11 (j)
J_LOOP:		sub		r11, r11, 1				// Dec r11
			qbne	J_LOOP, r11, 0
// i loop

			sub		r0,	r0, 1				// Dec i
			qbne	I_LOOP, r0, 0

// If user button pressed, then let ARM know we are halting and then halt!
			
			qbbc	MAIN, r31.t9		
			mov 	r31.b0, PRU_R31_VEC_VALID | PRU_EVTOUT_1
			halt	

