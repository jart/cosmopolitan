/* input.c -- character input functions for readline. */

/* Copyright (C) 1994-2021 Free Software Foundation, Inc.

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

#if defined (__TANDEM)
#  define _XOPEN_SOURCE_EXTENDED 1
#  define _TANDEM_SOURCE 1
#  include <floss.h>
#endif

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <sys/types.h>
#include <fcntl.h>
#if defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif /* HAVE_SYS_FILE_H */

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#include <signal.h>

#include "posixselect.h"
#include "posixtime.h"

#if defined (FIONREAD_IN_SYS_IOCTL)
#  include <sys/ioctl.h>
#endif

#include <stdio.h>
#include <errno.h>

#if !defined (errno)
extern int errno;
#endif /* !errno */

/* System-specific feature definitions and include files. */
#include "rldefs.h"
#include "rlmbutil.h"

/* Some standard library routines. */
#include "readline.h"

#include "rlprivate.h"
#include "rlshell.h"
#include "xmalloc.h"

/* What kind of non-blocking I/O do we have? */
#if !defined (O_NDELAY) && defined (O_NONBLOCK)
#  define O_NDELAY O_NONBLOCK	/* Posix style */
#endif

#if defined (HAVE_PSELECT) || defined (HAVE_SELECT)
extern sigset_t _rl_orig_sigset;
#endif

/* Non-null means it is a pointer to a function to run while waiting for
   character input. */
rl_hook_func_t *rl_event_hook = (rl_hook_func_t *)NULL;

/* A function to call if a read(2) is interrupted by a signal. */
rl_hook_func_t *rl_signal_event_hook = (rl_hook_func_t *)NULL;

/* A function to call when readline times out after a time is specified. */
rl_hook_func_t *rl_timeout_event_hook = (rl_hook_func_t *)NULL;

/* A function to replace _rl_input_available for applications using the
   callback interface. */
rl_hook_func_t *rl_input_available_hook = (rl_hook_func_t *)NULL;

rl_getc_func_t *rl_getc_function = rl_getc;

static int _keyboard_input_timeout = 100000;		/* 0.1 seconds; it's in usec */

static int ibuffer_space (void);
static int rl_get_char (int *);
static int rl_gather_tyi (void);

/* Windows isatty returns true for every character device, including the null
   device, so we need to perform additional checks. */
#if defined (_WIN32) && !defined (__CYGWIN__)
#include <io.h>
#include <conio.h>
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

int
win32_isatty (int fd)
{
  if (_isatty(fd))
    {
      HANDLE h;
      DWORD ignored;

      if ((h = (HANDLE) _get_osfhandle (fd)) == INVALID_HANDLE_VALUE)
	{
	  errno = EBADF;
	  return 0;
	}
      if (GetConsoleMode (h, &ignored) != 0)
	return 1;
    }
  errno = ENOTTY;
  return 0;
}

#define isatty(x)	win32_isatty(x)
#endif

/* Readline timeouts */

/* I don't know how to set a timeout for _getch() in MinGW32, so we use
   SIGALRM. */
#if (defined (HAVE_PSELECT) || defined (HAVE_SELECT)) && !defined (__MINGW32__)
#  define RL_TIMEOUT_USE_SELECT
#else
#  define RL_TIMEOUT_USE_SIGALRM
#endif

int rl_set_timeout (unsigned int, unsigned int);
int rl_timeout_remaining (unsigned int *, unsigned int *);

int _rl_timeout_init (void);
int _rl_timeout_sigalrm_handler (void);
int _rl_timeout_select (int, fd_set *, fd_set *, fd_set *, const struct timeval *, const sigset_t *);

static void _rl_timeout_handle (void);
#if defined (RL_TIMEOUT_USE_SIGALRM)
static int set_alarm (unsigned int *, unsigned int *);
static void reset_alarm (void);
#endif

/* We implement timeouts as a future time using a supplied interval
   (timeout_duration) from when the timeout is set (timeout_point).
   That allows us to easily determine whether the timeout has occurred
   and compute the time remaining until it does. */
