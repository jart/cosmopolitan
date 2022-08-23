/* Builtin function expansion for GNU Make.
Copyright (C) 1988-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* clang-format off */
#include "third_party/make/makeint.inc"
/**/
#include "third_party/make/filedef.h"
#include "third_party/make/variable.h"
#include "third_party/make/dep.h"
#include "third_party/make/job.h"
#include "third_party/make/os.h"
#include "third_party/make/commands.h"
#include "libc/mem/critbit0.h"
#include "libc/log/rop.h"
#include "libc/runtime/runtime.h"
#include "third_party/make/debug.h"

struct function_table_entry
  {
    union {
      char *(*func_ptr) (char *output, char **argv, const char *fname);
      gmk_func_ptr alloc_func_ptr;
    } fptr;
    const char *name;
    unsigned char len;
    unsigned char minimum_args;
    unsigned char maximum_args;
    unsigned int expand_args:1;
    unsigned int alloc_fn:1;
  };

static unsigned long
function_table_entry_hash_1 (const void *keyv)
{
  const struct function_table_entry *key = keyv;
  return_STRING_N_HASH_1 (key->name, key->len);
}

static unsigned long
function_table_entry_hash_2 (const void *keyv)
{
  const struct function_table_entry *key = keyv;
  return_STRING_N_HASH_2 (key->name, key->len);
}

static int
function_table_entry_hash_cmp (const void *xv, const void *yv)
{
  const struct function_table_entry *x = xv;
  const struct function_table_entry *y = yv;
  int result = x->len - y->len;
  if (result)
    return result;
  return_STRING_N_COMPARE (x->name, y->name, x->len);
}

static struct hash_table function_table;


/* Store into VARIABLE_BUFFER at O the result of scanning TEXT and replacing
   each occurrence of SUBST with REPLACE. TEXT is null-terminated.  SLEN is
   the length of SUBST and RLEN is the length of REPLACE.  If BY_WORD is
   nonzero, substitutions are done only on matches which are complete
   whitespace-delimited words.  */

char *
subst_expand (char *o, const char *text, const char *subst, const char *replace,
              size_t slen, size_t rlen, int by_word)
{
  const char *t = text;
  const char *p;

  if (slen == 0 && !by_word)
    {
      /* The first occurrence of "" in any string is its end.  */
      o = variable_buffer_output (o, t, strlen (t));
      if (rlen > 0)
        o = variable_buffer_output (o, replace, rlen);
      return o;
    }

  do
    {
      if (by_word && slen == 0)
        /* When matching by words, the empty string should match
           the end of each word, rather than the end of the whole text.  */
        p = end_of_token (next_token (t));
      else
        {
          p = strstr (t, subst);
          if (p == 0)
            {
              /* No more matches.  Output everything left on the end.  */
              o = variable_buffer_output (o, t, strlen (t));
              return o;
            }
        }

      /* Output everything before this occurrence of the string to replace.  */
      if (p > t)
        o = variable_buffer_output (o, t, p - t);

      /* If we're substituting only by fully matched words,
         or only at the ends of words, check that this case qualifies.  */
      if (by_word
          && ((p > text && !ISSPACE (p[-1]))
              || ! STOP_SET (p[slen], MAP_SPACE|MAP_NUL)))
        /* Struck out.  Output the rest of the string that is
           no longer to be replaced.  */
        o = variable_buffer_output (o, subst, slen);
      else if (rlen > 0)
        /* Output the replacement string.  */
        o = variable_buffer_output (o, replace, rlen);

      /* Advance T past the string to be replaced.  */
      t = p + slen;
    } while (*t != '\0');

  return o;
}


/* Store into VARIABLE_BUFFER at O the result of scanning TEXT
   and replacing strings matching PATTERN with REPLACE.
   If PATTERN_PERCENT is not nil, PATTERN has already been
   run through find_percent, and PATTERN_PERCENT is the result.
   If REPLACE_PERCENT is not nil, REPLACE has already been
   run through find_percent, and REPLACE_PERCENT is the result.
   Note that we expect PATTERN_PERCENT and REPLACE_PERCENT to point to the
   character _AFTER_ the %, not to the % itself.
*/

char *
patsubst_expand_pat (char *o, const char *text,
                     const char *pattern, const char *replace,
                     const char *pattern_percent, const char *replace_percent)
{
  size_t pattern_prepercent_len, pattern_postpercent_len;
  size_t replace_prepercent_len, replace_postpercent_len;
  const char *t;
  size_t len;
  int doneany = 0;

  /* Record the length of REPLACE before and after the % so we don't have to
     compute these lengths more than once.  */
  if (replace_percent)
    {
      replace_prepercent_len = replace_percent - replace - 1;
      replace_postpercent_len = strlen (replace_percent);
    }
  else
    {
      replace_prepercent_len = strlen (replace);
      replace_postpercent_len = 0;
    }

  if (!pattern_percent)
    /* With no % in the pattern, this is just a simple substitution.  */
    return subst_expand (o, text, pattern, replace,
                         strlen (pattern), strlen (replace), 1);

  /* Record the length of PATTERN before and after the %
     so we don't have to compute it more than once.  */
  pattern_prepercent_len = pattern_percent - pattern - 1;
  pattern_postpercent_len = strlen (pattern_percent);

  while ((t = find_next_token (&text, &len)) != 0)
    {
      int fail = 0;

      /* Is it big enough to match?  */
      if (len < pattern_prepercent_len + pattern_postpercent_len)
        fail = 1;

      /* Does the prefix match? */
      if (!fail && pattern_prepercent_len > 0
          && (*t != *pattern
              || t[pattern_prepercent_len - 1] != pattern_percent[-2]
              || !strneq (t + 1, pattern + 1, pattern_prepercent_len - 1)))
        fail = 1;

      /* Does the suffix match? */
      if (!fail && pattern_postpercent_len > 0
          && (t[len - 1] != pattern_percent[pattern_postpercent_len - 1]
              || t[len - pattern_postpercent_len] != *pattern_percent
              || !strneq (&t[len - pattern_postpercent_len],
                          pattern_percent, pattern_postpercent_len - 1)))
        fail = 1;

      if (fail)
        /* It didn't match.  Output the string.  */
        o = variable_buffer_output (o, t, len);
      else
        {
          /* It matched.  Output the replacement.  */

          /* Output the part of the replacement before the %.  */
          o = variable_buffer_output (o, replace, replace_prepercent_len);

          if (replace_percent != 0)
            {
              /* Output the part of the matched string that
                 matched the % in the pattern.  */
              o = variable_buffer_output (o, t + pattern_prepercent_len,
                                          len - (pattern_prepercent_len
                                                 + pattern_postpercent_len));
              /* Output the part of the replacement after the %.  */
              o = variable_buffer_output (o, replace_percent,
                                          replace_postpercent_len);
            }
        }

      /* Output a space, but not if the replacement is "".  */
      if (fail || replace_prepercent_len > 0
          || (replace_percent != 0 && len + replace_postpercent_len > 0))
        {
          o = variable_buffer_output (o, " ", 1);
          doneany = 1;
        }
    }
  if (doneany)
    /* Kill the last space.  */
    --o;

  return o;
}

/* Store into VARIABLE_BUFFER at O the result of scanning TEXT
   and replacing strings matching PATTERN with REPLACE.
   If PATTERN_PERCENT is not nil, PATTERN has already been
   run through find_percent, and PATTERN_PERCENT is the result.
   If REPLACE_PERCENT is not nil, REPLACE has already been
   run through find_percent, and REPLACE_PERCENT is the result.
   Note that we expect PATTERN_PERCENT and REPLACE_PERCENT to point to the
   character _AFTER_ the %, not to the % itself.
*/

