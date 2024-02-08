/* signames.c -- Create an array of signal names. */

/* Copyright (C) 2006-2021 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#include <stdio.h>

#include <sys/types.h>
#include <signal.h>

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#if !defined (NSIG)
#  define NSIG 64
#endif

/*
 * Special traps:
 *	EXIT == 0
 *	DEBUG == NSIG
 *	ERR == NSIG+1
 *	RETURN == NSIG+2
 */
#define LASTSIG NSIG+2

char *signal_names[2 * (LASTSIG)];

#define signal_names_size (sizeof(signal_names)/sizeof(signal_names[0]))

/* AIX 4.3 defines SIGRTMIN and SIGRTMAX as 888 and 999 respectively.
   I don't want to allocate so much unused space for the intervening signal
   numbers, so we just punt if SIGRTMAX is past the bounds of the
   signal_names array (handled in configure). */
#if defined (SIGRTMAX) && defined (UNUSABLE_RT_SIGNALS)
#  undef SIGRTMAX
#  undef SIGRTMIN
#endif

#if defined (SIGRTMAX) || defined (SIGRTMIN)
#  define RTLEN 14
#  define RTLIM 256
#endif

#if defined (BUILDTOOL)
extern char *progname;
#endif

void
initialize_signames ()
{
  register int i;
#if defined (SIGRTMAX) || defined (SIGRTMIN)
  int rtmin, rtmax, rtcnt;
#endif

  for (i = 1; i < signal_names_size; i++)
    signal_names[i] = (char *)NULL;

  /* `signal' 0 is what we do on exit. */
  signal_names[0] = "EXIT";

  /* Place signal names which can be aliases for more common signal
     names first.  This allows (for example) SIGABRT to overwrite SIGLOST. */

  /* POSIX 1003.1b-1993 real time signals, but take care of incomplete
     implementations. According to the standard, both SIGRTMIN and
     SIGRTMAX must be defined, SIGRTMIN must be strictly less than
     SIGRTMAX, and the difference must be at least 7; that is, there
     must be at least eight distinct real time signals. */

  /* The generated signal names are SIGRTMIN, SIGRTMIN+1, ...,
     SIGRTMIN+x, SIGRTMAX-x, ..., SIGRTMAX-1, SIGRTMAX. If the number
     of RT signals is odd, there is an extra SIGRTMIN+(x+1).
     These names are the ones used by ksh and /usr/xpg4/bin/sh on SunOS5. */

#if defined (SIGRTMIN)
  rtmin = SIGRTMIN;
  signal_names[rtmin] = "SIGRTMIN";
#endif

#if defined (SIGRTMAX)
  rtmax = SIGRTMAX;
  signal_names[rtmax] = "SIGRTMAX";
#endif

#if defined (SIGRTMAX) && defined (SIGRTMIN)
  if (rtmax > rtmin)
    {
      rtcnt = (rtmax - rtmin - 1) / 2;
      /* croak if there are too many RT signals */
      if (rtcnt >= RTLIM/2)
	{
	  rtcnt = RTLIM/2-1;
#ifdef BUILDTOOL
	  fprintf(stderr, "%s: error: more than %d real time signals, fix `%s'\n",
		  progname, RTLIM, progname);
#endif
	}

      for (i = 1; i <= rtcnt; i++)
	{
	  signal_names[rtmin+i] = (char *)malloc(RTLEN);
	  if (signal_names[rtmin+i])
	    sprintf (signal_names[rtmin+i], "SIGRTMIN+%d", i);
	  signal_names[rtmax-i] = (char *)malloc(RTLEN);
	  if (signal_names[rtmax-i])
	    sprintf (signal_names[rtmax-i], "SIGRTMAX-%d", i);
	}

      if (rtcnt < RTLIM/2-1 && rtcnt != (rtmax-rtmin)/2)
	{
	  /* Need an extra RTMIN signal */
	  signal_names[rtmin+rtcnt+1] = (char *)malloc(RTLEN);
	  if (signal_names[rtmin+rtcnt+1])
	    sprintf (signal_names[rtmin+rtcnt+1], "SIGRTMIN+%d", rtcnt+1);
	}
    }
#endif /* SIGRTMIN && SIGRTMAX */

#if defined (SIGLOST)	/* resource lost (eg, record-lock lost) */
  signal_names[SIGLOST] = "SIGLOST";
#endif

/* AIX */
#if defined (SIGMSG)	/* HFT input data pending */
  signal_names[SIGMSG] = "SIGMSG";
#endif

#if defined (SIGDANGER)	/* system crash imminent */
  signal_names[SIGDANGER] = "SIGDANGER";
#endif

#if defined (SIGMIGRATE) /* migrate process to another CPU */
  signal_names[SIGMIGRATE] = "SIGMIGRATE";
#endif

#if defined (SIGPRE)	/* programming error */
  signal_names[SIGPRE] = "SIGPRE";
#endif

#if defined (SIGPHONE)	/* Phone interrupt */
  signal_names[SIGPHONE] = "SIGPHONE";
#endif

#if defined (SIGVIRT)	/* AIX virtual time alarm */
  signal_names[SIGVIRT] = "SIGVIRT";
#endif

#if defined (SIGTINT)	/* Interrupt */
  signal_names[SIGTINT] = "SIGTINT";
#endif

#if defined (SIGALRM1)	/* m:n condition variables */
  signal_names[SIGALRM1] = "SIGALRM1";
#endif

#if defined (SIGWAITING)	/* m:n scheduling */
  signal_names[SIGWAITING] = "SIGWAITING";
#endif

#if defined (SIGGRANT)	/* HFT monitor mode granted */
  signal_names[SIGGRANT] = "SIGGRANT";
#endif

#if defined (SIGKAP)	/* keep alive poll from native keyboard */
  signal_names[SIGKAP] = "SIGKAP";
#endif

#if defined (SIGRETRACT) /* HFT monitor mode retracted */
  signal_names[SIGRETRACT] = "SIGRETRACT";
#endif

#if defined (SIGSOUND)	/* HFT sound sequence has completed */
  signal_names[SIGSOUND] = "SIGSOUND";
#endif

#if defined (SIGSAK)	/* Secure Attention Key */
  signal_names[SIGSAK] = "SIGSAK";
#endif

#if defined (SIGCPUFAIL)	/* Predictive processor deconfiguration */
  signal_names[SIGCPUFAIL] = "SIGCPUFAIL";
#endif

#if defined (SIGAIO)	/* Asynchronous I/O */
  signal_names[SIGAIO] = "SIGAIO";
#endif

#if defined (SIGLAB)	/* Security label changed */
  signal_names[SIGLAB] = "SIGLAB";
#endif

/* SunOS5 */
#if defined (SIGLWP)	/* Solaris: special signal used by thread library */
  signal_names[SIGLWP] = "SIGLWP";
#endif

#if defined (SIGFREEZE)	/* Solaris: special signal used by CPR */
  signal_names[SIGFREEZE] = "SIGFREEZE";
#endif

#if defined (SIGTHAW)	/* Solaris: special signal used by CPR */
  signal_names[SIGTHAW] = "SIGTHAW";
#endif

#if defined (SIGCANCEL)	/* Solaris: thread cancellation signal used by libthread */
  signal_names[SIGCANCEL] = "SIGCANCEL";
#endif

#if defined (SIGXRES)	/* Solaris: resource control exceeded */
  signal_names[SIGXRES] = "SIGXRES";
#endif

#if defined (SIGJVM1)	/* Solaris: Java Virtual Machine 1 */
  signal_names[SIGJVM1] = "SIGJVM1";
#endif

#if defined (SIGJVM2)	/* Solaris: Java Virtual Machine 2 */
  signal_names[SIGJVM2] = "SIGJVM2";
#endif

#if defined (SIGDGTIMER1)
  signal_names[SIGDGTIMER1] = "SIGDGTIMER1";
#endif

#if defined (SIGDGTIMER2)
  signal_names[SIGDGTIMER2] = "SIGDGTIMER2";
#endif

#if defined (SIGDGTIMER3)
  signal_names[SIGDGTIMER3] = "SIGDGTIMER3";
#endif

#if defined (SIGDGTIMER4)
  signal_names[SIGDGTIMER4] = "SIGDGTIMER4";
#endif

#if defined (SIGDGNOTIFY)
  signal_names[SIGDGNOTIFY] = "SIGDGNOTIFY";
#endif

/* Apollo */
#if defined (SIGAPOLLO)
  signal_names[SIGAPOLLO] = "SIGAPOLLO";
#endif

/* HP-UX */
#if defined (SIGDIL)	/* DIL signal (?) */
  signal_names[SIGDIL] = "SIGDIL";
#endif

/* System V */
#if defined (SIGCLD)	/* Like SIGCHLD.  */
  signal_names[SIGCLD] = "SIGCLD";
#endif

#if defined (SIGPWR)	/* power state indication */
  signal_names[SIGPWR] = "SIGPWR";
#endif

#if defined (SIGPOLL)	/* Pollable event (for streams)  */
  signal_names[SIGPOLL] = "SIGPOLL";
#endif

/* Unknown */
#if defined (SIGWINDOW)
  signal_names[SIGWINDOW] = "SIGWINDOW";
#endif

/* Linux */
#if defined (SIGSTKFLT)
  signal_names[SIGSTKFLT] = "SIGSTKFLT";
#endif

/* FreeBSD */
#if defined (SIGTHR)	/* thread interrupt */
  signal_names[SIGTHR] = "SIGTHR";
#endif

/* Common */
#if defined (SIGHUP)	/* hangup */
  signal_names[SIGHUP] = "SIGHUP";
#endif

#if defined (SIGINT)	/* interrupt */
  signal_names[SIGINT] = "SIGINT";
#endif

#if defined (SIGQUIT)	/* quit */
  signal_names[SIGQUIT] = "SIGQUIT";
#endif

#if defined (SIGILL)	/* illegal instruction (not reset when caught) */
  signal_names[SIGILL] = "SIGILL";
#endif

#if defined (SIGTRAP)	/* trace trap (not reset when caught) */
  signal_names[SIGTRAP] = "SIGTRAP";
#endif

#if defined (SIGIOT)	/* IOT instruction */
  signal_names[SIGIOT] = "SIGIOT";
#endif

#if defined (SIGABRT)	/* Cause current process to dump core. */
  signal_names[SIGABRT] = "SIGABRT";
#endif

#if defined (SIGEMT)	/* EMT instruction */
  signal_names[SIGEMT] = "SIGEMT";
#endif

#if defined (SIGFPE)	/* floating point exception */
  signal_names[SIGFPE] = "SIGFPE";
#endif

#if defined (SIGKILL)	/* kill (cannot be caught or ignored) */
  signal_names[SIGKILL] = "SIGKILL";
#endif

#if defined (SIGBUS)	/* bus error */
  signal_names[SIGBUS] = "SIGBUS";
#endif

#if defined (SIGSEGV)	/* segmentation violation */
  signal_names[SIGSEGV] = "SIGSEGV";
#endif

#if defined (SIGSYS)	/* bad argument to system call */
  signal_names[SIGSYS] = "SIGSYS";
#endif

#if defined (SIGPIPE)	/* write on a pipe with no one to read it */
  signal_names[SIGPIPE] = "SIGPIPE";
#endif

#if defined (SIGALRM)	/* alarm clock */
  signal_names[SIGALRM] = "SIGALRM";
#endif

#if defined (SIGTERM)	/* software termination signal from kill */
  signal_names[SIGTERM] = "SIGTERM";
#endif

#if defined (SIGURG)	/* urgent condition on IO channel */
  signal_names[SIGURG] = "SIGURG";
#endif

#if defined (SIGSTOP)	/* sendable stop signal not from tty */
  signal_names[SIGSTOP] = "SIGSTOP";
#endif

#if defined (SIGTSTP)	/* stop signal from tty */
  signal_names[SIGTSTP] = "SIGTSTP";
#endif

#if defined (SIGCONT)	/* continue a stopped process */
  signal_names[SIGCONT] = "SIGCONT";
#endif

#if defined (SIGCHLD)	/* to parent on child stop or exit */
  signal_names[SIGCHLD] = "SIGCHLD";
#endif

#if defined (SIGTTIN)	/* to readers pgrp upon background tty read */
  signal_names[SIGTTIN] = "SIGTTIN";
#endif

#if defined (SIGTTOU)	/* like TTIN for output if (tp->t_local&LTOSTOP) */
  signal_names[SIGTTOU] = "SIGTTOU";
#endif

#if defined (SIGIO)	/* input/output possible signal */
  signal_names[SIGIO] = "SIGIO";
#endif

#if defined (SIGXCPU)	/* exceeded CPU time limit */
  signal_names[SIGXCPU] = "SIGXCPU";
#endif

#if defined (SIGXFSZ)	/* exceeded file size limit */
  signal_names[SIGXFSZ] = "SIGXFSZ";
#endif

#if defined (SIGVTALRM)	/* virtual time alarm */
  signal_names[SIGVTALRM] = "SIGVTALRM";
#endif

#if defined (SIGPROF)	/* profiling time alarm */
  signal_names[SIGPROF] = "SIGPROF";
#endif

#if defined (SIGWINCH)	/* window changed */
  signal_names[SIGWINCH] = "SIGWINCH";
#endif

/* 4.4 BSD */
#if defined (SIGINFO) && !defined (_SEQUENT_)	/* information request */
  signal_names[SIGINFO] = "SIGINFO";
#endif

#if defined (SIGUSR1)	/* user defined signal 1 */
  signal_names[SIGUSR1] = "SIGUSR1";
#endif

#if defined (SIGUSR2)	/* user defined signal 2 */
  signal_names[SIGUSR2] = "SIGUSR2";
#endif

#if defined (SIGKILLTHR)	/* BeOS: Kill Thread */
  signal_names[SIGKILLTHR] = "SIGKILLTHR";
#endif

  for (i = 0; i < NSIG; i++)
    if (signal_names[i] == (char *)NULL)
      {
	signal_names[i] = (char *)malloc (18);
	if (signal_names[i])
	  sprintf (signal_names[i], "SIGJUNK(%d)", i);
      }

  signal_names[NSIG] = "DEBUG";
  signal_names[NSIG+1] = "ERR";
  signal_names[NSIG+2] = "RETURN";
}