static struct timeval timeout_point;
static struct timeval timeout_duration;

/* **************************************************************** */
/*								    */
/*			Character Input Buffering       	    */
/*								    */
/* **************************************************************** */

static int pop_index, push_index;
static unsigned char ibuffer[512];
static int ibuffer_len = sizeof (ibuffer) - 1;

#define any_typein (push_index != pop_index)

int
_rl_any_typein (void)
{
  return any_typein;
}

int
_rl_pushed_input_available (void)
{
  return (push_index != pop_index);
}

/* Return the amount of space available in the buffer for stuffing
   characters. */
static int
ibuffer_space (void)
{
  if (pop_index > push_index)
    return (pop_index - push_index - 1);
  else
    return (ibuffer_len - (push_index - pop_index));
}

/* Get a key from the buffer of characters to be read.
   Return the key in KEY.
   Result is non-zero if there was a key, or 0 if there wasn't. */
static int
rl_get_char (int *key)
{
  if (push_index == pop_index)
    return (0);

  *key = ibuffer[pop_index++];
#if 0
  if (pop_index >= ibuffer_len)
#else
  if (pop_index > ibuffer_len)
#endif
    pop_index = 0;

  return (1);
}

/* Stuff KEY into the *front* of the input buffer.
   Returns non-zero if successful, zero if there is
   no space left in the buffer. */
int
_rl_unget_char (int key)
{
  if (ibuffer_space ())
    {
      pop_index--;
      if (pop_index < 0)
	pop_index = ibuffer_len;
      ibuffer[pop_index] = key;
      return (1);
    }
  return (0);
}

/* If a character is available to be read, then read it and stuff it into
   IBUFFER.  Otherwise, just return.  Returns number of characters read
   (0 if none available) and -1 on error (EIO). */
static int
rl_gather_tyi (void)
{
  int tty;
  register int tem, result;
  int chars_avail, k;
  char input;
#if defined(HAVE_SELECT)
  fd_set readfds, exceptfds;
  struct timeval timeout;
#endif

  chars_avail = 0;
  input = 0;
  tty = fileno (rl_instream);

#if defined (HAVE_PSELECT) || defined (HAVE_SELECT)
  FD_ZERO (&readfds);
  FD_ZERO (&exceptfds);
  FD_SET (tty, &readfds);
  FD_SET (tty, &exceptfds);
  USEC_TO_TIMEVAL (_keyboard_input_timeout, timeout);
#if defined (RL_TIMEOUT_USE_SELECT)
  result = _rl_timeout_select (tty + 1, &readfds, (fd_set *)NULL, &exceptfds, &timeout, NULL);
#else
  result = select (tty + 1, &readfds, (fd_set *)NULL, &exceptfds, &timeout);
#endif
  if (result <= 0)
    return 0;	/* Nothing to read. */
#endif

  result = -1;
  errno = 0;
#if defined (FIONREAD)
  result = ioctl (tty, FIONREAD, &chars_avail);
  if (result == -1 && errno == EIO)
    return -1;
  if (result == -1)
    chars_avail = 0;
#endif

#if defined (O_NDELAY)
  if (result == -1)
    {
      tem = fcntl (tty, F_GETFL, 0);

      fcntl (tty, F_SETFL, (tem | O_NDELAY));
      chars_avail = read (tty, &input, 1);

      fcntl (tty, F_SETFL, tem);
      if (chars_avail == -1 && errno == EAGAIN)
	return 0;
      if (chars_avail == -1 && errno == EIO)
	return -1;
      if (chars_avail == 0)	/* EOF */
	{
	  rl_stuff_char (EOF);
	  return (0);
	}
    }
#endif /* O_NDELAY */

#if defined (__MINGW32__)
  /* Use getch/_kbhit to check for available console input, in the same way
     that we read it normally. */
   chars_avail = isatty (tty) ? _kbhit () : 0;
   result = 0;
#endif

  /* If there's nothing available, don't waste time trying to read
     something. */
  if (chars_avail <= 0)
    return 0;

  tem = ibuffer_space ();

  if (chars_avail > tem)
    chars_avail = tem;

  /* One cannot read all of the available input.  I can only read a single
     character at a time, or else programs which require input can be
     thwarted.  If the buffer is larger than one character, I lose.
     Damn! */
  if (tem < ibuffer_len)
    chars_avail = 0;

  if (result != -1)
    {
      while (chars_avail--)
	{
	  RL_CHECK_SIGNALS ();
	  k = (*rl_getc_function) (rl_instream);
	  if (rl_stuff_char (k) == 0)
	    break;			/* some problem; no more room */
	  if (k == NEWLINE || k == RETURN)
	    break;
	}
    }
  else
    {
      if (chars_avail)
	rl_stuff_char (input);
    }

  return 1;
}

