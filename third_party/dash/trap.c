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

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "shell.h"
#include "main.h"
#include "nodes.h"	/* for other headers */
#include "eval.h"
#include "init.h"
#include "jobs.h"
#include "show.h"
#include "options.h"
#include "syntax.h"
#include "output.h"
#include "parser.h"
#include "memalloc.h"
#include "error.h"
#include "trap.h"
#include "mystring.h"

/*
 * Sigmode records the current value of the signal handlers for the various
 * modes.  A value of zero means that the current handler is not known.
 * S_HARD_IGN indicates that the signal was ignored on entry to the shell,
 */

#define S_DFL 1			/* default signal handling (SIG_DFL) */
#define S_CATCH 2		/* signal is caught */
#define S_IGN 3			/* signal is ignored (SIG_IGN) */
#define S_HARD_IGN 4		/* signal is ignored permenantly */
#define S_RESET 5		/* temporary - to reset a hard ignored sig */


/* trap handler commands */
static char *trap[NSIG];
/* traps have not been fully cleared */
static int ptrap;
/* number of non-null traps */
int trapcnt;
/* current value of signal */
char sigmode[NSIG - 1];
/* indicates specified signal received */
static char gotsig[NSIG - 1];
/* last pending signal */
volatile sig_atomic_t pending_sig;
/* received SIGCHLD */
volatile sig_atomic_t gotsigchld;

extern char *signal_names[];

static int decode_signum(const char *);
MKINIT void clear_traps(union node *);

#ifdef mkinit
INCLUDE "memalloc.h"
INCLUDE "trap.h"

INIT {
	sigmode[SIGCHLD - 1] = S_DFL;
	setsignal(SIGCHLD);
}

FORKRESET {
	clear_traps(n);
}
#endif

/*
 * The trap builtin.
 */

int
trapcmd(int argc, char **argv)
{
	char *action;
	char **ap;
	int signo;

	nextopt(nullstr);
	ap = argptr;
	if (!*ap) {
		for (signo = 0 ; signo < NSIG ; signo++) {
			if (trap[signo] != NULL) {
				out1fmt(
					"trap -- %s %s\n",
					single_quote(trap[signo]),
					signal_names[signo]
				);
			}
		}
		return 0;
	}
	if (ptrap)
		clear_traps(NULL);
	if (!ap[1] || decode_signum(*ap) >= 0)
		action = NULL;
	else
		action = *ap++;
	while (*ap) {
		if ((signo = decode_signal(*ap, 0)) < 0) {
			outfmt(out2, "trap: %s: bad trap\n", *ap);
			return 1;
		}
		INTOFF;
		if (action) {
			if (action[0] == '-' && action[1] == '\0')
				action = NULL;
			else {
				if (*action)
					trapcnt++;
				action = savestr(action);
			}
		}
		if (trap[signo]) {
			if (*trap[signo])
				trapcnt--;
			ckfree(trap[signo]);
		}
		trap[signo] = action;
		if (signo != 0)
			setsignal(signo);
		INTON;
		ap++;
	}
	return 0;
}



/*
 * Clear traps on a fork.
 */

void clear_traps(union node *n)
{
	int simplecmd;
	char **tp;

	simplecmd = issimplecmd(n, TRAPCMD->name);

	INTOFF;
	for (tp = trap ; tp < &trap[NSIG] ; tp++) {
		if (*tp && **tp) {	/* trap not NULL or SIG_IGN */
			char *otp = *tp;

			*tp = NULL;
			if (tp != &trap[0])
				setsignal(tp - trap);

			if (simplecmd)
				*tp = otp;
			else
				ckfree(*tp);
		}
	}
	trapcnt = 0;
	ptrap = simplecmd;
	INTON;
}



/*
 * Set the signal handler for the specified signal.  The routine figures
 * out what it should be set to.
 */