char *
patsubst_expand (char *o, const char *text, char *pattern, char *replace)
{
  const char *pattern_percent = find_percent (pattern);
  const char *replace_percent = find_percent (replace);

  /* If there's a percent in the pattern or replacement skip it.  */
  if (replace_percent)
    ++replace_percent;
  if (pattern_percent)
    ++pattern_percent;

  return patsubst_expand_pat (o, text, pattern, replace,
                              pattern_percent, replace_percent);
}


/* Look up a function by name.  */

static const struct function_table_entry *
lookup_function (const char *s)
{
  struct function_table_entry function_table_entry_key;
  const char *e = s;

  while (STOP_SET (*e, MAP_USERFUNC))
    e++;

  if (e == s || !STOP_SET(*e, MAP_NUL|MAP_SPACE))
    return NULL;

  function_table_entry_key.name = s;
  function_table_entry_key.len = (unsigned char) (e - s);

  return hash_find_item (&function_table, &function_table_entry_key);
}


/* Return 1 if PATTERN matches STR, 0 if not.  */

int
pattern_matches (const char *pattern, const char *percent, const char *str)
{
  size_t sfxlen, strlength;

  if (percent == 0)
    {
      size_t len = strlen (pattern) + 1;
      char *new_chars = alloca (len);
      memcpy (new_chars, pattern, len);
      percent = find_percent (new_chars);
      if (percent == 0)
        return streq (new_chars, str);
      pattern = new_chars;
    }

  sfxlen = strlen (percent + 1);
  strlength = strlen (str);

  if (strlength < (percent - pattern) + sfxlen
      || !strneq (pattern, str, percent - pattern))
    return 0;

  return !strcmp (percent + 1, str + (strlength - sfxlen));
}


/* Find the next comma or ENDPAREN (counting nested STARTPAREN and
   ENDPARENtheses), starting at PTR before END.  Return a pointer to
   next character.

   If no next argument is found, return NULL.
*/

static char *
find_next_argument (char startparen, char endparen,
                    const char *ptr, const char *end)
{
  int count = 0;

  for (; ptr < end; ++ptr)
    if (!STOP_SET (*ptr, MAP_VARSEP|MAP_COMMA))
      continue;

    else if (*ptr == startparen)
      ++count;

    else if (*ptr == endparen)
      {
        --count;
        if (count < 0)
          return NULL;
      }

    else if (*ptr == ',' && !count)
      return (char *)ptr;

  /* We didn't find anything.  */
  return NULL;
}


/* Glob-expand LINE.  The returned pointer is
   only good until the next call to string_glob.  */

static char *
string_glob (char *line)
{
  static char *result = 0;
  static size_t length;
  struct nameseq *chain;
  size_t idx;

  chain = PARSE_FILE_SEQ (&line, struct nameseq, MAP_NUL, NULL,
                          /* We do not want parse_file_seq to strip './'s.
                             That would break examples like:
                             $(patsubst ./%.c,obj/%.o,$(wildcard ./?*.c)).  */
                          PARSEFS_NOSTRIP|PARSEFS_NOCACHE|PARSEFS_EXISTS);

  if (result == 0)
    {
      length = 100;
      result = xmalloc (100);
    }

  idx = 0;
  while (chain != 0)
    {
      struct nameseq *next = chain->next;
      size_t len = strlen (chain->name);

      if (idx + len + 1 > length)
        {
          length += (len + 1) * 2;
          result = xrealloc (result, length);
        }
      memcpy (&result[idx], chain->name, len);
      idx += len;
      result[idx++] = ' ';

      /* Because we used PARSEFS_NOCACHE above, we have to free() NAME.  */
      free ((char *)chain->name);
      free (chain);
      chain = next;
    }

  /* Kill the last space and terminate the string.  */
  if (idx == 0)
    result[0] = '\0';
  else
    result[idx - 1] = '\0';

  return result;
}

/*
  Builtin functions
 */

static char *
func_patsubst (char *o, char **argv, const char *funcname UNUSED)
{
  o = patsubst_expand (o, argv[2], argv[0], argv[1]);
  return o;
}


static char *
func_join (char *o, char **argv, const char *funcname UNUSED)
{
  int doneany = 0;

  /* Write each word of the first argument directly followed
     by the corresponding word of the second argument.
     If the two arguments have a different number of words,
     the excess words are just output separated by blanks.  */
  const char *tp;
  const char *pp;
  const char *list1_iterator = argv[0];
  const char *list2_iterator = argv[1];
  do
    {
      size_t len1, len2;

      tp = find_next_token (&list1_iterator, &len1);
      if (tp != 0)
        o = variable_buffer_output (o, tp, len1);

      pp = find_next_token (&list2_iterator, &len2);
      if (pp != 0)
        o = variable_buffer_output (o, pp, len2);

      if (tp != 0 || pp != 0)
        {
          o = variable_buffer_output (o, " ", 1);
          doneany = 1;
        }
    }
  while (tp != 0 || pp != 0);
  if (doneany)
    /* Kill the last blank.  */
    --o;

  return o;
}


static char *
func_origin (char *o, char **argv, const char *funcname UNUSED)
{
  /* Expand the argument.  */
  struct variable *v = lookup_variable (argv[0], strlen (argv[0]));
  if (v == 0)
    o = variable_buffer_output (o, "undefined", 9);
  else
    switch (v->origin)
      {
      default:
      case o_invalid:
        abort ();
        break;
      case o_default:
        o = variable_buffer_output (o, "default", 7);
        break;
      case o_env:
        o = variable_buffer_output (o, "environment", 11);
        break;
      case o_file:
        o = variable_buffer_output (o, "file", 4);
        break;
      case o_env_override:
        o = variable_buffer_output (o, "environment override", 20);
        break;
      case o_command:
        o = variable_buffer_output (o, "command line", 12);
        break;
      case o_override:
        o = variable_buffer_output (o, "override", 8);
        break;
      case o_automatic:
        o = variable_buffer_output (o, "automatic", 9);
        break;
      }

  return o;
}

static char *
func_flavor (char *o, char **argv, const char *funcname UNUSED)
{
  struct variable *v = lookup_variable (argv[0], strlen (argv[0]));

  if (v == 0)
    o = variable_buffer_output (o, "undefined", 9);
  else
    if (v->recursive)
      o = variable_buffer_output (o, "recursive", 9);
    else
      o = variable_buffer_output (o, "simple", 6);

  return o;
}


static char *
func_notdir_suffix (char *o, char **argv, const char *funcname)
{
  /* Expand the argument.  */
  const char *list_iterator = argv[0];
  const char *p2;
  int doneany =0;
  size_t len=0;

  int is_suffix = funcname[0] == 's';
  int is_notdir = !is_suffix;
  int stop = MAP_DIRSEP | (is_suffix ? MAP_DOT : 0);
  while ((p2 = find_next_token (&list_iterator, &len)) != 0)
    {
      const char *p = p2 + len - 1;

      while (p >= p2 && ! STOP_SET (*p, stop))
        --p;

      if (p >= p2)
        {
          if (is_notdir)
            ++p;
          else if (*p != '.')
            continue;
          o = variable_buffer_output (o, p, len - (p - p2));
        }
      else if (is_notdir)
        o = variable_buffer_output (o, p2, len);

      if (is_notdir || p >= p2)
        {
          o = variable_buffer_output (o, " ", 1);
          doneany = 1;
        }
    }

  if (doneany)
    /* Kill last space.  */
    --o;

  return o;
}