int
rl_set_keyboard_input_timeout (int u)
{
  int o;

  o = _keyboard_input_timeout;
  if (u >= 0)
    _keyboard_input_timeout = u;
  return (o);
}

/* Is there input available to be read on the readline input file
   descriptor?  Only works if the system has select(2) or FIONREAD.
   Uses the value of _keyboard_input_timeout as the timeout; if another
   readline function wants to specify a timeout and not leave it up to
   the user, it should use _rl_input_queued(timeout_value_in_microseconds)
   instead. */
int
_rl_input_available (void)
{
#if defined (HAVE_PSELECT) || defined (HAVE_SELECT)
  fd_set readfds, exceptfds;
  struct timeval timeout;
#endif
#if !defined (HAVE_SELECT) && defined (FIONREAD)
  int chars_avail;
#endif
  int tty;

  if (rl_input_available_hook)
    return (*rl_input_available_hook) ();

  tty = fileno (rl_instream);

#if 0  // TODO(ahgamut): Why do we need it?
#if defined (HAVE_PSELECT) || defined (HAVE_SELECT)
  FD_ZERO (&readfds);
  FD_ZERO (&exceptfds);
  FD_SET (tty, &readfds);
  FD_SET (tty, &exceptfds);
  USEC_TO_TIMEVAL (_keyboard_input_timeout, timeout);
#  if defined (RL_TIMEOUT_USE_SELECT)
  return (_rl_timeout_select (tty + 1, &readfds, (fd_set *)NULL, &exceptfds, &timeout, NULL) > 0);
#  else
  return (select (tty + 1, &readfds, (fd_set *)NULL, &exceptfds, &timeout) > 0);
#  endif
#else
#endif

#if defined (FIONREAD)
  if (ioctl (tty, FIONREAD, &chars_avail) == 0)
    return (chars_avail);
#endif

#endif

#if defined (__MINGW32__)
  if (isatty (tty))
    return (_kbhit ());
#endif

  return 0;
}

int
_rl_nchars_available ()
{
  int chars_avail, fd, result;
  
  chars_avail = 0;
     
#if defined (FIONREAD)
  fd = fileno (rl_instream);
  errno = 0;    
  result = ioctl (fd, FIONREAD, &chars_avail);    
  if (result == -1 && errno == EIO)    
    return -1;    
#endif

  return chars_avail;
}

int
_rl_input_queued (int t)
{
  int old_timeout, r;

  old_timeout = rl_set_keyboard_input_timeout (t);
  r = _rl_input_available ();
  rl_set_keyboard_input_timeout (old_timeout);
  return r;
}

void
_rl_insert_typein (int c)
{    	
  int key, t, i;
  char *string;

  i = key = 0;
  string = (char *)xmalloc (ibuffer_len + 1);
  string[i++] = (char) c;

  while ((t = rl_get_char (&key)) &&
	 _rl_keymap[key].type == ISFUNC &&
	 _rl_keymap[key].function == rl_insert)
    string[i++] = key;

  if (t)
    _rl_unget_char (key);

  string[i] = '\0';
  rl_insert_text (string);
  xfree (string);
}

