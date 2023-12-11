/* gettimeofday.c - gettimeofday replacement using time() */

/* Copyright (C) 2020 Free Software Foundation, Inc.

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

#if !defined (HAVE_GETTIMEOFDAY)

#include "posixtime.h"

/* A version of gettimeofday that just sets tv_sec from time(3) */
int
gettimeofday (struct timeval *tv, void *tz)
{
  tv->tv_sec = (time_t) time ((time_t *)0);
  tv->tv_usec = 0;
  return 0;
}
#endif
