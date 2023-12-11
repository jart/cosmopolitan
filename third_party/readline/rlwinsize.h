/* rlwinsize.h -- an attempt to isolate some of the system-specific defines
   for `struct winsize' and TIOCGWINSZ. */

/* Copyright (C) 1997-2009 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library (Readline), a library
   for reading lines of text with interactive input and history editing.      

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (_RLWINSIZE_H_)
#define _RLWINSIZE_H_

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

/* Try to find the definitions of `struct winsize' and TIOGCWINSZ */

#if defined (GWINSZ_IN_SYS_IOCTL) && !defined (TIOCGWINSZ)
#  include <sys/ioctl.h>
#endif /* GWINSZ_IN_SYS_IOCTL && !TIOCGWINSZ */

#if defined (STRUCT_WINSIZE_IN_TERMIOS) && !defined (STRUCT_WINSIZE_IN_SYS_IOCTL)
#  include <termios.h>
#endif /* STRUCT_WINSIZE_IN_TERMIOS && !STRUCT_WINSIZE_IN_SYS_IOCTL */

/* Not in either of the standard places, look around. */
#if !defined (STRUCT_WINSIZE_IN_TERMIOS) && !defined (STRUCT_WINSIZE_IN_SYS_IOCTL)
#  if defined (HAVE_SYS_STREAM_H)
#    include <sys/stream.h>
#  endif /* HAVE_SYS_STREAM_H */
#  if defined (HAVE_SYS_PTEM_H) /* SVR4.2, at least, has it here */
#    include <sys/ptem.h>
#    define _IO_PTEM_H          /* work around SVR4.2 1.1.4 bug */
#  endif /* HAVE_SYS_PTEM_H */
#  if defined (HAVE_SYS_PTE_H)  /* ??? */
#    include <sys/pte.h>
#  endif /* HAVE_SYS_PTE_H */
#endif /* !STRUCT_WINSIZE_IN_TERMIOS && !STRUCT_WINSIZE_IN_SYS_IOCTL */

#if defined (M_UNIX) && !defined (_SCO_DS) && !defined (tcflow)
#  define tcflow(fd, action)	ioctl(fd, TCXONC, action)
#endif

#endif /* _RL_WINSIZE_H */
