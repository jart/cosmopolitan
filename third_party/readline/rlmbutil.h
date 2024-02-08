/* rlmbutil.h -- utility functions for multibyte characters. */

/* Copyright (C) 2001-2021 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library (Readline), a library
   for reading lines of text with interactive input and history editing.      

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (_RL_MBUTIL_H_)
#define _RL_MBUTIL_H_

#include "rlstdc.h"

/************************************************/
/* check multibyte capability for I18N code     */
/************************************************/

/* For platforms which support the ISO C amendment 1 functionality we
   support user defined character classes.  */
   /* Solaris 2.5 has a bug: <wchar.h> must be included before <wctype.h>.  */
#if defined (HAVE_WCTYPE_H) && defined (HAVE_WCHAR_H) && defined (HAVE_LOCALE_H)
#  include <wchar.h>
#  include <wctype.h>
#  if defined (HAVE_ISWCTYPE) && \
      defined (HAVE_ISWLOWER) && \
      defined (HAVE_ISWUPPER) && \
      defined (HAVE_MBSRTOWCS) && \
      defined (HAVE_MBRTOWC) && \
      defined (HAVE_MBRLEN) && \
      defined (HAVE_TOWLOWER) && \
      defined (HAVE_TOWUPPER) && \
      defined (HAVE_WCHAR_T) && \
      defined (HAVE_WCWIDTH)
     /* system is supposed to support XPG5 */
#    define HANDLE_MULTIBYTE      1
#  endif
#endif

/* If we don't want multibyte chars even on a system that supports them, let
   the configuring user turn multibyte support off. */
#if defined (NO_MULTIBYTE_SUPPORT)
#  undef HANDLE_MULTIBYTE
#endif

/* Some systems, like BeOS, have multibyte encodings but lack mbstate_t.  */
#if HANDLE_MULTIBYTE && !defined (HAVE_MBSTATE_T)
#  define wcsrtombs(dest, src, len, ps) (wcsrtombs) (dest, src, len, 0)
#  define mbsrtowcs(dest, src, len, ps) (mbsrtowcs) (dest, src, len, 0)
#  define wcrtomb(s, wc, ps) (wcrtomb) (s, wc, 0)
#  define mbrtowc(pwc, s, n, ps) (mbrtowc) (pwc, s, n, 0)
#  define mbrlen(s, n, ps) (mbrlen) (s, n, 0)
#  define mbstate_t int
#endif

/* Make sure MB_LEN_MAX is at least 16 on systems that claim to be able to
   handle multibyte chars (some systems define MB_LEN_MAX as 1) */
#ifdef HANDLE_MULTIBYTE
#  include <limits.h>
#  if defined(MB_LEN_MAX) && (MB_LEN_MAX < 16)
#    undef MB_LEN_MAX
#  endif
#  if !defined (MB_LEN_MAX)
#    define MB_LEN_MAX 16
#  endif
#endif

/************************************************/
/* end of multibyte capability checks for I18N  */
/************************************************/

/*
 * wchar_t doesn't work for 32-bit values on Windows using MSVC
 */
#ifdef WCHAR_T_BROKEN
#  define WCHAR_T char32_t
#  define MBRTOWC mbrtoc32
#  define WCRTOMB c32rtomb
#else	/* normal systems */
#  define WCHAR_T wchar_t
#  define MBRTOWC mbrtowc
#  define WCRTOMB wcrtomb
#endif

/*
 * Flags for _rl_find_prev_mbchar and _rl_find_next_mbchar:
 *
 * MB_FIND_ANY		find any multibyte character
 * MB_FIND_NONZERO	find a non-zero-width multibyte character
 */

#define MB_FIND_ANY	0x00
#define MB_FIND_NONZERO	0x01

extern int _rl_find_prev_mbchar (char *, int, int);
extern int _rl_find_next_mbchar (char *, int, int, int);

#ifdef HANDLE_MULTIBYTE

extern int _rl_compare_chars (char *, int, mbstate_t *, char *, int, mbstate_t *);
extern int _rl_get_char_len (char *, mbstate_t *);
extern int _rl_adjust_point (char *, int, mbstate_t *);

