/* isearch.c - incremental searching */

/* **************************************************************** */
/*								    */
/*			I-Search and Searching			    */
/*								    */
/* **************************************************************** */

/* Copyright (C) 1987-2021 Free Software Foundation, Inc.

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

#include <sys/types.h>

#include <stdio.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif

#include "rldefs.h"
#include "rlmbutil.h"

#include "readline.h"
#include "history.h"

#include "rlprivate.h"
#include "xmalloc.h"

/* Variables exported to other files in the readline library. */
char *_rl_isearch_terminators = (char *)NULL;

_rl_search_cxt *_rl_iscxt = 0;

static int rl_search_history (int, int);

static _rl_search_cxt *_rl_isearch_init (int);
static void _rl_isearch_fini (_rl_search_cxt *);

/* Last line found by the current incremental search, so we don't `find'
   identical lines many times in a row.  Now part of isearch context. */
/* static char *prev_line_found; */

/* Last search string and its length. */
static char *last_isearch_string;
static int last_isearch_string_len;

static char * const default_isearch_terminators = "\033\012";

_rl_search_cxt *
_rl_scxt_alloc (int type, int flags)
{
  _rl_search_cxt *cxt;

  cxt = (_rl_search_cxt *)xmalloc (sizeof (_rl_search_cxt));

  cxt->type = type;
  cxt->sflags = flags;

  cxt->search_string = 0;
  cxt->search_string_size = cxt->search_string_index = 0;

  cxt->lines = 0;
  cxt->allocated_line = 0;
  cxt->hlen = cxt->hindex = 0;

  cxt->save_point = rl_point;
  cxt->save_mark = rl_mark;
  cxt->save_line = where_history ();
  cxt->last_found_line = cxt->save_line;
  cxt->prev_line_found = 0;

  cxt->save_undo_list = 0;

  cxt->keymap = _rl_keymap;
  cxt->okeymap = _rl_keymap;

  cxt->history_pos = 0;
  cxt->direction = 0;

  cxt->prevc = cxt->lastc = 0;

  cxt->sline = 0;
  cxt->sline_len = cxt->sline_index = 0;

  cxt->search_terminators = 0;

  return cxt;
}

void
_rl_scxt_dispose (_rl_search_cxt *cxt, int flags)
{
  FREE (cxt->search_string);
  FREE (cxt->allocated_line);
  FREE (cxt->lines);

  xfree (cxt);
}

/* Search backwards through the history looking for a string which is typed
   interactively.  Start with the current line. */
int
rl_reverse_search_history (int sign, int key)
{
  return (rl_search_history (-sign, key));
}

/* Search forwards through the history looking for a string which is typed
   interactively.  Start with the current line. */
int
rl_forward_search_history (int sign, int key)
{
  return (rl_search_history (sign, key));
}

/* Display the current state of the search in the echo-area.
   SEARCH_STRING contains the string that is being searched for,
   DIRECTION is zero for forward, or non-zero for reverse,
   WHERE is the history list number of the current line.  If it is
   -1, then this line is the starting one. */
static void
rl_display_search (char *search_string, int flags, int where)
{
  char *message;
  int msglen, searchlen;

  searchlen = (search_string && *search_string) ? strlen (search_string) : 0;

  message = (char *)xmalloc (searchlen + 64);
  msglen = 0;

#if defined (NOTDEF)
  if (where != -1)
    {
      sprintf (message, "[%d]", where + history_base);
      msglen = strlen (message);
    }
#endif /* NOTDEF */

  message[msglen++] = '(';

  if (flags & SF_FAILED)
    {
      strcpy (message + msglen, "failed ");
      msglen += 7;
    }

  if (flags & SF_REVERSE)
    {
      strcpy (message + msglen, "reverse-");
      msglen += 8;
    }

  strcpy (message + msglen, "i-search)`");
  msglen += 10;

  if (search_string && *search_string)
    {
      strcpy (message + msglen, search_string);
      msglen += searchlen;
    }
  else
    _rl_optimize_redisplay ();

  strcpy (message + msglen, "': ");

  rl_message ("%s", message);
  xfree (message);
#if 0
  /* rl_message calls this */
  (*rl_redisplay_function) ();
#endif
}

