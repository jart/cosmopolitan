/* Basic filename support macros.
   Copyright (C) 2001-2004, 2007-2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _FILENAME_H
#define _FILENAME_H

#ifdef __cplusplus
extern "C" {
#endif


/* Pathname support.
   ISSLASH(C)           tests whether C is a directory separator character.
   IS_ABSOLUTE_PATH(P)  tests whether P is an absolute path.  If it is not,
                        it may be concatenated to a directory pathname.
   IS_PATH_WITH_DIR(P)  tests whether P contains a directory specification.
 */
# define ISSLASH(C) ((C) == '/')
# define HAS_DEVICE(Filename) ((void) (Filename), 0)
# define FILE_SYSTEM_PREFIX_LEN(Filename) ((void) (Filename), 0)
# define FILE_SYSTEM_DRIVE_PREFIX_CAN_BE_RELATIVE 0
# define IS_ABSOLUTE_FILE_NAME(Filename) ISSLASH ((Filename)[0])
# define IS_RELATIVE_FILE_NAME(Filename) (! ISSLASH ((Filename)[0]))
# define IS_FILE_NAME_WITH_DIR(Filename) (strchr ((Filename), '/') != NULL)


#ifdef __cplusplus
}
#endif

#endif /* _FILENAME_H */
