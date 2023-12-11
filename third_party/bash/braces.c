/* braces.c -- code for doing word expansion in curly braces. */

/* Copyright (C) 1987-2020 Free Software Foundation, Inc.

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

/* Stuff in curly braces gets expanded before all other shell expansions. */

#include "config.h"

#if defined (BRACE_EXPANSION)

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <errno.h>

#include "bashansi.h"
#include "bashintl.h"

#if defined (SHELL)
#  include "shell.h"
#else
#  if defined (TEST)
typedef char *WORD_DESC;
typedef char **WORD_LIST;
#define _(X)	X
#  endif /* TEST */
#endif /* SHELL */

#include "typemax.h"		/* INTMAX_MIN, INTMAX_MAX */
#include "general.h"
#include "shmbutil.h"
#include "chartypes.h"

#ifndef errno
extern int errno;
#endif

#define brace_whitespace(c) (!(c) || (c) == ' ' || (c) == '\t' || (c) == '\n')

#define BRACE_SEQ_SPECIFIER	".."

extern int asprintf PARAMS((char **, const char *, ...)) __attribute__((__format__ (printf, 2, 3)));

/* Basic idea:

   Segregate the text into 3 sections: preamble (stuff before an open brace),
   postamble (stuff after the matching close brace) and amble (stuff after
   preamble, and before postamble).  Expand amble, and then tack on the
   expansions to preamble.  Expand postamble, and tack on the expansions to
   the result so far.
 */

/* The character which is used to separate arguments. */
static const int brace_arg_separator = ',';

#if defined (PARAMS)
static int brace_gobbler PARAMS((char *, size_t, int *, int));
static char **expand_amble PARAMS((char *, size_t, int));
static char **expand_seqterm PARAMS((char *, size_t));
static char **mkseq PARAMS((intmax_t, intmax_t, intmax_t, int, int));
static char **array_concat PARAMS((char **, char **));
#else
static int brace_gobbler ();
static char **expand_amble ();
static char **expand_seqterm ();
static char **mkseq();
static char **array_concat ();
#endif

#if 0
static void
dump_result (a)
     char **a;
{
  int i;

  for (i = 0; a[i]; i++)
    printf ("dump_result: a[%d] = -%s-\n", i, a[i]);
}
#endif

