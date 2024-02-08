/* xmbsrtowcs.c -- replacement function for mbsrtowcs */

/* Copyright (C) 2002-2020 Free Software Foundation, Inc.

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

/* Ask for GNU extensions to get extern declaration for mbsnrtowcs if
   available via glibc. */
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
#endif

#include "config.h"

#include "bashansi.h"

/* <wchar.h>, <wctype.h> and <stdlib.h> are included in "shmbutil.h".
   If <wchar.h>, <wctype.h>, mbsrtowcs(), exist, HANDLE_MULTIBYTE
   is defined as 1. */
#include "shmbutil.h"

#if HANDLE_MULTIBYTE

#include <errno.h>
#if !defined (errno)
extern int errno;
#endif

#define WSBUF_INC 32

#ifndef FREE
#  define FREE(x)	do { if (x) free (x); } while (0)
#endif

#if ! HAVE_STRCHRNUL
extern char *strchrnul PARAMS((const char *, int));
#endif

/* On some locales (ex. ja_JP.sjis), mbsrtowc doesn't convert 0x5c to U<0x5c>.
   So, this function is made for converting 0x5c to U<0x5c>. */

static mbstate_t local_state;
static int local_state_use = 0;

size_t
xmbsrtowcs (dest, src, len, pstate)
    wchar_t *dest;
    const char **src;
    size_t len;
    mbstate_t *pstate;
{
  mbstate_t *ps;
  size_t mblength, wclength, n;

  ps = pstate;
  if (pstate == NULL)
    {
      if (!local_state_use)
	{
	  memset (&local_state, '\0', sizeof(mbstate_t));
	  local_state_use = 1;
	}
      ps = &local_state;
    }

  n = strlen (*src);

  if (dest == NULL)
    {
      wchar_t *wsbuf;
      const char *mbs;
      mbstate_t psbuf;

      /* It doesn't matter if malloc fails here, since mbsrtowcs should do
	 the right thing with a NULL first argument. */
      wsbuf = (wchar_t *) malloc ((n + 1) * sizeof(wchar_t));
      mbs = *src;
      psbuf = *ps;

      wclength = mbsrtowcs (wsbuf, &mbs, n, &psbuf);

      if (wsbuf)
	free (wsbuf);
      return wclength;
    }
      
  for (wclength = 0; wclength < len; wclength++, dest++)
    {
      if (mbsinit(ps))
	{
	  if (**src == '\0')
	    {
	      *dest = L'\0';
	      *src = NULL;
	      return (wclength);
	    }
	  else if (**src == '\\')
	    {
	      *dest = L'\\';
	      mblength = 1;
	    }
	  else
	    mblength = mbrtowc(dest, *src, n, ps);
	}
      else
	mblength = mbrtowc(dest, *src, n, ps);

      /* Cannot convert multibyte character to wide character. */
      if (mblength == (size_t)-1 || mblength == (size_t)-2)
	return (size_t)-1;

      *src += mblength;
      n -= mblength;

      /* The multibyte string  has  been  completely  converted,
	 including  the terminating '\0'. */
      if (*dest == L'\0')
	{
	  *src = NULL;
	  break;
	}
    }

    return (wclength);
}

#if HAVE_MBSNRTOWCS
/* Convert a multibyte string to a wide character string. Memory for the
   new wide character string is obtained with malloc.

   Fast multiple-character version of xdupmbstowcs used when the indices are
   not required and mbsnrtowcs is available. */

