/* strtrans.c - Translate and untranslate strings with ANSI-C escape sequences. */

/* Copyright (C) 2000-2015 Free Software Foundation, Inc.

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

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include <stdio.h>
#include "chartypes.h"

#include "shell.h"

#include "shmbchar.h"
#include "shmbutil.h"

#ifdef ESC
#undef ESC
#endif
#define ESC '\033'	/* ASCII */

/* Convert STRING by expanding the escape sequences specified by the
   ANSI C standard.  If SAWC is non-null, recognize `\c' and use that
   as a string terminator.  If we see \c, set *SAWC to 1 before
   returning.  LEN is the length of STRING.  If (FLAGS&1) is non-zero,
   that we're translating a string for `echo -e', and therefore should not
   treat a single quote as a character that may be escaped with a backslash.
   If (FLAGS&2) is non-zero, we're expanding for the parser and want to
   quote CTLESC and CTLNUL with CTLESC.  If (flags&4) is non-zero, we want
   to remove the backslash before any unrecognized escape sequence. */
char *
ansicstr (string, len, flags, sawc, rlen)
     char *string;
     int len, flags, *sawc, *rlen;
{
  int c, temp;
  char *ret, *r, *s;
  unsigned long v;
  size_t clen;
  int b, mb_cur_max;
#if defined (HANDLE_MULTIBYTE)
  wchar_t wc;
#endif

  if (string == 0 || *string == '\0')
    return ((char *)NULL);

  mb_cur_max = MB_CUR_MAX;
#if defined (HANDLE_MULTIBYTE)
  temp = 4*len + 4;
  if (temp < 12)
    temp = 12;				/* ensure enough for eventual u32cesc */
  ret = (char *)xmalloc (temp);
#else
  ret = (char *)xmalloc (2*len + 1);	/* 2*len for possible CTLESC */
#endif
  for (r = ret, s = string; s && *s; )
    {
      c = *s++;
      if (c != '\\' || *s == '\0')
	{
	  clen = 1;
#if defined (HANDLE_MULTIBYTE)
	  if ((locale_utf8locale && (c & 0x80)) ||
	      (locale_utf8locale == 0 && mb_cur_max > 0 && is_basic (c) == 0))
	    {
	      clen = mbrtowc (&wc, s - 1, mb_cur_max, 0);
	      if (MB_INVALIDCH (clen))
		clen = 1;
	    }
#endif
	  *r++ = c;
	  for (--clen; clen > 0; clen--)
	    *r++ = *s++;
	}
      else
	{
	  switch (c = *s++)
	    {
#if defined (__STDC__)
	    case 'a': c = '\a'; break;
	    case 'v': c = '\v'; break;
#else
	    case 'a': c = (int) 0x07; break;
	    case 'v': c = (int) 0x0B; break;
#endif
	    case 'b': c = '\b'; break;
	    case 'e': case 'E':		/* ESC -- non-ANSI */
	      c = ESC; break;
	    case 'f': c = '\f'; break;
	    case 'n': c = '\n'; break;
	    case 'r': c = '\r'; break;
	    case 't': c = '\t'; break;
	    case '1': case '2': case '3':
	    case '4': case '5': case '6':
	    case '7':
#if 1
	      if (flags & 1)
		{
		  *r++ = '\\';
		  break;
		}
	    /*FALLTHROUGH*/
#endif
	    case '0':
	      /* If (FLAGS & 1), we're translating a string for echo -e (or
		 the equivalent xpg_echo option), so we obey the SUSv3/
		 POSIX-2001 requirement and accept 0-3 octal digits after
		 a leading `0'. */
	      temp = 2 + ((flags & 1) && (c == '0'));
	      for (c -= '0'; ISOCTAL (*s) && temp--; s++)
		c = (c * 8) + OCTVALUE (*s);
	      c &= 0xFF;
	      break;
	    case 'x':			/* Hex digit -- non-ANSI */
	      if ((flags & 2) && *s == '{')
		{
		  flags |= 16;		/* internal flag value */
		  s++;
		}
	      /* Consume at least two hex characters */
	      for (temp = 2, c = 0; ISXDIGIT ((unsigned char)*s) && temp--; s++)
		c = (c * 16) + HEXVALUE (*s);
	      /* DGK says that after a `\x{' ksh93 consumes ISXDIGIT chars
		 until a non-xdigit or `}', so potentially more than two
		 chars are consumed. */
	      if (flags & 16)
		{
		  for ( ; ISXDIGIT ((unsigned char)*s); s++)
		    c = (c * 16) + HEXVALUE (*s);
		  flags &= ~16;
		  if (*s == '}')
		    s++;
	        }
	      /* \x followed by non-hex digits is passed through unchanged */
	      else if (temp == 2)
		{
		  *r++ = '\\';
		  c = 'x';
		}
	      c &= 0xFF;
	      break;
#if defined (HANDLE_MULTIBYTE)
	    case 'u':
	    case 'U':
	      temp = (c == 'u') ? 4 : 8;	/* \uNNNN \UNNNNNNNN */
	      for (v = 0; ISXDIGIT ((unsigned char)*s) && temp--; s++)
		v = (v * 16) + HEXVALUE (*s);
	      if (temp == ((c == 'u') ? 4 : 8))
		{
		  *r++ = '\\';	/* c remains unchanged */
		  break;
		}
	      else if (v <= 0x7f)	/* <= 0x7f translates directly */
		{
		  c = v;
		  break;
		}
	      else
		{
		  temp = u32cconv (v, r);
		  r += temp;
		  continue;
		}
#endif
	    case '\\':
	      break;
	    case '\'': case '"': case '?':
	      if (flags & 1)
		*r++ = '\\';
	      break;
	    case 'c':
	      if (sawc)
		{
		  *sawc = 1;
		  *r = '\0';
		  if (rlen)
		    *rlen = r - ret;
		  return ret;
		}
	      else if ((flags & 1) == 0 && *s == 0)
		;		/* pass \c through */
	      else if ((flags & 1) == 0 && (c = *s))
		{
		  s++;
		  if ((flags & 2) && c == '\\' && c == *s)
		    s++;	/* Posix requires $'\c\\' do backslash escaping */
		  c = TOCTRL(c);
		  break;
		}
		/*FALLTHROUGH*/
	    default:
		if ((flags & 4) == 0)
		  *r++ = '\\';
		break;
	    }
	  if ((flags & 2) && (c == CTLESC || c == CTLNUL))
	    *r++ = CTLESC;
	  *r++ = c;
	}
    }
  *r = '\0';
  if (rlen)
    *rlen = r - ret;
  return ret;
}

