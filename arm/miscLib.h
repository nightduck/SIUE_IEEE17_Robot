//
// Function prototype declarations
//

// Period of PWM clock in ms
// Measure on the scope and enter
// correct value here

#define     PWM_CLK_PERIOD     0.2

// PWM resolution modes

#define     BITS_IS_8          1
#define     BITS_IS_10         2
#define     BITS_IS_12         3

#define  GPIO_LED_PIN    44

// Function prototype declarations

void getGUIvars(char *str) ;
void GPIOinit(void) ;
void configPRU(void) ;
void memoryDump(void) ;

