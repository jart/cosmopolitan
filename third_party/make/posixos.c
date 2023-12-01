/* POSIX-based operating system interface for GNU Make.
Copyright (C) 2016-2023 Free Software Foundation, Inc.
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

#include "libc/dce.h"
#include "makeint.h"

#include <stdio.h>

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
# define FD_OK(_f) (fcntl ((_f), F_GETFD) != -1)
#elif defined(HAVE_SYS_FILE_H)
# include <sys/file.h>
#endif
#if MK_OS_ZOS
/* FIXME: HAVE_PSELECT path hangs on z/OS */
#undef HAVE_PSELECT
#endif

#if !defined(FD_OK)
# define FD_OK(_f) 1
#endif

#if defined(HAVE_PSELECT) && defined(HAVE_SYS_SELECT_H)
# include <sys/select.h>
#endif

#include "debug.h"
#include "job.h"
#include "os.h"

#define STREAM_OK(_s) ((fcntl (fileno (_s), F_GETFD) != -1) || (errno != EBADF))

unsigned int
check_io_state ()
{
  static unsigned int state = IO_UNKNOWN;

  /* We only need to compute this once per process.  */
  if (state != IO_UNKNOWN)
    return state;

  if (STREAM_OK (stdin))
    state |= IO_STDIN_OK;
  if (STREAM_OK (stdout))
    state |= IO_STDOUT_OK;
  if (STREAM_OK (stderr))
    state |= IO_STDERR_OK;

  if (ALL_SET (state, IO_STDOUT_OK|IO_STDERR_OK))
    {
      struct stat stbuf_o, stbuf_e;

      if (fstat (fileno (stdout), &stbuf_o) == 0
          && fstat (fileno (stderr), &stbuf_e) == 0
          && stbuf_o.st_dev == stbuf_e.st_dev
          && stbuf_o.st_ino == stbuf_e.st_ino)
        state |= IO_COMBINED_OUTERR;
    }

  return state;
}

#if defined(MAKE_JOBSERVER)

#define FIFO_PREFIX    "fifo:"

/* This section provides OS-specific functions to support the jobserver.  */

/* True if this is the root make instance.  */
static unsigned char job_root = 0;

/* These track the state of the jobserver pipe.  Passed to child instances.  */
static int job_fds[2] = { -1, -1 };

/* Used to signal read() that a SIGCHLD happened.  Always CLOEXEC.
   If we use pselect() this will never be created and always -1.
 */
static int job_rfd = -1;

/* Token written to the pipe (could be any character...)  */
static char token = '+';

/* The type of jobserver we're using.  */
enum js_type
  {
    js_none = 0,        /* No jobserver.  */
    js_pipe,            /* Use a simple pipe as the jobserver.  */
    js_fifo             /* Use a named pipe as the jobserver.  */
  };

static enum js_type js_type = js_none;

/* The name of the named pipe (if used).  */
static char *fifo_name = NULL;

static int
make_job_rfd ()
{
#ifdef HAVE_PSELECT
  /* Pretend we succeeded.  */
  return 0;
#else
  EINTRLOOP (job_rfd, dup (job_fds[0]));
  if (job_rfd >= 0)
    fd_noinherit (job_rfd);

  return job_rfd;
#endif
}

static void
set_blocking (int fd, int blocking)
{
  /* If we're not using pselect() don't change the blocking.  */
#ifdef HAVE_PSELECT
  int flags;
  EINTRLOOP (flags, fcntl (fd, F_GETFL));
  if (flags >= 0)
    {
      int r;
      flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
      EINTRLOOP (r, fcntl (fd, F_SETFL, flags));
      if (r < 0)
        pfatal_with_name ("fcntl(O_NONBLOCK)");
    }
#else
  (void) fd;
  (void) blocking;
#endif
}

