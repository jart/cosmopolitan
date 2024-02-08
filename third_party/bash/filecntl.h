/* filecntl.h - Definitions to set file descriptors to close-on-exec. */

/* Copyright (C) 1993 Free Software Foundation, Inc.

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

#if !defined (_FILECNTL_H_)
#define _FILECNTL_H_

#include <fcntl.h>

/* Definitions to set file descriptors to close-on-exec, the Posix way. */
#if !defined (FD_CLOEXEC)
#define FD_CLOEXEC     1
#endif

#define FD_NCLOEXEC    0

#define SET_CLOSE_ON_EXEC(fd)  (fcntl ((fd), F_SETFD, FD_CLOEXEC))
#define SET_OPEN_ON_EXEC(fd)   (fcntl ((fd), F_SETFD, FD_NCLOEXEC))

/* How to open a file in non-blocking mode, the Posix.1 way. */
#if !defined (O_NONBLOCK)
#  if defined (O_NDELAY)
#    define O_NONBLOCK O_NDELAY
#  else
#    define O_NONBLOCK 0
#  endif
#endif

/* Make sure O_BINARY and O_TEXT are defined to avoid Windows-specific code. */
#if !defined (O_BINARY)
#  define O_BINARY 0
#endif
#if !defined (O_TEXT)
#  define O_TEXT 0
#endif

#endif /* ! _FILECNTL_H_ */