/* Return an array of strings; the brace expansion of TEXT. */
char **
brace_expand (text)
     char *text;
{
  register int start;
  size_t tlen;
  char *preamble, *postamble, *amble;
  size_t alen;
  char **tack, **result;
  int i, j, c, c1;

  DECLARE_MBSTATE;

  /* Find the text of the preamble. */
  tlen = strlen (text);
  i = 0;
#if defined (CSH_BRACE_COMPAT)
  c = brace_gobbler (text, tlen, &i, '{');	/* } */
#else
  /* Make sure that when we exit this loop, c == 0 or text[i] begins a
     valid brace expansion sequence. */
  do
    {
      c = brace_gobbler (text, tlen, &i, '{');	/* } */
      c1 = c;
      /* Verify that c begins a valid brace expansion word.  If it doesn't, we
	 go on.  Loop stops when there are no more open braces in the word. */
      if (c)
	{
	  start = j = i + 1;	/* { */
	  c = brace_gobbler (text, tlen, &j, '}');
	  if (c == 0)		/* it's not */
	    {
	      i++;
	      c = c1;
	      continue;
	    }
	  else			/* it is */
	    {
	      c = c1;
	      break;
	    }
	}
      else
	break;
    }
  while (c);
#endif /* !CSH_BRACE_COMPAT */

  preamble = (char *)xmalloc (i + 1);
  if (i > 0)
    strncpy (preamble, text, i);
  preamble[i] = '\0';

  result = (char **)xmalloc (2 * sizeof (char *));
  result[0] = preamble;
  result[1] = (char *)NULL;

  /* Special case.  If we never found an exciting character, then
     the preamble is all of the text, so just return that. */
  if (c != '{')
    return (result);

  /* Find the amble.  This is the stuff inside this set of braces. */
  start = ++i;
  c = brace_gobbler (text, tlen, &i, '}');

  /* What if there isn't a matching close brace? */
  if (c == 0)
    {
#if defined (NOTDEF)
      /* Well, if we found an unquoted BRACE_ARG_SEPARATOR between START
	 and I, then this should be an error.  Otherwise, it isn't. */
      j = start;
      while (j < i)
	{
	  if (text[j] == '\\')
	    {
	      j++;
	      ADVANCE_CHAR (text, tlen, j);
	      continue;
	    }

	  if (text[j] == brace_arg_separator)
	    {	/* { */
	      strvec_dispose (result);
	      set_exit_status (EXECUTION_FAILURE);
	      report_error ("no closing `%c' in %s", '}', text);
	      throw_to_top_level ();
	    }
	  ADVANCE_CHAR (text, tlen, j);
	}
#endif
      free (preamble);		/* Same as result[0]; see initialization. */
      result[0] = savestring (text);
      return (result);
    }

#if defined (SHELL)
  amble = substring (text, start, i);
  alen = i - start;
#else
  amble = (char *)xmalloc (1 + (i - start));
  strncpy (amble, &text[start], (i - start));
  alen = i - start;
  amble[alen] = '\0';
#endif

#if defined (SHELL)
  INITIALIZE_MBSTATE;

  /* If the amble does not contain an unquoted BRACE_ARG_SEPARATOR, then
     just return without doing any expansion.  */
  j = 0;
  while (amble[j])
    {
      if (amble[j] == '\\')
	{
	  j++;
	  ADVANCE_CHAR (amble, alen, j);
	  continue;
	}

      if (amble[j] == brace_arg_separator)
	break;

      ADVANCE_CHAR (amble, alen, j);
    }

  if (amble[j] == 0)
    {
      tack = expand_seqterm (amble, alen);
      if (tack)
	goto add_tack;
      else if (text[i + 1])
	{
	  /* If the sequence expansion fails (e.g., because the integers
	     overflow), but there is more in the string, try and process
	     the rest of the string, which may contain additional brace
	     expansions.  Treat the unexpanded sequence term as a simple
	     string (including the braces). */
	  tack = strvec_create (2);
	  tack[0] = savestring (text+start-1);
	  tack[0][i-start+2] = '\0';
	  tack[1] = (char *)0;
	  goto add_tack;
	}
      else
	{
	  free (amble);
	  free (preamble);
	  result[0] = savestring (text);
	  return (result);
	}
    }
#endif /* SHELL */

  tack = expand_amble (amble, alen, 0);
add_tack:
  result = array_concat (result, tack);
  free (amble);
  if (tack != result)
    strvec_dispose (tack);

  postamble = text + i + 1;

  if (postamble && *postamble)
    {
      tack = brace_expand (postamble);
      result = array_concat (result, tack);
      if (tack != result)
	strvec_dispose (tack);
    }

  return (result);
}

/* Expand the text found inside of braces.  We simply try to split the
   text at BRACE_ARG_SEPARATORs into separate strings.  We then brace
   expand each slot which needs it, until there are no more slots which
   need it. */
