/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1997-2005
 *	Herbert Xu <herbert@gondor.apana.org.au>.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Errors and exceptions.
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "jobs.h"
#include "shell.h"
#include "main.h"
#include "options.h"
#include "output.h"
#include "error.h"
#include "show.h"
#include "eval.h"
#include "parser.h"
#include "system.h"


/*
 * Code to handle exceptions in C.
 */

struct jmploc *handler;
int exception;
int suppressint;
volatile sig_atomic_t intpending;
int errlinno;


static void exverror(int, const char *, va_list)
    __attribute__((__noreturn__));

/*
 * Called to raise an exception.  Since C doesn't include exceptions, we
 * just do a longjmp to the exception handler.  The type of exception is
 * stored in the global variable "exception".
 */

void
exraise(int e)
{
#ifdef DEBUG
	if (handler == NULL)
		abort();
#endif

	if (vforked)
		_exit(exitstatus);

	INTOFF;

	exception = e;
	longjmp(handler->loc, 1);
}


/*
 * Called from trap.c when a SIGINT is received.  (If the user specifies
 * that SIGINT is to be trapped or ignored using the trap builtin, then
 * this routine is not called.)  Suppressint is nonzero when interrupts
 * are held using the INTOFF macro.  (The test for iflag is just
 * defensive programming.)
 */

void
onint(void) {

	intpending = 0;
	sigclearmask();
	if (!(rootshell && iflag)) {
		signal(SIGINT, SIG_DFL);
		raise(SIGINT);
	}
	exitstatus = SIGINT + 128;
	exraise(EXINT);
	/* NOTREACHED */
}

static void
exvwarning2(const char *msg, va_list ap)
{
	struct output *errs;
	const char *name;
	const char *fmt;

	errs = out2;
	name = arg0 ? arg0 : "sh";
	if (!commandname)
		fmt = "%s: %d: ";
	else
		fmt = "%s: %d: %s: ";
	outfmt(errs, fmt, name, errlinno, commandname);
	doformat(errs, msg, ap);
#if FLUSHERR
	outc('\n', errs);
#else
	outcslow('\n', errs);
#endif
}

#define exvwarning(a, b, c) exvwarning2(b, c)

/*
 * Exverror is called to raise the error exception.  If the second argument
 * is not NULL then error prints an error message using printf style
 * formatting.  It then raises the error exception.
 */
static void
exverror(int cond, const char *msg, va_list ap)
{
#ifdef DEBUG
	if (msg) {
		va_list aq;
		TRACE(("exverror(%d, \"", cond));
		va_copy(aq, ap);
		TRACEV((msg, aq));
		va_end(aq);
		TRACE(("\") pid=%d\n", getpid()));
	} else
		TRACE(("exverror(%d, NULL) pid=%d\n", cond, getpid()));
	if (msg)
#endif
		exvwarning(-1, msg, ap);

	flushall();
	exraise(cond);
	/* NOTREACHED */
}


void
sh_error(const char *msg, ...)
{
	va_list ap;

	exitstatus = 2;

	va_start(ap, msg);
	exverror(EXERROR, msg, ap);
	/* NOTREACHED */
	va_end(ap);
}


void
exerror(int cond, const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	exverror(cond, msg, ap);
	/* NOTREACHED */
	va_end(ap);
}

/*
 * error/warning routines for external builtins
 */

void
sh_warnx(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	exvwarning(-1, fmt, ap);
	va_end(ap);
}


/*
 * Return a string describing an error.  The returned string may be a
 * pointer to a static buffer that will be overwritten on the next call.
 * Action describes the operation that got the error.
 */

const char *
errmsg(int e, int action)
{
	if (e != ENOENT && e != ENOTDIR)
		return strerror(e);

	if (action & E_OPEN)
		return "No such file";
	else if (action & E_CREAT)
		return "Directory nonexistent";
	else
		return "not found";
}


#ifdef REALLY_SMALL
void
__inton() {
	if (--suppressint == 0 && intpending) {
		onint();
	}
}
#endif