static _rl_search_cxt *
_rl_isearch_init (int direction)
{
  _rl_search_cxt *cxt;
  register int i;
  HIST_ENTRY **hlist;

  cxt = _rl_scxt_alloc (RL_SEARCH_ISEARCH, 0);
  if (direction < 0)
    cxt->sflags |= SF_REVERSE;

  cxt->search_terminators = _rl_isearch_terminators ? _rl_isearch_terminators
						: default_isearch_terminators;

  /* Create an array of pointers to the lines that we want to search. */
  hlist = history_list ();
  rl_maybe_replace_line ();
  i = 0;
  if (hlist)
    for (i = 0; hlist[i]; i++);

  /* Allocate space for this many lines, +1 for the current input line,
     and remember those lines. */
  cxt->lines = (char **)xmalloc ((1 + (cxt->hlen = i)) * sizeof (char *));
  for (i = 0; i < cxt->hlen; i++)
    cxt->lines[i] = hlist[i]->line;

  if (_rl_saved_line_for_history)
    cxt->lines[i] = _rl_saved_line_for_history->line;
  else
    {
      /* Keep track of this so we can free it. */
      cxt->allocated_line = (char *)xmalloc (1 + strlen (rl_line_buffer));
      strcpy (cxt->allocated_line, &rl_line_buffer[0]);
      cxt->lines[i] = cxt->allocated_line;
    }

  cxt->hlen++;

  /* The line where we start the search. */
  cxt->history_pos = cxt->save_line;

  rl_save_prompt ();

  /* Initialize search parameters. */
  cxt->search_string = (char *)xmalloc (cxt->search_string_size = 128);
  cxt->search_string[cxt->search_string_index = 0] = '\0';

  /* Normalize DIRECTION into 1 or -1. */
  cxt->direction = (direction >= 0) ? 1 : -1;

  cxt->sline = rl_line_buffer;
  cxt->sline_len = strlen (cxt->sline);
  cxt->sline_index = rl_point;

  _rl_iscxt = cxt;		/* save globally */

  /* experimental right now */
  _rl_init_executing_keyseq ();

  return cxt;
}

static void
_rl_isearch_fini (_rl_search_cxt *cxt)
{
  /* First put back the original state. */
  rl_replace_line (cxt->lines[cxt->save_line], 0);

  rl_restore_prompt ();

  /* Save the search string for possible later use. */
  FREE (last_isearch_string);
  last_isearch_string = cxt->search_string;
  last_isearch_string_len = cxt->search_string_index;
  cxt->search_string = 0;
  cxt->search_string_size = 0;
  cxt->search_string_index = 0;

  if (cxt->last_found_line < cxt->save_line)
    rl_get_previous_history (cxt->save_line - cxt->last_found_line, 0);
  else
    rl_get_next_history (cxt->last_found_line - cxt->save_line, 0);

  /* If the string was not found, put point at the end of the last matching
     line.  If last_found_line == orig_line, we didn't find any matching
     history lines at all, so put point back in its original position. */
  if (cxt->sline_index < 0)
    {
      if (cxt->last_found_line == cxt->save_line)
	cxt->sline_index = cxt->save_point;
      else
	cxt->sline_index = strlen (rl_line_buffer);
      rl_mark = cxt->save_mark;
      rl_deactivate_mark ();
    }

  rl_point = cxt->sline_index;
  /* Don't worry about where to put the mark here; rl_get_previous_history
     and rl_get_next_history take care of it.
     If we want to highlight the search string, this is where to set the
     point and mark to do it. */
  _rl_fix_point (0);
  rl_deactivate_mark ();

/*  _rl_optimize_redisplay (); */
  rl_clear_message ();
}

/* XXX - we could use _rl_bracketed_read_mbstring () here. */
int
_rl_search_getchar (_rl_search_cxt *cxt)
{
  int c;

  /* Read a key and decide how to proceed. */
  RL_SETSTATE(RL_STATE_MOREINPUT);
  c = cxt->lastc = rl_read_key ();
  RL_UNSETSTATE(RL_STATE_MOREINPUT);

#if defined (HANDLE_MULTIBYTE)
  /* This ends up with C (and LASTC) being set to the last byte of the
     multibyte character.  In most cases c == lastc == mb[0] */
  if (c >= 0 && MB_CUR_MAX > 1 && rl_byte_oriented == 0)
    c = cxt->lastc = _rl_read_mbstring (cxt->lastc, cxt->mb, MB_LEN_MAX);
#endif

  RL_CHECK_SIGNALS ();
  return c;
}

