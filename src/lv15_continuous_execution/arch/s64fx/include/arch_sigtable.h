/* arch_sigtable.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __SPARC_ARCH_SIGTABLE_H
#define __SPARC_ARCH_SIGTABLE_H

#include <signal.h>
/* for sparc signal name list */
static char *sig_name[] = {
	"DUMMY",								/*  0, dummy */
	"SIGHUP",           "SIGINT",  "SIGQUIT",  "SIGILL",  "SIGTRAP",	/*  1- 5 */
	"SIGABRT / SIGIOT", "SIGEMT",  "SIGFPE",   "SIGKILL", "SIGBUS",		/*  6-10 */
	"SIGSEGV",          "SIGSYS",  "SIGPIPE",  "SIGALRM", "SIGTERM",	/* 11-15 */
	"SIGURG",           "SIGSTOP", "SIGTSTP",  "SIGCONT", "SIGCHLD",	/* 16-20 */
	"SIGTTIN",          "SIGTTOU", "SIGIO",    "SIGXCPU", "SIGXFSZ",	/* 21-25 */
	"SIGVTALRM",        "SIGPROF", "SIGWINCH", "SIGLOST", "SIGUSR1",	/* 26-30 */
	"SIGUSR2"								/* 31 */
};

/* for sparc signal num list */
static int sig_num[] = {
	SIGHUP,    SIGINT,  SIGQUIT,  SIGILL,  SIGTRAP,		/*  1- 5 */
	SIGABRT,   SIGIOT,  SIGEMT,   SIGFPE,  SIGKILL, SIGBUS,	/*  6-10(SIGABRT==SIGIOT) */
	SIGSEGV,   SIGSYS,  SIGPIPE,  SIGALRM, SIGTERM,		/* 11-15 */
	SIGURG,    SIGSTOP, SIGTSTP,  SIGCONT, SIGCHLD,		/* 16-20 */
	SIGTTIN,   SIGTTOU, SIGIO,    SIGXCPU, SIGXFSZ,		/* 21-25 */
	SIGVTALRM, SIGPROF, SIGWINCH, SIGLOST, SIGUSR1,		/* 26-30 */
	SIGUSR2							/* 31 */
};

#endif /* __SPARC_ARCH_SIGTABLE_H */
