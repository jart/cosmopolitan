/* rlshell.h -- utility functions normally provided by bash. */

/* Copyright (C) 1999-2021 Free Software Foundation, Inc.

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

#if !defined (_RL_SHELL_H_)
#define _RL_SHELL_H_

#include "rlstdc.h"

extern char *sh_single_quote (char *);
extern void sh_set_lines_and_columns (int, int);
extern char *sh_get_env_value (const char *);
extern char *sh_get_home_dir (void);
extern int sh_unset_nodelay_mode (int);

#endif /* _RL_SHELL_H_ */
