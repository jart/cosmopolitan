/* setlinebuf.c - line-buffer a stdio stream. */

/* Copyright (C) 1997,2022 Free Software Foundation, Inc.

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

#include <stdio.h>

#include "xmalloc.h"

#if defined (USING_BASH_MALLOC)
#  define LBUF_BUFSIZE	2016
#else
#  define LBUF_BUFSIZE	BUFSIZ
#endif

static char *stdoutbuf = 0;
static char *stderrbuf = 0;

/* Cause STREAM to buffer lines as opposed to characters or blocks. */
int
sh_setlinebuf (stream)
     FILE *stream;
{
#if !defined (HAVE_SETLINEBUF) && !defined (HAVE_SETVBUF)
  return (0);
#endif

#if defined (HAVE_SETVBUF)
  char *local_linebuf;

#if defined (USING_BASH_MALLOC)
  if (stream == stdout && stdoutbuf == 0)
    local_linebuf = stdoutbuf = (char *)xmalloc (LBUF_BUFSIZE);
  else if (stream == stderr && stderrbuf == 0)
    local_linebuf = stderrbuf = (char *)xmalloc (LBUF_BUFSIZE);
  else
    local_linebuf = (char *)NULL;	/* let stdio handle it */
#else
  local_linebuf = (char *)NULL;
#endif

  return (setvbuf (stream, local_linebuf, _IOLBF, LBUF_BUFSIZE));
#else /* !HAVE_SETVBUF */

  setlinebuf (stream);
  return (0);

#endif /* !HAVE_SETVBUF */
}
