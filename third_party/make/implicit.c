/* Implicit rule searching for GNU Make.
Copyright (C) 1988-2023 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "makeint.h"
#include "filedef.h"
#include "rule.h"
#include "dep.h"
#include "debug.h"
#include "variable.h"
#include "job.h"      /* struct child, used inside commands.h */
#include "commands.h" /* set_file_variables */
#include "shuffle.h"
#include <assert.h>

static int pattern_search (struct file *file, int archive,
                           unsigned int depth, unsigned int recursions,
                           int allow_compat_rules);

/* For a FILE which has no commands specified, try to figure out some
   from the implicit pattern rules.
   Returns 1 if a suitable implicit rule was found,
   after modifying FILE to contain the appropriate commands and deps,
   or returns 0 if no implicit rule was found.  */

int
try_implicit_rule (struct file *file, unsigned int depth)
{
  DBF (DB_IMPLICIT, _("Looking for an implicit rule for '%s'.\n"));

  /* The order of these searches was previously reversed.  My logic now is
     that since the non-archive search uses more information in the target
     (the archive search omits the archive name), it is more specific and
     should come first.  */

  if (pattern_search (file, 0, depth, 0, 0))
    return 1;

#ifndef NO_ARCHIVES
  /* If this is an archive member reference, use just the
     archive member name to search for implicit rules.  */
  if (ar_name (file->name))
    {
      DBF (DB_IMPLICIT,
           _("Looking for archive-member implicit rule for '%s'.\n"));
      if (pattern_search (file, 1, depth, 0, 0))
        return 1;
      DBS (DB_IMPLICIT,
           (_("No archive-member implicit rule found for '%s'.\n"),
            file->name));
    }
#endif

  return 0;
}


/* Scans the BUFFER for the next word with whitespace as a separator.
   Returns the pointer to the beginning of the word. LENGTH hold the
   length of the word.  */

static const char *
get_next_word (const char *buffer, size_t *length)
{
  const char *p = buffer, *beg;
  char c;

  /* Skip any leading whitespace.  */
  NEXT_TOKEN (p);

  beg = p;
  c = *(p++);

  if (c == '\0')
    return 0;


  /* We already found the first value of "c", above.  */
  while (1)
    {
      char closeparen;
      int count;

      switch (c)
        {
        case '\0':
        case ' ':
        case '\t':
          goto done_word;

        case '$':
          c = *(p++);
          if (c == '$')
            break;

          /* This is a variable reference, so read it to the matching
             close paren.  */

          if (c == '(')
            closeparen = ')';
          else if (c == '{')
            closeparen = '}';
          else
            /* This is a single-letter variable reference.  */
            break;

          for (count = 0; *p != '\0'; ++p)
            {
              if (*p == c)
                ++count;
              else if (*p == closeparen && --count < 0)
                {
                  ++p;
                  break;
                }
            }
          break;

        case '|':
          goto done;

        default:
          break;
        }

      c = *(p++);
    }
 done_word:
  --p;

 done:
  if (length)
    *length = p - beg;

  return beg;
}

/* This structure stores information about the expanded prerequisites for a
   pattern rule.  NAME is always set to the strcache'd name of the prereq.
   FILE and PATTERN will be set for intermediate files only.  IGNORE_MTIME is
   copied from the prerequisite we expanded.
 */
struct patdeps
  {
    const char *name;
    const char *pattern;
    struct file *file;
    unsigned int ignore_mtime : 1;
    unsigned int ignore_automatic_vars : 1;
    unsigned int is_explicit : 1;
    unsigned int wait_here : 1;
  };

/* This structure stores information about pattern rules that we need
   to try.
*/
struct tryrule
  {
    struct rule *rule;

    /* Stem length for this match. */
    size_t stemlen;

    /* Index of the target in this rule that matched the file. */
    unsigned int matches;

    /* Definition order of this rule. Used to implement stable sort.*/
    unsigned int order;

    /* Nonzero if the LASTSLASH logic was used in matching this rule. */
    char checked_lastslash;
  };

int
stemlen_compare (const void *v1, const void *v2)
{
  const struct tryrule *r1 = v1;
  const struct tryrule *r2 = v2;
  int r = (int) (r1->stemlen - r2->stemlen);
  return r != 0 ? r : (int) (r1->order - r2->order);
}

