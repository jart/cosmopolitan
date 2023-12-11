/* timeval.c - functions to perform operations on struct timevals */

/* Copyright (C) 1999 Free Software Foundation, Inc.

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

#if defined (HAVE_TIMEVAL)

#include <sys/types.h>
#include "posixtime.h"

#include "bashintl.h"
#include "stdc.h"

#ifndef locale_decpoint
extern int locale_decpoint PARAMS((void));
#endif

#include <stdio.h>

struct timeval *
difftimeval (d, t1, t2)
     struct timeval *d, *t1, *t2;
{
  d->tv_sec = t2->tv_sec - t1->tv_sec;
  d->tv_usec = t2->tv_usec - t1->tv_usec;
  if (d->tv_usec < 0)
    {
      d->tv_usec += 1000000;
      d->tv_sec -= 1;
      if (d->tv_sec < 0)		/* ??? -- BSD/OS does this */
	{
	  d->tv_sec = 0;
	  d->tv_usec = 0;
	}
    }
  return d;
}

struct timeval *
addtimeval (d, t1, t2)
     struct timeval *d, *t1, *t2;
{
  d->tv_sec = t1->tv_sec + t2->tv_sec;
  d->tv_usec = t1->tv_usec + t2->tv_usec;
  if (d->tv_usec >= 1000000)
    {
      d->tv_usec -= 1000000;
      d->tv_sec += 1;
    }
  return d;
}

struct timeval *
multimeval (d, m)
     struct timeval *d;
     int m;
{
  time_t t;

  t = d->tv_usec * m;
  d->tv_sec = d->tv_sec * m + t / 1000000;
  d->tv_usec = t % 1000000;
  return d;
}

struct timeval *
divtimeval (d, m)
     struct timeval *d;
     int m;
{
  time_t t;

  t = d->tv_sec;
  d->tv_sec = t / m;
  d->tv_usec = (d->tv_usec + 1000000 * (t % m)) / m;
  return d;
}
  
/* Do "cpu = ((user + sys) * 10000) / real;" with timevals.
   Barely-tested code from Deven T. Corzine <deven@ties.org>. */
int
timeval_to_cpu (rt, ut, st)
     struct timeval *rt, *ut, *st;	/* real, user, sys */
{
  struct timeval t1, t2;
  register int i;

  addtimeval (&t1, ut, st);
  t2.tv_sec = rt->tv_sec;
  t2.tv_usec = rt->tv_usec;

  for (i = 0; i < 6; i++)
    {
      if ((t1.tv_sec > 99999999) || (t2.tv_sec > 99999999))
	break;
      t1.tv_sec *= 10;
      t1.tv_sec += t1.tv_usec / 100000;
      t1.tv_usec *= 10;
      t1.tv_usec %= 1000000;
      t2.tv_sec *= 10;
      t2.tv_sec += t2.tv_usec / 100000;
      t2.tv_usec *= 10;
      t2.tv_usec %= 1000000;
    }
  for (i = 0; i < 4; i++)
    {
      if (t1.tv_sec < 100000000)
	t1.tv_sec *= 10;
      else
	t2.tv_sec /= 10;
    }

  return ((t2.tv_sec == 0) ? 0 : t1.tv_sec / t2.tv_sec);
}  

/* Convert a pointer to a struct timeval to seconds and thousandths of a
   second, returning the values in *SP and *SFP, respectively.  This does
   rounding on the fractional part, not just truncation to three places. */
void
timeval_to_secs (tvp, sp, sfp)
     struct timeval *tvp;
     time_t *sp;
     int *sfp;
{
  int rest;

  *sp = tvp->tv_sec;

  *sfp = tvp->tv_usec % 1000000;	/* pretty much a no-op */
  rest = *sfp % 1000;
  *sfp = (*sfp * 1000) / 1000000;
  if (rest >= 500)
    *sfp += 1;

  /* Sanity check */
  if (*sfp >= 1000)
    {
      *sp += 1;
      *sfp -= 1000;
    }
}
  
/* Print the contents of a struct timeval * in a standard way to stdio
   stream FP.  */
void
print_timeval (fp, tvp)
     FILE *fp;
     struct timeval *tvp;
{
  time_t timestamp;
  long minutes;
  int seconds, seconds_fraction;

  timeval_to_secs (tvp, &timestamp, &seconds_fraction);

  minutes = timestamp / 60;
  seconds = timestamp % 60;

  fprintf (fp, "%ldm%d%c%03ds",  minutes, seconds, locale_decpoint (), seconds_fraction);
}

#endif /* HAVE_TIMEVAL */
