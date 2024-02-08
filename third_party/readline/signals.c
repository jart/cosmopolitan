/* signals.c -- signal handling support for readline. */

/* Copyright (C) 1987-2021 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library (Readline), a library
   for reading lines of text with interactive input and history editing.      

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
*/

#define READLINE_LIBRARY

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <stdio.h>		/* Just for NULL.  Yuck. */
#include <sys/types.h>
#include <signal.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

/* System-specific feature definitions and include files. */
#include "rldefs.h"

#if defined (GWINSZ_IN_SYS_IOCTL)
#  include <sys/ioctl.h>
#endif /* GWINSZ_IN_SYS_IOCTL */

/* Some standard library routines. */
#include "readline.h"
#include "history.h"

#include "rlprivate.h"

#if defined (HANDLE_SIGNALS)

#define SIGHANDLER_RETURN return

/* This typedef is equivalent to the one for Function; it allows us
   to say SigHandler *foo = signal (SIGKILL, SIG_IGN); */
typedef void SigHandler (int);

#if defined (HAVE_POSIX_SIGNALS)
typedef struct sigaction sighandler_cxt;
#  define rl_sigaction(s, nh, oh)	sigaction(s, nh, oh)
#else
typedef struct { SigHandler *sa_handler; int sa_mask, sa_flags; } sighandler_cxt;
#endif /* !HAVE_POSIX_SIGNALS */

#ifndef SA_RESTART
#  define SA_RESTART 0
#endif

static SigHandler *rl_set_sighandler (int, SigHandler *, sighandler_cxt *);
static void rl_maybe_set_sighandler (int, SigHandler *, sighandler_cxt *);
static void rl_maybe_restore_sighandler (int, sighandler_cxt *);

static void rl_signal_handler (int);
static void _rl_handle_signal (int);
     
/* Exported variables for use by applications. */

/* If non-zero, readline will install its own signal handlers for
   SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGALRM, SIGTSTP, SIGTTIN, and SIGTTOU. */
int rl_catch_signals = 1;

/* If non-zero, readline will install a signal handler for SIGWINCH. */
#ifdef SIGWINCH
int rl_catch_sigwinch = 1;
#else
int rl_catch_sigwinch = 0;	/* for the readline state struct in readline.c */
#endif

/* Private variables. */
int volatile _rl_caught_signal = 0;	/* should be sig_atomic_t, but that requires including <signal.h> everywhere */

/* If non-zero, print characters corresponding to received signals as long as
   the user has indicated his desire to do so (_rl_echo_control_chars). */
int _rl_echoctl = 0;

int _rl_intr_char = 0;
int _rl_quit_char = 0;
int _rl_susp_char = 0;

static int signals_set_flag;
static int sigwinch_set_flag;

#if defined (HAVE_POSIX_SIGNALS)
sigset_t _rl_orig_sigset;
#endif /* !HAVE_POSIX_SIGNALS */

/* **************************************************************** */
/*					        		    */
/*			   Signal Handling                          */
/*								    */
/* **************************************************************** */

static sighandler_cxt old_int, old_term, old_hup, old_alrm, old_quit;
#if defined (SIGTSTP)
static sighandler_cxt old_tstp, old_ttou, old_ttin;
#endif
#if defined (SIGWINCH)
static sighandler_cxt old_winch;
#endif

_rl_sigcleanup_func_t *_rl_sigcleanup;
void *_rl_sigcleanarg;

/* Readline signal handler functions. */

/* Called from RL_CHECK_SIGNALS() macro to run signal handling code. */
void
_rl_signal_handler (int sig)
{
  _rl_caught_signal = 0;	/* XXX */

#if defined (SIGWINCH)
  if (sig == SIGWINCH)
    {
      RL_SETSTATE(RL_STATE_SIGHANDLER);

      rl_resize_terminal ();
      /* XXX - experimental for now */
      /* Call a signal hook because though we called the original signal handler
	 in rl_sigwinch_handler below, we will not resend the signal to
	 ourselves. */
      if (rl_signal_event_hook)
	(*rl_signal_event_hook) ();

      RL_UNSETSTATE(RL_STATE_SIGHANDLER);
    }
  else
#endif
    _rl_handle_signal (sig);

  SIGHANDLER_RETURN;
}

