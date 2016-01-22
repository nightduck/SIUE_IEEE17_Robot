//
// Library of routines to support the LCD03 module
//

#include <m8c.h>        // part specific constants and macros
#include <psocapi.h>    // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <string.h>
#include <psocgpioint.h>
//
#include "LCDlib.h"

//

extern    BYTE    I2C_RXbuffer[64] ;
extern    BYTE    I2C_TXbuffer[64] ;
extern    char    LCDbuf[21] ;

extern    void    I2Cread(BYTE, BYTE) ;
extern    void    I2Cwrite(BYTE, BYTE) ;


//
// Routine to print string to the LCD module which is connected
// to the I2C bus.  The routine accepts a null terminated string.
// We do not return until the data has been successfully delivered
// to the LCD module.
//
BOOL LCDprint(char * string) {
   	BYTE 	i, length ;
	
//
// Determine the length of the string
//
   	length = 0 ;
   	for (i = 0; i < 20; i++) {
     	if (*(string+i) == 0) {
	     	length = i ;
		 	break ;
		}
	}
 	if (length == 0) return FALSE ;

// Transfer string to I2C_TXbuffer.  Make first character a NULL

    I2C_TXbuffer[0] = 0x0C ;	
	for (i = 0; i < length; i++) I2C_TXbuffer[i+1] = *(string+i) ;
	length += 1 ;
//
// Send it out the I2C port
//
	I2Cwrite(LCD_I2C_ADDR, length) ;	

	return TRUE ;

}
//
// Routine to set cursor to home position
// Calls the LCDprint function
//
void LCDhome(void) {

	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x01 ;	// Code for home cursor
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;
	
	return ;
}

//
// Routine to clear the screen
// Calls the LCDprint function
//

void LCDclear(void) {

	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x0C ;	// Code for clear display
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;

	return ;
}

//
// Routine to hide the cursor on the LCD screen
// Calls the LCDprint function
//

void LCDhide(void) {

	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x04 ;	// Code for hide cursor
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;

	return ;
}
//
// Routine to show the blinking cursor on the LCD screen
// Calls the LCDprint function
//

void LCDshow(void) {

	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x06 ;	// Code for show blinking cursor
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;

	return ;
}
//
// Issue a carriage return - linefeed 
//
void LCDcrlf(void) {

	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x0D ;	// Code for cr lf
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;
}
//
// Issue a smart linefeed 
//
void LCDlf(void) {

	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x0A ;	// Code for smart linefeed
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;
}

//
// Set cursor (row and column)
//
void LCDcursor(BYTE row, BYTE col) {
   	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x03 ;	// Code for set cursor
	LCDbuf[1] = row ;	
	LCDbuf[2] = col ;	
	LCDbuf[3] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;
}
//
// Horizontal tab
//
void LCDhtab(void) {
   	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x09 ;	// Code for horizontal tab
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;
}

//
// Vertical tab
//
void LCDvtab(void) {
   	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x0B ;	// Code for vertical tab
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;
}

//
// Sets tab size
//
void LCDtabsize(BYTE size) {
   	BOOL 	LCDprint(char *) ;
	
	LCDbuf[0] = 0x0B ;	// Code for set tab size
	LCDbuf[1] = size ;	// Code for set tab size
	LCDbuf[2] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;
}

//
// Routine to print out a string at specified
// location on LCD module
// 
void LCDstr(BYTE row, BYTE col, char * string) {
 	void	LCDcursor(BYTE, BYTE) ;
   	BOOL 	LCDprint(char *) ;	
//
// Position the cursor
//
  	LCDcursor(row, col) ;
//
// Print the string
//
	LCDprint(string) ;
 	return ;
}
	
//
// Routine to print out an integer at specified
// location on LCD module
// 
void LCDint(BYTE row, BYTE col, int num) {
 	void	LCDcursor(BYTE, BYTE) ;
   	BOOL 	LCDprint(char *) ;	
	
	char    *string ;
//
// Position the cursor
//
  	LCDcursor(row, col) ;
//
// Print the string
//
    itoa(string, num, 10) ;
	LCDprint(string) ;
 	return ;
}   

//
// Routine to print out a BYTE at specified
// location on LCD module
// 
void LCDbyte(BYTE row, BYTE col, BYTE num) {
 	void	LCDcursor(BYTE, BYTE) ;
   	BOOL 	LCDprint(char *) ;	
	
	char    string[10] ;
	BYTE    mask ;
	int		i ;
//
// Position the cursor
//
  	LCDcursor(row, col) ;
//
// Print the string
//
    string[8] = 0x00 ;  // Null character
	mask = 0x80 ;
	for (i=0; i<8; i++) {
	   string[i] = '0' ;
	   if (mask & num) string[i] = '1' ;
	   mask = mask >> 1 ;
	}
	LCDprint(string) ;
 	return ;
}   


//
// Routine to print out a long integer at specified
// location on LCD module
// 
void LCDlong(BYTE row, BYTE col, long num) {
 	void	LCDcursor(BYTE, BYTE) ;
   	BOOL 	LCDprint(char *) ;	
	
	char    *string ;
//
// Position the cursor
//
  	LCDcursor(row, col) ;
//
// Print the string
//
    ltoa(string, num, 10) ;
	LCDprint(string) ;
 	return ;
}   

//
// Routine to print out a float at specified
// location on LCD module
// 
void LCDfloat(BYTE row, BYTE col, float num) {
 	void	LCDcursor(BYTE, BYTE) ;
   	BOOL 	LCDprint(char *) ;	
	int		status ;
	
	char    *string ;
//
// Position the cursor
//
  	LCDcursor(row, col) ;
//
// Print the string
//
    string = ftoa(num, &status) ;
	LCDprint(string) ;
 	return ;
}  

//
// Routine to blank a single line
//
void LCDblank(BYTE row) {
  	void	LCDcursor(BYTE, BYTE) ;
	BOOL 	LCDprint(char *) ;	
	char	blank[] = "                    " ;  
	
	LCDcursor(row, 1) ;
	LCDprint(blank) ;
	return ;
}

//
// Routine to turn backlight on and off
//
void LCDbacklight(BYTE state) {
	BYTE	code ;
   	BOOL 	LCDprint(char *) ;
	
	code = 0x00 ;
	if (state == ON) code = 0x13 ;
	if (state == OFF) code = 0x14 ;
	
	LCDbuf[0] = code ;	// Code for vertical tab
	LCDbuf[1] = 0x00 ;	// Null
	LCDprint(LCDbuf) ;	
}

// 
// Routine to wait for a key on keypad to be pressed
// and then return its ASCII value
//
BYTE GetKey(void) {
    BYTE 	ASCIIcode ;

	ASCIIcode = 0x30 ;
	
	return  ASCIIcode ;
}