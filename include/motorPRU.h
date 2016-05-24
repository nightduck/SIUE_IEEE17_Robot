#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "prussdrv.h"
#include "pruss_intc_mapping.h"
#include "mio.h"
#include "BBBlib.h"

#define 	ON		1
#define		OFF		0

#define		PRU0		0
#define		PRU1		1

#define		SWITCH		47
#define		LED		44
#define		ACC_INT		2


/*
Need to add these to a struct that will be declared
static void *pru0DataMemory;
static unsigned int *pru0DataMemory_int;

static void *pru1DataMemory;
static unsigned int *pru1DataMemory_int;

static void *pruSharedDataMemory;
static unsigned int *pruSharedDataMemory_int;
*/

void motorInit(void);
void motorCleanup(void);

//idea functions
/*

retrive PID structure for data storage in memory
void *motorGetData(void);
void *motorSetData(struct PID);


*/ 
