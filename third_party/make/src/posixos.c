/* POSIX-based operating system interface for GNU Make.
Copyright (C) 2016-2020 Free Software Foundation, Inc.
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

#include <stdio.h>

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#elif defined(HAVE_SYS_FILE_H)
# include <sys/file.h>
#endif

#if defined(HAVE_PSELECT) && defined(HAVE_SYS_SELECT_H)
# include <sys/select.h>
#endif

#include "debug.h"
#include "job.h"
#include "os.h"

#ifdef MAKE_JOBSERVER

/* This section provides OS-specific functions to support the jobserver.  */

/* These track the state of the jobserver pipe.  Passed to child instances.  */
static int job_fds[2] = { -1, -1 };

/* Used to signal read() that a SIGCHLD happened.  Always CLOEXEC.
   If we use pselect() this will never be created and always -1.
 */
static int job_rfd = -1;

/* Token written to the pipe (could be any character...)  */
static char token = '+';

static int
make_job_rfd (void)
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
#endif
}

unsigned int
jobserver_setup (int slots)
{
  int r;

  EINTRLOOP (r, pipe (job_fds));
  if (r < 0)
    pfatal_with_name (_("creating jobs pipe"));

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

  return 1;
}

unsigned int
jobserver_parse_auth (const char *auth)
{
  /* Given the command-line parameter, parse it.  */
  if (sscanf (auth, "%d,%d", &job_fds[0], &job_fds[1]) != 2)
    OS (fatal, NILF,
        _("internal error: invalid --jobserver-auth string '%s'"), auth);

  DB (DB_JOBS,
      (_("Jobserver client (fds %d,%d)\n"), job_fds[0], job_fds[1]));

#ifdef HAVE_FCNTL_H
# define FD_OK(_f) (fcntl ((_f), F_GETFD) != -1)
#else
# define FD_OK(_f) 1
#endif

  /* Make sure our pipeline is valid, and (possibly) create a duplicate pipe,
     that will be closed in the SIGCHLD handler.  If this fails with EBADF,
     the parent has closed the pipe on us because it didn't think we were a
     submake.  If so, warn and default to -j1.  */

  if (!FD_OK (job_fds[0]) || !FD_OK (job_fds[1]) || make_job_rfd () < 0)
    {
      if (errno != EBADF)
        pfatal_with_name (_("jobserver pipeline"));

      job_fds[0] = job_fds[1] = -1;

      return 0;
    }

  /* When using pselect() we want the read to be non-blocking.  */
  set_blocking (job_fds[0], 0);

  return 1;
}

char *
jobserver_get_auth (void)
{
  char *auth = xmalloc ((INTSTR_LENGTH * 2) + 2);
  sprintf (auth, "%d,%d", job_fds[0], job_fds[1]);
  return auth;
}

unsigned int
jobserver_enabled (void)
{
  return job_fds[0] >= 0;
}

void
jobserver_clear (void)
{
  if (job_fds[0] >= 0)
    close (job_fds[0]);
  if (job_fds[1] >= 0)
    close (job_fds[1]);
  if (job_rfd >= 0)
    close (job_rfd);

  job_fds[0] = job_fds[1] = job_rfd = -1;
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
jobserver_acquire_all (void)
{
  unsigned int tokens = 0;

  /* Use blocking reads to wait for all outstanding jobs.  */
  set_blocking (job_fds[0], 1);

  /* Close the write side, so the read() won't hang forever.  */
  close (job_fds[1]);
  job_fds[1] = -1;

  while (1)
    {
      char intake;
      int r;
      EINTRLOOP (r, read (job_fds[0], &intake, 1));
      if (r != 1)
        return tokens;
      ++tokens;
    }
}

/* Prepare the jobserver to start a child process.  */
void
jobserver_pre_child (int recursive)
{
  if (recursive && job_fds[0] >= 0)
    {
      fd_inherit (job_fds[0]);
      fd_inherit (job_fds[1]);
    }
}

/* Reconfigure the jobserver after starting a child process.  */
void
jobserver_post_child (int recursive)
{
  if (recursive && job_fds[0] >= 0)
    {
      fd_noinherit (job_fds[0]);
      fd_noinherit (job_fds[1]);
    }
}

void
jobserver_signal (void)
{
  if (job_rfd >= 0)
    {
      close (job_rfd);
      job_rfd = -1;
    }
}

void
jobserver_pre_acquire (void)
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

      /* read() should never return 0: only the master make can reap all the
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

static RETSIGTYPE
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

  if (errno != EINTR && errno != EBADF)
    pfatal_with_name (_("read jobs pipe"));

  if (errno == EBADF)
    DB (DB_JOBS, ("Read returned EBADF.\n"));

  return 0;
}

#endif /* HAVE_PSELECT */

#endif /* MAKE_JOBSERVER */

/* Create a "bad" file descriptor for stdin when parallel jobs are run.  */
int
get_bad_stdin (void)
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
          (void) close (pd[1]);
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
    EINTRLOOP(flags, fcntl(fd, F_GETFD));
    if (flags >= 0)
      {
        int r;
        flags |= FD_CLOEXEC;
        EINTRLOOP(r, fcntl(fd, F_SETFD, flags));
      }
}
#endif