static char *
func_basename_dir (char *o, char **argv, const char *funcname)
{
  /* Expand the argument.  */
  const char *p3 = argv[0];
  const char *p2;
  int doneany = 0;
  size_t len = 0;

  int is_basename = funcname[0] == 'b';
  int is_dir = !is_basename;
  int stop = MAP_DIRSEP | (is_basename ? MAP_DOT : 0) | MAP_NUL;
  while ((p2 = find_next_token (&p3, &len)) != 0)
    {
      const char *p = p2 + len - 1;
      while (p >= p2 && ! STOP_SET (*p, stop))
        --p;

      if (p >= p2 && (is_dir))
        o = variable_buffer_output (o, p2, ++p - p2);
      else if (p >= p2 && (*p == '.'))
        o = variable_buffer_output (o, p2, p - p2);
      else if (is_dir)
        o = variable_buffer_output (o, "./", 2);
      else
        /* The entire name is the basename.  */
        o = variable_buffer_output (o, p2, len);
      o = variable_buffer_output (o, " ", 1);

      doneany = 1;
    }

  if (doneany)
    /* Kill last space.  */
    --o;

  return o;
}

static char *
func_addsuffix_addprefix (char *o, char **argv, const char *funcname)
{
  size_t fixlen = strlen (argv[0]);
  const char *list_iterator = argv[1];
  int is_addprefix = funcname[3] == 'p';
  int is_addsuffix = !is_addprefix;

  int doneany = 0;
  const char *p;
  size_t len;

  while ((p = find_next_token (&list_iterator, &len)) != 0)
    {
      if (is_addprefix)
        o = variable_buffer_output (o, argv[0], fixlen);
      o = variable_buffer_output (o, p, len);
      if (is_addsuffix)
        o = variable_buffer_output (o, argv[0], fixlen);
      o = variable_buffer_output (o, " ", 1);
      doneany = 1;
    }

  if (doneany)
    /* Kill last space.  */
    --o;

  return o;
}

static char *
func_subst (char *o, char **argv, const char *funcname UNUSED)
{
  o = subst_expand (o, argv[2], argv[0], argv[1], strlen (argv[0]),
                    strlen (argv[1]), 0);

  return o;
}


static char *
func_firstword (char *o, char **argv, const char *funcname UNUSED)
{
  size_t i;
  const char *words = argv[0];    /* Use a temp variable for find_next_token */
  const char *p = find_next_token (&words, &i);

  if (p != 0)
    o = variable_buffer_output (o, p, i);

  return o;
}

static char *
func_lastword (char *o, char **argv, const char *funcname UNUSED)
{
  size_t i;
  const char *words = argv[0];    /* Use a temp variable for find_next_token */
  const char *p = NULL;
  const char *t;

  while ((t = find_next_token (&words, &i)) != NULL)
    p = t;

  if (p != 0)
    o = variable_buffer_output (o, p, i);

  return o;
}

static char *
func_words (char *o, char **argv, const char *funcname UNUSED)
{
  int i = 0;
  const char *word_iterator = argv[0];
  char buf[20];

  while (find_next_token (&word_iterator, NULL) != 0)
    ++i;

  sprintf (buf, "%d", i);
  o = variable_buffer_output (o, buf, strlen (buf));

  return o;
}

/* Set begpp to point to the first non-whitespace character of the string,
 * and endpp to point to the last non-whitespace character of the string.
 * If the string is empty or contains nothing but whitespace, endpp will be
 * begpp-1.
 */
char *
strip_whitespace (const char **begpp, const char **endpp)
{
  while (*begpp <= *endpp && ISSPACE (**begpp))
    (*begpp) ++;
  while (*endpp >= *begpp && ISSPACE (**endpp))
    (*endpp) --;
  return (char *)*begpp;
}

static void
check_numeric (const char *s, const char *msg)
{
  const char *end = s + strlen (s) - 1;
  const char *beg = s;
  strip_whitespace (&s, &end);

  for (; s <= end; ++s)
    if (!ISDIGIT (*s))  /* ISDIGIT only evals its arg once: see makeint.h.  */
      break;

  if (s <= end || end - beg < 0)
    OSS (fatal, *expanding_var, "%s: '%s'", msg, beg);
}



static char *
func_word (char *o, char **argv, const char *funcname UNUSED)
{
  const char *end_p;
  const char *p;
  int i;

  /* Check the first argument.  */
  check_numeric (argv[0], _("non-numeric first argument to 'word' function"));
  i = atoi (argv[0]);

  if (i == 0)
    O (fatal, *expanding_var,
       _("first argument to 'word' function must be greater than 0"));

  end_p = argv[1];
  while ((p = find_next_token (&end_p, 0)) != 0)
    if (--i == 0)
      break;

  if (i == 0)
    o = variable_buffer_output (o, p, end_p - p);

  return o;
}

static char *
func_wordlist (char *o, char **argv, const char *funcname UNUSED)
{
  int start, count;

  /* Check the arguments.  */
  check_numeric (argv[0],
                 _("non-numeric first argument to 'wordlist' function"));
  check_numeric (argv[1],
                 _("non-numeric second argument to 'wordlist' function"));

  start = atoi (argv[0]);
  if (start < 1)
    ON (fatal, *expanding_var,
        "invalid first argument to 'wordlist' function: '%d'", start);

  count = atoi (argv[1]) - start + 1;

  if (count > 0)
    {
      const char *p;
      const char *end_p = argv[2];

      /* Find the beginning of the "start"th word.  */
      while (((p = find_next_token (&end_p, 0)) != 0) && --start)
        ;

      if (p)
        {
          /* Find the end of the "count"th word from start.  */
          while (--count && (find_next_token (&end_p, 0) != 0))
            ;

          /* Return the stuff in the middle.  */
          o = variable_buffer_output (o, p, end_p - p);
        }
    }

  return o;
}

static char *
func_findstring (char *o, char **argv, const char *funcname UNUSED)
{
  /* Find the first occurrence of the first string in the second.  */
  if (strstr (argv[1], argv[0]) != 0)
    o = variable_buffer_output (o, argv[0], strlen (argv[0]));

  return o;
}

static char *
func_foreach (char *o, char **argv, const char *funcname UNUSED)
{
  /* expand only the first two.  */
  char *varname = expand_argument (argv[0], NULL);
  char *list = expand_argument (argv[1], NULL);
  const char *body = argv[2];

  int doneany = 0;
  const char *list_iterator = list;
  const char *p;
  size_t len;
  struct variable *var;

  /* Clean up the variable name by removing whitespace.  */
  char *vp = next_token (varname);
  end_of_token (vp)[0] = '\0';

  push_new_variable_scope ();
  var = define_variable (vp, strlen (vp), "", o_automatic, 0);

  /* loop through LIST,  put the value in VAR and expand BODY */
  while ((p = find_next_token (&list_iterator, &len)) != 0)
    {
      char *result = 0;

      free (var->value);
      var->value = xstrndup (p, len);

      result = allocated_variable_expand (body);

      o = variable_buffer_output (o, result, strlen (result));
      o = variable_buffer_output (o, " ", 1);
      doneany = 1;
      free (result);
    }

  if (doneany)
    /* Kill the last space.  */
    --o;

  pop_variable_scope ();
  free (varname);
  free (list);

  return o;
}

struct a_word
{
  struct a_word *next;
  struct a_word *chain;
  char *str;
  size_t length;
  int matched;
};

static unsigned long
a_word_hash_1 (const void *key)
{
  return_STRING_HASH_1 (((struct a_word const *) key)->str);
}

static unsigned long
a_word_hash_2 (const void *key)
{
  return_STRING_HASH_2 (((struct a_word const *) key)->str);
}