extern int _rl_read_mbchar (char *, int);
extern int _rl_read_mbstring (int, char *, int);

extern int _rl_is_mbchar_matched (char *, int, int, char *, int);

extern WCHAR_T _rl_char_value (char *, int);
extern int _rl_walphabetic (WCHAR_T);

#define _rl_to_wupper(wc)	(iswlower (wc) ? towupper (wc) : (wc))
#define _rl_to_wlower(wc)	(iswupper (wc) ? towlower (wc) : (wc))

#define MB_NEXTCHAR(b,s,c,f) \
	((MB_CUR_MAX > 1 && rl_byte_oriented == 0) \
		? _rl_find_next_mbchar ((b), (s), (c), (f)) \
		: ((s) + (c)))
#define MB_PREVCHAR(b,s,f) \
	((MB_CUR_MAX > 1 && rl_byte_oriented == 0) \
		? _rl_find_prev_mbchar ((b), (s), (f)) \
		: ((s) - 1))

#define MB_INVALIDCH(x)		((x) == (size_t)-1 || (x) == (size_t)-2)
#define MB_NULLWCH(x)		((x) == 0)

/* Try and shortcut the printable ascii characters to cut down the number of
   calls to a libc wcwidth() */
static inline int
_rl_wcwidth (WCHAR_T wc)
{
  switch (wc)
    {
    case ' ': case '!': case '"': case '#': case '%':
    case '&': case '\'': case '(': case ')': case '*':
    case '+': case ',': case '-': case '.': case '/':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case ':': case ';': case '<': case '=': case '>':
    case '?':
    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y':
    case 'Z':
    case '[': case '\\': case ']': case '^': case '_':
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y':
    case 'z': case '{': case '|': case '}': case '~':
      return 1;
    default:
      return wcwidth (wc);
    }
}

/* Unicode combining characters range from U+0300 to U+036F */
#define UNICODE_COMBINING_CHAR(x) ((x) >= 768 && (x) <= 879)

#if defined (WCWIDTH_BROKEN)
#  define WCWIDTH(wc)	((_rl_utf8locale && UNICODE_COMBINING_CHAR(wc)) ? 0 : _rl_wcwidth(wc))
#else
#  define WCWIDTH(wc)	_rl_wcwidth(wc)
#endif

#if defined (WCWIDTH_BROKEN)
#  define IS_COMBINING_CHAR(x)	(WCWIDTH(x) == 0 && iswcntrl(x) == 0)
#else
#  define IS_COMBINING_CHAR(x)	(WCWIDTH(x) == 0)
#endif

#define UTF8_SINGLEBYTE(c)	(((c) & 0x80) == 0)
#define UTF8_MBFIRSTCHAR(c)	(((c) & 0xc0) == 0xc0)
#define UTF8_MBCHAR(c)		(((c) & 0xc0) == 0x80)

#else /* !HANDLE_MULTIBYTE */

#undef MB_LEN_MAX
#undef MB_CUR_MAX

#define MB_LEN_MAX	1
#define MB_CUR_MAX	1

#define _rl_find_prev_mbchar(b, i, f)		(((i) == 0) ? (i) : ((i) - 1))
#define _rl_find_next_mbchar(b, i1, i2, f)	((i1) + (i2))

#define _rl_char_value(buf,ind)	((buf)[(ind)])

#define _rl_walphabetic(c)	(rl_alphabetic (c))

#define _rl_to_wupper(c)	(_rl_to_upper (c))
#define _rl_to_wlower(c)	(_rl_to_lower (c))

#define MB_NEXTCHAR(b,s,c,f)	((s) + (c))
#define MB_PREVCHAR(b,s,f)	((s) - 1)

#define MB_INVALIDCH(x)		(0)
#define MB_NULLWCH(x)		(0)

#define UTF8_SINGLEBYTE(c)	(1)

#if !defined (HAVE_WCHAR_T) && !defined (wchar_t)
#  define wchar_t int
#endif

#endif /* !HANDLE_MULTIBYTE */

extern int rl_byte_oriented;

#endif /* _RL_MBUTIL_H_ */
