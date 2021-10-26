/* getdtablesize() function: Return maximum possible file descriptor value + 1.
   Copyright (C) 2008-2020 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2008.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#if defined _WIN32 && ! defined __CYGWIN__

# include <stdio.h>

# if HAVE_MSVC_INVALID_PARAMETER_HANDLER
#  include "msvc-inval.h"
# endif

# if HAVE_MSVC_INVALID_PARAMETER_HANDLER
static int
_setmaxstdio_nothrow (int newmax)
{
  int result;

  TRY_MSVC_INVAL
    {
      result = _setmaxstdio (newmax);
    }
  CATCH_MSVC_INVAL
    {
      result = -1;
    }
  DONE_MSVC_INVAL;

  return result;
}
# else
#  define _setmaxstdio_nothrow _setmaxstdio
# endif

/* Cache for the previous getdtablesize () result.  Safe to cache because
   Windows also lacks setrlimit.  */
static int dtablesize;

int
getdtablesize (void)
{
  if (dtablesize == 0)
    {
      /* We are looking for the number N such that the valid file descriptors
         are 0..N-1.  It can be obtained through a loop as follows:
           {
             int fd;
             for (fd = 3; fd < 65536; fd++)
               if (dup2 (0, fd) == -1)
                 break;
             return fd;
           }
         On Windows XP, the result is 2048.
         The drawback of this loop is that it allocates memory for a libc
         internal array that is never freed.

         The number N can also be obtained as the upper bound for
         _getmaxstdio ().  _getmaxstdio () returns the maximum number of open
         FILE objects.  The sanity check in _setmaxstdio reveals the maximum
         number of file descriptors.  This too allocates memory, but it is
         freed when we call _setmaxstdio with the original value.  */
      int orig_max_stdio = _getmaxstdio ();
      unsigned int bound;
      for (bound = 0x10000; _setmaxstdio_nothrow (bound) < 0; bound = bound / 2)
        ;
      _setmaxstdio_nothrow (orig_max_stdio);
      dtablesize = bound;
    }
  return dtablesize;
}

#else

# include <limits.h>
# include <sys/resource.h>

# ifndef RLIM_SAVED_CUR
#  define RLIM_SAVED_CUR RLIM_INFINITY
# endif
# ifndef RLIM_SAVED_MAX
#  define RLIM_SAVED_MAX RLIM_INFINITY
# endif

# ifdef __CYGWIN__
  /* Cygwin 1.7.25 auto-increases the RLIMIT_NOFILE soft limit until it
     hits the compile-time constant hard limit of 3200.  We might as
     well just report the hard limit.  */
#  define rlim_cur rlim_max
# endif

int
getdtablesize (void)
{
  struct rlimit lim;

  if (getrlimit (RLIMIT_NOFILE, &lim) == 0
      && 0 <= lim.rlim_cur && lim.rlim_cur <= INT_MAX
      && lim.rlim_cur != RLIM_INFINITY
      && lim.rlim_cur != RLIM_SAVED_CUR
      && lim.rlim_cur != RLIM_SAVED_MAX)
    return lim.rlim_cur;

  return INT_MAX;
}

#endif
