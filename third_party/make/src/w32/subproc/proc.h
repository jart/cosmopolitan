/* Definitions for Windows
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

#ifndef  _PROC_H
#define _PROC_H

typedef int bool_t;

#define E_SCALL         101
#define E_IO            102
#define E_NO_MEM        103
#define E_FORK          104

extern bool_t arr2envblk(char **arr, char **envblk_out, size_t *envsize_needed);

#endif
