/* undo.c - manage list of changes to lines, offering opportunity to undo them */

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

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>           /* for _POSIX_VERSION */
#endif /* HAVE_UNISTD_H */

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#include <stdio.h>

/* System-specific feature definitions and include files. */
#include "rldefs.h"

/* Some standard library routines. */
#include "readline.h"
#include "history.h"

#include "rlprivate.h"
#include "xmalloc.h"

#include "histlib.h"

/* Non-zero tells rl_delete_text and rl_insert_text to not add to
   the undo list. */
int _rl_doing_an_undo = 0;

/* How many unclosed undo groups we currently have. */
int _rl_undo_group_level = 0;

/* The current undo list for THE_LINE. */
UNDO_LIST *rl_undo_list = (UNDO_LIST *)NULL;

/* **************************************************************** */
/*								    */
/*			Undo, and Undoing			    */
/*								    */
/* **************************************************************** */

static UNDO_LIST *
alloc_undo_entry (enum undo_code what, int start, int end, char *text)
{
  UNDO_LIST *temp;

  temp = (UNDO_LIST *)xmalloc (sizeof (UNDO_LIST));
  temp->what = what;
  temp->start = start;
  temp->end = end;
  temp->text = text;

  temp->next = (UNDO_LIST *)NULL;
  return temp;
}

/* Remember how to undo something.  Concatenate some undos if that
   seems right. */
void
rl_add_undo (enum undo_code what, int start, int end, char *text)
{
  UNDO_LIST *temp;

  temp = alloc_undo_entry (what, start, end, text);
  temp->next = rl_undo_list;
  rl_undo_list = temp;
}

/* Free an UNDO_LIST */
void
_rl_free_undo_list (UNDO_LIST *ul)
{
  UNDO_LIST *release;

  while (ul)
    {
      release = ul;
      ul = ul->next;

      if (release->what == UNDO_DELETE)
	xfree (release->text);

      xfree (release);
    }
}

/* Free the existing undo list. */
void
rl_free_undo_list (void)
{
  UNDO_LIST *release, *orig_list;

  orig_list = rl_undo_list;
  _rl_free_undo_list (rl_undo_list);
  rl_undo_list = (UNDO_LIST *)NULL;
  _hs_replace_history_data (-1, (histdata_t *)orig_list, (histdata_t *)NULL);
}

UNDO_LIST *
_rl_copy_undo_entry (UNDO_LIST *entry)
{
  UNDO_LIST *new;

  new = alloc_undo_entry (entry->what, entry->start, entry->end, (char *)NULL);
  new->text = entry->text ? savestring (entry->text) : 0;
  return new;
}

UNDO_LIST *
_rl_copy_undo_list (UNDO_LIST *head)
{
  UNDO_LIST *list, *new, *roving, *c;

  if (head == 0)
    return head;

  list = head;
  new = 0;
  while (list)
    {
      c = _rl_copy_undo_entry (list);
      if (new == 0)
	roving = new = c;
      else
	{
	  roving->next = c;
	  roving = roving->next;
	}
      list = list->next;
    }

  roving->next = 0;
  return new;
}

/* Undo the next thing in the list.  Return 0 if there
   is nothing to undo, or non-zero if there was. */