static size_t
xdupmbstowcs2 (destp, src)
    wchar_t **destp;	/* Store the pointer to the wide character string */
    const char *src;	/* Multibyte character string */
{
  const char *p;	/* Conversion start position of src */
  wchar_t *wsbuf;	/* Buffer for wide characters. */
  size_t wsbuf_size;	/* Size of WSBUF */
  size_t wcnum;		/* Number of wide characters in WSBUF */
  mbstate_t state;	/* Conversion State */
  size_t n, wcslength;	/* Number of wide characters produced by the conversion. */
  const char *end_or_backslash;
  size_t nms;	/* Number of multibyte characters to convert at one time. */
  mbstate_t tmp_state;
  const char *tmp_p;

  memset (&state, '\0', sizeof(mbstate_t));

  wsbuf_size = 0;
  wsbuf = NULL;

  p = src;
  wcnum = 0;
  do
    {
      end_or_backslash = strchrnul(p, '\\');
      nms = end_or_backslash - p;
      if (*end_or_backslash == '\0')
	nms++;

      /* Compute the number of produced wide-characters. */
      tmp_p = p;
      tmp_state = state;

      if (nms == 0 && *p == '\\')	/* special initial case */
	nms = wcslength = 1;
      else
	wcslength = mbsnrtowcs (NULL, &tmp_p, nms, 0, &tmp_state);

      if (wcslength == 0)
	{
	  tmp_p = p;		/* will need below */
	  tmp_state = state;
	  wcslength = 1;	/* take a single byte */
	}

      /* Conversion failed. */
      if (wcslength == (size_t)-1)
	{
	  free (wsbuf);
	  *destp = NULL;
	  return (size_t)-1;
	}

      /* Resize the buffer if it is not large enough. */
      if (wsbuf_size < wcnum+wcslength+1)	/* 1 for the L'\0' or the potential L'\\' */
	{
	  wchar_t *wstmp;

	  while (wsbuf_size < wcnum+wcslength+1) /* 1 for the L'\0' or the potential L'\\' */
	    wsbuf_size += WSBUF_INC;

	  wstmp = (wchar_t *) realloc (wsbuf, wsbuf_size * sizeof (wchar_t));
	  if (wstmp == NULL)
	    {
	      free (wsbuf);
	      *destp = NULL;
	      return (size_t)-1;
	    }
	  wsbuf = wstmp;
	}

      /* Perform the conversion. This is assumed to return 'wcslength'.
	 It may set 'p' to NULL. */
      n = mbsnrtowcs(wsbuf+wcnum, &p, nms, wsbuf_size-wcnum, &state);

      if (n == 0 && p == 0)
	{
	  wsbuf[wcnum] = L'\0';
	  break;
	}

      /* Compensate for taking single byte on wcs conversion failure above. */
      if (wcslength == 1 && (n == 0 || n == (size_t)-1))
	{
	  state = tmp_state;
	  p = tmp_p;
	  wsbuf[wcnum] = *p;
	  if (*p == 0)
	    break;
	  else
	    {
	      wcnum++; p++;
	    }
	}
      else
        wcnum += wcslength;

      if (mbsinit (&state) && (p != NULL) && (*p == '\\'))
	{
	  wsbuf[wcnum++] = L'\\';
	  p++;
	}
    }
  while (p != NULL);

  *destp = wsbuf;

  /* Return the length of the wide character string, not including `\0'. */
  return wcnum;
}
#endif /* HAVE_MBSNRTOWCS */

/* Convert a multibyte string to a wide character string. Memory for the
   new wide character string is obtained with malloc.

   The return value is the length of the wide character string. Returns a
   pointer to the wide character string in DESTP. If INDICESP is not NULL,
   INDICESP stores the pointer to the pointer array. Each pointer is to
   the first byte of each multibyte character. Memory for the pointer array
   is obtained with malloc, too.
   If conversion is failed, the return value is (size_t)-1 and the values
   of DESTP and INDICESP are NULL. */

size_t
xdupmbstowcs (destp, indicesp, src)
    wchar_t **destp;	/* Store the pointer to the wide character string */
    char ***indicesp;	/* Store the pointer to the pointer array. */
    const char *src;	/* Multibyte character string */
{
  const char *p;	/* Conversion start position of src */
  wchar_t wc;		/* Created wide character by conversion */
  wchar_t *wsbuf;	/* Buffer for wide characters. */
  char **indices; 	/* Buffer for indices. */
  size_t wsbuf_size;	/* Size of WSBUF */
  size_t wcnum;		/* Number of wide characters in WSBUF */
  mbstate_t state;	/* Conversion State */

  /* In case SRC or DESP is NULL, conversion doesn't take place. */
  if (src == NULL || destp == NULL)
    {
      if (destp)
	*destp = NULL;
      if (indicesp)
	*indicesp = NULL;
      return (size_t)-1;
    }

#if HAVE_MBSNRTOWCS
  if (indicesp == NULL)
    return (xdupmbstowcs2 (destp, src));
#endif

  memset (&state, '\0', sizeof(mbstate_t));
  wsbuf_size = WSBUF_INC;

  wsbuf = (wchar_t *) malloc (wsbuf_size * sizeof(wchar_t));
  if (wsbuf == NULL)
    {
      *destp = NULL;
      if (indicesp)
        *indicesp = NULL;
      return (size_t)-1;
    }

  indices = NULL;
  if (indicesp)
    {
      indices = (char **) malloc (wsbuf_size * sizeof(char *));
      if (indices == NULL)
	{
	  free (wsbuf);
	  *destp = NULL;
	  *indicesp = NULL;
	  return (size_t)-1;
	}
    }

  p = src;
  wcnum = 0;
  do
    {
      size_t mblength;	/* Byte length of one multibyte character. */

      if (mbsinit (&state))
	{
	  if (*p == '\0')
	    {
	      wc = L'\0';
	      mblength = 1;
	    }
	  else if (*p == '\\')
	    {
	      wc = L'\\';
	      mblength = 1;
	    }
	  else
	    mblength = mbrtowc(&wc, p, MB_LEN_MAX, &state);
	}
      else
	mblength = mbrtowc(&wc, p, MB_LEN_MAX, &state);

      /* Conversion failed. */
      if (MB_INVALIDCH (mblength))
	{
	  free (wsbuf);
	  FREE (indices);
	  *destp = NULL;
	  if (indicesp)
	    *indicesp = NULL;
	  return (size_t)-1;
	}

      ++wcnum;

      /* Resize buffers when they are not large enough. */
      if (wsbuf_size < wcnum)
	{
	  wchar_t *wstmp;
	  char **idxtmp;

	  wsbuf_size += WSBUF_INC;

	  wstmp = (wchar_t *) realloc (wsbuf, wsbuf_size * sizeof (wchar_t));
	  if (wstmp == NULL)
	    {
	      free (wsbuf);
	      FREE (indices);
	      *destp = NULL;
	      if (indicesp)
		*indicesp = NULL;
	      return (size_t)-1;
	    }
	  wsbuf = wstmp;

	  if (indicesp)
	    {
	      idxtmp = (char **) realloc (indices, wsbuf_size * sizeof (char *));
	      if (idxtmp == NULL)
		{
		  free (wsbuf);
		  free (indices);
		  *destp = NULL;
		  if (indicesp)
		    *indicesp = NULL;
		  return (size_t)-1;
		}
	      indices = idxtmp;
	    }
	}

      wsbuf[wcnum - 1] = wc;
      if (indices)
        indices[wcnum - 1] = (char *)p;
      p += mblength;
    }
  while (MB_NULLWCH (wc) == 0);

  /* Return the length of the wide character string, not including `\0'. */
  *destp = wsbuf;
  if (indicesp != NULL)
    *indicesp = indices;

  return (wcnum - 1);
}