static int
a_word_hash_cmp (const void *x, const void *y)
{
  int result = (int) ((struct a_word const *) x)->length - ((struct a_word const *) y)->length;
  if (result)
    return result;
  return_STRING_COMPARE (((struct a_word const *) x)->str,
                         ((struct a_word const *) y)->str);
}

struct a_pattern
{
  struct a_pattern *next;
  char *str;
  char *percent;
  size_t length;
};

static char *
func_filter_filterout (char *o, char **argv, const char *funcname)
{
  struct a_word *wordhead;
  struct a_word **wordtail;
  struct a_word *wp;
  struct a_pattern *pathead;
  struct a_pattern **pattail;
  struct a_pattern *pp;

  struct hash_table a_word_table;
  int is_filter = funcname[CSTRLEN ("filter")] == '\0';
  const char *pat_iterator = argv[0];
  const char *word_iterator = argv[1];
  int literals = 0;
  int words = 0;
  int hashing = 0;
  char *p;
  size_t len;

  /* Chop ARGV[0] up into patterns to match against the words.
     We don't need to preserve it because our caller frees all the
     argument memory anyway.  */

  pattail = &pathead;
  while ((p = find_next_token (&pat_iterator, &len)) != 0)
    {
      struct a_pattern *pat = alloca (sizeof (struct a_pattern));

      *pattail = pat;
      pattail = &pat->next;

      if (*pat_iterator != '\0')
        ++pat_iterator;

      pat->str = p;
      p[len] = '\0';
      pat->percent = find_percent (p);
      if (pat->percent == 0)
        literals++;

      /* find_percent() might shorten the string so LEN is wrong.  */
      pat->length = strlen (pat->str);
    }
  *pattail = 0;

  /* Chop ARGV[1] up into words to match against the patterns.  */

  wordtail = &wordhead;
  while ((p = find_next_token (&word_iterator, &len)) != 0)
    {
      struct a_word *word = alloca (sizeof (struct a_word));

      *wordtail = word;
      wordtail = &word->next;

      if (*word_iterator != '\0')
        ++word_iterator;

      p[len] = '\0';
      word->str = p;
      word->length = len;
      word->matched = 0;
      word->chain = 0;
      words++;
    }
  *wordtail = 0;

  /* Only use a hash table if arg list lengths justifies the cost.  */
  hashing = (literals >= 2 && (literals * words) >= 10);
  if (hashing)
    {
      hash_init (&a_word_table, words, a_word_hash_1, a_word_hash_2,
                 a_word_hash_cmp);
      for (wp = wordhead; wp != 0; wp = wp->next)
        {
          struct a_word *owp = hash_insert (&a_word_table, wp);
          if (owp)
            wp->chain = owp;
        }
    }

  if (words)
    {
      int doneany = 0;

      /* Run each pattern through the words, killing words.  */
      for (pp = pathead; pp != 0; pp = pp->next)
        {
          if (pp->percent)
            for (wp = wordhead; wp != 0; wp = wp->next)
              wp->matched |= pattern_matches (pp->str, pp->percent, wp->str);
          else if (hashing)
            {
              struct a_word a_word_key;
              a_word_key.str = pp->str;
              a_word_key.length = pp->length;
              wp = hash_find_item (&a_word_table, &a_word_key);
              while (wp)
                {
                  wp->matched |= 1;
                  wp = wp->chain;
                }
            }
          else
            for (wp = wordhead; wp != 0; wp = wp->next)
              wp->matched |= (wp->length == pp->length
                              && strneq (pp->str, wp->str, wp->length));
        }

      /* Output the words that matched (or didn't, for filter-out).  */
      for (wp = wordhead; wp != 0; wp = wp->next)
        if (is_filter ? wp->matched : !wp->matched)
          {
            o = variable_buffer_output (o, wp->str, strlen (wp->str));
            o = variable_buffer_output (o, " ", 1);
            doneany = 1;
          }

      if (doneany)
        /* Kill the last space.  */
        --o;
    }

  if (hashing)
    hash_free (&a_word_table, 0);

  return o;
}


static char *
func_strip (char *o, char **argv, const char *funcname UNUSED)
{
  const char *p = argv[0];
  int doneany = 0;

  while (*p != '\0')
    {
      int i=0;
      const char *word_start;

      NEXT_TOKEN (p);
      word_start = p;
      for (i=0; *p != '\0' && !ISSPACE (*p); ++p, ++i)
        {}
      if (!i)
        break;
      o = variable_buffer_output (o, word_start, i);
      o = variable_buffer_output (o, " ", 1);
      doneany = 1;
    }

  if (doneany)
    /* Kill the last space.  */
    --o;

  return o;
}

/*
  Print a warning or fatal message.
*/
static char *
func_error (char *o, char **argv, const char *funcname)
{
  char **argvp;
  char *msg, *p;
  size_t len;

  /* The arguments will be broken on commas.  Rather than create yet
     another special case where function arguments aren't broken up,
     just create a format string that puts them back together.  */
  for (len=0, argvp=argv; *argvp != 0; ++argvp)
    len += strlen (*argvp) + 2;

  p = msg = alloca (len + 1);
  msg[0] = '\0';

  for (argvp=argv; argvp[1] != 0; ++argvp)
    {
      strcpy (p, *argvp);
      p += strlen (*argvp);
      *(p++) = ',';
      *(p++) = ' ';
    }
  strcpy (p, *argvp);

  switch (*funcname)
    {
    case 'e':
      OS (fatal, reading_file, "%s", msg);

    case 'w':
      OS (error, reading_file, "%s", msg);
      break;

    case 'i':
      outputs (0, msg);
      outputs (0, "\n");
      break;

    default:
      OS (fatal, *expanding_var, "Internal error: func_error: '%s'", funcname);
    }

  /* The warning function expands to the empty string.  */
  return o;
}


/*
  chop argv[0] into words, and sort them.
 */
static char *
func_sort (char *o, char **argv, const char *funcname UNUSED)
{
  const char *t;
  char **words;
  int wordi;
  char *p;
  size_t len;

  /* Find the maximum number of words we'll have.  */
  t = argv[0];
  wordi = 0;
  while ((p = find_next_token (&t, NULL)) != 0)
    {
      ++t;
      ++wordi;
    }

  words = xmalloc ((wordi == 0 ? 1 : wordi) * sizeof (char *));

  /* Now assign pointers to each string in the array.  */
  t = argv[0];
  wordi = 0;
  while ((p = find_next_token (&t, &len)) != 0)
    {
      ++t;
      p[len] = '\0';
      words[wordi++] = p;
    }

  if (wordi)
    {
      int i;

      /* Now sort the list of words.  */
      qsort (words, wordi, sizeof (char *), alpha_compare);

      /* Now write the sorted list, uniquified.  */
      for (i = 0; i < wordi; ++i)
        {
          len = strlen (words[i]);
          if (i == wordi - 1 || strlen (words[i + 1]) != len
              || strcmp (words[i], words[i + 1]))
            {
              o = variable_buffer_output (o, words[i], len);
              o = variable_buffer_output (o, " ", 1);
            }
        }

      /* Kill the last space.  */
      --o;
    }

  free (words);

  return o;
}

/*
  chop argv[0] into words, and remove duplicates.
 */
static char *
func_uniq (char *o, char **argv, const char *funcname UNUSED)
{
  char *p;
  size_t len;
  int mutated;
  bool once = false;
  const char *s = argv[0];
  struct critbit0 t = {0};

  while ((p = find_next_token (&s, &len)))
    {
      ++s;
      p[len] = 0;
      RETURN_ON_ERROR ((mutated = critbit0_insert (&t, p)));
      if (mutated)
        {
          if (once)
            o = variable_buffer_output (o, " ", 1);
          else
            once = true;
          o = variable_buffer_output (o, p, len);
        }
    }

  critbit0_clear (&t);

  return o;

OnError:
  critbit0_clear (&t);
  OSS (error, NILF, "%s: function failed: %s",
       "uniq", strerror (errno));
  exit (1);
}

