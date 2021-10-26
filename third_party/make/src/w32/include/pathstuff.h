/* Definitions for Windows path manipulation.
Copyright (C) 1996-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _PATHSTUFF_H
#define _PATHSTUFF_H

char *convert_Path_to_windows32(char *Path, char to_delim);
char *convert_vpath_to_windows32(char *Path, char to_delim);
char *w32ify(const char *file, int resolve);
char *getcwd_fs(char *buf, int len);

#endif
