/* `dir', `vdir' and `ls' directory listing programs for GNU.

   Modified by Chet Ramey for Readline.

   Copyright (C) 1985, 1988, 1990-1991, 1995-2010, 2012 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Richard Stallman and David MacKenzie.  */

/* Color support by Peter Anvin <Peter.Anvin@linux.org> and Dennis
   Flaherty <dennisf@denix.elk.miles.com> based on original patches by
   Greg Lee <lee@uhunix.uhcc.hawaii.edu>.  */

#ifndef _PARSE_COLORS_H_
#define _PARSE_COLORS_H_

#include "readline.h"

#define LEN_STR_PAIR(s) sizeof (s) - 1, s

void _rl_parse_colors (void);

static const char *const indicator_name[]=
  {
    "lc", "rc", "ec", "rs", "no", "fi", "di", "ln", "pi", "so",
    "bd", "cd", "mi", "or", "ex", "do", "su", "sg", "st",
    "ow", "tw", "ca", "mh", "cl", NULL
  };

/* Buffer for color sequences */
static char *color_buf;

#endif /* !_PARSE_COLORS_H_ */
