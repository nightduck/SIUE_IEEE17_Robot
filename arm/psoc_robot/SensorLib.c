#include <m8c.h>        // part specific constants and macros
#include <psocapi.h>    // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <psocgpioint.h>

#include "SensorLib.h"
#include "utilities.h"
#include "LCDlib.h"


extern  BYTE    I2C_RXbuffer[64] ;
extern  BYTE    I2C_TXbuffer[64] ;

extern  void    Wait(long) ;


//
// Select one of 4 analog input channels
//
void SelectSensor(BYTE channel) {
   AMUX4_1_InputSelect(channel) ;
}

// 
// Routine to write to I2C device
// User must load I2C_TXbuffer before calling
//
void	I2Cwrite(BYTE addr, BYTE num_bytes) {

	I2CHW_1_bWriteBytes(addr, I2C_TXbuffer, num_bytes, I2CHW_1_CompleteXfer) ;
	while (!(I2CHW_1_bReadI2CStatus() & I2CHW_WR_COMPLETE)) ;
  	I2CHW_1_ClrWrStatus() ;
    return ;  
}

// 
// Routine to read from I2C device
// Data returned in global I2C_RXbuffer
//

void	I2Cread(BYTE addr, BYTE num_bytes) {

    I2CHW_1_fReadBytes(addr, I2C_RXbuffer, num_bytes, I2CHW_1_CompleteXfer) ;
 	while(!(I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE) ) ;	
	I2CHW_1_ClrRdStatus() ;	
  	return ;	
}

//
//	Read the compass heading.
//  Returns compass heading as a float with a resolution of 0.1 degrees.
//

float   CompassHeading(void) {

    float   degrees ;
//
// Send the command 'A' to the compass module
// This routine will take about 6 ms.  Upon receiving the 'A', the
// compass module will calibrate, take some readings, average, and
// then send back a heading ...
//
    I2C_TXbuffer[0] = 0x41 ;			
    I2Cwrite(COMPASS_I2C_ADDR, 1) ;
//
// Turn the two bytes into a compass heading with a resolution
// of 0.1 degrees
//
	I2Cread(COMPASS_I2C_ADDR, 2) ;
   	degrees = 256.0 * I2C_RXbuffer[0] + I2C_RXbuffer[1] ;
   	degrees /= 10.0 ;
   	return  degrees ;
}

// 
//
// Routine to get distance form optical range finder in inches
//

float GetDistance(int NumSamples) {

    int 	i ;
	int 	ADCsample ;
	float	distance ;
	float	value ;
	float	sum ;
	
	void	SelectSensor(BYTE) ;

//
// Select optical range sensor
//
	SelectSensor(OPTICAL_RANGE_SENSOR) ;
//
// Set the gain on PGA
//
	PGA_1_SetGain(PGA_1_G1_00) ;

//
// Start taking samples with ADC
// Take NUM_SAMPLES samples but keep only the last
// The ADC is 2nd order and 64X oversampling.
// Clock is VC2 (480 kHz)
// Sample Rate = Clock rate / 4 * 64 = 1.875 kS/s
//

	DelSig_1_StartAD() ;
	
	for (i =0 ; i < 10 ; i++) {
	   while (!DelSig_1_fIsDataAvailable()) ;
	   ADCsample = DelSig_1_iGetDataClearFlag() ;
	}

	sum = 0.0 ;
	for (i =0 ; i < NumSamples; i++) {
	   while (!DelSig_1_fIsDataAvailable()) ;
	   ADCsample = DelSig_1_iGetDataClearFlag() ;
	   sum += (float) ADCsample ;
	}
   	value = (sum / NumSamples) / ADC_FS ;
	distance = exp((value-C)/A) - B ;
//
// Turn off the ADC
//
	DelSig_1_StopAD() ;

	return distance ;
//	return value ;
}

//
// Routine to determine current battery voltage
//
float GetBatteryVoltage(int NumSamples) {
    int 	i ;
	int 	ADCsample ;
	float	voltage ;
	float	sum ;
	
	void	SelectSensor(BYTE) ;

//
// Select battry monitor sensor
//
	SelectSensor(BATTERY_MONITOR) ;
//
// Set gain on PGA
//
	PGA_1_SetGain(PGA_1_G1_00) ;
//
// Start taking samples with ADC
// Take NUM_SAMPLES samples but keep only the last
// The ADC is 2nd order and 64X oversampling.
// Clock is VC2 (480 kHz)
// Sample Rate = Clock rate / 4 * 64 = 1.875 kS/s

	DelSig_1_StartAD() ;

	for (i =0 ; i < 10 ; i++) {
	   while (!DelSig_1_fIsDataAvailable()) ;
	   ADCsample = DelSig_1_iGetDataClearFlag() ;
	}
//
//  Lets take 500 samples (about 1/4 sec) and average
//  them.
//
	sum = 0.0 ;
	for (i =0 ; i < NumSamples ; i++) {
	   while (!DelSig_1_fIsDataAvailable()) ;
	   ADCsample = DelSig_1_iGetDataClearFlag() ;
	   sum += (float) ADCsample ;
	}	
//
// Convert the ADC sample

   	voltage = K_BATT * ((sum / NumSamples) / ADC_FS) ;
//
// Turn off the ADC
//
	DelSig_1_StopAD() ;

	return voltage ;
}



