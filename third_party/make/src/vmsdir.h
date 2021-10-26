/* dirent.h for vms
Copyright (C) 1996-2020 Free Software Foundation, Inc.
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

#ifndef VMSDIR_H
#define VMSDIR_H

#include <rms.h>

#define	MAXNAMLEN	255

#ifndef __DECC
#if !defined (__GNUC__) && !defined (__ALPHA)
typedef unsigned long u_long;
typedef unsigned short u_short;
#endif
#endif

struct direct
{
  off_t d_off;
  u_long d_fileno;
  u_short d_reclen;
  u_short d_namlen;
  char d_name[MAXNAMLEN + 1];
};

#undef DIRSIZ
#define DIRSIZ(dp)		\
  (((sizeof (struct direct)	\
     - (MAXNAMLEN+1)		\
     + ((dp)->d_namlen+1))	\
    + 3) & ~3)

#define d_ino	d_fileno		/* compatibility */


/*
 * Definitions for library routines operating on directories.
 */

typedef struct DIR
{
  struct direct dir;
  char d_result[MAXNAMLEN + 1];
#if defined (__ALPHA) || defined (__DECC)
  struct FAB fab;
#else
  struct fabdef fab;
#endif
} DIR;

#ifndef NULL
#define NULL 0
#endif

#define rewinddir(dirp)	seekdir((dirp), (long)0)

DIR *opendir ();
struct direct *readdir (DIR *dfd);
int closedir (DIR *dfd);
const char *vmsify (const char *name, int type);

#endif /* VMSDIR_H */
