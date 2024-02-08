/* pathnames.h -- absolute filenames that bash wants for various defaults. */

/* Copyright (C) 1987-2009 Free Software Foundation, Inc.

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

#if !defined (_PATHNAMES_H_)
#define _PATHNAMES_H_

/* The default file for hostname completion. */
#define DEFAULT_HOSTS_FILE "/etc/hosts"

/* The default login shell startup file. */
#define SYS_PROFILE "/etc/profile"

/* The default location of the bash debugger initialization/startup file. */
#define DEBUGGER_START_FILE	"/zip/usr/share/bashdb/bashdb-main.inc"

#endif /* _PATHNAMES_H */
