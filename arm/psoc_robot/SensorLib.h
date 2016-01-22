//
// I2C address for the compass module
// Documentation says $42 for writes but I think
// it is $21 because of the R/W bit confusion
//
#define		COMPASS_I2C_ADDR	0x21
//
// Analog inputs
// Port 4 is digital port used by light sensor
//

#define	   	OPTICAL_RANGE_SENSOR	0x01     // Port0_2
#define		BATTERY_MONITOR			0x03	 // Port0_6

#define		K_BATT		23.873
#define		ADC_FS		1024
#define     VREF        4.0

//
// Distance = exp((reading - C)/A) - B
//
#define		A		-0.1304
#define     B		-2.8734
#define		C		0.5145