/****************************************
 * Quick test of I2C routines
 * Using real-time clock
 * Also using the acclerometer
 ****************************************/

#include <stdio.h>
#include "bbbLib.h"
#include "i2c_adc.h"

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Main program
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

int main(void) {
  
   int adc_handle ;
   adc_handle = configADC() ;

   int i ;
   int  value ;
      
   for (i = 0; i < 20; i++) {
       value = readI2C_ADC(adc_handle) ;
       printf("i = %d and value = %d\n", i, value) ;
       delay_ms(990) ;
   }
   cleanupADC(adc_handle) ;

   return(0);
}


