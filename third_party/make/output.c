/* Output to stdout / stderr for GNU Make
Copyright (C) 2013-2023 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "makeint.h"
#include "os.h"
#include "output.h"

/* GNU Make no longer supports pre-ANSI89 environments.  */

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#else
# include <sys/file.h>
#endif

struct output *output_context = NULL;
unsigned int stdio_traced = 0;

#define OUTPUT_NONE (-1)

#define OUTPUT_ISSET(_out) ((_out)->out >= 0 || (_out)->err >= 0)

/* Write a string to the current STDOUT or STDERR.  */
static void
_outputs (struct output *out, int is_err, const char *msg)
{
  FILE *f;

  if (out && out->syncout)
    {
      int fd = is_err ? out->err : out->out;
      if (fd != OUTPUT_NONE)
        {
          size_t len = strlen (msg);
          int r;
          EINTRLOOP (r, lseek (fd, 0, SEEK_END));
          writebuf (fd, msg, len);
          return;
        }
    }

  f = is_err ? stderr : stdout;
  fputs (msg, f);
  fflush (f);
}

/* Write a message indicating that we've just entered or
   left (according to ENTERING) the current directory.  */

static int
log_working_directory (int entering)
{
  static char *buf = NULL;
  static size_t len = 0;
  size_t need;
  const char *fmt;
  char *p;

  /* Get enough space for the longest possible output.  */
  need = strlen (program) + INTSTR_LENGTH + 2 + 1;
  if (starting_directory)
    need += strlen (starting_directory);

  /* Use entire sentences to give the translators a fighting chance.  */
  if (makelevel == 0)
    if (starting_directory == 0)
      if (entering)
        fmt = _("%s: Entering an unknown directory\n");
      else
        fmt = _("%s: Leaving an unknown directory\n");
    else
      if (entering)
        fmt = _("%s: Entering directory '%s'\n");
      else
        fmt = _("%s: Leaving directory '%s'\n");
  else
    if (starting_directory == 0)
      if (entering)
        fmt = _("%s[%u]: Entering an unknown directory\n");
      else
        fmt = _("%s[%u]: Leaving an unknown directory\n");
    else
      if (entering)
        fmt = _("%s[%u]: Entering directory '%s'\n");
      else
        fmt = _("%s[%u]: Leaving directory '%s'\n");

  need += strlen (fmt);

  if (need > len)
    {
      buf = xrealloc (buf, need);
      len = need;
    }

  p = buf;
  if (print_data_base_flag)
    {
      *(p++) = '#';
      *(p++) = ' ';
    }

  if (makelevel == 0)
    if (starting_directory == 0)
      sprintf (p, fmt , program);
    else
      sprintf (p, fmt, program, starting_directory);
  else if (starting_directory == 0)
    sprintf (p, fmt, program, makelevel);
  else
    sprintf (p, fmt, program, makelevel, starting_directory);

  _outputs (NULL, 0, buf);

  return 1;
}


#ifndef NO_OUTPUT_SYNC

/* Support routine for output_sync() */
static void
pump_from_tmp (int from, FILE *to)
{
  static char buffer[8192];

#ifdef WINDOWS32
  int prev_mode;

  /* "from" is opened by open_tmpfd, which does it in binary mode, so
     we need the mode of "to" to match that.  */
  prev_mode = _setmode (fileno (to), _O_BINARY);
#endif

  if (lseek (from, 0, SEEK_SET) == -1)
    perror ("lseek()");

  while (1)
    {
      int len;
      EINTRLOOP (len, read (from, buffer, sizeof (buffer)));
      if (len < 0)
        perror ("read()");
      if (len <= 0)
        break;
      if (fwrite (buffer, len, 1, to) < 1)
        {
          perror ("fwrite()");
          break;
        }
      fflush (to);
    }

#ifdef WINDOWS32
  /* Switch "to" back to its original mode, so that log messages by
     Make have the same EOL format as without --output-sync.  */
  _setmode (fileno (to), prev_mode);
#endif
}

/* Returns a file descriptor to a temporary file, that will be automatically
   deleted on exit.  */
int
output_tmpfd (void)
{
  int fd = get_tmpfd (NULL);
  fd_set_append (fd);
  return fd;
}

/* Adds file descriptors to the child structure to support output_sync; one
   for stdout and one for stderr as long as they are open.  If stdout and
   stderr share a device they can share a temp file too.
   Will reset output_sync on error.  */
