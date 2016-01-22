
//
// Module to control a pair of DC motors.
// PID control.
// Commands accepted via I2C interface.
// An  LCD two line display also can be used for debugging.
//

//
// Added to support the line following routine
// Make true to enter line following mode
//
BOOL	LFmode ;

// LCD strings

char 	  str[21]  ;	                      // String for printing
char	  LCDbuf[21] ;                        // Buffer used by LCD module

// Timer interrupt variables

BOOL			TimesUp ;	

// Check for switch closure variables
//
BOOL           	PrevKeyState ;
BOOL           	CurKeyState ;

// I2C variables

BYTE           	I2C_RXbuffer[64] ;
BYTE           	I2C_TXbuffer[64] ;
//
// Motor variables
//

WheelStruct     RightWheel, LeftWheel ;

//
// Distance threshold
//

float			DistanceThreshold = -1.0 ;

//
// Lead foot control
//
BOOL	AllowLeadFoot = FALSE ;

//
//  Declare a structure to hold LF PID loop data
//

LFpidStruct   LFpid ;

//
// LCD related routines (LCDlib.c)
//
BOOL 	LCDprint(char * ) ;	
void	LCDhide(void) ;
void    LCDshow(void) ;
void	LCDhome(void) ;
void	LCDclear(void) ;
void    LCDcrlf(void) ;
void    LCDcursor(BYTE, BYTE) ;
void    LCDhtab(void) ;
void    LCDvtab(void) ;
void    LCDtabsize(BYTE) ;
void    LCDlf(void) ;
void    LCDstr(BYTE, BYTE, char *) ;
void    LCDint(BYTE, BYTE, int) ;
void    LCDbyte(BYTE , BYTE , BYTE ) ;
void    LCDfloat(BYTE, BYTE, float) ;
void    LCDlong(BYTE, BYTE, long) ;
void    LCDblank(BYTE) ;
void	LCDbacklight(BYTE) ;

//
// Initialization routines (init.c)
//
void   	initPSoC(void) ;              // Initialize 
//
// Utilility routines (utilities.c)
//
BOOL  	ScanForKeyPress(void) ;       // Checks for a switch pressed 
void   	Wait(long) ;	              // Waits specified number of usec
BOOL    LongWait(unsigned int) ;      // Waits for specified number of msec
void    SetAlarm(long)        ;       // Sets alarm for x usec
void    ClearAlarm(void);
void    TurnLED(BYTE) ;
//
// Motor routines (MotorLib.c)
//
BOOL   	SetDutyCycle(BYTE, BYTE) ;
void    SetPWMvalues(int, int) ;
void    InitWheels(void) ;
void   	Coast(void) ;  				  // Causes motors to coast to a stop
void    ApplyBrake(void) ;
void    ReleaseBrake(void) ;	
void   	ReadDecoderValues(void) ;	  // Reads left and right decoder counters
void   	StartCountingPulses(void) ;	  // Starts counting pulses from decoders
void    StopCountingPulses(void) ;    // Stops counting pulses from decoders
void   	PIDupdate(void) ;	
void    SetVelocity(float, float) ;
void    SetDirection(BYTE, BYTE) ;
void    SetDistance(float, float) ;
float   move(BYTE, BYTE, float, float) ;
void    rotate(BYTE, float, float) ;
void	right(void) ;
void    left(void) ;
float	fwd(float, float) ;
float	bwd(float, float) ;
float	TicsToInches(int) ;


//
// servo related routines (ServoLib.c)
//
BOOL   	servo(BYTE, int) ;			  // Sets servo # to specified value
void 	TurnLightSensor(int, int) ;
//
// Sensor routines (SensorLib.c)
//
float 	GetDistance(int) ;
float	GetBatteryVoltage(int) ;
BYTE	LineSensor(void) ;
float	CompassHeading(void) ;
void	I2Cread(BYTE, BYTE) ;
void	I2Cwrite(BYTE, BYTE) ;
float   GetLineSensorVoltage(int) ;
void 	LFmonitor(void);


