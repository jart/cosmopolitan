/* alias.c -- Not a full alias, but just the kind that we use in the
   shell.  Csh style alias is somewhere else (`over there, in a box'). */

/* Copyright (C) 1987-2021 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#if defined (ALIAS)

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>
#include "chartypes.h"
#include "bashansi.h"
#include "command.h"
#include "general.h"
#include "externs.h"
#include "alias.h"

#if defined (PROGRAMMABLE_COMPLETION)
#  include "pcomplete.h"
#endif

#if defined (HAVE_MBSTR_H) && defined (HAVE_MBSCHR)
#  include <mbstr.h>		/* mbschr */
#endif

#define ALIAS_HASH_BUCKETS	64	/* must be power of two */

typedef int sh_alias_map_func_t PARAMS((alias_t *));

static void free_alias_data PARAMS((PTR_T));
static alias_t **map_over_aliases PARAMS((sh_alias_map_func_t *));
static void sort_aliases PARAMS((alias_t **));
static int qsort_alias_compare PARAMS((alias_t **, alias_t **));

#if defined (READLINE)
static int skipquotes PARAMS((char *, int));
static int skipws PARAMS((char *, int));
static int rd_token PARAMS((char *, int));
#endif

/* Non-zero means expand all words on the line.  Otherwise, expand
   after first expansion if the expansion ends in a space. */
int alias_expand_all = 0;

/* The list of aliases that we have. */
HASH_TABLE *aliases = (HASH_TABLE *)NULL;

void
initialize_aliases ()
{
  if (aliases == 0)
    aliases = hash_create (ALIAS_HASH_BUCKETS);
}

/* Scan the list of aliases looking for one with NAME.  Return NULL
   if the alias doesn't exist, else a pointer to the alias_t. */
alias_t *
find_alias (name)
     char *name;
{
  BUCKET_CONTENTS *al;

  if (aliases == 0)
    return ((alias_t *)NULL);

  al = hash_search (name, aliases, 0);
  return (al ? (alias_t *)al->data : (alias_t *)NULL);
}

/* Return the value of the alias for NAME, or NULL if there is none. */
char *
get_alias_value (name)
     char *name;
{
  alias_t *alias;

  if (aliases == 0)
    return ((char *)NULL);

  alias = find_alias (name);
  return (alias ? alias->value : (char *)NULL);
}

/* Make a new alias from NAME and VALUE.  If NAME can be found,
   then replace its value. */
void
add_alias (name, value)
     char *name, *value;
{
  BUCKET_CONTENTS *elt;
  alias_t *temp;
  int n;

  if (aliases == 0)
    {
      initialize_aliases ();
      temp = (alias_t *)NULL;
    }
  else
    temp = find_alias (name);

  if (temp)
    {
      free (temp->value);
      temp->value = savestring (value);
      temp->flags &= ~AL_EXPANDNEXT;
      if (value[0])
	{
	  n = value[strlen (value) - 1];
	  if (n == ' ' || n == '\t')
	    temp->flags |= AL_EXPANDNEXT;
	}
    }
  else
    {
      temp = (alias_t *)xmalloc (sizeof (alias_t));
      temp->name = savestring (name);
      temp->value = savestring (value);
      temp->flags = 0;

      if (value[0])
	{
	  n = value[strlen (value) - 1];
	  if (n == ' ' || n == '\t')
	    temp->flags |= AL_EXPANDNEXT;
	}

      elt = hash_insert (savestring (name), aliases, HASH_NOSRCH);
      elt->data = temp;
#if defined (PROGRAMMABLE_COMPLETION)
      set_itemlist_dirty (&it_aliases);
#endif
    }
}

/* Delete a single alias structure. */
static void
free_alias_data (data)
     PTR_T data;
{
  register alias_t *a;

  a = (alias_t *)data;

  if (a->flags & AL_BEINGEXPANDED)
    clear_string_list_expander (a);	/* call back to the parser */

  free (a->value);
  free (a->name);
  free (data);
}

/* Remove the alias with name NAME from the alias table.  Returns
   the number of aliases left in the table, or -1 if the alias didn't
   exist. */
int
remove_alias (name)
     char *name;
{
  BUCKET_CONTENTS *elt;

  if (aliases == 0)
    return (-1);

  elt = hash_remove (name, aliases, 0);
  if (elt)
    {
      free_alias_data (elt->data);
      free (elt->key);		/* alias name */
      free (elt);		/* XXX */
#if defined (PROGRAMMABLE_COMPLETION)
      set_itemlist_dirty (&it_aliases);
#endif
      return (aliases->nentries);
    }
  return (-1);
}

/* Delete all aliases. */
void
delete_all_aliases ()
{
  if (aliases == 0)
    return;

  hash_flush (aliases, free_alias_data);
  hash_dispose (aliases);
  aliases = (HASH_TABLE *)NULL;
#if defined (PROGRAMMABLE_COMPLETION)
  set_itemlist_dirty (&it_aliases);
#endif
}

/* Return an array of aliases that satisfy the conditions tested by FUNCTION.
   If FUNCTION is NULL, return all aliases. */
