/* Program name management.
   Copyright (C) 2016-2020 Free Software Foundation, Inc.

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

#ifndef _GL_GETPROGNAME_H
#define _GL_GETPROGNAME_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return the base name of the executing program.
   On native Windows this will usually end in ".exe" or ".EXE". */
#ifndef HAVE_GETPROGNAME
extern char const *getprogname (void)
# ifdef HAVE_DECL_PROGRAM_INVOCATION_NAME
  _GL_ATTRIBUTE_PURE
# endif
  ;
#endif

#ifdef __cplusplus
}
#endif

#endif
