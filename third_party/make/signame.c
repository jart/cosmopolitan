/* Convert between signal names and numbers.
Copyright (C) 1990-2023 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "makeint.h"

/* If the system provides strsignal, we don't need it. */

#if !HAVE_STRSIGNAL

/* If the system provides sys_siglist, we'll use that.
   Otherwise create our own.
 */

#if !HAVE_DECL_SYS_SIGLIST

/* Some systems do not define NSIG in <signal.h>.  */
#ifndef NSIG
#ifdef  _NSIG
#define NSIG    _NSIG
#else
#define NSIG    32
#endif
#endif

/* There is too much variation in Sys V signal numbers and names, so
   we must initialize them at runtime.  */

static const char *undoc;

static const char *sys_siglist[NSIG];

/* Table of abbreviations for signals.  Note:  A given number can
   appear more than once with different abbreviations.  */
#define SIG_TABLE_SIZE  (NSIG*2)

typedef struct
  {
    int number;
    const char *abbrev;
  } num_abbrev;

static num_abbrev sig_table[SIG_TABLE_SIZE];

/* Number of elements of sig_table used.  */
static int sig_table_nelts = 0;

/* Enter signal number NUMBER into the tables with ABBREV and NAME.  */

static void
init_sig (int number, const char *abbrev, const char *name)
{
  /* If this value is ever greater than NSIG it seems like it'd be a bug in
     the system headers, but... better safe than sorry.  We know, for
     example, that this isn't always true on VMS.  */

  if (number >= 0 && number < NSIG)
    sys_siglist[number] = name;

  if (sig_table_nelts < SIG_TABLE_SIZE)
    {
      sig_table[sig_table_nelts].number = number;
      sig_table[sig_table_nelts++].abbrev = abbrev;
    }
}

