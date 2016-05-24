typedef struct{
    int32_t     currentCnt;
    int32_t     pastCnt;
    int32_t     setPoint
    int32_t     currentV;
    int32_t     pastV;
    int32_t     e0;
    int32_t     e1;
    int32_t     e2;
    int32_t     distance;
    int32_t     targetDistance;
    int32_t     PWMValue;
    int32_t     iState;
    int32_t     iMax;
    int32_t     pGain;
    int32_t     iGain;
    int32_t     dGain;
    int32_t     diameter;
    int32_t     ticsPerInch;
}   wheel_t;

typedef struct{
    wheel_t    M1;
    wheel_t    M2;
    wheel_t    M3;
    wheel_t    M4;
}   pid_sys;

