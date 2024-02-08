/* timers - functions to manage shell timers */

/* Copyright (C) 2021 Free Software Foundation, Inc.

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
#include "posixtime.h"

#if defined (HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined (HAVE_SELECT)
#  include "posixselect.h"
#  include "stat-time.h"
#endif

#include "sig.h"
#include "bashjmp.h"
#include "xmalloc.h"

#include "timer.h"

#include <errno.h>
#if !defined (errno)
extern int errno;
#endif /* !errno */

#ifndef FREE
#define FREE(s)  do { if (s) free (s); } while (0)
#endif

extern unsigned int falarm (unsigned int, unsigned int);

static void shtimer_zero (sh_timer *);

static void
shtimer_zero (sh_timer *t)
{
  t->tmout.tv_sec = 0;
  t->tmout.tv_usec = 0;

  t->fd = -1;
  t->flags = t->alrmflag = 0;

  t->alrm_handler = t->old_handler = 0;

  memset (t->jmpenv, '\0', sizeof (t->jmpenv));

  t->tm_handler = 0;
  t->data = 0;
}

sh_timer *
shtimer_alloc (void)
{
  sh_timer *t;

  t = (sh_timer *)xmalloc (sizeof (sh_timer));
  shtimer_zero (t);
  return t;
}

void
shtimer_flush (sh_timer *t)
{
  /* The caller can manage t->data arbitrarily as long as it frees and sets
     t->data to 0 before calling this function. Otherwise, we do what we can
     to avoid memleaks. */
  FREE (t->data);
  shtimer_zero (t);
}

void
shtimer_dispose (sh_timer *t)
{
  free (t);
}

/* We keep the timer as an offset into the future from the time it's set. */
void
shtimer_set (sh_timer *t, time_t sec, long usec)
{
  struct timeval now;

  if (t->flags & SHTIMER_ALARM)
    {
      t->alrmflag = 0;		/* just paranoia */
      t->old_handler = set_signal_handler (SIGALRM, t->alrm_handler);
      t->flags |= SHTIMER_SIGSET;
      falarm (t->tmout.tv_sec = sec, t->tmout.tv_usec = usec);
      t->flags |= SHTIMER_ALRMSET;
      return;
    }

  if (gettimeofday (&now, 0) < 0)
    timerclear (&now);

  t->tmout.tv_sec = now.tv_sec + sec;
  t->tmout.tv_usec = now.tv_usec + usec;
  if (t->tmout.tv_usec > USEC_PER_SEC)
    {
      t->tmout.tv_sec++;
      t->tmout.tv_usec -= USEC_PER_SEC;
    }
}

void
shtimer_unset (sh_timer *t)
{
  t->tmout.tv_sec = 0;
  t->tmout.tv_usec = 0;

  if (t->flags & SHTIMER_ALARM)
    {
      t->alrmflag = 0;
      if (t->flags & SHTIMER_ALRMSET)
	falarm (0, 0);
      if (t->old_handler && (t->flags & SHTIMER_SIGSET))
	{
	  set_signal_handler (SIGALRM, t->old_handler);
	  t->flags &= ~SHTIMER_SIGSET;
	  t->old_handler = 0;
	}
    }
}

void
shtimer_cleanup (sh_timer *t)
{
  shtimer_unset (t);
}

void
shtimer_clear (sh_timer *t)
{
  shtimer_unset (t);
  shtimer_dispose (t);
}

int
shtimer_chktimeout (sh_timer *t)
{
  struct timeval now;
  int r;

  /* Use the flag to avoid returning sigalrm_seen here */
  if (t->flags & SHTIMER_ALARM)
    return t->alrmflag;

  /* Could check a flag for this */
  if (t->tmout.tv_sec == 0 && t->tmout.tv_usec == 0)
    return 0;

  if (gettimeofday (&now, 0) < 0)
    return 0;
  r = ((now.tv_sec > t->tmout.tv_sec) ||
	(now.tv_sec == t->tmout.tv_sec && now.tv_usec >= t->tmout.tv_usec));

  return r;
}

#if defined (HAVE_SELECT) || defined (HAVE_PSELECT)
int
shtimer_select (sh_timer *t)
{
  int r, nfd;
  sigset_t blocked_sigs, prevmask;
  struct timeval now, tv;
  fd_set readfds;
#if defined (HAVE_PSELECT)
  struct timespec ts;
#endif

  /* We don't want a SIGCHLD to interrupt this */
  sigemptyset (&blocked_sigs);
#  if defined (SIGCHLD)
  sigaddset (&blocked_sigs, SIGCHLD);
#  endif

  if (gettimeofday (&now, 0) < 0)
    {
      if (t->flags & SHTIMER_LONGJMP)
	sh_longjmp (t->jmpenv, 1);
      else
	return -1;
    }

  /* If the timer has already expired, return immediately */    
  if ((now.tv_sec > t->tmout.tv_sec) ||
	(now.tv_sec == t->tmout.tv_sec && now.tv_usec >= t->tmout.tv_usec))
    {
      if (t->flags & SHTIMER_LONGJMP)
	sh_longjmp (t->jmpenv, 1);
      else if (t->tm_handler)
	return ((*t->tm_handler) (t));
      else
	return 0;
    }

  /* compute timeout */
  tv.tv_sec = t->tmout.tv_sec - now.tv_sec;
  tv.tv_usec = t->tmout.tv_usec - now.tv_usec;
  if (tv.tv_usec < 0)
    {
      tv.tv_sec--;
      tv.tv_usec += USEC_PER_SEC;
    }

#if defined (HAVE_PSELECT)
  ts.tv_sec = tv.tv_sec;
  ts.tv_nsec = tv.tv_usec * 1000;
#else
  sigemptyset (&prevmask);
#endif /* !HAVE_PSELECT */

  nfd = (t->fd >= 0) ? t->fd + 1 : 0;
  FD_ZERO (&readfds);
  if (t->fd >= 0)
    FD_SET (t->fd, &readfds);

#if defined (HAVE_PSELECT)
  r = pselect(nfd, &readfds, (fd_set *)0, (fd_set *)0, &ts, &blocked_sigs);
#else
  sigprocmask (SIG_SETMASK, &blocked_sigs, &prevmask);
  r = select(nfd, &readfds, (fd_set *)0, (fd_set *)0, &tv);
  sigprocmask (SIG_SETMASK, &prevmask, NULL);
#endif

  if (r < 0)
    return r;		/* caller will handle */
  else if (r == 0 && (t->flags & SHTIMER_LONGJMP))
    sh_longjmp (t->jmpenv, 1);
  else if (r == 0 && t->tm_handler)
    return ((*t->tm_handler) (t));
  else
    return r;
}
#endif /* !HAVE_TIMEVAL || !HAVE_SELECT */

int
shtimer_alrm (sh_timer *t)
{
  return 0;
}
