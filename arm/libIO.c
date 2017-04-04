//
// Additional routines for 2017 robot
//

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdint.h>
#include    <math.h>
#include    <string.h>

#include   "bbbLib.h"
#include   "fix.h"
#include   "mem.h"
#include   "robotLib.h"
#include   "servo_driver.h"
#include   "libIO.h"
#include   "navigation.h"
#include   "die.h"

// Global variable
// Pointer to shared memory

  extern shared_memory_t  *shared_memory ;

// GUI variables

  extern  GUIvars_t       GUIvars ;

// Debug variable

  extern  int             debug ;

// ****************************************************
// Routine to initialize the GPIO we need for 2017 robot
// ****************************************************
void initRobotIO(void) {

//
// Here are pins for the 4-bit BCD number we want
// to display on the 7 segment display
//

// LED7SEG_A (Least significant bit) -> GPIO2[2]

   initPin(A) ;                    
   setPinDirection(A, OUT) ;
   setPinValue(A, ON) ;

// LED7SEG_B -> GPIO2[3]

   initPin(B) ;                    
   setPinDirection(B, OUT) ;
   setPinValue(B, ON) ;

// LED7SEG_C -> GPIO2[5]

   initPin(C) ;                    
   setPinDirection(C, OUT) ;
   setPinValue(C, ON) ;

// LED7SEG_D (Most significant bit) -> GPIO2[4]

   initPin(D) ;                    
   setPinDirection(D, OUT) ;
   setPinValue(D, ON) ;

// Green GO button

   initPin(GO) ;                    
   setPinDirection(GO, IN) ;

// Red STOP button

   initPin(STOP) ;                    
   setPinDirection(STOP, IN) ;

// Neo Clock

   initPin(NEO_CLK) ;                    
   setPinDirection(NEO_CLK, OUT) ;
   setPinValue(NEO_CLK, OFF) ;

// Stud enable

   initPin(STUD_ENA) ;                    
   setPinDirection(STUD_ENA, OUT) ;
   setPinValue(STUD_ENA, OFF) ;

// Stud status

   initPin(STUD_STATUS) ;                    
   setPinDirection(STUD_STATUS, IN) ;
   
   return ;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Function to turn on the stud finder and wait for it to initialize. Also
// serves as a pause in between pressing the start button and moving
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void initStudFinder() {
    setPinValue(STUD_ENA, ON);
    delay_ms(999);
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to perform cleanup. Turns off lights, the studfinder, etc
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void cleanup() {
    clearNeoPixelMap();             //Clear Neopixel
    NeoPixelDisplay();
    DPD(0);                         //Clear 7seg display
    setPinValue(STUD_ENA, OFF);     //Turn off stud finder
    haltDieMotor();		    //Stop motor, if moving
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to display value on the 7 segment display
//
// Die Point Dispay (DPD)
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void DPD(char  die_point) {
   switch (die_point) {
      case '0' :  setPinValue(A, OFF) ;
                  setPinValue(B, OFF) ;
                  setPinValue(C, OFF) ;
                  setPinValue(D, OFF) ;
                  break ;

      case '1' :  setPinValue(A, ON) ;
                  setPinValue(B, OFF) ;
                  setPinValue(C, OFF) ;
                  setPinValue(D, OFF) ;
                  break ;

      case '2' :  setPinValue(A, OFF) ;
                  setPinValue(B, ON) ;
                  setPinValue(C, OFF) ;
                  setPinValue(D, OFF) ;
                  break ;

      case '3' :  setPinValue(A, ON) ;
                  setPinValue(B, ON) ;
                  setPinValue(C, OFF) ;
                  setPinValue(D, OFF) ;
                  break ;

      case '4' :  setPinValue(A, OFF) ;
                  setPinValue(B, OFF) ;
                  setPinValue(C, ON) ;
                  setPinValue(D, OFF) ;
                  break ;

      case '5' :  setPinValue(A, ON) ;
                  setPinValue(B, OFF) ;
                  setPinValue(C, ON) ;
                  setPinValue(D, OFF) ;
                  break ;

      case '6' :  setPinValue(A, OFF) ;
                  setPinValue(B, ON) ;
                  setPinValue(C, ON) ;
                  setPinValue(D, OFF) ;
                  break ;

      default :   setPinValue(A, ON) ;
                  setPinValue(B, ON) ;
                  setPinValue(C, ON) ;
                  setPinValue(D, ON) ;
                  break ;
   }
   return ;
}

// 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to test the Adafruit servo controller
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void testServo(void) {  

//  int  i ;

// Reset the servo driver

   resetServoDriver() ;
   delay_ms(900) ;

// Set rep rate to 50 Hz

   setServoFREQ(50.0) ;
   delay_ms(900) ;

// Turn servo one direction and then back the other

   setServoPW(0, 150) ;
   delay_ms(900) ;

/*
   for (i=150; i<=350; i+=50) {
       setServoPW(0, i) ;
       delay_ms(900) ;
       delay_ms(900) ;
    }
*/
} // end testServo() ;

// 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to wait for GO button to be pressed
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void waitGO(void) {

   while (getPinValue(GO) == 1) { } ;
   return ;

} // end waitGO()


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to wait for GO button to be pressed
// 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void waitSTOP(void) {

   while (getPinValue(STOP) == 1) { } ;
   return ;

} // end waitSTOP()


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to clear Neo Pixel Display 
// 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void clearNeoPixelMap(void) {

   int row, col, byte ;

// Clear the map

   for (row = 0; row < 8; row++) {
      for (col = 0; col < 8; col++) {
         for (byte = 0; byte < 3; byte++) {
               shared_memory->NeoMap[row][col][byte] = 0 ;
         } // end byte
       } // end col
    } // end row

}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to load map for testing purposes
// 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void  loadNeoPixelMap(void) {
   int col ;

   clearNeoPixelMap() ;

// Make row 0 red
 
   for (col =0; col < 8; col++) {       
       shared_memory->NeoMap[0][col][RED] = 64 ;
   }

// Make row 1 green

   for (col =0; col < 8; col++) {       
       shared_memory->NeoMap[1][col][GREEN] = 64 ;
   }

// Make row 2 blue

   for (col =0; col < 8; col++) {       
       shared_memory->NeoMap[2][col][BLUE] = 64 ;
   }

// Make row 3 white

   for (col =0; col < 8; col++) {       
       shared_memory->NeoMap[3][col][BLUE] = 64 ;
       shared_memory->NeoMap[3][col][GREEN] = 64 ;
       shared_memory->NeoMap[3][col][RED] = 64 ;

 }

// Make row 4 blue

   for (col =0; col < 8; col++) {       
       shared_memory->NeoMap[4][col][BLUE] = 64 ;
   }

// Make row 5 RED

   for (col =0; col < 8; col++) {       
       shared_memory->NeoMap[5][col][RED] = 64 ;
   }

// Make pixel in row 7, column 0 yellow

   shared_memory->NeoMap[7][0][RED] = 150 ;
   shared_memory->NeoMap[7][0][GREEN] = 80 ;
   shared_memory->NeoMap[7][0][BLUE] = 2 ;

   return ;
} // end NeoPixelTestMap()

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// NeoPixelDisplay routine
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void NeoPixelDisplay(void) {

// Wait for IDLE state

//    printf("Wait for IDLE in Neo Display\n") ;
    waitForIdle() ;

// Start the Neo Pixel Display command

    shared_memory->command.code = NEO_PIXEL ;
    shared_memory->command.status = START ;

// Wait for PRU 0 to complete the command
// PRU 0 will perform tasks of switching from
// motor to neo-pixel mode and of telling PRU 1
// that it should run the command.
   
//    printf("Wait for COMPLETE in Neo Display\n") ;
    waitForComplete() ;

// After command is seen to complete then set
// command to a no-op and state as being idle

    shared_memory->command.code = NOP ;
    shared_memory->command.status = IDLE ;

//    printf("Leaving Neo Display\n") ;
   
// Return ;

    return ;

} // end NeoPixelDisplay() 
   
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Test 7 segment Display
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void test7seg(void) {

   DPD('b') ;
   delay_ms(900) ;
   waitGO() ;
   DPD('1') ;
   delay_ms(900) ;
   DPD('2') ;
   delay_ms(900) ;; 
   DPD('3') ;
   delay_ms(900) ;
   DPD('4') ;
   delay_ms(900) ;
   DPD('5') ;
   delay_ms(900) ; 
   DPD('6') ;
   delay_ms(900) ;
   DPD('b') ;
   delay_ms(900) ;
  
   return ;
}
 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to test stud finder interface
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void  testStudFinder(void) {

   setPinValue(STUD_ENA, ON) ;

   int  i ;

   for (i = 0 ; i < 20 ; i++) {
      if (getPinValue(STUD_STATUS) == 1) printf("RED LED OFF!\n") ;
      else printf("RED LED ON!\n") ;
      delay_ms(500) ;
   } // end for
      
   delay_ms(900) ;
   setPinValue(STUD_ENA, OFF) ;
   return ;
}    
// 
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Routine to test our new IO pins
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void testIO(void) {
   printf("Entering testIO()\n") ;

// Initialize GPIO that we need

   initRobotIO() ;
   testStudFinder() ;

   
   clearNeoPixelMap() ;
   NeoPixelDisplay() ;

// Test 7 segment display

   test7seg() ;

// Test Neo Pixel Display


   waitSTOP() ;
   delay_ms(900) ;
   loadNeoPixelMap() ;
   NeoPixelDisplay() ;

   delay_ms(900) ;
   delay_ms(900) ;
   delay_ms(900) ;
   delay_ms(900) ;
   delay_ms(900) ;
   delay_ms(900) ;
   clearNeoPixelMap() ;
   NeoPixelDisplay() ;


//   testServo() ;

   return ;
} // end testIO()

