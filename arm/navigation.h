/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   navigation.h
 * Author: nightduck
 *
 * Created on November 7, 2016, 12:15 PM
 */

#ifndef NAVIGATION_H
#define NAVIGATION_H

#define DRIVEBLIND	FALSE

#define SPEED		7

#define FORWARD		0
#define RIGHT   	1
#define BACKWARD     	2
#define LEFT    	3

#define CW		0
#define CCW		1
#define SCW		2
#define SCCW		3

#define UNDEFINED	0x80
#define MUST_EXPLORE	0x40	//For when a square is undefined, but you want to go there anyways
#define UNVISITED       0x20    //When you have not been directly over a square
#define CANDIDATE       0x10    //For use in the tunnelGapFill AI
#define OBSTACLE	0x04
#define TUNNEL		0x02
#define MAIN_TUNNEL	0x03
#define EMPTY		0x00

#define FXDPT		16.0	//Constant to divide by to convert *AccumDrift (fixed point) to floating point (see navigation.c)
#define FULL_ROT	5760	//360*16. This a complete 360 when stored in the fixed point format
#define LEFT_TURN_OFFSET        0	//Robot overturns when turing left. This is to compensate
#define SINGLE_WHEEL_MULTIPLIER 0.958   //Multiplier to use when rotating on one wheel

//Return values for movement functions
#define MOVE_FWD		0x10
#define MOVE_FWD1		0x11	//Forward movements
#define MOVE_FWD2		0x12
#define MOVE_FWD3		0x13
#define MOVE_FWD4		0x14
#define MOVE_FWD5		0x15
#define MOVE_FWD6		0x16
#define MOVE_BWD		0x20
#define MOVE_BWD1		0x21	//Backward movements
#define MOVE_BWD2		0x22
#define MOVE_BWD3		0x23
#define MOVE_BWD4		0x24
#define MOVE_BWD5		0x25
#define MOVE_BWD6		0x26
#define MOVE_ROTATE		0x40	//&ing with this will return true for any rotation motion
#define MOVE_RLEFT		0x40	//Rotation with both wheels in opposite directions
#define MOVE_RRIGHT		0x41
#define MOVE_RSWLEFT		0x42	//Rotation with only one wheel moving at once
#define MOVE_RSWRIGHT		0x43
#define MOVE_RFLEFT		0x44	//Rotation and moving forward in an arc
#define MOVE_RFRIGHT		0x45
#define MOVE_UTURN		0x4F

//Indices for voting matrix
#define VOTE_OBSTACLE		0
#define VOTE_MAINTUNNEL		1
#define VOTE_BONUSTUNNEL	2

//Define the weighting factor for observing tunnels
#define TUNNEL_VOTE_WEIGHT      50

//Distance to shorten a backward movement
#define BWD_OFFSET 0

//Max distance the IR can read (actually the max is this # minus 9)
#define IR_RANGE    30

typedef int (*fp)();

//Path finding algorithm. Returns array of function pointers that when executed,
//get you from point A to point B
struct square {
    unsigned char data;                //Contains information about square
    unsigned char dist;        //Used for BFS
    unsigned char row;
    unsigned char col;
    unsigned char orient;
    struct square * parent;        //Used for BFS
    struct square * next;            //Points to the next square in the shortest path (calculated after BFS)
};

//Each distance needs it's own function since they can't take parameters
//Moves forward 1 square
int forward(int prev);

//Moves forward 2 squares.
int forward2(int prev);

//Moves forward 3 squares
int forward3(int prev);

//Moves forward 4 squares
int forward4(int prev);

//Moves forward 5 squares
int forward5(int prev);

//moves forward 6 squares
int forward6(int prev);

//Moves backward 1 square
int backward(int prev);

//Moves backward 2 square
int backward2(int prev);

//Moves backward 3 square
int backward3(int prev);

//Moves backward 4 square
int backward4(int prev);

//Moves backward 5 square
int backward5(int prev);

//Moves backward 6 square
int backward6(int prev);

//Rotates left 90 degrees
int rotLeft(int prev);

//Rotates right 90 degrees
int rotRight(int prev);

//Rotates left 90 degrees using one wheel
int rotLeftSingleWheel(int prev);

//Rotates right 90 degrees using one wheel
int rotRightSingleWheel(int prev);

//Fluid equivalent of forward, left, forward
int leftFwd(int prev);

//Fluid equivalent of forward, right, forward
int rightFwd(int prev);

//Fluid equivalent of forward, left, forward, right, forward
int leftSigmoid(int prev);

//Fluid equivalent of forward, right, forward, left, forward
int rightSigmoid(int prev);

//Rotates 180 degrees
int uturn(int prev);

//Fluid equivalent of forward, left, forward, left, forward
int uturnLeft(int prev);

//Fluid equivalent of forward, right, forward, right, forward
int uturnRight(int prev);

//Brake
int brake(int prev);

//End function
int endPath(int prev);

//Function to rotate slightly to correct and rotational drift
void correctRotation();

//Given a list of functions, travels the path given. May use substitute motions
//or modify path based on obstacles on the board. This does NOT generate a
//continuous fluid motion (though, if that were implemented, this is where it
//would be).
void travelPath(char board[7][7], int votes[7][7][3], fp* path, char row, char col, char orient); 

//Given fp, will print name of function (for debugging purposes)
void translatefp(fp fn);

//Assuming the die to read is in front, this is the sequence of motions to pick
//up a lid and read the die. It moves forward, calls lift lid, backs up, reads
//the die, and prints the appropriate value on the 7 segment display
void dieReadingSequence(); 

//tunnelGapFill takes a board with possibly undetected tunnels and modifies it to fill the gaps
int tunnelGapFill(char board[7][7]);


//Path finding algorithm. Returns array of function pointers that when executed,
//get you from point A to point B
//Usage:
//      void * funcs = pathFind(board, start, end);
//      int i = 0;
//      while(funcs[i]()) i++;
fp * shortestPath(char board[7][7], fp *fnList, char startRow, char startCol, char startOrient, char endRow, char endCol, char endOrient);

void observe(char board[7][7], int votes[7][7][3], char row, char col, char orient, int move);
void runRound();

void testDrive();
void testShortestPath();
void testRobot(void);
#endif /* NAVIGATION_H */
