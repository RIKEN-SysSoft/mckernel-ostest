/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "test_mck.h"
#include "testsuite.h"

#define BUF_LEN 256

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	unsigned long *buf_1 = NULL;
	const int num = BUF_LEN / sizeof(unsigned long) ;
	unsigned long buf_2[ num ];
	unsigned long dump_off = 0;
	char *segv_addr = NULL;
	int i = 0;

	/* allocate */
	buf_1 = calloc(BUF_LEN, 1);
	if (buf_1 == NULL) {
		printf("calloc() failed. %d\n", errno);
		tp_assert(0, "TP failed.");
	}

    printf( "\n=== buf_1 =====================================================\n" ) ;

	/* write magic number */
	printf("buf_1 = %p\n", buf_1);
	for (i = 0; i < BUF_LEN / sizeof(unsigned long); i++) {
		*(buf_1 + i) = i;
	}

	/* dump */
	i = 0;
	dump_off = (unsigned long)buf_1;
	while (i < BUF_LEN / sizeof(unsigned long)) {
		printf("%#lx:\t0x%016lx\t0x%016lx\n",
			dump_off, *(buf_1 + i), *(buf_1 + i + 1) );
		i += 2;
		dump_off += sizeof(unsigned long) * 2;
	}

    printf( "\n=== buf_2 =====================================================\n" ) ;

	/* write magic number */
	printf("buf_2 = %p\n", buf_2);
	for (i = 0; i < BUF_LEN / sizeof(unsigned long); i++) {
		buf_2[i] = ~i;
	}

	/* dump */
	i = 0;
	for (i = 0; i < num ; i++) {
		
		printf( "0x%016lx", buf_2[i] );
		
		if(i+1 < num) printf(", ") ;
				
		if( ( (i+1) % 3 ) == 0 )  printf( "\n" ) ;

	}
	printf("\n") ;

	printf("\n") ;
	printf("you need check corefile by GDB.\n");
	fflush(stdout);

     /* Initialize general registers (set expect data)   */
     /* (exclude x29-31)                                 */
	register_write();

	/* generate corefile by SIGSEGV */
	*segv_addr = 0;

	/* Why return ??? */
	return "Why return NULL access ???";
}
