/* Implicit rule searching for GNU Make.
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

#include "makeint.h"
#include "filedef.h"
#include "rule.h"
#include "dep.h"
#include "debug.h"
#include "variable.h"
#include "job.h"      /* struct child, used inside commands.h */
#include "commands.h" /* set_file_variables */

static int pattern_search (struct file *file, int archive,
                           unsigned int depth, unsigned int recursions);

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

  if (pattern_search (file, 0, depth, 0))
    return 1;

#ifndef NO_ARCHIVES
  /* If this is an archive member reference, use just the
     archive member name to search for implicit rules.  */
  if (ar_name (file->name))
    {
      DBF (DB_IMPLICIT,
           _("Looking for archive-member implicit rule for '%s'.\n"));
      if (pattern_search (file, 1, depth, 0))
        return 1;
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
                unsigned int depth, unsigned int recursions)
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
  char *depname = alloca (namelen + max_pattern_dep_length + 4);

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
  struct rule *rule;

  char *pathdir = NULL;
  size_t pathlen;

  PATH_VAR (stem_str); /* @@ Need to get rid of stem, stemlen, etc. */

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
          DBS (DB_IMPLICIT, (_("Avoiding implicit rule recursion.\n")));
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

          if (stemlen + (check_lastslash ? pathlen : 0) > GET_PATH_MAX)
            {
              DBS (DB_IMPLICIT, (_("Stem too long: '%s%.*s'.\n"),
                                 check_lastslash ? pathdir : "",
                                 (int) stemlen, stem));
              continue;
            }

          DBS (DB_IMPLICIT, (_("Trying pattern rule with stem '%.*s'.\n"),
                             (int) stemlen, stem));

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

          /* Temporary assign STEM to file->stem (needed to set file
             variables below).   */
          file->stem = stem_str;

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
              char *p;

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
                  p = strchr (nptr, '%');
                  if (p == 0)
                    strcpy (depname, nptr);
                  else
                    {
                      char *o = depname;
                      if (check_lastslash)
                        {
                          memcpy (o, filename, pathlen);
                          o += pathlen;
                        }
                      memcpy (o, nptr, p - nptr);
                      o += p - nptr;
                      memcpy (o, stem, stemlen);
                      o += stemlen;
                      strcpy (o, p + 1);
                    }

                  /* Parse the expanded string.  It might have wildcards.  */
                  p = depname;
                  dl = PARSE_FILE_SEQ (&p, struct dep, MAP_NUL, NULL, PARSEFS_ONEWORD);
                  for (d = dl; d != NULL; d = d->next)
                    {
                      ++deps_found;
                      d->ignore_mtime = dep->ignore_mtime;
                      d->ignore_automatic_vars = dep->ignore_automatic_vars;
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
                  struct dep **dptr;

                  nptr = get_next_word (nptr, &len);
                  if (nptr == 0)
                    continue;

                  /* See this is a transition to order-only prereqs.  */
                  if (! order_only && len == 1 && nptr[0] == '|')
                    {
                      order_only = 1;
                      nptr += len;
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

                  p = lindex (nptr, nptr + len, '%');
                  if (p == 0)
                    {
                      memcpy (depname, nptr, len);
                      depname[len] = '\0';
                    }
                  else
                    {
                      size_t i = p - nptr;
                      char *o = depname;
                      memcpy (o, nptr, i);
                      o += i;
                      if (check_lastslash)
                        {
                          add_dir = 1;
                          memcpy (o, "$(*F)", 5);
                          o += 5;
                        }
                      else
                        {
                          memcpy (o, "$*", 2);
                          o += 2;
                        }
                      memcpy (o, p + 1, len - i - 1);
                      o[len - i - 1] = '\0';
                    }

                  /* Set up for the next word.  */
                  nptr += len;

                  /* Initialize and set file variables if we haven't already
                     done so. */
                  if (!file_vars_initialized)
                    {
                      initialize_file_variables (file, 0);
                      set_file_variables (file);
                      file_vars_initialized = 1;
                    }
                  /* Update the stem value in $* for this rule.  */
                  else if (!file_variables_set)
                    {
                      define_variable_for_file (
                        "*", 1, file->stem, o_automatic, 0, file);
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
                                                       add_dir ? pathdir : NULL, PARSEFS_NONE);
                      *dptr = dp;

                      for (d = dp; d != NULL; d = d->next)
                        {
                          ++deps_found;
                          if (order_only)
                            d->ignore_mtime = 1;
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
                  struct dep *expl_d;
                  int is_rule = d->name == dep_name (dep);

                  if (file_impossible_p (d->name))
                    {
                      /* If this prereq has already been ruled "impossible",
                         then the rule fails.  Don't bother trying it on the
                         second pass either since we know that will fail.  */
                      DBS (DB_IMPLICIT,
                           (is_rule
                            ? _("Rejecting impossible rule prerequisite '%s'.\n")
                            : _("Rejecting impossible implicit prerequisite '%s'.\n"),
                            d->name));
                      tryrules[ri].rule = 0;

                      failed = 1;
                      break;
                    }

                  memset (pat, '\0', sizeof (struct patdeps));
                  pat->ignore_mtime = d->ignore_mtime;
                  pat->ignore_automatic_vars = d->ignore_automatic_vars;

                  DBS (DB_IMPLICIT,
                       (is_rule
                        ? _("Trying rule prerequisite '%s'.\n")
                        : _("Trying implicit prerequisite '%s'.\n"), d->name));

                  /* If this prereq is also explicitly mentioned for FILE,
                     skip all tests below since it must be built no matter
                     which implicit rule we choose. */

                  for (expl_d = file->deps; expl_d != 0; expl_d = expl_d->next)
                    if (streq (dep_name (expl_d), d->name))
                      break;
                  if (expl_d != 0)
                    {
                      (pat++)->name = d->name;
                      continue;
                    }

                  /* The DEP->changed flag says that this dependency resides
                     in a nonexistent directory.  So we normally can skip
                     looking for the file.  However, if CHECK_LASTSLASH is
                     set, then the dependency file we are actually looking for
                     is in a different directory (the one gotten by prepending
                     FILENAME's directory), so it might actually exist.  */

                  /* @@ dep->changed check is disabled. */
                  if (lookup_file (d->name) != 0
                      /*|| ((!dep->changed || check_lastslash) && */
                      || file_exists_p (d->name))
                    {
                      (pat++)->name = d->name;
                      continue;
                    }

                  /* This code, given FILENAME = "lib/foo.o", dependency name
                     "lib/foo.c", and VPATH=src, searches for
                     "src/lib/foo.c".  */
                  {
                    const char *vname = vpath_search (d->name, 0, NULL, NULL);
                    if (vname)
                      {
                        DBS (DB_IMPLICIT,
                             (_("Found prerequisite '%s' as VPATH '%s'\n"),
                              d->name, vname));
                        (pat++)->name = d->name;
                        continue;
                      }
                  }

                  /* We could not find the file in any place we should look.
                     Try to make this dependency as an intermediate file, but
                     only on the second pass.  */

                  if (intermed_ok)
                    {
                      DBS (DB_IMPLICIT,
                           (_("Looking for a rule with intermediate file '%s'.\n"),
                            d->name));

                      if (int_file == 0)
                        int_file = alloca (sizeof (struct file));
                      memset (int_file, '\0', sizeof (struct file));
                      int_file->name = d->name;

                      if (pattern_search (int_file,
                                          0,
                                          depth + 1,
                                          recursions + 1))
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
                      file_impossible (d->name);
                    }

                  /* A dependency of this rule does not exist. Therefore, this
                     rule fails.  */
                  failed = 1;
                  break;
                }

              /* Free the ns chain.  */
              free_dep_chain (dl);

              if (failed)
                break;
            }

          /* Reset the stem in FILE. */

          file->stem = 0;

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

          /* We don't want to delete an intermediate file that happened
             to be a prerequisite of some (other) target. Mark it as
             secondary.  We don't want it to be precious as that disables
             DELETE_ON_ERROR etc.  */
          if (f != 0)
            f->secondary = 1;
          else
            f = enter_file (imf->name);

          f->deps = imf->deps;
          f->cmds = imf->cmds;
          f->stem = imf->stem;
          f->variables = imf->variables;
          f->pat_variables = imf->pat_variables;
          f->pat_searched = imf->pat_searched;
          f->also_make = imf->also_make;
          f->is_target = 1;
          f->intermediate = 1;
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
      dep->ignore_automatic_vars = pat->ignore_automatic_vars;
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
    }

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

  /* Set precious flag. */
  {
    struct file *f = lookup_file (rule->targets[tryrules[foundrule].matches]);
    if (f && f->precious)
      file->precious = 1;
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
          memcpy (p, rule->targets[ri],
                  rule->suffixes[ri] - rule->targets[ri] - 1);
          p += rule->suffixes[ri] - rule->targets[ri] - 1;
          memcpy (p, file->stem, fullstemlen);
          p += fullstemlen;
          memcpy (p, rule->suffixes[ri],
                  rule->lens[ri] - (rule->suffixes[ri] - rule->targets[ri])+1);
          new->name = strcache_add (nm);
          new->file = enter_file (new->name);
          new->next = file->also_make;

          /* Set precious flag. */
          f = lookup_file (rule->targets[ri]);
          if (f && f->precious)
            new->file->precious = 1;

          /* Set the is_target flag so that this file is not treated as
             intermediate by the pattern rule search algorithm and
             file_exists_p cannot pick it up yet.  */
          new->file->is_target = 1;

          file->also_make = new;
        }

 done:
  free (tryrules);
  free (deplist);

  return rule != 0;
}
