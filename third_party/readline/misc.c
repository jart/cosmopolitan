/* misc.c -- miscellaneous bindable readline functions. */

/* Copyright (C) 1987-2022 Free Software Foundation, Inc.

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

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#if defined (HAVE_LOCALE_H)
#  include <locale.h>
#endif

#include <stdio.h>

/* System-specific feature definitions and include files. */
#include "rldefs.h"
#include "rlmbutil.h"

/* Some standard library routines. */
#include "readline.h"
#include "history.h"

#include "rlprivate.h"
#include "histlib.h"
#include "rlshell.h"
#include "xmalloc.h"

static int rl_digit_loop (void);
static void _rl_history_set_point (void);

/* If non-zero, rl_get_previous_history and rl_get_next_history attempt
   to preserve the value of rl_point from line to line. */
int _rl_history_preserve_point = 0;

_rl_arg_cxt _rl_argcxt;

/* Saved target point for when _rl_history_preserve_point is set.  Special
   value of -1 means that point is at the end of the line. */
int _rl_history_saved_point = -1;

/* **************************************************************** */
/*								    */
/*			Numeric Arguments			    */
/*								    */
/* **************************************************************** */

int
_rl_arg_overflow (void)
{
  if (rl_numeric_arg > 1000000)
    {
      _rl_argcxt = 0;
      rl_explicit_arg = rl_numeric_arg = 0;
      rl_ding ();
      rl_restore_prompt ();
      rl_clear_message ();
      RL_UNSETSTATE(RL_STATE_NUMERICARG);
      return 1;
    }
  return 0;
}

void
_rl_arg_init (void)
{
  rl_save_prompt ();
  _rl_argcxt = 0;
  RL_SETSTATE(RL_STATE_NUMERICARG);
}

int
_rl_arg_getchar (void)
{
  int c;

  rl_message ("(arg: %d) ", rl_arg_sign * rl_numeric_arg);
  RL_SETSTATE(RL_STATE_MOREINPUT);
  c = rl_read_key ();
  RL_UNSETSTATE(RL_STATE_MOREINPUT);

  return c;
}

/* Process C as part of the current numeric argument.  Return -1 if the
   argument should be aborted, 0 if we should not read any more chars, and
   1 if we should continue to read chars. */
int
_rl_arg_dispatch (_rl_arg_cxt cxt, int c)
{
  int key, r;

  key = c;

  /* If we see a key bound to `universal-argument' after seeing digits,
      it ends the argument but is otherwise ignored. */
  if (c >= 0 && _rl_keymap[c].type == ISFUNC && _rl_keymap[c].function == rl_universal_argument)
    {
      if ((cxt & NUM_SAWDIGITS) == 0)
	{
	  rl_numeric_arg *= 4;
	  return 1;
	}
      else if (RL_ISSTATE (RL_STATE_CALLBACK))
        {
          _rl_argcxt |= NUM_READONE;
          return 0;	/* XXX */
        }
      else
	{
	  key = _rl_bracketed_read_key ();
	  rl_restore_prompt ();
	  rl_clear_message ();
	  RL_UNSETSTATE(RL_STATE_NUMERICARG);
	  if (key < 0)
	    return -1;
	  return (_rl_dispatch (key, _rl_keymap));
	}
    }

  c = UNMETA (c);

  if (_rl_digit_p (c))
    {
      r = _rl_digit_value (c);    	
      rl_numeric_arg = rl_explicit_arg ? (rl_numeric_arg * 10) +  r : r;
      rl_explicit_arg = 1;
      _rl_argcxt |= NUM_SAWDIGITS;
    }
  else if (c == '-' && rl_explicit_arg == 0)
    {
      rl_numeric_arg = 1;
      _rl_argcxt |= NUM_SAWMINUS;
      rl_arg_sign = -1;
    }
  else
    {
      /* Make M-- command equivalent to M--1 command. */
      if ((_rl_argcxt & NUM_SAWMINUS) && rl_numeric_arg == 1 && rl_explicit_arg == 0)
	rl_explicit_arg = 1;
      rl_restore_prompt ();
      rl_clear_message ();
      RL_UNSETSTATE(RL_STATE_NUMERICARG);

      r = _rl_dispatch (key, _rl_keymap);
      if (RL_ISSTATE (RL_STATE_CALLBACK))
	{
	  /* At worst, this will cause an extra redisplay.  Otherwise,
	     we have to wait until the next character comes in. */
	  if (rl_done == 0)
	    (*rl_redisplay_function) ();
	  r = 0;
	}
      return r;
    }

  return 1;
}

