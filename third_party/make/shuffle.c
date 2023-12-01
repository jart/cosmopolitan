/* Provide prerequisite shuffle support.
Copyright (C) 2022-2023 Free Software Foundation, Inc.
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

#include "shuffle.h"

#include "filedef.h"
#include "dep.h"

/* Supported shuffle modes.  */
static void random_shuffle_array (void ** a, size_t len);
static void reverse_shuffle_array (void ** a, size_t len);
static void identity_shuffle_array (void ** a, size_t len);

/* The way goals and rules are shuffled during update.  */
enum shuffle_mode
  {
    /* No shuffle data is populated or used.  */
    sm_none,
    /* Random within dependency list.  */
    sm_random,
    /* Inverse order.  */
    sm_reverse,
    /* identity order. Differs from SM_NONE by explicitly populating
       the traversal order.  */
    sm_identity,
  };

/* Shuffle configuration.  */
static struct
  {
    enum shuffle_mode mode;
    unsigned int seed;
    void (*shuffler) (void **a, size_t len);
    char strval[INTSTR_LENGTH + 1];
  } config = { sm_none, 0, NULL, "" };

/* Return string value of --shuffle= option passed.
   If none was passed or --shuffle=none was used function
   returns NULL.  */
const char *
shuffle_get_mode ()
{
  return config.strval[0] == '\0' ? NULL : config.strval;
}

void
shuffle_set_mode (const char *cmdarg)
{
  /* Parse supported '--shuffle' mode.  */
  if (strcasecmp (cmdarg, "reverse") == 0)
    {
      config.mode = sm_reverse;
      config.shuffler = reverse_shuffle_array;
      strcpy (config.strval, "reverse");
    }
  else if (strcasecmp (cmdarg, "identity") == 0)
    {
      config.mode = sm_identity;
      config.shuffler = identity_shuffle_array;
      strcpy (config.strval, "identity");
    }
  else if (strcasecmp (cmdarg, "none") == 0)
    {
      config.mode = sm_none;
      config.shuffler = NULL;
      config.strval[0] = '\0';
    }
  else
    {
      if (strcasecmp (cmdarg, "random") == 0)
        config.seed = make_rand ();
      else
        {
          /* Assume explicit seed.  */
          const char *err;
          config.seed = make_toui (cmdarg, &err);
          if (err)
            OSS (fatal, NILF, _("invalid shuffle mode: %s: '%s'"), err, cmdarg);
        }

      config.mode = sm_random;
      config.shuffler = random_shuffle_array;
      sprintf (config.strval, "%u", config.seed);
    }
}

/* Shuffle array elements using RAND().  */
static void
random_shuffle_array (void **a, size_t len)
{
  size_t i;
  for (i = 0; i < len; i++)
    {
      void *t;

      /* Pick random element and swap. */
      unsigned int j = make_rand () % len;
      if (i == j)
        continue;

      /* Swap. */
      t = a[i];
      a[i] = a[j];
      a[j] = t;
    }
}

/* Shuffle array elements using reverse order.  */
static void
reverse_shuffle_array (void **a, size_t len)
{
  size_t i;
  for (i = 0; i < len / 2; i++)
    {
      void *t;

      /* Pick mirror and swap. */
      size_t j = len - 1 - i;

      /* Swap. */
      t = a[i];
      a[i] = a[j];
      a[j] = t;
    }
}

/* Shuffle array elements using identity order.  */
static void
identity_shuffle_array (void **a UNUSED, size_t len UNUSED)
{
  /* No-op!  */
}

/* Shuffle list of dependencies by populating '->shuf'
   field in each 'struct dep'.  */
static void
shuffle_deps (struct dep *deps)
{
  size_t ndeps = 0;
  struct dep *dep;
  void **da;
  void **dp;

  for (dep = deps; dep; dep = dep->next)
    {
      /* Do not reshuffle prerequisites if any .WAIT is present.  */
      if (dep->wait_here)
        return;

      ndeps++;
    }

  if (ndeps == 0)
    return;

  /* Allocate array of all deps, store, shuffle, write back.  */
  da = xmalloc (sizeof (struct dep *) * ndeps);

  /* Store locally.  */
  for (dep = deps, dp = da; dep; dep = dep->next, dp++)
    *dp = dep;

  /* Shuffle.  */
  config.shuffler (da, ndeps);

  /* Write back.  */
  for (dep = deps, dp = da; dep; dep = dep->next, dp++)
    dep->shuf = *dp;

  free (da);
}

/* Shuffle 'deps' of each 'file' recursively.  */
static void
shuffle_file_deps_recursive (struct file *f)
{
  struct dep *dep;

  /* Implicit rules do not always provide any depends.  */
  if (!f)
    return;

  /* Avoid repeated shuffles and loops.  */
  if (f->was_shuffled)
    return;
  f->was_shuffled = 1;

  shuffle_deps (f->deps);

  /* Shuffle dependencies. */
  for (dep = f->deps; dep; dep = dep->next)
    shuffle_file_deps_recursive (dep->file);
}

/* Shuffle goal dependencies first, then shuffle dependency list
   of each file reachable from goaldep recursively.  Used by
   --shuffle flag to introduce artificial non-determinism in build
   order.  .*/

void
shuffle_deps_recursive (struct dep *deps)
{
  struct dep *dep;

  /* Exit early if shuffling was not requested.  */
  if (config.mode == sm_none)
    return;

  /* Do not reshuffle prerequisites if .NOTPARALLEL was specified.  */
  if (not_parallel)
    return;

  /* Set specific seed at the top level of recursion.  */
  if (config.mode == sm_random)
    make_seed (config.seed);

  shuffle_deps (deps);

  /* Shuffle dependencies. */
  for (dep = deps; dep; dep = dep->next)
    shuffle_file_deps_recursive (dep->file);
}
