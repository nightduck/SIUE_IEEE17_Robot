
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// Define some useful subroutines
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#include <stdint.h>
#include "pru_cfg.h"
#include "pru_intc.h"
#include "pru0Lib.h"

// Global variables that allow us to handle GPIO

extern   int   *clrGPIO0_reg ;
extern   int   *setGPIO0_reg ;
extern   int   *readGPIO0_reg ;

extern   int   *clrGPIO1_reg ;
extern   int   *setGPIO1_reg ;
extern   int   *readGPIO1_reg ;

extern   int   *clrGPIO2_reg ;
extern   int   *setGPIO2_reg ;
extern   int   *readGPIO2_reg ;

extern   int   *clrGPIO3_reg ;
extern   int   *setGPIO3_reg ;
extern   int   *readGPIO3_reg ;


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// GPIO intialization
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void initGPIO(void) {
   clrGPIO0_reg = (int *) (GPIO0 | GPIO_CLEARDATAOUT) ;
   setGPIO0_reg = (int *) (GPIO0 | GPIO_SETDATAOUT) ; 
   readGPIO0_reg = (int *) (GPIO0 | GPIO_DATAIN) ;

   clrGPIO1_reg = (int *) (GPIO1 | GPIO_CLEARDATAOUT) ;
   setGPIO1_reg = (int *) (GPIO1 | GPIO_SETDATAOUT) ; 
   readGPIO1_reg = (int *) (GPIO1 | GPIO_DATAIN) ;

   clrGPIO2_reg = (int *) (GPIO2 | GPIO_CLEARDATAOUT) ;
   setGPIO2_reg = (int *) (GPIO2 | GPIO_SETDATAOUT) ; 
   readGPIO2_reg = (int *) (GPIO2 | GPIO_DATAIN) ;

   clrGPIO3_reg = (int *) (GPIO3 | GPIO_CLEARDATAOUT) ;
   setGPIO3_reg = (int *) (GPIO3 | GPIO_SETDATAOUT) ; 
   readGPIO3_reg = (int *) (GPIO3 | GPIO_DATAIN) ;
   return ;  
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Subroutine to write to GPIO0 pin
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void GPIO0pin(int pin, int value) {
   switch (value) {
      case OFF:   *clrGPIO0_reg = *readGPIO0_reg  | (1 << pin) ;

                  break ;
      case ON:    *setGPIO0_reg = *readGPIO0_reg |  (1 << pin) ;
                  break ;      
   }
   return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Subroutine to write to GPIO1 pin
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void GPIO1pin(int pin, int value) {
   switch (value) {
      case OFF:   *clrGPIO1_reg = *readGPIO1_reg  | (1 << pin) ;
                  break ;
      case ON:    *setGPIO1_reg = *readGPIO1_reg |  (1 << pin) ;
                  break ;      
   }
   return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Subroutine to write to GPIO2 pin
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void GPIO2pin(int pin, int value) {
   switch (value) {
      case OFF:   *clrGPIO2_reg = *readGPIO2_reg  | (1 << pin) ;

                  break ;
      case ON:    *setGPIO2_reg = *readGPIO2_reg |  (1 << pin) ;
                  break ;      
   }
   return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Subroutine to write to GPIO3 pin
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void GPIO3pin(int pin, int value) {
   switch (value) {
      case OFF:   *clrGPIO3_reg = *readGPIO3_reg  | (1 << pin) ;
                  break ;
      case ON:    *setGPIO3_reg = *readGPIO3_reg |  (1 << pin) ;
                  break ;      
   }
   return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to enable buffers
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void enableBuffers(void) {
   GPIO3pin(DRV_PIN, 0) ;
   return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to disable buffers
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void disableBuffers(void) {
   GPIO3pin(DRV_PIN, 1) ;
   return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Subroutine slowly blink LED
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void blinkLED(void) {
   static int LEDstate = OFF ; 
   if (LEDstate == ON) {
       GPIO1pin(LED_PIN, OFF) ;
       LEDstate = OFF ;
   } else {
       GPIO1pin(LED_PIN, ON) ;
       LEDstate = ON ;
   } // end else

   return ;
} // blink LED



