/* sig.c - interface for shell signal handlers and signal initialization. */

/* Copyright (C) 1994-2021 Free Software Foundation, Inc.

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

#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>
#include <signal.h>

#include "bashintl.h"

#include "shell.h"
#include "execute_cmd.h"
#if defined (JOB_CONTROL)
#include "jobs.h"
#endif /* JOB_CONTROL */
#include "siglist.h"
#include "sig.h"
#include "trap.h"

#include "common.h"
#include "builtext.h"

#if defined (READLINE)
#  include "bashline.h"
#  include "third_party/readline/readline.h"
#endif

#if defined (HISTORY)
#  include "bashhist.h"
#endif

extern void initialize_siglist PARAMS((void));
extern void set_original_signal PARAMS((int, SigHandler *));

#if !defined (JOB_CONTROL)
extern void initialize_job_signals PARAMS((void));
#endif

/* Non-zero after SIGINT. */
volatile sig_atomic_t interrupt_state = 0;

/* Non-zero after SIGWINCH */
volatile sig_atomic_t sigwinch_received = 0;

/* Non-zero after SIGTERM */
volatile sig_atomic_t sigterm_received = 0;

/* Set to the value of any terminating signal received. */
volatile sig_atomic_t terminating_signal = 0;

/* The environment at the top-level R-E loop.  We use this in
   the case of error return. */
procenv_t top_level;

#if defined (JOB_CONTROL) || defined (HAVE_POSIX_SIGNALS)
/* The signal masks that this shell runs with. */
sigset_t top_level_mask;
#endif /* JOB_CONTROL */

/* When non-zero, we throw_to_top_level (). */
int interrupt_immediately = 0;

/* When non-zero, we call the terminating signal handler immediately. */
int terminate_immediately = 0;

#if defined (SIGWINCH)
static SigHandler *old_winch = (SigHandler *)SIG_DFL;
#endif

static void initialize_shell_signals PARAMS((void));

void
initialize_signals (reinit)
     int reinit;
{
  initialize_shell_signals ();
  initialize_job_signals ();
#if !defined (HAVE_SYS_SIGLIST) && !defined (HAVE_UNDER_SYS_SIGLIST) && !defined (HAVE_STRSIGNAL)
  if (reinit == 0)
    initialize_siglist ();
#endif /* !HAVE_SYS_SIGLIST && !HAVE_UNDER_SYS_SIGLIST && !HAVE_STRSIGNAL */
}

/* A structure describing a signal that terminates the shell if not
   caught.  The orig_handler member is present so children can reset
   these signals back to their original handlers. */
struct termsig {
     int signum;
     SigHandler *orig_handler;
     int orig_flags;
     int core_dump;
};

#define NULL_HANDLER (SigHandler *)SIG_DFL

/* The list of signals that would terminate the shell if not caught.
   We catch them, but just so that we can write the history file,
   and so forth. */
static struct termsig terminating_signals[] = {
#ifdef SIGHUP
{  SIGHUP, NULL_HANDLER, 0 },
#endif

#ifdef SIGINT
{  SIGINT, NULL_HANDLER, 0 },
#endif

#ifdef SIGILL
{  SIGILL, NULL_HANDLER, 0, 1},
#endif

#ifdef SIGTRAP
{  SIGTRAP, NULL_HANDLER, 0, 1 },
#endif

#ifdef SIGIOT
{  SIGIOT, NULL_HANDLER, 0, 1 },
#endif

#ifdef SIGDANGER
{  SIGDANGER, NULL_HANDLER, 0 },
#endif

#ifdef SIGEMT
{  SIGEMT, NULL_HANDLER, 0 },
#endif

#ifdef SIGFPE
{  SIGFPE, NULL_HANDLER, 0, 1 },
#endif

#ifdef SIGBUS
{  SIGBUS, NULL_HANDLER, 0, 1 },
#endif

#ifdef SIGSEGV
{  SIGSEGV, NULL_HANDLER, 0, 1 },
#endif

#ifdef SIGSYS
{  SIGSYS, NULL_HANDLER, 0, 1 },
#endif

#ifdef SIGPIPE
{  SIGPIPE, NULL_HANDLER, 0 },
#endif

#ifdef SIGALRM
{  SIGALRM, NULL_HANDLER, 0 },
#endif

#ifdef SIGTERM
{  SIGTERM, NULL_HANDLER, 0 },
#endif

/* These don't generate core dumps on anything but Linux, but we're doing
   this just for Linux anyway. */
#ifdef SIGXCPU
{  SIGXCPU, NULL_HANDLER, 0, 1 },
#endif

#ifdef SIGXFSZ
{  SIGXFSZ, NULL_HANDLER, 0, 1 },
#endif

#ifdef SIGVTALRM
{  SIGVTALRM, NULL_HANDLER, 0 },
#endif

#if 0
#ifdef SIGPROF
{  SIGPROF, NULL_HANDLER, 0 },
#endif
#endif

#ifdef SIGLOST
{  SIGLOST, NULL_HANDLER, 0 },
#endif

#ifdef SIGUSR1
{  SIGUSR1, NULL_HANDLER, 0 },
#endif

#ifdef SIGUSR2
{  SIGUSR2, NULL_HANDLER, 0 },
#endif
};

