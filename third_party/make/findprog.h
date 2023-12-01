/* Locating a program in PATH.
   Copyright (C) 2001-2003, 2009-2023 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

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

#ifndef _FINDPROG_H
#define _FINDPROG_H

#ifdef __cplusplus
extern "C" {
#endif


/* Looks up a program in the PATH.
   Attempts to determine the pathname that would be called by execlp/execvp
   of PROGNAME.  If successful, it returns a pathname containing a slash
   (either absolute or relative to the current directory).  Otherwise, it
   returns PROGNAME unmodified.
   Because of the latter case, callers should use execlp/execvp, not
   execl/execv on the returned pathname.
   The returned string is freshly malloc()ed if it is != PROGNAME.  */
extern const char *find_in_path (const char *progname);

/* Looks up a program in the given PATH-like string.

   The PATH argument consists of a list of directories, separated by ':' or
   (on native Windows) by ';'.  An empty PATH element designates the current
   directory.  A null PATH is equivalent to an empty PATH, that is, to the
   singleton list that contains only the current directory.

   If DIRECTORY is not NULL, all relative filenames (i.e. PROGNAME when it
   contains a slash, and the PATH elements) are considered relative to
   DIRECTORY instead of relative to the current directory of this process.

   Determines the pathname that would be called by execlp/execvp of PROGNAME.
   - If successful, it returns a pathname containing a slash (either absolute
     or relative to the current directory).  The returned string can be used
     with either execl/execv or execlp/execvp.  It is freshly malloc()ed if it
     is != PROGNAME.
   - Otherwise, it sets errno and returns NULL.
     Specific errno values include:
       - ENOENT: means that the program's file was not found.
       - EACCES: means that the program's file cannot be accessed (due to some
         issue with one of the ancestor directories) or lacks the execute
         permissions.
       - ENOMEM: means out of memory.
   If OPTIMIZE_FOR_EXEC is true, the function saves some work, under the
   assumption that the resulting pathname will not be accessed directly,
   only through execl/execv or execlp/execvp.

   Here, a "slash" means:
     - On POSIX systems excluding Cygwin: a '/',
     - On Windows, OS/2, DOS platforms: a '/' or '\'. */
extern const char *find_in_given_path (const char *progname, const char *path,
                                       const char *directory,
                                       bool optimize_for_exec);


#ifdef __cplusplus
}
#endif

#endif /* _FINDPROG_H */
