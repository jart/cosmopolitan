/* patchlevel.h -- current bash patch level */

/* Copyright (C) 2001-2021 Free Software Foundation, Inc.

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

#if !defined (_PATCHLEVEL_H_)
#define _PATCHLEVEL_H_

/* It's important that there be no other strings in this file that match the
   regexp `^#define[ 	]*PATCHLEVEL', since that's what support/mkversion.sh
   looks for to find the patch level (for the sccs version string). */

#define PATCHLEVEL 0

#endif /* _PATCHLEVEL_H_ */