/* Add KEY to the buffer of characters to be read.  Returns 1 if the
   character was stuffed correctly; 0 otherwise. */
int
rl_stuff_char (int key)
{
  if (ibuffer_space () == 0)
    return 0;

  if (key == EOF)
    {
      key = NEWLINE;
      rl_pending_input = EOF;
      RL_SETSTATE (RL_STATE_INPUTPENDING);
    }
  ibuffer[push_index++] = key;
#if 0
  if (push_index >= ibuffer_len)
#else
  if (push_index > ibuffer_len)
#endif
    push_index = 0;

  return 1;
}

/* Make C be the next command to be executed. */
int
rl_execute_next (int c)
{
  rl_pending_input = c;
  RL_SETSTATE (RL_STATE_INPUTPENDING);
  return 0;
}

/* Clear any pending input pushed with rl_execute_next() */
int
rl_clear_pending_input (void)
{
  rl_pending_input = 0;
  RL_UNSETSTATE (RL_STATE_INPUTPENDING);
  return 0;
}

/* **************************************************************** */
/*								    */
/*			    Timeout utility			    */
/*								    */
/* **************************************************************** */

#if defined (RL_TIMEOUT_USE_SIGALRM)
#  if defined (HAVE_SETITIMER)

static int
set_alarm (unsigned int *secs, unsigned int *usecs)
{
  struct itimerval it;

  timerclear (&it.it_interval);
  timerset (&it.it_value, *secs, *usecs);
  return setitimer (ITIMER_REAL, &it, NULL);
}

static void
reset_alarm ()
{
  struct itimerval it;

  timerclear (&it.it_interval);
  timerclear (&it.it_value);
  setitimer (ITIMER_REAL, &it, NULL);
}
#  else
static int
set_alarm (unsigned int *secs, unsigned int *usecs)
{
  if (*secs == 0 || *usecs >= USEC_PER_SEC / 2)
    (*secs)++;
  *usecs = 0;

  return alarm (*secs);
}
static void
reset_alarm ()
{
  alarm (0);
}
#  endif
#endif

/* Set a timeout which will be used for the next call of `readline
   ()'.  When (0, 0) are specified the timeout is cleared.  */
int
rl_set_timeout (unsigned int secs, unsigned int usecs)
{
  timeout_duration.tv_sec = secs + usecs / USEC_PER_SEC;
  timeout_duration.tv_usec = usecs % USEC_PER_SEC;

  return 0;
}

/* Start measuring the time.  Returns 0 on success.  Returns -1 on
   error. */
int
_rl_timeout_init (void)
{
  unsigned int secs, usecs;

  /* Clear the timeout state of the previous edit */
  RL_UNSETSTATE(RL_STATE_TIMEOUT);
  timerclear (&timeout_point);

  /* Return 0 when timeout is unset. */
  if (timerisunset (&timeout_duration))
    return 0;

  /* Return -1 on gettimeofday error. */
  if (gettimeofday(&timeout_point, 0) != 0)
    {
      timerclear (&timeout_point);
      return -1;
    }

  secs = timeout_duration.tv_sec;
  usecs = timeout_duration.tv_usec;

#if defined (RL_TIMEOUT_USE_SIGALRM)
  /* If select(2)/pselect(2) is unavailable, use SIGALRM. */
  if (set_alarm (&secs, &usecs) < 0)
    return -1;
#endif

  timeout_point.tv_sec += secs;
  timeout_point.tv_usec += usecs;
  if (timeout_point.tv_usec >= USEC_PER_SEC)
    {
      timeout_point.tv_sec++;
      timeout_point.tv_usec -= USEC_PER_SEC;
    }

  return 0;
}

/* Get the remaining time until the scheduled timeout.  Returns -1 on
   error or no timeout set with secs and usecs unchanged.  Returns 0
   on an expired timeout with secs and usecs unchanged.  Returns 1
   when the timeout has not yet expired.  The remaining time is stored
   in secs and usecs.  When NULL is specified to either of the
   arguments, just the expiration is tested. */