static char **
expand_amble (text, tlen, flags)
     char *text;
     size_t tlen;
     int flags;
{
  char **result, **partial, **tresult;
  char *tem;
  int start, i, c;

#if defined (SHELL)
  DECLARE_MBSTATE;
#endif

  result = (char **)NULL;

  start = i = 0;
  c = 1;
  while (c)
    {
      c = brace_gobbler (text, tlen, &i, brace_arg_separator);
#if defined (SHELL)
      tem = substring (text, start, i);
#else
      tem = (char *)xmalloc (1 + (i - start));
      strncpy (tem, &text[start], (i - start));
      tem[i - start] = '\0';
#endif

      partial = brace_expand (tem);

      if (!result)
	result = partial;
      else
	{
	  register int lr, lp, j;

	  lr = strvec_len (result);
	  lp = strvec_len (partial);

	  tresult = strvec_mresize (result, lp + lr + 1);
	  if (tresult == 0)
	    {
	      internal_error (_("brace expansion: cannot allocate memory for %s"), tem);
	      free (tem);
	      strvec_dispose (partial);
	      strvec_dispose (result);
	      result = (char **)NULL;
	      return result;
	    }
	  else
	    result = tresult;

	  for (j = 0; j < lp; j++)
	    result[lr + j] = partial[j];

	  result[lr + j] = (char *)NULL;
	  free (partial);
	}
      free (tem);
#if defined (SHELL)
      ADVANCE_CHAR (text, tlen, i);
#else
      i++;
#endif
      start = i;
    }
  return (result);
}

#define ST_BAD	0
#define ST_INT	1
#define ST_CHAR	2
#define ST_ZINT	3

static char **
mkseq (start, end, incr, type, width)
     intmax_t start, end, incr;
     int type, width;
{
  intmax_t n, prevn;
  int i, nelem;
  char **result, *t;

  if (incr == 0)
    incr = 1;

  if (start > end && incr > 0)
    incr = -incr;
  else if (start < end && incr < 0)
    {
      if (incr == INTMAX_MIN)		/* Don't use -INTMAX_MIN */
	return ((char **)NULL);
      incr = -incr;
    }

  /* Check that end-start will not overflow INTMAX_MIN, INTMAX_MAX.  The +3
     and -2, not strictly necessary, are there because of the way the number
     of elements and value passed to strvec_create() are calculated below. */
  if (SUBOVERFLOW (end, start, INTMAX_MIN+3, INTMAX_MAX-2))
    return ((char **)NULL);

  prevn = sh_imaxabs (end - start);
  /* Need to check this way in case INT_MAX == INTMAX_MAX */
  if (INT_MAX == INTMAX_MAX && (ADDOVERFLOW (prevn, 2, INT_MIN, INT_MAX)))
    return ((char **)NULL);
  /* Make sure the assignment to nelem below doesn't end up <= 0 due to
     intmax_t overflow */
  else if (ADDOVERFLOW ((prevn/sh_imaxabs(incr)), 1, INTMAX_MIN, INTMAX_MAX))
    return ((char **)NULL);

  /* XXX - TOFIX: potentially allocating a lot of extra memory if
     imaxabs(incr) != 1 */
  /* Instead of a simple nelem = prevn + 1, something like:
  	nelem = (prevn / imaxabs(incr)) + 1;
     would work */
  if ((prevn / sh_imaxabs (incr)) > INT_MAX - 3)	/* check int overflow */
    return ((char **)NULL);
  nelem = (prevn / sh_imaxabs(incr)) + 1;
  result = strvec_mcreate (nelem + 1);
  if (result == 0)
    {
      internal_error (_("brace expansion: failed to allocate memory for %u elements"), (unsigned int)nelem);
      return ((char **)NULL);
    }

  /* Make sure we go through the loop at least once, so {3..3} prints `3' */
  i = 0;
  n = start;
  do
    {
#if defined (SHELL)
      if (ISINTERRUPT)
        {
          result[i] = (char *)NULL;
          strvec_dispose (result);
          result = (char **)NULL;
        }
      QUIT;
#endif
      if (type == ST_INT)
	result[i++] = t = itos (n);
      else if (type == ST_ZINT)
	{
	  int len, arg;
	  arg = n;
	  len = asprintf (&t, "%0*d", width, arg);
	  result[i++] = t;
	}
      else
	{
	  if (t = (char *)malloc (2))
	    {
	      t[0] = n;
	      t[1] = '\0';
	    }
	  result[i++] = t;
	}

      /* We failed to allocate memory for this number, so we bail. */
      if (t == 0)
	{
	  char *p, lbuf[INT_STRLEN_BOUND(intmax_t) + 1];

	  /* Easier to do this than mess around with various intmax_t printf
	     formats (%ld? %lld? %jd?) and PRIdMAX. */
	  p = inttostr (n, lbuf, sizeof (lbuf));
	  internal_error (_("brace expansion: failed to allocate memory for `%s'"), p);
	  strvec_dispose (result);
	  return ((char **)NULL);
	}

      /* Handle overflow and underflow of n+incr */
      if (ADDOVERFLOW (n, incr, INTMAX_MIN, INTMAX_MAX))
        break;

      n += incr;

      if ((incr < 0 && n < end) || (incr > 0 && n > end))
	break;
    }
  while (1);

  result[i] = (char *)0;
  return (result);
}

