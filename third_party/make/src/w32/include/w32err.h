/* Definitions for Windows error handling.
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

#ifndef _W32ERR_H_
#define _W32ERR_H_

#ifndef EXTERN_DECL
#define EXTERN_DECL(entry, args) entry args
#endif

EXTERN_DECL(const char * map_windows32_error_to_string, (DWORD error));

#endif /* !_W32ERR_H */
