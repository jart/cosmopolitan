/* hashcmd.c - functions for managing a hash table mapping command names to
	       full pathnames. */

/* Copyright (C) 1997-2021 Free Software Foundation, Inc.

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

#include "bashtypes.h"
#include "posixstat.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"

#include "shell.h"
#include "flags.h"
#include "findcmd.h"
#include "hashcmd.h"

HASH_TABLE *hashed_filenames = (HASH_TABLE *)NULL;

static void phash_freedata PARAMS((PTR_T));

void
phash_create ()
{
  if (hashed_filenames == 0)
    hashed_filenames = hash_create (FILENAME_HASH_BUCKETS);
}

static void
phash_freedata (data)
     PTR_T data;
{
  free (((PATH_DATA *)data)->path);
  free (data);
}

void
phash_flush ()
{
  if (hashed_filenames)
    hash_flush (hashed_filenames, phash_freedata);
}

/* Remove FILENAME from the table of hashed commands. */
int
phash_remove (filename)
     const char *filename;
{
  register BUCKET_CONTENTS *item;

  if (hashing_enabled == 0 || hashed_filenames == 0)
    return 0;

  item = hash_remove (filename, hashed_filenames, 0);
  if (item)
    {
      if (item->data)
	phash_freedata (item->data);
      free (item->key);
      free (item);
      return 0;
    }
  return 1;
}

/* Place FILENAME (key) and FULL_PATH (data->path) into the
   hash table.  CHECK_DOT if non-null is for future calls to
   phash_search (); it means that this file was found
   in a directory in $PATH that is not an absolute pathname.
   FOUND is the initial value for times_found. */
void
phash_insert (filename, full_path, check_dot, found)
     char *filename, *full_path;
     int check_dot, found;
{
  register BUCKET_CONTENTS *item;

  if (hashing_enabled == 0)
    return;

  if (hashed_filenames == 0)
    phash_create ();

  item = hash_insert (filename, hashed_filenames, 0);
  if (item->data)
    free (pathdata(item)->path);
  else
    {
      item->key = savestring (filename);
      item->data = xmalloc (sizeof (PATH_DATA));
    }
  pathdata(item)->path = savestring (full_path);
  pathdata(item)->flags = 0;
  if (check_dot)
    pathdata(item)->flags |= HASH_CHKDOT;
  if (*full_path != '/')
    pathdata(item)->flags |= HASH_RELPATH;
  item->times_found = found;
}

/* Return the full pathname that FILENAME hashes to.  If FILENAME
   is hashed, but (data->flags & HASH_CHKDOT) is non-zero, check
   ./FILENAME and return that if it is executable.  This always
   returns a newly-allocated string; the caller is responsible
   for freeing it. */
char *
phash_search (filename)
     const char *filename;
{
  register BUCKET_CONTENTS *item;
  char *path, *dotted_filename, *tail;
  int same;

  if (hashing_enabled == 0 || hashed_filenames == 0)
    return ((char *)NULL);

  item = hash_search (filename, hashed_filenames, 0);

  if (item == NULL)
    return ((char *)NULL);

  /* If this filename is hashed, but `.' comes before it in the path,
     see if ./filename is executable.  If the hashed value is not an
     absolute pathname, see if ./`hashed-value' exists. */
  path = pathdata(item)->path;
  if (pathdata(item)->flags & (HASH_CHKDOT|HASH_RELPATH))
    {
      tail = (pathdata(item)->flags & HASH_RELPATH) ? path : (char *)filename;	/* XXX - fix const later */
      /* If the pathname does not start with a `./', add a `./' to it. */
      if (tail[0] != '.' || tail[1] != '/')
	{
	  dotted_filename = (char *)xmalloc (3 + strlen (tail));
	  dotted_filename[0] = '.'; dotted_filename[1] = '/';
	  strcpy (dotted_filename + 2, tail);
	}
      else
	dotted_filename = savestring (tail);

      if (executable_file (dotted_filename))
	return (dotted_filename);

      free (dotted_filename);

#if 0
      if (pathdata(item)->flags & HASH_RELPATH)
	return ((char *)NULL);
#endif

      /* Watch out.  If this file was hashed to "./filename", and
	 "./filename" is not executable, then return NULL. */

      /* Since we already know "./filename" is not executable, what
	 we're really interested in is whether or not the `path'
	 portion of the hashed filename is equivalent to the current
	 directory, but only if it starts with a `.'.  (This catches
	 ./. and so on.)  same_file () tests general Unix file
	 equivalence -- same device and inode. */
      if (*path == '.')
	{
	  same = 0;
	  tail = (char *)strrchr (path, '/');

	  if (tail)
	    {
	      *tail = '\0';
	      same = same_file (".", path, (struct stat *)NULL, (struct stat *)NULL);
	      *tail = '/';
	    }

	  return same ? (char *)NULL : savestring (path);
	}
    }

  return (savestring (path));
}
