/*
 * rename - rename a file
 */

/* Copyright (C) 1999 Free Software Foundation, Inc.

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

#if !defined (HAVE_RENAME)

#include "bashtypes.h"
#include "posixstat.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif
#include <errno.h>

#include "stdc.h"

#ifndef errno
extern int errno;
#endif

int
rename (from, to)
     const char *from, *to;
{
  struct stat fb, tb;

  if (stat (from, &fb) < 0)
    return -1;  

  if (stat (to, &tb) < 0)
    {
      if (errno != ENOENT)
	return -1;
    }
  else
    {
      if (fb.st_dev == tb.st_dev && fb.st_ino == tb.st_ino)
        return 0;		/* same file */
      if (unlink (to) < 0 && errno != ENOENT)
        return -1;
    }

  if (link (from, to) < 0)
    return (-1);

  if (unlink (from) < 0 && errno != ENOENT)
    {
      int e = errno;
      unlink (to);
      errno = e;
      return (-1);
    }

  return (0);
}
#endif /* !HAVE_RENAME */
