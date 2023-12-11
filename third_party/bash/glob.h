/* File-name wildcard pattern matching for GNU.
   Copyright (C) 1985-2021 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne-Again SHell.

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

#ifndef	_GLOB_H_
#define	_GLOB_H_

#include "stdc.h"

#define GX_MARKDIRS	0x001	/* mark directory names with trailing `/' */
#define GX_NOCASE	0x002	/* ignore case */
#define GX_MATCHDOT	0x004	/* match `.' literally */
#define GX_MATCHDIRS	0x008	/* match only directory names */
#define GX_ALLDIRS	0x010	/* match all directory names, no others */
#define GX_NULLDIR	0x100	/* internal -- no directory preceding pattern */
#define GX_ADDCURDIR	0x200	/* internal -- add passed directory name */
#define GX_GLOBSTAR	0x400	/* turn on special handling of ** */
#define GX_RECURSE	0x800	/* internal -- glob_filename called recursively */
#define GX_SYMLINK	0x1000	/* internal -- symlink to a directory */
#define GX_NEGATE	0x2000	/* internal -- extglob pattern being negated */

extern int glob_pattern_p PARAMS((const char *));
extern char **glob_vector PARAMS((char *, char *, int));
extern char **glob_filename PARAMS((char *, int));

extern int extglob_pattern_p PARAMS((const char *));

extern char *glob_error_return;
extern int noglob_dot_filenames;
extern int glob_ignore_case;

#endif /* _GLOB_H_ */
