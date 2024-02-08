/* keymaps.c -- Functions and keymaps for the GNU Readline library. */

/* Copyright (C) 1988,1989-2009,2017 Free Software Foundation, Inc.

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

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#include <stdio.h>	/* for FILE * definition for readline.h */

#include "readline.h"
#include "rlconf.h"

#include "emacs_keymap.inc"

#if defined (VI_MODE)
#include "vi_keymap.inc"
#endif

#include "xmalloc.h"

/* **************************************************************** */
/*								    */
/*		      Functions for manipulating Keymaps.	    */
/*								    */
/* **************************************************************** */


/* Return a new, empty keymap.
   Free it with free() when you are done. */
Keymap
rl_make_bare_keymap (void)
{
  register int i;
  Keymap keymap;

  keymap = (Keymap)xmalloc (KEYMAP_SIZE * sizeof (KEYMAP_ENTRY));
  for (i = 0; i < KEYMAP_SIZE; i++)
    {
      keymap[i].type = ISFUNC;
      keymap[i].function = (rl_command_func_t *)NULL;
    }

#if 0
  for (i = 'A'; i < ('Z' + 1); i++)
    {
      keymap[i].type = ISFUNC;
      keymap[i].function = rl_do_lowercase_version;
    }
#endif

  return (keymap);
}

/* A convenience function that returns 1 if there are no keys bound to
   functions in KEYMAP */
int
rl_empty_keymap (Keymap keymap)
{
  int i;

  for (i = 0; i < ANYOTHERKEY; i++)
    {
      if (keymap[i].type != ISFUNC || keymap[i].function)
	return 0;
    }
  return 1;
}

/* Return a new keymap which is a copy of MAP.  Just copies pointers, does
   not copy text of macros or descend into child keymaps. */
Keymap
rl_copy_keymap (Keymap map)
{
  register int i;
  Keymap temp;

  temp = rl_make_bare_keymap ();
  for (i = 0; i < KEYMAP_SIZE; i++)
    {
      temp[i].type = map[i].type;
      temp[i].function = map[i].function;
    }
  return (temp);
}

/* Return a new keymap with the printing characters bound to rl_insert,
   the uppercase Meta characters bound to run their lowercase equivalents,
   and the Meta digits bound to produce numeric arguments. */
Keymap
rl_make_keymap (void)
{
  register int i;
  Keymap newmap;

  newmap = rl_make_bare_keymap ();

  /* All ASCII printing characters are self-inserting. */
  for (i = ' '; i < 127; i++)
    newmap[i].function = rl_insert;

  newmap[TAB].function = rl_insert;
  newmap[RUBOUT].function = rl_rubout;	/* RUBOUT == 127 */
  newmap[CTRL('H')].function = rl_rubout;

#if KEYMAP_SIZE > 128
  /* Printing characters in ISO Latin-1 and some 8-bit character sets. */
  for (i = 128; i < 256; i++)
    newmap[i].function = rl_insert;
#endif /* KEYMAP_SIZE > 128 */

  return (newmap);
}

/* Free the storage associated with MAP. */
void
rl_discard_keymap (Keymap map)
{
  int i;

  if (map == 0)
    return;

  for (i = 0; i < KEYMAP_SIZE; i++)
    {
      switch (map[i].type)
	{
	case ISFUNC:
	  break;

	case ISKMAP:
	  rl_discard_keymap ((Keymap)map[i].function);
	  xfree ((char *)map[i].function);
	  break;

	case ISMACR:
	  xfree ((char *)map[i].function);
	  break;
	}
    }
}

/* Convenience function that discards, then frees, MAP. */
void
rl_free_keymap (Keymap map)
{
  rl_discard_keymap (map);
  xfree ((char *)map);
}