#define TERMSIGS_LENGTH (sizeof (terminating_signals) / sizeof (struct termsig))

#define XSIG(x) (terminating_signals[x].signum)
#define XHANDLER(x) (terminating_signals[x].orig_handler)
#define XSAFLAGS(x) (terminating_signals[x].orig_flags)
#define XCOREDUMP(x) (terminating_signals[x].core_dump)

static int termsigs_initialized = 0;

/* Initialize signals that will terminate the shell to do some
   unwind protection.  For non-interactive shells, we only call
   this when a trap is defined for EXIT (0) or when trap is run
   to display signal dispositions. */
void
initialize_terminating_signals ()
{
  register int i;
#if defined (HAVE_POSIX_SIGNALS)
  struct sigaction act, oact;
#endif

  if (termsigs_initialized)
    return;

  /* The following code is to avoid an expensive call to
     set_signal_handler () for each terminating_signals.  Fortunately,
     this is possible in Posix.  Unfortunately, we have to call signal ()
     on non-Posix systems for each signal in terminating_signals. */
#if defined (HAVE_POSIX_SIGNALS)
  act.sa_handler = termsig_sighandler;
  act.sa_flags = 0;
  sigemptyset (&act.sa_mask);
  sigemptyset (&oact.sa_mask);
  for (i = 0; i < TERMSIGS_LENGTH; i++)
    sigaddset (&act.sa_mask, XSIG (i));
  for (i = 0; i < TERMSIGS_LENGTH; i++)
    {
      /* If we've already trapped it, don't do anything. */
      if (signal_is_trapped (XSIG (i)))
	continue;

      sigaction (XSIG (i), &act, &oact);
      XHANDLER(i) = oact.sa_handler;
      XSAFLAGS(i) = oact.sa_flags;

#if 0
      set_original_signal (XSIG(i), XHANDLER(i));	/* optimization */
#else
      set_original_signal (XSIG(i), act.sa_handler);	/* optimization */
#endif

      /* Don't do anything with signals that are ignored at shell entry
	 if the shell is not interactive. */
      /* XXX - should we do this for interactive shells, too? */
      if (interactive_shell == 0 && XHANDLER (i) == SIG_IGN)
	{
	  sigaction (XSIG (i), &oact, &act);
	  set_signal_hard_ignored (XSIG (i));
	}
#if defined (SIGPROF) && !defined (_MINIX)
      if (XSIG (i) == SIGPROF && XHANDLER (i) != SIG_DFL && XHANDLER (i) != SIG_IGN)
	sigaction (XSIG (i), &oact, (struct sigaction *)NULL);
#endif /* SIGPROF && !_MINIX */
    }
#else /* !HAVE_POSIX_SIGNALS */

  for (i = 0; i < TERMSIGS_LENGTH; i++)
    {
      /* If we've already trapped it, don't do anything. */
      if (signal_is_trapped (XSIG (i)))
	continue;

      XHANDLER(i) = signal (XSIG (i), termsig_sighandler);
      XSAFLAGS(i) = 0;
      /* Don't do anything with signals that are ignored at shell entry
	 if the shell is not interactive. */
      /* XXX - should we do this for interactive shells, too? */
      if (interactive_shell == 0 && XHANDLER (i) == SIG_IGN)
	{
	  signal (XSIG (i), SIG_IGN);
	  set_signal_hard_ignored (XSIG (i));
	}
#ifdef SIGPROF
      if (XSIG (i) == SIGPROF && XHANDLER (i) != SIG_DFL && XHANDLER (i) != SIG_IGN)
	signal (XSIG (i), XHANDLER (i));
#endif
    }

#endif /* !HAVE_POSIX_SIGNALS */

  termsigs_initialized = 1;
}

