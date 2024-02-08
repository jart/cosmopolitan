/* typemax.h -- encapsulate max values for long, long long, etc. */

/* Copyright (C) 2001-2021 Free Software Foundation, Inc.

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

/*
 * NOTE:  This should be included after config.h, limits.h, stdint.h, and
 *	  inttypes.h
 */

#ifndef _SH_TYPEMAX_H
#define _SH_TYPEMAX_H

#ifndef CHAR_BIT
#  define CHAR_BIT 8
#endif

/* Nonzero if the integer type T is signed.  */
#ifndef TYPE_SIGNED
#  define TYPE_SIGNED(t)	(! ((t) 0 < (t) -1))
#endif

#ifndef TYPE_SIGNED_MAGNITUDE
#  define TYPE_SIGNED_MAGNITUDE(t) ((t) ~ (t) 0 < (t) -1)
#endif

#ifndef TYPE_WIDTH
#  define TYPE_WIDTH(t) (sizeof (t) * CHAR_BIT)
#endif

#ifndef TYPE_MINIMUM
#  define TYPE_MINIMUM(t) ((t) ~ TYPE_MAXIMUM (t))
#endif

#ifndef TYPE_MAXIMUM
#  define TYPE_MAXIMUM(t)  \
  ((t) (! TYPE_SIGNED (t) \
        ? (t) -1 \
        : ((((t) 1 << (TYPE_WIDTH (t) - 2)) - 1) * 2 + 1)))
#endif

#ifdef HAVE_LONG_LONG_INT
#  ifndef LLONG_MAX
#    define LLONG_MAX   TYPE_MAXIMUM(long long int)
#    define LLONG_MIN	TYPE_MINIMUM(long long int)
#  endif
#  ifndef ULLONG_MAX
#    define ULLONG_MAX  TYPE_MAXIMUM(unsigned long long int)
#  endif
#endif

#ifndef ULONG_MAX
#  define ULONG_MAX	((unsigned long) ~(unsigned long) 0)
#endif

#ifndef LONG_MAX
#  define LONG_MAX	((long int) (ULONG_MAX >> 1))
#  define LONG_MIN	((long int) (-LONG_MAX - 1L))
#endif

#ifndef INT_MAX		/* ouch */
#  define INT_MAX	TYPE_MAXIMUM(int)
#  define INT_MIN	TYPE_MINIMUM(int)
#  define UINT_MAX	((unsigned int) ~(unsigned int)0)
#endif

#ifndef SHRT_MAX
#  define SHRT_MAX	TYPE_MAXIMUM(short)
#  define SHRT_MIN	TYPE_MINIMUM(short)
#  define USHRT_MAX	((unsigned short) ~(unsigned short)0)
#endif

#ifndef UCHAR_MAX
#  define UCHAR_MAX	255
#endif

/* workaround for gcc bug in versions < 2.7 */
#if defined (HAVE_LONG_LONG_INT) && __GNUC__ == 2 && __GNUC_MINOR__ < 7
static const unsigned long long int maxquad = ULLONG_MAX;
#  undef ULLONG_MAX
#  define ULLONG_MAX maxquad
#endif

#if !defined (INTMAX_MAX) || !defined (INTMAX_MIN)

#if SIZEOF_INTMAX_T == SIZEOF_LONG_LONG
#  define INTMAX_MAX	LLONG_MAX
#  define INTMAX_MIN	LLONG_MIN
#elif SIZEOF_INTMAX_T == SIZEOF_LONG
#  define INTMAX_MAX	LONG_MAX
#  define INTMAX_MIN	LONG_MIN
#else
#  define INTMAX_MAX	INT_MAX
#  define INTMAX_MIN	INT_MIN
#endif

#endif

#ifndef SSIZE_MAX
#  define SSIZE_MAX	INT_MAX
#endif

#ifndef SIZE_MAX
#  define SIZE_MAX	((size_t) ~(size_t)0)
#endif

#ifndef sh_imaxabs
#  define sh_imaxabs(x)	(((x) >= 0) ? (x) : -(x))
#endif

/* Handle signed arithmetic overflow and underflow.  Have to do it this way
   to avoid compilers optimizing out simpler overflow checks. */

/* Make sure that a+b does not exceed MAXV or is smaller than MINV (if b < 0).
   Assumes that b > 0 if a > 0 and b < 0 if a < 0 */
#define ADDOVERFLOW(a,b,minv,maxv) \
	((((a) > 0) && ((b) > ((maxv) - (a)))) || \
	 (((a) < 0) && ((b) < ((minv) - (a)))))

/* Make sure that a-b is not smaller than MINV or exceeds MAXV (if b < 0).
   Assumes that b > 0 if a > 0 and b < 0 if a < 0 */
#define SUBOVERFLOW(a,b,minv,maxv) \
	((((b) > 0) && ((a) < ((minv) + (b)))) || \
	 (((b) < 0) && ((a) > ((maxv) + (b)))))

#endif /* _SH_TYPEMAX_H */
