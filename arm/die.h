#include <unistd.h>
#include "bbbLib.h"

int PWMhandle;

struct coordinates {
    int x;
    int y;
};

int initPWMcontr();
int liftLid();
int dieCount();
struct coordinates findCache();
void haltDieMotor();
