// Declare a structure to hold the GUI variables

typedef struct {
    int      exitFlag ;
    int      sonarEna ;
    int      lineEna ;
    int      rtcEna ;
    int      accelEna ;
    int      motorType ;
    float    Kp ;
    float    Ki ;
    float    Kd ;
    float    samplePeriod ;
    float    wheelDiam ;
    float    turnRad ;
    float    ticsPerRev ;
    int      M1Ena ;
    int      M2Ena ;
    int      M3Ena ;
    int      M4Ena ;
    int      PWMresMode ;
} GUIvars_t ;