static char **
expand_seqterm (text, tlen)
     char *text;
     size_t tlen;
{
  char *t, *lhs, *rhs;
  int lhs_t, rhs_t, lhs_l, rhs_l, width;
  intmax_t lhs_v, rhs_v, incr;
  intmax_t tl, tr;
  char **result, *ep, *oep;

  t = strstr (text, BRACE_SEQ_SPECIFIER);
  if (t == 0)
    return ((char **)NULL);

  lhs_l = t - text;		/* index of start of BRACE_SEQ_SPECIFIER */
  lhs = substring (text, 0, lhs_l);
  rhs = substring (text, lhs_l + sizeof(BRACE_SEQ_SPECIFIER) - 1, tlen);

  if (lhs[0] == 0 || rhs[0] == 0)
    {
      free (lhs);
      free (rhs);
      return ((char **)NULL);
    }

  /* Now figure out whether LHS and RHS are integers or letters.  Both
     sides have to match. */
  lhs_t = (legal_number (lhs, &tl)) ? ST_INT :
  		((ISALPHA (lhs[0]) && lhs[1] == 0) ?  ST_CHAR : ST_BAD);

  /* Decide on rhs and whether or not it looks like the user specified
     an increment */
  ep = 0;
  if (ISDIGIT (rhs[0]) || ((rhs[0] == '+' || rhs[0] == '-') && ISDIGIT (rhs[1])))
    {
      rhs_t = ST_INT;
      errno = 0;
      tr = strtoimax (rhs, &ep, 10);
      if (errno == ERANGE || (ep && *ep != 0 && *ep != '.'))
	rhs_t = ST_BAD;			/* invalid */
    }
  else if (ISALPHA (rhs[0]) && (rhs[1] == 0 || rhs[1] == '.'))
    {
      rhs_t = ST_CHAR;
      ep = rhs + 1;
    }
  else
    {
      rhs_t = ST_BAD;
      ep = 0;
    }

  incr = 1;
  if (rhs_t != ST_BAD)
    {
      oep = ep;
      errno = 0;
      if (ep && *ep == '.' && ep[1] == '.' && ep[2])
	incr = strtoimax (ep + 2, &ep, 10);
      if (*ep != 0 || errno == ERANGE)
	rhs_t = ST_BAD;			/* invalid incr or overflow */
      tlen -= ep - oep;
    }

  if (lhs_t != rhs_t || lhs_t == ST_BAD || rhs_t == ST_BAD)
    {
      free (lhs);
      free (rhs);
      return ((char **)NULL);
    }

  /* OK, we have something.  It's either a sequence of integers, ascending
     or descending, or a sequence or letters, ditto.  Generate the sequence,
     put it into a string vector, and return it. */
  
  if (lhs_t == ST_CHAR)
    {
      lhs_v = (unsigned char)lhs[0];
      rhs_v = (unsigned char)rhs[0];
      width = 1;
    }
  else
    {
      lhs_v = tl;		/* integer truncation */
      rhs_v = tr;

      /* Decide whether or not the terms need zero-padding */
      rhs_l = tlen - lhs_l - sizeof (BRACE_SEQ_SPECIFIER) + 1;
      width = 0;
      if (lhs_l > 1 && lhs[0] == '0')
	width = lhs_l, lhs_t = ST_ZINT;
      if (lhs_l > 2 && lhs[0] == '-' && lhs[1] == '0')
	width = lhs_l, lhs_t = ST_ZINT;
      if (rhs_l > 1 && rhs[0] == '0' && width < rhs_l)
	width = rhs_l, lhs_t = ST_ZINT;
      if (rhs_l > 2 && rhs[0] == '-' && rhs[1] == '0' && width < rhs_l)
	width = rhs_l, lhs_t = ST_ZINT;

      if (width < lhs_l && lhs_t == ST_ZINT)
        width = lhs_l;
      if (width < rhs_l && lhs_t == ST_ZINT)
        width = rhs_l;
    }

  result = mkseq (lhs_v, rhs_v, incr, lhs_t, width);

  free (lhs);
  free (rhs);

  return (result);
}

