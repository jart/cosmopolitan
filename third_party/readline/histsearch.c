/* histsearch.c -- searching the history list. */

/* Copyright (C) 1989, 1992-2009,2017,2021 Free Software Foundation, Inc.

   This file contains the GNU History Library (History), a set of
   routines for managing the text of previously typed lines.

   History is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   History is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with History.  If not, see <http://www.gnu.org/licenses/>.
*/

#define READLINE_LIBRARY

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <stdio.h>
#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#if defined (HAVE_FNMATCH)
#  include <fnmatch.h>
#endif

#include "history.h"
#include "histlib.h"
#include "xmalloc.h"

/* The list of alternate characters that can delimit a history search
   string. */
char *history_search_delimiter_chars = (char *)NULL;

static int history_search_internal (const char *, int, int);

/* Search the history for STRING, starting at history_offset.
   If DIRECTION < 0, then the search is through previous entries, else
   through subsequent.  If ANCHORED is non-zero, the string must
   appear at the beginning of a history line, otherwise, the string
   may appear anywhere in the line.  If the string is found, then
   current_history () is the history entry, and the value of this
   function is the offset in the line of that history entry that the
   string was found in.  Otherwise, nothing is changed, and a -1 is
   returned. */

static int
history_search_internal (const char *string, int direction, int flags)
{
  register int i, reverse;
  register char *line;
  register int line_index;
  int string_len, anchored, patsearch;
  HIST_ENTRY **the_history; 	/* local */

  i = history_offset;
  reverse = (direction < 0);
  anchored = (flags & ANCHORED_SEARCH);
#if defined (HAVE_FNMATCH)
  patsearch = (flags & PATTERN_SEARCH);
#else
  patsearch = 0;
#endif

  /* Take care of trivial cases first. */
  if (string == 0 || *string == '\0')
    return (-1);

  if (!history_length || ((i >= history_length) && !reverse))
    return (-1);

  if (reverse && (i >= history_length))
    i = history_length - 1;

#define NEXT_LINE() do { if (reverse) i--; else i++; } while (0)

  the_history = history_list ();
  string_len = strlen (string);
  while (1)
    {
      /* Search each line in the history list for STRING. */

      /* At limit for direction? */
      if ((reverse && i < 0) || (!reverse && i == history_length))
	return (-1);

      line = the_history[i]->line;
      line_index = strlen (line);

      /* If STRING is longer than line, no match. */
      if (patsearch == 0 && (string_len > line_index))
	{
	  NEXT_LINE ();
	  continue;
	}

      /* Handle anchored searches first. */
      if (anchored == ANCHORED_SEARCH)
	{
#if defined (HAVE_FNMATCH)
	  if (patsearch)
	    {
	      if (fnmatch (string, line, 0) == 0)
		{
		  history_offset = i;
		  return (0);
		}
	    }
	  else
#endif
	  if (STREQN (string, line, string_len))
	    {
	      history_offset = i;
	      return (0);
	    }

	  NEXT_LINE ();
	  continue;
	}

      /* Do substring search. */
      if (reverse)
	{
	  line_index -= (patsearch == 0) ? string_len : 1;

	  while (line_index >= 0)
	    {
#if defined (HAVE_FNMATCH)
	      if (patsearch)
		{
		  if (fnmatch (string, line + line_index, 0) == 0)
		    {
		      history_offset = i;
		      return (line_index);
		    }
		}
	      else
#endif
	      if (STREQN (string, line + line_index, string_len))
		{
		  history_offset = i;
		  return (line_index);
		}
	      line_index--;
	    }
	}
      else
	{
	  register int limit;

	  limit = line_index - string_len + 1;
	  line_index = 0;

	  while (line_index < limit)
	    {
#if defined (HAVE_FNMATCH)
	      if (patsearch)
		{
		  if (fnmatch (string, line + line_index, 0) == 0)
		    {
		      history_offset = i;
		      return (line_index);
		    }
		}
	      else
#endif
	      if (STREQN (string, line + line_index, string_len))
		{
		  history_offset = i;
		  return (line_index);
		}
	      line_index++;
	    }
	}
      NEXT_LINE ();
    }
}

int
_hs_history_patsearch (const char *string, int direction, int flags)
{
  char *pat;
  size_t len, start;
  int ret, unescaped_backslash;

#if defined (HAVE_FNMATCH)
  /* Assume that the string passed does not have a leading `^' and any
     anchored search request is captured in FLAGS */
  len = strlen (string);
  ret = len - 1;
  /* fnmatch is required to reject a pattern that ends with an unescaped
     backslash */
  if ((unescaped_backslash = (string[ret] == '\\')))
    {
      while (ret > 0 && string[--ret] == '\\')
	unescaped_backslash = 1 - unescaped_backslash;
    }
  if (unescaped_backslash)
    return -1;
  pat = (char *)xmalloc (len + 3);
  /* If the search string is not anchored, we'll be calling fnmatch (assuming
     we have it). Prefix a `*' to the front of the search string so we search
     anywhere in the line. */
  if ((flags & ANCHORED_SEARCH) == 0 && string[0] != '*')
    {
      pat[0] = '*';
      start = 1;
      len++;
    }
  else
    {
      start = 0;
    }

  /* Attempt to reduce the number of searches by tacking a `*' onto the end
     of a pattern that doesn't have one.  Assume a pattern that ends in a
     backslash contains an even number of trailing backslashes; we check
     above */
  strcpy (pat + start, string);
  if (pat[len - 1] != '*')
    {
      pat[len] = '*';		/* XXX */
      pat[len+1] = '\0';
    }
#else
  pat = string;
#endif

  ret = history_search_internal (pat, direction, flags|PATTERN_SEARCH);

  if (pat != string)
    xfree (pat);
  return ret;
}
	
/* Do a non-anchored search for STRING through the history in DIRECTION. */
int
history_search (const char *string, int direction)
{
  return (history_search_internal (string, direction, NON_ANCHORED_SEARCH));
}

/* Do an anchored search for string through the history in DIRECTION. */
int
history_search_prefix (const char *string, int direction)
{
  return (history_search_internal (string, direction, ANCHORED_SEARCH));
}

/* Search for STRING in the history list.  DIR is < 0 for searching
   backwards.  POS is an absolute index into the history list at
   which point to begin searching. */
int
history_search_pos (const char *string, int dir, int pos)
{
  int ret, old;

  old = where_history ();
  history_set_pos (pos);
  if (history_search (string, dir) == -1)
    {
      history_set_pos (old);
      return (-1);
    }
  ret = where_history ();
  history_set_pos (old);
  return ret;
}
