/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/pyconfig.h"
#include "third_party/python/Include/pyfpe.h"
/*
 * The signal handler for SIGFPE is actually declared in an external
 * module fpectl, or as preferred by the user.  These variable
 * definitions are required in order to compile Python without
 * getting missing externals, but to actually handle SIGFPE requires
 * defining a handler and enabling generation of SIGFPE.
 */

#ifdef WANT_SIGFPE_HANDLER
jmp_buf PyFPE_jbuf;
int PyFPE_counter = 0;
#endif

/* Have this outside the above #ifdef, since some picky ANSI compilers issue a
   warning when compiling an empty file. */

double
PyFPE_dummy(void *dummy)
{
	return 1.0;
}