/*
  $(if condition,true-part[,false-part])

  CONDITION is false iff it evaluates to an empty string.  White
  space before and after condition are stripped before evaluation.

  If CONDITION is true, then TRUE-PART is evaluated, otherwise FALSE-PART is
  evaluated (if it exists).  Because only one of the two PARTs is evaluated,
  you can use $(if ...) to create side-effects (with $(shell ...), for
  example).
*/

static char *
func_if (char *o, char **argv, const char *funcname UNUSED)
{
  const char *begp = argv[0];
  const char *endp = begp + strlen (argv[0]) - 1;
  int result = 0;

  /* Find the result of the condition: if we have a value, and it's not
     empty, the condition is true.  If we don't have a value, or it's the
     empty string, then it's false.  */

  strip_whitespace (&begp, &endp);

  if (begp <= endp)
    {
      char *expansion = expand_argument (begp, endp+1);

      result = expansion[0] != '\0';
      free (expansion);
    }

  /* If the result is true (1) we want to eval the first argument, and if
     it's false (0) we want to eval the second.  If the argument doesn't
     exist we do nothing, otherwise expand it and add to the buffer.  */

  argv += 1 + !result;

  if (*argv)
    {
      char *expansion = expand_argument (*argv, NULL);

      o = variable_buffer_output (o, expansion, strlen (expansion));

      free (expansion);
    }

  return o;
}

/*
  $(or condition1[,condition2[,condition3[...]]])

  A CONDITION is false iff it evaluates to an empty string.  White
  space before and after CONDITION are stripped before evaluation.

  CONDITION1 is evaluated.  If it's true, then this is the result of
  expansion.  If it's false, CONDITION2 is evaluated, and so on.  If none of
  the conditions are true, the expansion is the empty string.

  Once a CONDITION is true no further conditions are evaluated
  (short-circuiting).
*/

static char *
func_or (char *o, char **argv, const char *funcname UNUSED)
{
  for ( ; *argv ; ++argv)
    {
      const char *begp = *argv;
      const char *endp = begp + strlen (*argv) - 1;
      char *expansion;
      size_t result = 0;

      /* Find the result of the condition: if it's false keep going.  */

      strip_whitespace (&begp, &endp);

      if (begp > endp)
        continue;

      expansion = expand_argument (begp, endp+1);
      result = strlen (expansion);

      /* If the result is false keep going.  */
      if (!result)
        {
          free (expansion);
          continue;
        }

      /* It's true!  Keep this result and return.  */
      o = variable_buffer_output (o, expansion, result);
      free (expansion);
      break;
    }

  return o;
}

/*
  $(and condition1[,condition2[,condition3[...]]])

  A CONDITION is false iff it evaluates to an empty string.  White
  space before and after CONDITION are stripped before evaluation.

  CONDITION1 is evaluated.  If it's false, then this is the result of
  expansion.  If it's true, CONDITION2 is evaluated, and so on.  If all of
  the conditions are true, the expansion is the result of the last condition.

  Once a CONDITION is false no further conditions are evaluated
  (short-circuiting).
*/

static char *
func_and (char *o, char **argv, const char *funcname UNUSED)
{
  char *expansion;

  while (1)
    {
      const char *begp = *argv;
      const char *endp = begp + strlen (*argv) - 1;
      size_t result;

      /* An empty condition is always false.  */
      strip_whitespace (&begp, &endp);
      if (begp > endp)
        return o;

      expansion = expand_argument (begp, endp+1);
      result = strlen (expansion);

      /* If the result is false, stop here: we're done.  */
      if (!result)
        break;

      /* Otherwise the result is true.  If this is the last one, keep this
         result and quit.  Otherwise go on to the next one!  */

      if (*(++argv))
        free (expansion);
      else
        {
          o = variable_buffer_output (o, expansion, result);
          break;
        }
    }

  free (expansion);

  return o;
}

static char *
func_wildcard (char *o, char **argv, const char *funcname UNUSED)
{
   char *p = string_glob (argv[0]);
   o = variable_buffer_output (o, p, strlen (p));
   return o;
}

/*
  $(eval <makefile string>)

  Always resolves to the empty string.

  Treat the arguments as a segment of makefile, and parse them.
*/

static char *
func_eval (char *o, char **argv, const char *funcname UNUSED)
{
  char *buf;
  size_t len;

  /* Eval the buffer.  Pop the current variable buffer setting so that the
     eval'd code can use its own without conflicting.  */

  install_variable_buffer (&buf, &len);

  eval_buffer (argv[0], NULL);

  restore_variable_buffer (buf, len);

  return o;
}


static char *
func_value (char *o, char **argv, const char *funcname UNUSED)
{
  /* Look up the variable.  */
  struct variable *v = lookup_variable (argv[0], strlen (argv[0]));

  /* Copy its value into the output buffer without expanding it.  */
  if (v)
    o = variable_buffer_output (o, v->value, strlen (v->value));

  return o;
}

/*
  \r is replaced on UNIX as well. Is this desirable?
 */
static void
fold_newlines (char *buffer, size_t *length, int trim_newlines)
{
  char *dst = buffer;
  char *src = buffer;
  char *last_nonnl = buffer - 1;
  src[*length] = 0;
  for (; *src != '\0'; ++src)
    {
      if (src[0] == '\r' && src[1] == '\n')
        continue;
      if (*src == '\n')
        {
          *dst++ = ' ';
        }
      else
        {
          last_nonnl = dst;
          *dst++ = *src;
        }
    }

  if (!trim_newlines && (last_nonnl < (dst - 2)))
    last_nonnl = dst - 2;

  *(++last_nonnl) = '\0';
  *length = last_nonnl - buffer;
}

pid_t shell_function_pid = 0;
static int shell_function_completed;

void
shell_completed (int exit_code, int exit_sig)
{
  char buf[256];

  shell_function_pid = 0;
  if (exit_sig == 0 && exit_code == 127)
    shell_function_completed = -1;
  else
    shell_function_completed = 1;

  if (exit_code == 0 && exit_sig > 0)
    exit_code = 128 + exit_sig;

  sprintf (buf, "%d", exit_code);
  define_variable_cname (".SHELLSTATUS", buf, o_override, 0);
}


/*
  Do shell spawning, with the naughty bits for different OSes.
 */

