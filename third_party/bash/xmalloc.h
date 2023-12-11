/* xmalloc.h -- defines for the `x' memory allocation functions */

/* Copyright (C) 2001-2020 Free Software Foundation, Inc.

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

#if !defined (_XMALLOC_H_)
#define _XMALLOC_H_

#include "stdc.h"
#include "bashansi.h"

/* Generic pointer type. */
#ifndef PTR_T

#if defined (__STDC__)
#  define PTR_T	void *
#else
#  define PTR_T char *
#endif

#endif /* PTR_T */

/* Allocation functions in xmalloc.c */
extern PTR_T xmalloc PARAMS((size_t));
extern PTR_T xrealloc PARAMS((void *, size_t));
extern void xfree PARAMS((void *));

#if defined(USING_BASH_MALLOC) && !defined (DISABLE_MALLOC_WRAPPERS)
extern PTR_T sh_xmalloc PARAMS((size_t, const char *, int));
extern PTR_T sh_xrealloc PARAMS((void *, size_t, const char *, int));
extern void sh_xfree PARAMS((void *, const char *, int));

#define xmalloc(x)	sh_xmalloc((x), __FILE__, __LINE__)
#define xrealloc(x, n)	sh_xrealloc((x), (n), __FILE__, __LINE__)
#define xfree(x)	sh_xfree((x), __FILE__, __LINE__)

#ifdef free
#undef free
#endif
#define free(x)		sh_xfree((x), __FILE__, __LINE__)

extern PTR_T sh_malloc PARAMS((size_t, const char *, int));

#ifdef malloc
#undef malloc
#endif
#define malloc(x)	sh_malloc((x), __FILE__, __LINE__)

#endif	/* USING_BASH_MALLOC */

#endif	/* _XMALLOC_H_ */