static int
signame_init (void)
{
  int i;

  undoc = xstrdup (_("unknown signal"));

  /* Initialize signal names.  */
  for (i = 0; i < NSIG; i++)
    sys_siglist[i] = undoc;

  /* Initialize signal names.  */
#if defined (SIGHUP)
  init_sig (SIGHUP, "HUP", _("Hangup"));
#endif
#if defined (SIGINT)
  init_sig (SIGINT, "INT", _("Interrupt"));
#endif
#if defined (SIGQUIT)
  init_sig (SIGQUIT, "QUIT", _("Quit"));
#endif
#if defined (SIGILL)
  init_sig (SIGILL, "ILL", _("Illegal Instruction"));
#endif
#if defined (SIGTRAP)
  init_sig (SIGTRAP, "TRAP", _("Trace/breakpoint trap"));
#endif
  /* If SIGIOT == SIGABRT, we want to print it as SIGABRT because
     SIGABRT is in ANSI and POSIX.1 and SIGIOT isn't.  */
#if defined (SIGABRT)
  init_sig (SIGABRT, "ABRT", _("Aborted"));
#endif
#if defined (SIGIOT)
  init_sig (SIGIOT, "IOT", _("IOT trap"));
#endif
#if defined (SIGEMT)
  init_sig (SIGEMT, "EMT", _("EMT trap"));
#endif
#if defined (SIGFPE)
  init_sig (SIGFPE, "FPE", _("Floating point exception"));
#endif
#if defined (SIGKILL)
  init_sig (SIGKILL, "KILL", _("Killed"));
#endif
#if defined (SIGBUS)
  init_sig (SIGBUS, "BUS", _("Bus error"));
#endif
#if defined (SIGSEGV)
  init_sig (SIGSEGV, "SEGV", _("Segmentation fault"));
#endif
#if defined (SIGSYS)
  init_sig (SIGSYS, "SYS", _("Bad system call"));
#endif
#if defined (SIGPIPE)
  init_sig (SIGPIPE, "PIPE", _("Broken pipe"));
#endif
#if defined (SIGALRM)
  init_sig (SIGALRM, "ALRM", _("Alarm clock"));
#endif
#if defined (SIGTERM)
  init_sig (SIGTERM, "TERM", _("Terminated"));
#endif
#if defined (SIGUSR1)
  init_sig (SIGUSR1, "USR1", _("User defined signal 1"));
#endif
#if defined (SIGUSR2)
  init_sig (SIGUSR2, "USR2", _("User defined signal 2"));
#endif
  /* If SIGCLD == SIGCHLD, we want to print it as SIGCHLD because that
     is what is in POSIX.1.  */
#if defined (SIGCHLD)
  init_sig (SIGCHLD, "CHLD", _("Child exited"));
#endif
#if defined (SIGCLD)
  init_sig (SIGCLD, "CLD", _("Child exited"));
#endif
#if defined (SIGPWR)
  init_sig (SIGPWR, "PWR", _("Power failure"));
#endif
#if defined (SIGTSTP)
  init_sig (SIGTSTP, "TSTP", _("Stopped"));
#endif
#if defined (SIGTTIN)
  init_sig (SIGTTIN, "TTIN", _("Stopped (tty input)"));
#endif
#if defined (SIGTTOU)
  init_sig (SIGTTOU, "TTOU", _("Stopped (tty output)"));
#endif
#if defined (SIGSTOP)
  init_sig (SIGSTOP, "STOP", _("Stopped (signal)"));
#endif
#if defined (SIGXCPU)
  init_sig (SIGXCPU, "XCPU", _("CPU time limit exceeded"));
#endif
#if defined (SIGXFSZ)
  init_sig (SIGXFSZ, "XFSZ", _("File size limit exceeded"));
#endif
#if defined (SIGVTALRM)
  init_sig (SIGVTALRM, "VTALRM", _("Virtual timer expired"));
#endif
#if defined (SIGPROF)
  init_sig (SIGPROF, "PROF", _("Profiling timer expired"));
#endif
#if defined (SIGWINCH)
  /* "Window size changed" might be more accurate, but even if that
     is all that it means now, perhaps in the future it will be
     extended to cover other kinds of window changes.  */
  init_sig (SIGWINCH, "WINCH", _("Window changed"));
#endif
#if defined (SIGCONT)
  init_sig (SIGCONT, "CONT", _("Continued"));
#endif
#if defined (SIGURG)
  init_sig (SIGURG, "URG", _("Urgent I/O condition"));
#endif
#if defined (SIGIO)
  /* "I/O pending" has also been suggested.  A disadvantage is that signal
     only happens when the process has asked for it, not every time I/O is
     pending.  Another disadvantage is the confusion from giving it a
     different name than under Unix.  */
  init_sig (SIGIO, "IO", _("I/O possible"));
#endif
#if defined (SIGWIND)
  init_sig (SIGWIND, "WIND", _("SIGWIND"));
#endif
#if defined (SIGPHONE)
  init_sig (SIGPHONE, "PHONE", _("SIGPHONE"));
#endif
#if defined (SIGPOLL)
  init_sig (SIGPOLL, "POLL", _("I/O possible"));
#endif
#if defined (SIGLOST)
  init_sig (SIGLOST, "LOST", _("Resource lost"));
#endif
#if defined (SIGDANGER)
  init_sig (SIGDANGER, "DANGER", _("Danger signal"));
#endif
#if defined (SIGINFO)
  init_sig (SIGINFO, "INFO", _("Information request"));
#endif
#if defined (SIGNOFP)
  init_sig (SIGNOFP, "NOFP", _("Floating point co-processor not available"));
#endif

  return 1;
}

#endif  /* HAVE_DECL_SYS_SIGLIST */


char *
strsignal (int sig)
{
  static char buf[] = "Signal 12345678901234567890";

#if ! HAVE_DECL_SYS_SIGLIST
# if HAVE_DECL__SYS_SIGLIST
#  define sys_siglist _sys_siglist
# elif HAVE_DECL___SYS_SIGLIST
#  define sys_siglist __sys_siglist
# else
  static int sig_initted = 0;

  if (!sig_initted)
    sig_initted = signame_init ();
# endif
#endif

  if (sig > 0 && sig < NSIG)
    return (char *) sys_siglist[sig];

  sprintf (buf, "Signal %d", sig);
  return buf;
}

#endif  /* HAVE_STRSIGNAL */