char *
func_shell_base (char *o, char **argv, int trim_newlines)
{
  char *batch_filename = NULL;
  int errfd;
  char **command_argv = NULL;
  char **envp;
  int pipedes[2];
  pid_t pid;

  /* Construct the argument list.  */
  command_argv = construct_command_argv (argv[0], NULL, NULL, 0,
                                         &batch_filename);
  if (command_argv == 0)
    {
      return o;
    }

  /* Using a target environment for 'shell' loses in cases like:
       export var = $(shell echo foobie)
       bad := $(var)
     because target_environment hits a loop trying to expand $(var) to put it
     in the environment.  This is even more confusing when 'var' was not
     explicitly exported, but just appeared in the calling environment.

     See Savannah bug #10593.

  envp = target_environment (NULL);
  */

  envp = environ;

  /* Set up the output in case the shell writes something.  */
  output_start ();

  errfd = (output_context && output_context->err >= 0
           ? output_context->err : FD_STDERR);

  if (pipe (pipedes) < 0)
    {
      OS (error, reading_file, "pipe: %s", strerror (errno));
      pid = -1;
      goto done;
    }

  /* Close handles that are unnecessary for the child process.  */
  fd_noinherit (pipedes[1]);
  fd_noinherit (pipedes[0]);

  {
    struct childbase child;
    child.cmd_name = NULL;
    child.output.syncout = 1;
    child.output.out = pipedes[1];
    child.output.err = errfd;
    child.environment = envp;

    pid = child_execute_job (&child, 1, command_argv, false);

    free (child.cmd_name);
  }

  if (pid < 0)
    {
      shell_completed (127, 0);
      goto done;
    }

  {
    char *buffer;
    size_t maxlen, i;
    int cc;

    /* Record the PID for reap_children.  */
    shell_function_pid = pid;
    shell_function_completed = 0;

    /* Close the write side of the pipe.  We test for -1, since
       pipedes[1] is -1 on MS-Windows, and some versions of MS
       libraries barf when 'close' is called with -1.  */
    if (pipedes[1] >= 0)
      close (pipedes[1]);

    /* Set up and read from the pipe.  */

    maxlen = 200;
    buffer = xmalloc (maxlen + 1);

    /* Read from the pipe until it gets EOF.  */
    for (i = 0; ; i += cc)
      {
        if (i == maxlen)
          {
            maxlen += 512;
            buffer = xrealloc (buffer, maxlen + 1);
          }

        EINTRLOOP (cc, read (pipedes[0], &buffer[i], maxlen - i));
        if (cc <= 0)
          break;
      }
    buffer[i] = '\0';

    /* Close the read side of the pipe.  */
    (void) close (pipedes[0]);

    /* Loop until child_handler or reap_children()  sets
       shell_function_completed to the status of our child shell.  */
    while (shell_function_completed == 0)
      reap_children (1, 0);

    if (batch_filename)
      {
        DB (DB_VERBOSE, (_("Cleaning up temporary batch file %s\n"),
                         batch_filename));
        remove (batch_filename);
        free (batch_filename);
      }
    shell_function_pid = 0;

    /* shell_completed() will set shell_function_completed to 1 when the
       child dies normally, or to -1 if it dies with status 127, which is
       most likely an exec fail.  */

    if (shell_function_completed == -1)
      {
        /* This likely means that the execvp failed, so we should just
           write the error message in the pipe from the child.  */
        fputs (buffer, stderr);
        fflush (stderr);
      }
    else
      {
        /* The child finished normally.  Replace all newlines in its output
           with spaces, and put that in the variable output buffer.  */
        fold_newlines (buffer, &i, trim_newlines);
        o = variable_buffer_output (o, buffer, i);
      }

    free (buffer);
  }

 done:
  if (command_argv)
    {
      /* Free the storage only the child needed.  */
      free (command_argv[0]);
      free (command_argv);
    }

  return o;
}

static char *
func_shell (char *o, char **argv, const char *funcname UNUSED)
{
  return func_shell_base (o, argv, 1);
}

#ifdef EXPERIMENTAL

/*
  equality. Return is string-boolean, i.e., the empty string is false.
 */
static char *
func_eq (char *o, char **argv, char *funcname UNUSED)
{
  int result = ! strcmp (argv[0], argv[1]);
  o = variable_buffer_output (o,  result ? "1" : "", result);
  return o;
}

/*
  string-boolean not operator.
 */
static char *
func_not (char *o, char **argv, char *funcname UNUSED)
{
  const char *s = argv[0];
  int result = 0;
  NEXT_TOKEN (s);
  result = ! (*s);
  o = variable_buffer_output (o,  result ? "1" : "", result);
  return o;
}
#endif


#define IS_ABSOLUTE(n) (n[0] == '/')
#define ROOT_LEN 1

/* Return the absolute name of file NAME which does not contain any '.',
   '..' components nor any repeated path separators ('/').   */

static char *
abspath (const char *name, char *apath)
{
  char *dest;
  const char *start, *end, *apath_limit;
  unsigned long root_len = ROOT_LEN;

  if (name[0] == '\0')
    return NULL;

  apath_limit = apath + GET_PATH_MAX;

  if (!IS_ABSOLUTE(name))
    {
      /* It is unlikely we would make it until here but just to make sure. */
      if (!starting_directory)
        return NULL;

      strcpy (apath, starting_directory);
      dest = strchr (apath, '\0');
    }
  else
    {
      memcpy (apath, name, root_len);
      apath[root_len] = '\0';
      dest = apath + root_len;
      /* Get past the root, since we already copied it.  */
      name += root_len;
    }

  for (start = end = name; *start != '\0'; start = end)
    {
      size_t len;

      /* Skip sequence of multiple path-separators.  */
      while (STOP_SET (*start, MAP_DIRSEP))
        ++start;

      /* Find end of path component.  */
      for (end = start; ! STOP_SET (*end, MAP_DIRSEP|MAP_NUL); ++end)
        ;

      len = end - start;

      if (len == 0)
        break;
      else if (len == 1 && start[0] == '.')
        /* nothing */;
      else if (len == 2 && start[0] == '.' && start[1] == '.')
        {
          /* Back up to previous component, ignore if at root already.  */
          if (dest > apath + root_len)
            for (--dest; ! STOP_SET (dest[-1], MAP_DIRSEP); --dest)
              ;
        }
      else
        {
          if (! STOP_SET (dest[-1], MAP_DIRSEP))
            *dest++ = '/';

          if (dest + len >= apath_limit)
            return NULL;

          dest = memcpy (dest, start, len);
          dest += len;
          *dest = '\0';
        }
    }

  /* Unless it is root strip trailing separator.  */
  if (dest > apath + root_len && STOP_SET (dest[-1], MAP_DIRSEP))
    --dest;

  *dest = '\0';

  return apath;
}


static char *
func_realpath (char *o, char **argv, const char *funcname UNUSED)
{
  /* Expand the argument.  */
  const char *p = argv[0];
  const char *path = 0;
  int doneany = 0;
  size_t len = 0;

  while ((path = find_next_token (&p, &len)) != 0)
    {
      if (len < GET_PATH_MAX)
        {
          char *rp;
          struct stat st;
          PATH_VAR (in);
          PATH_VAR (out);

          strncpy (in, path, len);
          in[len] = '\0';

#ifdef HAVE_REALPATH
          ENULLLOOP (rp, realpath (in, out));
# if defined _AIX
          /* AIX realpath() doesn't remove trailing slashes correctly.  */
          if (rp)
            {
              char *ep = rp + strlen (rp) - 1;
              while (ep > rp && ep[0] == '/')
                *(ep--) = '\0';
            }
# endif
#else
          rp = abspath (in, out);
#endif

          if (rp)
            {
              int r;
              EINTRLOOP (r, stat (out, &st));
              if (r == 0)
                {
                  o = variable_buffer_output (o, out, strlen (out));
                  o = variable_buffer_output (o, " ", 1);
                  doneany = 1;
                }
            }
        }
    }

  /* Kill last space.  */
  if (doneany)
    --o;

  return o;
}