/* Handle C-u style numeric args, as well as M--, and M-digits. */
static int
rl_digit_loop (void)
{
  int c, r;

  while (1)
    {
      if (_rl_arg_overflow ())
	return 1;

      c = _rl_arg_getchar ();

      if (c < 0)
	{
	  _rl_abort_internal ();
	  return -1;
	}

      r = _rl_arg_dispatch (_rl_argcxt, c);
      if (r <= 0 || (RL_ISSTATE (RL_STATE_NUMERICARG) == 0))
        break;
    }

  return r;
}

/* Create a default argument. */
void
_rl_reset_argument (void)
{
  rl_numeric_arg = rl_arg_sign = 1;
  rl_explicit_arg = 0;
  _rl_argcxt = 0;
}

/* Start a numeric argument with initial value KEY */
int
rl_digit_argument (int ignore, int key)
{
  _rl_arg_init ();
  if (RL_ISSTATE (RL_STATE_CALLBACK))
    {
      _rl_arg_dispatch (_rl_argcxt, key);
      rl_message ("(arg: %d) ", rl_arg_sign * rl_numeric_arg);
      return 0;
    }
  else
    {
      rl_execute_next (key);
      return (rl_digit_loop ());
    }
}

/* C-u, universal argument.  Multiply the current argument by 4.
   Read a key.  If the key has nothing to do with arguments, then
   dispatch on it.  If the key is the abort character then abort. */
int
rl_universal_argument (int count, int key)
{
  _rl_arg_init ();
  rl_numeric_arg *= 4;

  return (RL_ISSTATE (RL_STATE_CALLBACK) ? 0 : rl_digit_loop ());
}

int
_rl_arg_callback (_rl_arg_cxt cxt)
{
  int c, r;

  c = _rl_arg_getchar ();
  if (c < 0)
    return (1);		/* EOF */

  if (_rl_argcxt & NUM_READONE)
    {
      _rl_argcxt &= ~NUM_READONE;
      rl_restore_prompt ();
      rl_clear_message ();
      RL_UNSETSTATE(RL_STATE_NUMERICARG);
      rl_execute_next (c);
      return 0;
    }

  r = _rl_arg_dispatch (cxt, c);
  if (r > 0)
    rl_message ("(arg: %d) ", rl_arg_sign * rl_numeric_arg);
  return (r != 1);
}

/* What to do when you abort reading an argument. */
int
rl_discard_argument (void)
{
  rl_ding ();
  rl_clear_message ();
  _rl_reset_argument ();

  return 0;
}

/* **************************************************************** */
/*								    */
/*			History Utilities			    */
/*								    */
/* **************************************************************** */

/* We already have a history library, and that is what we use to control
   the history features of readline.  This is our local interface to
   the history mechanism. */

/* While we are editing the history, this is the saved
   version of the original line. */
HIST_ENTRY *_rl_saved_line_for_history = (HIST_ENTRY *)NULL;

/* Set the history pointer back to the last entry in the history. */
void
_rl_start_using_history (void)
{
  using_history ();
  if (_rl_saved_line_for_history)
    _rl_free_saved_history_line ();
  _rl_saved_line_for_history = (HIST_ENTRY *)NULL;
  _rl_history_search_pos = -99;		/* some random invalid history position */
}

/* Free the contents (and containing structure) of a HIST_ENTRY. */
void
_rl_free_history_entry (HIST_ENTRY *entry)
{
  if (entry == 0)
    return;

  FREE (entry->line);
  FREE (entry->timestamp);

  xfree (entry);
}

/* Perhaps put back the current line if it has changed. */
int
rl_maybe_replace_line (void)
{
  HIST_ENTRY *temp;

  temp = current_history ();
  /* If the current line has changed, save the changes. */
  if (temp && ((UNDO_LIST *)(temp->data) != rl_undo_list))
    {
      temp = replace_history_entry (where_history (), rl_line_buffer, (histdata_t)rl_undo_list);
      xfree (temp->line);
      FREE (temp->timestamp);
      xfree (temp);
    }
  return 0;
}

