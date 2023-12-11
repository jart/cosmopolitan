/* shquote - functions to quote and dequote strings */

/* Copyright (C) 1999-2020 Free Software Foundation, Inc.

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

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>
#include "stdc.h"

#include "syntax.h"
#include "xmalloc.h"

#include "shmbchar.h"
#include "shmbutil.h"

extern char *ansic_quote PARAMS((char *, int, int *));
extern int ansic_shouldquote PARAMS((const char *));

/* Default set of characters that should be backslash-quoted in strings */
static const char bstab[256] =
  {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0, 0,	/* TAB, NL */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

    1, 1, 1, 0, 1, 0, 1, 1,	/* SPACE, !, DQUOTE, DOL, AMP, SQUOTE */
    1, 1, 1, 0, 1, 0, 0, 0,	/* LPAR, RPAR, STAR, COMMA */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 1, 1,	/* SEMI, LESSTHAN, GREATERTHAN, QUEST */

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 0,	/* LBRACK, BS, RBRACK, CARAT */

    1, 0, 0, 0, 0, 0, 0, 0,	/* BACKQ */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 0, 0,	/* LBRACE, BAR, RBRACE */

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
  };

/* **************************************************************** */
/*								    */
/*	 Functions for quoting strings to be re-read as input	    */
/*								    */
/* **************************************************************** */

/* Return a new string which is the single-quoted version of STRING.
   Used by alias and trap, among others. */
char *
sh_single_quote (string)
     const char *string;
{
  register int c;
  char *result, *r;
  const char *s;

  result = (char *)xmalloc (3 + (4 * strlen (string)));
  r = result;

  if (string[0] == '\'' && string[1] == 0)
    {
      *r++ = '\\';
      *r++ = '\'';
      *r++ = 0;
      return result;
    }

  *r++ = '\'';

  for (s = string; s && (c = *s); s++)
    {
      *r++ = c;

      if (c == '\'')
	{
	  *r++ = '\\';	/* insert escaped single quote */
	  *r++ = '\'';
	  *r++ = '\'';	/* start new quoted string */
	}
    }

  *r++ = '\'';
  *r = '\0';

  return (result);
}

/* Quote STRING using double quotes.  Return a new string. */
char *
sh_double_quote (string)
     const char *string;
{
  register unsigned char c;
  int mb_cur_max;
  char *result, *r;
  size_t slen;
  const char *s, *send;
  DECLARE_MBSTATE;

  slen = strlen (string);
  send = string + slen;
  mb_cur_max = MB_CUR_MAX;

  result = (char *)xmalloc (3 + (2 * strlen (string)));
  r = result;
  *r++ = '"';

  for (s = string; s && (c = *s); s++)
    {
      /* Backslash-newline disappears within double quotes, so don't add one. */
      if ((sh_syntaxtab[c] & CBSDQUOTE) && c != '\n')
	*r++ = '\\';

#if defined (HANDLE_MULTIBYTE)
      if ((locale_utf8locale && (c & 0x80)) ||
	  (locale_utf8locale == 0 && mb_cur_max > 1 && is_basic (c) == 0))
	{
	  COPY_CHAR_P (r, s, send);
	  s--;		/* compensate for auto-increment in loop above */
	  continue;
	}
#endif

      /* Assume that the string will not be further expanded, so no need to
	 add CTLESC to protect CTLESC or CTLNUL. */
      *r++ = c;
    }

  *r++ = '"';
  *r = '\0';

  return (result);
}

/* Turn S into a simple double-quoted string.  If FLAGS is non-zero, quote
   double quote characters in S with backslashes. */
char *
sh_mkdoublequoted (s, slen, flags)
     const char *s;
     int slen, flags;
{
  char *r, *ret;
  const char *send;
  int rlen, mb_cur_max;
  DECLARE_MBSTATE;

  send = s + slen;
  mb_cur_max = flags ? MB_CUR_MAX : 1;
  rlen = (flags == 0) ? slen + 3 : (2 * slen) + 1;
  ret = r = (char *)xmalloc (rlen);

  *r++ = '"';
  while (*s)
    {
      if (flags && *s == '"')
	*r++ = '\\';

#if defined (HANDLE_MULTIBYTE)
      if  (flags && ((locale_utf8locale && (*s & 0x80)) ||
		     (locale_utf8locale == 0 && mb_cur_max > 1 && is_basic (*s) == 0)))
	{
	  COPY_CHAR_P (r, s, send);
	  continue;
	}
#endif
      *r++ = *s++;
    }
  *r++ = '"';
  *r = '\0';

  return ret;
}

/* Remove backslashes that are quoting characters that are special between
   double quotes.  Return a new string.  XXX - should this handle CTLESC
   and CTLNUL? */
char *
sh_un_double_quote (string)
     char *string;
{
  register int c, pass_next;
  char *result, *r, *s;

  r = result = (char *)xmalloc (strlen (string) + 1);

  for (pass_next = 0, s = string; s && (c = *s); s++)
    {
      if (pass_next)
	{
	  *r++ = c;
	  pass_next = 0;
	  continue;
	}
      if (c == '\\' && (sh_syntaxtab[(unsigned char) s[1]] & CBSDQUOTE))
	{
	  pass_next = 1;
	  continue;
	}
      *r++ = c;
    }

  *r = '\0';
  return result;
}

