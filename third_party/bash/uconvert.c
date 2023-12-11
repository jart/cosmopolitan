/* uconvert - convert string representations of decimal numbers into whole
	      number/fractional value pairs. */

/* Copyright (C) 2008,2009,2020 Free Software Foundation, Inc.

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

#include <stdio.h>
#include "chartypes.h"

#include "shell.h"
#include "builtins.h"

#define DECIMAL	'.'		/* XXX - should use locale */

#define RETURN(x) \
do { \
  if (ip) *ip = ipart * mult; \
  if (up) *up = upart; \
  if (ep) *ep = p; \
  return (x); \
} while (0)

/*
 * An incredibly simplistic floating point converter.
 */
static int multiplier[7] = { 1, 100000, 10000, 1000, 100, 10, 1 };

/* Take a decimal number int-part[.[micro-part]] and convert it to the whole
   and fractional portions.  The fractional portion is returned in
   millionths (micro); callers are responsible for multiplying appropriately.
   EP, if non-null, gets the address of the character where conversion stops.
   Return 1 if value converted; 0 if invalid integer for either whole or
   fractional parts. */
int
uconvert(s, ip, up, ep)
     char *s;
     long *ip, *up;
     char **ep;
{
  int n, mult;
  long ipart, upart;
  char *p;

  ipart = upart = 0;
  mult = 1;

  if (s && (*s == '-' || *s == '+'))
    {
      mult = (*s == '-') ? -1 : 1;
      p = s + 1;
    }
  else
    p = s;

  for ( ; p && *p; p++)
    {
      if (*p == DECIMAL)		/* decimal point */
	break;
      if (DIGIT(*p) == 0)
	RETURN(0);
      ipart = (ipart * 10) + (*p - '0');
    }

  if (p == 0 || *p == 0)	/* callers ensure p can never be 0; this is to shut up clang */
    RETURN(1);

  if (*p == DECIMAL)
    p++;

  /* Look for up to six digits past a decimal point. */
  for (n = 0; n < 6 && p[n]; n++)
    {
      if (DIGIT(p[n]) == 0)
	{
	  if (ep)
	    {
	      upart *= multiplier[n];
	      p += n;		/* To set EP */
	    }
	  RETURN(0);
	}
      upart = (upart * 10) + (p[n] - '0');
    }

  /* Now convert to millionths */
  upart *= multiplier[n];

  if (n == 6 && p[6] >= '5' && p[6] <= '9')
    upart++;			/* round up 1 */

  if (ep)
    {
      p += n;
      while (DIGIT(*p))
	p++;
    }

  RETURN(1);
}