/* Restore the _rl_saved_line_for_history if there is one. */
int
rl_maybe_unsave_line (void)
{
  if (_rl_saved_line_for_history)
    {
      /* Can't call with `1' because rl_undo_list might point to an undo
	 list from a history entry, as in rl_replace_from_history() below. */
      rl_replace_line (_rl_saved_line_for_history->line, 0);
      rl_undo_list = (UNDO_LIST *)_rl_saved_line_for_history->data;

      /* Doesn't free `data'. */
      _rl_free_history_entry (_rl_saved_line_for_history);
      _rl_saved_line_for_history = (HIST_ENTRY *)NULL;
      rl_point = rl_end;	/* rl_replace_line sets rl_end */
    }
  else
    rl_ding ();
  return 0;
}

/* Save the current line in _rl_saved_line_for_history. */
int
rl_maybe_save_line (void)
{
  if (_rl_saved_line_for_history == 0)
    {
      _rl_saved_line_for_history = (HIST_ENTRY *)xmalloc (sizeof (HIST_ENTRY));
      _rl_saved_line_for_history->line = savestring (rl_line_buffer);
      _rl_saved_line_for_history->timestamp = (char *)NULL;
      _rl_saved_line_for_history->data = (char *)rl_undo_list;
    }

  return 0;
}

int
_rl_free_saved_history_line (void)
{
  UNDO_LIST *orig;

  if (_rl_saved_line_for_history)
    {
      if (rl_undo_list && rl_undo_list == (UNDO_LIST *)_rl_saved_line_for_history->data)
	rl_undo_list = 0;
      /* Have to free this separately because _rl_free_history entry can't:
	 it doesn't know whether or not this has application data. Only the
	 callers that know this is _rl_saved_line_for_history can know that
	 it's an undo list. */
      if (_rl_saved_line_for_history->data)
	_rl_free_undo_list ((UNDO_LIST *)_rl_saved_line_for_history->data);
      _rl_free_history_entry (_rl_saved_line_for_history);
      _rl_saved_line_for_history = (HIST_ENTRY *)NULL;
    }
  return 0;
}

static void
_rl_history_set_point (void)
{
  rl_point = (_rl_history_preserve_point && _rl_history_saved_point != -1)
		? _rl_history_saved_point
		: rl_end;
  if (rl_point > rl_end)
    rl_point = rl_end;

#if defined (VI_MODE)
  if (rl_editing_mode == vi_mode && _rl_keymap != vi_insertion_keymap)
    rl_point = 0;
#endif /* VI_MODE */

  if (rl_editing_mode == emacs_mode)
    rl_mark = (rl_point == rl_end ? 0 : rl_end);
}

void
rl_replace_from_history (HIST_ENTRY *entry, int flags)
{
  /* Can't call with `1' because rl_undo_list might point to an undo list
     from a history entry, just like we're setting up here. */
  rl_replace_line (entry->line, 0);
  rl_undo_list = (UNDO_LIST *)entry->data;
  rl_point = rl_end;
  rl_mark = 0;

#if defined (VI_MODE)
  if (rl_editing_mode == vi_mode)
    {
      rl_point = 0;
      rl_mark = rl_end;
    }
#endif
}

/* Process and free undo lists attached to each history entry prior to the
   current entry, inclusive, reverting each line to its saved state.  This 
   is destructive, and state about the current line is lost.  This is not
   intended to be called while actively editing, and the current line is
   not assumed to have been added to the history list. */