static alias_t **
map_over_aliases (function)
     sh_alias_map_func_t *function;
{
  register int i;
  register BUCKET_CONTENTS *tlist;
  alias_t *alias, **list;
  int list_index;

  i = HASH_ENTRIES (aliases);
  if (i == 0)
    return ((alias_t **)NULL);

  list = (alias_t **)xmalloc ((i + 1) * sizeof (alias_t *));
  for (i = list_index = 0; i < aliases->nbuckets; i++)
    {
      for (tlist = hash_items (i, aliases); tlist; tlist = tlist->next)
	{
	  alias = (alias_t *)tlist->data;

	  if (!function || (*function) (alias))
	    {
	      list[list_index++] = alias;
	      list[list_index] = (alias_t *)NULL;
	    }
	}
    }
  return (list);
}

static void
sort_aliases (array)
     alias_t **array;
{
  qsort (array, strvec_len ((char **)array), sizeof (alias_t *), (QSFUNC *)qsort_alias_compare);
}

static int
qsort_alias_compare (as1, as2)
     alias_t **as1, **as2;
{
  int result;

  if ((result = (*as1)->name[0] - (*as2)->name[0]) == 0)
    result = strcmp ((*as1)->name, (*as2)->name);

  return (result);
}

/* Return a sorted list of all defined aliases */
alias_t **
all_aliases ()
{
  alias_t **list;

  if (aliases == 0 || HASH_ENTRIES (aliases) == 0)
    return ((alias_t **)NULL);

  list = map_over_aliases ((sh_alias_map_func_t *)NULL);
  if (list)
    sort_aliases (list);
  return (list);
}

char *
alias_expand_word (s)
     char *s;
{
  alias_t *r;

  r = find_alias (s);
  return (r ? savestring (r->value) : (char *)NULL);
}

/* Readline support functions -- expand all aliases in a line. */

#if defined (READLINE)

/* Return non-zero if CHARACTER is a member of the class of characters
   that are self-delimiting in the shell (this really means that these
   characters delimit tokens). */
#define self_delimiting(character) (member ((character), " \t\n\r;|&()"))

/* Return non-zero if CHARACTER is a member of the class of characters
   that delimit commands in the shell. */
#define command_separator(character) (member ((character), "\r\n;|&("))

/* If this is 1, we are checking the next token read for alias expansion
   because it is the first word in a command. */
static int command_word;

/* This is for skipping quoted strings in alias expansions. */
#define quote_char(c)  (((c) == '\'') || ((c) == '"'))

/* Consume a quoted string from STRING, starting at string[START] (so
   string[START] is the opening quote character), and return the index
   of the closing quote character matching the opening quote character.
   This handles single matching pairs of unquoted quotes; it could afford
   to be a little smarter... This skips words between balanced pairs of
   quotes, words where the first character is quoted with a `\', and other
   backslash-escaped characters. */

static int
skipquotes (string, start)
     char *string;
     int start;
{
  register int i;
  int delimiter = string[start];

  /* i starts at START + 1 because string[START] is the opening quote
     character. */
  for (i = start + 1 ; string[i] ; i++)
    {
      if (string[i] == '\\')
	{
	  i++;		/* skip backslash-quoted quote characters, too */
	  if (string[i] == 0)
	    break;
	  continue;
	}

      if (string[i] == delimiter)
	return i;
    }
  return (i);
}

/* Skip the white space and any quoted characters in STRING, starting at
   START.  Return the new index into STRING, after zero or more characters
   have been skipped. */
static int
skipws (string, start)
     char *string;
     int start;
{
  register int i;
  int pass_next, backslash_quoted_word;
  unsigned char peekc;

  /* skip quoted strings, in ' or ", and words in which a character is quoted
     with a `\'. */
  i = backslash_quoted_word = pass_next = 0;

  /* Skip leading whitespace (or separator characters), and quoted words.
     But save it in the output.  */

  for (i = start; string[i]; i++)
    {
      if (pass_next)
	{
	  pass_next = 0;
	  continue;
	}

      if (whitespace (string[i]))
	{
	  backslash_quoted_word = 0; /* we are no longer in a backslash-quoted word */
	  continue;
	}

      if (string[i] == '\\')
	{
	  peekc = string[i+1];
	  if (peekc == 0)
	    break;
	  if (ISLETTER (peekc))
	    backslash_quoted_word++;	/* this is a backslash-quoted word */
	  else
	    pass_next++;
	  continue;
	}

      /* This only handles single pairs of non-escaped quotes.  This
	 overloads backslash_quoted_word to also mean that a word like
	 ""f is being scanned, so that the quotes will inhibit any expansion
	 of the word. */
      if (quote_char(string[i]))
	{
	  i = skipquotes (string, i);
	  /* This could be a line that contains a single quote character,
	     in which case skipquotes () terminates with string[i] == '\0'
	     (the end of the string).  Check for that here. */
	  if (string[i] == '\0')
	    break;

	  peekc = string[i + 1];
	  if (ISLETTER (peekc))
	    backslash_quoted_word++;
	  continue;
	}

      /* If we're in the middle of some kind of quoted word, let it
	 pass through. */
      if (backslash_quoted_word)
	continue;

      /* If this character is a shell command separator, then set a hint for
	 alias_expand that the next token is the first word in a command. */

      if (command_separator (string[i]))
	{
	  command_word++;
	  continue;
	}
      break;
    }
  return (i);
}