int
rl_timeout_remaining (unsigned int *secs, unsigned int *usecs)
{
  struct timeval current_time;

  /* Return -1 when timeout is unset. */
  if (timerisunset (&timeout_point))
    {
      errno = 0;
      return -1;
    }

  /* Return -1 on error. errno is set by gettimeofday. */
  if (gettimeofday(&current_time, 0) != 0)
    return -1;

  /* Return 0 when timeout has already expired. */
  /* could use timercmp (&timeout_point, &current_time, <) here */
  if (current_time.tv_sec > timeout_point.tv_sec ||
	(current_time.tv_sec == timeout_point.tv_sec &&
	 current_time.tv_usec >= timeout_point.tv_usec))
    return 0;

  if (secs && usecs)
    {
      *secs = timeout_point.tv_sec - current_time.tv_sec;
      *usecs = timeout_point.tv_usec - current_time.tv_usec;
      if (timeout_point.tv_usec < current_time.tv_usec)
	{
	  (*secs)--;
	  *usecs += USEC_PER_SEC;
	}
    }

  return 1;
}

/* This should only be called if RL_TIMEOUT_USE_SELECT is defined. */

#if defined (HAVE_PSELECT) || defined (HAVE_SELECT)
int
_rl_timeout_select (int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout, const sigset_t *sigmask)
{
  int result;
#if defined (HAVE_PSELECT)
  struct timespec ts;
#else
  sigset_t origmask;
  struct timeval tv;
#endif
  int tmout_status;
  struct timeval tmout;
  unsigned int sec, usec;

  /* When the remaining time for rl_timeout is shorter than the
     keyboard input timeout, replace `timeout' with the remaining time
     for `rl_timeout' and set `tmout_status = 1'. */
  tmout_status = rl_timeout_remaining (&sec, &usec);
  tmout.tv_sec = sec;
  tmout.tv_usec = usec;

  if (tmout_status == 0)
    _rl_timeout_handle ();
  else if (tmout_status == 1)
    {
      if (timeout == NULL || timercmp (&tmout, timeout, <))
	timeout = &tmout;
      else
	tmout_status = -1;
    }

#if defined (HAVE_PSELECT)
  if (timeout)
    {
      TIMEVAL_TO_TIMESPEC (timeout, &ts);
      result = pselect (nfds, readfds, writefds, exceptfds, &ts, sigmask);
    }
  else
    result = pselect (nfds, readfds, writefds, exceptfds, NULL, sigmask);
#else
  if (sigmask)
    sigprocmask (SIG_SETMASK, sigmask, &origmask);

  if (timeout)
    {
      tv.tv_sec = timeout->tv_sec;
      tv.tv_usec = timeout->tv_usec;
      result = select (nfds, readfds, writefds, exceptfds, &tv);
    }
  else
    result = select (nfds, readfds, writefds, exceptfds, NULL);

  if (sigmask)
    sigprocmask (SIG_SETMASK, &origmask, NULL);
#endif

  if (tmout_status == 1 && result == 0)
    _rl_timeout_handle ();

  return result;
}
#endif

static void
_rl_timeout_handle ()
{
  if (rl_timeout_event_hook)
    (*rl_timeout_event_hook) ();

  RL_SETSTATE(RL_STATE_TIMEOUT);
  _rl_abort_internal ();
}

int
_rl_timeout_handle_sigalrm ()
{
#if defined (RL_TIMEOUT_USE_SIGALRM)
  if (timerisunset (&timeout_point))
    return -1;

  /* Reset `timeout_point' to the current time to ensure that later
     calls of `rl_timeout_pending ()' return 0 (timeout expired). */
  if (gettimeofday(&timeout_point, 0) != 0)
    timerclear (&timeout_point);

  reset_alarm ();

  _rl_timeout_handle ();
#endif
  return -1;
}
/* **************************************************************** */
/*								    */
/*			     Character Input			    */
/*								    */
/* **************************************************************** */