unsigned int
jobserver_setup (int slots, const char *style)
{
  int r;

#if JOBSERVER_USE_FIFO
  if (!style || strcmp (style, "fifo") == 0)
    {
  /* Unfortunately glibc warns about uses of mktemp even though we aren't
     using it in dangerous way here.  So avoid this by generating our own
     temporary file name.  */
# define  FNAME_PREFIX "GMfifo"
      const char *tmpdir = get_tmpdir ();

      fifo_name = xmalloc (strlen (tmpdir) + CSTRLEN (FNAME_PREFIX)
                           + INTSTR_LENGTH + 2);
      sprintf (fifo_name, "%s/" FNAME_PREFIX "%" MK_PRI64_PREFIX "d",
               tmpdir, (long long)make_pid ());

      EINTRLOOP (r, mkfifo (fifo_name, 0600));
      if (r < 0)
        {
          perror_with_name("jobserver mkfifo: ", fifo_name);
          free (fifo_name);
          fifo_name = NULL;
        }
      else
        {
          /* We have to open the read side in non-blocking mode, else it will
             hang until the write side is open.  */
          EINTRLOOP (job_fds[0], open (fifo_name, O_RDONLY|O_NONBLOCK));
          if (job_fds[0] < 0)
            OSS (fatal, NILF, _("cannot open jobserver %s: %s"),
                 fifo_name, strerror (errno));

          EINTRLOOP (job_fds[1], open (fifo_name, O_WRONLY));
          if (job_fds[0] < 0)
            OSS (fatal, NILF, _("cannot open jobserver %s: %s"),
                 fifo_name, strerror (errno));

          js_type = js_fifo;
        }
    }
#endif

  if (js_type == js_none)
    {
      if (style && strcmp (style, "pipe") != 0)
        OS (fatal, NILF, _("unknown jobserver auth style '%s'"), style);

      EINTRLOOP (r, pipe (job_fds));
      if (r < 0)
        pfatal_with_name (_("creating jobs pipe"));

      js_type = js_pipe;
    }

  /* By default we don't send the job pipe FDs to our children.
     See jobserver_pre_child() and jobserver_post_child().  */
  fd_noinherit (job_fds[0]);
  fd_noinherit (job_fds[1]);

  if (make_job_rfd () < 0)
    pfatal_with_name (_("duping jobs pipe"));

  while (slots--)
    {
      EINTRLOOP (r, write (job_fds[1], &token, 1));
      if (r != 1)
        pfatal_with_name (_("init jobserver pipe"));
    }

  /* When using pselect() we want the read to be non-blocking.  */
  set_blocking (job_fds[0], 0);

  job_root = 1;

  return 1;
}

unsigned int
jobserver_parse_auth (const char *auth)
{
  int rfd, wfd;

  /* Given the command-line parameter, parse it.  */

  /* First see if we're using a named pipe.  */
  if (strncmp (auth, FIFO_PREFIX, CSTRLEN (FIFO_PREFIX)) == 0)
    {
      fifo_name = xstrdup (auth + CSTRLEN (FIFO_PREFIX));

      EINTRLOOP (job_fds[0], open (fifo_name, O_RDONLY));
      if (job_fds[0] < 0)
        {
          OSS (error, NILF,
               _("cannot open jobserver %s: %s"), fifo_name, strerror (errno));
          return 0;
        }

      EINTRLOOP (job_fds[1], open (fifo_name, O_WRONLY));
      if (job_fds[1] < 0)
        {
          OSS (error, NILF,
               _("cannot open jobserver %s: %s"), fifo_name, strerror (errno));
          return 0;
        }
      js_type = js_fifo;
    }
  /* If not, it must be a simple pipe.  */
  else if (sscanf (auth, "%d,%d", &rfd, &wfd) == 2)
    {
      /* The parent overrode our FDs because we aren't a recursive make.  */
      if (rfd == -2 || wfd == -2)
        return 0;

      /* Make sure our pipeline is valid.  */
      if (!FD_OK (rfd) || !FD_OK (wfd))
        return 0;

      job_fds[0] = rfd;
      job_fds[1] = wfd;

      js_type = js_pipe;
    }
  /* Who knows what it is?  */
  else
    {
      OS (error, NILF, _("invalid --jobserver-auth string '%s'"), auth);
      return 0;
    }

  /* Create a duplicate pipe, if needed, that will be closed in the SIGCHLD
     handler.  If this fails with EBADF, the parent closed the pipe on us as
     it didn't think we were a submake.  If so, warn and default to -j1.  */

  if (make_job_rfd () < 0)
    {
      if (errno != EBADF)
        pfatal_with_name ("jobserver readfd");

      jobserver_clear ();

      return 0;
    }

  /* When using pselect() we want the read to be non-blocking.  */
  set_blocking (job_fds[0], 0);

  /* By default we don't send the job pipe FDs to our children.
     See jobserver_pre_child() and jobserver_post_child().  */
  fd_noinherit (job_fds[0]);
  fd_noinherit (job_fds[1]);

  return 1;
}

