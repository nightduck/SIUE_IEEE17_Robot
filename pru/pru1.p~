// Assembly language code to run on pru 1
// PWM generation and reading of wheel encoders

// Modified by gle on 26 Nov 2016 to handle the NeoPixel Array

#include    "./pru1.h"
.origin		0
.entrypoint	START

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

START:		      
            clear_REGS	
            movi32   sharedMem, PRU_SHARED_MEM_ADDR
			ocp_port_ENABLE
            read_pwm_res
            read_clk_cnt

// Initialize some registers for GPIO 

            gpio_setup
      
// Keep reading state from shared_memory
// Waiting for PRU 0 to tell us to run
  
WAIT_FOR_RUN_FLAG:  
            hard_brake
            get_state
            qbbc    WAIT_FOR_RUN_FLAG, run_flag
            zero_encoder_regs
//
// Determine which mode we are in 
// Either motor mode or display mode
// Added by gle:26Nov2016 to support NeoPixel array
 
            read_mode_value
            qbeq    MAIN, mode, #0   // Motor mode
            qba     NEOPIXEL
 
MAIN:		        
            send_pru0_interrupt
            get_state 

// See if run flag is still set
      
            qbbc    STOP, run_flag 

// clkCntReg tells us how many PWM clocks we
// should generate before we interrupt PRU0

            mov     i, clkCntReg
I_LOOP:
		    read_pwm_values	
            pwm_start 

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   

// Will do the following either 256, 1024, or 4096 times
// depending on user selected PWM resolution
// Want to do this as fast as we possible can.
// Completing j-loop constitutes one PWM clock cycle.
                  
			mov	     j, pwmResReg	 
J_LOOP:     
            check_encoder_edges

ENC_1_CHK:  enc_cnt      enc1, enc1_bit
ENC_2_CHK:  enc_cnt      enc2, enc2_bit		
ENC_3_CHK:  enc_cnt      enc3, enc3_bit
ENC_4_CHK:  enc_cnt      enc4, enc4_bit 

PWM_1_CHK:  pwm_timer    M1_ctrl, pwm1, M1_0, M1_1
PWM_2_CHK:  pwm_timer    M2_ctrl, pwm2, M2_0, M2_1
PWM_3_CHK:  pwm_timer    M3_ctrl, pwm3, M3_0, M3_1
PWM_4_CHK:  pwm_timer    M4_ctrl, pwm4, M4_0, M4_1

            dec     j			
			qbne	J_LOOP, j, 0
          
            dec     i
            qbne    I_LOOP, i, 0

// End of sample period
// Write encoder values into shared memory and go back to MAIN

			sbbo	&enc1, sharedMem, ENC_OS, 16 
            qba     MAIN
//
// We come here when run flag has been de-asserted
// Turn LED off!
//

STOP:
            hard_brake
			qbbs    HALT_PRU1, halt_flag
            qba     WAIT_FOR_RUN_FLAG

// HALT flag set .. we are quitting

HALT_PRU1:  
            hard_brake                   	
            send_ARM_interrupt    
            halt	

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// NEW CODE (gle:26Nov2016) 
//
// Routine to hande NeoPixel array
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//
// Point mapAddr at the start of 192 byte NeoPixel data
//
NEOPIXEL:
           mov   mapAddr, sharedMem
           add   mapAddr, mapAddr, MAP_OS           
//
// Load byteCnt with 192 and then grab a data byte 192 times
//
           mov   byteCnt, #192
GET_BYTE:  lbbo  data, mapAddr, 0, 1
           mov   bitCnt, #7
//
// Check to see if we have a 0 or a 1
//
CHK_BIT:   qbbc  IS0, data, bitCnt
//
// Here is what we do if it is a 1
//
IS1:       mov   j, T1H
           neo_clk_ON
ONE_DLY0:  dec   j
           qbne  ONE_DLY0, j, 0
           mov   j, T1L
           neo_clk_OFF
ONE_DLY1:  dec   j
           qbne  ONE_DLY1, j, 0
           qba   NEXT_BIT
//
// And here is what we do if it is a 0
//
IS0:       mov    j, T0H
           neo_clk_ON
ZERO_DLY0: dec   j
           qbne  ZERO_DLY0, j, 0
           mov   j, T0L
           neo_clk_OFF
ZERO_DLY1: dec   j
           qbne  ZERO_DLY1, j, 0
           qba   NEXT_BIT
NEXT_BIT:  dec   bitCnt
           qbne  CHK_BIT, bitCnt, 0xff
//
// We are done with the byte .. go get another
// if we haven't done all 192 bytes
// Advance the pointer (i.e. mapAddr) by 1 
//
           add   mapAddr, mapAddr, #1
           dec   byteCnt
           qbne  GET_BYTE, byteCnt, 0
//
// We are done
// We have to let PRU 0 know that we are done
// We should also clear the run bit
// And save back to shared memory
// Then  go back to the start
//  

           clr  stateReg.b1.t0 
           store_state
           send_pru0_interrupt
           qba   START



