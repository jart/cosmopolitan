/* Output to stdout / stderr for GNU make
Copyright (C) 2013-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "makeint.h"
#include "os.h"
#include "output.h"

/* GNU make no longer supports pre-ANSI89 environments.  */

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

#ifdef WINDOWS32
# include <windows.h>
# include <io.h>
# include "sub_proc.h"
#endif /* WINDOWS32 */

struct output *output_context = NULL;
unsigned int stdio_traced = 0;

#define OUTPUT_NONE (-1)

#define OUTPUT_ISSET(_out) ((_out)->out >= 0 || (_out)->err >= 0)

#ifdef HAVE_FCNTL_H
# define STREAM_OK(_s) ((fcntl (fileno (_s), F_GETFD) != -1) || (errno != EBADF))
#else
# define STREAM_OK(_s) 1
#endif

/* Write a string to the current STDOUT or STDERR.  */
static void
_outputs (struct output *out, int is_err, const char *msg)
{
  if (! out || ! out->syncout)
    {
      FILE *f = is_err ? stderr : stdout;
      fputs (msg, f);
      fflush (f);
    }
  else
    {
      int fd = is_err ? out->err : out->out;
      size_t len = strlen (msg);
      int r;
      EINTRLOOP (r, lseek (fd, 0, SEEK_END));
      writebuf (fd, msg, len);
    }
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

/* Set a file descriptor to be in O_APPEND mode.
   If it fails, just ignore it.  */

static void
set_append_mode (int fd)
{
#if defined(F_GETFL) && defined(F_SETFL) && defined(O_APPEND)
  int flags = fcntl (fd, F_GETFL, 0);
  if (flags >= 0)
    {
      int r;
      EINTRLOOP(r, fcntl (fd, F_SETFL, flags | O_APPEND));
    }
#endif
}


#ifndef NO_OUTPUT_SYNC

/* Semaphore for use in -j mode with output_sync. */
static sync_handle_t sync_handle = -1;

#define FD_NOT_EMPTY(_f) ((_f) != OUTPUT_NONE && lseek ((_f), 0, SEEK_END) > 0)

/* Set up the sync handle.  Disables output_sync on error.  */
static int
sync_init (void)
{
  int combined_output = 0;

#ifdef WINDOWS32
  if ((!STREAM_OK (stdout) && !STREAM_OK (stderr))
      || (sync_handle = create_mutex ()) == -1)
    {
      perror_with_name ("output-sync suppressed: ", "stderr");
      output_sync = 0;
    }
  else
    {
      combined_output = same_stream (stdout, stderr);
      prepare_mutex_handle_string (sync_handle);
    }

#else
  if (STREAM_OK (stdout))
    {
      struct stat stbuf_o, stbuf_e;

      sync_handle = fileno (stdout);
      combined_output = (fstat (fileno (stdout), &stbuf_o) == 0
                         && fstat (fileno (stderr), &stbuf_e) == 0
                         && stbuf_o.st_dev == stbuf_e.st_dev
                         && stbuf_o.st_ino == stbuf_e.st_ino);
    }
  else if (STREAM_OK (stderr))
    sync_handle = fileno (stderr);
  else
    {
      perror_with_name ("output-sync suppressed: ", "stderr");
      output_sync = 0;
    }
#endif

  return combined_output;
}

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

/* Obtain the lock for writing output.  */
static void *
acquire_semaphore (void)
{
  static struct flock fl;

  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 1;
  if (fcntl (sync_handle, F_SETLKW, &fl) != -1)
    return &fl;
  perror ("fcntl()");
  return NULL;
}

/* Release the lock for writing output.  */
static void
release_semaphore (void *sem)
{
  struct flock *flp = (struct flock *)sem;
  flp->l_type = F_UNLCK;
  if (fcntl (sync_handle, F_SETLKW, flp) == -1)
    perror ("fcntl()");
}

/* Returns a file descriptor to a temporary file.  The file is automatically
   closed/deleted on exit.  Don't use a FILE* stream.  */
int
output_tmpfd (void)
{
  mode_t mask = umask (0077);
  int fd = -1;
  FILE *tfile = tmpfile ();

  if (! tfile)
    pfatal_with_name ("tmpfile");

  /* Create a duplicate so we can close the stream.  */
  fd = dup (fileno (tfile));
  if (fd < 0)
    pfatal_with_name ("dup");

  fclose (tfile);

  set_append_mode (fd);

  umask (mask);

  return fd;
}

/* Adds file descriptors to the child structure to support output_sync; one
   for stdout and one for stderr as long as they are open.  If stdout and
   stderr share a device they can share a temp file too.
   Will reset output_sync on error.  */
static void
setup_tmpfile (struct output *out)
{
  /* Is make's stdout going to the same place as stderr?  */
  static int combined_output = -1;

  if (combined_output < 0)
    combined_output = sync_init ();

  if (STREAM_OK (stdout))
    {
      int fd = output_tmpfd ();
      if (fd < 0)
        goto error;
      fd_noinherit (fd);
      out->out = fd;
    }

  if (STREAM_OK (stderr))
    {
      if (out->out != OUTPUT_NONE && combined_output)
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

  return;

  /* If we failed to create a temp file, disable output sync going forward.  */
 error:
  output_close (out);
  output_sync = OUTPUT_SYNC_NONE;
}

/* Synchronize the output of jobs in -j mode to keep the results of
   each job together. This is done by holding the results in temp files,
   one for stdout and potentially another for stderr, and only releasing
   them to "real" stdout/stderr when a semaphore can be obtained. */

void
output_dump (struct output *out)
{
  int outfd_not_empty = FD_NOT_EMPTY (out->out);
  int errfd_not_empty = FD_NOT_EMPTY (out->err);

  if (outfd_not_empty || errfd_not_empty)
    {
      int traced = 0;

      /* Try to acquire the semaphore.  If it fails, dump the output
         unsynchronized; still better than silently discarding it.
         We want to keep this lock for as little time as possible.  */
      void *sem = acquire_semaphore ();

      /* Log the working directory for this dump.  */
      if (print_directory_flag && output_sync != OUTPUT_SYNC_RECURSE)
        traced = log_working_directory (1);

      if (outfd_not_empty)
        pump_from_tmp (out->out, stdout);
      if (errfd_not_empty && out->err != out->out)
        pump_from_tmp (out->err, stderr);

      if (traced)
        log_working_directory (0);

      /* Exit the critical section.  */
      if (sem)
        release_semaphore (sem);

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


/* This code is stolen from gnulib.
   If/when we abandon the requirement to work with K&R compilers, we can
   remove this (and perhaps other parts of GNU make!) and migrate to using
   gnulib directly.

   This is called only through atexit(), which means die() has already been
   invoked.  So, call exit() here directly.  Apparently that works...?
*/

/* Close standard output, exiting with status 'exit_failure' on failure.
   If a program writes *anything* to stdout, that program should close
   stdout and make sure that it succeeds before exiting.  Otherwise,
   suppose that you go to the extreme of checking the return status
   of every function that does an explicit write to stdout.  The last
   printf can succeed in writing to the internal stream buffer, and yet
   the fclose(stdout) could still fail (due e.g., to a disk full error)
   when it tries to write out that buffered data.  Thus, you would be
   left with an incomplete output file and the offending program would
   exit successfully.  Even calling fflush is not always sufficient,
   since some file systems (NFS and CODA) buffer written/flushed data
   until an actual close call.

   Besides, it's wasteful to check the return value from every call
   that writes to stdout -- just let the internal stream state record
   the failure.  That's what the ferror test is checking below.

   It's important to detect such failures and exit nonzero because many
   tools (most notably 'make' and other build-management systems) depend
   on being able to detect failure in other tools via their exit status.  */

static void
close_stdout (void)
{
  int prev_fail = ferror (stdout);
  int fclose_fail = fclose (stdout);

  if (prev_fail || fclose_fail)
    {
      if (fclose_fail)
        perror_with_name (_("write error: stdout"), "");
      else
        O (error, NILF, _("write error: stdout"));
      exit (MAKE_TROUBLE);
    }
}


void
output_init (struct output *out)
{
  if (out)
    {
      out->out = out->err = OUTPUT_NONE;
      out->syncout = !!output_sync;
      return;
    }

  /* Configure this instance of make.  Be sure stdout is line-buffered.  */

#ifdef HAVE_SETVBUF
# ifdef SETVBUF_REVERSED
  setvbuf (stdout, _IOLBF, xmalloc (BUFSIZ), BUFSIZ);
# else  /* setvbuf not reversed.  */
  /* Some buggy systems lose if we pass 0 instead of allocating ourselves.  */
  setvbuf (stdout, 0, _IOLBF, BUFSIZ);
# endif /* setvbuf reversed.  */
#elif HAVE_SETLINEBUF
  setlinebuf (stdout);
#endif  /* setlinebuf missing.  */

  /* Force stdout/stderr into append mode.  This ensures parallel jobs won't
     lose output due to overlapping writes.  */
  set_append_mode (fileno (stdout));
  set_append_mode (fileno (stderr));

#ifdef HAVE_ATEXIT
  if (STREAM_OK (stdout))
    atexit (close_stdout);
#endif
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
    if (! stdio_traced && print_directory_flag)
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
  char *p;

  len += strlen (fmt) + strlen (program) + INTSTR_LENGTH + 4 + 1 + 1;
  p = get_buffer (len);

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

  assert (fmtbuf.buffer[len-1] == '\0');
  outputs (0, fmtbuf.buffer);
}

/* Print an error message.  */

void
error (const floc *flocp, size_t len, const char *fmt, ...)
{
  va_list args;
  char *p;

  len += (strlen (fmt) + strlen (program)
          + (flocp && flocp->filenm ? strlen (flocp->filenm) : 0)
          + INTSTR_LENGTH + 4 + 1 + 1);
  p = get_buffer (len);

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

  assert (fmtbuf.buffer[len-1] == '\0');
  outputs (1, fmtbuf.buffer);
}

/* Print an error message and exit.  */

void
fatal (const floc *flocp, size_t len, const char *fmt, ...)
{
  va_list args;
  const char *stop = _(".  Stop.\n");
  char *p;

  len += (strlen (fmt) + strlen (program)
          + (flocp && flocp->filenm ? strlen (flocp->filenm) : 0)
          + INTSTR_LENGTH + 8 + strlen (stop) + 1);
  p = get_buffer (len);

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

  assert (fmtbuf.buffer[len-1] == '\0');
  outputs (1, fmtbuf.buffer);

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
