/* hashcmd.h - Common defines for hashing filenames. */

/* Copyright (C) 1993-2020 Free Software Foundation, Inc.

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

#include "stdc.h"
#include "hashlib.h"

#define FILENAME_HASH_BUCKETS 256	/* must be power of two */

extern HASH_TABLE *hashed_filenames;

typedef struct _pathdata {
  char *path;		/* The full pathname of the file. */
  int flags;
} PATH_DATA;

#define HASH_RELPATH	0x01	/* this filename is a relative pathname. */
#define HASH_CHKDOT	0x02	/* check `.' since it was earlier in $PATH */

#define pathdata(x) ((PATH_DATA *)(x)->data)

extern void phash_create PARAMS((void));
extern void phash_flush PARAMS((void));

extern void phash_insert PARAMS((char *, char *, int, int));
extern int phash_remove PARAMS((const char *));
extern char *phash_search PARAMS((const char *));
