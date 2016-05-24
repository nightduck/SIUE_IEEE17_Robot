#include "motorPRU.h"

void motorInit(void){
    
    tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

    // Allocate and initialize memory for PRU Driver
    prussdrv_init();
    // Open interupt channel for PRU0 and PRU1
    prussdrv_open (PRU_EVTOUT_0);
    prussdrv_open (PRU_EVTOUT_1);
    
    // Map PRU INTC
    prussdrv_pruintc_init(&pruss_intc_initdata);

    //Load and execute binaries
    //  Give the start address for PRU0 code
    prussdrv_exec_program_at(PRU0, "./text.bin", START_ADDR);
    prussdrv_exec_program(PRU1, "./pru1.bin");
    
    return ;
}

void motorCleanup(void){
    
    prussdrv_pru_disable(PRU0);
    prussdrv_pru_disable(PRU1);
    prussdrv_exit();
    return ;
}



    
