/* siglist.h -- encapsulate various definitions for sys_siglist */

/* Copyright (C) 1993, 2001, 2005, 2008-2020 Free Software Foundation, Inc.

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

#if !defined (_SIGLIST_H_)
#define _SIGLIST_H_

#if !defined (SYS_SIGLIST_DECLARED) && !defined (HAVE_STRSIGNAL)

#if defined (HAVE_UNDER_SYS_SIGLIST) && !defined (HAVE_SYS_SIGLIST) && !defined (sys_siglist)
#  define sys_siglist _sys_siglist
#endif /* HAVE_UNDER_SYS_SIGLIST && !HAVE_SYS_SIGLIST && !sys_siglist */

#if !defined (sys_siglist)
extern char *sys_siglist[];
#endif /* !sys_siglist */

#endif /* !SYS_SIGLIST_DECLARED && !HAVE_STRSIGNAL */

#if !defined (strsignal) && !defined (HAVE_STRSIGNAL)
#  define strsignal(sig) (char *)sys_siglist[sig]
#endif /* !strsignal && !HAVE_STRSIGNAL */

#if !defined (strsignal) && !HAVE_DECL_STRSIGNAL
extern char *strsignal PARAMS((int));
#endif

#endif /* _SIGLIST_H */
