//
// Misc
//

//#define   WHEEL_DIAM_R      2.263
//#define   WHEEL_DIAM_L      2.263

#define   WHEEL_DIAM_R      3.0
#define   WHEEL_DIAM_L      3.0


#define   CNTS_PER_REV      624.0
#define   PI                3.14159
#define   DEG2RAD           0.017455

#define   RADIUS_CW         4.20
#define   RADIUS_CCW        3.95

#define   MAX_DIST			740.0
#define   MAX_SPEED         24.0
#define   MIN_DIST			0.1
#define	  MIN_SPEED			1.0
#define   TURN_SPEED        4.0
#define   UNIFORM      		0
#define   ACCEL       		1
#define   DE_ACCEL    		2
#define	  REL_DIST			0.75
#define   DELTA_V_PCNT      0.025

//
// T = 50, KP = 20.0, KI = 40.0, KD = 0.01 works well
//
// PID parameters
//
#define   KP          20.0
#define   KI          50.0
#define   KD          0.1
#define   T           50

#define   PWM_MIN     0
#define   PWM_MAX     2000

// Direction Codes

#define		FWD		0
#define		BWD		1
#define		CW		0
#define		CCW		1

//
// Wheel identifier
//
#define     RIGHT      0
#define     LEFT       1

//
// Added to support line following mode
// Define BLK and WHT
//
#define    BLACK     1
#define    WHITE     0
#define    W1        1
#define    W2        4
#define    KP_PID    1.0
#define    KI_PID    0.0
#define    KD_PID    0.0

//
// Define a wheel sructure
//

typedef   struct  {
   int      CurrentCnt ;       // Current value read from decoder counter
   int      PastCnt ;          // Past value read from decoder counter
   int      SetPoint ;         // Set point
   int      OrigSetPoint;      // Original set point requested (added for line following)
   int      CurrentV ;         // Current velocity
   int      PastV ;            // Past velocity
   int      e0 ;               // Current error
   int      e1 ;               // Last error
   int      e2 ;               // Last-Last error  
   int      Distance ;         // Distance in tics travelled by wheel
   int      TargetDistance ;   // Distance objective
   int      PWMvalue ;         // PWM value to be sent to wheel
   BYTE     Dir ;              // FWD or BWD
   float    iState ;           // Integral state
   float    iMax ;             // Maximum integrator state
   float    iMin ;             // Minimal integrator state
   float    pGain ;            // Proportional gain
   float    iGain ;            // Integral gain
   float    dGain ;            // Derivative gain
   float    Diameter ;
   float    TicsPerInch ;
   
} WheelStruct ;
   
//
// Define a line following structure
//
typedef   struct {
   int    w1 ;           	// Inner sensor weight
   int    w2 ;           	// Outer sensor weight
   BOOL	  L1 ;				// L1 sensor value
   BOOL	  L2 ;				// L2 sensor value
   BOOL	  R1 ;				// R1 sensor value
   BOOL	  R2 ;				// R2 sensor value
   float  Kp ;        		// Proportional gain
   float  Ki ;	     		// Integral gain
   float  Kd ;        		// Derivative gain
   float  error ;        	// PID error term
   float  prev_error ;    	// Previous error
   float  integral ;     	// Integral of error
   float  derivative ;   	// Derivative of error
   float  output ;       	// PID output
} LFpidStruct ;
   
   
   
   
   
   
   
   