char *
jobserver_get_auth ()
{
  char *auth;

  if (js_type == js_fifo) {
    auth = xmalloc (strlen (fifo_name) + CSTRLEN (FIFO_PREFIX) + 1);
    sprintf (auth, FIFO_PREFIX "%s", fifo_name);
  } else {
    auth = xmalloc ((INTSTR_LENGTH * 2) + 2);
    sprintf (auth, "%d,%d", job_fds[0], job_fds[1]);
  }

  return auth;
}

const char *
jobserver_get_invalid_auth ()
{
  /* If we're using a named pipe we don't need to invalidate the jobserver.  */
  if (js_type == js_fifo) {
    return NULL;
  }

  /* It's not really great that we are assuming the command line option
     here but other alternatives are also gross.  */
  return " --" JOBSERVER_AUTH_OPT "=-2,-2";
}

unsigned int
jobserver_enabled ()
{
  return js_type != js_none;
}

void
jobserver_clear ()
{
  if (job_fds[0] >= 0)
    close (job_fds[0]);
  if (job_fds[1] >= 0)
    close (job_fds[1]);
  if (job_rfd >= 0)
    close (job_rfd);

  job_fds[0] = job_fds[1] = job_rfd = -1;

  if (fifo_name)
    {
      if (job_root)
        {
          int r;
          EINTRLOOP (r, unlink (fifo_name));
        }

      if (!handling_fatal_signal)
        {
          free (fifo_name);
          fifo_name = NULL;
        }
    }

  js_type = js_none;
}

void
jobserver_release (int is_fatal)
{
  int r;
  EINTRLOOP (r, write (job_fds[1], &token, 1));
  if (r != 1)
    {
      if (is_fatal)
        pfatal_with_name (_("write jobserver"));
      perror_with_name ("write", "");
    }
}

unsigned int
jobserver_acquire_all ()
{
  int r;
  unsigned int tokens = 0;

  /* Use blocking reads to wait for all outstanding jobs.  */
  set_blocking (job_fds[0], 1);

  /* Close the write side, so the read() won't hang forever.  */
  close (job_fds[1]);
  job_fds[1] = -1;

  while (1)
    {
      char intake;
      EINTRLOOP (r, read (job_fds[0], &intake, 1));
      if (r != 1)
        break;
      ++tokens;
    }

  DB (DB_JOBS, ("Acquired all %u jobserver tokens.\n", tokens));

  jobserver_clear ();

  return tokens;
}

/* Prepare the jobserver to start a child process.  */
void
jobserver_pre_child (int recursive)
{
  if (recursive && js_type == js_pipe)
    {
      fd_inherit (job_fds[0]);
      fd_inherit (job_fds[1]);
    }
}

/* Reconfigure the jobserver after starting a child process.  */
void
jobserver_post_child (int recursive)
{
  if (recursive && js_type == js_pipe)
    {
      fd_noinherit (job_fds[0]);
      fd_noinherit (job_fds[1]);
    }
}

void
jobserver_signal ()
{
  if (job_rfd >= 0)
    {
      close (job_rfd);
      job_rfd = -1;
    }
}

void
jobserver_pre_acquire ()
{
  /* Make sure we have a dup'd FD.  */
  if (job_rfd < 0 && job_fds[0] >= 0 && make_job_rfd () < 0)
    pfatal_with_name (_("duping jobs pipe"));
}

#ifdef HAVE_PSELECT

