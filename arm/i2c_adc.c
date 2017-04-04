//
// Some routiness to allow use of adafruit i2c breakout
// board for TI ADS1115 ADC
//

// Need access to std i/o routines 

#include <stdio.h>

// Need access to i2c library funcitions

#include "bbbLib.h"
#include "i2c_adc.h"

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to configure ADC and open an I2C channel
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

int configADC(void) {

// Set up a write buffer

   unsigned char wr_buf[BUF_SIZE] ;

// Get a handle for the ADC
 
   int i2c_adc_handle = i2c_open(ADC_I2C_BUS, ADC_I2C_ADDR) ;

// Need to configure the ADC (+/- 5V range and one-shot mode)

   wr_buf[0] = ADC_CONFIG_REG ;

// Selection AIN0 and 5 volt range

   wr_buf[1] = 0x43 ;
   wr_buf[2] = 0x83 ;
   i2c_write(i2c_adc_handle, wr_buf, 3) ;

// Return the i2c adc handle

   return i2c_adc_handle ;

}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to cleanup after using ADC
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void  cleanupADC(int i2c_adc_handle) {

// Close the i2c channel

   i2c_close(i2c_adc_handle) ;

   return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to configure and read 16-bit value from ADC
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

int readI2C_ADC(int i2c_adc_handle) {

// Set up a read buffer

   unsigned char rd_buf[BUF_SIZE] ;

// Set up a write buffer

   unsigned char wr_buf[BUF_SIZE] ;

// Need to select the config register

   wr_buf[0] = ADC_CONFIG_REG ;

// Start the conversion

   wr_buf[1] = 0xc3 ;
   wr_buf[2] = 0x83 ;
   i2c_write(i2c_adc_handle, wr_buf, 3) ;

// Wait 10 ms to give ADC chance to convert

   delay_ms(10) ;

// Let's read the 16 bit value from the ADC

   wr_buf[0] = ADC_CONV_REG ;
   i2c_write_read(i2c_adc_handle, ADC_I2C_ADDR, wr_buf, 1, ADC_I2C_ADDR, rd_buf, 2) ;    

   int adcValue ;
   adcValue = (int) rd_buf[1] ;  
   adcValue &= 0xff ;
   adcValue |= ((int) rd_buf[0]) << 8 ;

   return adcValue;
}
