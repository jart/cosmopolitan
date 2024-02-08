/* itos.c -- Convert integer to string. */

/* Copyright (C) 1998-2002 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "shell.h"

char *
inttostr (i, buf, len)
     intmax_t i;
     char *buf;
     size_t len;
{
  return (fmtumax (i, 10, buf, len, 0));
}

/* Integer to string conversion.  This conses the string; the
   caller should free it. */
char *
itos (i)
     intmax_t i;
{
  char *p, lbuf[INT_STRLEN_BOUND(intmax_t) + 1];

  p = fmtumax (i, 10, lbuf, sizeof(lbuf), 0);
  return (savestring (p));
}

/* Integer to string conversion.  This conses the string using strdup;
   caller should free it and be prepared to deal with NULL return. */
char *
mitos (i)
     intmax_t i;
{
  char *p, lbuf[INT_STRLEN_BOUND(intmax_t) + 1];

  p = fmtumax (i, 10, lbuf, sizeof(lbuf), 0);
  return (strdup (p));
}

char *
uinttostr (i, buf, len)
     uintmax_t i;
     char *buf;
     size_t len;
{
  return (fmtumax (i, 10, buf, len, FL_UNSIGNED));
}

/* Integer to string conversion.  This conses the string; the
   caller should free it. */
char *
uitos (i)
     uintmax_t i;
{
  char *p, lbuf[INT_STRLEN_BOUND(uintmax_t) + 1];

  p = fmtumax (i, 10, lbuf, sizeof(lbuf), FL_UNSIGNED);
  return (savestring (p));
}
