/* mbschr.c - strchr(3) that handles multibyte characters. */

/* Copyright (C) 2002 Free Software Foundation, Inc.

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

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#include "bashansi.h"
#include "shmbutil.h"

extern int locale_mb_cur_max;
extern int locale_utf8locale;

#undef mbschr

extern char *utf8_mbschr (const char *, int);	/* XXX */

/* In some locales, the non-first byte of some multibyte characters have
   the same value as some ascii character.  Faced with these strings, a
   legacy strchr() might return the wrong value. */

char *
#if defined (PROTOTYPES)
mbschr (const char *s, int c)
#else
mbschr (s, c)
     const char *s;
     int c;
#endif
{
#if HANDLE_MULTIBYTE
  char *pos;
  mbstate_t state;
  size_t strlength, mblength;

  if (locale_utf8locale && c < 0x80)
    return (utf8_mbschr (s, c));		/* XXX */

  /* The locale encodings with said weird property are BIG5, BIG5-HKSCS,
     GBK, GB18030, SHIFT_JIS, and JOHAB.  They exhibit the problem only
     when c >= 0x30.  We can therefore use the faster bytewise search if
     c <= 0x30. */
  if ((unsigned char)c >= '0' && locale_mb_cur_max > 1)
    {
      pos = (char *)s;
      memset (&state, '\0', sizeof(mbstate_t));
      strlength = strlen (s);

      while (strlength > 0)
	{
	  if (is_basic (*pos))
	    mblength = 1;
	  else
	    {
	      mblength = mbrlen (pos, strlength, &state);
	      if (mblength == (size_t)-2 || mblength == (size_t)-1 || mblength == (size_t)0)
	        mblength = 1;
	    }

	  if (mblength == 1 && c == (unsigned char)*pos)
	    return pos;

	  strlength -= mblength;
	  pos += mblength;
	}

      return ((char *)NULL);
    }
  else
#endif
  return (strchr (s, c));
}
