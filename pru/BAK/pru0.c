//
// This is the motor control code that will run on PRU 0
// 
volatile register unsigned int __R31 ;

// Subroutine to perform initialization

int init(void) {
	return 0 ;
}


// The main routine

int main() {

// Perfrom initialization

	init() ;

// Just a test to show that you can use assembly instructions
// directly.  Also writing distintive value into data memory
// location 0 to prove that the C-program on arm can read it!

	__asm__ __volatile__(" ZERO  &r1, 4 \n") ;
	__asm__ __volatile__(" LDI   r2, 0xabab \n") ;
	__asm__ __volatile__(" SBBO  &r2, r1, 0, 4 \n") ;

// Exiting the application - send thee interrupt

   __R31 = 35;                      // PRUEVENT_0 on PRU0_R31_VEC_VALID 
   __halt();                        // halt the PRU
}
