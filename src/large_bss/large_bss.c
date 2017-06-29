/* large_bss.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <stdlib.h>

unsigned long bss_area[0x10000000]; /* 2 GB */

int main(int argc, char** argv)
{
	printf("large_bss TP done.\n");
	return 0; 
}