#define ENDSRCH_CHAR(c) \
  ((CTRL_CHAR (c) || META_CHAR (c) || (c) == RUBOUT) && ((c) != CTRL ('G')))

/* Process just-read character C according to isearch context CXT.  Return
   -1 if the caller should just free the context and return, 0 if we should
   break out of the loop, and 1 if we should continue to read characters. */
int
_rl_isearch_dispatch (_rl_search_cxt *cxt, int c)
{
  int n, wstart, wlen, limit, cval, incr;
  char *paste;
  size_t pastelen;
  int j;
  rl_command_func_t *f;

  f = (rl_command_func_t *)NULL;

  if (c < 0)
    {
      cxt->sflags |= SF_FAILED;
      cxt->history_pos = cxt->last_found_line;
      return -1;
    }

  _rl_add_executing_keyseq (c);

  /* XXX - experimental code to allow users to bracketed-paste into the search
     string even when ESC is one of the isearch-terminators. Not perfect yet. */
  if (_rl_enable_bracketed_paste && c == ESC && strchr (cxt->search_terminators, c) && (n = _rl_nchars_available ()) > (BRACK_PASTE_SLEN-1))
    {
      j = _rl_read_bracketed_paste_prefix (c);
      if (j == 1)
	{
	  cxt->lastc = -7;		/* bracketed paste, see below */
	  goto opcode_dispatch;	
        }
      else if (_rl_pushed_input_available ())	/* eat extra char we pushed back */
	c = cxt->lastc = rl_read_key ();
      else
	c = cxt->lastc;			/* last ditch */
    }

  /* If we are moving into a new keymap, modify cxt->keymap and go on.
     This can be a problem if c == ESC and we want to terminate the
     incremental search, so we check */
  if (c >= 0 && cxt->keymap[c].type == ISKMAP && strchr (cxt->search_terminators, cxt->lastc) == 0)
    {
      /* _rl_keyseq_timeout specified in milliseconds; _rl_input_queued
	 takes microseconds, so multiply by 1000.  If we don't get any
	 additional input and this keymap shadows another function, process
	 that key as if it was all we read. */
      if (_rl_keyseq_timeout > 0 &&
	    RL_ISSTATE (RL_STATE_CALLBACK) == 0 &&
	    RL_ISSTATE (RL_STATE_INPUTPENDING) == 0 &&
	    _rl_pushed_input_available () == 0 &&
	    ((Keymap)(cxt->keymap[c].function))[ANYOTHERKEY].function &&
	    _rl_input_queued (_rl_keyseq_timeout*1000) == 0)
	goto add_character;

      cxt->okeymap = cxt->keymap;
      cxt->keymap = FUNCTION_TO_KEYMAP (cxt->keymap, c);
      cxt->sflags |= SF_CHGKMAP;
      /* XXX - we should probably save this sequence, so we can do
	 something useful if this doesn't end up mapping to a command we
	 interpret here.  Right now we just save the most recent character
	 that caused the index into a new keymap. */
      cxt->prevc = c;
#if defined (HANDLE_MULTIBYTE)
      if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	{
	  if (cxt->mb[1] == 0)
	    {
	      cxt->pmb[0] = c;		/* XXX should be == cxt->mb[0] */
	      cxt->pmb[1] = '\0';
	    }
	  else
	    memcpy (cxt->pmb, cxt->mb, sizeof (cxt->pmb));
	}
#endif
      return 1;
    }

add_character:

  /* Translate the keys we do something with to opcodes. */
  if (c >= 0 && cxt->keymap[c].type == ISFUNC)
    {
      /* If we have a multibyte character, see if it's bound to something that
	 affects the search. */
#if defined (HANDLE_MULTIBYTE)
      if (MB_CUR_MAX > 1 && rl_byte_oriented == 0 && cxt->mb[1])
	f = rl_function_of_keyseq (cxt->mb, cxt->keymap, (int *)NULL);
      else
#endif
	{
	  f = cxt->keymap[c].function;
	  if (f == rl_do_lowercase_version)
	    f = cxt->keymap[_rl_to_lower (c)].function;
	}

      if (f == rl_reverse_search_history)
	cxt->lastc = (cxt->sflags & SF_REVERSE) ? -1 : -2;
      else if (f == rl_forward_search_history)
	cxt->lastc = (cxt->sflags & SF_REVERSE) ? -2 : -1;
      else if (f == rl_rubout)
	cxt->lastc = -3;
      else if (c == CTRL ('G') || f == rl_abort)
	cxt->lastc = -4;
      else if (c == CTRL ('W') || f == rl_unix_word_rubout)	/* XXX */
	cxt->lastc = -5;
      else if (c == CTRL ('Y') || f == rl_yank)	/* XXX */
	cxt->lastc = -6;
      else if (f == rl_bracketed_paste_begin)
	cxt->lastc = -7;
    }

  /* If we changed the keymap earlier while translating a key sequence into
     a command, restore it now that we've succeeded. */
  if (cxt->sflags & SF_CHGKMAP)
    {
      cxt->keymap = cxt->okeymap;
      cxt->sflags &= ~SF_CHGKMAP;
      /* If we indexed into a new keymap, but didn't map to a command that
	 affects the search (lastc > 0), and the character that mapped to a
	 new keymap would have ended the search (ENDSRCH_CHAR(cxt->prevc)),
	 handle that now as if the previous char would have ended the search
	 and we would have read the current character. */
      /* XXX - should we check cxt->mb? */
      if (cxt->lastc > 0 && ENDSRCH_CHAR (cxt->prevc))
	{
	  rl_stuff_char (cxt->lastc);
	  rl_execute_next (cxt->prevc);
	  /* XXX - do we insert everything in cxt->pmb? */
	  return (0);
	}
      /* Otherwise, if the current character is mapped to self-insert or
	 nothing (i.e., not an editing command), and the previous character
	 was a keymap index, then we need to insert both the previous
	 character and the current character into the search string. */
      else if (cxt->lastc > 0 && cxt->prevc > 0 &&
	       cxt->keymap[cxt->prevc].type == ISKMAP &&
	       (f == 0 || f == rl_insert))
	{
	  /* Make lastc be the next character read */
	  /* XXX - do we insert everything in cxt->mb? */
	  rl_execute_next (cxt->lastc);
	  /* Dispatch on the previous character (insert into search string) */
	  cxt->lastc = cxt->prevc;
#if defined (HANDLE_MULTIBYTE)
	  /* Have to overwrite cxt->mb here because dispatch uses it below */
	  if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	    {  
	      if (cxt->pmb[1] == 0)	  
		{
		  cxt->mb[0] = cxt->lastc;	/* == cxt->prevc */
		  cxt->mb[1] = '\0';
		}
	      else
		memcpy (cxt->mb, cxt->pmb, sizeof (cxt->mb));
	    }
#endif
	  cxt->prevc = 0;	  
	}
      else if (cxt->lastc > 0 && cxt->prevc > 0 && f && f != rl_insert)
	{
	  _rl_term_executing_keyseq ();		/* should this go in the caller? */

	  _rl_pending_command.map = cxt->keymap;
	  _rl_pending_command.count = 1;	/* XXX */
	  _rl_pending_command.key = cxt->lastc;
	  _rl_pending_command.func = f;
	  _rl_command_to_execute = &_rl_pending_command;

	  return (0);
	}
    }

  /* The characters in isearch_terminators (set from the user-settable
     variable isearch-terminators) are used to terminate the search but
     not subsequently execute the character as a command.  The default
     value is "\033\012" (ESC and C-J). */
  if (cxt->lastc > 0 && strchr (cxt->search_terminators, cxt->lastc))
    {
      /* ESC still terminates the search, but if there is pending
	 input or if input arrives within 0.1 seconds (on systems
	 with select(2)) it is used as a prefix character
	 with rl_execute_next.  WATCH OUT FOR THIS!  This is intended
	 to allow the arrow keys to be used like ^F and ^B are used
	 to terminate the search and execute the movement command.
	 XXX - since _rl_input_available depends on the application-
	 settable keyboard timeout value, this could alternatively
	 use _rl_input_queued(100000) */
      if (cxt->lastc == ESC && (_rl_pushed_input_available () || _rl_input_available ()))
	rl_execute_next (ESC);
      return (0);
    }

#if defined (HANDLE_MULTIBYTE)
  if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
    {
      if (cxt->lastc >= 0 && (cxt->mb[0] && cxt->mb[1] == '\0') && ENDSRCH_CHAR (cxt->lastc))
	{
	  /* This sets rl_pending_input to LASTC; it will be picked up the next
	     time rl_read_key is called. */
	  rl_execute_next (cxt->lastc);
	  return (0);
	}
    }
  else
#endif
    if (cxt->lastc >= 0 && ENDSRCH_CHAR (cxt->lastc))
      {
	/* This sets rl_pending_input to LASTC; it will be picked up the next
	   time rl_read_key is called. */
	rl_execute_next (cxt->lastc);
	return (0);
      }

  _rl_init_executing_keyseq ();

opcode_dispatch:
  /* Now dispatch on the character.  `Opcodes' affect the search string or
     state.  Other characters are added to the string.  */
  switch (cxt->lastc)
    {
    /* search again */
    case -1:
      if (cxt->search_string_index == 0)
	{
	  if (last_isearch_string)
	    {
	      cxt->search_string_size = 64 + last_isearch_string_len;
	      cxt->search_string = (char *)xrealloc (cxt->search_string, cxt->search_string_size);
	      strcpy (cxt->search_string, last_isearch_string);
	      cxt->search_string_index = last_isearch_string_len;
	      rl_display_search (cxt->search_string, cxt->sflags, -1);
	      break;
	    }
	  /* XXX - restore keymap here? */
	  return (1);
	}
      else if ((cxt->sflags & SF_REVERSE) && cxt->sline_index >= 0)
	cxt->sline_index--;
      else if (cxt->sline_index != cxt->sline_len)
	cxt->sline_index++;
      else
	rl_ding ();
      break;

    /* switch directions */
    case -2:
      cxt->direction = -cxt->direction;
      if (cxt->direction < 0)
	cxt->sflags |= SF_REVERSE;
      else
	cxt->sflags &= ~SF_REVERSE;
      break;

    /* delete character from search string. */
    case -3:	/* C-H, DEL */
      /* This is tricky.  To do this right, we need to keep a
	 stack of search positions for the current search, with
	 sentinels marking the beginning and end.  But this will
	 do until we have a real isearch-undo. */
      if (cxt->search_string_index == 0)
	rl_ding ();
      else if (MB_CUR_MAX == 1 || rl_byte_oriented)
	cxt->search_string[--cxt->search_string_index] = '\0';
      else
	{
	  wstart = _rl_find_prev_mbchar (cxt->search_string, cxt->search_string_index, MB_FIND_NONZERO);
	  if (wstart >= 0)
	    cxt->search_string[cxt->search_string_index = wstart] = '\0';
	  else
	    cxt->search_string[cxt->search_string_index = 0] = '\0';
	}

      if (cxt->search_string_index == 0)
	rl_ding ();

      break;

    case -4:	/* C-G, abort */
      rl_replace_line (cxt->lines[cxt->save_line], 0);
      rl_point = cxt->save_point;
      rl_mark = cxt->save_mark;
      rl_deactivate_mark ();
      rl_restore_prompt();
      rl_clear_message ();

      _rl_fix_point (1);	/* in case save_line and save_point are out of sync */
      return -1;

    case -5:	/* C-W */
      /* skip over portion of line we already matched and yank word */
      wstart = rl_point + cxt->search_string_index;
      if (wstart >= rl_end)
	{
	  rl_ding ();
	  break;
	}

      /* if not in a word, move to one. */
      cval = _rl_char_value (rl_line_buffer, wstart);
      if (_rl_walphabetic (cval) == 0)
	{
	  rl_ding ();
	  break;
	}
      n = MB_NEXTCHAR (rl_line_buffer, wstart, 1, MB_FIND_NONZERO);;
      while (n < rl_end)
	{
	  cval = _rl_char_value (rl_line_buffer, n);
	  if (_rl_walphabetic (cval) == 0)
	    break;
	  n = MB_NEXTCHAR (rl_line_buffer, n, 1, MB_FIND_NONZERO);;
	}
      wlen = n - wstart + 1;
      if (cxt->search_string_index + wlen + 1 >= cxt->search_string_size)
	{
	  cxt->search_string_size += wlen + 1;
	  cxt->search_string = (char *)xrealloc (cxt->search_string, cxt->search_string_size);
	}
      for (; wstart < n; wstart++)
	cxt->search_string[cxt->search_string_index++] = rl_line_buffer[wstart];
      cxt->search_string[cxt->search_string_index] = '\0';
      break;

    case -6:	/* C-Y */
      /* skip over portion of line we already matched and yank rest */
      wstart = rl_point + cxt->search_string_index;
      if (wstart >= rl_end)
	{
	  rl_ding ();
	  break;
	}
      n = rl_end - wstart + 1;
      if (cxt->search_string_index + n + 1 >= cxt->search_string_size)
	{
	  cxt->search_string_size += n + 1;
	  cxt->search_string = (char *)xrealloc (cxt->search_string, cxt->search_string_size);
	}
      for (n = wstart; n < rl_end; n++)
	cxt->search_string[cxt->search_string_index++] = rl_line_buffer[n];
      cxt->search_string[cxt->search_string_index] = '\0';
      break;

    case -7:	/* bracketed paste */
      paste = _rl_bracketed_text (&pastelen);
      if (paste == 0 || *paste == 0)
	{
	  xfree (paste);
	  break;
	}
      if (_rl_enable_active_region)
	rl_activate_mark ();
      if (cxt->search_string_index + pastelen + 1 >= cxt->search_string_size)
	{
	  cxt->search_string_size += pastelen + 2;
	  cxt->search_string = (char *)xrealloc (cxt->search_string, cxt->search_string_size);
	}
      memcpy (cxt->search_string + cxt->search_string_index, paste, pastelen);
      cxt->search_string_index += pastelen;
      cxt->search_string[cxt->search_string_index] = '\0';
      xfree (paste);
      break;

    /* Add character to search string and continue search. */
    default:
#if defined (HANDLE_MULTIBYTE)
      wlen = (cxt->mb[0] == 0 || cxt->mb[1] == 0) ? 1 : RL_STRLEN (cxt->mb);
#else
      wlen = 1;
#endif
      if (cxt->search_string_index + wlen + 1 >= cxt->search_string_size)
	{
	  cxt->search_string_size += 128;	/* 128 much greater than MB_CUR_MAX */
	  cxt->search_string = (char *)xrealloc (cxt->search_string, cxt->search_string_size);
	}
#if defined (HANDLE_MULTIBYTE)
      if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	{
	  int j;

	  if (cxt->mb[0] == 0 || cxt->mb[1] == 0)
	    cxt->search_string[cxt->search_string_index++] = cxt->mb[0];
	  else
	    for (j = 0; j < wlen; )
	      cxt->search_string[cxt->search_string_index++] = cxt->mb[j++];
	}
      else
#endif
	cxt->search_string[cxt->search_string_index++] = cxt->lastc;	/* XXX - was c instead of lastc */
      cxt->search_string[cxt->search_string_index] = '\0';
      break;
    }

  for (cxt->sflags &= ~(SF_FOUND|SF_FAILED);; )
    {
      if (cxt->search_string_index == 0)
	{
	  cxt->sflags |= SF_FAILED;
	  break;
	}

      limit = cxt->sline_len - cxt->search_string_index + 1;

      /* Search the current line. */
      while ((cxt->sflags & SF_REVERSE) ? (cxt->sline_index >= 0) : (cxt->sline_index < limit))
	{
	  if (STREQN (cxt->search_string, cxt->sline + cxt->sline_index, cxt->search_string_index))
	    {
	      cxt->sflags |= SF_FOUND;
	      break;
	    }
	  else
	    cxt->sline_index += cxt->direction;

	  if (cxt->sline_index < 0)
	    {
	      cxt->sline_index = 0;
	      break;
	    }
	}
      if (cxt->sflags & SF_FOUND)
	break;

      /* Move to the next line, but skip new copies of the line
	 we just found and lines shorter than the string we're
	 searching for. */
      do
	{
	  /* Move to the next line. */
	  cxt->history_pos += cxt->direction;

	  /* At limit for direction? */
	  if ((cxt->sflags & SF_REVERSE) ? (cxt->history_pos < 0) : (cxt->history_pos == cxt->hlen))
	    {
	      cxt->sflags |= SF_FAILED;
	      break;
	    }

	  /* We will need these later. */
	  cxt->sline = cxt->lines[cxt->history_pos];
	  cxt->sline_len = strlen (cxt->sline);
	}
      while ((cxt->prev_line_found && STREQ (cxt->prev_line_found, cxt->lines[cxt->history_pos])) ||
	     (cxt->search_string_index > cxt->sline_len));

      if (cxt->sflags & SF_FAILED)
	{
	  /* XXX - reset sline_index if < 0 */
	  if (cxt->sline_index < 0)
	    cxt->sline_index = 0;
	  break;
	}

      /* Now set up the line for searching... */
      cxt->sline_index = (cxt->sflags & SF_REVERSE) ? cxt->sline_len - cxt->search_string_index : 0;
    }

  /* reset the keymaps for the next time through the loop */
  cxt->keymap = cxt->okeymap = _rl_keymap;

  if (cxt->sflags & SF_FAILED)
    {
      /* We cannot find the search string.  Ding the bell. */
      rl_ding ();
      cxt->history_pos = cxt->last_found_line;
      rl_deactivate_mark ();
      rl_display_search (cxt->search_string, cxt->sflags, (cxt->history_pos == cxt->save_line) ? -1 : cxt->history_pos);
      return 1;
    }

  /* We have found the search string.  Just display it.  But don't
     actually move there in the history list until the user accepts
     the location. */
  if (cxt->sflags & SF_FOUND)
    {
      cxt->prev_line_found = cxt->lines[cxt->history_pos];
      rl_replace_line (cxt->lines[cxt->history_pos], 0);
      if (_rl_enable_active_region)
	rl_activate_mark ();	
      rl_point = cxt->sline_index;
      if (rl_mark_active_p () && cxt->search_string_index > 0)
	rl_mark = rl_point + cxt->search_string_index;
      cxt->last_found_line = cxt->history_pos;
      rl_display_search (cxt->search_string, cxt->sflags, (cxt->history_pos == cxt->save_line) ? -1 : cxt->history_pos);
    }

  return 1;
}