/* Use pselect() to atomically wait for both a signal and a file descriptor.
   It also provides a timeout facility so we don't need to use SIGALRM.

   This method relies on the fact that SIGCHLD will be blocked everywhere,
   and only unblocked (atomically) within the pselect() call, so we can
   never miss a SIGCHLD.
 */
unsigned int
jobserver_acquire (int timeout)
{
  struct timespec spec;
  struct timespec *specp = NULL;
  sigset_t empty;

  sigemptyset (&empty);

  if (timeout)
    {
      /* Alarm after one second (is this too granular?)  */
      spec.tv_sec = 1;
      spec.tv_nsec = 0;
      specp = &spec;
    }

  while (1)
    {
      fd_set readfds;
      int r;
      char intake;

      FD_ZERO (&readfds);
      FD_SET (job_fds[0], &readfds);

      r = pselect (job_fds[0]+1, &readfds, NULL, NULL, specp, &empty);
      if (r < 0)
        switch (errno)
          {
          case EINTR:
            /* SIGCHLD will show up as an EINTR.  */
            return 0;

          case EBADF:
            /* Someone closed the jobs pipe.
               That shouldn't happen but if it does we're done.  */
            O (fatal, NILF, _("job server shut down"));

          default:
            pfatal_with_name (_("pselect jobs pipe"));
          }

      if (r == 0)
        /* Timeout.  */
        return 0;

      /* The read FD is ready: read it!  This is non-blocking.  */
      EINTRLOOP (r, read (job_fds[0], &intake, 1));

      if (r < 0)
        {
          /* Someone sniped our token!  Try again.  */
          if (errno == EAGAIN)
            continue;

          pfatal_with_name (_("read jobs pipe"));
        }

      /* read() should never return 0: only the parent make can reap all the
         tokens and close the write side...??  */
      return r > 0;
    }
}

#else

/* This method uses a "traditional" UNIX model for waiting on both a signal
   and a file descriptor.  However, it's complex and since we have a SIGCHLD
   handler installed we need to check ALL system calls for EINTR: painful!

   Read a token.  As long as there's no token available we'll block.  We
   enable interruptible system calls before the read(2) so that if we get a
   SIGCHLD while we're waiting, we'll return with EINTR and we can process the
   death(s) and return tokens to the free pool.

   Once we return from the read, we immediately reinstate restartable system
   calls.  This allows us to not worry about checking for EINTR on all the
   other system calls in the program.

   There is one other twist: there is a span between the time reap_children()
   does its last check for dead children and the time the read(2) call is
   entered, below, where if a child dies we won't notice.  This is extremely
   serious as it could cause us to deadlock, given the right set of events.

   To avoid this, we do the following: before we reap_children(), we dup(2)
   the read FD on the jobserver pipe.  The read(2) call below uses that new
   FD.  In the signal handler, we close that FD.  That way, if a child dies
   during the section mentioned above, the read(2) will be invoked with an
   invalid FD and will return immediately with EBADF.  */

static void
job_noop (int sig UNUSED)
{
}

/* Set the child handler action flags to FLAGS.  */
static void
set_child_handler_action_flags (int set_handler, int set_alarm)
{
  struct sigaction sa;

#ifdef __EMX__
  /* The child handler must be turned off here.  */
  signal (SIGCHLD, SIG_DFL);
#endif

  memset (&sa, '\0', sizeof sa);
  sa.sa_handler = child_handler;
  sa.sa_flags = set_handler ? 0 : SA_RESTART;

#if defined SIGCHLD
  if (sigaction (SIGCHLD, &sa, NULL) < 0)
    pfatal_with_name ("sigaction: SIGCHLD");
#endif

#if defined SIGCLD && SIGCLD != SIGCHLD
  if (sigaction (SIGCLD, &sa, NULL) < 0)
    pfatal_with_name ("sigaction: SIGCLD");
#endif

#if defined SIGALRM
  if (set_alarm)
    {
      /* If we're about to enter the read(), set an alarm to wake up in a
         second so we can check if the load has dropped and we can start more
         work.  On the way out, turn off the alarm and set SIG_DFL.  */
      if (set_handler)
        {
          sa.sa_handler = job_noop;
          sa.sa_flags = 0;
          if (sigaction (SIGALRM, &sa, NULL) < 0)
            pfatal_with_name ("sigaction: SIGALRM");
          alarm (1);
        }
      else
        {
          alarm (0);
          sa.sa_handler = SIG_DFL;
          sa.sa_flags = 0;
          if (sigaction (SIGALRM, &sa, NULL) < 0)
            pfatal_with_name ("sigaction: SIGALRM");
        }
    }
#endif
}

