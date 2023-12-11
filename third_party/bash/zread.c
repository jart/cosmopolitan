/* zread - read data from file descriptor into buffer with retries */

/* Copyright (C) 1999-2020 Free Software Foundation, Inc.

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

#include <sys/types.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <signal.h>
#include <errno.h>

#if !defined (errno)
extern int errno;
#endif

#ifndef SEEK_CUR
#  define SEEK_CUR 1
#endif

#ifndef ZBUFSIZ
#  define ZBUFSIZ 4096
#endif

extern int executing_builtin;

extern void check_signals_and_traps (void);
extern void check_signals (void);
extern int signal_is_trapped (int);
extern int read_builtin_timeout (int);

/* Read LEN bytes from FD into BUF.  Retry the read on EINTR.  Any other
   error causes the loop to break. */
ssize_t
zread (fd, buf, len)
     int fd;
     char *buf;
     size_t len;
{
  ssize_t r;

  check_signals ();	/* check for signals before a blocking read */
  /* should generalize into a mechanism where different parts of the shell can
     `register' timeouts and have them checked here. */
  while (((r = read_builtin_timeout (fd)) < 0 || (r = read (fd, buf, len)) < 0) &&
	     errno == EINTR)
    {
      int t;
      t = errno;
      /* XXX - bash-5.0 */
      /* We check executing_builtin and run traps here for backwards compatibility */
      if (executing_builtin)
	check_signals_and_traps ();	/* XXX - should it be check_signals()? */
      else
	check_signals ();
      errno = t;
    }

  return r;
}

/* Read LEN bytes from FD into BUF.  Retry the read on EINTR, up to three
   interrupts.  Any other error causes the loop to break. */

#ifdef NUM_INTR
#  undef NUM_INTR
#endif
#define NUM_INTR 3

ssize_t
zreadretry (fd, buf, len)
     int fd;
     char *buf;
     size_t len;
{
  ssize_t r;
  int nintr;

  for (nintr = 0; ; )
    {
      r = read (fd, buf, len);
      if (r >= 0)
	return r;
      if (r == -1 && errno == EINTR)
	{
	  if (++nintr >= NUM_INTR)
	    return -1;
	  continue;
	}
      return r;
    }
}

/* Call read(2) and allow it to be interrupted.  Just a stub for now. */
ssize_t
zreadintr (fd, buf, len)
     int fd;
     char *buf;
     size_t len;
{
  check_signals ();
  return (read (fd, buf, len));
}

/* Read one character from FD and return it in CP.  Return values are as
   in read(2).  This does some local buffering to avoid many one-character
   calls to read(2), like those the `read' builtin performs. */

static char lbuf[ZBUFSIZ];
static size_t lind, lused;

ssize_t
zreadc (fd, cp)
     int fd;
     char *cp;
{
  ssize_t nr;

  if (lind == lused || lused == 0)
    {
      nr = zread (fd, lbuf, sizeof (lbuf));
      lind = 0;
      if (nr <= 0)
	{
	  lused = 0;
	  return nr;
	}
      lused = nr;
    }
  if (cp)
    *cp = lbuf[lind++];
  return 1;
}

/* Don't mix calls to zreadc and zreadcintr in the same function, since they
   use the same local buffer. */
ssize_t
zreadcintr (fd, cp)
     int fd;
     char *cp;
{
  ssize_t nr;

  if (lind == lused || lused == 0)
    {
      nr = zreadintr (fd, lbuf, sizeof (lbuf));
      lind = 0;
      if (nr <= 0)
	{
	  lused = 0;
	  return nr;
	}
      lused = nr;
    }
  if (cp)
    *cp = lbuf[lind++];
  return 1;
}

/* Like zreadc, but read a specified number of characters at a time.  Used
   for `read -N'. */
ssize_t
zreadn (fd, cp, len)
     int fd;
     char *cp;
     size_t len;
{
  ssize_t nr;

  if (lind == lused || lused == 0)
    {
      if (len > sizeof (lbuf))
	len = sizeof (lbuf);
      nr = zread (fd, lbuf, len);
      lind = 0;
      if (nr <= 0)
	{
	  lused = 0;
	  return nr;
	}
      lused = nr;
    }
  if (cp)
    *cp = lbuf[lind++];
  return 1;
}

void
zreset ()
{
  lind = lused = 0;
}

/* Sync the seek pointer for FD so that the kernel's idea of the last char
   read is the last char returned by zreadc. */
void
zsyncfd (fd)
     int fd;
{
  off_t off, r;

  off = lused - lind;
  r = 0;
  if (off > 0)
    r = lseek (fd, -off, SEEK_CUR);

  if (r != -1)
    lused = lind = 0;
}