/* Search the pattern rules for a rule with an existing dependency to make
   FILE.  If a rule is found, the appropriate commands and deps are put in FILE
   and 1 is returned.  If not, 0 is returned.

   If ARCHIVE is nonzero, FILE->name is of the form "LIB(MEMBER)".  A rule for
   "(MEMBER)" will be searched for, and "(MEMBER)" will not be chopped up into
   directory and filename parts.

   If an intermediate file is found by pattern search, the intermediate file
   is set up as a target by the recursive call and is also made a dependency
   of FILE.

   DEPTH is used for debugging messages.  */

static int
pattern_search (struct file *file, int archive,
                unsigned int depth, unsigned int recursions,
                int allow_compat_rules)
{
  /* Filename we are searching for a rule for.  */
  const char *filename = archive ? strchr (file->name, '(') : file->name;

  /* Length of FILENAME.  */
  size_t namelen = strlen (filename);

  /* The last slash in FILENAME (or nil if there is none).  */
  const char *lastslash;

  /* This is a file-object used as an argument in
     recursive calls.  It never contains any data
     except during a recursive call.  */
  struct file *int_file = 0;

  /* List of dependencies found recursively.  */
  unsigned int max_deps = max_pattern_deps;
  struct patdeps *deplist = xmalloc (max_deps * sizeof (struct patdeps));
  struct patdeps *pat = deplist;

  /* Names of possible dependencies are constructed in this buffer.
     We may replace % by $(*F) for second expansion, increasing the length.  */
  size_t deplen = namelen + max_pattern_dep_length + 4;
  char *depname = alloca (deplen);
#ifndef NDEBUG
  char *dend = depname + deplen;
#endif

  /* The start and length of the stem of FILENAME for the current rule.  */
  const char *stem = 0;
  size_t stemlen = 0;
  size_t fullstemlen = 0;

  /* Buffer in which we store all the rules that are possibly applicable.  */
  struct tryrule *tryrules = xmalloc (num_pattern_rules * max_pattern_targets
                                      * sizeof (struct tryrule));

  /* Number of valid elements in TRYRULES.  */
  unsigned int nrules;

  /* The index in TRYRULES of the rule we found.  */
  unsigned int foundrule;

  /* Nonzero if should consider intermediate files as dependencies.  */
  int intermed_ok;

  /* Nonzero if we have initialized file variables for this target.  */
  int file_vars_initialized = 0;

  /* Nonzero if we have matched a pattern-rule target
     that is not just '%'.  */
  int specific_rule_matched = 0;

  unsigned int ri;  /* uninit checks OK */
  int found_compat_rule = 0;
  struct rule *rule;

  char *pathdir = NULL;
  size_t pathlen;

  PATH_VAR (stem_str); /* @@ Need to get rid of stem, stemlen, etc. */

  ++depth;

#ifndef NO_ARCHIVES
  if (archive || ar_name (filename))
    lastslash = 0;
  else
#endif
    {
      /* Set LASTSLASH to point at the last slash in FILENAME
         but not counting any slash at the end.  (foo/bar/ counts as
         bar/ in directory foo/, not empty in directory foo/bar/.)  */
      lastslash = memrchr (filename, '/', namelen - 1);
#ifdef VMS
      if (lastslash == NULL)
        lastslash = strrchr (filename, ']');
      if (lastslash == NULL)
        lastslash = strrchr (filename, '>');
      if (lastslash == NULL)
        lastslash = strrchr (filename, ':');
#endif
#ifdef HAVE_DOS_PATHS
      /* Handle backslashes (possibly mixed with forward slashes)
         and the case of "d:file".  */
      {
        char *bslash = memrchr (filename, '\\', namelen - 1);
        if (lastslash == 0 || bslash > lastslash)
          lastslash = bslash;
        if (lastslash == 0 && filename[0] && filename[1] == ':')
          lastslash = filename + 1;
      }
#endif
    }

  pathlen = lastslash ? lastslash - filename + 1 : 0;

  /* First see which pattern rules match this target and may be considered.
     Put them in TRYRULES.  */

  nrules = 0;
  for (rule = pattern_rules; rule != 0; rule = rule->next)
    {
      unsigned int ti;

      /* If the pattern rule has deps but no commands, ignore it.
         Users cancel built-in rules by redefining them without commands.  */
      if (rule->deps != 0 && rule->cmds == 0)
        continue;

      /* If this rule is in use by a parent pattern_search,
         don't use it here.  */
      if (rule->in_use)
        {
          DBS (DB_IMPLICIT,
               (_("Avoiding implicit rule recursion for rule '%s'.\n"),
                get_rule_defn (rule)));
          continue;
        }

      for (ti = 0; ti < rule->num; ++ti)
        {
          const char *target = rule->targets[ti];
          const char *suffix = rule->suffixes[ti];
          char check_lastslash;

          /* Rules that can match any filename and are not terminal
             are ignored if we're recursing, so that they cannot be
             intermediate files.  */
          if (recursions > 0 && target[1] == '\0' && !rule->terminal)
            continue;

          if (rule->lens[ti] > namelen)
            /* It can't possibly match.  */
            continue;

          /* From the lengths of the filename and the pattern parts,
             find the stem: the part of the filename that matches the %.  */
          stem = filename + (suffix - target - 1);
          stemlen = namelen - rule->lens[ti] + 1;

          /* Set CHECK_LASTSLASH if FILENAME contains a directory
             prefix and the target pattern does not contain a slash.  */

          check_lastslash = 0;
          if (lastslash)
            {
#ifdef VMS
              check_lastslash = strpbrk (target, "/]>:") == NULL;
#else
              check_lastslash = strchr (target, '/') == 0;
#endif
#ifdef HAVE_DOS_PATHS
              /* Didn't find it yet: check for DOS-type directories.  */
              if (check_lastslash)
                {
                  char *b = strchr (target, '\\');
                  check_lastslash = !(b || (target[0] && target[1] == ':'));
                }
#endif
            }
          if (check_lastslash)
            {
              /* If so, don't include the directory prefix in STEM here.  */
              if (pathlen > stemlen)
                continue;
              stemlen -= pathlen;
              stem += pathlen;
            }

          /* Check that the rule pattern matches the text before the stem.  */
          if (check_lastslash)
            {
              if (stem > (lastslash + 1)
                  && !strneq (target, lastslash + 1, stem - lastslash - 1))
                continue;
            }
          else if (stem > filename
                   && !strneq (target, filename, stem - filename))
            continue;

          /* Check that the rule pattern matches the text after the stem.
             We could test simply use streq, but this way we compare the
             first two characters immediately.  This saves time in the very
             common case where the first character matches because it is a
             period.  */
          if (*suffix != stem[stemlen]
              || (*suffix != '\0' && !streq (&suffix[1], &stem[stemlen + 1])))
            continue;

          /* Record if we match a rule that not all filenames will match.  */
          if (target[1] != '\0')
            specific_rule_matched = 1;

          /* A rule with no dependencies and no commands exists solely to set
             specific_rule_matched when it matches.  Don't try to use it.  */
          if (rule->deps == 0 && rule->cmds == 0)
            continue;

          /* Record this rule in TRYRULES and the index of the matching
             target in MATCHES.  If several targets of the same rule match,
             that rule will be in TRYRULES more than once.  */
          tryrules[nrules].rule = rule;
          tryrules[nrules].matches = ti;
          tryrules[nrules].stemlen = stemlen + (check_lastslash ? pathlen : 0);
          tryrules[nrules].order = nrules;
          tryrules[nrules].checked_lastslash = check_lastslash;
          ++nrules;
        }
    }

  /* Bail out early if we haven't found any rules. */
  if (nrules == 0)
    goto done;

  /* Sort the rules to place matches with the shortest stem first. This
     way the most specific rules will be tried first. */
  if (nrules > 1)
    qsort (tryrules, nrules, sizeof (struct tryrule), stemlen_compare);

  /* If we have found a matching rule that won't match all filenames,
     retroactively reject any non-"terminal" rules that do always match.  */
  if (specific_rule_matched)
    for (ri = 0; ri < nrules; ++ri)
      if (!tryrules[ri].rule->terminal)
        {
          unsigned int j;
          for (j = 0; j < tryrules[ri].rule->num; ++j)
            if (tryrules[ri].rule->targets[j][1] == '\0')
              {
                tryrules[ri].rule = 0;
                break;
              }
        }

  /* Try each rule once without intermediate files, then once with them.  */
  for (intermed_ok = 0; intermed_ok < 2; ++intermed_ok)
    {
      pat = deplist;
      if (intermed_ok)
        DBS (DB_IMPLICIT, (_("Trying harder.\n")));

      /* Try each pattern rule till we find one that applies.  If it does,
         expand its dependencies (as substituted) and chain them in DEPS.  */
      for (ri = 0; ri < nrules; ri++)
        {
          struct dep *dep;
          char check_lastslash;
          unsigned int failed = 0;
          int file_variables_set = 0;
          unsigned int deps_found = 0;
          /* NPTR points to the part of the prereq we haven't processed.  */
          const char *nptr = 0;
          int order_only = 0;
          unsigned int matches;

          rule = tryrules[ri].rule;

          /* RULE is nil when we discover that a rule, already placed in
             TRYRULES, should not be applied.  */
          if (rule == 0)
            continue;

          /* Reject any terminal rules if we're looking to make intermediate
             files.  */
          if (intermed_ok && rule->terminal)
            continue;

          /* From the lengths of the filename and the matching pattern parts,
             find the stem: the part of the filename that matches the %.  */
          matches = tryrules[ri].matches;
          stem = filename + (rule->suffixes[matches]
                             - rule->targets[matches]) - 1;
          stemlen = (namelen - rule->lens[matches]) + 1;
          check_lastslash = tryrules[ri].checked_lastslash;
          if (check_lastslash)
            {
              stem += pathlen;
              stemlen -= pathlen;

              /* We need to add the directory prefix, so set it up.  */
              if (! pathdir)
                {
                  pathdir = alloca (pathlen + 1);
                  memcpy (pathdir, filename, pathlen);
                  pathdir[pathlen] = '\0';
                }
            }

          DBS (DB_IMPLICIT,
               (_("Trying pattern rule '%s' with stem '%.*s'.\n"),
                get_rule_defn (rule), (int) stemlen, stem));

          if (stemlen + (check_lastslash ? pathlen : 0) > GET_PATH_MAX)
            {
              DBS (DB_IMPLICIT, (_("Stem too long: '%s%.*s'.\n"),
                                 check_lastslash ? pathdir : "",
                                 (int) stemlen, stem));
              continue;
            }

          if (!check_lastslash)
            {
              memcpy (stem_str, stem, stemlen);
              stem_str[stemlen] = '\0';
            }
          else
            {
              /* We want to prepend the directory from
                 the original FILENAME onto the stem.  */
              memcpy (stem_str, filename, pathlen);
              memcpy (stem_str + pathlen, stem, stemlen);
              stem_str[pathlen + stemlen] = '\0';
            }

          /* If there are no prerequisites, then this rule matches.  */
          if (rule->deps == 0)
            break;

          /* Mark this rule as in use so a recursive pattern_search won't try
             to use it.  */
          rule->in_use = 1;

          /* Try each prerequisite; see if it exists or can be created.  We'll
             build a list of prereq info in DEPLIST.  Due to 2nd expansion we
             may have to process multiple prereqs for a single dep entry.  */

          pat = deplist;
          dep = rule->deps;
          nptr = dep_name (dep);
          while (1)
            {
              struct dep *dl, *d;

              /* If we're out of name to parse, start the next prereq.  */
              if (! nptr)
                {
                  dep = dep->next;
                  if (dep == 0)
                    break;
                  nptr = dep_name (dep);
                }

              /* If we don't need a second expansion, just replace the %.  */
              if (! dep->need_2nd_expansion)
                {
                  char *p;
                  int is_explicit = 1;
                  const char *cp = strchr (nptr, '%');
                  if (cp == 0)
                    strcpy (depname, nptr);
                  else
                    {
                      char *o = depname;
                      if (check_lastslash)
                        o = mempcpy (o, filename, pathlen);
                      o = mempcpy (o, nptr, cp - nptr);
                      o = mempcpy (o, stem, stemlen);
                      strcpy (o, cp + 1);
                      is_explicit = 0;
                    }

                  /* Parse the expanded string.  It might have wildcards.  */
                  p = depname;
                  dl = PARSE_FILE_SEQ (&p, struct dep, MAP_NUL, NULL,
                                       PARSEFS_ONEWORD|PARSEFS_WAIT);
                  for (d = dl; d != NULL; d = d->next)
                    {
                      ++deps_found;
                      d->ignore_mtime = dep->ignore_mtime;
                      d->ignore_automatic_vars = dep->ignore_automatic_vars;
                      d->wait_here |= dep->wait_here;
                      d->is_explicit = is_explicit;
                    }

                  /* We've used up this dep, so next time get a new one.  */
                  nptr = 0;
                }

              /* We have to perform second expansion on this prereq.  In an
                 ideal world we would take the dependency line, substitute the
                 stem, re-expand the whole line and chop it into individual
                 prerequisites.  Unfortunately this won't work because of the
                 "check_lastslash" twist.  Instead, we will have to go word by
                 word, taking $()'s into account.  For each word we will
                 substitute the stem, re-expand, chop it up, and, if
                 check_lastslash != 0, add the directory part to each
                 resulting prerequisite.  */
              else
                {
                  int add_dir = 0;
                  size_t len;
                  const char *end;
                  struct dep **dptr;
                  int is_explicit;
                  const char *cp;
                  char *p;

                  nptr = get_next_word (nptr, &len);
                  if (nptr == 0)
                    continue;
                  end = nptr + len;

                  /* See if this is a transition to order-only prereqs.  */
                  if (! order_only && len == 1 && nptr[0] == '|')
                    {
                      order_only = 1;
                      nptr = end;
                      continue;
                    }

                  /* If the dependency name has %, substitute the stem.  If we
                     just replace % with the stem value then later, when we do
                     the 2nd expansion, we will re-expand this stem value
                     again.  This is not good if you have certain characters
                     in your stem (like $).

                     Instead, we will replace % with $* or $(*F) and allow the
                     second expansion to take care of it for us.  This way
                     (since $* and $(*F) are simple variables) there won't be
                     additional re-expansion of the stem.  */

                  cp = lindex (nptr, end, '%');
                  if (cp == 0)
                    {
                      memcpy (depname, nptr, len);
                      depname[len] = '\0';
                      is_explicit = 1;
                    }
                  else
                    {
                      /* Go through all % between NPTR and END.
                         Copy contents of [NPTR, END) to depname, with the
                         first % after NPTR and then each first % after white
                         space replaced with $* or $(*F).  depname has enough
                         room to substitute each % with $(*F).  */
                      char *o = depname;

                      is_explicit = 0;
                      for (;;)
                        {
                          size_t i = cp - nptr;
                          assert (o + i < dend);
                          o = mempcpy (o, nptr, i);
                          if (check_lastslash)
                            {
                              add_dir = 1;
                              assert (o + 5 < dend);
                              o = mempcpy (o, "$(*F)", 5);
                            }
                          else
                            {
                              assert (o + 2 < dend);
                              o = mempcpy (o, "$*", 2);
                            }
                          assert (o < dend);
                          ++cp;
                          assert (cp <= end);
                          nptr = cp;
                          if (nptr == end)
                            break;

                          /* Skip the rest of this word then find the next %.
                             No need to worry about order-only, or nested
                             functions: NPTR went though get_next_word.  */
                          while (cp < end && ! END_OF_TOKEN (*cp))
                            ++cp;
                          cp = lindex (cp, end, '%');
                          if (cp == 0)
                            break;
                        }
                        len = end - nptr;
                        memcpy (o, nptr, len);
                        o[len] = '\0';
                    }

                  /* Set up for the next word.  */
                  nptr = end;

                  /* Initialize and set file variables if we haven't already
                     done so. */
                  if (!file_vars_initialized)
                    {
                      initialize_file_variables (file, 0);
                      set_file_variables (file, stem_str);
                      file_vars_initialized = 1;
                    }
                  /* Update the stem value in $* for this rule.  */
                  else if (!file_variables_set)
                    {
                      define_variable_for_file (
                        "*", 1, stem_str, o_automatic, 0, file);
                      file_variables_set = 1;
                    }

                  /* Perform the 2nd expansion.  */
                  p = variable_expand_for_file (depname, file);
                  dptr = &dl;

                  /* Parse the results into a deps list.  */
                  do
                    {
                      /* Parse the expanded string. */
                      struct dep *dp = PARSE_FILE_SEQ (&p, struct dep,
                                                       order_only ? MAP_NUL : MAP_PIPE,
                                                       add_dir ? pathdir : NULL,
                                                       PARSEFS_WAIT);
                      *dptr = dp;

                      for (d = dp; d != NULL; d = d->next)
                        {
                          ++deps_found;
                          if (order_only)
                            d->ignore_mtime = 1;
                          d->is_explicit = is_explicit;
                          dptr = &d->next;
                        }

                      /* If we stopped due to an order-only token, note it.  */
                      if (*p == '|')
                        {
                          order_only = 1;
                          ++p;
                        }
                    }
                  while (*p != '\0');
                }

              /* If there are more than max_pattern_deps prerequisites (due to
                 2nd expansion), reset it and realloc the arrays.  */

              if (deps_found > max_deps)
                {
                  size_t l = pat - deplist;
                  /* This might have changed due to recursion.  */
                  max_pattern_deps = MAX(max_pattern_deps, deps_found);
                  max_deps = max_pattern_deps;
                  deplist = xrealloc (deplist,
                                      max_deps * sizeof (struct patdeps));
                  pat = deplist + l;
                }

              /* Go through the nameseq and handle each as a prereq name.  */
              for (d = dl; d != 0; d = d->next)
                {
                  struct file *df;
                  int is_rule = d->name == dep_name (dep);
                  int explicit = 0;
                  struct dep *dp = 0;

                  if (file_impossible_p (d->name))
                    {
                      /* If this prereq has already been ruled "impossible",
                         then the rule fails.  Don't bother trying it on the
                         second pass either since we know that will fail.  */
                      DBS (DB_IMPLICIT,
                           (is_rule
                            ? _("Rejecting rule '%s' due to impossible rule"
                                " prerequisite '%s'.\n")
                            : _("Rejecting rule '%s' due to impossible implicit"
                                " prerequisite '%s'.\n"),
                            get_rule_defn (rule), d->name));
                      tryrules[ri].rule = 0;

                      failed = 1;
                      break;
                    }

                  memset (pat, '\0', sizeof (struct patdeps));
                  pat->ignore_mtime = d->ignore_mtime;
                  pat->ignore_automatic_vars = d->ignore_automatic_vars;
                  pat->wait_here = d->wait_here;
                  pat->is_explicit = d->is_explicit;

                  DBS (DB_IMPLICIT,
                       (is_rule
                        ? _("Trying rule prerequisite '%s'.\n")
                        : _("Trying implicit prerequisite '%s'.\n"), d->name));

                  df = lookup_file (d->name);

                  if (df && df->is_explicit)
                    pat->is_explicit = 1;

                  /* If we found a file for the dep, set its intermediate flag.
                     df->is_explicit is set when the dep file is mentioned
                     explicitly on some other rule.  d->is_explicit is set when
                     the dep file is mentioned explicitly on this rule.  E.g.:
                       %.x : %.y ; ...
                     then:
                       one.x:
                       one.y:        # df->is_explicit
                     vs.
                       one.x: one.y  # d->is_explicit
                  */
                  if (df && !df->is_explicit && !d->is_explicit)
                    df->intermediate = 1;

                  /* If the pattern prereq is also explicitly mentioned for
                     FILE, skip all tests below since it must be built no
                     matter which implicit rule we choose. */
                  if (df && df->is_target)
                    /* This prerequisite is mentioned explicitly as a target of
                       some rule.  */
                    explicit = 1;
                  else
                    for (dp = file->deps; dp != 0; dp = dp->next)
                      if (streq (d->name, dep_name (dp)))
                        break;

                  /* If dp is set, this prerequisite is mentioned explicitly as
                     a prerequisite of the current target.  */

                  if (explicit || dp)
                    {
                      (pat++)->name = d->name;
                      DBS (DB_IMPLICIT, (_("'%s' ought to exist.\n"), d->name));
                      continue;
                    }

                  if (file_exists_p (d->name))
                    {
                      (pat++)->name = d->name;
                      DBS (DB_IMPLICIT, (_("Found '%s'.\n"), d->name));
                      continue;
                    }

                  if (df && allow_compat_rules)
                    {
                      (pat++)->name = d->name;
                      DBS (DB_IMPLICIT,
                           (_("Using compatibility rule '%s' due to '%s'.\n"),
                            get_rule_defn (rule), d->name));
                      continue;
                    }

                  if (df)
                    {
                      /* This prerequisite is mentioned explicitly as a
                         prerequisite on some rule, but it is not a
                         prerequisite of the current target. Therefore, this
                         prerequisite does not qualify as ought-to-exist. Keep
                         note of this rule and continue the search.  If a more
                         suitable rule is not found, then use this rule.  */
                      DBS (DB_IMPLICIT,
                           (_("Prerequisite '%s' of rule '%s' does not qualify"
                              " as ought to exist.\n"),
                            d->name, get_rule_defn (rule)));
                      found_compat_rule = 1;
                    }

                  /* This code, given FILENAME = "lib/foo.o", dependency name
                     "lib/foo.c", and VPATH=src, searches for
                     "src/lib/foo.c".  */
                  {
                    const char *vname = vpath_search (d->name, 0, NULL, NULL);
                    if (vname)
                      {
                        DBS (DB_IMPLICIT,
                             (_("Found prerequisite '%s' as VPATH '%s'.\n"),
                              d->name, vname));
                        (pat++)->name = d->name;
                        continue;
                      }
                  }

                  /* We could not find the file in any place we should look.
                     Look for an implicit rule to make this dependency, but
                     only on the second pass.  */

                  if (intermed_ok)
                    {
                      DBS (DB_IMPLICIT,
                           (d->is_explicit || (df && df->is_explicit)
                            ? _("Looking for a rule with explicit file '%s'.\n")
                            : _("Looking for a rule with intermediate file '%s'.\n"),
                            d->name));

                      if (int_file == 0)
                        int_file = alloca (sizeof (struct file));
                      memset (int_file, '\0', sizeof (struct file));
                      int_file->name = d->name;

                      if (pattern_search (int_file,
                                          0,
                                          depth,
                                          recursions + 1,
                                          allow_compat_rules))
                        {
                          pat->pattern = int_file->name;
                          int_file->name = d->name;
                          pat->file = int_file;
                          int_file = 0;
                          (pat++)->name = d->name;
                          continue;
                        }

                      /* If we have tried to find P as an intermediate file
                         and failed, mark that name as impossible so we won't
                         go through the search again later.  */
                      if (int_file->variables)
                        free_variable_set (int_file->variables);
                      if (int_file->pat_variables)
                        free_variable_set (int_file->pat_variables);

                      /* Keep prerequisites explicitly mentioned on unrelated
                         rules as "possible" to let compatibility search find
                         such prerequisites.  */
                      if (df == 0)
                        file_impossible (d->name);
                    }

                  /* A dependency of this rule does not exist. Therefore, this
                     rule fails.  */
                  if (intermed_ok)
                    DBS (DB_IMPLICIT,
                         (_("Rejecting rule '%s' "
                            "due to impossible prerequisite '%s'.\n"),
                          get_rule_defn (rule), d->name));
                  else
                    DBS (DB_IMPLICIT, (_("Not found '%s'.\n"), d->name));

                  failed = 1;
                  break;
                }

              /* Free the ns chain.  */
              free_dep_chain (dl);

              if (failed)
                break;
            }

          /* This rule is no longer 'in use' for recursive searches.  */
          rule->in_use = 0;

          if (! failed)
            /* This pattern rule does apply.  Stop looking for one.  */
            break;

          /* This pattern rule does not apply.  Keep looking.  */
        }

      /* If we found an applicable rule without intermediate files, don't try
         with them.  */
      if (ri < nrules)
        break;

      rule = 0;
    }

  /* RULE is nil if the loop went through the list but everything failed.  */
  if (rule == 0)
    goto done;

  foundrule = ri;

  /* If we are recursing, store the pattern that matched FILENAME in
     FILE->name for use in upper levels.  */

  if (recursions > 0)
    /* Kludge-o-matic */
    file->name = rule->targets[tryrules[foundrule].matches];

  /* DEPLIST lists the prerequisites for the rule we found.  This includes the
     intermediate files, if any.  Convert them into entries on the deps-chain
     of FILE.  */

  while (pat-- > deplist)
    {
      struct dep *dep;
      const char *s;

      if (pat->file != 0)
        {
          /* If we need to use an intermediate file, make sure it is entered
             as a target, with the info that was found for it in the recursive
             pattern_search call.  We know that the intermediate file did not
             already exist as a target; therefore we can assume that the deps
             and cmds of F below are null before we change them.  */

          struct file *imf = pat->file;
          struct file *f = lookup_file (imf->name);

          if (!f)
            f = enter_file (imf->name);

          f->deps = imf->deps;
          f->cmds = imf->cmds;
          f->stem = imf->stem;
          /* Setting target specific variables for a file causes the file to be
             entered to the database as a prerequisite. Implicit search then
             treats this file as explicitly mentioned. Preserve target specific
             variables of this file.  */
          merge_variable_set_lists(&f->variables, imf->variables);
          f->pat_variables = imf->pat_variables;
          f->pat_searched = imf->pat_searched;
          f->also_make = imf->also_make;
          f->is_target = 1;
          f->is_explicit |= imf->is_explicit || pat->is_explicit;
          f->notintermediate |= imf->notintermediate || no_intermediates;
          f->intermediate |= !f->is_explicit && !f->notintermediate;
          f->tried_implicit = 1;

          imf = lookup_file (pat->pattern);
          if (imf != 0 && imf->precious)
            f->precious = 1;

          for (dep = f->deps; dep != 0; dep = dep->next)
            {
              dep->file = enter_file (dep->name);
              dep->name = 0;
              dep->file->tried_implicit |= dep->changed;
            }
        }

      dep = alloc_dep ();
      dep->ignore_mtime = pat->ignore_mtime;
      dep->is_explicit = pat->is_explicit;
      dep->ignore_automatic_vars = pat->ignore_automatic_vars;
      dep->wait_here = pat->wait_here;
      s = strcache_add (pat->name);
      if (recursions)
        dep->name = s;
      else
        {
          dep->file = lookup_file (s);
          if (dep->file == 0)
            dep->file = enter_file (s);
        }

      if (pat->file == 0 && tryrules[foundrule].rule->terminal)
        {
          /* If the file actually existed (was not an intermediate file), and
             the rule that found it was a terminal one, then we want to mark
             the found file so that it will not have implicit rule search done
             for it.  If we are not entering a 'struct file' for it now, we
             indicate this with the 'changed' flag.  */
          if (dep->file == 0)
            dep->changed = 1;
          else
            dep->file->tried_implicit = 1;
        }

      dep->next = file->deps;
      file->deps = dep;

      /* The file changed its dependencies; schedule the shuffle.  */
      file->was_shuffled = 0;
    }

  if (!file->was_shuffled)
    shuffle_deps_recursive (file->deps);

  if (!tryrules[foundrule].checked_lastslash)
    {
      /* Always allocate new storage, since STEM might be on the stack for an
         intermediate file.  */
      file->stem = strcache_add_len (stem, stemlen);
      fullstemlen = stemlen;
    }
  else
    {
      /* We want to prepend the directory from
         the original FILENAME onto the stem.  */
      fullstemlen = pathlen + stemlen;
      memcpy (stem_str, filename, pathlen);
      memcpy (stem_str + pathlen, stem, stemlen);
      stem_str[fullstemlen] = '\0';
      file->stem = strcache_add (stem_str);
    }

  file->cmds = rule->cmds;
  file->is_target = 1;

  /* Set precious and notintermediate flags. */
  {
    struct file *f = lookup_file (rule->targets[tryrules[foundrule].matches]);
    if (f)
      {
        if (f->precious)
          file->precious = 1;
        if (f->notintermediate || no_intermediates)
          file->notintermediate = 1;
      }
  }

  /* If this rule builds other targets, too, put the others into FILE's
     'also_make' member.  */

  if (rule->num > 1)
    for (ri = 0; ri < rule->num; ++ri)
      if (ri != tryrules[foundrule].matches)
        {
          char *nm = alloca (rule->lens[ri] + fullstemlen + 1);
          char *p = nm;
          struct file *f;
          struct dep *new = alloc_dep ();

          /* GKM FIMXE: handle '|' here too */
          p = mempcpy (p, rule->targets[ri],
                       rule->suffixes[ri] - rule->targets[ri] - 1);
          p = mempcpy (p, file->stem, fullstemlen);
          memcpy (p, rule->suffixes[ri],
                  rule->lens[ri] - (rule->suffixes[ri] - rule->targets[ri])+1);
          new->name = strcache_add (nm);
          new->file = enter_file (new->name);
          new->next = file->also_make;

          /* Set precious flag. */
          f = lookup_file (rule->targets[ri]);
          if (f)
            {
              if (f->precious)
                new->file->precious = 1;
              if (f->notintermediate || no_intermediates)
                new->file->notintermediate = 1;
            }

          /* Set the is_target flag so that this file is not treated as
             intermediate by the pattern rule search algorithm and
             file_exists_p cannot pick it up yet.  */
          new->file->is_target = 1;

          file->also_make = new;
        }

 done:
  free (tryrules);
  free (deplist);

  --depth;

  if (rule)
    {
      DBS (DB_IMPLICIT, (_("Found implicit rule '%s' for '%s'.\n"),
                         get_rule_defn (rule), filename));
      return 1;
    }

  if (found_compat_rule)
    {
      DBS (DB_IMPLICIT, (_("Searching for a compatibility rule for '%s'.\n"),
                         filename));
      assert (allow_compat_rules == 0);
      return pattern_search (file, archive, depth, recursions, 1);
    }

  DBS (DB_IMPLICIT, (_("No implicit rule found for '%s'.\n"), filename));
  return 0;
}
