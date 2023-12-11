/* zmapfd - read contents of file descriptor into a newly-allocated buffer */

/* Copyright (C) 2006-2020 Free Software Foundation, Inc.

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

#include "bashansi.h"
#include "command.h"
#include "general.h"

#if !defined (errno)
extern int errno;
#endif

#ifndef ZBUFSIZ
#  define ZBUFSIZ 4096
#endif

extern ssize_t zread PARAMS((int, char *, size_t));

/* Dump contents of file descriptor FD to *OSTR.  FN is the filename for
   error messages (not used right now). */
int
zmapfd (fd, ostr, fn)
     int fd;
     char **ostr;
     char *fn;
{
  ssize_t nr;
  int rval;
  char lbuf[ZBUFSIZ];
  char *result;
  size_t rsize, rind;

  rval = 0;
  result = (char *)xmalloc (rsize = ZBUFSIZ);
  rind = 0;

  while (1)
    {
      nr = zread (fd, lbuf, sizeof (lbuf));
      if (nr == 0)
	{
	  rval = rind;
	  break;
	}
      else if (nr < 0)
	{
	  free (result);
	  if (ostr)
	    *ostr = (char *)NULL;
	  return -1;
	}

      RESIZE_MALLOCED_BUFFER (result, rind, nr, rsize, ZBUFSIZ);
      memcpy (result+rind, lbuf, nr);
      rind += nr;
    }

  RESIZE_MALLOCED_BUFFER (result, rind, 1, rsize, 128);
  result[rind] = '\0';

  if (ostr)
    *ostr = result;
  else
    free (result);

  return rval;
}
