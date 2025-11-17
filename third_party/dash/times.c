/*
 * Copyright (c) 1999 Herbert Xu <herbert@gondor.apana.org.au>
 * This file contains code for the times builtin.
 */

#include <sys/times.h>
#include <unistd.h>
#ifdef USE_GLIBC_STDIO
#include <stdio.h>
#else
#include "bltin.h"
#endif
#include "system.h"

int timescmd(int argc, char *argv[]) {
	struct tms buf;
	long int clk_tck = sysconf(_SC_CLK_TCK);
	int mutime, mstime, mcutime, mcstime;
	double utime, stime, cutime, cstime;

	times(&buf);

	utime = (double)buf.tms_utime / clk_tck;
	mutime = utime / 60;
	utime -= mutime * 60.0;

	stime = (double)buf.tms_stime / clk_tck;
	mstime = stime / 60;
	stime -= mstime * 60.0;

	cutime = (double)buf.tms_cutime / clk_tck;
	mcutime = cutime / 60;
	cutime -= mcutime * 60.0;

	cstime = (double)buf.tms_cstime / clk_tck;
	mcstime = cstime / 60;
	cstime -= mcstime * 60.0;

	printf("%dm%fs %dm%fs\n%dm%fs %dm%fs\n", mutime, utime, mstime, stime,
	       mcutime, cutime, mcstime, cstime);
	return 0;
}
