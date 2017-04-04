//
// A library of miscellaneous routines
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>

#include "fix.h"
#include "mem.h"
#include "gui.h"
#include "BBBlib.h"
#include "miscLib.h"

// Global variable that points to shared memory

extern  shared_memory_t  *shared_memory ;

// GUI variables

extern  GUIvars_t   GUIvars ;

// Debug variable

extern  int   debug ;

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to get GUI variables 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void getGUIvars(char *str) {
   sscanf(str, "%d:%d:%d:%d:%d:%d:%f:%f:%f:%f:%f:%f:%f:%d:%d:%d:%d:%d", 
          &GUIvars.exitFlag,
          &GUIvars.sonarEna,
          &GUIvars.lineEna,
          &GUIvars.rtcEna,
          &GUIvars.accelEna,
          &GUIvars.motorType,
          &GUIvars.Kp,
          &GUIvars.Ki,
          &GUIvars.Kd,
          &GUIvars.samplePeriod,
          &GUIvars.wheelDiam,
          &GUIvars.turnRad,
          &GUIvars.ticsPerRev,
          &GUIvars.M1Ena,
          &GUIvars.M2Ena,
          &GUIvars.M3Ena,
          &GUIvars.M4Ena,
		  &GUIvars.PWMresMode
          ) ;

   if (debug) {
          printf("exit flag is %d\n", GUIvars.exitFlag) ;
          printf("sonarEna is %d\n", GUIvars.sonarEna) ;
          printf("lineEna is %d\n", GUIvars.lineEna) ;
          printf("rtcEna is %d\n", GUIvars.rtcEna) ;
          printf("accelEna is %d\n", GUIvars.accelEna) ;
          printf("Kp is %f\n", GUIvars.Kp) ;
          printf("Ki is %f\n", GUIvars.Ki) ;
          printf("Kd is %f\n", GUIvars.Kd) ;
          printf("samplePeriod is %f\n", GUIvars.samplePeriod) ;
          printf("wheelDiam is %f\n", GUIvars.wheelDiam) ;
          printf("turnRad is %f\n", GUIvars.turnRad) ;
          printf("ticsPerRev is %f\n", GUIvars.ticsPerRev) ;
          printf("M1Ena is %d\n", GUIvars.M1Ena) ;
          printf("M2Ena is %d\n", GUIvars.M2Ena) ;
          printf("M3Ena is %d\n", GUIvars.M3Ena) ;
          printf("M4Ena is %d\n", GUIvars.M4Ena) ;
          printf("PWMresMode is %d\n", GUIvars.PWMresMode) ;
   } // end if
   return ;
}

// *********************************************
// Routine to initialize the GPIO we need
// *********************************************

void GPIOinit(void) {

   initPin(2) ;                   // GPIO0[2] Accel GPIO interrupt
   setPinDirection(2, IN) ;

   initPin(44) ;                  // GPIO1[12] GPIO LED
   setPinDirection(44, OUT) ;

   initPin(47) ;                  // GPIO1[15] GPIO SWITCH
   setPinDirection(47, IN) ;

   initPin(115) ;                 // GPIO3[19] DRV_ENABLE
   setPinDirection(115, OUT);     // Has negitive logics so needs to be
   setPinValue(115, ON);          // set high when setting up otherwise it may go crazy
   return ;
}

// ######################################################
// Routine to init configure PRUS
// ######################################################