static void
rl_signal_handler (int sig)
{
  _rl_caught_signal = sig;
  SIGHANDLER_RETURN;
}

/* This is called to handle a signal when it is safe to do so (out of the
   signal handler execution path). Called by _rl_signal_handler for all the
   signals readline catches except SIGWINCH. */
static void
_rl_handle_signal (int sig)
{
  int block_sig;

#if defined (HAVE_POSIX_SIGNALS)
  sigset_t set, oset;
#else /* !HAVE_POSIX_SIGNALS */
#  if defined (HAVE_BSD_SIGNALS)
  long omask;
#  else /* !HAVE_BSD_SIGNALS */
  sighandler_cxt dummy_cxt;	/* needed for rl_set_sighandler call */
#  endif /* !HAVE_BSD_SIGNALS */
#endif /* !HAVE_POSIX_SIGNALS */

  RL_SETSTATE(RL_STATE_SIGHANDLER);

#if !defined (HAVE_BSD_SIGNALS) && !defined (HAVE_POSIX_SIGNALS)
  /* Since the signal will not be blocked while we are in the signal
     handler, ignore it until rl_clear_signals resets the catcher. */
#  if defined (SIGALRM)
  if (sig == SIGINT || sig == SIGALRM)
#  else
  if (sig == SIGINT)
#  endif
    rl_set_sighandler (sig, SIG_IGN, &dummy_cxt);
#endif /* !HAVE_BSD_SIGNALS && !HAVE_POSIX_SIGNALS */

  /* If there's a sig cleanup function registered, call it and `deregister'
     the cleanup function to avoid multiple calls */
  if (_rl_sigcleanup)
    {
      (*_rl_sigcleanup) (sig, _rl_sigcleanarg);
      _rl_sigcleanup = 0;
      _rl_sigcleanarg = 0;
    }

#if defined (HAVE_POSIX_SIGNALS)
  /* Get the current set of blocked signals. If we want to block a signal for
     the duration of the cleanup functions, make sure to add it to SET and
     set block_sig = 1 (see the SIGHUP case below). */
  block_sig = 0;	/* sentinel to block signals with sigprocmask */
  sigemptyset (&set);
  sigprocmask (SIG_BLOCK, (sigset_t *)NULL, &set);
#endif

  switch (sig)
    {
    case SIGINT:
      /* We will end up blocking SIGTTOU while we are resetting the tty, so
	 watch out for this if it causes problems. We could prevent this by
	 setting block_sig to 1 without modifying SET. */
      _rl_reset_completion_state ();
      rl_free_line_state ();
#if defined (READLINE_CALLBACKS)
      rl_callback_sigcleanup ();
#endif

      /* FALLTHROUGH */

#if defined (SIGTSTP)
    case SIGTSTP:
    case SIGTTIN:
    case SIGTTOU:
#  if defined (HAVE_POSIX_SIGNALS)
      /* Block SIGTTOU so we can restore the terminal settings to something
	 sane without stopping on SIGTTOU if we have been placed into the
	 background.  Even trying to get the current terminal pgrp with
	 tcgetpgrp() will generate SIGTTOU, so we don't bother.  We still do
	 this even if we've been stopped on SIGTTOU, since we handle signals
	 when we have returned from the signal handler and the signal is no
	 longer blocked. */
      if (block_sig == 0)
	{
	  sigaddset (&set, SIGTTOU);
	  block_sig = 1;
	}
#  endif
#endif /* SIGTSTP */
   /* Any signals that should be blocked during cleanup should go here. */
#if defined (SIGHUP)
    case SIGHUP:
#  if defined (_AIX)
      if (block_sig == 0)
	{
	  sigaddset (&set, sig);
	  block_sig = 1;
	}
#  endif // _AIX
#endif
    /* Signals that don't require blocking during cleanup should go here. */
    case SIGTERM:
#if defined (SIGALRM)
    case SIGALRM:
      if (sig == SIGALRM)
	_rl_timeout_handle_sigalrm ();
#endif
#if defined (SIGQUIT)
    case SIGQUIT:
#endif

#if defined (HAVE_POSIX_SIGNALS)
      if (block_sig)
	sigprocmask (SIG_BLOCK, &set, &oset);
#endif

      rl_echo_signal_char (sig);
      rl_cleanup_after_signal ();

      /* At this point, the application's signal handler, if any, is the
	 current handler. */

#if defined (HAVE_POSIX_SIGNALS)
      /* Unblock any signal(s) blocked above */
      if (block_sig)
	sigprocmask (SIG_UNBLOCK, &oset, (sigset_t *)NULL);
#endif

      /* We don't have to bother unblocking the signal because we are not
	 running in a signal handler context. */

#if defined (__EMX__)
      signal (sig, SIG_ACK);
#endif

#if defined (HAVE_KILL)
      kill (getpid (), sig);
#else
      raise (sig);		/* assume we have raise */
#endif

      /* We don't need to modify the signal mask now that this is not run in
	 a signal handler context. */

      rl_reset_after_signal ();      
    }

  RL_UNSETSTATE(RL_STATE_SIGHANDLER);
  SIGHANDLER_RETURN;
}