void readyLight() {
   clearNeoPixelMap();

   shared_memory->NeoMap[7][0][RED] = 150 ;
   shared_memory->NeoMap[7][0][GREEN] = 80 ;
   shared_memory->NeoMap[7][0][BLUE] = 2 ;

   NeoPixelDisplay() ;

   return ;
}

void printBoard(char board[7][7]) {
   int col, row ;

   clearNeoPixelMap() ;
//   printf("Map---------------------\n");
   for (row = 0; row < 7; row++) {
      for(col = 0; col < 7; col++) {
//           //DEBUGGING
//	if(board[row][col] & OBSTACLE) printf("X ");
//	else if(board[row][col] & MAIN_TUNNEL) printf("+ ");
//	else if(board[row][col] & UNDEFINED) printf("? ");
//	else printf("  "); 
        if(board[row][col] & MAIN_TUNNEL) {
            shared_memory->NeoMap[7-row][col][RED] = 64;
        } else if (board[row][col] & TUNNEL) {
            shared_memory->NeoMap[7-row][col][BLUE] = 64;
        } else if (board[row][col] & OBSTACLE) {
            shared_memory->NeoMap[7-row][col][GREEN] = 64;
        } else if (board[row][col] & UNDEFINED) {
            shared_memory->NeoMap[7-row][col][RED] = 32;
            shared_memory->NeoMap[7-row][col][BLUE] = 32;
        } else if (board[row][col] & UNVISITED) {
            shared_memory->NeoMap[7-row][col][BLUE] = 32;
            shared_memory->NeoMap[7-row][col][GREEN] = 16;
//        printf("\n");
        }
      }
   }
//   printf("Map--------------------\n");

// Make pixel in row 0, column 0 yellow

   shared_memory->NeoMap[7][0][RED] = 150 ;
   shared_memory->NeoMap[7][0][GREEN] = 80 ;
   shared_memory->NeoMap[7][0][BLUE] = 2 ;

   NeoPixelDisplay();

   return ;
}

//Same as printBoard, but only prints tunnels
void printBoardFinal(char board[7][7]) {
   int col, row ;

   clearNeoPixelMap() ;
   for (row = 0; row < 7; row++) {
      for(col = 0; col < 7; col++) {
        if(board[row][col] & MAIN_TUNNEL) {
            shared_memory->NeoMap[7-row][col][RED] = 64;
        } else if (board[row][col] & TUNNEL) {
            shared_memory->NeoMap[7-row][col][BLUE] = 64;
        }
      }
   }

// Make pixel in row 0, column 0 yellow

   shared_memory->NeoMap[7][0][RED] = 150 ;
   shared_memory->NeoMap[7][0][GREEN] = 80 ;
   shared_memory->NeoMap[7][0][BLUE] = 2 ;

   NeoPixelDisplay();

   return ;
}

int pollStudFinder(void) {
    return !getPinValue(STUD_STATUS);
}