static void
initialize_shell_signals ()
{
  if (interactive)
    initialize_terminating_signals ();

#if defined (JOB_CONTROL) || defined (HAVE_POSIX_SIGNALS)
  /* All shells use the signal mask they inherit, and pass it along
     to child processes.  Children will never block SIGCHLD, though. */
  sigemptyset (&top_level_mask);
  sigprocmask (SIG_BLOCK, (sigset_t *)NULL, &top_level_mask);
#  if defined (SIGCHLD)
  if (sigismember (&top_level_mask, SIGCHLD))
    {
      sigdelset (&top_level_mask, SIGCHLD);
      sigprocmask (SIG_SETMASK, &top_level_mask, (sigset_t *)NULL);
    }
#  endif
#endif /* JOB_CONTROL || HAVE_POSIX_SIGNALS */

  /* And, some signals that are specifically ignored by the shell. */
  set_signal_handler (SIGQUIT, SIG_IGN);

  if (interactive)
    {
      set_signal_handler (SIGINT, sigint_sighandler);
      get_original_signal (SIGTERM);
      set_signal_handler (SIGTERM, SIG_IGN);
      set_sigwinch_handler ();
    }
}

void
reset_terminating_signals ()
{
  register int i;
#if defined (HAVE_POSIX_SIGNALS)
  struct sigaction act;
#endif

  if (termsigs_initialized == 0)
    return;

#if defined (HAVE_POSIX_SIGNALS)
  act.sa_flags = 0;
  sigemptyset (&act.sa_mask);
  for (i = 0; i < TERMSIGS_LENGTH; i++)
    {
      /* Skip a signal if it's trapped or handled specially, because the
	 trap code will restore the correct value. */
      if (signal_is_trapped (XSIG (i)) || signal_is_special (XSIG (i)))
	continue;

      act.sa_handler = XHANDLER (i);
      act.sa_flags = XSAFLAGS (i);
      sigaction (XSIG (i), &act, (struct sigaction *) NULL);
    }
#else /* !HAVE_POSIX_SIGNALS */
  for (i = 0; i < TERMSIGS_LENGTH; i++)
    {
      if (signal_is_trapped (XSIG (i)) || signal_is_special (XSIG (i)))
	continue;

      signal (XSIG (i), XHANDLER (i));
    }
#endif /* !HAVE_POSIX_SIGNALS */

  termsigs_initialized = 0;
}
#undef XHANDLER

/* Run some of the cleanups that should be performed when we run
   jump_to_top_level from a builtin command context.  XXX - might want to
   also call reset_parser here. */
void
top_level_cleanup ()
{
  /* Clean up string parser environment. */
  while (parse_and_execute_level)
    parse_and_execute_cleanup (-1);

#if defined (PROCESS_SUBSTITUTION)
  unlink_fifo_list ();
#endif /* PROCESS_SUBSTITUTION */

  run_unwind_protects ();
  loop_level = continuing = breaking = funcnest = 0;
  executing_list = comsub_ignore_return = return_catch_flag = wait_intr_flag = 0;
}