void
setsignal(int signo)
{
	int action;
	int lvforked;
	char *t, tsig;
	struct sigaction act;

	lvforked = vforked;

	if ((t = trap[signo]) == NULL)
		action = S_DFL;
	else if (*t != '\0')
		action = S_CATCH;
	else
		action = S_IGN;
	if (rootshell && action == S_DFL && !lvforked) {
		switch (signo) {
		case SIGINT:
			if (iflag || minusc || sflag == 0)
				action = S_CATCH;
			break;
		case SIGQUIT:
#ifdef DEBUG
			if (debug)
				break;
#endif
			/* FALLTHROUGH */
		case SIGTERM:
			if (iflag)
				action = S_IGN;
			break;
#if JOBS
		case SIGTSTP:
		case SIGTTOU:
			if (mflag)
				action = S_IGN;
			break;
#endif
		}
	}

	if (signo == SIGCHLD)
		action = S_CATCH;

	t = &sigmode[signo - 1];
	tsig = *t;
	if (tsig == 0) {
		/*
		 * current setting unknown
		 */
		if (sigaction(signo, 0, &act) == -1) {
			/*
			 * Pretend it worked; maybe we should give a warning
			 * here, but other shells don't. We don't alter
			 * sigmode, so that we retry every time.
			 */
			return;
		}
		if (act.sa_handler == SIG_IGN) {
			if (mflag && (signo == SIGTSTP ||
			     signo == SIGTTIN || signo == SIGTTOU)) {
				tsig = S_IGN;	/* don't hard ignore these */
			} else
				tsig = S_HARD_IGN;
		} else {
			tsig = S_RESET;	/* force to be set */
		}
	}
	if (tsig == S_HARD_IGN || tsig == action)
		return;
	switch (action) {
	case S_CATCH:
		act.sa_handler = onsig;
		break;
	case S_IGN:
		act.sa_handler = SIG_IGN;
		break;
	default:
		act.sa_handler = SIG_DFL;
	}
	if (!lvforked)
		*t = action;
	act.sa_flags = 0;
	sigfillset(&act.sa_mask);
	sigaction(signo, &act, 0);
}

/*
 * Ignore a signal.
 */

void
ignoresig(int signo)
{
	if (sigmode[signo - 1] == S_IGN || sigmode[signo - 1] == S_HARD_IGN)
		return;
	signal(signo, SIG_IGN);
	if (!vforked)
		sigmode[signo - 1] = S_IGN;
}



/*
 * Signal handler.
 */

void
onsig(int signo)
{
	if (vforked)
		return;

	if (signo == SIGCHLD) {
		gotsigchld = 1;
		if (!trap[SIGCHLD])
			return;
	}

	gotsig[signo - 1] = 1;
	pending_sig = signo;

	if (signo == SIGINT && !trap[SIGINT]) {
		if (!suppressint)
			onint();
		intpending = 1;
	}
}



/*
 * Called to execute a trap.  Perhaps we should avoid entering new trap
 * handlers while we are executing a trap handler.
 */

void dotrap(void)
{
	char *p;
	char *q;
	int i;
	int status, last_status;

	if (!pending_sig)
		return;

	status = savestatus;
	last_status = status;
	if (likely(status < 0)) {
		status = exitstatus;
		savestatus = status;
	}
	pending_sig = 0;
	barrier();

	for (i = 0, q = gotsig; i < NSIG - 1; i++, q++) {
		if (!*q)
			continue;

		if (evalskip) {
			pending_sig = i + 1;
			break;
		}

		*q = 0;

		p = trap[i + 1];
		if (!p)
			continue;
		evalstring(p, 0);
		if (evalskip != SKIPFUNC)
			exitstatus = status;
	}

	savestatus = last_status;
}



/*
 * Controls whether the shell is interactive or not.
 */


void
setinteractive(int on)
{
	static int is_interactive;

	if (++on == is_interactive)
		return;
	is_interactive = on;
	setsignal(SIGINT);
	setsignal(SIGQUIT);
	setsignal(SIGTERM);
}



/*
 * Called to exit the shell.
 */

void
exitshell(void)
{
	struct jmploc loc;
	char *p;

	savestatus = exitstatus;
	TRACE(("pid %d, exitshell(%d)\n", getpid(), savestatus));
	if (setjmp(loc.loc))
		goto out;
	handler = &loc;
	if ((p = trap[0])) {
		trap[0] = NULL;
		if (ptrap)
			goto out;
		evalskip = 0;
		evalstring(p, 0);
		evalskip = SKIPFUNCDEF;
	}
out:
	exitreset();
	postexitreset();
	/*
	 * Disable job control so that whoever had the foreground before we
	 * started can get it back.
	 */
	if (likely(!setjmp(loc.loc)))
		setjobctl(0);
	flushall();
	_exit(exitstatus);
	/* NOTREACHED */
}

static int decode_signum(const char *string)
{
	int signo = -1;

	if (is_number(string)) {
		signo = atoi(string);
		if (signo >= NSIG)
			signo = -1;
	}

	return signo;
}

int decode_signal(const char *string, int minsig)
{
	int signo;

	signo = decode_signum(string);
	if (signo >= 0)
		return signo;

	for (signo = minsig; signo < NSIG; signo++) {
		if (!strcasecmp(string, signal_names[signo])) {
			return signo;
		}
	}

	return -1;
}

void sigblockall(sigset_t *oldmask)
{
	sigset_t mask;

	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask, oldmask);
}