void
_rl_revert_previous_lines (void)
{
  int hpos;
  HIST_ENTRY *entry;
  UNDO_LIST *ul, *saved_undo_list;
  char *lbuf;

  lbuf = savestring (rl_line_buffer);
  saved_undo_list = rl_undo_list;
  hpos = where_history ();

  entry = (hpos == history_length) ? previous_history () : current_history ();
  while (entry)
    {
      if (ul = (UNDO_LIST *)entry->data)
	{
	  if (ul == saved_undo_list)
	    saved_undo_list = 0;
	  /* Set up rl_line_buffer and other variables from history entry */
	  rl_replace_from_history (entry, 0);	/* entry->line is now current */
	  entry->data = 0;			/* entry->data is now current undo list */
	  /* Undo all changes to this history entry */
	  while (rl_undo_list)
	    rl_do_undo ();
	  /* And copy the reverted line back to the history entry, preserving
	     the timestamp. */
	  FREE (entry->line);
	  entry->line = savestring (rl_line_buffer);
	}
      entry = previous_history ();
    }

  /* Restore history state */
  rl_undo_list = saved_undo_list;	/* may have been set to null */
  history_set_pos (hpos);
  
  /* reset the line buffer */
  rl_replace_line (lbuf, 0);
  _rl_set_the_line ();

  /* and clean up */
  xfree (lbuf);
}  

/* Revert all lines in the history by making sure we are at the end of the
   history before calling _rl_revert_previous_lines() */
void
_rl_revert_all_lines (void)
{
  int pos;

  pos = where_history ();
  using_history ();
  _rl_revert_previous_lines ();
  history_set_pos (pos);
}

/* Free the history list, including private readline data and take care
   of pointer aliases to history data.  Resets rl_undo_list if it points
   to an UNDO_LIST * saved as some history entry's data member.  This
   should not be called while editing is active. */
void
rl_clear_history (void)
{
  HIST_ENTRY **hlist, *hent;
  register int i;
  UNDO_LIST *ul, *saved_undo_list;

  saved_undo_list = rl_undo_list;
  hlist = history_list ();		/* direct pointer, not copy */

  for (i = 0; i < history_length; i++)
    {
      hent = hlist[i];
      if (ul = (UNDO_LIST *)hent->data)
	{
	  if (ul == saved_undo_list)
	    saved_undo_list = 0;
	  _rl_free_undo_list (ul);
	  hent->data = 0;
	}
      _rl_free_history_entry (hent);
    }

  history_offset = history_length = 0;
  rl_undo_list = saved_undo_list;	/* should be NULL */
}

/* **************************************************************** */
/*								    */
/*			History Commands			    */
/*								    */
/* **************************************************************** */

/* Meta-< goes to the start of the history. */
int
rl_beginning_of_history (int count, int key)
{
  return (rl_get_previous_history (1 + where_history (), key));
}

/* Meta-> goes to the end of the history.  (The current line). */
int
rl_end_of_history (int count, int key)
{
  rl_maybe_replace_line ();
  using_history ();
  rl_maybe_unsave_line ();
  return 0;
}

/* Move down to the next history line. */
int
rl_get_next_history (int count, int key)
{
  HIST_ENTRY *temp;

  if (count < 0)
    return (rl_get_previous_history (-count, key));

  if (count == 0)
    return 0;

  rl_maybe_replace_line ();

  /* either not saved by rl_newline or at end of line, so set appropriately. */
  if (_rl_history_saved_point == -1 && (rl_point || rl_end))
    _rl_history_saved_point = (rl_point == rl_end) ? -1 : rl_point;

  temp = (HIST_ENTRY *)NULL;
  while (count)
    {
      temp = next_history ();
      if (!temp)
	break;
      --count;
    }

  if (temp == 0)
    rl_maybe_unsave_line ();
  else
    {
      rl_replace_from_history (temp, 0);
      _rl_history_set_point ();
    }
  return 0;
}

/* Get the previous item out of our interactive history, making it the current
   line.  If there is no previous history, just ding. */
int
rl_get_previous_history (int count, int key)
{
  HIST_ENTRY *old_temp, *temp;
  int had_saved_line;

  if (count < 0)
    return (rl_get_next_history (-count, key));

  if (count == 0 || history_list () == 0)
    return 0;

  /* either not saved by rl_newline or at end of line, so set appropriately. */
  if (_rl_history_saved_point == -1 && (rl_point || rl_end))
    _rl_history_saved_point = (rl_point == rl_end) ? -1 : rl_point;

  /* If we don't have a line saved, then save this one. */
  had_saved_line = _rl_saved_line_for_history != 0;
  rl_maybe_save_line ();

  /* If the current line has changed, save the changes. */
  rl_maybe_replace_line ();

  temp = old_temp = (HIST_ENTRY *)NULL;
  while (count)
    {
      temp = previous_history ();
      if (temp == 0)
	break;

      old_temp = temp;
      --count;
    }

  /* If there was a large argument, and we moved back to the start of the
     history, that is not an error.  So use the last value found. */
  if (!temp && old_temp)
    temp = old_temp;

  if (temp == 0)
    {
      if (had_saved_line == 0)
	_rl_free_saved_history_line ();
      rl_ding ();
    }
  else
    {
      rl_replace_from_history (temp, 0);
      _rl_history_set_point ();
    }

  return 0;
}

