/* shmbutil.h -- utility functions for multibyte characters. */

/* Copyright (C) 2002-2019 Free Software Foundation, Inc.

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
                                 
#if !defined (_SH_MBUTIL_H_)
#define _SH_MBUTIL_H_

#include "stdc.h"

/* Include config.h for HANDLE_MULTIBYTE */
#include "config.h"

#if defined (HANDLE_MULTIBYTE)
#include "shmbchar.h"

extern size_t xwcsrtombs PARAMS((char *, const wchar_t **, size_t, mbstate_t *));
extern size_t xmbsrtowcs PARAMS((wchar_t *, const char **, size_t, mbstate_t *));
extern size_t xdupmbstowcs PARAMS((wchar_t **, char ***, const char *));

extern size_t mbstrlen PARAMS((const char *));

extern char *xstrchr PARAMS((const char *, int));

extern int locale_mb_cur_max;	/* XXX */
extern int locale_utf8locale;	/* XXX */

#ifndef MB_INVALIDCH
#define MB_INVALIDCH(x)		((x) == (size_t)-1 || (x) == (size_t)-2)
#define MB_NULLWCH(x)		((x) == 0)
#endif

#define MBSLEN(s)	(((s) && (s)[0]) ? ((s)[1] ? mbstrlen (s) : 1) : 0)
#define MB_STRLEN(s)	((MB_CUR_MAX > 1) ? MBSLEN (s) : STRLEN (s))

#define MBLEN(s, n)	((MB_CUR_MAX > 1) ? mblen ((s), (n)) : 1)
#define MBRLEN(s, n, p)	((MB_CUR_MAX > 1) ? mbrlen ((s), (n), (p)) : 1)

#define UTF8_SINGLEBYTE(c)	(((c) & 0x80) == 0)
#define UTF8_MBFIRSTCHAR(c)	(((c) & 0xc0) == 0xc0)
#define UTF8_MBCHAR(c)		(((c) & 0xc0) == 0x80)

#else /* !HANDLE_MULTIBYTE */

#undef MB_LEN_MAX
#undef MB_CUR_MAX

#define MB_LEN_MAX	1
#define MB_CUR_MAX	1

#undef xstrchr
#define xstrchr(s, c)	strchr(s, c)

#ifndef MB_INVALIDCH
#define MB_INVALIDCH(x)		(0)
#define MB_NULLWCH(x)		(0)
#endif

#define MB_STRLEN(s)		(STRLEN(s))

#define MBLEN(s, n)		1
#define MBRLEN(s, n, p)		1

#ifndef wchar_t
#  define wchar_t	int
#endif

#define UTF8_SINGLEBYTE(c)	(1)
#define UTF8_MBFIRSTCHAR(c)	(0)

#endif /* !HANDLE_MULTIBYTE */

/* Declare and initialize a multibyte state.  Call must be terminated
   with `;'. */
#if defined (HANDLE_MULTIBYTE)
#  define DECLARE_MBSTATE \
	mbstate_t state; \
	memset (&state, '\0', sizeof (mbstate_t))
#else
#  define DECLARE_MBSTATE
#endif  /* !HANDLE_MULTIBYTE */

/* Initialize or reinitialize a multibyte state named `state'.  Call must be
   terminated with `;'. */
#if defined (HANDLE_MULTIBYTE)
#  define INITIALIZE_MBSTATE memset (&state, '\0', sizeof (mbstate_t))
#else
#  define INITIALIZE_MBSTATE
#endif  /* !HANDLE_MULTIBYTE */

/* Advance one (possibly multi-byte) character in string _STR of length
   _STRSIZE, starting at index _I.  STATE must have already been declared. */
#if defined (HANDLE_MULTIBYTE)
#  define ADVANCE_CHAR(_str, _strsize, _i) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    mbstate_t state_bak; \
	    size_t mblength; \
	    int _f; \
\
	    _f = is_basic ((_str)[_i]); \
	    if (_f) \
	      mblength = 1; \
	    else if (locale_utf8locale && (((_str)[_i] & 0x80) == 0)) \
	      mblength = (_str)[_i] != 0; \
	    else \
	      { \
	        state_bak = state; \
	        mblength = mbrlen ((_str) + (_i), (_strsize) - (_i), &state); \
	      } \
\
	    if (mblength == (size_t)-2 || mblength == (size_t)-1) \
	      { \
		state = state_bak; \
		(_i)++; \
	      } \
	    else if (mblength == 0) \
	      (_i)++; \
	    else \
	      (_i) += mblength; \
	  } \
	else \
	  (_i)++; \
      } \
    while (0)