#if defined (SIGWINCH)
static void
rl_sigwinch_handler (int sig)
{
  SigHandler *oh;

#if defined (MUST_REINSTALL_SIGHANDLERS)
  sighandler_cxt dummy_winch;

  /* We don't want to change old_winch -- it holds the state of SIGWINCH
     disposition set by the calling application.  We need this state
     because we call the application's SIGWINCH handler after updating
     our own idea of the screen size. */
  rl_set_sighandler (SIGWINCH, rl_sigwinch_handler, &dummy_winch);
#endif

  RL_SETSTATE(RL_STATE_SIGHANDLER);
  _rl_caught_signal = sig;

  /* If another sigwinch handler has been installed, call it. */
  oh = (SigHandler *)old_winch.sa_handler;
  if (oh &&  oh != (SigHandler *)SIG_IGN && oh != (SigHandler *)SIG_DFL)
    (*oh) (sig);

  RL_UNSETSTATE(RL_STATE_SIGHANDLER);
  SIGHANDLER_RETURN;
}
#endif  /* SIGWINCH */

/* Functions to manage signal handling. */

#if !defined (HAVE_POSIX_SIGNALS)
static int
rl_sigaction (int sig, sighandler_cxt *nh, sighandler_cxt *oh)
{
  oh->sa_handler = signal (sig, nh->sa_handler);
  return 0;
}
#endif /* !HAVE_POSIX_SIGNALS */

/* Set up a readline-specific signal handler, saving the old signal
   information in OHANDLER.  Return the old signal handler, like
   signal(). */
static SigHandler *
rl_set_sighandler (int sig, SigHandler *handler, sighandler_cxt *ohandler)
{
  sighandler_cxt old_handler;
#if defined (HAVE_POSIX_SIGNALS)
  struct sigaction act;

  act.sa_handler = handler;
#  if defined (SIGWINCH)
  act.sa_flags = (sig == SIGWINCH) ? SA_RESTART : 0;
#  else
  act.sa_flags = 0;
#  endif /* SIGWINCH */
  sigemptyset (&act.sa_mask);
  sigemptyset (&ohandler->sa_mask);
  sigaction (sig, &act, &old_handler);
#else
  old_handler.sa_handler = (SigHandler *)signal (sig, handler);
#endif /* !HAVE_POSIX_SIGNALS */

  /* XXX -- assume we have memcpy */
  /* If rl_set_signals is called twice in a row, don't set the old handler to
     rl_signal_handler, because that would cause infinite recursion. */
  if (handler != rl_signal_handler || old_handler.sa_handler != rl_signal_handler)
    memcpy (ohandler, &old_handler, sizeof (sighandler_cxt));

  return (ohandler->sa_handler);
}

/* Set disposition of SIG to HANDLER, returning old state in OHANDLER.  Don't
   change disposition if OHANDLER indicates the signal was ignored. */
