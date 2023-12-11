/* posixselect.h -- wrapper for select(2) includes and definitions */

/* Copyright (C) 2009 Free Software Foundation, Inc.

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

#ifndef _POSIXSELECT_H_
#define _POSIXSELECT_H_

#if defined (FD_SET) && !defined (HAVE_SELECT)
#  define HAVE_SELECT 1
#endif

#if defined (HAVE_SELECT)
#  if !defined (HAVE_SYS_SELECT_H) || !defined (M_UNIX)
#    include <sys/time.h>
#  endif
#endif /* HAVE_SELECT */
#if defined (HAVE_SYS_SELECT_H)
#  include <sys/select.h>
#endif

#ifndef USEC_PER_SEC
#  define USEC_PER_SEC 1000000
#endif

#define USEC_TO_TIMEVAL(us, tv) \
do { \
  (tv).tv_sec = (us) / USEC_PER_SEC; \
  (tv).tv_usec = (us) % USEC_PER_SEC; \
} while (0)

#endif /* _POSIXSELECT_H_ */
