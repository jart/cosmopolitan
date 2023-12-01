/* Miscellaneous global declarations and portability cruft for GNU Make.
Copyright (C) 2023 Free Software Foundation, Inc.
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

/*
  This file is included at the end of config.h

   That means it's included _everywhere_ as the first thing,
   INCLUDING content imported from gnulib.  BE AWARE!!
*/

#undef  HAVE_CONFIG_H
#define HAVE_CONFIG_H 1

/* Specify we want GNU source code.  This must be defined before any
   system headers are included.  */

#define _GNU_SOURCE 1

/* AIX requires this to be the first thing in the file.  */
#if HAVE_ALLOCA_H
# include <alloca.h>
#else
# ifdef _AIX
 #pragma alloca
# else
#  if !defined(__GNUC__) && !defined(WINDOWS32)
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca ();
#   endif
#  endif
# endif
#endif

/* Declare function prototypes for src/misc.c functions if needed.  */

#include <stddef.h>

#if !HAVE_STRCASECMP && !HAVE_STRICMP && !HAVE_STRCMPI
int strcasecmp (const char *s1, const char *s2);
#endif

#if !HAVE_STRNCASECMP && !HAVE_STRNICMP && !HAVE_STRNCMPI
int strncasecmp (const char *s1, const char *s2, size_t n);
#endif

#if !HAVE_MEMPCPY
void *mempcpy (void *dest, const void *src, size_t n);
#endif

#if !HAVE_STPCPY
char *stpcpy (char *dest, const char *src);
#endif
