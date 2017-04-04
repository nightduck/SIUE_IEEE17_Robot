/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "navigation.h"
#include "robotLib.h" //For movement
#include "amg.h"
#include "testGyro.h"
#include "mem.h"
#include "die.h"
#include "libIO.h"
#include "ir.h"
#include "fix.h"

extern shared_memory_t *shared_memory;
extern int debug;
extern GUIvars_t GUIvars;

//Accumulated drift (rdrift is rotation). Fixed point, where 16LSB = 1degree(or in)
short xAccumdrift = 0;
short yAccumdrift = 0;
//short rAccumdrift = 0;

//Location on the board
int x = 0;
int y = 0;
int r = 0; //0-fwd, 1-right, 2-bwd, 3-left

//Updates distance and drift after movement
//dist - number of square moves forward (or backward, if negative)
//pdrift - the undesired additional movement forward
//rdrift - the deviation from forward

void debugInfo() {
    printf("    xAccumdrift %hd\n    yAccumdrift %hd\n", xAccumdrift, yAccumdrift);
}

void updateLocation(int dist, short pdrift, short rdrift) {
    //if (debug) printf("updateLocation\n  dist %d\n  pdrift %hd\n  rdrift %hd\n", dist, pdrift, rdrift);

    //rAccumdrift += rdrift;
    switch (r) {
        case FORWARD: //If facing forward
            //if (debug) printf("  forward\n");
            y += dist;
            //            //This is now obsolete, since the robot does realtime drift correction
            //            if (!DRIVEBLIND) {
            //                yAccumdrift += pdrift;
            //                //xAccumdrift += tan(rdrift/FXDPT) * dist * 12;    //If rdrift is given in radians
            //                xAccumdrift += tan(rdrift / FXDPT * M_PI / 180) * dist * 12; //If rdrift is given in degrees
            //            }
            break;
        case RIGHT: //If facing right
            //if (debug) printf("  right\n");
            x += dist;
            //            if (!DRIVEBLIND) {
            //                xAccumdrift += pdrift;
            //                //yAccumdrift += tan(rdrift/FXDPT) * dist * 12;    //If rdrift is given in radians
            //                xAccumdrift += tan(rdrift / FXDPT * M_PI / 180) * dist * 12; //If rdrift is given in degrees
            //            }
            break;
        case BACKWARD: //If facing backwards
            //if (debug) printf("  backward\n");
            y -= dist;
            //            if (!DRIVEBLIND) {
            //                yAccumdrift -= pdrift;
            //                //xAccumdrift -= tan(rdrift/FXDPT) * dist * 12;    //If rdrift is given in radians
            //                xAccumdrift += tan(rdrift / FXDPT * M_PI / 180) * dist * 12; //If rdrift is given in degrees
            //            }
            break;
        case LEFT: //If facing left
            //if (debug) printf("  left\n");
            x -= dist;
            //            if (!DRIVEBLIND) {
            //                xAccumdrift -= pdrift;
            //                //yAccumdrift -= tan(rdrift/FXDPT) * dist * 12;    //If rdrift is given in radians
            //                xAccumdrift += tan(rdrift / FXDPT * M_PI / 180) * dist * 12; //If rdrift is given in degrees
            //            }
            break;
        default:
            //ERROR
            printf("Error-updateLocation: Rotation is invalid value\n");
    }
}

//Moves forward 1 square

