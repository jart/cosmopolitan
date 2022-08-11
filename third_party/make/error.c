/* clang-format off */
/* Error handler for noninteractive utilities
   Copyright (C) 1990-1998, 2000-2007, 2009-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

/* Written by David MacKenzie <djm@gnu.ai.mit.edu>.  */

#include "libc/sysv/consts/f.h"
#include "libc/calls/calls.h"
#include "third_party/make/config.h"
#include "third_party/make/error.h"
#include "libc/stdio/stdio.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/fmt.h"
#include "libc/str/str.h"
#include "libc/runtime/runtime.h"
#include "third_party/make/stdio.h"

#if !_LIBC && ENABLE_NLS
#include "third_party/make/gettext.h"
# define _(msgid) gettext (msgid)
#endif

#ifdef _LIBC
# define mbsrtowcs __mbsrtowcs
# define USE_UNLOCKED_IO 0
# define _GL_ATTRIBUTE_FORMAT_PRINTF(a, b)
# define _GL_ARG_NONNULL(a)
#else
#include "third_party/make/getprogname.h"
#endif

#ifndef _
# define _(String) String
#endif

/* If NULL, error will flush stdout, then print on stderr the program
   name, a colon and a space.  Otherwise, error will call this
   function without parameters instead.  */
void (*error_print_progname) (void);

/* This variable is incremented each time 'error' is called.  */
unsigned int error_message_count;

#undef fcntl
#define program_name getprogname ()
#define __strerror_r strerror_r

/* Return non-zero if FD is open.  */
static int
is_open (int fd)
{
  return 0 <= fcntl (fd, F_GETFL);
}

static void
flush_stdout (void)
{
#if !_LIBC
  int stdout_fd;

# if GNULIB_FREOPEN_SAFER
  /* Use of gnulib's freopen-safer module normally ensures that
       fileno (stdout) == 1
     whenever stdout is open.  */
  stdout_fd = STDOUT_FILENO;
# else
  /* POSIX states that fileno (stdout) after fclose is unspecified.  But in
     practice it is not a problem, because stdout is statically allocated and
     the fd of a FILE stream is stored as a field in its allocated memory.  */
  stdout_fd = fileno (stdout);
# endif
  /* POSIX states that fflush (stdout) after fclose is unspecified; it
     is safe in glibc, but not on all other platforms.  fflush (NULL)
     is always defined, but too draconian.  */
  if (0 <= stdout_fd && is_open (stdout_fd))
#endif
    fflush (stdout);
}

static void
print_errno_message (int errnum)
{
  char const *s;
  s = strerror (errnum);
  if (! s)
    s = _("Unknown system error");
  fprintf (stderr, ": %s", s);
}

static void _GL_ATTRIBUTE_FORMAT_PRINTF (3, 0) _GL_ARG_NONNULL ((3))
error_tail (int status, int errnum, const char *message, va_list args)
{
  vfprintf (stderr, message, args);
  ++error_message_count;
  if (errnum)
    print_errno_message (errnum);
  putc ('\n', stderr);
  fflush (stderr);
  if (status)
    exit (status);
}


/* Print the program name and error message MESSAGE, which is a printf-style
   format string with optional args.
   If ERRNUM is nonzero, print its corresponding system error message.
   Exit with status STATUS if it is nonzero.  */
void
error (int status, int errnum, const char *message, ...)
{
  va_list args;

  flush_stdout ();
  if (error_print_progname)
    (*error_print_progname) ();
  else
    {
      fprintf (stderr, "%s: ", program_name);
    }

  va_start (args, message);
  error_tail (status, errnum, message, args);
  va_end (args);
}

/* Sometimes we want to have at most one error per line.  This
   variable controls whether this mode is selected or not.  */
int error_one_per_line;

void
error_at_line (int status, int errnum, const char *file_name,
               unsigned int line_number, const char *message, ...)
{
  va_list args;

  if (error_one_per_line)
    {
      static const char *old_file_name;
      static unsigned int old_line_number;

      if (old_line_number == line_number
          && (file_name == old_file_name
              || (old_file_name != NULL
                  && file_name != NULL
                  && strcmp (old_file_name, file_name) == 0)))

        /* Simply return and print nothing.  */
        return;

      old_file_name = file_name;
      old_line_number = line_number;
    }

  flush_stdout ();
  if (error_print_progname)
    (*error_print_progname) ();
  else
    {
      fprintf (stderr, "%s:", program_name);
    }

  fprintf (stderr, file_name != NULL ? "%s:%u: " : " ",
           file_name, line_number);

  va_start (args, message);
  error_tail (status, errnum, message, args);
  va_end (args);
}

#ifdef _LIBC
/* Make the weak alias.  */
# undef error
# undef error_at_line
weak_alias (__error, error)
weak_alias (__error_at_line, error_at_line)
#endif