/* Quote special characters in STRING using backslashes.  Return a new
   string.  NOTE:  if the string is to be further expanded, we need a
   way to protect the CTLESC and CTLNUL characters.  As I write this,
   the current callers will never cause the string to be expanded without
   going through the shell parser, which will protect the internal
   quoting characters.  TABLE, if set, points to a map of the ascii code
   set with char needing to be backslash-quoted if table[char]==1.  FLAGS,
   if 1, causes tildes to be quoted as well.  If FLAGS&2, backslash-quote
   other shell blank characters. */
   
char *
sh_backslash_quote (string, table, flags)
     char *string;
     char *table;
     int flags;
{
  int c, mb_cur_max;
  size_t slen;
  char *result, *r, *s, *backslash_table, *send;
  DECLARE_MBSTATE;

  slen = strlen (string);
  send = string + slen;
  result = (char *)xmalloc (2 * slen + 1);

  backslash_table = table ? table : (char *)bstab;
  mb_cur_max = MB_CUR_MAX;

  for (r = result, s = string; s && (c = *s); s++)
    {
#if defined (HANDLE_MULTIBYTE)
      /* XXX - isascii, even if is_basic(c) == 0 - works in most cases. */
      if (c >= 0 && c <= 127 && backslash_table[(unsigned char)c] == 1)
	{
	  *r++ = '\\';
	  *r++ = c;
	  continue;
	}
      if ((locale_utf8locale && (c & 0x80)) ||
	  (locale_utf8locale == 0 && mb_cur_max > 1 && is_basic (c) == 0))
	{
	  COPY_CHAR_P (r, s, send);
	  s--;		/* compensate for auto-increment in loop above */
	  continue;
	}
#endif
      if (backslash_table[(unsigned char)c] == 1)
	*r++ = '\\';
      else if (c == '#' && s == string)			/* comment char */
	*r++ = '\\';
      else if ((flags&1) && c == '~' && (s == string || s[-1] == ':' || s[-1] == '='))
        /* Tildes are special at the start of a word or after a `:' or `='
	   (technically unquoted, but it doesn't make a difference in practice) */
	*r++ = '\\';
      else if ((flags&2) && shellblank((unsigned char)c))
	*r++ = '\\';
      *r++ = c;
    }

  *r = '\0';
  return (result);
}

#if defined (PROMPT_STRING_DECODE) || defined (TRANSLATABLE_STRINGS)
/* Quote characters that get special treatment when in double quotes in STRING
   using backslashes. FLAGS is reserved for future use. Return a new string. */
char *
sh_backslash_quote_for_double_quotes (string, flags)
     char *string;
     int flags;
{
  unsigned char c;
  char *result, *r, *s, *send;
  size_t slen;
  int mb_cur_max;
  DECLARE_MBSTATE;
 
  slen = strlen (string);
  send = string + slen;
  mb_cur_max = MB_CUR_MAX;
  result = (char *)xmalloc (2 * slen + 1);

  for (r = result, s = string; s && (c = *s); s++)
    {
      /* Backslash-newline disappears within double quotes, so don't add one. */
      if ((sh_syntaxtab[c] & CBSDQUOTE) && c != '\n')
	*r++ = '\\';
      /* I should probably use the CSPECL flag for these in sh_syntaxtab[] */
      else if (c == CTLESC || c == CTLNUL)
	*r++ = CTLESC;		/* could be '\\'? */

#if defined (HANDLE_MULTIBYTE)
      if ((locale_utf8locale && (c & 0x80)) ||
	  (locale_utf8locale == 0 && mb_cur_max > 1 && is_basic (c) == 0))
	{
	  COPY_CHAR_P (r, s, send);
	  s--;		/* compensate for auto-increment in loop above */
	  continue;
	}
#endif

      *r++ = c;
    }

  *r = '\0';
  return (result);
}
#endif /* PROMPT_STRING_DECODE */

char *
sh_quote_reusable (s, flags)
     char *s;
     int flags;
{
  char *ret;

  if (s == 0)
    return s;
  else if (*s == 0)
    {
      ret = (char *)xmalloc (3);
      ret[0] = ret[1] = '\'';
      ret[2] = '\0';
    }
  else if (ansic_shouldquote (s))
    ret = ansic_quote (s, 0, (int *)0);
  else if (flags)
    ret = sh_backslash_quote (s, 0, 1);
  else
    ret = sh_single_quote (s);

  return ret;
}

int
sh_contains_shell_metas (string)
     const char *string;
{
  const char *s;

  for (s = string; s && *s; s++)
    {
      switch (*s)
	{
	case ' ': case '\t': case '\n':		/* IFS white space */
	case '\'': case '"': case '\\':		/* quoting chars */
	case '|': case '&': case ';':		/* shell metacharacters */
	case '(': case ')': case '<': case '>':
	case '!': case '{': case '}':		/* reserved words */
	case '*': case '[': case '?': case ']':	/* globbing chars */
	case '^':
	case '$': case '`':			/* expansion chars */
	  return (1);
	case '~':				/* tilde expansion */
	  if (s == string || s[-1] == '=' || s[-1] == ':')
	    return (1);
	  break;
	case '#':
	  if (s == string)			/* comment char */
	    return (1);
	  /* FALLTHROUGH */
	default:
	  break;
	}
    }

  return (0);
}

int
sh_contains_quotes (string)
     const char *string;
{
  const char *s;

  for (s = string; s && *s; s++)
    {
      if (*s == '\'' || *s == '"' || *s == '\\')
	return 1;
    }
  return 0;
}
