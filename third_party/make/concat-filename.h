/* Construct a full filename from a directory and a relative filename.
   Copyright (C) 2001-2004, 2007-2023 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _CONCAT_FILENAME_H
#define _CONCAT_FILENAME_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Concatenate a directory filename, a relative filename and an optional
   suffix.  Return a freshly allocated filename.  Return NULL and set errno
   upon memory allocation failure.  */
extern char *concatenated_filename (const char *directory,
                                    const char *filename, const char *suffix)
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE;

/* Concatenate a directory filename, a relative filename and an optional
   suffix.  Return a freshly allocated filename.  */
extern char *xconcatenated_filename (const char *directory,
                                     const char *filename, const char *suffix)
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE
  _GL_ATTRIBUTE_RETURNS_NONNULL;


#ifdef __cplusplus
}
#endif

#endif /* _CONCAT_FILENAME_H */
