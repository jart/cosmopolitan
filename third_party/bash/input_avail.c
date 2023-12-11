/* input_avail.c -- check whether or not data is available for reading on a
		    specified file descriptor. */

/* Copyright (C) 2008,2009-2019 Free Software Foundation, Inc.

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

#if defined (__TANDEM)
#  include <floss.h>
#endif

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <sys/types.h>
#include <fcntl.h>
#if defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif /* HAVE_SYS_FILE_H */

#if defined (HAVE_PSELECT)
#  include <signal.h>
#endif

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "bashansi.h"

#include "posixselect.h"

#if defined (FIONREAD_IN_SYS_IOCTL)
#  include <sys/ioctl.h>
#endif

#include <stdio.h>
#include <errno.h>

#if !defined (errno)
extern int errno;
#endif /* !errno */

#if !defined (O_NDELAY) && defined (O_NONBLOCK)
#  define O_NDELAY O_NONBLOCK	/* Posix style */
#endif

/* Return >= 1 if select/FIONREAD indicates data available for reading on
   file descriptor FD; 0 if no data available.  Return -1 on error. */
int
input_avail (fd)
     int fd;
{
  int result, chars_avail;
#if defined(HAVE_SELECT)
  fd_set readfds, exceptfds;
  struct timeval timeout;
#endif

  if (fd < 0)
    return -1;

  chars_avail = 0;

#if defined (HAVE_SELECT)
  FD_ZERO (&readfds);
  FD_ZERO (&exceptfds);
  FD_SET (fd, &readfds);
  FD_SET (fd, &exceptfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  result = select (fd + 1, &readfds, (fd_set *)NULL, &exceptfds, &timeout);
  return ((result <= 0) ? 0 : 1);
#endif

#if defined (FIONREAD)
  errno = 0;
  result = ioctl (fd, FIONREAD, &chars_avail);
  if (result == -1 && errno == EIO)
    return -1;
  return (chars_avail);
#endif

  return 0;
}

/* Wait until NCHARS are available for reading on file descriptor FD.
   This can wait indefinitely. Return -1 on error. */
int
nchars_avail (fd, nchars)
     int fd;
     int nchars;
{
  int result, chars_avail;
#if defined(HAVE_SELECT)
  fd_set readfds, exceptfds;
#endif
#if defined (HAVE_PSELECT) || defined (HAVE_SELECT)
  sigset_t set, oset;
#endif

  if (fd < 0 || nchars < 0)
    return -1;
  if (nchars == 0)
    return (input_avail (fd));

  chars_avail = 0;

#if defined (HAVE_SELECT)
  FD_ZERO (&readfds);
  FD_ZERO (&exceptfds);
  FD_SET (fd, &readfds);
  FD_SET (fd, &exceptfds);
#endif
#if defined (HAVE_SELECT) || defined (HAVE_PSELECT)
  sigprocmask (SIG_BLOCK, (sigset_t *)NULL, &set);
#  ifdef SIGCHLD
  sigaddset (&set, SIGCHLD);
#  endif
  sigemptyset (&oset);
#endif

  while (1)
    {
      result = 0;
#if defined (HAVE_PSELECT)
      /* XXX - use pselect(2) to block SIGCHLD atomically */
      result = pselect (fd + 1, &readfds, (fd_set *)NULL, &exceptfds, (struct timespec *)NULL, &set);
#elif defined (HAVE_SELECT)
      sigprocmask (SIG_BLOCK, &set, &oset);
      result = select (fd + 1, &readfds, (fd_set *)NULL, &exceptfds, (struct timeval *)NULL);
      sigprocmask (SIG_BLOCK, &oset, (sigset_t *)NULL);
#endif
      if (result < 0)
        return -1;

#if defined (FIONREAD)
      errno = 0;
      result = ioctl (fd, FIONREAD, &chars_avail);
      if (result == -1 && errno == EIO)
        return -1;
      if (chars_avail >= nchars)
        break;
#else
      break;
#endif
    }

  return 0;
}
