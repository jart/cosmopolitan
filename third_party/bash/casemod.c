/* casemod.c -- functions to change case of strings */

/* Copyright (C) 2008-2020 Free Software Foundation, Inc.

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

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "stdc.h"

#include "bashansi.h"
#include "bashintl.h"
#include "bashtypes.h"

#include <stdio.h>
#include <ctype.h>
#include "xmalloc.h"

#include "shmbchar.h"
#include "shmbutil.h"
#include "chartypes.h"
#include "typemax.h"

#include "strmatch.h"

#define _to_wupper(wc)	(iswlower (wc) ? towupper (wc) : (wc))
#define _to_wlower(wc)	(iswupper (wc) ? towlower (wc) : (wc))

#if !defined (HANDLE_MULTIBYTE)
#  define cval(s, i, l)	((s)[(i)])
#  define iswalnum(c)	(isalnum(c))
#  define TOGGLE(x)	(ISUPPER (x) ? tolower ((unsigned char)x) : (TOUPPER (x)))
#else
#  define TOGGLE(x)	(iswupper (x) ? towlower (x) : (_to_wupper(x)))
#endif

/* These must agree with the defines in externs.h */
#define CASE_NOOP	0x0000
#define CASE_LOWER	0x0001
#define CASE_UPPER	0x0002
#define CASE_CAPITALIZE	0x0004
#define CASE_UNCAP	0x0008
#define CASE_TOGGLE	0x0010
#define CASE_TOGGLEALL	0x0020
#define CASE_UPFIRST	0x0040
#define CASE_LOWFIRST	0x0080

#define CASE_USEWORDS	0x1000		/* modify behavior to act on words in passed string */

extern char *substring PARAMS((char *, int, int));

#ifndef UCHAR_MAX
#  define UCHAR_MAX	TYPE_MAXIMUM(unsigned char)
#endif

#if defined (HANDLE_MULTIBYTE)
static wchar_t
cval (s, i, l)
     char *s;
     int i, l;
{
  size_t tmp;
  wchar_t wc;
  mbstate_t mps;  

  if (MB_CUR_MAX == 1 || is_basic (s[i]))
    return ((wchar_t)s[i]);
  if (i >= (l - 1))
    return ((wchar_t)s[i]);
  memset (&mps, 0, sizeof (mbstate_t));
  tmp = mbrtowc (&wc, s + i, l - i, &mps);
  if (MB_INVALIDCH (tmp) || MB_NULLWCH (tmp))
    return ((wchar_t)s[i]);
  return wc;  
}
#endif

/* Modify the case of characters in STRING matching PAT based on the value of
   FLAGS.  If PAT is null, modify the case of each character */