int
_rl_isearch_cleanup (_rl_search_cxt *cxt, int r)
{
  if (r >= 0)
    _rl_isearch_fini (cxt);
  _rl_scxt_dispose (cxt, 0);
  _rl_iscxt = 0;

  RL_UNSETSTATE(RL_STATE_ISEARCH);

  return (r != 0);
}

/* Search through the history looking for an interactively typed string.
   This is analogous to i-search.  We start the search in the current line.
   DIRECTION is which direction to search; >= 0 means forward, < 0 means
   backwards. */
static int
rl_search_history (int direction, int invoking_key)
{
  _rl_search_cxt *cxt;		/* local for now, but saved globally */
  int c, r;

  RL_SETSTATE(RL_STATE_ISEARCH);
  cxt = _rl_isearch_init (direction);

  rl_display_search (cxt->search_string, cxt->sflags, -1);

  /* If we are using the callback interface, all we do is set up here and
      return.  The key is that we leave RL_STATE_ISEARCH set. */
  if (RL_ISSTATE (RL_STATE_CALLBACK))
    return (0);

  r = -1;
  for (;;)
    {
      c = _rl_search_getchar (cxt);
      /* We might want to handle EOF here (c == 0) */
      r = _rl_isearch_dispatch (cxt, cxt->lastc);
      if (r <= 0)
        break;
    }

  /* The searching is over.  The user may have found the string that she
     was looking for, or else she may have exited a failing search.  If
     LINE_INDEX is -1, then that shows that the string searched for was
     not found.  We use this to determine where to place rl_point. */
  return (_rl_isearch_cleanup (cxt, r));
}

#if defined (READLINE_CALLBACKS)
/* Called from the callback functions when we are ready to read a key.  The
   callback functions know to call this because RL_ISSTATE(RL_STATE_ISEARCH).
   If _rl_isearch_dispatch finishes searching, this function is responsible
   for turning off RL_STATE_ISEARCH, which it does using _rl_isearch_cleanup. */
int
_rl_isearch_callback (_rl_search_cxt *cxt)
{
  int c, r;

  c = _rl_search_getchar (cxt);
  /* We might want to handle EOF here */
  r = _rl_isearch_dispatch (cxt, cxt->lastc);

  return (r <= 0) ? _rl_isearch_cleanup (cxt, r) : 0;
}
#endif
