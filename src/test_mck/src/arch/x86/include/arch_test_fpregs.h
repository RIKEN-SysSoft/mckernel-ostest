/* arch_test_fpregs.h COPYRIGHT FUJITSU LIMITED 2016 */

#ifndef __X86_ARCH_TEST_FPREGS_H
#define __X86_ARCH_TEST_FPREGS_H

#include "test_data_sample.h"

#define FPREGS_TEST_DATA SAMPLE_TEST_DATA_512BYTES

struct fpregs {
     union  {
          __uint8_t   data[512];
          __uint64_t  ddata[64];
          __uint128_t qdata[32];
     };
};

static inline void fpregs_write(struct fpregs *regs) {
	// TODO: Impl.
	printf("Please implement fpregs_write()\n");
}
static inline void fpregs_read(struct fpregs *regs) {
	// TODO: Impl.
	printf("Please implement fpregs_read()\n");
}

static inline void show_current_fpregs(void)
{
	struct fpregs fregs;
	int i;

	memset(&fregs, 0, sizeof(struct fpregs));
	fpregs_read(&fregs);

	for (i = 0; i < 32; i++) {
		printf("q%02d: 0x%016lx_%016lx\n", 
			i, fregs.ddata[(i*2)+1], fregs.ddata[i*2]); // swap because endianess.
	}

	return;
}

#endif /* __X86_ARCH_TEST_FPREGS_H */