/* Start at INDEX, and skip characters in TEXT. Set INDEX to the
   index of the character matching SATISFY.  This understands about
   quoting.  Return the character that caused us to stop searching;
   this is either the same as SATISFY, or 0. */
/* If SATISFY is `}', we are looking for a brace expression, so we
   should enforce the rules that govern valid brace expansions:
	1) to count as an arg separator, a comma or `..' has to be outside
	   an inner set of braces.	 
*/
static int
brace_gobbler (text, tlen, indx, satisfy)
     char *text;
     size_t tlen;
     int *indx;
     int satisfy;
{
  register int i, c, quoted, level, commas, pass_next;
#if defined (SHELL)
  int si;
  char *t;
#endif
  DECLARE_MBSTATE;

  level = quoted = pass_next = 0;
#if defined (CSH_BRACE_COMPAT)
  commas = 1;
#else
  commas = (satisfy == '}') ? 0 : 1;
#endif

  i = *indx;
  while (c = text[i])
    {
      if (pass_next)
	{
	  pass_next = 0;
#if defined (SHELL)
	  ADVANCE_CHAR (text, tlen, i);
#else
	  i++;
#endif
	  continue;
	}

      /* A backslash escapes the next character.  This allows backslash to
	 escape the quote character in a double-quoted string. */
      if (c == '\\' && (quoted == 0 || quoted == '"' || quoted == '`'))
	{
	  pass_next = 1;
	  i++;
	  continue;
	}

#if defined (SHELL)
      /* If compiling for the shell, treat ${...} like \{...} */
      if (c == '$' && text[i+1] == '{' && quoted != '\'')		/* } */
	{
	  pass_next = 1;
	  i++;
	  if (quoted == 0)
	    level++;
	  continue;
	}
#endif

      if (quoted)
	{
	  if (c == quoted)
	    quoted = 0;
#if defined (SHELL)
	  /* The shell allows quoted command substitutions */
	  if (quoted == '"' && c == '$' && text[i+1] == '(')	/*)*/
	    goto comsub;
#endif
#if defined (SHELL)
	  ADVANCE_CHAR (text, tlen, i);
#else
	  i++;
#endif
	  continue;
	}

      if (c == '"' || c == '\'' || c == '`')
	{
	  quoted = c;
	  i++;
	  continue;
	}

#if defined (SHELL)
      /* Pass new-style command and process substitutions through unchanged. */
      if ((c == '$' || c == '<' || c == '>') && text[i+1] == '(')			/* ) */
	{
comsub:
	  si = i + 2;
	  t = extract_command_subst (text, &si, 0);
	  i = si;
	  free (t);
	  i++;
	  continue;
	}
#endif

      if (c == satisfy && level == 0 && quoted == 0 && commas > 0)
	{
	  /* We ignore an open brace surrounded by whitespace, and also
	     an open brace followed immediately by a close brace preceded
	     by whitespace.  */
	  if (c == '{' &&
	      ((!i || brace_whitespace (text[i - 1])) &&
	       (brace_whitespace (text[i + 1]) || text[i + 1] == '}')))
	    {
	      i++;
	      continue;
	    }

	    break;
	}

      if (c == '{')
	level++;
      else if (c == '}' && level)
	level--;
#if !defined (CSH_BRACE_COMPAT)
      else if (satisfy == '}' && c == brace_arg_separator && level == 0)
	commas++;
      else if (satisfy == '}' && STREQN (text+i, BRACE_SEQ_SPECIFIER, 2) &&
      		text[i+2] != satisfy && level == 0)
	commas++;
#endif

#if defined (SHELL)
      ADVANCE_CHAR (text, tlen, i);
#else
      i++;
#endif
    }

  *indx = i;
  return (c);
}