static char *
func_file (char *o, char **argv, const char *funcname UNUSED)
{
  char *fn = argv[0];

  if (fn[0] == '>')
    {
      FILE *fp;
      const char *mode = "w";

      /* We are writing a file.  */
      ++fn;
      if (fn[0] == '>')
        {
          mode = "a";
          ++fn;
        }
      NEXT_TOKEN (fn);

      if (fn[0] == '\0')
        O (fatal, *expanding_var, _("file: missing filename"));

      ENULLLOOP (fp, fopen (fn, mode));
      if (fp == NULL)
        OSS (fatal, reading_file, _("open: %s: %s"), fn, strerror (errno));

      if (argv[1])
        {
          size_t l = strlen (argv[1]);
          int nl = l == 0 || argv[1][l-1] != '\n';

          if (fputs (argv[1], fp) == EOF || (nl && fputc ('\n', fp) == EOF))
            OSS (fatal, reading_file, _("write: %s: %s"), fn, strerror (errno));
        }
      if (fclose (fp))
        OSS (fatal, reading_file, _("close: %s: %s"), fn, strerror (errno));
    }
  else if (fn[0] == '<')
    {
      char *preo = o;
      FILE *fp;

      ++fn;
      NEXT_TOKEN (fn);
      if (fn[0] == '\0')
        O (fatal, *expanding_var, _("file: missing filename"));

      if (argv[1])
        O (fatal, *expanding_var, _("file: too many arguments"));

      ENULLLOOP (fp, fopen (fn, "r"));
      if (fp == NULL)
        {
          if (errno == ENOENT)
            return o;
          OSS (fatal, reading_file, _("open: %s: %s"), fn, strerror (errno));
        }

      while (1)
        {
          char buf[1024];
          size_t l = fread (buf, 1, sizeof (buf), fp);
          if (l > 0)
            o = variable_buffer_output (o, buf, l);

          if (ferror (fp))
            if (errno != EINTR)
              OSS (fatal, reading_file, _("read: %s: %s"), fn, strerror (errno));
          if (feof (fp))
            break;
        }
      if (fclose (fp))
        OSS (fatal, reading_file, _("close: %s: %s"), fn, strerror (errno));

      /* Remove trailing newline.  */
      if (o > preo && o[-1] == '\n')
        if (--o > preo && o[-1] == '\r')
          --o;
    }
  else
    OS (fatal, *expanding_var, _("file: invalid file operation: %s"), fn);

  return o;
}

static char *
func_abspath (char *o, char **argv, const char *funcname UNUSED)
{
  /* Expand the argument.  */
  const char *p = argv[0];
  const char *path = 0;
  int doneany = 0;
  size_t len = 0;

  while ((path = find_next_token (&p, &len)) != 0)
    {
      if (len < GET_PATH_MAX)
        {
          PATH_VAR (in);
          PATH_VAR (out);

          strncpy (in, path, len);
          in[len] = '\0';

          if (abspath (in, out))
            {
              o = variable_buffer_output (o, out, strlen (out));
              o = variable_buffer_output (o, " ", 1);
              doneany = 1;
            }
        }
    }

  /* Kill last space.  */
  if (doneany)
    --o;

  return o;
}

/* Lookup table for builtin functions.

   This doesn't have to be sorted; we use a straight lookup.  We might gain
   some efficiency by moving most often used functions to the start of the
   table.

   If MAXIMUM_ARGS is 0, that means there is no maximum and all
   comma-separated values are treated as arguments.

   EXPAND_ARGS means that all arguments should be expanded before invocation.
   Functions that do namespace tricks (foreach) don't automatically expand.  */

static char *func_call (char *o, char **argv, const char *funcname);

#define FT_ENTRY(_name, _min, _max, _exp, _func) \
  { { (_func) }, STRING_SIZE_TUPLE(_name), (_min), (_max), (_exp), 0 }

static struct function_table_entry function_table_init[] =
{
 /*         Name            MIN MAX EXP? Function */
  FT_ENTRY ("abspath",       0,  1,  1,  func_abspath),
  FT_ENTRY ("addprefix",     2,  2,  1,  func_addsuffix_addprefix),
  FT_ENTRY ("addsuffix",     2,  2,  1,  func_addsuffix_addprefix),
  FT_ENTRY ("basename",      0,  1,  1,  func_basename_dir),
  FT_ENTRY ("dir",           0,  1,  1,  func_basename_dir),
  FT_ENTRY ("notdir",        0,  1,  1,  func_notdir_suffix),
  FT_ENTRY ("subst",         3,  3,  1,  func_subst),
  FT_ENTRY ("suffix",        0,  1,  1,  func_notdir_suffix),
  FT_ENTRY ("filter",        2,  2,  1,  func_filter_filterout),
  FT_ENTRY ("filter-out",    2,  2,  1,  func_filter_filterout),
  FT_ENTRY ("findstring",    2,  2,  1,  func_findstring),
  FT_ENTRY ("firstword",     0,  1,  1,  func_firstword),
  FT_ENTRY ("flavor",        0,  1,  1,  func_flavor),
  FT_ENTRY ("join",          2,  2,  1,  func_join),
  FT_ENTRY ("lastword",      0,  1,  1,  func_lastword),
  FT_ENTRY ("patsubst",      3,  3,  1,  func_patsubst),
  FT_ENTRY ("realpath",      0,  1,  1,  func_realpath),
  FT_ENTRY ("shell",         0,  1,  1,  func_shell),
  FT_ENTRY ("sort",          0,  1,  1,  func_sort),
  FT_ENTRY ("strip",         0,  1,  1,  func_strip),
  FT_ENTRY ("wildcard",      0,  1,  1,  func_wildcard),
  FT_ENTRY ("word",          2,  2,  1,  func_word),
  FT_ENTRY ("wordlist",      3,  3,  1,  func_wordlist),
  FT_ENTRY ("words",         0,  1,  1,  func_words),
  FT_ENTRY ("origin",        0,  1,  1,  func_origin),
  FT_ENTRY ("foreach",       3,  3,  0,  func_foreach),
  FT_ENTRY ("call",          1,  0,  1,  func_call),
  FT_ENTRY ("info",          0,  1,  1,  func_error),
  FT_ENTRY ("error",         0,  1,  1,  func_error),
  FT_ENTRY ("warning",       0,  1,  1,  func_error),
  FT_ENTRY ("if",            2,  3,  0,  func_if),
  FT_ENTRY ("or",            1,  0,  0,  func_or),
  FT_ENTRY ("and",           1,  0,  0,  func_and),
  FT_ENTRY ("value",         0,  1,  1,  func_value),
  FT_ENTRY ("eval",          0,  1,  1,  func_eval),
  FT_ENTRY ("file",          1,  2,  1,  func_file),
  FT_ENTRY ("uniq",          0,  1,  1,  func_uniq),
#ifdef EXPERIMENTAL
  FT_ENTRY ("eq",            2,  2,  1,  func_eq),
  FT_ENTRY ("not",           0,  1,  1,  func_not),
#endif
};

#define FUNCTION_TABLE_ENTRIES (sizeof (function_table_init) / sizeof (struct function_table_entry))


/* These must come after the definition of function_table.  */

static char *
expand_builtin_function (char *o, int argc, char **argv,
                         const struct function_table_entry *entry_p)
{
  char *p;

  if (argc < (int)entry_p->minimum_args)
    fatal (*expanding_var, strlen (entry_p->name),
           _("insufficient number of arguments (%d) to function '%s'"),
           argc, entry_p->name);

  /* I suppose technically some function could do something with no arguments,
     but so far no internal ones do, so just test it for all functions here
     rather than in each one.  We can change it later if necessary.  */

  if (!argc && !entry_p->alloc_fn)
    return o;

  if (!entry_p->fptr.func_ptr)
    OS (fatal, *expanding_var,
        _("unimplemented on this platform: function '%s'"), entry_p->name);

  if (!entry_p->alloc_fn)
    return entry_p->fptr.func_ptr (o, argv, entry_p->name);

  /* This function allocates memory and returns it to us.
     Write it to the variable buffer, then free it.  */

  p = entry_p->fptr.alloc_func_ptr (entry_p->name, argc, argv);
  if (p)
    {
      o = variable_buffer_output (o, p, strlen (p));
      free (p);
    }

  return o;
}