/* Read a key, including pending input. */
int
rl_read_key (void)
{
  int c, r;

  if (rl_pending_input)
    {
      c = rl_pending_input;	/* XXX - cast to unsigned char if > 0? */
      rl_clear_pending_input ();
    }
  else
    {
      /* If input is coming from a macro, then use that. */
      if ((c = _rl_next_macro_key ()))
	return ((unsigned char)c);

      /* If the user has an event function, then call it periodically. */
      if (rl_event_hook)
	{
	  while (rl_event_hook)
	    {
	      if (rl_get_char (&c) != 0)
		break;
		
	      if ((r = rl_gather_tyi ()) < 0)	/* XXX - EIO */
		{
		  rl_done = 1;
		  RL_SETSTATE (RL_STATE_DONE);
		  return (errno == EIO ? (RL_ISSTATE (RL_STATE_READCMD) ? READERR : EOF) : '\n');
		}
	      else if (r > 0)			/* read something */
		continue;

	      RL_CHECK_SIGNALS ();
	      if (rl_done)		/* XXX - experimental */
		return ('\n');
	      (*rl_event_hook) ();
	    }
	}
      else
	{
	  if (rl_get_char (&c) == 0)
	    c = (*rl_getc_function) (rl_instream);
/* fprintf(stderr, "rl_read_key: calling RL_CHECK_SIGNALS: _rl_caught_signal = %d\r\n", _rl_caught_signal); */
	  RL_CHECK_SIGNALS ();
	}
    }

  return (c);
}

