/* posixdir.h -- Posix directory reading includes and defines. */

/* Copyright (C) 1987,1991,2012,2019,2021 Free Software Foundation, Inc.

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

/* This file should be included instead of <dirent.h> or <sys/dir.h>. */

#if !defined (_POSIXDIR_H_)
#define _POSIXDIR_H_

#if defined (HAVE_DIRENT_H)
#  include <dirent.h>
#  if defined (HAVE_STRUCT_DIRENT_D_NAMLEN)
#    define D_NAMLEN(d)	((d)->d_namlen)
#  else
#    define D_NAMLEN(d)   (strlen ((d)->d_name))
#  endif /* !HAVE_STRUCT_DIRENT_D_NAMLEN */
#else
#  if defined (HAVE_SYS_NDIR_H)
#    include <sys/ndir.h>
#  endif
#  if defined (HAVE_SYS_DIR_H)
#    include <sys/dir.h>
#  endif
#  if defined (HAVE_NDIR_H)
#    include <ndir.h>
#  endif
#  if !defined (dirent)
#    define dirent direct
#  endif /* !dirent */
#  define D_NAMLEN(d)   ((d)->d_namlen)
#endif /* !HAVE_DIRENT_H */

/* The bash code fairly consistently uses d_fileno; make sure it's available */
#if defined (HAVE_STRUCT_DIRENT_D_INO) && !defined (HAVE_STRUCT_DIRENT_D_FILENO)
#  define d_fileno d_ino
#endif

/* Posix does not require that the d_ino field be present, and some
   systems do not provide it. */
#if !defined (HAVE_STRUCT_DIRENT_D_INO) || defined (BROKEN_DIRENT_D_INO)
#  define REAL_DIR_ENTRY(dp) 1
#else
#  define REAL_DIR_ENTRY(dp) (dp->d_ino != 0)
#endif /* _POSIX_SOURCE */

#if defined (HAVE_STRUCT_DIRENT_D_INO) && !defined (BROKEN_DIRENT_D_INO)
#  define D_INO_AVAILABLE
#endif

/* Signal the rest of the code that it can safely use dirent.d_fileno */
#if defined (D_INO_AVAILABLE) || defined (HAVE_STRUCT_DIRENT_D_FILENO)
#  define D_FILENO_AVAILABLE 1
#endif

#endif /* !_POSIXDIR_H_ */
