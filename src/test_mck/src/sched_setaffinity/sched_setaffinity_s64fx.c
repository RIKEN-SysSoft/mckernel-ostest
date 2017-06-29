/* sched_setaffinity_s64fx.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#if defined(__sparc)
void write_xg(unsigned long xg0, unsigned long xg1, unsigned long xg2, unsigned long xg3)
{
	__asm__ __volatile__(
	"sxar2\n\t"
	"mov	%0, %%xg0\n\t"
	"mov	%1, %%xg1\n\t"
	"sxar2\n\t"
	"mov	%2, %%xg30\n\t"
	"mov	%3, %%xg31\n\t"
	: : "r" (xg0), "r" (xg1), "r" (xg2), "r" (xg3));
}

void read_xg(unsigned long *xg0, unsigned long *xg1, unsigned long *xg2, unsigned long *xg3)
{
	__asm__ __volatile__(
	"sxar2\n\t"
	"stx,d	%%xg0, [%0]\n\t"
	"stx,d	%%xg1, [%1]\n\t"
	"sxar2\n\t"
	"stx,d	%%xg30, [%2]\n\t"
	"stx,d	%%xg31, [%3]\n\t"
	: : "r" (xg0), "r" (xg1), "r" (xg2), "r" (xg3));

}

void clear_xg(void)
{
	__asm__ __volatile__(
	"sxar2\n\t"
	"mov	%%g0, %%xg0\n\t"
	"mov	%%g0, %%xg1\n\t"
	"sxar2\n\t"
	"mov	%%g0, %%xg30\n\t"
	"mov	%%g0, %%xg31\n\t"
	: : );
}

void write_xf(float *xf0, float *xf1, float *xf2, float *xf3)
{
	__asm__ __volatile__(
	"sxar2\n\t"
	"ld	[%0], %%f64\n\t"
	"ld	[%1], %%f66\n\t"
	"sxar2\n\t"
	"ld	[%2], %%f508\n\t"
	"ld	[%3], %%f510\n\t"
	: : "r" (xf0), "r" (xf1), "r" (xf2), "r" (xf3));
}

void read_xf(float *xf0, float *xf1, float *xf2, float *xf3)
{
	__asm__ __volatile__(
	"sxar2\n\t"
	"st	%%f64, [%0]\n\t"
	"st	%%f66, [%1]\n\t"
	"sxar2\n\t"
	"st	%%f508, [%2]\n\t"
	"st	%%f510, [%3]\n\t"
	: : "r" (xf0), "r" (xf1), "r" (xf2), "r" (xf3));
}

void clear_xf(void)
{
	__asm__ __volatile__(
	"sxar2\n\t"
	"fzero	%%f64\n\t"
	"fzero	%%f66\n\t"
	"sxar2\n\t"
	"fzero	%%f508\n\t"
	"fzero	%%f510\n\t"
	: : );
}

void show_extended_registers(void)
{
	unsigned long xgr[] = { 0, 0, 0, 0 };
	float xfr[] = { 0, 0, 0, 0 };

	read_xg(&xgr[0], &xgr[1], &xgr[2], &xgr[3]);
	read_xf(&xfr[0], &xfr[1], &xfr[2], &xfr[3]);
	printf("xg0 = %lx, xg1 = %lx, xg30 = %lx, xg31 = %lx\n", xgr[0], xgr[1], xgr[2], xgr[3]);
	printf("f64 = %f, f66 = %f, f508 = %f, f510 = %f\n", xfr[0], xfr[1], xfr[2], xfr[3]);
}

#else

void write_xg(unsigned long xg0, unsigned long xg1, unsigned long xg2, unsigned long xg3){};
void write_xf(float *xf0, float *xf1, float *xf2, float *xf3){};
void show_extended_registers(void){};

#endif
