/* gle.c */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "child.h"
#include "utilities.h"


void main(int argc,char **argv) {
   	FILE 	*read_from, *write_to;
   	int 	childpid ;
	char	str[80] ;
    float   I[5] = {0.0, 0.1, 0.2, 0.3, 0.4} ;
    float   x, y ;

    x = 0.15 ; y = 0.65 ;

//    locate(I, &x, &y) ;

/* Launch tclsh .. set up read and write ports */
   
   	childpid = start_child("tclsh", &read_from, &write_to);

/* Tell tclsh to source the tcl script */
/* Anything sent to us from tcl script should be printed to screen */

   fprintf(write_to,"source touch_screen.tcl\n");
   while(1) {
        if (fgets(str, 80, read_from) <= 0) exit(0);
 		else printf("%s\n", str) ;
   }
}