static void
setup_tmpfile (struct output *out)
{
  static unsigned int in_setup = 0;
  unsigned int io_state;

  /* If something fails during setup we might recurse back into this function
     while writing errors.  Make sure we don't do so infinitely.  */
  if (in_setup)
    return;
  in_setup = 1;

  io_state = check_io_state ();

  if (NONE_SET (io_state, IO_STDOUT_OK|IO_STDERR_OK))
    {
      /* This is probably useless since stdout/stderr aren't working. */
      perror_with_name ("output-sync suppressed: ", "stderr");
      goto error;
    }

  if (ANY_SET (io_state, IO_STDOUT_OK))
    {
      int fd = output_tmpfd ();
      if (fd < 0)
        goto error;
      fd_noinherit (fd);
      out->out = fd;
    }

  if (ANY_SET (io_state, IO_STDERR_OK))
    {
      if (out->out != OUTPUT_NONE && ANY_SET (io_state, IO_COMBINED_OUTERR))
        out->err = out->out;
      else
        {
          int fd = output_tmpfd ();
          if (fd < 0)
            goto error;
          fd_noinherit (fd);
          out->err = fd;
        }
    }

  in_setup = 0;
  return;

  /* If we failed to create a temp file, disable output sync going forward.  */
 error:
  O (error, NILF,
     _("cannot open output-sync lock file, suppressing output-sync."));

  output_close (out);
  output_sync = OUTPUT_SYNC_NONE;
  osync_clear ();
  in_setup = 0;
}

/* Synchronize the output of jobs in -j mode to keep the results of
   each job together. This is done by holding the results in temp files,
   one for stdout and potentially another for stderr, and only releasing
   them to "real" stdout/stderr when a semaphore can be obtained. */

void
output_dump (struct output *out)
{
#define FD_NOT_EMPTY(_f) ((_f) != OUTPUT_NONE && lseek ((_f), 0, SEEK_END) > 0)

  int outfd_not_empty = FD_NOT_EMPTY (out->out);
  int errfd_not_empty = FD_NOT_EMPTY (out->err);

  if (outfd_not_empty || errfd_not_empty)
    {
      int traced = 0;

      /* Try to acquire the semaphore.  If it fails, dump the output
         unsynchronized; still better than silently discarding it.
         We want to keep this lock for as little time as possible.  */
      if (!osync_acquire ())
        {
          O (error, NILF,
             _("warning: Cannot acquire output lock, disabling output sync."));
          osync_clear ();
        }

      /* Log the working directory for this dump.  */

      if (output_sync != OUTPUT_SYNC_RECURSE && should_print_dir ())
        traced = log_working_directory (1);

      if (outfd_not_empty)
        pump_from_tmp (out->out, stdout);
      if (errfd_not_empty && out->err != out->out)
        pump_from_tmp (out->err, stderr);

      if (traced)
        log_working_directory (0);

      /* Exit the critical section.  */
      osync_release ();

      /* Truncate and reset the output, in case we use it again.  */
      if (out->out != OUTPUT_NONE)
        {
          int e;
          lseek (out->out, 0, SEEK_SET);
          EINTRLOOP (e, ftruncate (out->out, 0));
        }
      if (out->err != OUTPUT_NONE && out->err != out->out)
        {
          int e;
          lseek (out->err, 0, SEEK_SET);
          EINTRLOOP (e, ftruncate (out->err, 0));
        }
    }
}
#endif /* NO_OUTPUT_SYNC */


void
output_init (struct output *out)
{
  if (out)
    {
      out->out = out->err = OUTPUT_NONE;
      out->syncout = !!output_sync;
      return;
    }

  /* Force stdout/stderr into append mode (if they are files) to ensure
     parallel jobs won't lose output due to overlapping writes.  */
  fd_set_append (fileno (stdout));
  fd_set_append (fileno (stderr));
}

void
output_close (struct output *out)
{
  if (! out)
    {
      if (stdio_traced)
        log_working_directory (0);
      return;
    }

#ifndef NO_OUTPUT_SYNC
  output_dump (out);
#endif

  if (out->out >= 0)
    close (out->out);
  if (out->err >= 0 && out->err != out->out)
    close (out->err);

  output_init (out);
}

/* We're about to generate output: be sure it's set up.  */
void
output_start (void)
{
#ifndef NO_OUTPUT_SYNC
  /* If we're syncing output make sure the temporary file is set up.  */
  if (output_context && output_context->syncout)
    if (! OUTPUT_ISSET(output_context))
      setup_tmpfile (output_context);
#endif

  /* If we're not syncing this output per-line or per-target, make sure we emit
     the "Entering..." message where appropriate.  */
  if (output_sync == OUTPUT_SYNC_NONE || output_sync == OUTPUT_SYNC_RECURSE)
    if (! stdio_traced && should_print_dir ())
      stdio_traced = log_working_directory (1);
}