char *
sh_modcase (string, pat, flags)
     const char *string;
     char *pat;
     int flags;
{
  int start, next, end, retind;
  int inword, c, nc, nop, match, usewords;
  char *ret, *s;
  wchar_t wc;
  int mb_cur_max;
#if defined (HANDLE_MULTIBYTE)
  wchar_t nwc;
  char mb[MB_LEN_MAX+1];
  int mlen;
  size_t m;
  mbstate_t state;
#endif

  if (string == 0 || *string == 0)
    {
      ret = (char *)xmalloc (1);
      ret[0] = '\0';
      return ret;
    }

#if defined (HANDLE_MULTIBYTE)
  memset (&state, 0, sizeof (mbstate_t));
#endif

  start = 0;
  end = strlen (string);
  mb_cur_max = MB_CUR_MAX;

  ret = (char *)xmalloc (2*end + 1);
  retind = 0;

  /* See if we are supposed to split on alphanumerics and operate on each word */
  usewords = (flags & CASE_USEWORDS);
  flags &= ~CASE_USEWORDS;

  inword = 0;
  while (start < end)
    {
      wc = cval ((char *)string, start, end);

      if (iswalnum (wc) == 0)
	inword = 0;

      if (pat)
	{
	  next = start;
	  ADVANCE_CHAR (string, end, next);
	  s = substring ((char *)string, start, next);
	  match = strmatch (pat, s, FNM_EXTMATCH) != FNM_NOMATCH;
	  free (s);
	  if (match == 0)
            {
              /* copy unmatched portion */
              memcpy (ret + retind, string + start, next - start);
              retind += next - start;
              start = next;
              inword = 1;
              continue;
            }
	}

      /* XXX - for now, the toggling operators work on the individual
	 words in the string, breaking on alphanumerics.  Should I
	 leave the capitalization operators to do that also? */
      if (flags == CASE_CAPITALIZE)
	{
	  if (usewords)
	    nop = inword ? CASE_LOWER : CASE_UPPER;
	  else
	    nop = (start > 0) ? CASE_LOWER : CASE_UPPER;
	  inword = 1;
	}
      else if (flags == CASE_UNCAP)
	{
	  if (usewords)
	    nop = inword ? CASE_UPPER : CASE_LOWER;
	  else
	    nop = (start > 0) ? CASE_UPPER : CASE_LOWER;
	  inword = 1;
	}
      else if (flags == CASE_UPFIRST)
 	{
 	  if (usewords)
	    nop = inword ? CASE_NOOP : CASE_UPPER;
	  else
	    nop = (start > 0) ? CASE_NOOP : CASE_UPPER;
 	  inword = 1;
 	}
      else if (flags == CASE_LOWFIRST)
 	{
 	  if (usewords)
	    nop = inword ? CASE_NOOP : CASE_LOWER;
	  else
	    nop = (start > 0) ? CASE_NOOP : CASE_LOWER;
 	  inword = 1;
 	}
      else if (flags == CASE_TOGGLE)
	{
	  nop = inword ? CASE_NOOP : CASE_TOGGLE;
	  inword = 1;
	}
      else
	nop = flags;

      /* Can't short-circuit, some locales have multibyte upper and lower
	 case equivalents of single-byte ascii characters (e.g., Turkish) */
      if (mb_cur_max == 1)
	{
singlebyte:
	  switch (nop)
	    {
	    default:
	    case CASE_NOOP:  nc = wc; break;
	    case CASE_UPPER:  nc = TOUPPER (wc); break;
	    case CASE_LOWER:  nc = TOLOWER (wc); break;
	    case CASE_TOGGLEALL:
	    case CASE_TOGGLE: nc = TOGGLE (wc); break;
	    }
	  ret[retind++] = nc;
	}
#if defined (HANDLE_MULTIBYTE)
      else
	{
	  m = mbrtowc (&wc, string + start, end - start, &state);
	  /* Have to go through wide case conversion even for single-byte
	     chars, to accommodate single-byte characters where the
	     corresponding upper or lower case equivalent is multibyte. */
	  if (MB_INVALIDCH (m))
	    {
	      wc = (unsigned char)string[start];
	      goto singlebyte;
	    }
	  else if (MB_NULLWCH (m))
	    wc = L'\0';
	  switch (nop)
	    {
	    default:
	    case CASE_NOOP:  nwc = wc; break;
	    case CASE_UPPER:  nwc = _to_wupper (wc); break;
	    case CASE_LOWER:  nwc = _to_wlower (wc); break;
	    case CASE_TOGGLEALL:
	    case CASE_TOGGLE: nwc = TOGGLE (wc); break;
	    }

	  /* We don't have to convert `wide' characters that are in the
	     unsigned char range back to single-byte `multibyte' characters. */
	  if ((int)nwc <= UCHAR_MAX && is_basic ((int)nwc))
	    ret[retind++] = nwc;
	  else
	    {
	      mlen = wcrtomb (mb, nwc, &state);
	      if (mlen > 0)
		mb[mlen] = '\0';
	      /* Don't assume the same width */
	      strncpy (ret + retind, mb, mlen);
	      retind += mlen;
	    }
	}
#endif

      ADVANCE_CHAR (string, end, start);
    }

  ret[retind] = '\0';
  return ret;
}