/* Take a string STR, possibly containing non-printing characters, and turn it
   into a $'...' ANSI-C style quoted string.  Returns a new string. */
char *
ansic_quote (str, flags, rlen)
     char *str;
     int flags, *rlen;
{
  char *r, *ret, *s;
  int l, rsize;
  unsigned char c;
  size_t clen;
  int b;
#if defined (HANDLE_MULTIBYTE)
  wchar_t wc;
#endif

  if (str == 0 || *str == 0)
    return ((char *)0);

  l = strlen (str);
  rsize = 4 * l + 4;
  r = ret = (char *)xmalloc (rsize);

  *r++ = '$';
  *r++ = '\'';

  for (s = str; c = *s; s++)
    {
      b = l = 1;		/* 1 == add backslash; 0 == no backslash */
      clen = 1;

      switch (c)
	{
	case ESC: c = 'E'; break;
#ifdef __STDC__
	case '\a': c = 'a'; break;
	case '\v': c = 'v'; break;
#else
	case 0x07: c = 'a'; break;
	case 0x0b: c = 'v'; break;
#endif

	case '\b': c = 'b'; break;
	case '\f': c = 'f'; break;
	case '\n': c = 'n'; break;
	case '\r': c = 'r'; break;
	case '\t': c = 't'; break;
	case '\\':
	case '\'':
	  break;
	default:
#if defined (HANDLE_MULTIBYTE)
	  b = is_basic (c);
	  /* XXX - clen comparison to 0 is dicey */
	  if ((b == 0 && ((clen = mbrtowc (&wc, s, MB_CUR_MAX, 0)) < 0 || MB_INVALIDCH (clen) || iswprint (wc) == 0)) ||
	      (b == 1 && ISPRINT (c) == 0))
#else
	  if (ISPRINT (c) == 0)
#endif
	    {
	      *r++ = '\\';
	      *r++ = TOCHAR ((c >> 6) & 07);
	      *r++ = TOCHAR ((c >> 3) & 07);
	      *r++ = TOCHAR (c & 07);
	      continue;
	    }
	  l = 0;
	  break;
	}
      if (b == 0 && clen == 0)
	break;

      if (l)
	*r++ = '\\';

      if (clen == 1)
	*r++ = c;
      else
	{
	  for (b = 0; b < (int)clen; b++)
	    *r++ = (unsigned char)s[b];
	  s += clen - 1;	/* -1 because of the increment above */
	}
    }

  *r++ = '\'';
  *r = '\0';
  if (rlen)
    *rlen = r - ret;
  return ret;
}

#if defined (HANDLE_MULTIBYTE)
int
ansic_wshouldquote (string)
     const char *string;
{
  const wchar_t *wcs;
  wchar_t wcc;
  wchar_t *wcstr = NULL;
  size_t slen;

  slen = mbstowcs (wcstr, string, 0);

  if (slen == (size_t)-1)
    return 1;

  wcstr = (wchar_t *)xmalloc (sizeof (wchar_t) * (slen + 1));
  mbstowcs (wcstr, string, slen + 1);

  for (wcs = wcstr; wcc = *wcs; wcs++)
    if (iswprint(wcc) == 0)
      {
	free (wcstr);
	return 1;
      }

  free (wcstr);
  return 0;
}
#endif

/* return 1 if we need to quote with $'...' because of non-printing chars. */
int
ansic_shouldquote (string)
     const char *string;
{
  const char *s;
  unsigned char c;

  if (string == 0)
    return 0;

  for (s = string; c = *s; s++)
    {
#if defined (HANDLE_MULTIBYTE)
      if (is_basic (c) == 0)
	return (ansic_wshouldquote (s));
#endif
      if (ISPRINT (c) == 0)
	return 1;
    }

  return 0;
}

/* $'...' ANSI-C expand the portion of STRING between START and END and
   return the result.  The result cannot be longer than the input string. */
char *
ansiexpand (string, start, end, lenp)
     char *string;
     int start, end, *lenp;
{
  char *temp, *t;
  int len, tlen;

  temp = (char *)xmalloc (end - start + 1);
  for (tlen = 0, len = start; len < end; )
    temp[tlen++] = string[len++];
  temp[tlen] = '\0';

  if (*temp)
    {
      t = ansicstr (temp, tlen, 2, (int *)NULL, lenp);
      free (temp);
      return (t);
    }
  else
    {
      if (lenp)
	*lenp = 0;
      return (temp);
    }
}
