/* mailstat.c -- stat a mailbox file, handling maildir-type mail directories */

/* Copyright (C) 2001 Free Software Foundation, Inc.

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
 
#include <stdio.h>
#include <errno.h>

#include "bashtypes.h"
#include "posixstat.h"
#include "posixdir.h"
#include "bashansi.h"

#if defined (HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif

#include "maxpath.h"

/*
 *     Stat a file. If it's a maildir, check all messages
 *     in the maildir and present the grand total as a file.
 *     The fields in the 'struct stat' are from the mail directory.
 *     The following fields are emulated:
 *
 *     st_nlink	always 1, unless st_blocks is not present, in which case it's
 *		the total number of messages
 *     st_size	 total number of bytes in all files
 *     st_blocks       total number of messages, if present in struct stat
 *     st_atime	access time of newest file in maildir
 *     st_mtime	modify time of newest file in maildir
 *     st_mode	 S_IFDIR changed to S_IFREG
 *
 *     This is good enough for most mail-checking applications.
 */

int
mailstat(path, st)
     const char *path;
     struct stat *st;
{
  static struct stat st_new_last, st_ret_last;
  struct stat st_ret, st_tmp;
  DIR *dd;
  struct dirent *fn;
  char dir[PATH_MAX * 2], file[PATH_MAX * 2 + 1];
  int i, l;
  time_t atime, mtime;

  atime = mtime = 0;

  /* First see if it's a directory. */
  if ((i = stat(path, st)) != 0 || S_ISDIR(st->st_mode) == 0)
    return i;

  if (strlen(path) > sizeof(dir) - 5)
    {
#ifdef ENAMETOOLONG
      errno = ENAMETOOLONG;
#else
      errno = EINVAL;
#endif
      return -1;
    }

  st_ret = *st;
  st_ret.st_nlink = 1;
  st_ret.st_size  = 0;
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
  st_ret.st_blocks  = 0;
#else
  st_ret.st_nlink = 0;
#endif
  st_ret.st_mode  &= ~S_IFDIR;
  st_ret.st_mode  |= S_IFREG;

  /* See if cur/ is present */
  sprintf(dir, "%s/cur", path);
  if (stat(dir, &st_tmp) || S_ISDIR(st_tmp.st_mode) == 0)
    return 0;
  st_ret.st_atime = st_tmp.st_atime;

  /* See if tmp/ is present */
  sprintf(dir, "%s/tmp", path);
  if (stat(dir, &st_tmp) || S_ISDIR(st_tmp.st_mode) == 0)
    return 0;
  st_ret.st_mtime = st_tmp.st_mtime;

  /* And new/ */
  sprintf(dir, "%s/new", path);
  if (stat(dir, &st_tmp) || S_ISDIR(st_tmp.st_mode) == 0)
    return 0;
  st_ret.st_mtime = st_tmp.st_mtime;

  /* Optimization - if new/ didn't change, nothing else did. */
  if (st_tmp.st_dev == st_new_last.st_dev &&
      st_tmp.st_ino == st_new_last.st_ino &&
      st_tmp.st_atime == st_new_last.st_atime &&
      st_tmp.st_mtime == st_new_last.st_mtime)
    {
      *st = st_ret_last;
      return 0;
    }
  st_new_last = st_tmp;

  /* Loop over new/ and cur/ */
  for (i = 0; i < 2; i++)
    {
      sprintf(dir, "%s/%s", path, i ? "cur" : "new");
      sprintf(file, "%s/", dir);
      l = strlen(file);
      if ((dd = opendir(dir)) == NULL)
	return 0;
      while ((fn = readdir(dd)) != NULL)
	{
	  if (fn->d_name[0] == '.' || strlen(fn->d_name) + l >= sizeof(file))
	    continue;
	  strcpy(file + l, fn->d_name);
	  if (stat(file, &st_tmp) != 0)
	    continue;
	  st_ret.st_size += st_tmp.st_size;
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
	  st_ret.st_blocks++;
#else
	  st_ret.st_nlink++;
#endif
	  if (st_tmp.st_atime != st_tmp.st_mtime && st_tmp.st_atime > atime)
	    atime = st_tmp.st_atime;
	  if (st_tmp.st_mtime > mtime)
	    mtime = st_tmp.st_mtime;
	}
      closedir(dd);
    }

/*  if (atime) */	/* Set atime even if cur/ is empty */
      st_ret.st_atime = atime;
    if (mtime)
      st_ret.st_mtime = mtime;

    *st = st_ret_last = st_ret;
    return 0;
}
