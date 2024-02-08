/* posixtime.h -- wrapper for time.h, sys/times.h mess. */

/* Copyright (C) 1999-2021 Free Software Foundation, Inc.

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

#ifndef _POSIXTIME_H_
#define _POSIXTIME_H_

/* include this after config.h */
/* Some systems require this, mostly for the definition of `struct timezone'.
   For example, Dynix/ptx has that definition in <time.h> rather than
      sys/time.h */
#if defined (HAVE_SYS_TIME_H)
#  include <sys/time.h>
#endif
#include <time.h>

#if !defined (HAVE_SYSCONF) || !defined (_SC_CLK_TCK)
#  if !defined (CLK_TCK)
#    if defined (HZ)
#      define CLK_TCK	HZ
#    else
#      define CLK_TCK	60			/* 60HZ */
#    endif
#  endif /* !CLK_TCK */
#endif /* !HAVE_SYSCONF && !_SC_CLK_TCK */

#if !HAVE_TIMEVAL
struct timeval
{
  time_t tv_sec;
  long int tv_usec;
};
#endif

#if !HAVE_GETTIMEOFDAY
extern int gettimeofday PARAMS((struct timeval *, void *));
#endif

/* These exist on BSD systems, at least. */
#if !defined (timerclear)
#  define timerclear(tvp)	do { (tvp)->tv_sec = 0; (tvp)->tv_usec = 0; } while (0)
#endif
#if !defined (timerisset)
#  define timerisset(tvp)	((tvp)->tv_sec || (tvp)->tv_usec)
#endif
#if !defined (timercmp)
#  define timercmp(a, b, CMP) \
	(((a)->tv_sec == (b)->tv_sec) ? ((a)->tv_usec CMP (b)->tv_usec) \
				      : ((a)->tv_sec CMP (b)->tv_sec))
#endif

/* These are non-standard. */
#if !defined (timerisunset)
#  define timerisunset(tvp)	((tvp)->tv_sec == 0 && (tvp)->tv_usec == 0)
#endif
#if !defined (timerset)
#  define timerset(tvp, s, u)	do { tvp->tv_sec = s; tvp->tv_usec = u; } while (0)
#endif

#ifndef TIMEVAL_TO_TIMESPEC
#  define TIMEVAL_TO_TIMESPEC(tv, ts) \
  do { \
    (ts)->tv_sec = (tv)->tv_sec; \
    (ts)->tv_nsec = (tv)->tv_usec * 1000; \
  } while (0)
#endif

#endif /* _POSIXTIME_H_ */
