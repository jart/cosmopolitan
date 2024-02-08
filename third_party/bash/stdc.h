/* stdc.h -- macros to make source compile on both ANSI C and K&R C
   compilers. */

/* Copyright (C) 1993-2021 Free Software Foundation, Inc.

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

#if !defined (_STDC_H_)
#define _STDC_H_

/* Adapted from BSD /usr/include/sys/cdefs.h. */

/* A function can be defined using prototypes and compile on both ANSI C
   and traditional C compilers with something like this:
	extern char *func PARAMS((char *, char *, int)); */

#if !defined (PARAMS)
#  if defined (__STDC__) || defined (__GNUC__) || defined (__cplusplus) || defined (PROTOTYPES)
#    define PARAMS(protos) protos
#  else 
#    define PARAMS(protos) ()
#  endif
#endif

/* Fortify, at least, has trouble with this definition */
#if defined (HAVE_STRINGIZE)
#  define CPP_STRING(x) #x
#else
#  define CPP_STRING(x) "x"
#endif

#if !defined (__STDC__)

#if defined (__GNUC__)		/* gcc with -traditional */
#  if !defined (signed)
#    define signed __signed
#  endif
#  if !defined (volatile)
#    define volatile __volatile
#  endif
#else /* !__GNUC__ */
#  if !defined (inline)
#    define inline
#  endif
#  if !defined (signed)
#    define signed
#  endif
#  if !defined (volatile)
#    define volatile
#  endif
#endif /* !__GNUC__ */

#endif /* !__STDC__ */

#ifndef __attribute__
#  if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 8)
#    define __attribute__(x)
#  endif
#endif

/* For those situations when gcc handles inlining a particular function but
   other compilers complain. */
#ifdef __GNUC__
#  define INLINE inline
#else
#  define INLINE
#endif

#if defined (PREFER_STDARG)
#  define SH_VA_START(va, arg)  va_start(va, arg)
#else
#  define SH_VA_START(va, arg)  va_start(va)
#endif

#endif /* !_STDC_H_ */