int
rl_getc (FILE *stream)
{
  int result;
  unsigned char c;
  int fd;
#if defined (HAVE_PSELECT)
  sigset_t empty_set;
  fd_set readfds;
#endif

  fd = fileno (stream);
  while (1)
    {
      RL_CHECK_SIGNALS ();

      /* We know at this point that _rl_caught_signal == 0 */

#if defined (__MINGW32__)
      if (isatty (fd)
	return (_getch ());	/* "There is no error return." */
#endif
      result = 0;
#if defined (HAVE_PSELECT) || defined (HAVE_SELECT)
      /* At this point, if we have pselect, we're using select/pselect for the
	 timeouts. We handled MinGW above. */
      FD_ZERO (&readfds);
      FD_SET (fd, &readfds);
#  if defined (HANDLE_SIGNALS)
      result = _rl_timeout_select (fd + 1, &readfds, NULL, NULL, NULL, &_rl_orig_sigset);
#  else
      sigemptyset (&empty_set);
      sigprocmask (SIG_BLOCK, (sigset_t *)NULL, &empty_set);
      result = _rl_timeout_select (fd + 1, &readfds, NULL, NULL, NULL, &empty_set);
#  endif /* HANDLE_SIGNALS */
      if (result == 0)
        _rl_timeout_handle ();		/* check the timeout */
#endif
      if (result >= 0)
	result = read (fd, &c, sizeof (unsigned char));

      if (result == sizeof (unsigned char))
	return (c);

      /* If zero characters are returned, then the file that we are
	 reading from is empty!  Return EOF in that case. */
      if (result == 0)
	return (EOF);

#if defined (__BEOS__)
      if (errno == EINTR)
	continue;
#endif

#if defined (EWOULDBLOCK)
#  define X_EWOULDBLOCK EWOULDBLOCK
#else
#  define X_EWOULDBLOCK -99
#endif

#if defined (EAGAIN)
#  define X_EAGAIN EAGAIN
#else
#  define X_EAGAIN -99
#endif

      if (errno == X_EWOULDBLOCK || errno == X_EAGAIN)
	{
	  if (sh_unset_nodelay_mode (fd) < 0)
	    return (EOF);
	  continue;
	}

#undef X_EWOULDBLOCK
#undef X_EAGAIN

/* fprintf(stderr, "rl_getc: result = %d errno = %d\n", result, errno); */

handle_error:
      /* If the error that we received was EINTR, then try again,
	 this is simply an interrupted system call to read ().  We allow
	 the read to be interrupted if we caught SIGHUP, SIGTERM, or any
	 of the other signals readline treats specially. If the
	 application sets an event hook, call it for other signals.
	 Otherwise (not EINTR), some error occurred, also signifying EOF. */
      if (errno != EINTR)
	return (RL_ISSTATE (RL_STATE_READCMD) ? READERR : EOF);
      /* fatal signals of interest */
#if defined (SIGHUP)
      else if (_rl_caught_signal == SIGHUP || _rl_caught_signal == SIGTERM)
#else
      else if (_rl_caught_signal == SIGTERM)
#endif
	return (RL_ISSTATE (RL_STATE_READCMD) ? READERR : EOF);
      /* keyboard-generated signals of interest */
#if defined (SIGQUIT)
      else if (_rl_caught_signal == SIGINT || _rl_caught_signal == SIGQUIT)
#else
      else if (_rl_caught_signal == SIGINT)
#endif
        RL_CHECK_SIGNALS ();
#if defined (SIGTSTP)
      else if (_rl_caught_signal == SIGTSTP)
	RL_CHECK_SIGNALS ();
#endif
      /* non-keyboard-generated signals of interest */
#if defined (SIGWINCH)
      else if (_rl_caught_signal == SIGWINCH)
	RL_CHECK_SIGNALS ();
#endif /* SIGWINCH */
#if defined (SIGALRM)
      else if (_rl_caught_signal == SIGALRM
#  if defined (SIGVTALRM)
		|| _rl_caught_signal == SIGVTALRM
#  endif
	      )
        RL_CHECK_SIGNALS ();
#endif  /* SIGALRM */

      if (rl_signal_event_hook)
	(*rl_signal_event_hook) ();
    }
}

#if defined (HANDLE_MULTIBYTE)
/* read multibyte char */
int
_rl_read_mbchar (char *mbchar, int size)
{
  int mb_len, c;
  size_t mbchar_bytes_length;
  WCHAR_T wc;
  mbstate_t ps, ps_back;

  memset(&ps, 0, sizeof (mbstate_t));
  memset(&ps_back, 0, sizeof (mbstate_t));

  mb_len = 0;  
  while (mb_len < size)
    {
      c = (mb_len == 0) ? _rl_bracketed_read_key () : rl_read_key ();

      if (c < 0)
	break;

      mbchar[mb_len++] = c;

      mbchar_bytes_length = MBRTOWC (&wc, mbchar, mb_len, &ps);
      if (mbchar_bytes_length == (size_t)(-1))
	break;		/* invalid byte sequence for the current locale */
      else if (mbchar_bytes_length == (size_t)(-2))
	{
	  /* shorted bytes */
	  ps = ps_back;
	  continue;
	} 
      else if (mbchar_bytes_length == 0)
	{
	  mbchar[0] = '\0';	/* null wide character */
	  mb_len = 1;
	  break;
	}
      else if (mbchar_bytes_length > (size_t)(0))
	break;
    }

  return mb_len;
}

/* Read a multibyte-character string whose first character is FIRST into
   the buffer MB of length MLEN.  Returns the last character read, which
   may be FIRST.  Used by the search functions, among others.  Very similar
   to _rl_read_mbchar. */
int
_rl_read_mbstring (int first, char *mb, int mlen)
{
  int i, c, n;
  mbstate_t ps;

  c = first;
  memset (mb, 0, mlen);
  for (i = 0; c >= 0 && i < mlen; i++)
    {
      mb[i] = (char)c;
      memset (&ps, 0, sizeof (mbstate_t));
      n = _rl_get_char_len (mb, &ps);
      if (n == -2)
	{
	  /* Read more for multibyte character */
	  RL_SETSTATE (RL_STATE_MOREINPUT);
	  c = rl_read_key ();
	  RL_UNSETSTATE (RL_STATE_MOREINPUT);
	}
      else
	break;
    }
  return c;
}
#endif /* HANDLE_MULTIBYTE */
