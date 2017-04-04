// We will use I2C2 which is enumerated as 1 on the BBB (silly! I know)
// SCL on P9_17 (3.3 V tolerant)
// SDA on P9_18 (3.3 V tolerant)

#define 	ADC_I2C_BUS 		2

// Base address for the adc

#define 	ADC_I2C_ADDR 		0x48

// Write buffer size

#define		BUF_SIZE    		12

// ADC registers

#define		ADC_CONV_REG		0x00
#define     ADC_CONFIG_REG      0x01

// Useful defines

#define   	TRUE	1
#define		FALSE 	0

//Threshhold

#define         BOTTOM_IR_THRESHHOLD 20000

// 
// Function declaration

// Configures ADC and gets us an i2c handle

int configADC(void) ;

// Reads 16-bit value from ADC

int readI2C_ADC(int i2c_adc_handle) ;

// Cleanup

void cleanupADC(int i2c_adc_handle) ;

