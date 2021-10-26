/* Locating a program in a given path.
   Copyright (C) 2001-2004, 2006-2020 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001, 2019.

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


#include <config.h>

/* Specification.  */
#include "findprog.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "filename.h"
#include "concat-filename.h"
#include "xalloc.h"

#if (defined _WIN32 && !defined __CYGWIN__) || defined __EMX__ || defined __DJGPP__
  /* Native Windows, OS/2, DOS */
# define NATIVE_SLASH '\\'
#else
  /* Unix */
# define NATIVE_SLASH '/'
#endif

/* Separator in PATH like lists of pathnames.  */
#if (defined _WIN32 && !defined __CYGWIN__) || defined __EMX__ || defined __DJGPP__
  /* Native Windows, OS/2, DOS */
# define PATH_SEPARATOR ';'
#else
  /* Unix */
# define PATH_SEPARATOR ':'
#endif

/* The list of suffixes that the execlp/execvp function tries when searching
   for the program.  */
static const char * const suffixes[] =
  {
    #if defined _WIN32 && !defined __CYGWIN__ /* Native Windows */
    "", ".com", ".exe", ".bat", ".cmd"
    /* Note: Files without any suffix are not considered executable.  */
    /* Note: The cmd.exe program does a different lookup: It searches according
       to the PATHEXT environment variable.
       See <https://stackoverflow.com/questions/7839150/>.
       Also, it executes files ending .bat and .cmd directly without letting the
       kernel interpret the program file.  */
    #elif defined __CYGWIN__
    "", ".exe", ".com"
    #elif defined __EMX__
    "", ".exe"
    #elif defined __DJGPP__
    "", ".com", ".exe", ".bat"
    #else /* Unix */
    ""
    #endif
  };

const char *
find_in_given_path (const char *progname, const char *path,
                    bool optimize_for_exec)
{
  {
    bool has_slash = false;
    {
      const char *p;

      for (p = progname; *p != '\0'; p++)
        if (ISSLASH (*p))
          {
            has_slash = true;
            break;
          }
    }
    if (has_slash)
      {
        /* If progname contains a slash, it is either absolute or relative to
           the current directory.  PATH is not used.  */
        if (optimize_for_exec)
          /* The execl/execv/execlp/execvp functions will try the various
             suffixes anyway and fail if no executable is found.  */
          return progname;
        else
          {
            /* Try the various suffixes and see whether one of the files
               with such a suffix is actually executable.  */
            int failure_errno;
            size_t i;
            #if defined _WIN32 && !defined __CYGWIN__ /* Native Windows */
            const char *progbasename;

            {
              const char *p;

              progbasename = progname;
              for (p = progname; *p != '\0'; p++)
                if (ISSLASH (*p))
                  progbasename = p + 1;
            }
            #endif

            /* Try all platform-dependent suffixes.  */
            failure_errno = ENOENT;
            for (i = 0; i < sizeof (suffixes) / sizeof (suffixes[0]); i++)
              {
                const char *suffix = suffixes[i];

                #if defined _WIN32 && !defined __CYGWIN__ /* Native Windows */
                /* File names without a '.' are not considered executable, and
                   for file names with a '.' no additional suffix is tried.  */
                if ((*suffix != '\0') != (strchr (progbasename, '.') != NULL))
                #endif
                  {
                    /* Concatenate progname and suffix.  */
                    char *progpathname =
                      xconcatenated_filename ("", progname, suffix);

                    /* On systems which have the eaccess() system call, let's
                       use it.  On other systems, let's hope that this program
                       is not installed setuid or setgid, so that it is ok to
                       call access() despite its design flaw.  */
                    if (eaccess (progpathname, X_OK) == 0)
                      {
                        /* Found!  */
                        if (strcmp (progpathname, progname) == 0)
                          {
                            free (progpathname);
                            return progname;
                          }
                        else
                          return progpathname;
                      }

                    if (errno != ENOENT)
                      failure_errno = errno;

                    free (progpathname);
                  }
              }

            errno = failure_errno;
            return NULL;
          }
      }
  }

  if (path == NULL)
    /* If PATH is not set, the default search path is implementation dependent.
       In practice, it is treated like an empty PATH.  */
    path = "";

  {
    int failure_errno;
    /* Make a copy, to prepare for destructive modifications.  */
    char *path_copy = xstrdup (path);
    char *path_rest;
    char *cp;

    failure_errno = ENOENT;
    for (path_rest = path_copy; ; path_rest = cp + 1)
      {
        const char *dir;
        bool last;
        size_t i;

        /* Extract next directory in PATH.  */
        dir = path_rest;
        for (cp = path_rest; *cp != '\0' && *cp != PATH_SEPARATOR; cp++)
          ;
        last = (*cp == '\0');
        *cp = '\0';

        /* Empty PATH components designate the current directory.  */
        if (dir == cp)
          dir = ".";

        /* Try all platform-dependent suffixes.  */
        for (i = 0; i < sizeof (suffixes) / sizeof (suffixes[0]); i++)
          {
            const char *suffix = suffixes[i];

            #if defined _WIN32 && !defined __CYGWIN__ /* Native Windows */
            /* File names without a '.' are not considered executable, and
               for file names with a '.' no additional suffix is tried.  */
            if ((*suffix != '\0') != (strchr (progname, '.') != NULL))
            #endif
              {
                /* Concatenate dir, progname, and suffix.  */
                char *progpathname =
                  xconcatenated_filename (dir, progname, suffix);

                /* On systems which have the eaccess() system call, let's
                   use it.  On other systems, let's hope that this program
                   is not installed setuid or setgid, so that it is ok to
                   call access() despite its design flaw.  */
                if (eaccess (progpathname, X_OK) == 0)
                  {
                    /* Found!  */
                    if (strcmp (progpathname, progname) == 0)
                      {
                        free (progpathname);

                        /* Add the "./" prefix for real, that
                           xconcatenated_filename() optimized away.  This
                           avoids a second PATH search when the caller uses
                           execl/execv/execlp/execvp.  */
                        progpathname =
                          XNMALLOC (2 + strlen (progname) + 1, char);
                        progpathname[0] = '.';
                        progpathname[1] = NATIVE_SLASH;
                        memcpy (progpathname + 2, progname,
                                strlen (progname) + 1);
                      }

                    free (path_copy);
                    return progpathname;
                  }

                if (errno != ENOENT)
                  failure_errno = errno;

                free (progpathname);
              }
          }

        if (last)
          break;
      }

    /* Not found in PATH.  */
    free (path_copy);

    errno = failure_errno;
    return NULL;
  }
}
