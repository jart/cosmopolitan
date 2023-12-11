/* posixwait.h -- job control definitions from POSIX 1003.1 */

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

#if !defined (_POSIXWAIT_H_)
#  define _POSIXWAIT_H_

/* If _POSIX_VERSION is not defined, we assume that <sys/wait.h> defines
   a `union wait' and various macros used to manipulate it.  Look in
   unionwait.h for the things we expect to find. */
#if defined (HAVE_SYS_WAIT_H)
#  include <sys/wait.h>
#else /* !HAVE_SYS_WAIT_H */
#  if !defined (_POSIX_VERSION)
#    include "unionwait.h"
#  endif
#endif  /* !HAVE_SYS_WAIT_H */

/* How to get the status of a job.  For Posix, this is just an
   int, but for other systems we have to crack the union wait. */
#if !defined (_POSIX_VERSION)
typedef union wait WAIT;
#  define WSTATUS(t)  (t.w_status)
#else /* _POSIX_VERSION */
typedef int WAIT;
#  define WSTATUS(t)  (t)
#endif /* _POSIX_VERSION */

/* Make sure that parameters to wait3 are defined. */
#if !defined (WNOHANG)
#  define WNOHANG 1
#  define WUNTRACED 2
#endif /* WNOHANG */

/* More Posix P1003.1 definitions.  In the POSIX versions, the parameter is
   passed as an `int', in the non-POSIX version, as `union wait'. */
#if defined (_POSIX_VERSION)

#  if !defined (WSTOPSIG)
#    define WSTOPSIG(s)       ((s) >> 8)
#  endif /* !WSTOPSIG */

#  if !defined (WTERMSIG)
#    define WTERMSIG(s)	      ((s) & 0177)
#  endif /* !WTERMSIG */

#  if !defined (WEXITSTATUS)
#    define WEXITSTATUS(s)    ((s) >> 8)
#  endif /* !WEXITSTATUS */

#  if !defined (WIFSTOPPED)
#    define WIFSTOPPED(s)     (((s) & 0177) == 0177)
#  endif /* !WIFSTOPPED */

#  if !defined (WIFEXITED)
#    define WIFEXITED(s)      (((s) & 0377) == 0)
#  endif /* !WIFEXITED */

#  if !defined (WIFSIGNALED)
#    define WIFSIGNALED(s)    (!WIFSTOPPED(s) && !WIFEXITED(s))
#  endif /* !WIFSIGNALED */

#  if !defined (WIFCORED)
#    if defined (WCOREDUMP)
#      define WIFCORED(s)	(WCOREDUMP(s))
#    else
#      define WIFCORED(s)       ((s) & 0200)
#    endif
#  endif /* !WIFCORED */

#else /* !_POSIX_VERSION */

#  if !defined (WSTOPSIG)
#    define WSTOPSIG(s)	      ((s).w_stopsig)
#  endif /* !WSTOPSIG */

#  if !defined (WTERMSIG)
#    define WTERMSIG(s)	      ((s).w_termsig)
#  endif /* !WTERMSIG */

#  if !defined (WEXITSTATUS)
#    define WEXITSTATUS(s)    ((s).w_retcode)
#  endif /* !WEXITSTATUS */

#  if !defined (WIFCORED)
#    define WIFCORED(s)       ((s).w_coredump)
#  endif /* !WIFCORED */

#endif /* !_POSIX_VERSION */

#endif /* !_POSIXWAIT_H_ */