void configPRU(void) {

// Tells us when to exit program from GUI mode
   
   shared_memory->exitFlag = GUIvars.exitFlag ;

// Not currently using delay

   shared_memory->delay = (0x1dcd6500 >> 3) ;

// Either DC or Servo

   shared_memory->motorType = GUIvars.motorType ;

// Motor enables

   shared_memory->motorENA[M1] = GUIvars.M1Ena ;
   shared_memory->motorENA[M2] = GUIvars.M2Ena ;
   shared_memory->motorENA[M3] = GUIvars.M3Ena ;
   shared_memory->motorENA[M4] = GUIvars.M4Ena ;

// Clear the state register (shuts down the PWM) 

   shared_memory->state = 0 ;
    
//
// Sample period in ms
// Divide by period of one pwm clock in ms
//

   float  tmp ;
   tmp = (GUIvars.samplePeriod / PWM_CLK_PERIOD) + 0.5 ;
   shared_memory->PWMclkCnt = (uint32_t) (tmp) ;

// PWM resolution

   switch (GUIvars.PWMresMode) {
      case BITS_IS_8:     shared_memory->PWMres = 255 ;
                         break ;
      case BITS_IS_10:   shared_memory->PWMres = 1023 ;
                         break ; 
      case BITS_IS_12:   shared_memory->PWMres = 4095 ;
                         break ;
   } // end switch

// Initialize DC motor structures

   int i ;
   for (i = 0; i < NUM_MOTORS; i++) {
        shared_memory->motor[i].Kp = TOFIX(GUIvars.Kp, Q) ;
        shared_memory->motor[i].Ki = TOFIX(GUIvars.Ki, Q) ;
        shared_memory->motor[i].Kd = TOFIX(GUIvars.Kd, Q) ;

        shared_memory->motor[i].wheelDiam = TOFIX(GUIvars.wheelDiam, Q) ;  
        tmp = GUIvars.ticsPerRev / (PI * GUIvars.wheelDiam) ;  
        shared_memory->motor[i].ticsPerInch = TOFIX(tmp, Q) ;    
   } // end i loop

   return ;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Dump of entire memory structure to a file
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void memoryDump(void) {

   FILE  *fid ;
   int   i ;

   fid = fopen("memory_dump.txt", "w") ;

   for (i = 0; i < NUM_MOTORS; i++) {
        fprintf(fid, "mem->motorENA[%d] = %d\n", i+1, shared_memory->motorENA[i]) ;     
   } // end i loop

   for (i = 0; i < NUM_MOTORS; i++) {
        fprintf(fid, "mem->pwm[%d] = %d\n", i+1, shared_memory->pwm[i]) ;    
        fprintf(fid, "mem->enc[%d] = %d\n", i+1, shared_memory->enc[i]) ; 
   } // end i loop

   for (i = 0; i < NUM_MOTORS; i++) {
        fprintf(fid, "mem->motor[%d].Kp = %d\n", i+1, shared_memory->motor[i].Kp) ;  
        fprintf(fid, "mem->motor[%d].Ki = %d\n", i+1, shared_memory->motor[i].Ki) ;    
        fprintf(fid, "mem->motor[%d].Kd = %d\n", i+1, shared_memory->motor[i].Kd) ;    
        fprintf(fid, "mem->motor[%d].wheelDiam = %d\n", i+1, shared_memory->motor[i].wheelDiam) ;  
        fprintf(fid, "mem->motor[%d].ticsperInch = %d\n", i+1, shared_memory->motor[i].ticsPerInch) ;       
   } // end i loop
 
   fprintf(fid, "mem->delay = %x\n", shared_memory->delay) ;
   fprintf(fid, "mem->state = %x\n", shared_memory->state) ;
   fprintf(fid, "mem->PWMclkCnt = %u\n", shared_memory->PWMclkCnt) ;
   fprintf(fid, "mem->PWMres = %u\n", shared_memory->PWMres) ;
   fprintf(fid, "mem->exitFlag = %d\n", shared_memory->exitFlag) ;
   fprintf(fid, "mem->motorType = %u\n", shared_memory->motorType) ;
   fprintf(fid, "mem->scr = %u\n", shared_memory->scr) ;
   fprintf(fid, "mem->interruptCounter = %u\n", shared_memory->interruptCounter) ;

   for (i=0; i<BUF_LEN; i++) {
      fprintf(fid, "mem->enc_data[%d] = %u\n", i, shared_memory->enc_data[i]) ;
   }

   fclose(fid) ;

   return ;
} // end memoryDump




