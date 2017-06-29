/* arch_sigtable.h COPYRIGHT FUJITSU LIMITED 2015 */

#ifndef __X86_ARCH_SIGTABLE_H
#define __X86_ARCH_SIGTABLE_H

#include <signal.h>
/* for x86 signal name list */
static char *sig_name[] = {
	"DUMMY",									/*  0, dummy */
	"SIGHUP",            "SIGINT",  "SIGQUIT", "SIGILL",          "SIGTRAP",	/*  1- 5 */
	"SIGABRT / SIGIOT",  "SIGBUS",  "SIGFPE",  "SIGKILL",         "SIGUSR1",	/*  6-10 */
	"SIGSEGV",           "SIGUSR2", "SIGPIPE", "SIGALRM",         "SIGTERM",	/* 11-15 */
	"SIGSTKFLT",         "SIGCHLD", "SIGCONT", "SIGSTOP",         "SIGTSTP",	/* 16-20 */
	"SIGTTIN",           "SIGTTOU", "SIGURG",  "SIGXCPU",         "SIGXFSZ",	/* 21-25 */
	"SIGVTALRM",         "SIGPROF", "SIGWINCH","SIGIO / SIGPOLL", "SIGPWR",		/* 26-30 */
	"SIGSYS / SIGUNUSED"								/* 31 */
};

/* for x86 signal num table */
static int sig_num[] = {
	SIGHUP,    SIGINT,    SIGQUIT,  SIGILL,  SIGTRAP,		/*  1- 5 */
	SIGABRT,   SIGIOT,    SIGBUS,   SIGFPE,  SIGKILL, SIGUSR1,	/*  6-10(SIGABRT==SIGIOT) */
	SIGSEGV,   SIGUSR2,   SIGPIPE,  SIGALRM, SIGTERM,		/* 11-15 */
	SIGSTKFLT, SIGCHLD,   SIGCONT,  SIGSTOP, SIGTSTP,		/* 16-20 */
	SIGTTIN,   SIGTTOU,   SIGURG,   SIGXCPU, SIGXFSZ,		/* 21-25 */
	SIGVTALRM, SIGPROF,   SIGWINCH, SIGIO,   SIGPOLL, SIGPWR,	/* 26-30(SIGIO==SIGPOLL) */
	SIGSYS,    SIGUNUSED						/* 31(SIGSYS==SIGUNUSED) */
};

#endif /* __X86_ARCH_SIGTABLE_H */
