/* pcomplib.c - library functions for programmable completion. */

/* Copyright (C) 1999-2021 Free Software Foundation, Inc.

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

#if defined (PROGRAMMABLE_COMPLETION)

#include "bashansi.h"
#include <stdio.h>

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashintl.h"

#include "shell.h"
#include "pcomplete.h"

#define COMPLETE_HASH_BUCKETS	512	/* must be power of two */

#define STRDUP(x)	((x) ? savestring (x) : (char *)NULL)

HASH_TABLE *prog_completes = (HASH_TABLE *)NULL;

static void free_progcomp PARAMS((PTR_T));

COMPSPEC *
compspec_create ()
{
  COMPSPEC *ret;

  ret = (COMPSPEC *)xmalloc (sizeof (COMPSPEC));
  ret->refcount = 0;

  ret->actions = (unsigned long)0;
  ret->options = (unsigned long)0;

  ret->globpat = (char *)NULL;
  ret->words = (char *)NULL;
  ret->prefix = (char *)NULL;
  ret->suffix = (char *)NULL;
  ret->funcname = (char *)NULL;
  ret->command = (char *)NULL;
  ret->lcommand = (char *)NULL;
  ret->filterpat = (char *)NULL;

  return ret;
}

void
compspec_dispose (cs)
     COMPSPEC *cs;
{
  cs->refcount--;
  if (cs->refcount == 0)
    {
      FREE (cs->globpat);
      FREE (cs->words);
      FREE (cs->prefix);
      FREE (cs->suffix);
      FREE (cs->funcname);
      FREE (cs->command);
      FREE (cs->lcommand);
      FREE (cs->filterpat);

      free (cs);
    }
}

COMPSPEC *
compspec_copy (cs)
     COMPSPEC *cs;
{
  COMPSPEC *new;

  new = (COMPSPEC *)xmalloc (sizeof (COMPSPEC));

  new->refcount = 1; 	/* was cs->refcount, but this is a fresh copy */
  new->actions = cs->actions;
  new->options = cs->options;

  new->globpat = STRDUP (cs->globpat);
  new->words = STRDUP (cs->words);
  new->prefix = STRDUP (cs->prefix);
  new->suffix = STRDUP (cs->suffix);
  new->funcname = STRDUP (cs->funcname);
  new->command = STRDUP (cs->command);
  new->lcommand = STRDUP (cs->lcommand);
  new->filterpat = STRDUP (cs->filterpat);

  return new;
}

void
progcomp_create ()
{
  if (prog_completes == 0)
    prog_completes = hash_create (COMPLETE_HASH_BUCKETS);
}

int
progcomp_size ()
{
  return (HASH_ENTRIES (prog_completes));
}

static void
free_progcomp (data)
     PTR_T data;
{
  COMPSPEC *cs;

  cs = (COMPSPEC *)data;
  compspec_dispose (cs);
}
  
void
progcomp_flush ()
{
  if (prog_completes)
    hash_flush (prog_completes, free_progcomp);
}

void
progcomp_dispose ()
{
  if (prog_completes)
    hash_dispose (prog_completes);
  prog_completes = (HASH_TABLE *)NULL;
}

int
progcomp_remove (cmd)
     char *cmd;
{
  register BUCKET_CONTENTS *item;

  if (prog_completes == 0)
    return 1;

  item = hash_remove (cmd, prog_completes, 0);
  if (item)
    {
      if (item->data)
	free_progcomp (item->data);
      free (item->key);
      free (item);
      return (1);
    }
  return (0);
}

int
progcomp_insert (cmd, cs)
      char *cmd;
      COMPSPEC *cs;
{
  register BUCKET_CONTENTS *item;

  if (cs == NULL)
    programming_error (_("progcomp_insert: %s: NULL COMPSPEC"), cmd);

  if (prog_completes == 0)
    progcomp_create ();

  cs->refcount++;
  item = hash_insert (cmd, prog_completes, 0);
  if (item->data)
    free_progcomp (item->data);
  else
    item->key = savestring (cmd);
  item->data = cs;

  return 1;
}

COMPSPEC *
progcomp_search (cmd)
     const char *cmd;
{
  register BUCKET_CONTENTS *item;
  COMPSPEC *cs;

  if (prog_completes == 0)
    return ((COMPSPEC *)NULL);

  item = hash_search (cmd, prog_completes, 0);

  if (item == NULL)
    return ((COMPSPEC *)NULL);

  cs = (COMPSPEC *)item->data;

  return (cs);
}

void
progcomp_walk (pfunc)
     hash_wfunc *pfunc;
{
  if (prog_completes == 0 || pfunc == 0 || HASH_ENTRIES (prog_completes) == 0)
    return;

  hash_walk (prog_completes, pfunc);
}

#endif /* PROGRAMMABLE_COMPLETION */