#else
#  define ADVANCE_CHAR(_str, _strsize, _i)	(_i)++
#endif  /* !HANDLE_MULTIBYTE */

/* Advance one (possibly multibyte) character in the string _STR of length
   _STRSIZE.
   SPECIAL:  assume that _STR will be incremented by 1 after this call. */
#if defined (HANDLE_MULTIBYTE)
#  define ADVANCE_CHAR_P(_str, _strsize) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    mbstate_t state_bak; \
	    size_t mblength; \
	    int _f; \
\
	    _f = is_basic (*(_str)); \
	    if (_f) \
	      mblength = 1; \
	    else if (locale_utf8locale && ((*(_str) & 0x80) == 0)) \
	      mblength = *(_str) != 0; \
	    else \
	      { \
		state_bak = state; \
		mblength = mbrlen ((_str), (_strsize), &state); \
	      } \
\
	    if (mblength == (size_t)-2 || mblength == (size_t)-1) \
	      { \
		state = state_bak; \
		mblength = 1; \
	      } \
	    else \
	      (_str) += (mblength < 1) ? 0 : (mblength - 1); \
	  } \
      } \
    while (0)
#else
#  define ADVANCE_CHAR_P(_str, _strsize)
#endif  /* !HANDLE_MULTIBYTE */

/* Back up one (possibly multi-byte) character in string _STR of length
   _STRSIZE, starting at index _I.  STATE must have already been declared. */
#if defined (HANDLE_MULTIBYTE)
#  define BACKUP_CHAR(_str, _strsize, _i) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    mbstate_t state_bak; \
	    size_t mblength; \
	    int _x, _p; /* _x == temp index into string, _p == prev index */ \
\
	    _x = _p = 0; \
	    while (_x < (_i)) \
	      { \
	        state_bak = state; \
	        mblength = mbrlen ((_str) + (_x), (_strsize) - (_x), &state); \
\
		if (mblength == (size_t)-2 || mblength == (size_t)-1) \
		  { \
		    state = state_bak; \
		    _x++; \
		  } \
		else if (mblength == 0) \
		  _x++; \
		else \
		  { \
		    _p = _x; /* _p == start of prev mbchar */ \
		    _x += mblength; \
		  } \
	      } \
	    (_i) = _p; \
	  } \
	else \
	  (_i)--; \
      } \
    while (0)
#else
#  define BACKUP_CHAR(_str, _strsize, _i)	(_i)--
#endif  /* !HANDLE_MULTIBYTE */

/* Back up one (possibly multibyte) character in the string _BASE of length
   _STRSIZE starting at _STR (_BASE <= _STR <= (_BASE + _STRSIZE) ).
   SPECIAL: DO NOT assume that _STR will be decremented by 1 after this call. */
#if defined (HANDLE_MULTIBYTE)
#  define BACKUP_CHAR_P(_base, _strsize, _str) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    mbstate_t state_bak; \
	    size_t mblength; \
	    char *_x, _p; /* _x == temp pointer into string, _p == prev pointer */ \
\
	    _x = _p = _base; \
	    while (_x < (_str)) \
	      { \
	        state_bak = state; \
	        mblength = mbrlen (_x, (_strsize) - _x, &state); \
\
		if (mblength == (size_t)-2 || mblength == (size_t)-1) \
		  { \
		    state = state_bak; \
		    _x++; \
		  } \
		else if (mblength == 0) \
		  _x++; \
		else \
		  { \
		    _p = _x; /* _p == start of prev mbchar */ \
		    _x += mblength; \
		  } \
	      } \
	    (_str) = _p; \
	  } \
	else \
	  (_str)--; \
      } \
    while (0)
#else
#  define BACKUP_CHAR_P(_base, _strsize, _str) (_str)--
#endif  /* !HANDLE_MULTIBYTE */

/* Copy a single character from the string _SRC to the string _DST.
   _SRCEND is a pointer to the end of _SRC. */
#if defined (HANDLE_MULTIBYTE)
#  define COPY_CHAR_P(_dst, _src, _srcend) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    mbstate_t state_bak; \
	    size_t mblength; \
	    int _k; \
