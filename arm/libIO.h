//
// Function prototypes
//

void    testIO(void) ;
void    initRobotIO(void) ;
void    initStudFinder(void) ;
void    cleanup(void) ;
void    DPD(char die_point) ;
void    testServo(void) ;
void    waitGO(void) ;
void    waitSTOP(void) ;
void    test7seg(void) ;
void    loadNeoPixelMap(void) ;
void    NeoPixelDisplay(void) ;
void    clearNeoPixelMap(void) ;
void    testStudFinder(void) ;
void    readLight(void);
void    printBoard(char board[7][7]);
void    printBoardFinal(char board[7][7]);
int     pollStudFinder(void);

// Pin dewclarations

#define  A     66
#define  B     67
#define  C     69
#define  D     68

// GO and STOP buttons

#define  GO        27
#define  STOP      65
#define  NEO_CLK   26

// Stud enable and stud status pins

#define  STUD_ENA      23
#define  STUD_STATUS   22

// Pixel colors

#define  GREEN   0
#define  RED     1
#define  BLUE    2