unsigned int
jobserver_acquire (int timeout)
{
  char intake;
  int got_token;
  int saved_errno;

  /* Set interruptible system calls, and read() for a job token.  */
  set_child_handler_action_flags (1, timeout);

  EINTRLOOP (got_token, read (job_rfd, &intake, 1));
  saved_errno = errno;

  set_child_handler_action_flags (0, timeout);

  if (got_token == 1)
    return 1;

  /* If the error _wasn't_ expected (EINTR or EBADF), fatal.  Otherwise,
     go back and reap_children(), and try again.  */
  errno = saved_errno;

  if (errno != EINTR && errno != EBADF && errno != EAGAIN)
    pfatal_with_name (_("read jobs pipe"));

  if (errno == EBADF)
    DB (DB_JOBS, ("Read returned EBADF.\n"));

  return 0;
}

#endif /* HAVE_PSELECT */

#endif /* MAKE_JOBSERVER */

#if !defined(NO_OUTPUT_SYNC)

#define MUTEX_PREFIX    "fnm:"

static int osync_handle = -1;

static char *osync_tmpfile = NULL;

static unsigned int sync_root = 0;

unsigned int
osync_enabled ()
{
  return osync_handle >= 0;
}

void
osync_setup ()
{
  osync_handle = get_tmpfd (&osync_tmpfile);
  fd_noinherit (osync_handle);
  sync_root = 1;
}

char *
osync_get_mutex ()
{
  char *mutex = NULL;

  if (osync_enabled ())
    {
      /* Prepare the mutex handle string for our children.  */
      mutex = xmalloc (strlen (osync_tmpfile) + CSTRLEN (MUTEX_PREFIX) + 1);
      sprintf (mutex, MUTEX_PREFIX "%s", osync_tmpfile);
    }

  return mutex;
}

unsigned int
osync_parse_mutex (const char *mutex)
{
  if (strncmp (mutex, MUTEX_PREFIX, CSTRLEN (MUTEX_PREFIX)) != 0)
    {
      OS (error, NILF, _("invalid --sync-mutex string '%s'"), mutex);
      return 0;
    }

  free (osync_tmpfile);
  osync_tmpfile = xstrdup (mutex + CSTRLEN (MUTEX_PREFIX));

  EINTRLOOP (osync_handle, open (osync_tmpfile, O_WRONLY));
  if (osync_handle < 0)
    OSS (fatal, NILF, _("cannot open output sync mutex %s: %s"),
         osync_tmpfile, strerror (errno));

  fd_noinherit (osync_handle);

  return 1;
}

void
osync_clear ()
{
  if (osync_handle >= 0)
    {
      close (osync_handle);
      osync_handle = -1;
    }

  if (sync_root && osync_tmpfile)
    {
      int r;

      EINTRLOOP (r, unlink (osync_tmpfile));
      free (osync_tmpfile);
      osync_tmpfile = NULL;
    }
}

unsigned int
osync_acquire ()
{
  if (osync_enabled())
    {
      struct flock fl;

      fl.l_type = F_WRLCK;
      fl.l_whence = SEEK_SET;
      fl.l_start = 0;
      fl.l_len = 1;
      /* We don't want to keep waiting on EINTR.  */
      if (fcntl (osync_handle, F_SETLKW, &fl) == -1)
        {
          perror ("fcntl()");
          return 0;
        }
    }

  return 1;
}

