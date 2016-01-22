//
// Here are some miscellaneous subroutines
//
//
// Report value read from line sensor
//

/*

void MonitorLineSensor(void) {

	BYTE	value ;
//	float   float_val ;

	LCDclear() ;
	LCDhome() ;
	LCDbacklight(OFF) ;
	LCDstr(1, 1, cstrcpy(str, "Line sensor:")) ;
	
	TurnLED(ON) ;
//
//  Read digital input on Port 0, bit 1
//
	while (TRUE) {
		value = DIN1_Data_ADDR & DIN1_MASK ; 
		LCDblank(2) ;
		if (value == 0) {
	         LCDstr(2, 8, cstrcpy(str, "WHITE")) ;
	    }
		else {
		  	 LCDstr(2, 8, cstrcpy(str, "BLACK")) ;
	    }
		LongWait(250) ;
	}
	return ;
}

//
// Report compass headings (once per second)
//

void MonitorCompass(void) {

	float	value ;
//
// Calibrate the compass
//
	LCDclear() ;
	LCDhome() ;
	LCDbacklight(OFF) ;
	LCDstr(1, 1, cstrcpy(str, "Calibrating compass")) ;
	LCDstr(2, 1, cstrcpy(str, "...about 20 sec ...")) ;
//
// Send 'C' to enter calibration mode
//
    I2C_TXbuffer[0] = 'C' ;
	I2Cwrite(COMPASS_I2C_ADDR, 1) ;
//
// Rotate robot 720 degrees
//
    rotate(CW, 720.0, 2.0) ;
//
// Send 'E' to exit calibration mode
//
    I2C_TXbuffer[0] = 'E' ;
	I2Cwrite(COMPASS_I2C_ADDR, 1) ;
//
// Start displaying compass headings
//
	LCDclear() ;
	LCDhome() ;
	LCDbacklight(OFF) ;
	LCDstr(1, 1, cstrcpy(str, "Compass heading:")) ;
	
	TurnLED(ON) ;
	while (TRUE) {
		value = CompassHeading() ;
		LCDblank(2) ;
		LCDfloat(2, 1, value) ;
		LongWait(1000) ;
	}

	return ;
}

///////////////////////////////////////////////////////////////////////////

//
// Monitor battery voltage
//

void MonitorBatteryVoltage(void) {
	float	Dist, BattVolt ;

	LCDclear() ;
	LCDhome() ;
	LCDbacklight(ON) ;
	LCDstr(1, 1, cstrcpy(str, "Battery Voltage")) ;
	LCDstr(3, 1, cstrcpy(str, "Optical Range")) ;
	while (TRUE) {
		BattVolt = GetBatteryVoltage(500) ;
		Dist = GetDistance(500);
		LCDblank(2) ;
		LCDblank(4) ;
		LCDfloat(2, 1, BattVolt) ;
		LCDfloat(4, 1, Dist) ;
		LongWait(1000) ;
	}
	TurnLED(ON) ;
}

//
// Test drive routine
//

void TestDrive(void) {
   float	speed ;
   float	x, y ;

	speed = 12.0 ;
	x = 44.0 ;
	y = 24.0 ;

	rotate(CW, 10.0, 2.0 ) ;
	rotate(CCW, 10.0, 2.0 ) ;	
	rotate(CCW, 10.0, 2.0 ) ;	
	rotate(CW, 10.0, 2.0 ) ;
	LongWait(500) ;
	servo(0, 900) ;
	fwd(2.0, 2.0) ;
	LongWait(500) ;
	servo(0, 2500) ;
	LongWait(500) ;
	bwd(2.0, 2.0) ;
	
	fwd(x, speed) ;
	right() ;
	fwd(y, speed) ;
	right()  ;
	fwd(x, speed) ;
	right() ;
	fwd(y, speed);
	right() ;
	
	LongWait(500) ;
	servo(0, 900) ;
	

	return ;
}
  

//
// Routine to test the serial port
//
void SerialPortTest(void) {

	char 	* strPtr ; 			             // Parameter pointer
//
//	Send a message out the serial port
//
	UART_1_CPutString("\r\nSIUE Stinger Robot Project V1.1\r\n") ;
	UART_1_CPutString("\r\n11 March 2010\r\n") ;
	UART_1_CPutString("\r\nWelcome to PSoC UART test program. V1.1 \r\n");

	UART_1_CmdReset(); 		// Initialize receiver/cmd

	UART_1_IntCntl(UART_1_ENABLE_RX_INT); // Enable RX interrupts
	
//	Counter8_WritePeriod(155); // Set up baud rate generator
//	Counter8_WriteCompareValue(77);

//	Counter8_Start(); // Turn on baud rate generator
//	UART_Start(UART_PARITY_NONE); // Enable UART
//	M8C_EnableGInt ; // Turn on interrupts


	while(1) {
		if(UART_1_bCmdCheck()) { // Wait for command
			if(strPtr = UART_1_szGetParam()) { // More than delimiter?
			    UART_1_CPutString("Found valid command\r\nCommand =>");
			    UART_1_PutString(strPtr); // Print out command
			    UART_1_CPutString("<\r\nParameters:\r\n");
				
			    while(strPtr = UART_1_szGetParam()) { // loop on each parameter
				   UART_1_CPutString(" <");
				   UART_1_PutString(strPtr); // Print each parameter
				   UART_1_CPutString(">\r\n");
			    }
		     }
		     UART_1_CmdReset(); // Reset command buffer
	    }
	}

	return ;
} 


*/