/* Convert wide character string to multibyte character string. Treat invalid
   wide characters as bytes.  Used only in unusual circumstances.

   Written by Bruno Haible <bruno@clisp.org>, 2008, adapted by Chet Ramey
   for use in Bash. */

/* Convert wide character string *SRCP to a multibyte character string and
   store the result in DEST. Store at most LEN bytes in DEST. */
size_t
xwcsrtombs (char *dest, const wchar_t **srcp, size_t len, mbstate_t *ps)
{
  const wchar_t *src;
  size_t cur_max;			/* XXX - locale_cur_max */
  char buf[64], *destptr, *tmp_dest;
  unsigned char uc;
  mbstate_t prev_state;

  cur_max = MB_CUR_MAX;
  if (cur_max > sizeof (buf))		/* Holy cow. */
    return (size_t)-1;

  src = *srcp;

  if (dest != NULL)
    {
      destptr = dest;

      for (; len > 0; src++)
	{
	  wchar_t wc;
	  size_t ret;

	  wc = *src;
	  /* If we have room, store directly into DEST. */
	  tmp_dest = destptr;
	  ret = wcrtomb (len >= cur_max ? destptr : buf, wc, ps);

	  if (ret == (size_t)(-1))		/* XXX */
	    {
	      /* Since this is used for globbing and other uses of filenames,
		 treat invalid wide character sequences as bytes.  This is
		 intended to be symmetric with xdupmbstowcs2. */
handle_byte:
	      destptr = tmp_dest;	/* in case wcrtomb modified it */
	      uc = wc;
	      ret = 1;
	      if (len >= cur_max)
		*destptr = uc;
	      else
		buf[0] = uc;
	      if (ps)
		memset (ps, 0, sizeof (mbstate_t));
	    }

	  if (ret > cur_max)		/* Holy cow */
	    goto bad_input;

	  if (len < ret)
	    break;

	  if (len < cur_max)
	    memcpy (destptr, buf, ret);

	  if (wc == 0)
	    {
	      src = NULL;
	      /* Here mbsinit (ps).  */
	      break;
	    }
	  destptr += ret;
	  len -= ret;
	}
      *srcp = src;
      return destptr - dest;
    }
  else
    {
      /* Ignore dest and len, don't store *srcp at the end, and
	 don't clobber *ps.  */
      mbstate_t state = *ps;
      size_t totalcount = 0;

      for (;; src++)
	{
	  wchar_t wc;
	  size_t ret;

	  wc = *src;
	  ret = wcrtomb (buf, wc, &state);

	  if (ret == (size_t)(-1))
	    goto bad_input2;
	  if (wc == 0)
	    {
	      /* Here mbsinit (&state).  */
	      break;
	    }
	  totalcount += ret;
	}
      return totalcount;
    }

bad_input:
  *srcp = src;
bad_input2:
  errno = EILSEQ;
  return (size_t)(-1);
}

#endif /* HANDLE_MULTIBYTE */
