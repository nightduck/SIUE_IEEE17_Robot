//
// Routines that make doing fixed point operations easy
//

#define     PI        3.14159
#define     Q         12
#define     twoQ      24

#define     Q24     24
#define     Q12     12
#define     Q0      0

//
// Define a series of useful macros for performing fixed-point math
//

#define  FADD(op1,op2)      ( (op1) + (op2) )
#define  FSUB(op1,op2)      ( (op1) - (op2) )
#define  FMUL(op1,op2,q)    ((int32_t) (((int64_t) ((int64_t) (op1) * (int64_t) (op2))) >> q))

// #define  FDIV(op1,op2,q)    ( (int32_t) (((int64_t)(op1) << q)/ ((int64_t) op2 )) )

// Convert from a q1 format to q2 format

#define  FCONV(op1,q1,q2)     (((q2) > (q1)) ? ((op1) << ((q2)-(q1))) : ((op1) >> ((q1)-(q2))))

// General operation between op1 in q1 format and op2 in q2 format
// returning the result in q3 format

#define  FADDG(op1,op2,q1,q2,q3)   (FCONV(op1,q1,q3) + FCONV(op2,q2,q3))
#define  FSUBG(op1,op2,q1,q2,q3)   (FCONV(op1,q1,q3) - FCONV(op2,q2,q3))
#define  FMULG(op1,op2,q1,q2,q3)   (FCONV(FMUL(op1,op2,0), (q1)+(q2), q3))

// Convert a float to a fixed-point representation in q format

#define  TOFIX(op1, q)       ((int32_t) ((op1) * ((float) (1 << (q)))))

// Convert a fixed-point number back to a float

#define  TOFLT(op1, q)       ( ((float) (op1)) / ((float) (1 << (q))) )