int
rl_do_undo (void)
{
  UNDO_LIST *release, *search;
  int waiting_for_begin, start, end;
  HIST_ENTRY *cur, *temp;

#define TRANS(i) ((i) == -1 ? rl_point : ((i) == -2 ? rl_end : (i)))

  start = end = waiting_for_begin = 0;
  do
    {
      if (rl_undo_list == 0)
	return (0);

      _rl_doing_an_undo = 1;
      RL_SETSTATE(RL_STATE_UNDOING);

      /* To better support vi-mode, a start or end value of -1 means
	 rl_point, and a value of -2 means rl_end. */
      if (rl_undo_list->what == UNDO_DELETE || rl_undo_list->what == UNDO_INSERT)
	{
	  start = TRANS (rl_undo_list->start);
	  end = TRANS (rl_undo_list->end);
	}

      switch (rl_undo_list->what)
	{
	/* Undoing deletes means inserting some text. */
	case UNDO_DELETE:
	  rl_point = start;
	  _rl_fix_point (1);
	  rl_insert_text (rl_undo_list->text);
	  xfree (rl_undo_list->text);
	  break;

	/* Undoing inserts means deleting some text. */
	case UNDO_INSERT:
	  rl_delete_text (start, end);
	  rl_point = start;
	  _rl_fix_point (1);
	  break;

	/* Undoing an END means undoing everything 'til we get to a BEGIN. */
	case UNDO_END:
	  waiting_for_begin++;
	  break;

	/* Undoing a BEGIN means that we are done with this group. */
	case UNDO_BEGIN:
	  if (waiting_for_begin)
	    waiting_for_begin--;
	  else
	    rl_ding ();
	  break;
	}

      _rl_doing_an_undo = 0;
      RL_UNSETSTATE(RL_STATE_UNDOING);

      release = rl_undo_list;
      rl_undo_list = rl_undo_list->next;
      release->next = 0;	/* XXX */

      /* If we are editing a history entry, make sure the change is replicated
	 in the history entry's line */
      cur = current_history ();
      if (cur && cur->data && (UNDO_LIST *)cur->data == release)
	{
	  temp = replace_history_entry (where_history (), rl_line_buffer, (histdata_t)rl_undo_list);
	  xfree (temp->line);
	  FREE (temp->timestamp);
	  xfree (temp);
	}

      /* Make sure there aren't any history entries with that undo list */
      _hs_replace_history_data (-1, (histdata_t *)release, (histdata_t *)rl_undo_list);

      /* And make sure this list isn't anywhere in the saved line for history */
      if (_rl_saved_line_for_history && _rl_saved_line_for_history->data)
	{
	  /* Brute force; no finesse here */
	  search = (UNDO_LIST *)_rl_saved_line_for_history->data;
	  if (search == release)
	    _rl_saved_line_for_history->data = rl_undo_list;
	  else
	    {
	      while (search->next)
		{
		  if (search->next == release)
		    {
		      search->next = rl_undo_list;
		      break;
		    }
		  search = search->next;
		}
	    }
	}

      xfree (release);
    }
  while (waiting_for_begin);

  return (1);
}
#undef TRANS

int
_rl_fix_last_undo_of_type (int type, int start, int end)
{
  UNDO_LIST *rl;

  for (rl = rl_undo_list; rl; rl = rl->next)
    {
      if (rl->what == type)
	{
	  rl->start = start;
	  rl->end = end;
	  return 0;
	}
    }
  return 1;
}

/* Begin a group.  Subsequent undos are undone as an atomic operation. */
int
rl_begin_undo_group (void)
{
  rl_add_undo (UNDO_BEGIN, 0, 0, 0);
  _rl_undo_group_level++;
  return 0;
}

/* End an undo group started with rl_begin_undo_group (). */
int
rl_end_undo_group (void)
{
  rl_add_undo (UNDO_END, 0, 0, 0);
  _rl_undo_group_level--;
  return 0;
}

/* Save an undo entry for the text from START to END. */
int
rl_modifying (int start, int end)
{
  if (start > end)
    {
      SWAP (start, end);
    }

  if (start != end)
    {
      char *temp = rl_copy_text (start, end);
      rl_begin_undo_group ();
      rl_add_undo (UNDO_DELETE, start, end, temp);
      rl_add_undo (UNDO_INSERT, start, end, (char *)NULL);
      rl_end_undo_group ();
    }
  return 0;
}

/* Revert the current line to its previous state. */
int
rl_revert_line (int count, int key)
{
  if (rl_undo_list == 0)
    rl_ding ();
  else
    {
      while (rl_undo_list)
	rl_do_undo ();
#if defined (VI_MODE)
      if (rl_editing_mode == vi_mode)
	rl_point = rl_mark = 0;		/* rl_end should be set correctly */
#endif
    }
    
  return 0;
}

/* Do some undoing of things that were done. */
int
rl_undo_command (int count, int key)
{
  if (count < 0)
    return 0;	/* Nothing to do. */

  while (count)
    {
      if (rl_do_undo ())
	count--;
      else
	{
	  rl_ding ();
	  break;
	}
    }
  return 0;
}
