/* redir.h - functions from redir.c. */

/* Copyright (C) 1997, 2001, 2005, 2008, 2009-2021 Free Software Foundation, Inc.

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

#if !defined (_REDIR_H_)
#define _REDIR_H_

#include "stdc.h"

/* Values for flags argument to do_redirections */
#define RX_ACTIVE	0x01	/* do it; don't just go through the motions */
#define RX_UNDOABLE	0x02	/* make a list to undo these redirections */
#define RX_CLEXEC	0x04	/* set close-on-exec for opened fds > 2 */
#define RX_INTERNAL	0x08
#define RX_USER		0x10
#define RX_SAVCLEXEC	0x20	/* set close-on-exec off in restored fd even though saved on has it on */
#define RX_SAVEFD	0x40	/* fd used to save another even if < SHELL_FD_BASE */

extern void redirection_error PARAMS((REDIRECT *, int, char *));
extern int do_redirections PARAMS((REDIRECT *, int));
extern char *redirection_expand PARAMS((WORD_DESC *));
extern int stdin_redirects PARAMS((REDIRECT *));

/* in builtins/evalstring.c for now, could move later */
extern int open_redir_file PARAMS((REDIRECT *, char **));

#endif /* _REDIR_H_ */
