/* winsize.c - handle window size changes and information. */

/* Copyright (C) 2005-2020 Free Software Foundation, Inc.

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

#include "stdc.h"

#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <sys/ioctl.h>

/* Try to find the definitions of `struct winsize' and TIOGCWINSZ */

#if 0
#if defined (GWINSZ_IN_SYS_IOCTL) && !defined (TIOCGWINSZ)
#  include <sys/ioctl.h>
#endif /* GWINSZ_IN_SYS_IOCTL && !TIOCGWINSZ */
#endif

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

#include <stdio.h>

/* Return the fd from which we are actually getting input. */
#define input_tty() (shell_tty != -1) ? shell_tty : fileno (stderr)

#if !defined (errno)
extern int errno;
#endif /* !errno */

extern int shell_tty;

#if defined (READLINE)
/* Let's not call readline, forcing readline to initialize the termcap/terminfo
   variables it needs, unless we have to. */
extern int interactive_shell;
extern int no_line_editing;
extern int bash_readline_initialized;
extern void rl_set_screen_size PARAMS((int, int));
#endif
extern void sh_set_lines_and_columns PARAMS((int, int));

void
get_new_window_size (from_sig, rp, cp)
     int from_sig;
     int *rp, *cp;
{
#if defined (TIOCGWINSZ)
  struct winsize win;
  int tty;

  tty = input_tty ();
  if (tty >= 0 && (ioctl (tty, TIOCGWINSZ, &win) == 0) &&
      win.ws_row > 0 && win.ws_col > 0)
    {
      sh_set_lines_and_columns (win.ws_row, win.ws_col);
#if defined (READLINE)
      if ((interactive_shell && no_line_editing == 0) || bash_readline_initialized)
	rl_set_screen_size (win.ws_row, win.ws_col);
#endif
      if (rp)
	*rp = win.ws_row;
      if (cp)
	*cp = win.ws_col;
    }
#endif
}