static void
rl_maybe_set_sighandler (int sig, SigHandler *handler, sighandler_cxt *ohandler)
{
  sighandler_cxt dummy;
  SigHandler *oh;

  sigemptyset (&dummy.sa_mask);
  dummy.sa_flags = 0;
  oh = rl_set_sighandler (sig, handler, ohandler);
  if (oh == (SigHandler *)SIG_IGN)
    rl_sigaction (sig, ohandler, &dummy);
}

/* Set the disposition of SIG to HANDLER, if HANDLER->sa_handler indicates the
   signal was not being ignored.  MUST only be called for signals whose
   disposition was changed using rl_maybe_set_sighandler or for which the
   SIG_IGN check was performed inline (e.g., SIGALRM below). */
static void
rl_maybe_restore_sighandler (int sig, sighandler_cxt *handler)
{
  sighandler_cxt dummy;

  sigemptyset (&dummy.sa_mask);
  dummy.sa_flags = 0;
  if (handler->sa_handler != SIG_IGN)
    rl_sigaction (sig, handler, &dummy);
}

int
rl_set_signals (void)
{
  sighandler_cxt dummy;
  SigHandler *oh;
#if defined (HAVE_POSIX_SIGNALS)
  static int sigmask_set = 0;
  static sigset_t bset, oset;
#endif

#if defined (HAVE_POSIX_SIGNALS)
  if (rl_catch_signals && sigmask_set == 0)
    {
      sigemptyset (&bset);

      sigaddset (&bset, SIGINT);
      sigaddset (&bset, SIGTERM);
#if defined (SIGHUP)
      sigaddset (&bset, SIGHUP);
#endif
#if defined (SIGQUIT)
      sigaddset (&bset, SIGQUIT);
#endif
#if defined (SIGALRM)
      sigaddset (&bset, SIGALRM);
#endif
#if defined (SIGTSTP)
      sigaddset (&bset, SIGTSTP);
#endif
#if defined (SIGTTIN)
      sigaddset (&bset, SIGTTIN);
#endif
#if defined (SIGTTOU)
      sigaddset (&bset, SIGTTOU);
#endif
      sigmask_set = 1;
    }      
#endif /* HAVE_POSIX_SIGNALS */

  if (rl_catch_signals && signals_set_flag == 0)
    {
#if defined (HAVE_POSIX_SIGNALS)
      sigemptyset (&_rl_orig_sigset);
      sigprocmask (SIG_BLOCK, &bset, &_rl_orig_sigset);
#endif

      rl_maybe_set_sighandler (SIGINT, rl_signal_handler, &old_int);
      rl_maybe_set_sighandler (SIGTERM, rl_signal_handler, &old_term);
#if defined (SIGHUP)
      rl_maybe_set_sighandler (SIGHUP, rl_signal_handler, &old_hup);
#endif
#if defined (SIGQUIT)
      rl_maybe_set_sighandler (SIGQUIT, rl_signal_handler, &old_quit);
#endif

#if defined (SIGALRM)
      oh = rl_set_sighandler (SIGALRM, rl_signal_handler, &old_alrm);
      if (oh == (SigHandler *)SIG_IGN)
	rl_sigaction (SIGALRM, &old_alrm, &dummy);
#if defined (HAVE_POSIX_SIGNALS) && defined (SA_RESTART)
      /* If the application using readline has already installed a signal
	 handler with SA_RESTART, SIGALRM will cause reads to be restarted
	 automatically, so readline should just get out of the way.  Since
	 we tested for SIG_IGN above, we can just test for SIG_DFL here. */
      if (oh != (SigHandler *)SIG_DFL && (old_alrm.sa_flags & SA_RESTART))
	rl_sigaction (SIGALRM, &old_alrm, &dummy);
#endif /* HAVE_POSIX_SIGNALS */
#endif /* SIGALRM */

#if defined (SIGTSTP)
      rl_maybe_set_sighandler (SIGTSTP, rl_signal_handler, &old_tstp);
#endif /* SIGTSTP */

#if defined (SIGTTOU)
      rl_maybe_set_sighandler (SIGTTOU, rl_signal_handler, &old_ttou);
#endif /* SIGTTOU */

#if defined (SIGTTIN)
      rl_maybe_set_sighandler (SIGTTIN, rl_signal_handler, &old_ttin);
#endif /* SIGTTIN */

      signals_set_flag = 1;

#if defined (HAVE_POSIX_SIGNALS)
      sigprocmask (SIG_SETMASK, &_rl_orig_sigset, (sigset_t *)NULL);
#endif
    }
  else if (rl_catch_signals == 0)
    {
#if defined (HAVE_POSIX_SIGNALS)
      sigemptyset (&_rl_orig_sigset);
      sigprocmask (SIG_BLOCK, (sigset_t *)NULL, &_rl_orig_sigset);
#endif
    }

#if defined (SIGWINCH)
  if (rl_catch_sigwinch && sigwinch_set_flag == 0)
    {
      rl_maybe_set_sighandler (SIGWINCH, rl_sigwinch_handler, &old_winch);
      sigwinch_set_flag = 1;
    }
#endif /* SIGWINCH */

  return 0;
}

