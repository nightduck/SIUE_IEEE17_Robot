//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Motor Lib defines
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//---
// Direction codes
//---

#define     M1              0
#define     M2              1
#define     M3              2
#define     M4              3

#define     NUM_MOTORS      4
#define     BUF_LEN         200


// 
// Here the codes for status of commands
//

#define   IDLE          0
#define   START         1
#define   ACTIVE        2
#define   COMPLETED     3
#define   ABORTED       4

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// A structure that decribes a command from
// the ARM to PRU 0
// We'll assume no more than three arguments
// but we can always add more.
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

typedef struct {
    int32_t     code ;
    int32_t     status ;
    int32_t     arg1 ;
    int32_t     arg2 ;
    int32_t     arg3 ;
} command_t ;

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// A DC motor structure
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

typedef struct {
    int32_t     setpoint ;         // desired velocity (in tics)
    int32_t     distance ;         // dist in tics (actual)
    int32_t     targetDistance ;   // dist in tics (desired)
    int32_t     wheelDirection ;   // CW or CCW
    int32_t     brakeType ;        // COAST or HARD
    int32_t     e0 ;               // current error
    int32_t     e1 ;               // past error
    int32_t     e2 ;               // past "past error"
    int32_t     Kp ;               // proportional gain (Q12)
    int32_t     Ki ;               // integral gain (Q12)
    int32_t     Kd ;               // deriviative gain (Q12)
    int32_t     wheelDiam ;        // diameter in inches (Q12)
    int32_t     ticsPerInch;       // encoder tics per inch
    int32_t     PWMmin ;           // minumum PWM out allowed
    int32_t     PWMmax ;           // maximum PWM out allowed
    int32_t     PWMout ;           // PWM output
}   DCmotor_t;



// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Our shared memory structure
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

typedef struct {
    uint32_t     pwm[NUM_MOTORS] ;      // shared mem byte os of 0
    uint32_t     enc[NUM_MOTORS] ;      // os of 16
    uint32_t     delay ;                // os of 32
    uint32_t     state ;                // os of 36
    uint32_t     PWMclkCnt ;            // os of 40
    uint32_t     PWMres ;               // os of 44
    uint32_t     exitFlag ;             // exit when true
    uint32_t     interruptCounter ;     // sample counter
    uint32_t     motorType ;            // DC or stepper
    uint32_t     motorENA[NUM_MOTORS] ; // Motor enables
    uint32_t     scr ;                  // scratchpad register   
    uint32_t     enc_data[BUF_LEN] ;    // Buffer of encoder data
    command_t    command ;              // Motor command structure
    DCmotor_t    motor[NUM_MOTORS] ;    // DC motor structure
}   shared_memory_t ;