/* Return a new array of strings which is the result of appending each
   string in ARR2 to each string in ARR1.  The resultant array is
   len (arr1) * len (arr2) long.  For convenience, ARR1 (and its contents)
   are free ()'ed.  ARR1 can be NULL, in that case, a new version of ARR2
   is returned. */
static char **
array_concat (arr1, arr2)
     char **arr1, **arr2;
{
  register int i, j, len, len1, len2;
  register char **result;

  if (arr1 == 0)
    return (arr2);		/* XXX - see if we can get away without copying? */

  if (arr2 == 0)
    return (arr1);		/* XXX - caller expects us to free arr1 */

  /* We can only short-circuit if the array consists of a single null element;
     otherwise we need to replicate the contents of the other array and
     prefix (or append, below) an empty element to each one. */
  if (arr1[0] && arr1[0][0] == 0 && arr1[1] == 0)
    {
      strvec_dispose (arr1);
      return (arr2);		/* XXX - use flags to see if we can avoid copying here */
    }

  if (arr2[0] && arr2[0][0] == 0 && arr2[1] == 0)
    return (arr1);		/* XXX - rather than copying and freeing it */

  len1 = strvec_len (arr1);
  len2 = strvec_len (arr2);

  result = (char **)malloc ((1 + (len1 * len2)) * sizeof (char *));
  if (result == 0)
    return (result);

  len = 0;
  for (i = 0; i < len1; i++)
    {
      int strlen_1 = strlen (arr1[i]);

      for (j = 0; j < len2; j++)
	{
	  result[len] = (char *)xmalloc (1 + strlen_1 + strlen (arr2[j]));
	  strcpy (result[len], arr1[i]);
	  strcpy (result[len] + strlen_1, arr2[j]);
	  len++;
	}
      free (arr1[i]);
    }
  free (arr1);

  result[len] = (char *)NULL;
  return (result);
}

#if defined (TEST)
#include <stdio.h>

void *
xmalloc(n)
     size_t n;
{
  return (malloc (n));
}

void *
xrealloc(p, n)
     void *p;
     size_t n;
{
  return (realloc (p, n));
}

int
internal_error (format, arg1, arg2)
     char *format, *arg1, *arg2;
{
  fprintf (stderr, format, arg1, arg2);
  fprintf (stderr, "\n");
}
      
main ()
{
  char example[256];

  for (;;)
    {
      char **result;
      int i;

      fprintf (stderr, "brace_expand> ");

      if ((!fgets (example, 256, stdin)) ||
	  (strncmp (example, "quit", 4) == 0))
	break;

      if (strlen (example))
	example[strlen (example) - 1] = '\0';

      result = brace_expand (example);

      for (i = 0; result[i]; i++)
	printf ("%s\n", result[i]);

      strvec_dispose (result);
    }
}

/*
 * Local variables:
 * compile-command: "gcc -g -Bstatic -DTEST -o brace_expand braces.c general.o"
 * end:
 */

#endif /* TEST */
#endif /* BRACE_EXPANSION */