/* What to do when we've been interrupted, and it is safe to handle it. */
void
throw_to_top_level ()
{
  int print_newline = 0;

  if (interrupt_state)
    {
      if (last_command_exit_value < 128)
	last_command_exit_value = 128 + SIGINT;
      set_pipestatus_from_exit (last_command_exit_value);
      print_newline = 1;
      DELINTERRUPT;
    }

  if (interrupt_state)
    return;

  last_command_exit_signal = (last_command_exit_value > 128) ?
				(last_command_exit_value - 128) : 0;
  last_command_exit_value |= 128;
  set_pipestatus_from_exit (last_command_exit_value);

  /* Run any traps set on SIGINT, mostly for interactive shells */
  if (signal_is_trapped (SIGINT) && signal_is_pending (SIGINT))
    run_interrupt_trap (1);

  /* Clean up string parser environment. */
  while (parse_and_execute_level)
    parse_and_execute_cleanup (-1);

  if (running_trap > 0)
    {
      run_trap_cleanup (running_trap - 1);
      running_trap = 0;
    }

#if defined (JOB_CONTROL)
  give_terminal_to (shell_pgrp, 0);
#endif /* JOB_CONTROL */

  /* This needs to stay because jobs.c:make_child() uses it without resetting
     the signal mask. */
  restore_sigmask ();  

  reset_parser ();

#if defined (READLINE)
  if (interactive)
    bashline_reset ();
#endif /* READLINE */

#if defined (PROCESS_SUBSTITUTION)
  unlink_fifo_list ();
#endif /* PROCESS_SUBSTITUTION */

  run_unwind_protects ();
  loop_level = continuing = breaking = funcnest = 0;
  executing_list = comsub_ignore_return = return_catch_flag = wait_intr_flag = 0;

  if (interactive && print_newline)
    {
      fflush (stdout);
      fprintf (stderr, "\n");
      fflush (stderr);
    }

  /* An interrupted `wait' command in a script does not exit the script. */
  if (interactive || (interactive_shell && !shell_initialized) ||
      (print_newline && signal_is_trapped (SIGINT)))
    jump_to_top_level (DISCARD);
  else
    jump_to_top_level (EXITPROG);
}

/* This is just here to isolate the longjmp calls. */
void
jump_to_top_level (value)
     int value;
{
  sh_longjmp (top_level, value);
}

void
restore_sigmask ()
{
#if defined (JOB_CONTROL) || defined (HAVE_POSIX_SIGNALS)
  sigprocmask (SIG_SETMASK, &top_level_mask, (sigset_t *)NULL);
#endif
}

sighandler
termsig_sighandler (sig)
     int sig;
{
  /* If we get called twice with the same signal before handling it,
     terminate right away. */
  if (
#ifdef SIGHUP
    sig != SIGHUP &&
#endif
#ifdef SIGINT
    sig != SIGINT &&
#endif
#ifdef SIGDANGER
    sig != SIGDANGER &&
#endif
#ifdef SIGPIPE
    sig != SIGPIPE &&
#endif
#ifdef SIGALRM
    sig != SIGALRM &&
#endif
#ifdef SIGTERM
    sig != SIGTERM &&
#endif
#ifdef SIGXCPU
    sig != SIGXCPU &&
#endif
#ifdef SIGXFSZ
    sig != SIGXFSZ &&
#endif
#ifdef SIGVTALRM
    sig != SIGVTALRM &&
#endif
#ifdef SIGLOST
    sig != SIGLOST &&
#endif
#ifdef SIGUSR1
    sig != SIGUSR1 &&
#endif
#ifdef SIGUSR2
   sig != SIGUSR2 &&
#endif
   sig == terminating_signal)
    terminate_immediately = 1;

  terminating_signal = sig;

  if (terminate_immediately)
    {
#if defined (HISTORY)
      /* XXX - will inhibit history file being written */
#  if defined (READLINE)
      if (interactive_shell == 0 || interactive == 0 || (sig != SIGHUP && sig != SIGTERM) || no_line_editing || (RL_ISSTATE (RL_STATE_READCMD) == 0))
#  endif
        history_lines_this_session = 0;
#endif
      terminate_immediately = 0;
      termsig_handler (sig);
    }

#if defined (READLINE)
  /* Set the event hook so readline will call it after the signal handlers
     finish executing, so if this interrupted character input we can get
     quick response.  If readline is active or has modified the terminal we
     need to set this no matter what the signal is, though the check for
     RL_STATE_TERMPREPPED is possibly redundant. */
  if (RL_ISSTATE (RL_STATE_SIGHANDLER) || RL_ISSTATE (RL_STATE_TERMPREPPED))
    bashline_set_event_hook ();
#endif

  SIGRETURN (0);
}

