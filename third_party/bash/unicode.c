/* unicode.c - functions to convert unicode characters */

/* Copyright (C) 2010-2020 Free Software Foundation, Inc.

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

#if defined (HANDLE_MULTIBYTE)

#include "stdc.h"
#include <wchar.h>
#include "bashansi.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdio.h>
#include <limits.h>

#if HAVE_ICONV
#  include <iconv.h>
#endif

#include "xmalloc.h"

#ifndef USHORT_MAX
#  ifdef USHRT_MAX
#    define USHORT_MAX USHRT_MAX
#  else
#    define USHORT_MAX ((unsigned short) ~(unsigned short)0)
#  endif
#endif

#if !defined (STREQ)
#  define STREQ(a, b) ((a)[0] == (b)[0] && strcmp ((a), (b)) == 0)
#endif /* !STREQ */

#if defined (HAVE_LOCALE_CHARSET)
extern const char *locale_charset PARAMS((void));
#else
extern char *get_locale_var PARAMS((char *));
#endif

extern int locale_utf8locale;

static int u32init = 0;
static int utf8locale = 0;
#if defined (HAVE_ICONV)
static iconv_t localconv;
#endif

#ifndef HAVE_LOCALE_CHARSET
static char charsetbuf[40];

static char *
stub_charset ()
{
  char *locale, *s, *t;

  locale = get_locale_var ("LC_CTYPE");
  if (locale == 0 || *locale == 0)
    {
      strcpy (charsetbuf, "ASCII");
      return charsetbuf;
    }
  s = strrchr (locale, '.');
  if (s)
    {
      strncpy (charsetbuf, s+1, sizeof (charsetbuf) - 1);
      charsetbuf[sizeof (charsetbuf) - 1] = '\0';
      t = strchr (charsetbuf, '@');
      if (t)
	*t = 0;
      return charsetbuf;
    }
  strncpy (charsetbuf, locale, sizeof (charsetbuf) - 1);
  charsetbuf[sizeof (charsetbuf) - 1] = '\0';
  return charsetbuf;
}
#endif

void
u32reset ()
{
#if defined (HAVE_ICONV)
  if (u32init && localconv != (iconv_t)-1)
    {
      iconv_close (localconv);
      localconv = (iconv_t)-1;
    }
#endif
  u32init = 0;
  utf8locale = 0;
}

/* u32toascii ? */
int
u32tochar (x, s)
     unsigned long x;
     char *s;
{
  int l;

  l = (x <= UCHAR_MAX) ? 1 : ((x <= USHORT_MAX) ? 2 : 4);

  if (x <= UCHAR_MAX)
    s[0] = x & 0xFF;
  else if (x <= USHORT_MAX)	/* assume unsigned short = 16 bits */
    {
      s[0] = (x >> 8) & 0xFF;
      s[1] = x & 0xFF;
    }
  else
    {
      s[0] = (x >> 24) & 0xFF;
      s[1] = (x >> 16) & 0xFF;
      s[2] = (x >> 8) & 0xFF;
      s[3] = x & 0xFF;
    }
  s[l] = '\0';
  return l;  
}

int
u32tocesc (wc, s)
     u_bits32_t wc;
     char *s;
{
  int l;

  if (wc < 0x10000)
    l = sprintf (s, "\\u%04X", wc);
  else
    l = sprintf (s, "\\U%08X", wc);
  return l;
}

/* Convert unsigned 32-bit int to utf-8 character string */
int
u32toutf8 (wc, s)
     u_bits32_t wc;
     char *s;
{
  int l;

  if (wc < 0x0080)
    {
      s[0] = (char)wc;
      l = 1;
    }
  else if (wc < 0x0800)
    {
      s[0] = (wc >> 6) | 0xc0;
      s[1] = (wc & 0x3f) | 0x80;
      l = 2;
    }
  else if (wc < 0x10000)
    {
      /* Technically, we could return 0 here if 0xd800 <= wc <= 0x0dfff */
      s[0] = (wc >> 12) | 0xe0;
      s[1] = ((wc >> 6) & 0x3f) | 0x80;
      s[2] = (wc & 0x3f) | 0x80;
      l = 3;
    }
  else if (wc < 0x200000)
    {
      s[0] = (wc >> 18) | 0xf0;
      s[1] = ((wc >> 12) & 0x3f) | 0x80;
      s[2] = ((wc >>  6) & 0x3f) | 0x80;
      s[3] = (wc & 0x3f) | 0x80;
      l = 4;
    }
  /* Strictly speaking, UTF-8 doesn't have characters longer than 4 bytes */
  else if (wc < 0x04000000)
    {
      s[0] = (wc >> 24) | 0xf8;
      s[1] = ((wc >> 18) & 0x3f) | 0x80;
      s[2] = ((wc >> 12) & 0x3f) | 0x80;
      s[3] = ((wc >>  6) & 0x3f) | 0x80;
      s[4] = (wc & 0x3f) | 0x80;
      l = 5;
    }
  else if (wc < 0x080000000)
    {
      s[0] = (wc >> 30) | 0xfc;
      s[1] = ((wc >> 24) & 0x3f) | 0x80;
      s[2] = ((wc >> 18) & 0x3f) | 0x80;
      s[3] = ((wc >> 12) & 0x3f) | 0x80;
      s[4] = ((wc >>  6) & 0x3f) | 0x80;
      s[5] = (wc & 0x3f) | 0x80;
      l = 6;
    }
  else
    l = 0;

  s[l] = '\0';
  return l;
}