/* Check for a function invocation in *STRINGP.  *STRINGP points at the
   opening ( or { and is not null-terminated.  If a function invocation
   is found, expand it into the buffer at *OP, updating *OP, incrementing
   *STRINGP past the reference and returning nonzero.  If not, return zero.  */

int
handle_function (char **op, const char **stringp)
{
  const struct function_table_entry *entry_p;
  char openparen = (*stringp)[0];
  char closeparen = openparen == '(' ? ')' : '}';
  const char *beg;
  const char *end;
  int count = 0;
  char *abeg = NULL;
  char **argv, **argvp;
  int nargs;

  beg = *stringp + 1;

  entry_p = lookup_function (beg);

  if (!entry_p)
    return 0;

  /* We found a builtin function.  Find the beginning of its arguments (skip
     whitespace after the name).  */

  beg += entry_p->len;
  NEXT_TOKEN (beg);

  /* Find the end of the function invocation, counting nested use of
     whichever kind of parens we use.  Since we're looking, count commas
     to get a rough estimate of how many arguments we might have.  The
     count might be high, but it'll never be low.  */

  for (nargs=1, end=beg; *end != '\0'; ++end)
    if (!STOP_SET (*end, MAP_VARSEP|MAP_COMMA))
      continue;
    else if (*end == ',')
      ++nargs;
    else if (*end == openparen)
      ++count;
    else if (*end == closeparen && --count < 0)
      break;

  if (count >= 0)
    fatal (*expanding_var, strlen (entry_p->name),
           _("unterminated call to function '%s': missing '%c'"),
           entry_p->name, closeparen);

  *stringp = end;

  /* Get some memory to store the arg pointers.  */
  argvp = argv = alloca (sizeof (char *) * (nargs + 2));

  /* Chop the string into arguments, then a nul.  As soon as we hit
     MAXIMUM_ARGS (if it's >0) assume the rest of the string is part of the
     last argument.

     If we're expanding, store pointers to the expansion of each one.  If
     not, make a duplicate of the string and point into that, nul-terminating
     each argument.  */

  if (entry_p->expand_args)
    {
      const char *p;
      for (p=beg, nargs=0; p <= end; ++argvp)
        {
          const char *next;

          ++nargs;

          if (nargs == entry_p->maximum_args
              || ((next = find_next_argument (openparen, closeparen, p, end)) == NULL))
            next = end;

          *argvp = expand_argument (p, next);
          p = next + 1;
        }
    }
  else
    {
      size_t len = end - beg;
      char *p, *aend;

      abeg = xmalloc (len+1);
      memcpy (abeg, beg, len);
      abeg[len] = '\0';
      aend = abeg + len;

      for (p=abeg, nargs=0; p <= aend; ++argvp)
        {
          char *next;

          ++nargs;

          if (nargs == entry_p->maximum_args
              || ((next = find_next_argument (openparen, closeparen, p, aend)) == NULL))
            next = aend;

          *argvp = p;
          *next = '\0';
          p = next + 1;
        }
    }
  *argvp = NULL;

  /* Finally!  Run the function...  */
  *op = expand_builtin_function (*op, nargs, argv, entry_p);

  /* Free memory.  */
  if (entry_p->expand_args)
    for (argvp=argv; *argvp != 0; ++argvp)
      free (*argvp);
  else
    free (abeg);

  return 1;
}


/* User-defined functions.  Expand the first argument as either a builtin
   function or a make variable, in the context of the rest of the arguments
   assigned to $1, $2, ... $N.  $0 is the name of the function.  */

static char *
func_call (char *o, char **argv, const char *funcname UNUSED)
{
  static int max_args = 0;
  char *fname;
  char *body;
  size_t flen;
  int i;
  int saved_args;
  const struct function_table_entry *entry_p;
  struct variable *v;

  /* Clean up the name of the variable to be invoked.  */
  fname = next_token (argv[0]);
  end_of_token (fname)[0] = '\0';

  /* Calling nothing is a no-op */
  if (*fname == '\0')
    return o;

  /* Are we invoking a builtin function?  */

  entry_p = lookup_function (fname);
  if (entry_p)
    {
      /* How many arguments do we have?  */
      for (i=0; argv[i+1]; ++i)
        ;
      return expand_builtin_function (o, i, argv+1, entry_p);
    }

  /* Not a builtin, so the first argument is the name of a variable to be
     expanded and interpreted as a function.  Find it.  */
  flen = strlen (fname);

  v = lookup_variable (fname, flen);

  if (v == 0)
    warn_undefined (fname, flen);

  if (v == 0 || *v->value == '\0')
    return o;

  body = alloca (flen + 4);
  body[0] = '$';
  body[1] = '(';
  memcpy (body + 2, fname, flen);
  body[flen+2] = ')';
  body[flen+3] = '\0';

  /* Set up arguments $(1) .. $(N).  $(0) is the function name.  */

  push_new_variable_scope ();

  for (i=0; *argv; ++i, ++argv)
    {
      char num[11];

      sprintf (num, "%d", i);
      define_variable (num, strlen (num), *argv, o_automatic, 0);
    }

  /* If the number of arguments we have is < max_args, it means we're inside
     a recursive invocation of $(call ...).  Fill in the remaining arguments
     in the new scope with the empty value, to hide them from this
     invocation.  */

  for (; i < max_args; ++i)
    {
      char num[11];

      sprintf (num, "%d", i);
      define_variable (num, strlen (num), "", o_automatic, 0);
    }

  /* Expand the body in the context of the arguments, adding the result to
     the variable buffer.  */

  v->exp_count = EXP_COUNT_MAX;

  saved_args = max_args;
  max_args = i;
  o = variable_expand_string (o, body, flen+3);
  max_args = saved_args;

  v->exp_count = 0;

  pop_variable_scope ();

  return o + strlen (o);
}

void
define_new_function (const floc *flocp, const char *name,
                     unsigned int min, unsigned int max, unsigned int flags,
                     gmk_func_ptr func)
{
  const char *e = name;
  struct function_table_entry *ent;
  size_t len;

  while (STOP_SET (*e, MAP_USERFUNC))
    e++;
  len = e - name;

  if (len == 0)
    O (fatal, flocp, _("Empty function name"));
  if (*name == '.' || *e != '\0')
    OS (fatal, flocp, _("Invalid function name: %s"), name);
  if (len > 255)
    OS (fatal, flocp, _("Function name too long: %s"), name);
  if (min > 255)
    ONS (fatal, flocp,
         _("Invalid minimum argument count (%u) for function %s"), min, name);
  if (max > 255 || (max && max < min))
    ONS (fatal, flocp,
         _("Invalid maximum argument count (%u) for function %s"), max, name);

  ent = xmalloc (sizeof (struct function_table_entry));
  ent->name = name;
  ent->len = (unsigned char) len;
  ent->minimum_args = (unsigned char) min;
  ent->maximum_args = (unsigned char) max;
  ent->expand_args = ANY_SET(flags, GMK_FUNC_NOEXPAND) ? 0 : 1;
  ent->alloc_fn = 1;
  ent->fptr.alloc_func_ptr = func;

  hash_insert (&function_table, ent);
}

void
hash_init_function_table (void)
{
  hash_init (&function_table, FUNCTION_TABLE_ENTRIES * 2,
             function_table_entry_hash_1, function_table_entry_hash_2,
             function_table_entry_hash_cmp);
  hash_load (&function_table, function_table_init,
             FUNCTION_TABLE_ENTRIES, sizeof (struct function_table_entry));
}
