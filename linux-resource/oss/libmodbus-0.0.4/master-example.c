#include <stdlib.h>
#include <stdio.h>
#include "modbus.h"

/* compiling : gcc master-example.c -o master-example -lmodbus */

int main()
{
   Mbm_trame trame;
   Sen_value senval;
   int result;
   int data_in[128];
   int data_out[128];
   
   printf("build:%s,%s\n",__TIME__,__DATE__);
   /* print debugging informations */
   Mb_verbose=1;

   trame.slave=1;				/* slave number*/ 
   trame.function=3;			/* send data */
   trame.address=0;
   trame.length=6;
   trame.timeout=1;
   getSenVal(trame, &senval, data_in, data_out);

   return 0;
}