int
rl_clear_signals (void)
{
  sighandler_cxt dummy;

  if (rl_catch_signals && signals_set_flag == 1)
    {
      /* Since rl_maybe_set_sighandler doesn't override a SIG_IGN handler,
	 we should in theory not have to restore a handler where
	 old_xxx.sa_handler == SIG_IGN.  That's what rl_maybe_restore_sighandler
	 does.  Fewer system calls should reduce readline's per-line
	 overhead */
      rl_maybe_restore_sighandler (SIGINT, &old_int);
      rl_maybe_restore_sighandler (SIGTERM, &old_term);
#if defined (SIGHUP)
      rl_maybe_restore_sighandler (SIGHUP, &old_hup);
#endif
#if defined (SIGQUIT)
      rl_maybe_restore_sighandler (SIGQUIT, &old_quit);
#endif
#if defined (SIGALRM)
      rl_maybe_restore_sighandler (SIGALRM, &old_alrm);
#endif

#if defined (SIGTSTP)
      rl_maybe_restore_sighandler (SIGTSTP, &old_tstp);
#endif /* SIGTSTP */

#if defined (SIGTTOU)
      rl_maybe_restore_sighandler (SIGTTOU, &old_ttou);
#endif /* SIGTTOU */

#if defined (SIGTTIN)
      rl_maybe_restore_sighandler (SIGTTIN, &old_ttin);
#endif /* SIGTTIN */

      signals_set_flag = 0;
    }

#if defined (SIGWINCH)
  if (rl_catch_sigwinch && sigwinch_set_flag == 1)
    {
      sigemptyset (&dummy.sa_mask);
      rl_sigaction (SIGWINCH, &old_winch, &dummy);
      sigwinch_set_flag = 0;
    }
#endif

  return 0;
}

/* Clean up the terminal and readline state after catching a signal, before
   resending it to the calling application. */
void
rl_cleanup_after_signal (void)
{
  _rl_clean_up_for_exit ();
  if (rl_deprep_term_function)
    (*rl_deprep_term_function) ();
  rl_clear_pending_input ();
  rl_clear_signals ();
}

/* Reset the terminal and readline state after a signal handler returns. */
void
rl_reset_after_signal (void)
{
  if (rl_prep_term_function)
    (*rl_prep_term_function) (_rl_meta_flag);
  rl_set_signals ();
}

/* Free up the readline variable line state for the current line (undo list,
   any partial history entry, any keyboard macros in progress, and any
   numeric arguments in process) after catching a signal, before calling
   rl_cleanup_after_signal(). */ 
void
rl_free_line_state (void)
{
  register HIST_ENTRY *entry;

  rl_free_undo_list ();

  entry = current_history ();
  if (entry)
    entry->data = (char *)NULL;

  _rl_kill_kbd_macro ();
  rl_clear_message ();
  _rl_reset_argument ();
}

int
rl_pending_signal (void)
{
  return (_rl_caught_signal);
}

void
rl_check_signals (void)
{
  RL_CHECK_SIGNALS ();
}
#endif  /* HANDLE_SIGNALS */

