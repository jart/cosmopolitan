/* xmalloc.c -- safe versions of malloc and realloc */

/* Copyright (C) 1991-2017 Free Software Foundation, Inc.

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

#include <stdio.h>

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#include "xmalloc.h"

/* **************************************************************** */
/*								    */
/*		   Memory Allocation and Deallocation.		    */
/*								    */
/* **************************************************************** */

static void
memory_error_and_abort (char *fname)
{
  fprintf (stderr, "%s: out of virtual memory\n", fname);
  exit (2);
}

/* Return a pointer to free()able block of memory large enough
   to hold BYTES number of bytes.  If the memory cannot be allocated,
   print an error message and abort. */
PTR_T
xmalloc (size_t bytes)
{
  PTR_T temp;

  temp = malloc (bytes);
  if (temp == 0)
    memory_error_and_abort ("xmalloc");
  return (temp);
}

PTR_T
xrealloc (PTR_T pointer, size_t bytes)
{
  PTR_T temp;

  temp = pointer ? realloc (pointer, bytes) : malloc (bytes);

  if (temp == 0)
    memory_error_and_abort ("xrealloc");
  return (temp);
}