/* Characters that may appear in a token.  Basically, anything except white
   space and a token separator. */
#define token_char(c)	(!((whitespace (string[i]) || self_delimiting (string[i]))))

/* Read from START in STRING until the next separator character, and return
   the index of that separator.  Skip backslash-quoted characters.  Call
   skipquotes () for quoted strings in the middle or at the end of tokens,
   so all characters show up (e.g. foo'' and foo""bar) */
static int
rd_token (string, start)
     char *string;
     int start;
{
  register int i;

  /* From here to next separator character is a token. */
  for (i = start; string[i] && token_char (string[i]); i++)
    {
      if (string[i] == '\\')
	{
	  i++;	/* skip backslash-escaped character */
	  if (string[i] == 0)
	    break;
	  continue;
	}

      /* If this character is a quote character, we want to call skipquotes
	 to get the whole quoted portion as part of this word.  That word
	 will not generally match an alias, even if te unquoted word would
	 have.  The presence of the quotes in the token serves then to
	 inhibit expansion. */
      if (quote_char (string[i]))
	{
	  i = skipquotes (string, i);
	  /* This could be a line that contains a single quote character,
	     in which case skipquotes () terminates with string[i] == '\0'
	     (the end of the string).  Check for that here. */
	  if (string[i] == '\0')
	    break;

	  /* Now string[i] is the matching quote character, and the
	     quoted portion of the token has been scanned. */
	  continue;
	}
    }
  return (i);
}

/* Return a new line, with any aliases substituted. */
char *
alias_expand (string)
     char *string;
{
  register int i, j, start;
  char *line, *token;
  int line_len, tl, real_start, expand_next, expand_this_token;
  alias_t *alias;

  line_len = strlen (string) + 1;
  line = (char *)xmalloc (line_len);
  token = (char *)xmalloc (line_len);

  line[0] = i = 0;
  expand_next = 0;
  command_word = 1; /* initialized to expand the first word on the line */

  /* Each time through the loop we find the next word in line.  If it
     has an alias, substitute the alias value.  If the value ends in ` ',
     then try again with the next word.  Else, if there is no value, or if
     the value does not end in space, we are done. */

  for (;;)
    {

      token[0] = 0;
      start = i;

      /* Skip white space and quoted characters */
      i = skipws (string, start);

      if (start == i && string[i] == '\0')
	{
	  free (token);
	  return (line);
	}

      /* copy the just-skipped characters into the output string,
	 expanding it if there is not enough room. */
      j = strlen (line);
      tl = i - start;	/* number of characters just skipped */
      RESIZE_MALLOCED_BUFFER (line, j, (tl + 1), line_len, (tl + 50));
      strncpy (line + j, string + start, tl);
      line[j + tl] = '\0';

      real_start = i;

      command_word = command_word || (command_separator (string[i]));
      expand_this_token = (command_word || expand_next);
      expand_next = 0;

      /* Read the next token, and copy it into TOKEN. */
      start = i;
      i = rd_token (string, start);

      tl = i - start;	/* token length */

      /* If tl == 0, but we're not at the end of the string, then we have a
	 single-character token, probably a delimiter */
      if (tl == 0 && string[i] != '\0')
	{
	  tl = 1;
	  i++;		/* move past it */
	}

      strncpy (token, string + start, tl);
      token [tl] = '\0';

      /* If there is a backslash-escaped character quoted in TOKEN,
	 then we don't do alias expansion.  This should check for all
	 other quoting characters, too. */
      if (mbschr (token, '\\'))
	expand_this_token = 0;

      /* If we should be expanding here, if we are expanding all words, or if
	 we are in a location in the string where an expansion is supposed to
	 take place, see if this word has a substitution.  If it does, then do
	 the expansion.  Note that we defer the alias value lookup until we
	 are sure we are expanding this token. */

      if ((token[0]) &&
	  (expand_this_token || alias_expand_all) &&
	  (alias = find_alias (token)))
	{
	  char *v;
	  int vlen, llen;

	  v = alias->value;
	  vlen = strlen (v);
	  llen = strlen (line);

	  /* +3 because we possibly add one more character below. */
	  RESIZE_MALLOCED_BUFFER (line, llen, (vlen + 3), line_len, (vlen + 50));

	  strcpy (line + llen, v);

	  if ((expand_this_token && vlen && whitespace (v[vlen - 1])) ||
	      alias_expand_all)
	    expand_next = 1;
	}
      else
	{
	  int llen, tlen;

	  llen = strlen (line);
	  tlen = i - real_start; /* tlen == strlen(token) */

	  RESIZE_MALLOCED_BUFFER (line, llen, (tlen + 1), line_len, (llen + tlen + 50));

	  strncpy (line + llen, string + real_start, tlen);
	  line[llen + tlen] = '\0';
	}
      command_word = 0;
    }
}
#endif /* READLINE */
#endif /* ALIAS */
