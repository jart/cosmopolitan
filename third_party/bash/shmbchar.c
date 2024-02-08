/* Copyright (C) 2001, 2006, 2009, 2010, 2012, 2015-2018 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


#include "config.h"

#if defined (HANDLE_MULTIBYTE)
#include <stdlib.h>
#include <limits.h>

#include <errno.h>

#include "shmbutil.h"
#include "shmbchar.h"

#ifndef errno
extern int errno;
#endif

#if IS_BASIC_ASCII

/* Bit table of characters in the ISO C "basic character set".  */
const unsigned int is_basic_table [UCHAR_MAX / 32 + 1] =
{
  0x00001a00,           /* '\t' '\v' '\f' */
  0xffffffef,           /* ' '...'#' '%'...'?' */
  0xfffffffe,           /* 'A'...'Z' '[' '\\' ']' '^' '_' */
  0x7ffffffe            /* 'a'...'z' '{' '|' '}' '~' */
  /* The remaining bits are 0.  */
};

#endif /* IS_BASIC_ASCII */

extern int locale_utf8locale;

extern char *utf8_mbsmbchar (const char *);
extern int utf8_mblen (const char *, size_t);

/* Count the number of characters in S, counting multi-byte characters as a
   single character. */
size_t
mbstrlen (s)
     const char *s;
{
  size_t clen, nc;
  mbstate_t mbs = { 0 }, mbsbak = { 0 };
  int f, mb_cur_max;

  nc = 0;
  mb_cur_max = MB_CUR_MAX;
  while (*s && (clen = (f = is_basic (*s)) ? 1 : mbrlen(s, mb_cur_max, &mbs)) != 0)
    {
      if (MB_INVALIDCH(clen))
	{
	  clen = 1;	/* assume single byte */
	  mbs = mbsbak;
	}

      if (f == 0)
	mbsbak = mbs;

      s += clen;
      nc++;
    }
  return nc;
}

/* Return pointer to first multibyte char in S, or NULL if none. */
/* XXX - if we know that the locale is UTF-8, we can just check whether or
   not any byte has the eighth bit turned on */
char *
mbsmbchar (s)
     const char *s;
{
  char *t;
  size_t clen;
  mbstate_t mbs = { 0 };
  int mb_cur_max;

  if (locale_utf8locale)
    return (utf8_mbsmbchar (s));	/* XXX */

  mb_cur_max = MB_CUR_MAX;
  for (t = (char *)s; *t; t++)
    {
      if (is_basic (*t))
	continue;

      if (locale_utf8locale)		/* not used if above code active */
	clen = utf8_mblen (t, mb_cur_max);
      else
	clen = mbrlen (t, mb_cur_max, &mbs);

      if (clen == 0)
        return 0;
      if (MB_INVALIDCH(clen))
	continue;

      if (clen > 1)
	return t;
    }
  return 0;
}

int
sh_mbsnlen(src, srclen, maxlen)
     const char *src;
     size_t srclen;
     int maxlen;
{
  int count;
  int sind;
  DECLARE_MBSTATE;

  for (sind = count = 0; src[sind]; )
    {
      count++;		/* number of multibyte characters */
      ADVANCE_CHAR (src, srclen, sind);
      if (sind > maxlen)
        break;
    }

  return count;
}
#endif
