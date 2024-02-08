/* xmalloc.h -- memory allocation that aborts on errors. */

/* Copyright (C) 1999-2009,2010-2021 Free Software Foundation, Inc.

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

#if !defined (_XMALLOC_H_)
#define _XMALLOC_H_

#if defined (READLINE_LIBRARY)
#  include "rlstdc.h"
#else
#  include "third_party/readline/rlstdc.h"
#endif

#ifndef PTR_T

#ifdef __STDC__
#  define PTR_T	void *
#else
#  define PTR_T	char *
#endif

#endif /* !PTR_T */

extern PTR_T xmalloc (size_t);
extern PTR_T xrealloc (void *, size_t);
extern void xfree (void *);

#endif /* _XMALLOC_H_ */