void
outputs (int is_err, const char *msg)
{
  if (! msg || *msg == '\0')
    return;

  output_start ();

  _outputs (output_context, is_err, msg);
}


static struct fmtstring
  {
    char *buffer;
    size_t size;
  } fmtbuf = { NULL, 0 };

static char *
get_buffer (size_t need)
{
  /* Make sure we have room.  NEED includes space for \0.  */
  if (need > fmtbuf.size)
    {
      fmtbuf.size += need * 2;
      fmtbuf.buffer = xrealloc (fmtbuf.buffer, fmtbuf.size);
    }

  fmtbuf.buffer[need-1] = '\0';

  return fmtbuf.buffer;
}

/* Print a message on stdout.  */

void
message (int prefix, size_t len, const char *fmt, ...)
{
  va_list args;
  char *start;
  char *p;

  len += strlen (fmt) + strlen (program) + INTSTR_LENGTH + 4 + 1 + 1;
  start = p = get_buffer (len);

  if (prefix)
    {
      if (makelevel == 0)
        sprintf (p, "%s: ", program);
      else
        sprintf (p, "%s[%u]: ", program, makelevel);
      p += strlen (p);
    }

  va_start (args, fmt);
  vsprintf (p, fmt, args);
  va_end (args);

  strcat (p, "\n");

  assert (start[len-1] == '\0');
  outputs (0, start);
}

/* Print an error message.  */

void
error (const floc *flocp, size_t len, const char *fmt, ...)
{
  va_list args;
  char *start;
  char *p;

  len += (strlen (fmt) + strlen (program)
          + (flocp && flocp->filenm ? strlen (flocp->filenm) : 0)
          + INTSTR_LENGTH + 4 + 1 + 1);
  start = p = get_buffer (len);

  if (flocp && flocp->filenm)
    sprintf (p, "%s:%lu: ", flocp->filenm, flocp->lineno + flocp->offset);
  else if (makelevel == 0)
    sprintf (p, "%s: ", program);
  else
    sprintf (p, "%s[%u]: ", program, makelevel);
  p += strlen (p);

  va_start (args, fmt);
  vsprintf (p, fmt, args);
  va_end (args);

  strcat (p, "\n");

  assert (start[len-1] == '\0');
  outputs (1, start);
}

/* Print an error message and exit.  */

void
fatal (const floc *flocp, size_t len, const char *fmt, ...)
{
  va_list args;
  const char *stop = _(".  Stop.\n");
  char *start;
  char *p;

  len += (strlen (fmt) + strlen (program)
          + (flocp && flocp->filenm ? strlen (flocp->filenm) : 0)
          + INTSTR_LENGTH + 8 + strlen (stop) + 1);
  start = p = get_buffer (len);

  if (flocp && flocp->filenm)
    sprintf (p, "%s:%lu: *** ", flocp->filenm, flocp->lineno + flocp->offset);
  else if (makelevel == 0)
    sprintf (p, "%s: *** ", program);
  else
    sprintf (p, "%s[%u]: *** ", program, makelevel);
  p += strlen (p);

  va_start (args, fmt);
  vsprintf (p, fmt, args);
  va_end (args);

  strcat (p, stop);

  assert (start[len-1] == '\0');
  outputs (1, start);

  die (MAKE_FAILURE);
}

/* Print an error message from errno.  */

void
perror_with_name (const char *str, const char *name)
{
  const char *err = strerror (errno);
  OSSS (error, NILF, _("%s%s: %s"), str, name, err);
}

/* Print an error message from errno and exit.  */

void
pfatal_with_name (const char *name)
{
  const char *err = strerror (errno);
  OSS (fatal, NILF, _("%s: %s"), name, err);

  /* NOTREACHED */
}

/* Print a message about out of memory (not using more heap) and exit.
   Our goal here is to be sure we don't try to allocate more memory, which
   means we don't want to use string translations or normal cleanup.  */

void
out_of_memory ()
{
  writebuf (FD_STDOUT, program, strlen (program));
  writebuf (FD_STDOUT, STRING_SIZE_TUPLE (": *** virtual memory exhausted\n"));
  exit (MAKE_FAILURE);
}