int forward(int prev) {
    if (debug) {
        printf("Forward 1\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    float offset = 0;

    if (prev == MOVE_RSWLEFT || prev == MOVE_RSWRIGHT) offset -= GUIvars.turnRad * 2;

    //short pstart = getXDrift(); //From the accelerometer API

    //    //DEBUGGING
    //    if (debug) {
    //        printf("Initial direction %hd\n", getOrientation);
    //    }   

    switch (r) {
        case FORWARD: //If facing foward
            fwd(12 + offset - yAccumdrift, SPEED); //Move forward 9 inches in 2 seconds
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            fwd(12 + offset - xAccumdrift, SPEED); //Move right 9 inches in 2 seconds
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            fwd(12 + offset + yAccumdrift, SPEED); //Move backward 9 inches in 2 seconds
            yAccumdrift = 0;
            break;
        case LEFT:
            fwd(12 + offset + xAccumdrift, SPEED); //Move left 9 inches in 2 seconds
            xAccumdrift = 0;
    }

    //short pdrift = getXDrift() - pstart;
    short rdrift = (DRIVEBLIND) ? 0 : getOrientation() - defaultHeading - (r * FULL_ROT / 4);

    if (debug) printf("    rdrift %hd\n", rdrift);

    //Calculate drift.
    updateLocation(1, 0, rdrift);
    return 0x11;
}

//Moves forward 2 square

int forward2(int prev) {
    if (debug) {
        printf("Forward 2\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    float offset = 0;

    if (prev == MOVE_RSWLEFT || prev == MOVE_RSWRIGHT) offset -= GUIvars.turnRad * 2;

    //TODO: change rdrift to match forward()'s version
    short rdrift = getOrientation();
    //fwd(24+FWD_OFFSET,SPEED); //Move forward 9 inches in 2 seconds

    switch (r) {
        case FORWARD: //If facing foward
            fwd(24 + offset - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            fwd(24 + offset - xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            fwd(24 + offset + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            fwd(24 + offset + xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    //Calculate drift
    updateLocation(2, 0, rdrift);
    return 0x12;
}

//Moves forward 3 square

int forward3(int prev) {
    if (debug) {
        printf("Forward 3\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    float offset = 0;

    if (prev == MOVE_RSWLEFT || prev == MOVE_RSWRIGHT) offset -= GUIvars.turnRad * 2;

    short rdrift = getOrientation();
    //fwd(36+FWD_OFFSET,SPEED); //Move forward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            fwd(36 + offset - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            fwd(36 + offset - xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            fwd(36 + offset + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            fwd(36 + offset + xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    //Calculate drift.
    updateLocation(3, 0, 0);
    return 0x13;
}

//Moves forward 4 square

int forward4(int prev) {
    if (debug) {
        printf("Forward 4\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    float offset = 0;

    if (prev == MOVE_RSWLEFT || prev == MOVE_RSWRIGHT) offset -= GUIvars.turnRad * 2;

    short rdrift = getOrientation();
    //fwd(48+FWD_OFFSET,SPEED); //Move forward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            fwd(48 + offset - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            fwd(48 + offset - xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            fwd(48 + offset + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            fwd(48 + offset + xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(4, 0, 0);
    //Calculate drift.
    return 0x14;
}

//Moves forward 5 square

int forward5(int prev) {
    if (debug) {
        printf("Forward 5\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    float offset = 0;

    if (prev == MOVE_RSWLEFT || prev == MOVE_RSWRIGHT) offset -= GUIvars.turnRad * 2;

    short rdrift = getOrientation();
    //fwd(60+FWD_OFFSET,SPEED); //Move forward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            fwd(60 + offset - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            fwd(60 + offset - xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            fwd(60 + offset + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            //TODO: Get rid of conditional statments
            fwd(60 + offset + xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(5, 0, 0);
    //Calculate drift.
    return 0x15;
}

//Moves forward 6 square

int forward6(int prev) {
    if (debug) {
        printf("Forward 6\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();
    printf("Rotation corrected\n");
    float offset = 0;

    if (prev == MOVE_RSWLEFT || prev == MOVE_RSWRIGHT) offset -= GUIvars.turnRad * 2;

    short rdrift = getOrientation();
    switch (r) {
        case FORWARD: //If facing foward
            fwd(72 + offset - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            fwd(72 + offset - xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            fwd(72 + offset + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            fwd(72 + offset + xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(6, 0, 0);
    //Calculate drift.
    return 0x16;
}

//Moves backward 1 square

int backward(int prev) {
    if (debug) {
        printf("Backward 1\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    short rdrift = getOrientation();
    //bwd(12+BWD_OFFSET,SPEED); //Move backward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            bwd(12 + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            bwd(12 + xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            bwd(12 - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            bwd(12 - xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(-1, 0, 0);
    //Calculate drift
    return 0x21;
}

//Moves backward 2 square

int backward2(int prev) {
    if (debug) {
        printf("Backward 2\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    short rdrift = getOrientation();
    bwd(24 + BWD_OFFSET, SPEED); //Move backward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            bwd(24 + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            bwd(24 + xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            bwd(24 - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            bwd(24 - xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(-2, 0, 0);
    //Calculate drift
    return 0x22;
}

//Moves backward 3 square

int backward3(int prev) {
    if (debug) {
        printf("Backward 3\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    short rdrift = getOrientation();
    //bwd(36+BWD_OFFSET,SPEED); //Move backward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            bwd(36 + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            bwd(36 + xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            bwd(36 - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            bwd(36 - xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(-3, 0, 0);
    //Calculate drift
    return 0x23;
}

//Moves backward 4 square

int backward4(int prev) {
    if (debug) {
        printf("Backward 4\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    short rdrift = getOrientation();
    //bwd(48+BWD_OFFSET,SPEED); //Move backward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            bwd(48 + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            bwd(48 + xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            bwd(48 - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            bwd(48 - xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(-4, 0, 0);
    //Calculate drift
    return 0x24;
}

//Moves backward 5 square

int backward5(int prev) {
    if (debug) {
        printf("Backward 5\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    short rdrift = getOrientation();
    //bwd(60+BWD_OFFSET,SPEED); //Move backward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            bwd(60 + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            bwd(60 + xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            bwd(60 - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            bwd(60 - xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(-5, 0, 0);
    //Calculate drift
    return 0x25;
}

//Moves backward 6 square

int backward6(int prev) {
    if (debug) {
        printf("Backward 6\n");
        debugInfo();
    }

    if (!DRIVEBLIND) correctRotation();

    short rdrift = getOrientation();
    bwd(72 + BWD_OFFSET, SPEED); //Move backward 9 inches in 2 seconds
    switch (r) {
        case FORWARD: //If facing foward
            bwd(72 + yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case RIGHT: //If facing right
            bwd(72 + xAccumdrift, SPEED);
            xAccumdrift = 0;
            break;
        case BACKWARD: //If facing backward
            bwd(72 - yAccumdrift, SPEED);
            yAccumdrift = 0;
            break;
        case LEFT: //If facing backward
            bwd(72 - xAccumdrift, SPEED);
            xAccumdrift = 0;
    }

    rdrift = (DRIVEBLIND) ? 0 : getOrientation() - rdrift;

    updateLocation(-6, 0, 0);
    //Calculate drift
    return 0x26;
}

//Rotates left 90 degrees, turning wheels opposite directions

int rotLeft(int prev) {
    if (debug) {
        printf("Rotate left\n");
        debugInfo();
    }

    short rdrift = (DRIVEBLIND) ? 0 : (getOrientation() - defaultHeading) - (r * FULL_ROT / 4);

    if (debug) {
        printf("    rdrift %hd\n", rdrift);
        printf("    defaultHeading %hd\n", defaultHeading);
        printf("    angle to rotate %f\n", ((FULL_ROT + rdrift + (FULL_ROT / 4)) % FULL_ROT) / FXDPT);
    }

    r = (r + 3) % 4; //Set new value for r
    /*
            direction currently facing: getOrientation() - defaultHeading
            direction desired to face:  r * FULL_ROT/4			can be 0, 1440, 2880, 4320

            current		0	1440	2880	4320
            0		0	4320	2880	1400
            1000		100	5320	3880	2400
            1440		1400	0	4320	2880
            360 + current - wanted
     */


    rotate(((FULL_ROT + rdrift + (FULL_ROT / 4)) % FULL_ROT) / FXDPT - LEFT_TURN_OFFSET, SPEED, CCW); //Rotate, adjusting for offset
    //Also offset specifically for left turn
    //Math: 1) 90 degrees plus drift (plus 360 for modding purposes)
    //      2) Mod it so it's between 0 and 360
    //      3) Convert fixed point to floating point by dividing by FXDPT (16.0)
    //rotate(90, SPEED, CCW);

    //POTENTIAL BUG: value returned from getOrientation doesn't overflow neatly


    return MOVE_RLEFT;
}

//Rotates right 90 degrees, turning wheels opposite directions

int rotRight(int prev) {
    if (debug) {
        printf("Rotate right\n");
        debugInfo();
    }

    short rdrift = (DRIVEBLIND) ? 0 : (r * FULL_ROT / 4) - (getOrientation() - defaultHeading);

    if (debug) {
        printf("    rdrift %hd\n", rdrift);
        printf("    defaultHeading %hd\n", defaultHeading);
        printf("    angle to rotate %f\n", ((FULL_ROT + rdrift + (FULL_ROT / 4)) % FULL_ROT) / FXDPT);
    }

    r = (r + 1) % 4; //Set new value for r

    rotate(((FULL_ROT + rdrift + (FULL_ROT / 4)) % FULL_ROT) / FXDPT, SPEED, CW); //Rotate, adjusting for offset
    //rotate(90, SPEED, CW);

    return MOVE_RRIGHT;
}

//Rotates left 90 degrees, using just the right wheel

int rotLeftSingleWheel(int prev) {
    if (debug) {
        printf("Rotate left single wheel\n");
        debugInfo();
    }

    short rdrift = (DRIVEBLIND) ? 0 : (getOrientation() - defaultHeading) - (r * FULL_ROT / 4);

    if (debug) {
        printf("    rdrift %hd\n", rdrift);
        printf("    defaultHeading %hd\n", defaultHeading);
        printf("    angle to rotate %f\n", ((FULL_ROT + rdrift + (FULL_ROT / 4)) % FULL_ROT) / FXDPT);
    }

    r = (r + 3) % 4; //Set new value for r
    /*
            direction currently facing: getOrientation() - defaultHeading
            direction desired to face:  r * FULL_ROT/4			can be 0, 1440, 2880, 4320

            current		0	1440	2880	4320
            0		0	4320	2880	1400
            1000		100	5320	3880	2400
            1440		1400	0	4320	2880
            360 + current - wanted
     */


    rotate(SINGLE_WHEEL_MULTIPLIER * ((FULL_ROT + rdrift + (FULL_ROT / 4)) % FULL_ROT) / FXDPT, SPEED, SCCW); //Rotate, adjusting for offset
    //Math: 1) 90 degrees plus drift (plus 360 for modding purposes)
    //      2) Mod it so it's between 0 and 360
    //      3) Convert fixed point to floating point by dividing by FXDPT (16.0)
    //rotate(90, SPEED, CCW);

    //POTENTIAL BUG: value returned from getOrientation doesn't overflow neatly

    return MOVE_RSWLEFT;
}

//Rotates right 90 degrees, using just the left wheel

int rotRightSingleWheel(int prev) {
    if (debug) {
        printf("Rotate right single wheel\n");
        debugInfo();
    }

    short rdrift = (DRIVEBLIND) ? 0 : (r * FULL_ROT / 4) - (getOrientation() - defaultHeading);

    if (debug) {
        printf("    rdrift %hd\n", rdrift);
        printf("    defaultHeading %hd\n", defaultHeading);
        printf("    angle to rotate %f\n", ((FULL_ROT + rdrift + (FULL_ROT / 4)) % FULL_ROT) / FXDPT);
    }

    r = (r + 1) % 4; //Set new value for r

    rotate(SINGLE_WHEEL_MULTIPLIER * ((FULL_ROT + rdrift + (FULL_ROT / 4)) % FULL_ROT) / FXDPT, SPEED, SCW); //Rotate, adjusting for offset
    //rotate(90, 9, CW);

    //Calculate drift
    return MOVE_RSWRIGHT;
}

//Rotates left and moves forward in an arch by rotating both wheels

int leftFwd(int prev) {
    return MOVE_RFLEFT;
}

//Rotates right and moves forward in an arch by rotating both wheels

int rightFwd(int prev) {
    return MOVE_RFRIGHT;
}

//Rotates 180 degrees

int uturn(int prev) { //TODO: Verify this is working
    if (debug) {
        printf("U-Turn\n");
        debugInfo();
    }

    short rdrift = (DRIVEBLIND) ? 0 : (getOrientation() - defaultHeading) - (r * FULL_ROT / 4);

    if (debug) {
        printf("    r %d\n", r);
        printf("    rdrift %hd\n", rdrift);
        printf("    defaultHeading %hd\n", defaultHeading);
    }

    r = (r + 2) % 4;
    rotate(((FULL_ROT + rdrift + (FULL_ROT / 2)) % FULL_ROT) / FXDPT, 9, CCW); //CCW, bc robot is better at left turns
    //rotate(180, SPEED, CCW);

    //Calculate drift
    return MOVE_UTURN;
}

//Brake

int brake(int prev) {
    applyBrake();
    return -1;
}

int endPath(int prev) {
    return 0;
}

void correctRotation() {
    float drift = (getOrientation() - ((defaultHeading + (r * FULL_ROT / 4)) % FULL_ROT)) / 16.0;
    if (debug) {
        printf("Correct rotation\n");
        printf("defaultHeading: %d\n", defaultHeading);
        printf("r: %d\n", r);
        printf("drift %f\n", drift);
    }
    if (drift < 0) rotate(-1 * drift, SPEED, CW);
    else if (drift > 0) rotate(drift, SPEED, CCW);

    //If we're not blocking (and we actually moved), then block here
    if (!BLOCK && drift != 0) {
        waitForComplete();
        shared_memory->command.code = NOP;
        shared_memory->command.status = IDLE;
        shared_memory->state = 0;
    }

    //TODO: If you want to implement rotation thresholds, here is the place to do it
}

int valid(char data) {
    //Return false if the square is undefined or an obstacle
    return (!(data & OBSTACLE) && !((data & UNDEFINED) && !(data & MUST_EXPLORE)));
}

char * getDirection(char dir) {
    switch (dir) {
        case 0:
            return "FORWARD";
            break;
        case 1:
            return "RIGHT";
            break;
        case 2:
            return "BACKWARD";
            break;
        case 3:
            return "LEFT";
            break;
        default:
            return "err";
    }
}

void dieReadingSequence() {
    //Go forward 4 inches
    fwd(4, SPEED);
    if (!BLOCK) {
        waitForComplete();
        shared_memory->command.code = NOP;
        shared_memory->command.status = IDLE;
        shared_memory->state = 0;
    }
    correctRotation();

    //Inch forward until cache is in view
    float pixPerInch = 84.4;
    int i;
    struct coordinates coor;
    float dist = 4;
    for (i = 0; i < 6; i++) {
        coor = findCache();
	printf("    Coor (%d, %d)\n",coor.x,coor.y);
        if (coor.y != 0) break;
        else {
            fwd(1, SPEED);
            if (!BLOCK) {
                waitForComplete();
                shared_memory->command.code = NOP;
                shared_memory->command.status = IDLE;
                shared_memory->state = 0;
            }
            correctRotation();
            dist += 1;
        }
    }
    coor = findCache();
    printf("    Coor (%d, %d)\n\nFOUND CACHE, ENTERING WHILE LOOP\n",coor.x,coor.y);
    
    //If the lid was found, pick it up
    if(coor.x != 0) {
        fwd(coor.y / pixPerInch + 0.5, SPEED);
        if (!BLOCK) {
           waitForComplete();
           shared_memory->command.code = NOP;
           shared_memory->command.status = IDLE;
           shared_memory->state = 0;
        }
        correctRotation();
        dist += coor.y / pixPerInch + 0.5;

//        //Move arm over cache
//        fwd(2, SPEED);
//        if (!BLOCK) {
//            waitForComplete();
//            shared_memory->command.code = NOP;
//            shared_memory->command.status = IDLE;
//            shared_memory->state = 0;
//        }

        //Pick up lid
        delay_ms(250);
        liftLid();
        delay_ms(400);

        //Back up so camera can see
        bwd(2.5, SPEED);
        if (!BLOCK) {
            waitForComplete();
            shared_memory->command.code = NOP;
            shared_memory->command.status = IDLE;
            shared_memory->state = 0;
        }
        dist -= 2.5;
    
        //Look at die
        int count = dieCount();

        if (count == 0) {
            //Set count to random number
            srand(time(NULL));
            count = (rand() % 6) + 1;
        }


        DPD(count + 48);

        //Straighten out, and back up to previous square
        correctRotation();

        bwd(dist, SPEED);
        if (!BLOCK) {
            waitForComplete();
            shared_memory->command.code = NOP;
            shared_memory->command.status = IDLE;
            shared_memory->state = 0;
        }
        correctRotation();
    } else {
        //If cache not picked up, return to original square
        bwd(dist, SPEED);
        if (!BLOCK) {
            waitForComplete();
            shared_memory->command.code = NOP;
            shared_memory->command.status = IDLE;
            shared_memory->state = 0;
        }
        correctRotation();
    }
}

//Returns true if there is a main tunnel connecting (i,j) and (k,l)
//Returns false if depth equals 0. pi and pj are the coordinates of
//i and j in the calling fn.

int completeTunnel(char board[7][7], int i, int j, int pi, int pj, int ppi, int ppj, 
        int pppi, int pppj, int epi, int epj, int turns, int depth) {
    
    if (turns > 3 || !(board[i][j] & MAIN_TUNNEL) || i > 6 || i < 0 || j > 6 || j < 0) return 0;
        //If i or j are both on the edge (0 or 6), and not because it's that starting square
    else if ((((i % 6) == 0) || ((j % 6) == 0)) && (i != epi || j != epj)) {
        //If the start and end tunnels are on the same edge as the original ep, return false
        if (((epi % 6 == 0) && (i == epi)) || ((epj % 6 == 0) && (j == epj))) return 0;
        else if (depth < 7) return 0; //If tunnel shorter than 7, invalid. Return false
        else return 100 - ((board[i][j] & CANDIDATE) ? 1 : 0);
    }
    //Head up (down in printout) if you are not moving into previous square, and it isn't popping a tight u-turn
    else {
        int count;
        int countFinal = 0;
        if (i + 1 != pi && ((i+1) != ppi)) {
            count = completeTunnel(board, i + 1, j, i, j, pi, pj, ppi, ppj, epi, epj, (j - pj != 0) ? turns+1 : turns, depth+1);
            if (count >= countFinal) countFinal = count - ((board[i][j] & CANDIDATE) ? 1 : 0);
        }
        //Head down (up in printout)
        if (i - 1 != pi && ((i-1) != ppi)) {
            count = completeTunnel(board, i - 1, j, i, j, pi, pj, ppi, ppj, epi, epj, (j - pj != 0) ? turns+1 : turns, depth+1);
            if (count >= countFinal) countFinal = count - ((board[i][j] & CANDIDATE) ? 1 : 0);
        }
        //Head right
        if (j + 1 != pj && ((j+1) != ppj)) {
            count = completeTunnel(board, i, j + 1, i, j, pi, pj, ppi, ppj, epi, epj, (i - pi != 0) ? turns+1 : turns, depth+1);
            if (count >= countFinal) countFinal = count - ((board[i][j] & CANDIDATE) ? 1 : 0);
        }
        //Head left
        if (j - 1 != pj && ((j-1) != ppj)) {
            count = completeTunnel(board, i, j - 1, i, j, pi, pj, ppi, ppj, epi, epj, (i - pi != 0) ? turns+1 : turns, depth+1);
            if (count >= countFinal) countFinal = count - ((board[i][j] & CANDIDATE) ? 1 : 0);
        }
        return countFinal;
    }
}

int numAdj(char board[7][7], char row, char col) {
    int count = 0;
    if(row>0 && col>0 && board[row-1][col-1] & MAIN_TUNNEL) count++;
    if(col>0 && board[row][col-1] & MAIN_TUNNEL) count++;
    if(row<6 && col>0 && board[row+1][col-1] & MAIN_TUNNEL) count++;
    if(row>0 && board[row-1][col] & MAIN_TUNNEL) count++;
    if(row<6 && board[row+1][col] & MAIN_TUNNEL) count++;
    if(row>0 && col<6 && board[row-1][col+1] & MAIN_TUNNEL) count++;
    if(col<6 && board[row][col+1] & MAIN_TUNNEL) count++;
    if(row<6 && col<6 && board[row+1][col+1] & MAIN_TUNNEL) count++;
    
    return count;
}

int contains2by2s(char board[7][7]) {
    int i, j;
    for(i = 0; i < 7; i++) {
        for(j = 0; j < 7; j++) {
            if (numAdj(board, i, j) > 2) return 1;
        }
    }
    return 0;
}

int hasTunnel(char board[7][7]) {
    int i, j, temp, max = 0;
/*
            pi = (i == 0) ? -1 : ((i==6) ? 7 : i);
            pj = (j == 0) ? -1 : ((j==6) ? 7 : j);
*/
    //if (contains2by2s(board)) return 0;        
            
    for (i = 1; i < 6; i++) {
        if ((board[i][0] & MAIN_TUNNEL) && (board[i][1] & MAIN_TUNNEL)) {
            temp = completeTunnel(board, i, 0, i, -1, i, -2, i, -3, i, 0, 0, 0);
            if (temp > max) max = temp;
        }
        if ((board[i][6] & MAIN_TUNNEL) && (board[i][5] & MAIN_TUNNEL)) {
            temp = completeTunnel(board, i, 6, i, 7, i, 8, i, 9, i, 6, 0, 0);
            if (temp > max) max = temp;
        }
    }
    for (j = 1; j < 6; j++) {
        if ((board[0][j] & MAIN_TUNNEL) && (board[1][j] & MAIN_TUNNEL)) {
            temp = completeTunnel(board, 0, j, -1, j, -2, j, -3, j, 0, j, 0, 0);
            if (temp > max) max = temp;
        }
        if ((board[6][j] & MAIN_TUNNEL) && (board[5][j] & MAIN_TUNNEL)) {
            temp = completeTunnel(board, 6, j, 7, j, 8, j, 9, j, 6, j, 0, 0);
            if (temp > max) max = temp;
                    
        }
    }
    return max;
}

int tunnelGapFill(char board[7][7]) {
    char cand[49][2]; //List of coordinate tuples of possible locations for tunnels
    int i, j;
    int count = 0;

    if (debug)printf("Tunnel Gap fill\n");

    //DEBUGGING
    if (debug) {
        printf("Board-------------------------------\n");
        for (i = 0; i < 7; i++) {
            for (j = 0; j < 7; j++) {
                printf("%c ", board[i][j] + 48);
            }
            printf("\n");
        }
        printf("-----------------------------------\n");
    }

    //Scan board for undiscovered tunnels
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            int c = 0;
            if(!(board[i][j] & MAIN_TUNNEL)) {
                if(i > 0 && board[i-1][j] & MAIN_TUNNEL) c++;
                if(i < 6 && board[i+1][j] & MAIN_TUNNEL) c++;
                if(i > 0 && board[i][j-1] & MAIN_TUNNEL) c++;
                if(j < 6 && board[i][j+1] & MAIN_TUNNEL) c++;
                if(c > 1) {
                    cand[count][0] = i;
                    cand[count][1] = j;
                    board[i][j] |= CANDIDATE;
                    count++;
                } else if (i==0 && j!=0 && board[i+1][j] & MAIN_TUNNEL) {
                    cand[count][0] = i;
                    cand[count][1] = j;
                    board[i][j] |= CANDIDATE;
                    count++;                    
                } else if (i==6 && j!=6 && board[i-1][j] & MAIN_TUNNEL) {
                    cand[count][0] = i;
                    cand[count][1] = j;
                    board[i][j] |= CANDIDATE;
                    count++;                    
                } else if (i!=0 && j==0 && board[i][j+1] & MAIN_TUNNEL) {
                    cand[count][0] = i;
                    cand[count][1] = j;
                    board[i][j] |= CANDIDATE;
                    count++;                    
                } else if (i!=0 && j==6 && board[i][j-1] & MAIN_TUNNEL) {
                    cand[count][0] = i;
                    cand[count][1] = j;
                    board[i][j] |= CANDIDATE;
                    count++;                    
                }
            }
            
            //TODO: Scan for isolated groups of bonus tunnels
        }
    }

    
    //Do a binary count and turn each potential tunnel on or off according to
    //their respective bit and each time check if a full tunnel is complete
    int best, temp, max = 0;
    for(i = 0; i < (1 << count); i++) {
        //For each candidate, turn set to MAIN_TUNNEL (or not) depending on bit state
        for(j = 0; j < count; j++) {
            //printf("    (%d, %d), j=%d, i=0x%x ", (int)cand[j][0], (int)cand[j][1], j, i);
            //If i is 0110 and j is 2. Then 0110 & 0010 = TRUE, so turn on tunnel
            if(i & ((1<<j))) {
                //printf("    on\n");
                board[cand[j][0]][cand[j][1]] |= MAIN_TUNNEL;
            }
            else {
                board[cand[j][0]][cand[j][1]] &= ~MAIN_TUNNEL;
                //printf("    off\n");
            }
        }
        //printf("\n");

        //Check for tunnel
        temp = hasTunnel(board);
        if (temp > max) {
            max = temp;
            best = i;
        }
    }
    
    //If a complete tunnel wasn't found, revert to 
    if(max == 0) {
        for (i = 0; i < count; i++) {
            board[cand[i][0]][cand[i][1]] &= ~(MAIN_TUNNEL | CANDIDATE);
        }
        printf("    Failure\n");
        return 0; //Unsuccessful
    }
    //If a tunnel was found, restore that state and prune off unneccesary values
    else {
        //Restore ideal state
        for(j = 0; j < count; j++) {
            //printf("    (%d, %d), j=%d, i=0x%x ", (int)cand[j][0], (int)cand[j][1], j, i);
            //If i is 0110 and j is 2. Then 0110 & 0010 = TRUE, so turn on tunnel
            if(best & ((1<<j))) {
                //printf("    on\n");
                board[cand[j][0]][cand[j][1]] |= MAIN_TUNNEL;
            }
            else {
                board[cand[j][0]][cand[j][1]] &= ~MAIN_TUNNEL;
                //printf("    off\n");
            }
        }
        
        //Prune off unneccesary candidates
        for (i = 0; i < count; i++) {
            board[cand[i][0]][cand[i][1]] &= ~MAIN_TUNNEL; //Turn the tile off

            if (!hasTunnel(board)) {
                printf("    Tried removing %d, %d. Tunnel broke\n", (int)cand[i][0], (int)cand[i][1]); 
                board[cand[i][0]][cand[i][1]] |= MAIN_TUNNEL; //If it breaks the tunnel, turn it back on
            }
        }

        //Prune off false positives
        for (i = 0; i < 7; i++) {
            for (j = 0; j < 7; j++) {
                board[i][j] &= ~MAIN_TUNNEL; //Turn tile off

                if (!hasTunnel(board)) {
                    board[i][j] |= MAIN_TUNNEL; //If it breaks the tunnel, turn it back on
                }
            }
        }
        
        for (i = 0; i < count; i++) {
            board[cand[i][0]][cand[i][1]] &= ~CANDIDATE;
        }
        
        printf("    Success\n");
        return 1; //Successful
    }
}

void travelPath(char board[7][7], int votes[7][7][3], fp* path, char row, char col, char orient) {
    //TODO: Add code here to modify path based on board
    if (debug) printf("Traveling\n");
    int i, v;
    i = v = 0;
    do {
        printf("Next instruction: ");
        translatefp(path[i]);
        v = path[i](v); //Perform movement
        if (v) observe(board, votes, y, x, r, v); //Take observations (if non blocked, this will execute while in motion)
        //Here, y,x,&r are the coordinates after movement

        i++;
        row = y; //Update coordinates
        col = x;
        orient = r;
    } while (v);
}

fp * shortestPath(char board[7][7], fp *fnList, char startRow, char startCol, char startOrient, char endRow, char endCol, char endOrient) {
    struct square cube[7][7][4]; //Each plane of the cube corresponds to a different orientation
    //but is otherwise identical to board
    if (debug) printf("Shortest path (%d, %d, %d) -> (%d, %d, %d)\n", startRow, startCol, startOrient, endRow, endCol, endOrient);

    //Return NULL if the square is impossible to reach
    if (board[endRow][endCol] & OBSTACLE) return NULL;

    //Return NULL is path length is zero
    if ((startRow == endRow) && (startCol == endCol) && (startOrient == endOrient)) return NULL;

    //TODO: Return NULL if the square is impossible to reach through more complicated means

    //Initialize node values
    int i = 0;
    int j = 0;
    int k = 0;
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            for (k = 0; k < 4; k++) {
                cube[i][j][k].data = board[i][j];
                cube[i][j][k].dist = 255;
                cube[i][j][k].row = i;
                cube[i][j][k].col = j;
                cube[i][j][k].orient = k;
                cube[i][j][k].parent = NULL;
                cube[i][j][k].next = NULL;
            }
        }
    }

    //Exists so the square will be visited even if it's UNDEFINED
    for (i = 0; i < 4; i++)
        cube[endRow][endCol][i].data ^= MUST_EXPLORE;

    //Initialize root node
    cube[startRow][startCol][startOrient].dist = 0;

    struct square **queue = malloc(196 * sizeof (struct square*)); //Create queue big enough to traverse each square
    struct square thing;
    queue[0] = &thing;
    queue[0] = &cube[startRow][startCol][startOrient]; //Add root node to the queue
    int queueIndex = 0; //Next index to read
    int queueEnd = 1; //First empty index

    //BFS
    struct square *current;
    while (queueIndex < queueEnd) {
        current = queue[queueIndex];
        queueIndex++;

        //DEBUGGING
        //printf("current = (%d,%d,%s)\n", (int) current->row, (int) current->col,
        //    getDirection(current->orient));

        //Find lateral movements
        struct square *node;
        switch (current->orient) {
            case FORWARD: //Facing forward
                //If not on the last row, add option to move forward
                node = &cube[current->row + 1][current->col][current->orient];
                if (current->row < 6 && valid(node->data) && current->dist + 1 < node->dist) {
                    //DEBUGGING
                    //printf("       (%d, %d, %s) added\n", (int) node->row, (int) node->col, getDirection(node->orient));
                    node->dist = current->dist + 1;
                    node->parent = current;
                    queue[queueEnd] = node;
                    queueEnd++;
                }
                break;
            case LEFT: //Facing left
                //If not on the first column, add option to move absolute left
                node = &cube[current->row][current->col - 1][current->orient];
                if (current->col > 0 && valid(node->data) && current->dist + 1 < node->dist) {
                    //DEBUGGING
                    //printf("       (%d, %d, %s) added\n", (int) node->row, (int) node->col, getDirection(node->orient));
                    node->dist = current->dist + 1;
                    node->parent = current;
                    queue[queueEnd] = node;
                    queueEnd++;
                }
                break;
            case BACKWARD: //Facing backward
                //If not on the first row, add option to move backward
                node = &cube[current->row - 1][current->col][current->orient];
                if (current->row > 0 && valid(node->data) && current->dist + 1 < node->dist) {
                    //DEBUGGING
                    //printf("       (%d, %d, %s) added\n", (int) node->row, (int) node->col, getDirection(node->orient));
                    node->dist = current->dist + 1;
                    node->parent = current;
                    queue[queueEnd] = node;
                    queueEnd++;
                }
                break;
            case RIGHT: //Facing right
                //If not on the first column, add option to move right
                node = &cube[current->row][current->col + 1][current->orient];
                if (current->col < 6 && valid(node->data) && current->dist + 1 < node->dist) {
                    //DEBUGGING
                    //printf("       (%d, %d, %s) added\n", (int) node->row, (int) node->col, getDirection(node->orient));
                    node->dist = current->dist + 1;
                    node->parent = current;
                    queue[queueEnd] = node;
                    queueEnd++;
                }
                break;
            default:
                printf("Error, invalid orientation!");
        }

        //Find possible rotations
        for (i = ((current->orient + 1) & 0x3); i != current->orient; i = ((i + 1) & 0x03)) {
            struct square *node = &cube[current->row][current->col][i];
            if (current->dist + 1 < node->dist) {
                //DEBUGGING
                //printf("       (%d, %d, %s) added\n", (int) node->row, (int) node->col, getDirection(node->orient));
                node->dist = current->dist + 1;
                node->parent = current;
                queue[queueEnd] = node;
                queueEnd++;
            }
        }
    }

    //Trace out shortest path
    current = &cube[endRow][endCol][endOrient];
    while (current->parent != NULL) {
        if (debug) {
            printf("(%d,%d,%s)<-(%d,%d,%s)\n", (int) current->row, (int) current->col, getDirection(current->orient),
                    (int) current->parent->row, (int) current->parent->col, getDirection(current->parent->orient));
        }
        current->parent->next = current;
        current = current->parent;
    }

    //Compile functions for path
    int index = 0; //Next index to fill in fnList
    int total = 0; //Total distance traveled in straight line. Increments to
    //accumulate a forward direction, decrements to track a backwards one
    while (current->next != NULL) {
        if (current->orient != current->next->orient) {
            //Resolve total, which has been accumulating up until now
            if (total != 0) {
                //Add backwards or forwards movement to fnList
                switch (total) {
                    case 6:
                        fnList[index] = forward6;
                        break;
                    case 5:
                        fnList[index] = forward5;
                        break;
                    case 4:
                        fnList[index] = forward4;
                        break;
                    case 3:
                        fnList[index] = forward3;
                        break;
                    case 2:
                        fnList[index] = forward2;
                        break;
                    case 1:
                        fnList[index] = forward;
                        break;
                    case -1:
                        fnList[index] = backward;
                        break;
                    case -2:
                        fnList[index] = backward2;
                        break;
                    case -3:
                        fnList[index] = backward3;
                        break;
                    case -4:
                        fnList[index] = backward4;
                        break;
                    case -5:
                        fnList[index] = backward5;
                        break;
                    case -6:
                        fnList[index] = backward6;
                        break;
                }
                index++;
                total = 0;
            }
            //Handle rotation
            unsigned char rotation = current->next->orient - current->orient;
            switch (rotation & 0x3) {
                case LEFT:
                    fnList[index] = rotLeft;
                    break;
                case BACKWARD:
                    fnList[index] = uturn;
                    break;
                case RIGHT:
                    fnList[index] = rotRight;
                    break;
                default:
                    printf("invalid rotation: (%hhx,%hhx,%s)--%hhx->(%hhx,%hhx,%s)\n",
                            current->row, current->col, getDirection(current->orient),
                            rotation, current->next->row,
                            current->next->col, getDirection(current->next->orient));
            }
            index++;
        } else if ((current->orient == FORWARD && current->row < current->next->row)
                || (current->orient == LEFT && current->col > current->next->col)
                || (current->orient == BACKWARD && current->row > current->next->row)
                || (current->orient == RIGHT && current->col < current->next->col)) {
            total++; //Move forward
        } else if ((current->orient == FORWARD && current->row > current->next->row)
                || (current->orient == LEFT && current->col < current->next->col)
                || (current->orient == BACKWARD && current->row < current->next->row)
                || (current->orient == RIGHT && current->col > current->next->col)) {
            total--; //Move backward
        } else {
            printf("6958: wut\n");
        }

        //DEBUGGING
        //        printf("(%d,%d,%s)->\n", (int) current->row, (int) current->col, getDirection(current->orient));
        current = current->next;
    }
    if (total != 0) {
        //Add backwards or forwards movement to fnList
        switch (total) {
            case 6:
                fnList[index] = forward6;
                break;
            case 5:
                fnList[index] = forward5;
                break;
            case 4:
                fnList[index] = forward4;
                break;
            case 3:
                fnList[index] = forward3;
                break;
            case 2:
                fnList[index] = forward2;
                break;
            case 1:
                fnList[index] = forward;
                break;
            case -1:
                fnList[index] = backward;
                break;
            case -2:
                fnList[index] = backward2;
                break;
            case -3:
                fnList[index] = backward3;
                break;
            case -4:
                fnList[index] = backward4;
                break;
            case -5:
                fnList[index] = backward5;
                break;
            case -6:
                fnList[index] = backward6;
                break;
        }
        index++;
        total = 0;
    }

    for (i = 0; i < 4; i++)
        cube[endRow][endCol][i].data ^= MUST_EXPLORE; //Turn the bit off

    fnList[index] = endPath;
    return fnList;
}

void translatefp(fp fn) {
    if (fn == forward) {
        printf("0x%d : forward\n", (int)fn);
    } else if (fn == forward2) {
        printf("0x%d : forward2\n", (int)fn);
    } else if (fn == forward3) {
        printf("0x%d : forward3\n", (int)fn);
    } else if (fn == forward4) {
        printf("0x%d : forward4\n", (int)fn);
    } else if (fn == forward5) {
        printf("0x%d : forward5\n", (int)fn);
    } else if (fn == forward6) {
        printf("0x%d : forward6\n", (int)fn);
    } else if (fn == backward) {
        printf("0x%d : backward\n", (int)fn);
    } else if (fn == backward2) {
        printf("0x%d : backward2\n", (int)fn);
    } else if (fn == backward3) {
        printf("0x%d : backward3\n", (int)fn);
    } else if (fn == backward4) {
        printf("0x%d : backward4\n", (int)fn);
    } else if (fn == backward5) {
        printf("0x%d : backward5\n", (int)fn);
    } else if (fn == backward6) {
        printf("0x%d : backward6\n", (int)fn);
    } else if (fn == rotLeft) {
        printf("0x%d : rotLeft\n", (int)fn);
    } else if (fn == rotRight) {
        printf("0x%d : rotRight\n", (int)fn);
    } else if (fn == endPath) {
        printf("0x%d : endPath\n", (int)fn);
    } else {
        printf("0x%d : unknown\n", (int)fn);
    }
}

void observe(char board[7][7], int votes[7][7][3], char row, char col, char orient, int move) {
    //Row, col and orient are the coordinate after movement. So if this is being called
    //while in motion, they belong to the destination square. If it's being called after
    //movement, they are representative of present conditions

    if (debug) printf("Observe (%d,%d): 0x%02x\n", (int) row, (int) col, move);

    //boardVotes keeps tally of obstacle measurements (boardVotes[x][y][0]),
    //main tunnel measurements (boardVotes[x][y][1]), and bonus tunnel
    //measurements (boardVotes[x][y][2]). At the end of observe, tallies are
    //counted and a decision is made, which may be overruled. IE tunnels will
    //never be on the corners and obstacles won't be on the edge

    int i, j;

    int xblock; //The starting column
    int yblock; //The starting row
    float xoffset; //The x offset in inches on the board (0 to 84)
    float yoffset; //The y offset in inches on the board (0 to 84)
    float ticsPerInch = GUIvars.ticsPerRev / (GUIvars.wheelDiam * PI);
    shared_memory->setpointPID.sp = 0; //Reset setpoint

    if (BLOCK) {
        //TODO: Code here to make onetime measurements and return
    }

    //Wait for motors to start, then initialize prevTics and prevDist
    while (query(STATUS) != ACTIVE) delay_ms(10);
    int prevTics = (shared_memory->motor[M1].distance + shared_memory->motor[M2].distance) / 2;
    float prevDist = 0;

    //Initialize block and offsets  
    if ((move & MOVE_FWD) || (move & MOVE_BWD)) {
        switch (orient) {
            case FORWARD:
                xblock = col;
                yblock = row - ((move & MOVE_FWD) ? (move & 0xF) : -1 * (move & 0xF));
                break;
            case LEFT:
                xblock = col + ((move & MOVE_FWD) ? (move & 0xF) : -1 * (move & 0xF));
                yblock = row;
                break;
            case BACKWARD:
                xblock = col;
                yblock = row + ((move & MOVE_FWD) ? (move & 0xF) : -1 * (move & 0xF));
                break;
            case RIGHT:
                xblock = col - ((move & MOVE_FWD) ? (move & 0xF) : -1 * (move & 0xF));
                yblock = row;
                break;
        }
    } else if (move & MOVE_ROTATE) {
        xblock = col;
        yblock = row;
    }
    xoffset = xblock * 12 + xAccumdrift + 6;
    yoffset = yblock * 12 + yAccumdrift + 6;

    while (query(STATUS) != COMPLETED) {
        //Read tics and calculate distance travelled (in cm)
        int tics = (shared_memory->motor[M1].distance + shared_memory->motor[M2].distance) / 2;
        float dist = tics / ticsPerInch;

        //Read acc and change setpoint if neccesary
        //Also, calculate exact position on board
        if (!DRIVEBLIND) {

            if (!(move & MOVE_ROTATE)) {

                //Read accelerometer and correct direction
                int drift = (getOrientation() - ((defaultHeading + (r * FULL_ROT / 4)) % FULL_ROT));
                if (drift > 0) shared_memory->setpointPID.sp = drift / -4 - 1;
                else if (drift < 0) shared_memory->setpointPID.sp = drift / -4 + 1;
                else shared_memory->setpointPID.sp = 0;
                //                shared_memory->setpointPID.sp = -1*drift; 

                //Set block and offset values
                switch (orient) {
                    case FORWARD:
                        yoffset = yblock * 12 + yAccumdrift + 6 + dist;
                        //Average the tic count for each motor and convert to cm

                        // printf("m1/2: %d/%d, target: %d/%d, tics/cm: %f\n", shared_memory->motor[M1].distance, 
                        //       shared_memory->motor[M2].distance, shared_memory->motor[M1].targetDistance, 
                        //       shared_memory->motor[M2].targetDistance, ticsPerInch);


                        //TODO: Calculate Accumdrifts . eg xAccumdrift += tan(drift in rad) * dist; xoffset += xAccumdrift;
                        //			         yAccumdrift += dist - sqrt(pow(dist,2) - pow(xAccumdrift)); yoffset += dist - yAccumdrift
                        //                                 Notice that Accumdrifts are in 16LSB fixed point. Accomodate or change that.
                        //TODO: Add previous drift to average in use of calculating Accumdrifts
                        //TODO: If Accumdrift passes a threshhold, turn to compensate. Do not correct course to increase an Accumdrift
                        //TODO: To counteract an Accumdrift parallel to path of travel, increase/decrease motor[M1/2].targetDistance
                        break;
                    case LEFT:
                        xoffset = xblock * 12 + xAccumdrift + 6 - dist;
                        break;
                    case BACKWARD:
                        yoffset = yblock * 12 + yAccumdrift + 6 - dist;
                        break;
                    case RIGHT:
                        xoffset = xblock * 12 + xAccumdrift + 6 + dist;
                        break;
                }
            }
        }

        //Read IR and stud finder and tally votes or assign tunnels as neccesary
        //TODO: Use location of obstacle to assign Accumdrifts
        int leftIR;
        int studfinder;
        switch (orient) {
            case FORWARD:
                //Should equal the y coordinate of the device's location
                leftIR = yoffset - 3;
                studfinder = yoffset + 2;

                if (leftIR % 12 > 1 && leftIR % 12 < 11) { //If within 1 inch margins
                    float leftDist = getDist(LEFTIR) + 9; //Distance the center of the bot is
                    //from the center of the obstacle
                    if (leftDist < IR_RANGE) {
                        //printf("Obstacle at (%f, %f): %f\n", xoffset, yoffset, leftDist-9);
                        /*
                         * +------------+------------+
                         * |<xoff><-----leftDist----------->
                         * |   +----+   |            |
                         * |   |    |   |            |
                         * |   |  <3><-getDist(LIR)-><--6-->
                         * |   +----+^  |            |
                         * |      leftIR|            |
                         * +---------|--+------------+
                         * |         |  |            |
                         */
                        votes[(int) (leftIR / 12)][(int) ((xoffset + leftDist) / 12)][VOTE_OBSTACLE]++;
                        
                        //If an obstacle is sensed, veer away from it
                        if(leftDist < 11 && votes[(int) (leftIR / 12)][(int) ((xoffset + leftDist) / 12)][VOTE_OBSTACLE] > 25)
                            shared_memory->setpointPID.sp--;                        

                        //Clear every other square in the way
                        for (i = 12; i < (int) (leftDist); i += 12) {
                            votes[(int) (leftIR / 12)][(int) ((xoffset + i) / 12)][VOTE_OBSTACLE]--;
                        }
                        //board[(int)(leftIR/12)][(int)((xoffset+leftDist)/12)] |= OBSTACLE;
                        //board[(int)(leftIR/12)][(int)((xoffset+leftDist)/12)] &= ~UNDEFINED;

                        /*
                         //Ideally, if we're perfectly centered, leftDist should
                         //be 12, 24, or max+9. Find deviation and set Accumdrift
                         //to that
                         
                         xAccumDrift = ((leftDist - 6) % 12) - 6;
                         //Maybe: xAccumDrift = (((leftDist-6) % 12)-6)/2 + xAccumDrift/2;
                         */
                    } else {
                        //Path clear. Clear all obstacles
                        for (i = 12; i < IR_RANGE; i += 12) {
                            votes[(int) (leftIR / 12)][(int) ((xoffset + i) / 12)][VOTE_OBSTACLE]--;
                        }
                    }
                    //Place obstacle at (xoffset/12 + leftDist / 12, yoffset/12)
                    //TODO: Use obstacle offset ((xoffset + leftDist) % 12 - 6) to calculate Accumdrift
                    //TODO: Take reading and apply to voting system
                }

                if (studfinder % 12 > 1 && studfinder % 12 < 11) {
                    if (pollStudFinder()) {
                        votes[(int) (studfinder / 12)][(int) (xoffset / 12)][VOTE_MAINTUNNEL] += TUNNEL_VOTE_WEIGHT;
                        //board[(int) (studfinder / 12)][(int) (xoffset / 12)] |= MAIN_TUNNEL;
                    } else {
                        votes[(int) (studfinder / 12)][(int) (xoffset / 12)][VOTE_MAINTUNNEL]--;
                    }
                    board[(int) (studfinder / 12)][(int) (xoffset / 12)] &= ~(UNDEFINED | UNVISITED);
                    //TODO: Take reading and apply to voting system
                }
                break;
            case LEFT:
                leftIR = xoffset + 3;
                studfinder = xoffset - 2;

                if (leftIR % 12 > 1 && leftIR % 12 < 11) {
                    float leftDist = getDist(LEFTIR) + 9;
                    if (leftDist < IR_RANGE) {
                        printf("Obstacle at (%f, %f): %f\n", xoffset, yoffset, leftDist-9);
                        votes[(int) (yoffset + leftDist) / 12][(int) (leftIR / 12)][VOTE_OBSTACLE]++;
                        
                        //If an obstacle is sensed, veer away from it
                        if(leftDist < 11 && votes[(int) (yoffset + leftDist) / 12][(int) (leftIR / 12)][VOTE_OBSTACLE] > 25)
                            shared_memory->setpointPID.sp--; 

                        //Clear every other square in the way
                        for (i = 12; i < (int) (leftDist); i += 12) {
                            votes[(int) ((yoffset + i) / 12)][(int) (leftIR / 12)][VOTE_OBSTACLE]--;
                        }
                    } else {
                        //Path clear. Clear all obstacles
                        for (i = 12; i < IR_RANGE; i += 12) {
                            votes[(int) ((yoffset + i) / 12)][(int) (leftIR / 12)][VOTE_OBSTACLE]--;
                        }
                    }
                }

                if (studfinder % 12 > 1 && studfinder % 12 < 11) {
                    if (pollStudFinder()) {
                        votes[(int) (yoffset / 12)][(int) (studfinder / 12)][VOTE_MAINTUNNEL] += TUNNEL_VOTE_WEIGHT;
                        //board[(int) (yoffset / 12)][(int) (studfinder / 12)] |= MAIN_TUNNEL;
                    } else {
                        votes[(int) (yoffset / 12)][(int) (studfinder / 12)][VOTE_MAINTUNNEL]--;
                    }
                    board[(int) (yoffset / 12)][(int) (studfinder / 12)] &= ~(UNDEFINED | UNVISITED);
                    //TODO: Take reading and apply to voting system
                }
                break;
            case BACKWARD:
                leftIR = yoffset + 3;
                studfinder = yoffset - 2;

                if (leftIR % 12 > 1 && leftIR % 12 < 11) {
                    float leftDist = getDist(LEFTIR) + 9;
                    if (leftDist < IR_RANGE) {
                        printf("Obstacle at (%f, %f): %f\n", xoffset, yoffset, leftDist - 9);
                        votes[(int) (leftIR / 12)][(int) ((xoffset - leftDist) / 12)][VOTE_OBSTACLE]++;
                        
                        //If an obstacle is sensed, veer away from it
                        if(leftDist < 11 && votes[(int) (leftIR / 12)][(int) ((xoffset - leftDist) / 12)][VOTE_OBSTACLE] > 25)
                            shared_memory->setpointPID.sp--;

                        //Clear every other square in the way
                        for (i = 12; i < (int) (leftDist); i += 12) {
                            votes[(int) (leftIR / 12)][(int) ((xoffset - i) / 12)][VOTE_OBSTACLE]--;
                        }
                    } else {
                        //Path clear. Clear all obstacles
                        for (i = 12; i < IR_RANGE; i += 12) {
                            votes[(int) (leftIR / 12)][(int) ((xoffset - i) / 12)][VOTE_OBSTACLE]--;
                        }
                    }
                }

                if (studfinder % 12 > 1 && studfinder % 12 < 11) {
                    if (pollStudFinder()) {
                        votes[(int) (studfinder / 12)][(int) (xoffset / 12)][VOTE_MAINTUNNEL] += TUNNEL_VOTE_WEIGHT;
                        //board[(int) (studfinder / 12)][(int) (xoffset / 12)] |= MAIN_TUNNEL;
                    } else {
                        votes[(int) (studfinder / 12)][(int) (xoffset / 12)][VOTE_MAINTUNNEL]--;
                    }
                    board[(int) (studfinder / 12)][(int) (xoffset / 12)] &= ~(UNDEFINED | UNVISITED);
                    //TODO: Take reading and apply to voting system
                }
                break;
            case RIGHT:
                leftIR = xoffset - 3;
                studfinder = xoffset + 2;

                if (leftIR % 12 > 1 && leftIR % 12 < 11) {
                    float leftDist = getDist(LEFTIR) + 9;
                    if (leftDist < IR_RANGE) {
                        printf("Obstacle at (%f, %f): %f\n", xoffset, yoffset, leftDist-9);
                        votes[(int) ((yoffset - leftDist) / 12)][(int) (leftIR / 12)][VOTE_OBSTACLE]++;
                        
                        //If an obstacle is sensed, veer away from it
                        if(leftDist < 11 && votes[(int) ((yoffset - leftDist) / 12)][(int) (leftIR / 12)][VOTE_OBSTACLE] > 25)
                            shared_memory->setpointPID.sp--;

                        //Clear every other square in the way
                        for (i = 12; i < (int) (leftDist); i += 12) {
                            votes[(int) ((yoffset - i) / 12)][(int) (leftIR / 12)][VOTE_OBSTACLE]--;
                        }
                    } else {
                        for (i = 12; i < IR_RANGE; i += 12) {
                            votes[(int) ((yoffset - i) / 12)][(int) (leftIR / 12)][VOTE_OBSTACLE]--;
                        }
                    }
                }

                if (studfinder % 12 > 1 && studfinder % 12 < 11) {
                    if (pollStudFinder()) {
                        votes[(int) (yoffset / 12)][(int) (studfinder / 12)][VOTE_MAINTUNNEL] += TUNNEL_VOTE_WEIGHT;
                        //board[(int) (yoffset / 12)][(int) (studfinder / 12)] |= MAIN_TUNNEL;
                    } else {
                        votes[(int) (yoffset / 12)][(int) (studfinder / 12)][VOTE_MAINTUNNEL]--;
                    }
                    board[(int) (yoffset / 12)][(int) (studfinder / 12)] &= ~(UNDEFINED | UNVISITED);
                    //TODO: Take reading and apply to voting system
                }
                break;
        }

        //TODO: Read lower IR
        //Modify board as appropriate

        //Kill time
        prevTics = tics;
        prevDist = dist;
        delay_ms(20);
    }

    shared_memory->command.code = NOP;
    shared_memory->command.status = IDLE;
    shared_memory->state = 0;

    //Count votes
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            if (votes[i][j][VOTE_OBSTACLE] > 0) {
                board[i][j] |= OBSTACLE;
                board[i][j] &= ~UNDEFINED;
                //printf("(%d, %d): %d YAY\n", i, j, votes[i][j][VOTE_OBSTACLE]);
            } else if (votes[i][j][VOTE_OBSTACLE] < 0) {
                board[i][j] &= ~OBSTACLE;
                board[i][j] &= ~UNDEFINED;
                //printf("(%d, %d): %d NAY\n", i, j, votes[i][j][VOTE_OBSTACLE]);
            }

            if (votes[i][j][VOTE_MAINTUNNEL] > 0) {
                board[i][j] |= MAIN_TUNNEL;
            } else if (votes[i][j][VOTE_MAINTUNNEL] < 0) {
                board[i][j] &= ~MAIN_TUNNEL;
            }
            
            //If on an edge, there won't be any obstacles
            if (i == 0 || i == 6 || j == 6 || j == 0) board[i][j] &= ~OBSTACLE;

            //If on a corner, can't be a tunnel

            if ((i == 0 && (j == 0 || j == 6)) || (i == 6 && (j == 0 || j == 6))) board[i][j] &= ~MAIN_TUNNEL;
        }
    }
    printf("\n");
    printBoard(board);
}

void runRound() {
    char board[7][7]; //TODO: Explore the possibility of making this global
    int votes[7][7][3];
    int i, j, k, margin;


    //TODO: Initialize board
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            board[i][j] = (UNDEFINED | UNVISITED);
            for (k = 0; k < 3; k++) {
                votes[i][j][k] = 0;
            }
        }
        board[i][0] &= ~UNDEFINED;
        board[i][6] &= ~UNDEFINED;
    }
    for (j=1; j < 6; j++) {
        board[0][j] &= ~UNDEFINED;
        board[6][j] &= ~UNDEFINED;
    }

    printBoard(board);

    x = y = 0; //These 3 variables are set here and then never touched by
    r = FORWARD; //this function. Only updateLocation() edits them
    //updateLocation is called by individual movement functions,
    //so when program is in non-blocking mode, they are updated
    //prematurely, before the robot is done moving. As long as
    //observe() is called after each movement, this isn't an issue

    fp path[100];

    //	//DEBUGGING
    //            if (shortestPath(board, path, y, x, r, 6, 0, FORWARD) != NULL) {
    //                travelPath(board, votes, path, y, x, r);
    //            }
    //	return;
    
    fwd(1,SPEED);   //Center the axis on the square
    if (!BLOCK) {
        waitForComplete();
        shared_memory->command.code = NOP;
        shared_memory->command.status = IDLE;
        shared_memory->state = 0;
    }
    correctRotation();
    delay_ms(250);

    printf("Begin spiral\n");
    //Begin spiral inwards
    for (margin = 0; margin < 3; margin++) {
        //Travel first along left column, up to 5th square
        if (shortestPath(board, path, y, x, r, 6 - margin, margin, FORWARD) != NULL) {
            travelPath(board, votes, path, y, x, r);
        }

//        //DEBUGGING
//        waitGO();
//        delay_ms(750);

        //Travel along top row
        if (shortestPath(board, path, y, x, r, 6 - margin, 6 - margin, RIGHT) != NULL) {
            travelPath(board, votes, path, y, x, r);
        }

//        //DEBUGGING
//        waitGO();
//        delay_ms(750);

        //Travel along left column
        if (shortestPath(board, path, y, x, r, margin, 6 - margin, BACKWARD) != NULL) {
            travelPath(board, votes, path, y, x, r);
        }

//        //DEBUGGING
//        waitGO();
//        delay_ms(750);

        //Travel along bottom row to 2nd square
        if (shortestPath(board, path, y, x, r, margin, 1 + margin, LEFT) != NULL) {
            travelPath(board, votes, path, y, x, r);
        }

//        //DEBUGGING
//        waitGO();
//        delay_ms(750);
    }

    //Go to center square
    if (shortestPath(board, path, y, x, r, 3, 3, FORWARD) != NULL) {
        travelPath(board, votes, path, y, x, r);
    }

//    //DEBUGGING
//    waitGO();
//    delay_ms(750);

    //Post travel analysis
    if (!tunnelGapFill(board)) {
        for(i = 0; i < 7; i++) {
            for(j = 0; j < 7; j++) {
                if(board[i][j] & UNVISITED && shortestPath(board, path, y, x, r, i, j, r) != NULL) {
                    printf("Missed (%d, %d). Exploring now\n", i, j);
                    travelPath(board, votes, path, y, x, r);
                }
            }
        }
    } else {
        //If a tunnel was calculated, make sure no subsequent observations will
        //interfere
        for(i = 0; i < 7; i++) {
            for(j = 0; j < 7; j++) {
                if(board[i][j] & MAIN_TUNNEL) votes[i][j][VOTE_MAINTUNNEL] = 10000;
                else votes[i][j][VOTE_MAINTUNNEL] = -10000;
            }
        }
    }
    printBoard(board);

    int endRow;
    int endCol;
    int endOrient;
    for(i = 1; i < 6; i++) {
        if(board[i][0] & MAIN_TUNNEL) {		//Check left edge
            printf("On left edge\n");
            endRow = i-1;
            endCol = 0;
            endOrient = FORWARD;
            break;
        } else if (board[6][i] & MAIN_TUNNEL) {	//Check top edge
            printf("On top edge\n");
            endRow = 6;
            endCol = i-1;
            endOrient = RIGHT;
            break;
        } else if (board[i][6] & MAIN_TUNNEL) {	//Check right edge
            printf("On right edge\n");
            endRow = i+1;
            endCol = 6;
            endOrient = BACKWARD;
            break;
        } else if (board[0][i] & MAIN_TUNNEL) {	//Check bottom edge
            printf("On bottom edge\n");
            endRow = 0;
            endCol = i+1;
            endOrient = LEFT;
            break;
        }	
    }

    if(shortestPath(board, path, y, x, r, endRow, endCol, endOrient) != NULL) {
        travelPath(board, votes, path, y, x, r);
    }

    dieReadingSequence();

    //Return to starting square
    if (shortestPath(board, path, y, x, r, 0, 0, FORWARD) != NULL) {
        travelPath(board, votes, path, y, x, r);
    }
    
    bwd(1,SPEED);   //Move the robot entirely within the square
    if (!BLOCK) {
        waitForComplete();
        shared_memory->command.code = NOP;
        shared_memory->command.status = IDLE;
        shared_memory->state = 0;
    }
    correctRotation();

    tunnelGapFill(board);
    printBoardFinal(board);
}

//DEBUGGING---------------------------------------------------------------------

void testDrive() {
    int i, j;
    initAMG();
    printf("Orientation: %f\n", getOrientation() / FXDPT);
    fp movements[64];
    for (i = 0; i < 16; i++) {

        rotRight(0);
        usleep(100000);
    }
    movements[10] = endPath;
    //    for(j = 0; i < 4; j++) {
    //        for(i = 0; i < 8; i+=2) {
    //            printf("j=%d, i=%d\n",j,i);
    //            movements[j*16+i] = forward;
    //            movements[j*16+i+1] = rotRight;
    //        } 
    //        for(i = 8; i < 16; i+=2) {
    //            printf("j=%d, i=%d\n",j,i);
    //            movements[j*16+i] = forward;
    //            movements[j*16+i+1] = rotLeft;
    //        }
    //        printf("j=%d\n",j);
    //    }
    //   
    //    i = j = 0;
    //    do {
    //        j=movements[i](j);
    //        i++;
    //    }
    //    while(j);

    //    for(i = 0; i < 8; i++) {
    //          for(j = 0; j < 32; j++) {
    //                forward(0);
    //                rotRightSingleWheel(0);
    //          }
    //          for(j = 0; j < 4; j++) {
    //              rotate(90,9,CCW);
    //          }
    //          for(j = 0; j < 2; j++) {
    //              uturn();
    //          }
    //        forward(0);
    ////        readDataCSV();
    //        rotRightSingleWheel(0);
    ////        readDataCSV();
    //        forward(0);
    ////        readDataCSV();
    //        rotLeftSingleWheel(0);
    ////        readDataCSV();
    //        forward(0);
    ////        readDataCSV();
    //        rotLeftSingleWheel(0);
    ////        readDataCSV();
    //        forward(0);
    ////        readDataCSV();
    //        rotLeftSingleWheel(0);
    ////        readDataCSV();
    //        forward(0);
    ////        readDataCSV();
    //        rotLeftSingleWheel(0);
    ////        readDataCSV();
    //        forward(0);
    ////        readDataCSV();
    //        rotRightSingleWheel(0);
    ////        readDataCSV();
    //        forward(0);
    ////        readDataCSV();
    //        rotRightSingleWheel(0);
    ////        readDataCSV();
    //        forward(0);
    ////        readDataCSV();
    //        rotRightSingleWheel(0);
    //    }
}

void testShortestPath() {
    char board[7][7];
    int r = 6;
    int c = 0;

    board[0][0] = EMPTY;
    board[0][1] = EMPTY;
    board[0][2] = EMPTY;
    board[0][3] = EMPTY;
    board[0][4] = EMPTY;
    board[0][5] = EMPTY;
    board[0][6] = UNDEFINED;
    board[1][0] = EMPTY;
    board[1][1] = EMPTY;
    board[1][2] = EMPTY;
    board[1][3] = EMPTY;
    board[1][4] = OBSTACLE;
    board[1][5] = EMPTY;
    board[1][6] = UNDEFINED;
    board[2][0] = EMPTY;
    board[2][1] = EMPTY;
    board[2][2] = EMPTY;
    board[2][3] = OBSTACLE;
    board[2][4] = EMPTY;
    board[2][5] = EMPTY;
    board[2][6] = UNDEFINED;
    board[3][0] = EMPTY;
    board[3][1] = OBSTACLE;
    board[3][2] = EMPTY;
    board[3][3] = EMPTY;
    board[3][4] = EMPTY;
    board[3][5] = EMPTY;
    board[3][6] = UNDEFINED;
    board[4][0] = EMPTY;
    board[4][1] = OBSTACLE;
    board[4][2] = EMPTY;
    board[4][3] = UNDEFINED;
    board[4][4] = UNDEFINED;
    board[4][5] = UNDEFINED;
    board[4][6] = UNDEFINED;
    board[5][0] = EMPTY;
    board[5][1] = OBSTACLE;
    board[5][2] = EMPTY;
    board[5][3] = UNDEFINED;
    board[5][4] = UNDEFINED;
    board[5][5] = UNDEFINED;
    board[5][6] = UNDEFINED;
    board[6][0] = EMPTY;
    board[6][1] = EMPTY;
    board[6][2] = EMPTY;
    board[6][3] = UNDEFINED;
    board[6][4] = UNDEFINED;
    board[6][5] = UNDEFINED;
    board[6][6] = UNDEFINED;

    unsigned char startRow = 3;
    unsigned char startCol = 0;
    unsigned char startDir = FORWARD;
    unsigned char endRow = 2;
    unsigned char endCol = 4;
    unsigned char endDir = BACKWARD;

    if (debug) {
        printf("+---------------+\n");
        for (r = 6; r >= 0; r--) {
            printf("|");
            for (c = 0; c < 7; c++) {
                if (r == startRow && c == startCol) printf(" S"); //Starting square
                else if (r == endRow && c == endCol) printf(" E"); //Ending square
                else if (board[r][c] & 0x80) printf(" ?"); //Undefined
                else if (board[r][c] & 0x04) printf(" x"); //Obstacle
                else if (board[r][c] & 0x02) printf(" O"); //Tunnel
                else printf("  "); //Empty
            }
            printf(" |\n");
        }
        printf("+---------------+\n");
    }

    if (debug) printf("start (%hhx,%hhx,%s)\n", startRow, startCol, getDirection(startDir));
    fp fnList[64];
    shortestPath(board, fnList, startRow, startCol, startDir, endRow, endCol, endDir);
    int i = 0;
    while (fnList[i]()) i++;

    if (debug) printf("final (%hhx,%hhx,%s)\n", endRow, endCol, getDirection(endDir));
}

void testRobot(void) {
    int i;

    if (debug) printf("Entering testRobot()\n");

    //    for(i = 0; i < 10000; i++) {
    //        printf("%d\n", getOrientation());
    //        delay_ms(500);
    //    }

    //   while(1) {
    //        if(pollStudFinder()) printf("Stud finder ON\n");
    //        else printf("Stud finder OFF\n");
    //        printf("IR: %f\n\n", getDist(LEFTIR));
    //        delay_ms(500);
    //   } 


    dieReadingSequence();

    //    for(i = 0; i < 16; i++) {
    //        rotRight(0);
    //    }
    //    delay_ms(999);
    //    for(i = 0; i < 16; i++) {
    //        rotLeft(0);
    //    }

    //    int diecount = dieCount();
    //    printf("Count: %d\n", diecount);

    if (debug) printf("Leaving testRobot()\n");
    return;
}