void
osync_release ()
{
  if (osync_enabled())
    {
      struct flock fl;

      fl.l_type = F_UNLCK;
      fl.l_whence = SEEK_SET;
      fl.l_start = 0;
      fl.l_len = 1;
      /* We don't want to keep waiting on EINTR.  */
      if (fcntl (osync_handle, F_SETLKW, &fl) == -1)
        perror ("fcntl()");
    }
}

#endif

/* Create a "bad" file descriptor for stdin when parallel jobs are run.  */
int
get_bad_stdin ()
{
  static int bad_stdin = -1;

  /* Set up a bad standard input that reads from a broken pipe.  */

  if (bad_stdin == -1)
    {
      /* Make a file descriptor that is the read end of a broken pipe.
         This will be used for some children's standard inputs.  */
      int pd[2];
      if (pipe (pd) == 0)
        {
          /* Close the write side.  */
          close (pd[1]);
          /* Save the read side.  */
          bad_stdin = pd[0];

          /* Set the descriptor to close on exec, so it does not litter any
             child's descriptor table.  When it is dup2'd onto descriptor 0,
             that descriptor will not close on exec.  */
          fd_noinherit (bad_stdin);
        }
    }

  return bad_stdin;
}

/* Set file descriptors to be inherited / not inherited by subprocesses.  */

#if !defined(F_SETFD) || !defined(F_GETFD)
void fd_inherit (int fd) {}
void fd_noinherit (int fd) {}

#else

# ifndef FD_CLOEXEC
#  define FD_CLOEXEC 1
# endif

void
fd_inherit (int fd)
{
  int flags;
  EINTRLOOP (flags, fcntl (fd, F_GETFD));
  if (flags >= 0)
    {
      int r;
      flags &= ~FD_CLOEXEC;
      EINTRLOOP (r, fcntl (fd, F_SETFD, flags));
    }
}

void
fd_noinherit (int fd)
{
    int flags;
    EINTRLOOP (flags, fcntl(fd, F_GETFD));
    if (flags >= 0)
      {
        int r;
        flags |= FD_CLOEXEC;
        EINTRLOOP (r, fcntl(fd, F_SETFD, flags));
      }
}
#endif

/* Set a file descriptor referring to a regular file to be in O_APPEND mode.
   If it fails, just ignore it.  */

void
fd_set_append (int fd)
{
#if defined(F_GETFL) && defined(F_SETFL) && defined(O_APPEND)
  struct stat stbuf;
  int flags;
  if (fstat (fd, &stbuf) == 0 && S_ISREG (stbuf.st_mode))
    {
      flags = fcntl (fd, F_GETFL, 0);
      if (flags >= 0)
        {
          int r;
          EINTRLOOP(r, fcntl (fd, F_SETFL, flags | O_APPEND));
        }
    }
#endif
}

/* Return a file descriptor for a new anonymous temp file, or -1.  */
int
os_anontmp ()
{
  const char *tdir = get_tmpdir ();
  int fd = -1;

  if (IsLinux ())
    {
      static unsigned int tmpfile_works = 1;

      if (tmpfile_works)
        {
          EINTRLOOP (fd, open (tdir, O_RDWR | O_TMPFILE | O_EXCL, 0600));
          if (fd >= 0)
            return fd;
    
          DB (DB_BASIC, (_("Cannot open '%s' with O_TMPFILE: %s.\n"),
                         tdir, strerror (errno)));
          tmpfile_works = 0;
        }
    }

#if HAVE_DUP
  /* If we can dup and we are creating temp files in the default location then
     try tmpfile() + dup() + fclose() to avoid ever having a named file.  */
  if (streq (tdir, DEFAULT_TMPDIR))
    {
      mode_t mask = umask (0077);
      FILE *tfile;
      ENULLLOOP (tfile, tmpfile ());
      if (!tfile)
        {
          OS (error, NILF, "tmpfile: %s", strerror (errno));
          return -1;
        }
      umask (mask);

      EINTRLOOP (fd, dup (fileno (tfile)));
      if (fd < 0)
        OS (error, NILF, "dup: %s", strerror (errno));
      fclose (tfile);
    }
#endif

  return fd;
}
