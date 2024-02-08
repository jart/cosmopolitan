/* compat.c -- backwards compatibility functions. */

/* Copyright (C) 2000-2021 Free Software Foundation, Inc.

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
#  include "config.h"
#endif

#include <stdio.h>

#include "rlstdc.h"
#include "rltypedefs.h"

extern void rl_free_undo_list (void);
extern int rl_maybe_save_line (void);
extern int rl_maybe_unsave_line (void);
extern int rl_maybe_replace_line (void);

extern int rl_crlf (void);
extern int rl_ding (void);
extern int rl_alphabetic (int);

extern char **rl_completion_matches (const char *, rl_compentry_func_t *);
extern char *rl_username_completion_function (const char *, int);
extern char *rl_filename_completion_function (const char *, int);

/* Provide backwards-compatible entry points for old function names. */

void
free_undo_list (void)
{
  rl_free_undo_list ();
}

int
maybe_replace_line (void)
{
  return rl_maybe_replace_line ();
}

int
maybe_save_line (void)
{
  return rl_maybe_save_line ();
}

int
maybe_unsave_line (void)
{
  return rl_maybe_unsave_line ();
}

int
ding (void)
{
  return rl_ding ();
}

int
crlf (void)
{
  return rl_crlf ();
}

int
alphabetic (int c)
{
  return rl_alphabetic (c);
}

char **
completion_matches (const char *s, rl_compentry_func_t *f)
{
  return rl_completion_matches (s, f);
}

char *
username_completion_function (const char *s, int i)
{
  return rl_username_completion_function (s, i);
}

char *
filename_completion_function (const char *s, int i)
{
  return rl_filename_completion_function (s, i);
}
