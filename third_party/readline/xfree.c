/* xfree.c -- safe version of free that ignores attempts to free NUL */

/* Copyright (C) 1991-2010,2017 Free Software Foundation, Inc.

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

#define READLINE_LIBRARY

#if defined (HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#include "xmalloc.h"

/* **************************************************************** */
/*								    */
/*		   Memory Deallocation.				    */
/*								    */
/* **************************************************************** */

/* Use this as the function to call when adding unwind protects so we
   don't need to know what free() returns. */
void
xfree (PTR_T string)
{
  if (string)
    free (string);
}
