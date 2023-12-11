/* psize.c - Find pipe size. */

/* Copyright (C) 1987, 1991 Free Software Foundation, Inc.

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

/*  Write output in 128-byte chunks until we get a sigpipe or write gets an
    EPIPE.  Then report how many bytes we wrote.  We assume that this is the
    pipe size. */
#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>
#ifndef _MINIX
#include "bashtypes.h"
#endif
#include <signal.h>
#include <errno.h>

#include "command.h"
#include "general.h"
#include "sig.h"

#ifndef errno
extern int errno;
#endif

int nw;

sighandler
sigpipe (sig)
     int sig;
{
  fprintf (stderr, "%d\n", nw);
  exit (0);
}

int
main (argc, argv)
     int argc;
     char **argv;
{
  char buf[128];
  register int i;

  for (i = 0; i < 128; i++)
    buf[i] = ' ';

  signal (SIGPIPE, sigpipe);

  nw = 0;
  for (;;)
    {
      int n;
      n = write (1, buf, 128);
      nw += n;
    }
  return (0);
}