void
termsig_handler (sig)
     int sig;
{
  static int handling_termsig = 0;
  int i, core;
  sigset_t mask;

  /* Simple semaphore to keep this function from being executed multiple
     times.  Since we no longer are running as a signal handler, we don't
     block multiple occurrences of the terminating signals while running. */
  if (handling_termsig)
    return;
  handling_termsig = 1;
  terminating_signal = 0;	/* keep macro from re-testing true. */

  /* I don't believe this condition ever tests true. */
  if (sig == SIGINT && signal_is_trapped (SIGINT))
    run_interrupt_trap (0);

#if defined (HISTORY)
  /* If we don't do something like this, the history will not be saved when
     an interactive shell is running in a terminal window that gets closed
     with the `close' button.  We can't test for RL_STATE_READCMD because
     readline no longer handles SIGTERM synchronously.  */
  if (interactive_shell && interactive && (sig == SIGHUP || sig == SIGTERM) && remember_on_history)
    maybe_save_shell_history ();
#endif /* HISTORY */

  if (this_shell_builtin == read_builtin)
    read_tty_cleanup ();

#if defined (JOB_CONTROL)
  if (sig == SIGHUP && (interactive || (subshell_environment & (SUBSHELL_COMSUB|SUBSHELL_PROCSUB))))
    hangup_all_jobs ();

  if ((subshell_environment & (SUBSHELL_COMSUB|SUBSHELL_PROCSUB)) == 0)
    end_job_control ();
#endif /* JOB_CONTROL */

#if defined (PROCESS_SUBSTITUTION)
  unlink_all_fifos ();
#  if defined (JOB_CONTROL)
  procsub_clear ();
#  endif
#endif /* PROCESS_SUBSTITUTION */

  /* Reset execution context */
  loop_level = continuing = breaking = funcnest = 0;
  executing_list = comsub_ignore_return = return_catch_flag = wait_intr_flag = 0;

  run_exit_trap ();	/* XXX - run exit trap possibly in signal context? */

  /* We don't change the set of blocked signals. If a user starts the shell
     with a terminating signal blocked, we won't get here (and if by some
     magic chance we do, we'll exit below). What we do is to restore the
     top-level signal mask, in case this is called from a terminating signal
     handler context, in which case the signal is blocked. */
  restore_sigmask ();

  set_signal_handler (sig, SIG_DFL);

  kill (getpid (), sig);

  if (dollar_dollar_pid != 1)
    exit (128+sig);		/* just in case the kill fails? */

  /* We get here only under extraordinary circumstances. */

  /* We are PID 1, and the kill above failed to kill the process. We assume
     this means that we are running as an init process in a pid namespace
     on Linux. In this case, we can't send ourselves a fatal signal, so we
     determine whether or not we should have generated a core dump with the
     kill call and attempt to trick the kernel into generating one if
     necessary. */
  sigprocmask (SIG_SETMASK, (sigset_t *)NULL, &mask);
  for (i = core = 0; i < TERMSIGS_LENGTH; i++)
    {
      set_signal_handler (XSIG (i), SIG_DFL);
      sigdelset (&mask, XSIG (i));
      if (sig == XSIG (i))
	core = XCOREDUMP (i);
    }
  sigprocmask (SIG_SETMASK, &mask, (sigset_t *)NULL);

  if (core)
    *((volatile unsigned long *) NULL) = 0xdead0000 + sig;	/* SIGSEGV */

  exit (128+sig);
}
#undef XSIG