/* With an argument, move back that many history lines, else move to the
   beginning of history. */
int
rl_fetch_history (int count, int c)
{
  int wanted, nhist;

  /* Giving an argument of n means we want the nth command in the history
     file.  The command number is interpreted the same way that the bash
     `history' command does it -- that is, giving an argument count of 450
     to this command would get the command listed as number 450 in the
     output of `history'. */
  if (rl_explicit_arg)
    {
      nhist = history_base + where_history ();
      /* Negative arguments count back from the end of the history list. */
      wanted = (count >= 0) ? nhist - count : -count;

      if (wanted <= 0 || wanted >= nhist)
	{
	  /* In vi mode, we don't change the line with an out-of-range
	     argument, as for the `G' command. */
	  if (rl_editing_mode == vi_mode)
	    rl_ding ();
	  else
	    rl_beginning_of_history (0, 0);
	}
      else
        rl_get_previous_history (wanted, c);
    }
  else
    rl_beginning_of_history (count, 0);

  return (0);
}

/* The equivalent of the Korn shell C-o operate-and-get-next-history-line
   editing command. */

/* This could stand to be global to the readline library */
static rl_hook_func_t *_rl_saved_internal_startup_hook = 0;
static int saved_history_logical_offset = -1;

#define HISTORY_FULL() (history_is_stifled () && history_length >= history_max_entries)

static int
set_saved_history ()
{
  int absolute_offset, count;

  if (saved_history_logical_offset >= 0)
    {
      absolute_offset = saved_history_logical_offset - history_base;
      count = where_history () - absolute_offset;
      rl_get_previous_history (count, 0);
    }
  saved_history_logical_offset = -1;
  _rl_internal_startup_hook = _rl_saved_internal_startup_hook;

  return (0);
}

int
rl_operate_and_get_next (int count, int c)
{
  /* Accept the current line. */
  rl_newline (1, c);

  saved_history_logical_offset = rl_explicit_arg ? count : where_history () + history_base + 1;

  _rl_saved_internal_startup_hook = _rl_internal_startup_hook;
  _rl_internal_startup_hook = set_saved_history;

  return 0;
}

/* **************************************************************** */
/*								    */
/*			    Editing Modes			    */
/*								    */
/* **************************************************************** */
/* How to toggle back and forth between editing modes. */
int
rl_vi_editing_mode (int count, int key)
{
#if defined (VI_MODE)
  _rl_set_insert_mode (RL_IM_INSERT, 1);	/* vi mode ignores insert mode */
  rl_editing_mode = vi_mode;
  rl_vi_insert_mode (1, key);
#endif /* VI_MODE */

  return 0;
}

int
rl_emacs_editing_mode (int count, int key)
{
  rl_editing_mode = emacs_mode;
  _rl_set_insert_mode (RL_IM_INSERT, 1); /* emacs mode default is insert mode */
  _rl_keymap = emacs_standard_keymap;

  if (_rl_show_mode_in_prompt)
    _rl_reset_prompt ();

  return 0;
}

/* Function for the rest of the library to use to set insert/overwrite mode. */
void
_rl_set_insert_mode (int im, int force)
{
#ifdef CURSOR_MODE
  _rl_set_cursor (im, force);
#endif

  rl_insert_mode = im;
}

/* Toggle overwrite mode.  A positive explicit argument selects overwrite
   mode.  A negative or zero explicit argument selects insert mode. */
int
rl_overwrite_mode (int count, int key)
{
  if (rl_explicit_arg == 0)
    _rl_set_insert_mode (rl_insert_mode ^ 1, 0);
  else if (count > 0)
    _rl_set_insert_mode (RL_IM_OVERWRITE, 0);
  else
    _rl_set_insert_mode (RL_IM_INSERT, 0);

  return 0;
}