\
	    _k = is_basic (*(_src)); \
	    if (_k) \
	      mblength = 1; \
	    else if (locale_utf8locale && ((*(_src) & 0x80) == 0)) \
	      mblength = *(_src) != 0; \
	    else \
	      { \
		state_bak = state; \
		mblength = mbrlen ((_src), (_srcend) - (_src), &state); \
	      } \
	    if (mblength == (size_t)-2 || mblength == (size_t)-1) \
	      { \
		state = state_bak; \
		mblength = 1; \
	      } \
	    else \
	      mblength = (mblength < 1) ? 1 : mblength; \
\
	    for (_k = 0; _k < mblength; _k++) \
	      *(_dst)++ = *(_src)++; \
	  } \
	else \
	  *(_dst)++ = *(_src)++; \
      } \
    while (0)
#else
#  define COPY_CHAR_P(_dst, _src, _srcend)	*(_dst)++ = *(_src)++
#endif  /* !HANDLE_MULTIBYTE */

/* Copy a single character from the string _SRC at index _SI to the string
   _DST at index _DI.  _SRCEND is a pointer to the end of _SRC. */
#if defined (HANDLE_MULTIBYTE)
#  define COPY_CHAR_I(_dst, _di, _src, _srcend, _si) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    mbstate_t state_bak; \
	    size_t mblength; \
	    int _k; \
\
	    _k = is_basic ((_src)[(_si)]); \
	    if (_k) \
	      mblength = 1; \
	    else if (locale_utf8locale && ((_src)[(_si)] & 0x80) == 0) \
	      mblength = (_src)[(_si)] != 0; \
	    else \
	      {\
		state_bak = state; \
		mblength = mbrlen ((_src) + (_si), (_srcend) - ((_src)+(_si)), &state); \
	      } \
	    if (mblength == (size_t)-2 || mblength == (size_t)-1) \
	      { \
		state = state_bak; \
		mblength = 1; \
	      } \
	    else \
	      mblength = (mblength < 1) ? 1 : mblength; \
\
	    for (_k = 0; _k < mblength; _k++) \
	      _dst[_di++] = _src[_si++]; \
	  } \
	else \
	  _dst[_di++] = _src[_si++]; \
      } \
    while (0)
#else
#  define COPY_CHAR_I(_dst, _di, _src, _srcend, _si)	_dst[_di++] = _src[_si++]
#endif  /* !HANDLE_MULTIBYTE */

/****************************************************************
 *								*
 * The following are only guaranteed to work in subst.c		*
 *								*
 ****************************************************************/

#if defined (HANDLE_MULTIBYTE)
#  define SCOPY_CHAR_I(_dst, _escchar, _sc, _src, _si, _slen) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    mbstate_t state_bak; \
	    size_t mblength; \
	    int _i; \
\
	    _i = is_basic ((_src)[(_si)]); \
	    if (_i) \
	      mblength = 1; \
	    else if (locale_utf8locale && ((_src)[(_si)] & 0x80) == 0) \
	      mblength = (_src)[(_si)] != 0; \
	    else \
	      { \
		state_bak = state; \
		mblength = mbrlen ((_src) + (_si), (_slen) - (_si), &state); \
	      } \
	    if (mblength == (size_t)-2 || mblength == (size_t)-1) \
	      { \
		state = state_bak; \
		mblength = 1; \
	      } \
	    else \
	      mblength = (mblength < 1) ? 1 : mblength; \
\
	    temp = xmalloc (mblength + 2); \
	    temp[0] = _escchar; \
	    for (_i = 0; _i < mblength; _i++) \
	      temp[_i + 1] = _src[_si++]; \
	    temp[mblength + 1] = '\0'; \
\
	    goto add_string; \
	  } \
	else \
	  { \
	    _dst[0] = _escchar; \
	    _dst[1] = _sc; \
	  } \
      } \
    while (0)
#else
#  define SCOPY_CHAR_I(_dst, _escchar, _sc, _src, _si, _slen) \
    _dst[0] = _escchar; \
    _dst[1] = _sc
#endif  /* !HANDLE_MULTIBYTE */

#if defined (HANDLE_MULTIBYTE)
#  define SCOPY_CHAR_M(_dst, _src, _srcend, _si) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    mbstate_t state_bak; \
	    size_t mblength; \
	    int _i; \
\
	    _i = is_basic (*((_src) + (_si))); \
	    if (_i) \
	      mblength = 1; \
	    else if (locale_utf8locale && (((_src)[_si] & 0x80) == 0)) \
              mblength = (_src)[_si] != 0; \
	    else \
	      { \
		state_bak = state; \
		mblength = mbrlen ((_src) + (_si), (_srcend) - ((_src) + (_si)), &state); \
	      } \
	    if (mblength == (size_t)-2 || mblength == (size_t)-1) \
	      { \
		state = state_bak; \
		mblength = 1; \
	      } \
	    else \
	      mblength = (mblength < 1) ? 1 : mblength; \
