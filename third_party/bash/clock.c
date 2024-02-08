/* clock.c - operations on struct tms and clock_t's */

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

#if defined (HAVE_TIMES)

#include <sys/types.h>
#include "posixtime.h"

#if defined (HAVE_SYS_TIMES_H)
#  include <sys/times.h>
#endif

#include <stdio.h>
#include "stdc.h"

#include "bashintl.h"

#ifndef locale_decpoint
extern int locale_decpoint PARAMS((void));
#endif

extern long get_clk_tck PARAMS((void));

void
clock_t_to_secs (t, sp, sfp)
     clock_t t;
     time_t *sp;
     int *sfp;
{
  static long clk_tck = -1;

  if (clk_tck == -1)
    clk_tck = get_clk_tck ();

  *sfp = t % clk_tck;
  *sfp = (*sfp * 1000) / clk_tck;

  *sp = t / clk_tck;

  /* Sanity check */
  if (*sfp >= 1000)
    {
      *sp += 1;
      *sfp -= 1000;
    }
}

/* Print the time defined by a clock_t (returned by the `times' and `time'
   system calls) in a standard way to stdio stream FP.  This is scaled in
   terms of the value of CLK_TCK, which is what is returned by the
   `times' call. */
void
print_clock_t (fp, t)
     FILE *fp;
     clock_t t;
{
  time_t timestamp;
  long minutes;
  int seconds, seconds_fraction;

  clock_t_to_secs (t, &timestamp, &seconds_fraction);

  minutes = timestamp / 60;
  seconds = timestamp % 60;

  fprintf (fp, "%ldm%d%c%03ds",  minutes, seconds, locale_decpoint(), seconds_fraction);
}
#endif /* HAVE_TIMES */
