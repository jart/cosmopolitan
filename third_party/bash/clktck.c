/* clktck.c - get the value of CLK_TCK. */

/* Copyright (C) 1997 Free Software Foundation, Inc.

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

#include "bashtypes.h"
#if defined (HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#if defined (HAVE_LIMITS_H)
#  include <limits.h>
#endif

#if !defined (HAVE_SYSCONF) || !defined (_SC_CLK_TCK)
#  if !defined (CLK_TCK)
#    if defined (HZ)
#      define CLK_TCK HZ
#    else
#      define CLK_TCK 60
#    endif
#  endif /* !CLK_TCK */
#endif /* !HAVE_SYSCONF && !_SC_CLK_TCK */

long
get_clk_tck ()
{
  static long retval = 0;

  if (retval != 0)
    return (retval);

#if defined (HAVE_SYSCONF) && defined (_SC_CLK_TCK)
  retval = sysconf (_SC_CLK_TCK);
#else /* !SYSCONF || !_SC_CLK_TCK */
  retval = CLK_TCK;
#endif /* !SYSCONF || !_SC_CLK_TCK */

  return (retval);
}