\
	    FASTCOPY(((_src) + (_si)), (_dst), mblength); \
\
	    (_dst) += mblength; \
	    (_si) += mblength; \
	  } \
	else \
	  { \
	    *(_dst)++ = _src[(_si)]; \
	    (_si)++; \
	  } \
      } \
    while (0)
#else
#  define SCOPY_CHAR_M(_dst, _src, _srcend, _si) \
	*(_dst)++ = _src[(_si)]; \
	(_si)++
#endif  /* !HANDLE_MULTIBYTE */

#if HANDLE_MULTIBYTE
#  define SADD_MBCHAR(_dst, _src, _si, _srcsize) \
    do \
      { \
	if (locale_mb_cur_max > 1) \
	  { \
	    int i; \
	    mbstate_t state_bak; \
	    size_t mblength; \
\
	    i = is_basic (*((_src) + (_si))); \
	    if (i) \
	      mblength = 1; \
	    else if (locale_utf8locale && (((_src)[_si] & 0x80) == 0)) \
	      mblength = (_src)[_si] != 0; \
	    else \
	      { \
		state_bak = state; \
		mblength = mbrlen ((_src) + (_si), (_srcsize) - (_si), &state); \
	      } \
	    if (mblength == (size_t)-1 || mblength == (size_t)-2) \
	      { \
		state = state_bak; \
		mblength = 1; \
	      } \
	    if (mblength < 1) \
	      mblength = 1; \
\
	    _dst = (char *)xmalloc (mblength + 1); \
	    for (i = 0; i < mblength; i++) \
	      (_dst)[i] = (_src)[(_si)++]; \
	    (_dst)[mblength] = '\0'; \
\
	    goto add_string; \
	  } \
      } \
    while (0)

#else
#  define SADD_MBCHAR(_dst, _src, _si, _srcsize)
#endif

/* Watch out when using this -- it's just straight textual substitution */
#if defined (HANDLE_MULTIBYTE)
#  define SADD_MBQCHAR_BODY(_dst, _src, _si, _srcsize) \
\
	    int i; \
	    mbstate_t state_bak; \
	    size_t mblength; \
\
	    i = is_basic (*((_src) + (_si))); \
	    if (i) \
	      mblength = 1; \
	    else if (locale_utf8locale && (((_src)[_si] & 0x80) == 0)) \
	      mblength = (_src)[_si] != 0; \
	    else \
	      { \
		state_bak = state; \
		mblength = mbrlen ((_src) + (_si), (_srcsize) - (_si), &state); \
	      } \
	    if (mblength == (size_t)-1 || mblength == (size_t)-2) \
	      { \
		state = state_bak; \
		mblength = 1; \
	      } \
	    if (mblength < 1) \
	      mblength = 1; \
\
	    (_dst) = (char *)xmalloc (mblength + 2); \
	    (_dst)[0] = CTLESC; \
	    for (i = 0; i < mblength; i++) \
	      (_dst)[i+1] = (_src)[(_si)++]; \
	    (_dst)[mblength+1] = '\0'; \
\
	    goto add_string

#  define SADD_MBCHAR_BODY(_dst, _src, _si, _srcsize) \
\
	    int i; \
	    mbstate_t state_bak; \
	    size_t mblength; \
\
	    i = is_basic (*((_src) + (_si))); \
	    if (i) \
	      mblength = 1; \
	    else if (locale_utf8locale && (((_src)[_si] & 0x80) == 0)) \
              mblength = (_src)[_si] != 0; \
            else \
	      { \
		state_bak = state; \
		mblength = mbrlen ((_src) + (_si), (_srcsize) - (_si), &state); \
	      } \
	    if (mblength == (size_t)-1 || mblength == (size_t)-2) \
	      { \
		state = state_bak; \
		mblength = 1; \
	      } \
	    if (mblength < 1) \
	      mblength = 1; \
\
	    (_dst) = (char *)xmalloc (mblength + 1); \
	    for (i = 0; i < mblength; i++) \
	      (_dst)[i+1] = (_src)[(_si)++]; \
	    (_dst)[mblength+1] = '\0'; \
\
	    goto add_string

#endif /* HANDLE_MULTIBYTE */
#endif /* _SH_MBUTIL_H_ */