/* Convert a 32-bit unsigned int (unicode) to a UTF-16 string.  Rarely used,
   only if sizeof(wchar_t) == 2. */
int
u32toutf16 (c, s)
     u_bits32_t c;
     wchar_t *s;
{
  int l;

  l = 0;
  if (c < 0x0d800 || (c >= 0x0e000 && c <= 0x0ffff))
    {
      s[0] = (wchar_t) (c & 0xFFFF);
      l = 1;
    }
  else if (c >= 0x10000 && c <= 0x010ffff)
    {
      c -= 0x010000;
      s[0] = (wchar_t)((c >> 10) + 0xd800);
      s[1] = (wchar_t)((c & 0x3ff) + 0xdc00);
      l = 2;
    }
  s[l] = 0;
  return l;
}

/* convert a single unicode-32 character into a multibyte string and put the
   result in S, which must be large enough (at least max(10,MB_LEN_MAX) bytes) */
int
u32cconv (c, s)
     unsigned long c;
     char *s;
{
  wchar_t wc;
  wchar_t ws[3];
  int n;
#if HAVE_ICONV
  const char *charset;
  char obuf[25], *optr;
  size_t obytesleft;
  const char *iptr;
  size_t sn;
#endif

#if __STDC_ISO_10646__
  wc = c;
  if (sizeof (wchar_t) == 4 && c <= 0x7fffffff)
    n = wctomb (s, wc);
  else if (sizeof (wchar_t) == 2 && c <= 0x10ffff && u32toutf16 (c, ws))
    n = wcstombs (s, ws, MB_LEN_MAX);
  else
    n = -1;
  if (n != -1)
    return n;
#endif

#if HAVE_ICONV
  /* this is mostly from coreutils-8.5/lib/unicodeio.c */
  if (u32init == 0)
    {
      utf8locale = locale_utf8locale;
      localconv = (iconv_t)-1;
      if (utf8locale == 0)
	{
#if HAVE_LOCALE_CHARSET
	  charset = locale_charset ();
#elif HAVE_NL_LANGINFO
	  charset = nl_langinfo (CODESET);
#else
	  charset = stub_charset ();
#endif
	  localconv = iconv_open (charset, "UTF-8");
	  if (localconv == (iconv_t)-1)
	    /* We assume ASCII when presented with an unknown encoding. */
	    localconv = iconv_open ("ASCII", "UTF-8");
	}
      u32init = 1;
    }

  /* NL_LANGINFO and locale_charset used when setting locale_utf8locale */
  
  /* If we have a UTF-8 locale, convert to UTF-8 and return converted value. */
  n = u32toutf8 (c, s);
  if (utf8locale)
    return n;

  /* If the conversion is not supported, even the ASCII requested above, we
     bail now.  Currently we return the UTF-8 conversion.  We could return
     u32tocesc(). */
  if (localconv == (iconv_t)-1)
    return n;
    
  optr = obuf;
  obytesleft = sizeof (obuf);
  iptr = s;
  sn = n;

  iconv (localconv, NULL, NULL, NULL, NULL);

  if (iconv (localconv, (ICONV_CONST char **)&iptr, &sn, &optr, &obytesleft) == (size_t)-1)
    {
      /* You get ISO C99 escape sequences if iconv fails */      
      n = u32tocesc (c, s);
      return n;
    }

  *optr = '\0';

  /* number of chars to be copied is optr - obuf if we want to do bounds
     checking */
  strcpy (s, obuf);
  return (optr - obuf);
#endif	/* HAVE_ICONV */

  if (locale_utf8locale)
    n = u32toutf8 (c, s);
  else
    n = u32tocesc (c, s);	/* fallback is ISO C99 escape sequences */
  return n;
}
#else
void
u32reset ()
{
}
#endif /* HANDLE_MULTIBYTE */
