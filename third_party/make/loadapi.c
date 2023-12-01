/* API for GNU Make dynamic objects.
Copyright (C) 2013-2023 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "makeint.h"

#include "filedef.h"
#include "variable.h"
#include "dep.h"

/* Allocate a buffer in our context, so we can free it.  */
char *
gmk_alloc (unsigned int len)
{
  return xmalloc (len);
}

/* Free a buffer returned by gmk_expand().  */
void
gmk_free (char *s)
{
  free (s);
}

/* Evaluate a buffer as make syntax.
   Ideally eval_buffer() will take const char *, but not yet.  */
void
gmk_eval (const char *buffer, const gmk_floc *gfloc)
{
  /* Preserve existing variable buffer context.  */
  char *pbuf;
  size_t plen;
  char *s;
  floc fl;
  floc *flp;

  if (gfloc)
    {
      fl.filenm = gfloc->filenm;
      fl.lineno = gfloc->lineno;
      fl.offset = 0;
      flp = &fl;
    }
  else
    flp = NULL;

  install_variable_buffer (&pbuf, &plen);

  s = xstrdup (buffer);
  eval_buffer (s, flp);
  free (s);

  restore_variable_buffer (pbuf, plen);
}

/* Expand a string and return an allocated buffer.
   Caller must call gmk_free() with this buffer.  */
char *
gmk_expand (const char *ref)
{
  return allocated_variable_expand (ref);
}

/* Register a function to be called from makefiles.  */
void
gmk_add_function (const char *name, gmk_func_ptr func,
                  unsigned int min, unsigned int max, unsigned int flags)
{
  define_new_function (reading_file, name, min, max, flags, func);
}