/* **************************************************************** */
/*								    */
/*			   SIGINT Management			    */
/*								    */
/* **************************************************************** */

#if defined (HAVE_POSIX_SIGNALS)
static sigset_t sigint_set, sigint_oset;
static sigset_t sigwinch_set, sigwinch_oset;
#else /* !HAVE_POSIX_SIGNALS */
#  if defined (HAVE_BSD_SIGNALS)
static int sigint_oldmask;
static int sigwinch_oldmask;
#  endif /* HAVE_BSD_SIGNALS */
#endif /* !HAVE_POSIX_SIGNALS */

static int sigint_blocked;
static int sigwinch_blocked;

/* Cause SIGINT to not be delivered until the corresponding call to
   release_sigint(). */
void
_rl_block_sigint (void)
{
  if (sigint_blocked)
    return;

  sigint_blocked = 1;
}

/* Allow SIGINT to be delivered. */
void
_rl_release_sigint (void)
{
  if (sigint_blocked == 0)
    return;

  sigint_blocked = 0;
  RL_CHECK_SIGNALS ();
}

/* Cause SIGWINCH to not be delivered until the corresponding call to
   release_sigwinch(). */
void
_rl_block_sigwinch (void)
{
  if (sigwinch_blocked)
    return;

#if defined (SIGWINCH)

#if defined (HAVE_POSIX_SIGNALS)
  sigemptyset (&sigwinch_set);
  sigemptyset (&sigwinch_oset);
  sigaddset (&sigwinch_set, SIGWINCH);
  sigprocmask (SIG_BLOCK, &sigwinch_set, &sigwinch_oset);
#else /* !HAVE_POSIX_SIGNALS */
#  if defined (HAVE_BSD_SIGNALS)
  sigwinch_oldmask = sigblock (sigmask (SIGWINCH));
#  else /* !HAVE_BSD_SIGNALS */
#    if defined (HAVE_USG_SIGHOLD)
  sighold (SIGWINCH);
#    endif /* HAVE_USG_SIGHOLD */
#  endif /* !HAVE_BSD_SIGNALS */
#endif /* !HAVE_POSIX_SIGNALS */

#endif /* SIGWINCH */

  sigwinch_blocked = 1;
}

/* Allow SIGWINCH to be delivered. */
void
_rl_release_sigwinch (void)
{
  if (sigwinch_blocked == 0)
    return;

#if defined (SIGWINCH)

#if defined (HAVE_POSIX_SIGNALS)
  sigprocmask (SIG_SETMASK, &sigwinch_oset, (sigset_t *)NULL);
#else
#  if defined (HAVE_BSD_SIGNALS)
  sigsetmask (sigwinch_oldmask);
#  else /* !HAVE_BSD_SIGNALS */
#    if defined (HAVE_USG_SIGHOLD)
  sigrelse (SIGWINCH);
#    endif /* HAVE_USG_SIGHOLD */
#  endif /* !HAVE_BSD_SIGNALS */
#endif /* !HAVE_POSIX_SIGNALS */

#endif /* SIGWINCH */

  sigwinch_blocked = 0;
}

/* **************************************************************** */
/*								    */
/*		Echoing special control characters		    */
/*								    */
/* **************************************************************** */
void
rl_echo_signal_char (int sig)
{
  char cstr[3];
  int cslen, c;

  if (_rl_echoctl == 0 || _rl_echo_control_chars == 0)
    return;

  switch (sig)
    {
    case SIGINT:  c = _rl_intr_char; break;
#if defined (SIGQUIT)
    case SIGQUIT: c = _rl_quit_char; break;
#endif
#if defined (SIGTSTP)
    case SIGTSTP: c = _rl_susp_char; break;
#endif
    default: return;
    }

  if (CTRL_CHAR (c) || c == RUBOUT)
    {
      cstr[0] = '^';
      cstr[1] = CTRL_CHAR (c) ? UNCTRL (c) : '?';
      cstr[cslen = 2] = '\0';
    }
  else
    {
      cstr[0] = c;
      cstr[cslen = 1] = '\0';
    }

  _rl_output_some_chars (cstr, cslen);
}
