/* fmtulong.c -- Convert unsigned long int to string. */

/* Copyright (C) 1998-2011 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#if defined (HAVE_LIMITS_H)
#  include <limits.h>
#endif

#include "bashansi.h"
#ifdef HAVE_STDDEF_H
#  include <stddef.h>
#endif

#ifdef HAVE_STDINT_H
#  include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#  include <inttypes.h>
#endif
#include "chartypes.h"
#include <errno.h>

#include "bashintl.h"

#include "stdc.h"

#include "typemax.h"

#ifndef errno
extern int errno;
#endif

#define x_digs  "0123456789abcdef"
#define X_digs  "0123456789ABCDEF"

/* XXX -- assumes uppercase letters, lowercase letters, and digits are
   contiguous */
#define FMTCHAR(x) \
  ((x) < 10) ? (x) + '0' \
	     : (((x) < 36) ? (x) - 10 + 'a' \
			   : (((x) < 62) ? (x) - 36 + 'A' \
					 : (((x) == 62) ? '@' : '_')))

#ifndef FL_PREFIX
#  define FL_PREFIX	0x01	/* add 0x, 0X, or 0 prefix as appropriate */
#  define FL_ADDBASE	0x02	/* add base# prefix to converted value */
#  define FL_HEXUPPER	0x04	/* use uppercase when converting to hex */
#  define FL_UNSIGNED	0x08	/* don't add any sign */
#endif

#ifndef LONG
#  define LONG	long
#  define UNSIGNED_LONG unsigned long
#endif

/* `unsigned long' (or unsigned long long) to string conversion for a given
   base.  The caller passes the output buffer and the size.  This should
   check for buffer underflow, but currently does not. */
char *
fmtulong (ui, base, buf, len, flags)
     UNSIGNED_LONG ui;
     int base;
     char *buf;
     size_t len;
     int flags;
{
  char *p;
  int sign;
  LONG si;

  if (base == 0)
    base = 10;

  if (base < 2 || base > 64)
    {
#if 1
      /* XXX - truncation possible with long translation */
      strncpy (buf, _("invalid base"), len - 1);
      buf[len-1] = '\0';
      errno = EINVAL;
      return (p = buf);
#else
      base = 10;
#endif
    }

  sign = 0;
  if ((flags & FL_UNSIGNED) == 0 && (LONG)ui < 0)
    {
      ui = -ui;
      sign = '-';
    }

  p = buf + len - 2;
  p[1] = '\0';

  /* handle common cases explicitly */
  switch (base)
    {
    case 10:
      if (ui < 10)
	{
	  *p-- = TOCHAR (ui);
	  break;
	}
      /* Favor signed arithmetic over unsigned arithmetic; it is faster on
	 many machines. */
      if ((LONG)ui < 0)
	{
	  *p-- = TOCHAR (ui % 10);
	  si = ui / 10;
	}
      else
        si = ui;
      do
	*p-- = TOCHAR (si % 10);
      while (si /= 10);
      break;

    case 8:
      do
	*p-- = TOCHAR (ui & 7);
      while (ui >>= 3);
      break;

    case 16:
      do
	*p-- = (flags & FL_HEXUPPER) ? X_digs[ui & 15] : x_digs[ui & 15];
      while (ui >>= 4);
      break;

    case 2:
      do
	*p-- = TOCHAR (ui & 1);
      while (ui >>= 1);
      break;

    default:
      do
	*p-- = FMTCHAR (ui % base);
      while (ui /= base);
      break;
    }

  if ((flags & FL_PREFIX) && (base == 8 || base == 16))
    {
      if (base == 16)
	{
	  *p-- = (flags & FL_HEXUPPER) ? 'X' : 'x';
	  *p-- = '0';
	}
      else if (p[1] != '0')
	*p-- = '0';
    }
  else if ((flags & FL_ADDBASE) && base != 10)
    {
      *p-- = '#';
      *p-- = TOCHAR (base % 10);
      if (base > 10)
        *p-- = TOCHAR (base / 10);
    }

  if (sign)
    *p-- = '-';

  return (p + 1);
}