/* What we really do when SIGINT occurs. */
sighandler
sigint_sighandler (sig)
     int sig;
{
#if defined (MUST_REINSTALL_SIGHANDLERS)
  signal (sig, sigint_sighandler);
#endif

  /* interrupt_state needs to be set for the stack of interrupts to work
     right.  Should it be set unconditionally? */
  if (interrupt_state == 0)
    ADDINTERRUPT;

  /* We will get here in interactive shells with job control active; allow
     an interactive wait to be interrupted.  wait_intr_flag is only set during
     the execution of the wait builtin and when wait_intr_buf is valid. */
  if (wait_intr_flag)
    {
      last_command_exit_value = 128 + sig;
      set_pipestatus_from_exit (last_command_exit_value);
      wait_signal_received = sig;
      SIGRETURN (0);
    }

  /* In interactive shells, we will get here instead of trap_handler() so
     note that we have a trap pending. */
  if (signal_is_trapped (sig))
    set_trap_state (sig);

  /* This is no longer used, but this code block remains as a reminder. */
  if (interrupt_immediately)
    {
      interrupt_immediately = 0;
      set_exit_status (128 + sig);
      throw_to_top_level ();
    }
#if defined (READLINE)
  /* Set the event hook so readline will call it after the signal handlers
     finish executing, so if this interrupted character input we can get
     quick response. */
  else if (RL_ISSTATE (RL_STATE_SIGHANDLER))
    bashline_set_event_hook ();
#endif

  SIGRETURN (0);
}

#if defined (SIGWINCH)
sighandler
sigwinch_sighandler (sig)
     int sig;
{
#if defined (MUST_REINSTALL_SIGHANDLERS)
  set_signal_handler (SIGWINCH, sigwinch_sighandler);
#endif /* MUST_REINSTALL_SIGHANDLERS */
  sigwinch_received = 1;
  SIGRETURN (0);
}
#endif /* SIGWINCH */

void
set_sigwinch_handler ()
{
#if defined (SIGWINCH)
 old_winch = set_signal_handler (SIGWINCH, sigwinch_sighandler);
#endif
}

void
unset_sigwinch_handler ()
{
#if defined (SIGWINCH)
  set_signal_handler (SIGWINCH, old_winch);
#endif
}

sighandler
sigterm_sighandler (sig)
     int sig;
{
  sigterm_received = 1;		/* XXX - counter? */
  SIGRETURN (0);
}

/* Signal functions used by the rest of the code. */
#if !defined (HAVE_POSIX_SIGNALS)

/* Perform OPERATION on NEWSET, perhaps leaving information in OLDSET. */
sigprocmask (operation, newset, oldset)
     int operation, *newset, *oldset;
{
  int old, new;

  if (newset)
    new = *newset;
  else
    new = 0;

  switch (operation)
    {
    case SIG_BLOCK:
      old = sigblock (new);
      break;

    case SIG_SETMASK:
      old = sigsetmask (new);
      break;

    default:
      internal_error (_("sigprocmask: %d: invalid operation"), operation);
    }

  if (oldset)
    *oldset = old;
}

#else

#if !defined (SA_INTERRUPT)
#  define SA_INTERRUPT 0
#endif

#if !defined (SA_RESTART)
#  define SA_RESTART 0
#endif

SigHandler *
set_signal_handler (sig, handler)
     int sig;
     SigHandler *handler;
{
  struct sigaction act, oact;

  act.sa_handler = handler;
  act.sa_flags = 0;

  /* XXX - bash-4.2 */
  /* We don't want a child death to interrupt interruptible system calls, even
     if we take the time to reap children */
#if defined (SIGCHLD)
  if (sig == SIGCHLD)
    act.sa_flags |= SA_RESTART;		/* XXX */
#endif
  /* Let's see if we can keep SIGWINCH from interrupting interruptible system
     calls, like open(2)/read(2)/write(2) */
#if defined (SIGWINCH)
  if (sig == SIGWINCH)
    act.sa_flags |= SA_RESTART;		/* XXX */
#endif
  /* If we're installing a SIGTERM handler for interactive shells, we want
     it to be as close to SIG_IGN as possible. */
  if (sig == SIGTERM && handler == sigterm_sighandler)
    act.sa_flags |= SA_RESTART;		/* XXX */

  sigemptyset (&act.sa_mask);
  sigemptyset (&oact.sa_mask);
  if (sigaction (sig, &act, &oact) == 0)
    return (oact.sa_handler);
  else
    return (SIG_DFL);
}
#endif /* HAVE_POSIX_SIGNALS */
