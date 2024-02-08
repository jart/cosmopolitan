/* zwrite - write contents of buffer to file descriptor, retrying on error */

/* Copyright (C) 1999-2002 Free Software Foundation, Inc.

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

#include <errno.h>

#if !defined (errno)
extern int errno;
#endif

/* Write NB bytes from BUF to file descriptor FD, retrying the write if
   it is interrupted.  We retry three times if we get a zero-length
   write.  Any other signal causes this function to return prematurely. */
int
zwrite (fd, buf, nb)
     int fd;
     char *buf;
     size_t nb;
{
  int n, i, nt;

  for (n = nb, nt = 0;;)
    {
      i = write (fd, buf, n);
      if (i > 0)
	{
	  n -= i;
	  if (n <= 0)
	    return nb;
	  buf += i;
	}
      else if (i == 0)
	{
	  if (++nt > 3)
	    return (nb - n);
	}
      else if (errno != EINTR)
	return -1;
    }
}
