/* Job execution and handling for GNU Make.
Copyright (C) 1988-2020 Free Software Foundation, Inc.
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

#include <assert.h>
#include <string.h>

#include "job.h"
#include "debug.h"
#include "filedef.h"
#include "commands.h"
#include "variable.h"
#include "os.h"

/* Default shell to use.  */
#ifdef WINDOWS32
# ifdef HAVE_STRINGS_H
#  include <strings.h>	/* for strcasecmp, strncasecmp */
# endif
# include <windows.h>

const char *default_shell = "sh.exe";
int no_default_sh_exe = 1;
int batch_mode_shell = 1;
HANDLE main_thread;

#elif defined (_AMIGA)

const char *default_shell = "";
extern int MyExecute (char **);
int batch_mode_shell = 0;

#elif defined (__MSDOS__)

/* The default shell is a pointer so we can change it if Makefile
   says so.  It is without an explicit path so we get a chance
   to search the $PATH for it (since MSDOS doesn't have standard
   directories we could trust).  */
const char *default_shell = "command.com";
int batch_mode_shell = 0;

#elif defined (__EMX__)

const char *default_shell = "/bin/sh";
int batch_mode_shell = 0;

#elif defined (VMS)

# include <descrip.h>
# include <stsdef.h>
const char *default_shell = "";
int batch_mode_shell = 0;

#define strsignal vms_strsignal
char * vms_strsignal (int status);

#ifndef C_FACILITY_NO
# define C_FACILITY_NO 0x350000
#endif
#ifndef VMS_POSIX_EXIT_MASK
# define VMS_POSIX_EXIT_MASK (C_FACILITY_NO | 0xA000)
#endif

#else

const char *default_shell = "/bin/sh";
int batch_mode_shell = 0;

#endif

#ifdef __MSDOS__
# include <process.h>
static int execute_by_shell;
static int dos_pid = 123;
int dos_status;
int dos_command_running;
#endif /* __MSDOS__ */

#ifdef _AMIGA
# include <proto/dos.h>
static int amiga_pid = 123;
static int amiga_status;
static char amiga_bname[32];
static int amiga_batch_file;
#endif /* Amiga.  */

#ifdef VMS
# ifndef __GNUC__
#   include <processes.h>
# endif
# include <starlet.h>
# include <lib$routines.h>
static void vmsWaitForChildren (int *);
#endif

#ifdef WINDOWS32
# include <windows.h>
# include <io.h>
# include <process.h>
# include "sub_proc.h"
# include "w32err.h"
# include "pathstuff.h"
# define WAIT_NOHANG 1
#endif /* WINDOWS32 */

#ifdef __EMX__
# include <process.h>
#endif

#if defined (HAVE_SYS_WAIT_H) || defined (HAVE_UNION_WAIT)
# include <sys/wait.h>
#endif

#ifdef HAVE_WAITPID
# define WAIT_NOHANG(status)    waitpid (-1, (status), WNOHANG)
#else   /* Don't have waitpid.  */
# ifdef HAVE_WAIT3
#  ifndef wait3
extern int wait3 ();
#  endif
#  define WAIT_NOHANG(status)   wait3 ((status), WNOHANG, (struct rusage *) 0)
# endif /* Have wait3.  */
#endif /* Have waitpid.  */

#ifdef USE_POSIX_SPAWN
# include <spawn.h>
# include "findprog.h"
#endif

#if !defined (wait) && !defined (POSIX)
int wait ();
#endif

#ifndef HAVE_UNION_WAIT

# define WAIT_T int

# ifndef WTERMSIG
#  define WTERMSIG(x) ((x) & 0x7f)
# endif
# ifndef WCOREDUMP
#  define WCOREDUMP(x) ((x) & 0x80)
# endif
# ifndef WEXITSTATUS
#  define WEXITSTATUS(x) (((x) >> 8) & 0xff)
# endif
# ifndef WIFSIGNALED
#  define WIFSIGNALED(x) (WTERMSIG (x) != 0)
# endif
# ifndef WIFEXITED
#  define WIFEXITED(x) (WTERMSIG (x) == 0)
# endif

#else   /* Have 'union wait'.  */

# define WAIT_T union wait
# ifndef WTERMSIG
#  define WTERMSIG(x) ((x).w_termsig)
# endif
# ifndef WCOREDUMP
#  define WCOREDUMP(x) ((x).w_coredump)
# endif
# ifndef WEXITSTATUS
#  define WEXITSTATUS(x) ((x).w_retcode)
# endif
# ifndef WIFSIGNALED
#  define WIFSIGNALED(x) (WTERMSIG(x) != 0)
# endif
# ifndef WIFEXITED
#  define WIFEXITED(x) (WTERMSIG(x) == 0)
# endif

#endif  /* Don't have 'union wait'.  */

#if !defined(HAVE_UNISTD_H) && !defined(WINDOWS32)
int dup2 ();
int execve ();
void _exit ();
# ifndef VMS
int geteuid ();
int getegid ();
int setgid ();
int getgid ();
# endif
#endif

/* Different systems have different requirements for pid_t.
   Plus we have to support gettext string translation... Argh.  */
static const char *
pid2str (pid_t pid)
{
  static char pidstring[100];
#if defined(WINDOWS32) && (__GNUC__ > 3 || _MSC_VER > 1300)
  /* %Id is only needed for 64-builds, which were not supported by
      older versions of Windows compilers.  */
  sprintf (pidstring, "%Id", pid);
#else
  sprintf (pidstring, "%lu", (unsigned long) pid);
#endif
  return pidstring;
}

#ifndef HAVE_GETLOADAVG
int getloadavg (double loadavg[], int nelem);
#endif

static void free_child (struct child *);
static void start_job_command (struct child *child);
static int load_too_high (void);
static int job_next_command (struct child *);
static int start_waiting_job (struct child *);

/* Chain of all live (or recently deceased) children.  */

struct child *children = 0;

/* Number of children currently running.  */

unsigned int job_slots_used = 0;

/* Nonzero if the 'good' standard input is in use.  */

static int good_stdin_used = 0;

/* Chain of children waiting to run until the load average goes down.  */

static struct child *waiting_jobs = 0;

/* Non-zero if we use a *real* shell (always so on Unix).  */

int unixy_shell = 1;

/* Number of jobs started in the current second.  */

unsigned long job_counter = 0;

/* Number of jobserver tokens this instance is currently using.  */

unsigned int jobserver_tokens = 0;


#ifdef WINDOWS32
/*
 * The macro which references this function is defined in makeint.h.
 */
int
w32_kill (pid_t pid, int sig)
{
  return ((process_kill ((HANDLE)pid, sig) == TRUE) ? 0 : -1);
}

/* This function creates a temporary file name with an extension specified
 * by the unixy arg.
 * Return an xmalloc'ed string of a newly created temp file and its
 * file descriptor, or die.  */
static char *
create_batch_file (char const *base, int unixy, int *fd)
{
  const char *const ext = unixy ? "sh" : "bat";
  const char *error_string = NULL;
  char temp_path[MAXPATHLEN]; /* need to know its length */
  unsigned path_size = GetTempPath (sizeof temp_path, temp_path);
  int path_is_dot = 0;
  /* The following variable is static so we won't try to reuse a name
     that was generated a little while ago, because that file might
     not be on disk yet, since we use FILE_ATTRIBUTE_TEMPORARY below,
     which tells the OS it doesn't need to flush the cache to disk.
     If the file is not yet on disk, we might think the name is
     available, while it really isn't.  This happens in parallel
     builds, where Make doesn't wait for one job to finish before it
     launches the next one.  */
  static unsigned uniq = 0;
  static int second_loop = 0;
  const size_t sizemax = strlen (base) + strlen (ext) + 10;

  if (path_size == 0)
    {
      path_size = GetCurrentDirectory (sizeof temp_path, temp_path);
      path_is_dot = 1;
    }

  ++uniq;
  if (uniq >= 0x10000 && !second_loop)
    {
      /* If we already had 64K batch files in this
         process, make a second loop through the numbers,
         looking for free slots, i.e. files that were
         deleted in the meantime.  */
      second_loop = 1;
      uniq = 1;
    }
  while (path_size > 0 &&
         path_size + sizemax < sizeof temp_path &&
         !(uniq >= 0x10000 && second_loop))
    {
      unsigned size = sprintf (temp_path + path_size,
                               "%s%s-%x.%s",
                               temp_path[path_size - 1] == '\\' ? "" : "\\",
                               base, uniq, ext);
      HANDLE h = CreateFile (temp_path,  /* file name */
                             GENERIC_READ | GENERIC_WRITE, /* desired access */
                             0,                            /* no share mode */
                             NULL,                         /* default security attributes */
                             CREATE_NEW,                   /* creation disposition */
                             FILE_ATTRIBUTE_NORMAL |       /* flags and attributes */
                             FILE_ATTRIBUTE_TEMPORARY,     /* we'll delete it */
                             NULL);                        /* no template file */

      if (h == INVALID_HANDLE_VALUE)
        {
          const DWORD er = GetLastError ();

          if (er == ERROR_FILE_EXISTS || er == ERROR_ALREADY_EXISTS)
            {
              ++uniq;
              if (uniq == 0x10000 && !second_loop)
                {
                  second_loop = 1;
                  uniq = 1;
                }
            }

          /* the temporary path is not guaranteed to exist */
          else if (path_is_dot == 0)
            {
              path_size = GetCurrentDirectory (sizeof temp_path, temp_path);
              path_is_dot = 1;
            }

          else
            {
              error_string = map_windows32_error_to_string (er);
              break;
            }
        }
      else
        {
          const unsigned final_size = path_size + size + 1;
          char *const path = xmalloc (final_size);
          memcpy (path, temp_path, final_size);
          *fd = _open_osfhandle ((intptr_t)h, 0);
          if (unixy)
            {
              char *p;
              int ch;
              for (p = path; (ch = *p) != 0; ++p)
                if (ch == '\\')
                  *p = '/';
            }
          return path; /* good return */
        }
    }

  *fd = -1;
  if (error_string == NULL)
    error_string = _("Cannot create a temporary file\n");
  O (fatal, NILF, error_string);

  /* not reached */
  return NULL;
}
#endif /* WINDOWS32 */

#ifdef __EMX__
/* returns whether path is assumed to be a unix like shell. */
int
_is_unixy_shell (const char *path)
{
  /* list of non unix shells */
  const char *known_os2shells[] = {
    "cmd.exe",
    "cmd",
    "4os2.exe",
    "4os2",
    "4dos.exe",
    "4dos",
    "command.com",
    "command",
    NULL
  };

  /* find the rightmost '/' or '\\' */
  const char *name = strrchr (path, '/');
  const char *p = strrchr (path, '\\');
  unsigned i;

  if (name && p)    /* take the max */
    name = (name > p) ? name : p;
  else if (p)       /* name must be 0 */
    name = p;
  else if (!name)   /* name and p must be 0 */
    name = path;

  if (*name == '/' || *name == '\\') name++;

  i = 0;
  while (known_os2shells[i] != NULL)
    {
      if (strcasecmp (name, known_os2shells[i]) == 0)
        return 0; /* not a unix shell */
      i++;
    }

  /* in doubt assume a unix like shell */
  return 1;
}
#endif /* __EMX__ */

/* determines whether path looks to be a Bourne-like shell. */
int
is_bourne_compatible_shell (const char *path)
{
  /* List of known POSIX (or POSIX-ish) shells.  */
  static const char *unix_shells[] = {
    "sh",
    "bash",
    "ksh",
    "rksh",
    "zsh",
    "ash",
    "dash",
    NULL
  };
  const char **s;

  /* find the rightmost '/' or '\\' */
  const char *name = strrchr (path, '/');
  char *p = strrchr (path, '\\');

  if (name && p)    /* take the max */
    name = (name > p) ? name : p;
  else if (p)       /* name must be 0 */
    name = p;
  else if (!name)   /* name and p must be 0 */
    name = path;

  if (*name == '/' || *name == '\\')
    ++name;

  /* this should be able to deal with extensions on Windows-like systems */
  for (s = unix_shells; *s != NULL; ++s)
    {
#if defined(WINDOWS32) || defined(__MSDOS__)
      size_t len = strlen (*s);
      if ((strlen (name) >= len && STOP_SET (name[len], MAP_DOT|MAP_NUL))
          && strncasecmp (name, *s, len) == 0)
#else
      if (strcmp (name, *s) == 0)
#endif
        return 1; /* a known unix-style shell */
    }

  /* if not on the list, assume it's not a Bourne-like shell */
  return 0;
}

#ifdef POSIX
extern sigset_t fatal_signal_set;

static void
block_sigs ()
{
  sigprocmask (SIG_BLOCK, &fatal_signal_set, (sigset_t *) 0);
}

static void
unblock_sigs ()
{
  sigprocmask (SIG_UNBLOCK, &fatal_signal_set, (sigset_t *) 0);
}

void
unblock_all_sigs ()
{
  sigset_t empty;
  sigemptyset (&empty);
  sigprocmask (SIG_SETMASK, &empty, (sigset_t *) 0);
}

#elif defined(HAVE_SIGSETMASK)

extern int fatal_signal_mask;

static void
block_sigs ()
{
  sigblock (fatal_signal_mask);
}

static void
unblock_sigs ()
{
  sigsetmask (siggetmask (0) & ~fatal_signal_mask)
}

void
unblock_all_sigs ()
{
  sigsetmask (0);
}

#else

#define block_sigs()
#define unblock_sigs()

void
unblock_all_sigs ()
{
}

#endif

/* Write an error message describing the exit status given in
   EXIT_CODE, EXIT_SIG, and COREDUMP, for the target TARGET_NAME.
   Append "(ignored)" if IGNORED is nonzero.  */

static void
child_error (struct child *child,
             int exit_code, int exit_sig, int coredump, int ignored)
{
  const char *pre = "*** ";
  const char *post = "";
  const char *dump = "";
  const struct file *f = child->file;
  const floc *flocp = &f->cmds->fileinfo;
  const char *nm;
  size_t l;

  if (ignored && run_silent)
    return;

  if (exit_sig && coredump)
    dump = _(" (core dumped)");

  if (ignored)
    {
      pre = "";
      post = _(" (ignored)");
    }

  if (! flocp->filenm)
    nm = _("<builtin>");
  else
    {
      char *a = alloca (strlen (flocp->filenm) + 6 + INTSTR_LENGTH + 1);
      sprintf (a, "%s:%lu", flocp->filenm, flocp->lineno + flocp->offset);
      nm = a;
    }

  l = strlen (pre) + strlen (nm) + strlen (f->name) + strlen (post);

  OUTPUT_SET (&child->output);

  show_goal_error ();

  if (exit_sig == 0)
    error (NILF, l + INTSTR_LENGTH,
           _("%s[%s: %s] Error %d%s"), pre, nm, f->name, exit_code, post);
  else
    {
      const char *s = strsignal (exit_sig);
      error (NILF, l + strlen (s) + strlen (dump),
             "%s[%s: %s] %s%s%s", pre, nm, f->name, s, dump, post);
    }

  OUTPUT_UNSET ();
}


/* Handle a dead child.  This handler may or may not ever be installed.

   If we're using the jobserver feature without pselect(), we need it.
   First, installing it ensures the read will interrupt on SIGCHLD.  Second,
   we close the dup'd read FD to ensure we don't enter another blocking read
   without reaping all the dead children.  In this case we don't need the
   dead_children count.

   If we don't have either waitpid or wait3, then make is unreliable, but we
   use the dead_children count to reap children as best we can.  */

static unsigned int dead_children = 0;

RETSIGTYPE
child_handler (int sig UNUSED)
{
  ++dead_children;

  jobserver_signal ();

#ifdef __EMX__
  /* The signal handler must called only once! */
  signal (SIGCHLD, SIG_DFL);
#endif
}

extern pid_t shell_function_pid;

/* Reap all dead children, storing the returned status and the new command
   state ('cs_finished') in the 'file' member of the 'struct child' for the
   dead child, and removing the child from the chain.  In addition, if BLOCK
   nonzero, we block in this function until we've reaped at least one
   complete child, waiting for it to die if necessary.  If ERR is nonzero,
   print an error message first.  */

void
reap_children (int block, int err)
{
#ifndef WINDOWS32
  WAIT_T status;
#endif
  /* Initially, assume we have some.  */
  int reap_more = 1;

#ifdef WAIT_NOHANG
# define REAP_MORE reap_more
#else
# define REAP_MORE dead_children
#endif

  /* As long as:

       We have at least one child outstanding OR a shell function in progress,
         AND
       We're blocking for a complete child OR there are more children to reap

     we'll keep reaping children.  */

  while ((children != 0 || shell_function_pid != 0)
         && (block || REAP_MORE))
    {
      unsigned int remote = 0;
      pid_t pid;
      int exit_code, exit_sig, coredump;
      struct child *lastc, *c;
      int child_failed;
      int any_remote, any_local;
      int dontcare;

      if (err && block)
        {
          static int printed = 0;

          /* We might block for a while, so let the user know why.
             Only print this message once no matter how many jobs are left.  */
          fflush (stdout);
          if (!printed)
            O (error, NILF, _("*** Waiting for unfinished jobs...."));
          printed = 1;
        }

      /* We have one less dead child to reap.  As noted in
         child_handler() above, this count is completely unimportant for
         all modern, POSIX-y systems that support wait3() or waitpid().
         The rest of this comment below applies only to early, broken
         pre-POSIX systems.  We keep the count only because... it's there...

         The test and decrement are not atomic; if it is compiled into:
                register = dead_children - 1;
                dead_children = register;
         a SIGCHLD could come between the two instructions.
         child_handler increments dead_children.
         The second instruction here would lose that increment.  But the
         only effect of dead_children being wrong is that we might wait
         longer than necessary to reap a child, and lose some parallelism;
         and we might print the "Waiting for unfinished jobs" message above
         when not necessary.  */

      if (dead_children > 0)
        --dead_children;

      any_remote = 0;
      any_local = shell_function_pid != 0;
      lastc = 0;
      for (c = children; c != 0; lastc = c, c = c->next)
        {
          any_remote |= c->remote;
          any_local |= ! c->remote;

          /* If pid < 0, this child never even started.  Handle it.  */
          if (c->pid < 0)
            {
              exit_sig = 0;
              coredump = 0;
              /* According to POSIX, 127 is used for command not found.  */
              exit_code = 127;
              goto process_child;
            }

          DB (DB_JOBS, (_("Live child %p (%s) PID %s %s\n"),
                        c, c->file->name, pid2str (c->pid),
                        c->remote ? _(" (remote)") : ""));
#ifdef VMS
          break;
#endif
        }

      /* First, check for remote children.  */
      if (any_remote)
        pid = remote_status (&exit_code, &exit_sig, &coredump, 0);
      else
        pid = 0;

      if (pid > 0)
        /* We got a remote child.  */
        remote = 1;
      else if (pid < 0)
        {
          /* A remote status command failed miserably.  Punt.  */
#if !defined(__MSDOS__) && !defined(_AMIGA) && !defined(WINDOWS32)
        remote_status_lose:
#endif
          pfatal_with_name ("remote_status");
        }
      else
        {
          /* No remote children.  Check for local children.  */
#if !defined(__MSDOS__) && !defined(_AMIGA) && !defined(WINDOWS32)
          if (any_local)
            {
#ifdef VMS
              /* Todo: This needs more untangling multi-process support */
              /* Just do single child process support now */
              vmsWaitForChildren (&status);
              pid = c->pid;

              /* VMS failure status can not be fully translated */
              status = $VMS_STATUS_SUCCESS (c->cstatus) ? 0 : (1 << 8);

              /* A Posix failure can be exactly translated */
              if ((c->cstatus & VMS_POSIX_EXIT_MASK) == VMS_POSIX_EXIT_MASK)
                status = (c->cstatus >> 3 & 255) << 8;
#else
#ifdef WAIT_NOHANG
              if (!block)
                pid = WAIT_NOHANG (&status);
              else
#endif
                EINTRLOOP (pid, wait (&status));
#endif /* !VMS */
            }
          else
            pid = 0;

          if (pid < 0)
            {
              /* The wait*() failed miserably.  Punt.  */
              pfatal_with_name ("wait");
            }
          else if (pid > 0)
            {
              /* We got a child exit; chop the status word up.  */
              exit_code = WEXITSTATUS (status);
              exit_sig = WIFSIGNALED (status) ? WTERMSIG (status) : 0;
              coredump = WCOREDUMP (status);
            }
          else
            {
              /* No local children are dead.  */
              reap_more = 0;

              if (!block || !any_remote)
                break;

              /* Now try a blocking wait for a remote child.  */
              pid = remote_status (&exit_code, &exit_sig, &coredump, 1);
              if (pid < 0)
                goto remote_status_lose;
              else if (pid == 0)
                /* No remote children either.  Finally give up.  */
                break;

              /* We got a remote child.  */
              remote = 1;
            }
#endif /* !__MSDOS__, !Amiga, !WINDOWS32.  */

#ifdef __MSDOS__
          /* Life is very different on MSDOS.  */
          pid = dos_pid - 1;
          status = dos_status;
          exit_code = WEXITSTATUS (status);
          if (exit_code == 0xff)
            exit_code = -1;
          exit_sig = WIFSIGNALED (status) ? WTERMSIG (status) : 0;
          coredump = 0;
#endif /* __MSDOS__ */
#ifdef _AMIGA
          /* Same on Amiga */
          pid = amiga_pid - 1;
          status = amiga_status;
          exit_code = amiga_status;
          exit_sig = 0;
          coredump = 0;
#endif /* _AMIGA */
#ifdef WINDOWS32
          {
            HANDLE hPID;
            HANDLE hcTID, hcPID;
            DWORD dwWaitStatus = 0;
            exit_code = 0;
            exit_sig = 0;
            coredump = 0;

            /* Record the thread ID of the main process, so that we
               could suspend it in the signal handler.  */
            if (!main_thread)
              {
                hcTID = GetCurrentThread ();
                hcPID = GetCurrentProcess ();
                if (!DuplicateHandle (hcPID, hcTID, hcPID, &main_thread, 0,
                                      FALSE, DUPLICATE_SAME_ACCESS))
                  {
                    DWORD e = GetLastError ();
                    fprintf (stderr,
                             "Determine main thread ID (Error %ld: %s)\n",
                             e, map_windows32_error_to_string (e));
                  }
                else
                  DB (DB_VERBOSE, ("Main thread handle = %p\n", main_thread));
              }

            /* wait for anything to finish */
            hPID = process_wait_for_any (block, &dwWaitStatus);
            if (hPID)
              {
                /* was an error found on this process? */
                int werr = process_last_err (hPID);

                /* get exit data */
                exit_code = process_exit_code (hPID);

                /* the extra tests of exit_code are here to prevent
                   map_windows32_error_to_string from calling 'fatal',
                   which will then call reap_children again */
                if (werr && exit_code > 0 && exit_code < WSABASEERR)
                  fprintf (stderr, "make (e=%d): %s", exit_code,
                           map_windows32_error_to_string (exit_code));

                /* signal */
                exit_sig = process_signal (hPID);

                /* cleanup process */
                process_cleanup (hPID);

                coredump = 0;
              }
            else if (dwWaitStatus == WAIT_FAILED)
              {
                /* The WaitForMultipleObjects() failed miserably.  Punt.  */
                pfatal_with_name ("WaitForMultipleObjects");
              }
            else if (dwWaitStatus == WAIT_TIMEOUT)
              {
                /* No child processes are finished.  Give up waiting. */
                reap_more = 0;
                break;
              }

            pid = (pid_t) hPID;
          }
#endif /* WINDOWS32 */
        }

      /* Check if this is the child of the 'shell' function.  */
      if (!remote && pid == shell_function_pid)
        {
          shell_completed (exit_code, exit_sig);
          break;
        }

      /* Search for a child matching the deceased one.  */
      lastc = 0;
      for (c = children; c != 0; lastc = c, c = c->next)
        if (c->pid == pid && c->remote == remote)
          break;

      if (c == 0)
        /* An unknown child died.
           Ignore it; it was inherited from our invoker.  */
        continue;

      DB (DB_JOBS, (exit_sig == 0 && exit_code == 0
                    ? _("Reaping winning child %p PID %s %s\n")
                    : _("Reaping losing child %p PID %s %s\n"),
                    c, pid2str (c->pid), c->remote ? _(" (remote)") : ""));

      /* If we have started jobs in this second, remove one.  */
      if (job_counter)
        --job_counter;

    process_child:

#if defined(USE_POSIX_SPAWN)
      /* Some versions of posix_spawn() do not detect errors such as command
         not found until after they fork.  In that case they will exit with a
         code of 127.  Try to detect that and provide a useful error message.
         Otherwise we'll just show the error below, as normal.  */
      if (exit_sig == 0 && exit_code == 127 && c->cmd_name)
        {
          const char *e = NULL;
          struct stat st;
          int r;

          /* There are various ways that this will show a different error than
             fork/exec.  To really get the right error we'd have to fall back
             to fork/exec but I don't want to bother with that.  Just do the
             best we can.  */

          EINTRLOOP(r, stat(c->cmd_name, &st));
          if (r < 0)
            e = strerror (errno);
          else if (S_ISDIR(st.st_mode) || !(st.st_mode & S_IXUSR))
            e = strerror (EACCES);
          else if (st.st_size == 0)
            e = strerror (ENOEXEC);

          if (e)
            OSS(error, NILF, "%s: %s", c->cmd_name, e);
        }
#endif

      /* Determine the failure status: 0 for success, 1 for updating target in
         question mode, 2 for anything else.  */
      if (exit_sig == 0 && exit_code == 0)
        child_failed = MAKE_SUCCESS;
      else if (exit_sig == 0 && exit_code == 1 && question_flag && c->recursive)
        child_failed = MAKE_TROUBLE;
      else
        child_failed = MAKE_FAILURE;

      if (c->sh_batch_file)
        {
          int rm_status;

          DB (DB_JOBS, (_("Cleaning up temp batch file %s\n"),
                        c->sh_batch_file));

          errno = 0;
          rm_status = remove (c->sh_batch_file);
          if (rm_status)
            DB (DB_JOBS, (_("Cleaning up temp batch file %s failed (%d)\n"),
                          c->sh_batch_file, errno));

          /* all done with memory */
          free (c->sh_batch_file);
          c->sh_batch_file = NULL;
        }

      /* If this child had the good stdin, say it is now free.  */
      if (c->good_stdin)
        good_stdin_used = 0;

      dontcare = c->dontcare;

      if (child_failed && !c->noerror && !ignore_errors_flag)
        {
          /* The commands failed.  Write an error message,
             delete non-precious targets, and abort.  */
          static int delete_on_error = -1;

          if (!dontcare && child_failed == MAKE_FAILURE)
            child_error (c, exit_code, exit_sig, coredump, 0);

          c->file->update_status = child_failed == MAKE_FAILURE ? us_failed : us_question;
          if (delete_on_error == -1)
            {
              struct file *f = lookup_file (".DELETE_ON_ERROR");
              delete_on_error = f != 0 && f->is_target;
            }
          if (exit_sig != 0 || delete_on_error)
            delete_child_targets (c);
        }
      else
        {
          if (child_failed)
            {
              /* The commands failed, but we don't care.  */
              child_error (c, exit_code, exit_sig, coredump, 1);
              child_failed = 0;
            }

          /* If there are more commands to run, try to start them.  */
          if (job_next_command (c))
            {
              if (handling_fatal_signal)
                {
                  /* Never start new commands while we are dying.
                     Since there are more commands that wanted to be run,
                     the target was not completely remade.  So we treat
                     this as if a command had failed.  */
                  c->file->update_status = us_failed;
                }
              else
                {
#ifndef NO_OUTPUT_SYNC
                  /* If we're sync'ing per line, write the previous line's
                     output before starting the next one.  */
                  if (output_sync == OUTPUT_SYNC_LINE)
                    output_dump (&c->output);
#endif
                  /* Check again whether to start remotely.
                     Whether or not we want to changes over time.
                     Also, start_remote_job may need state set up
                     by start_remote_job_p.  */
                  c->remote = start_remote_job_p (0);
                  start_job_command (c);
                  /* Fatal signals are left blocked in case we were
                     about to put that child on the chain.  But it is
                     already there, so it is safe for a fatal signal to
                     arrive now; it will clean up this child's targets.  */
                  unblock_sigs ();
                  if (c->file->command_state == cs_running)
                    /* We successfully started the new command.
                       Loop to reap more children.  */
                    continue;
                }

              if (c->file->update_status != us_success)
                /* We failed to start the commands.  */
                delete_child_targets (c);
            }
          else
            /* There are no more commands.  We got through them all
               without an unignored error.  Now the target has been
               successfully updated.  */
            c->file->update_status = us_success;
        }

      /* When we get here, all the commands for c->file are finished.  */

#ifndef NO_OUTPUT_SYNC
      /* Synchronize any remaining parallel output.  */
      output_dump (&c->output);
#endif

      /* At this point c->file->update_status is success or failed.  But
         c->file->command_state is still cs_running if all the commands
         ran; notice_finished_file looks for cs_running to tell it that
         it's interesting to check the file's modtime again now.  */

      if (! handling_fatal_signal)
        /* Notice if the target of the commands has been changed.
           This also propagates its values for command_state and
           update_status to its also_make files.  */
        notice_finished_file (c->file);

      /* Block fatal signals while frobnicating the list, so that
         children and job_slots_used are always consistent.  Otherwise
         a fatal signal arriving after the child is off the chain and
         before job_slots_used is decremented would believe a child was
         live and call reap_children again.  */
      block_sigs ();

      if (c->pid > 0)
        {
          DB (DB_JOBS, (_("Removing child %p PID %s%s from chain.\n"),
                        c, pid2str (c->pid), c->remote ? _(" (remote)") : ""));
        }

      /* There is now another slot open.  */
      if (job_slots_used > 0)
        job_slots_used -= c->jobslot;

      /* Remove the child from the chain and free it.  */
      if (lastc == 0)
        children = c->next;
      else
        lastc->next = c->next;

      free_child (c);

      unblock_sigs ();

      /* If the job failed, and the -k flag was not given, die,
         unless we are already in the process of dying.  */
      if (!err && child_failed && !dontcare && !keep_going_flag &&
          /* fatal_error_signal will die with the right signal.  */
          !handling_fatal_signal)
        die (child_failed);

      /* Only block for one child.  */
      block = 0;
    }

  return;
}

/* Free the storage allocated for CHILD.  */

static void
free_child (struct child *child)
{
  output_close (&child->output);

  if (!jobserver_tokens)
    ONS (fatal, NILF, "INTERNAL: Freeing child %p (%s) but no tokens left!\n",
         child, child->file->name);

  /* If we're using the jobserver and this child is not the only outstanding
     job, put a token back into the pipe for it.  */

  if (jobserver_enabled () && jobserver_tokens > 1)
    {
      jobserver_release (1);
      DB (DB_JOBS, (_("Released token for child %p (%s).\n"),
                    child, child->file->name));
    }

  --jobserver_tokens;

  if (handling_fatal_signal) /* Don't bother free'ing if about to die.  */
    return;

  if (child->command_lines != 0)
    {
      unsigned int i;
      for (i = 0; i < child->file->cmds->ncommand_lines; ++i)
        free (child->command_lines[i]);
      free (child->command_lines);
    }

  if (child->environment != 0)
    {
      char **ep = child->environment;
      while (*ep != 0)
        free (*ep++);
      free (child->environment);
    }

  free (child->cmd_name);
  free (child);
}


/* Start a job to run the commands specified in CHILD.
   CHILD is updated to reflect the commands and ID of the child process.

   NOTE: On return fatal signals are blocked!  The caller is responsible
   for calling 'unblock_sigs', once the new child is safely on the chain so
   it can be cleaned up in the event of a fatal signal.  */

static void
start_job_command (struct child *child)
{
  int flags;
  char *p;
#ifdef VMS
# define FREE_ARGV(_a)
  char *argv;
#else
# define FREE_ARGV(_a) do{ if (_a) { free ((_a)[0]); free (_a); } }while(0)
  char **argv;
#endif

  /* If we have a completely empty commandset, stop now.  */
  if (!child->command_ptr)
    goto next_command;

  /* Combine the flags parsed for the line itself with
     the flags specified globally for this target.  */
  flags = (child->file->command_flags
           | child->file->cmds->lines_flags[child->command_line - 1]);

  p = child->command_ptr;
  child->noerror = ((flags & COMMANDS_NOERROR) != 0);

  while (*p != '\0')
    {
      if (*p == '@')
        flags |= COMMANDS_SILENT;
      else if (*p == '+')
        flags |= COMMANDS_RECURSE;
      else if (*p == '-')
        child->noerror = 1;
      /* Don't skip newlines.  */
      else if (!ISBLANK (*p))
        break;
      ++p;
    }

  child->recursive = ((flags & COMMANDS_RECURSE) != 0);

  /* Update the file's command flags with any new ones we found.  We only
     keep the COMMANDS_RECURSE setting.  Even this isn't 100% correct; we are
     now marking more commands recursive than should be in the case of
     multiline define/endef scripts where only one line is marked "+".  In
     order to really fix this, we'll have to keep a lines_flags for every
     actual line, after expansion.  */
  child->file->cmds->lines_flags[child->command_line - 1] |= flags & COMMANDS_RECURSE;

  /* POSIX requires that a recipe prefix after a backslash-newline should
     be ignored.  Remove it now so the output is correct.  */
  {
    char prefix = child->file->cmds->recipe_prefix;
    char *p1, *p2;
    p1 = p2 = p;
    while (*p1 != '\0')
      {
        *(p2++) = *p1;
        if (p1[0] == '\n' && p1[1] == prefix)
          ++p1;
        ++p1;
      }
    *p2 = *p1;
  }

  /* Figure out an argument list from this command line.  */
  {
    char *end = 0;
#ifdef VMS
    /* Skip any leading whitespace */
    while (*p)
      {
        if (!ISSPACE (*p))
          {
            if (*p != '\\')
              break;
            if ((p[1] != '\n') && (p[1] != 'n') && (p[1] != 't'))
              break;
          }
        p++;
      }

    argv = p;
    /* Please note, for VMS argv is a string (not an array of strings) which
       contains the complete command line, which for multi-line variables
       still includes the newlines.  So detect newlines and set 'end' (which
       is used for child->command_ptr) instead of (re-)writing
       construct_command_argv */
    if (!one_shell)
      {
        char *s = p;
        int instring = 0;
        while (*s)
          {
            if (*s == '"')
              instring = !instring;
            else if (*s == '\\' && !instring && *(s+1) != 0)
              s++;
            else if (*s == '\n' && !instring)
              {
                end = s;
                break;
              }
            ++s;
          }
      }
#else
    argv = construct_command_argv (p, &end, child->file,
                                   child->file->cmds->lines_flags[child->command_line - 1],
                                   &child->sh_batch_file);
#endif
    if (end == NULL)
      child->command_ptr = NULL;
    else
      {
        *end++ = '\0';
        child->command_ptr = end;
      }
  }

  /* If -q was given, say that updating 'failed' if there was any text on the
     command line, or 'succeeded' otherwise.  The exit status of 1 tells the
     user that -q is saying 'something to do'; the exit status for a random
     error is 2.  */
  if (argv != 0 && question_flag && !(flags & COMMANDS_RECURSE))
    {
      FREE_ARGV (argv);
#ifdef VMS
      /* On VMS, argv[0] can be a null string here */
      if (argv[0] != 0)
        {
#endif
          child->file->update_status = us_question;
          notice_finished_file (child->file);
          return;
#ifdef VMS
        }
#endif
    }

  if (touch_flag && !(flags & COMMANDS_RECURSE))
    {
      /* Go on to the next command.  It might be the recursive one.
         We construct ARGV only to find the end of the command line.  */
      FREE_ARGV (argv);
      argv = 0;
    }

  if (argv == 0)
    {
    next_command:
#ifdef __MSDOS__
      execute_by_shell = 0;   /* in case construct_command_argv sets it */
#endif
      /* This line has no commands.  Go to the next.  */
      if (job_next_command (child))
        start_job_command (child);
      else
        {
          /* No more commands.  Make sure we're "running"; we might not be if
             (e.g.) all commands were skipped due to -n.  */
          set_command_state (child->file, cs_running);
          child->file->update_status = us_success;
          notice_finished_file (child->file);
        }

      OUTPUT_UNSET();
      return;
    }

  /* Are we going to synchronize this command's output?  Do so if either we're
     in SYNC_RECURSE mode or this command is not recursive.  We'll also check
     output_sync separately below in case it changes due to error.  */
  child->output.syncout = output_sync && (output_sync == OUTPUT_SYNC_RECURSE
                                          || !(flags & COMMANDS_RECURSE));

  OUTPUT_SET (&child->output);

#ifndef NO_OUTPUT_SYNC
  if (! child->output.syncout)
    /* We don't want to sync this command: to avoid misordered
       output ensure any already-synced content is written.  */
    output_dump (&child->output);
#endif

  /* Print the command if appropriate.  */
  if (just_print_flag || trace_flag
      || (!(flags & COMMANDS_SILENT) && !run_silent))
    OS (message, 0, "%s", p);

  /* Tell update_goal_chain that a command has been started on behalf of
     this target.  It is important that this happens here and not in
     reap_children (where we used to do it), because reap_children might be
     reaping children from a different target.  We want this increment to
     guaranteedly indicate that a command was started for the dependency
     chain (i.e., update_file recursion chain) we are processing.  */

  ++commands_started;

  /* Optimize an empty command.  People use this for timestamp rules,
     so avoid forking a useless shell.  Do this after we increment
     commands_started so make still treats this special case as if it
     performed some action (makes a difference as to what messages are
     printed, etc.  */

#if !defined(VMS) && !defined(_AMIGA)
  if (
#if defined __MSDOS__ || defined (__EMX__)
      unixy_shell       /* the test is complicated and we already did it */
#else
      (argv[0] && is_bourne_compatible_shell (argv[0]))
#endif
      && (argv[1] && argv[1][0] == '-'
        &&
            ((argv[1][1] == 'c' && argv[1][2] == '\0')
          ||
             (argv[1][1] == 'e' && argv[1][2] == 'c' && argv[1][3] == '\0')))
      && (argv[2] && argv[2][0] == ':' && argv[2][1] == '\0')
      && argv[3] == NULL)
    {
      FREE_ARGV (argv);
      goto next_command;
    }
#endif  /* !VMS && !_AMIGA */

  /* If -n was given, recurse to get the next line in the sequence.  */

  if (just_print_flag && !(flags & COMMANDS_RECURSE))
    {
      FREE_ARGV (argv);
      goto next_command;
    }

  /* We're sure we're going to invoke a command: set up the output.  */
  output_start ();

  /* Flush the output streams so they won't have things written twice.  */

  fflush (stdout);
  fflush (stderr);

  /* Decide whether to give this child the 'good' standard input
     (one that points to the terminal or whatever), or the 'bad' one
     that points to the read side of a broken pipe.  */

  child->good_stdin = !good_stdin_used;
  if (child->good_stdin)
    good_stdin_used = 1;

  child->deleted = 0;

#ifndef _AMIGA
  /* Set up the environment for the child.  */
  if (child->environment == 0)
    child->environment = target_environment (child->file);
#endif

#if !defined(__MSDOS__) && !defined(_AMIGA) && !defined(WINDOWS32)

#ifndef VMS
  /* start_waiting_job has set CHILD->remote if we can start a remote job.  */
  if (child->remote)
    {
      int is_remote, used_stdin;
      pid_t id;
      if (start_remote_job (argv, child->environment,
                            child->good_stdin ? 0 : get_bad_stdin (),
                            &is_remote, &id, &used_stdin))
        /* Don't give up; remote execution may fail for various reasons.  If
           so, simply run the job locally.  */
        goto run_local;
      else
        {
          if (child->good_stdin && !used_stdin)
            {
              child->good_stdin = 0;
              good_stdin_used = 0;
            }
          child->remote = is_remote;
          child->pid = id;
        }
    }
  else
#endif /* !VMS */
    {
      /* Fork the child process.  */

      char **parent_environ;

    run_local:
      block_sigs ();

      child->remote = 0;

#ifdef VMS
      child->pid = child_execute_job ((struct childbase *)child, 1, argv);

#else

      parent_environ = environ;

      jobserver_pre_child (flags & COMMANDS_RECURSE);

      child->pid = child_execute_job ((struct childbase *)child,
                                      child->good_stdin, argv);

      environ = parent_environ; /* Restore value child may have clobbered.  */
      jobserver_post_child (flags & COMMANDS_RECURSE);

#endif /* !VMS */
    }

#else   /* __MSDOS__ or Amiga or WINDOWS32 */
#ifdef __MSDOS__
  {
    int proc_return;

    block_sigs ();
    dos_status = 0;

    /* We call 'system' to do the job of the SHELL, since stock DOS
       shell is too dumb.  Our 'system' knows how to handle long
       command lines even if pipes/redirection is needed; it will only
       call COMMAND.COM when its internal commands are used.  */
    if (execute_by_shell)
      {
        char *cmdline = argv[0];
        /* We don't have a way to pass environment to 'system',
           so we need to save and restore ours, sigh...  */
        char **parent_environ = environ;

        environ = child->environment;

        /* If we have a *real* shell, tell 'system' to call
           it to do everything for us.  */
        if (unixy_shell)
          {
            /* A *real* shell on MSDOS may not support long
               command lines the DJGPP way, so we must use 'system'.  */
            cmdline = argv[2];  /* get past "shell -c" */
          }

        dos_command_running = 1;
        proc_return = system (cmdline);
        environ = parent_environ;
        execute_by_shell = 0;   /* for the next time */
      }
    else
      {
        dos_command_running = 1;
        proc_return = spawnvpe (P_WAIT, argv[0], argv, child->environment);
      }

    /* Need to unblock signals before turning off
       dos_command_running, so that child's signals
       will be treated as such (see fatal_error_signal).  */
    unblock_sigs ();
    dos_command_running = 0;

    /* If the child got a signal, dos_status has its
       high 8 bits set, so be careful not to alter them.  */
    if (proc_return == -1)
      dos_status |= 0xff;
    else
      dos_status |= (proc_return & 0xff);
    ++dead_children;
    child->pid = dos_pid++;
  }
#endif /* __MSDOS__ */
#ifdef _AMIGA
  amiga_status = MyExecute (argv);

  ++dead_children;
  child->pid = amiga_pid++;
  if (amiga_batch_file)
  {
     amiga_batch_file = 0;
     DeleteFile (amiga_bname);        /* Ignore errors.  */
  }
#endif  /* Amiga */
#ifdef WINDOWS32
  {
      HANDLE hPID;
      char* arg0;
      int outfd = FD_STDOUT;
      int errfd = FD_STDERR;

      /* make UNC paths safe for CreateProcess -- backslash format */
      arg0 = argv[0];
      if (arg0 && arg0[0] == '/' && arg0[1] == '/')
        for ( ; arg0 && *arg0; arg0++)
          if (*arg0 == '/')
            *arg0 = '\\';

      /* make sure CreateProcess() has Path it needs */
      sync_Path_environment ();

#ifndef NO_OUTPUT_SYNC
      /* Divert child output if output_sync in use.  */
      if (child->output.syncout)
        {
          if (child->output.out >= 0)
            outfd = child->output.out;
          if (child->output.err >= 0)
            errfd = child->output.err;
        }
#else
      outfd = errfd = -1;
#endif
      hPID = process_easy (argv, child->environment, outfd, errfd);

      if (hPID != INVALID_HANDLE_VALUE)
        child->pid = (pid_t) hPID;
      else
        {
          int i;
          unblock_sigs ();
          fprintf (stderr,
                   _("process_easy() failed to launch process (e=%ld)\n"),
                   process_last_err (hPID));
          for (i = 0; argv[i]; i++)
            fprintf (stderr, "%s ", argv[i]);
          fprintf (stderr, _("\nCounted %d args in failed launch\n"), i);
          child->pid = -1;
        }
  }
#endif /* WINDOWS32 */
#endif  /* __MSDOS__ or Amiga or WINDOWS32 */

  /* Bump the number of jobs started in this second.  */
  if (child->pid >= 0)
    ++job_counter;

  /* Set the state to running.  */
  set_command_state (child->file, cs_running);

  /* Free the storage used by the child's argument list.  */
  FREE_ARGV (argv);

  OUTPUT_UNSET();

#undef FREE_ARGV
}

/* Try to start a child running.
   Returns nonzero if the child was started (and maybe finished), or zero if
   the load was too high and the child was put on the 'waiting_jobs' chain.  */

static int
start_waiting_job (struct child *c)
{
  struct file *f = c->file;

  /* If we can start a job remotely, we always want to, and don't care about
     the local load average.  We record that the job should be started
     remotely in C->remote for start_job_command to test.  */

  c->remote = start_remote_job_p (1);

  /* If we are running at least one job already and the load average
     is too high, make this one wait.  */
  if (!c->remote
      && ((job_slots_used > 0 && load_too_high ())
#ifdef WINDOWS32
          || process_table_full ()
#endif
          ))
    {
      /* Put this child on the chain of children waiting for the load average
         to go down.  */
      set_command_state (f, cs_running);
      c->next = waiting_jobs;
      waiting_jobs = c;
      return 0;
    }

  /* Start the first command; reap_children will run later command lines.  */
  start_job_command (c);

  switch (f->command_state)
    {
    case cs_running:
      c->next = children;
      if (c->pid > 0)
        {
          DB (DB_JOBS, (_("Putting child %p (%s) PID %s%s on the chain.\n"),
                        c, c->file->name, pid2str (c->pid),
                        c->remote ? _(" (remote)") : ""));
          /* One more job slot is in use.  */
          ++job_slots_used;
          assert (c->jobslot == 0);
          c->jobslot = 1;
        }
      children = c;
      unblock_sigs ();
      break;

    case cs_not_started:
      /* All the command lines turned out to be empty.  */
      f->update_status = us_success;
      /* FALLTHROUGH */

    case cs_finished:
      notice_finished_file (f);
      free_child (c);
      break;

    default:
      assert (f->command_state == cs_finished);
      break;
    }

  return 1;
}

/* Create a 'struct child' for FILE and start its commands running.  */

void
new_job (struct file *file)
{
  struct commands *cmds = file->cmds;
  struct child *c;
  char **lines;
  unsigned int i;

  /* Let any previously decided-upon jobs that are waiting
     for the load to go down start before this new one.  */
  start_waiting_jobs ();

  /* Reap any children that might have finished recently.  */
  reap_children (0, 0);

  /* Chop the commands up into lines if they aren't already.  */
  chop_commands (cmds);

  /* Start the command sequence, record it in a new
     'struct child', and add that to the chain.  */

  c = xcalloc (sizeof (struct child));
  output_init (&c->output);

  c->file = file;
  c->sh_batch_file = NULL;

  /* Cache dontcare flag because file->dontcare can be changed once we
     return. Check dontcare inheritance mechanism for details.  */
  c->dontcare = file->dontcare;

  /* Start saving output in case the expansion uses $(info ...) etc.  */
  OUTPUT_SET (&c->output);

  /* Expand the command lines and store the results in LINES.  */
  lines = xmalloc (cmds->ncommand_lines * sizeof (char *));
  for (i = 0; i < cmds->ncommand_lines; ++i)
    {
      /* Collapse backslash-newline combinations that are inside variable
         or function references.  These are left alone by the parser so
         that they will appear in the echoing of commands (where they look
         nice); and collapsed by construct_command_argv when it tokenizes.
         But letting them survive inside function invocations loses because
         we don't want the functions to see them as part of the text.  */

      char *in, *out, *ref;

      /* IN points to where in the line we are scanning.
         OUT points to where in the line we are writing.
         When we collapse a backslash-newline combination,
         IN gets ahead of OUT.  */

      in = out = cmds->command_lines[i];
      while ((ref = strchr (in, '$')) != 0)
        {
          ++ref;                /* Move past the $.  */

          if (out != in)
            /* Copy the text between the end of the last chunk
               we processed (where IN points) and the new chunk
               we are about to process (where REF points).  */
            memmove (out, in, ref - in);

          /* Move both pointers past the boring stuff.  */
          out += ref - in;
          in = ref;

          if (*ref == '(' || *ref == '{')
            {
              char openparen = *ref;
              char closeparen = openparen == '(' ? ')' : '}';
              char *outref;
              int count;
              char *p;

              *out++ = *in++;   /* Copy OPENPAREN.  */
              outref = out;
              /* IN now points past the opening paren or brace.
                 Count parens or braces until it is matched.  */
              count = 0;
              while (*in != '\0')
                {
                  if (*in == closeparen && --count < 0)
                    break;
                  else if (*in == '\\' && in[1] == '\n')
                    {
                      /* We have found a backslash-newline inside a
                         variable or function reference.  Eat it and
                         any following whitespace.  */

                      int quoted = 0;
                      for (p = in - 1; p > ref && *p == '\\'; --p)
                        quoted = !quoted;

                      if (quoted)
                        /* There were two or more backslashes, so this is
                           not really a continuation line.  We don't collapse
                           the quoting backslashes here as is done in
                           collapse_continuations, because the line will
                           be collapsed again after expansion.  */
                        *out++ = *in++;
                      else
                        {
                          /* Skip the backslash, newline, and whitespace.  */
                          in += 2;
                          NEXT_TOKEN (in);

                          /* Discard any preceding whitespace that has
                             already been written to the output.  */
                          while (out > outref && ISBLANK (out[-1]))
                            --out;

                          /* Replace it all with a single space.  */
                          *out++ = ' ';
                        }
                    }
                  else
                    {
                      if (*in == openparen)
                        ++count;

                      *out++ = *in++;
                    }
                }
            }
        }

      /* There are no more references in this line to worry about.
         Copy the remaining uninteresting text to the output.  */
      if (out != in)
        memmove (out, in, strlen (in) + 1);

      /* Finally, expand the line.  */
      cmds->fileinfo.offset = i;
      lines[i] = allocated_variable_expand_for_file (cmds->command_lines[i],
                                                     file);
    }

  cmds->fileinfo.offset = 0;
  c->command_lines = lines;

  /* Fetch the first command line to be run.  */
  job_next_command (c);

  /* Wait for a job slot to be freed up.  If we allow an infinite number
     don't bother; also job_slots will == 0 if we're using the jobserver.  */

  if (job_slots != 0)
    while (job_slots_used == job_slots)
      reap_children (1, 0);

#ifdef MAKE_JOBSERVER
  /* If we are controlling multiple jobs make sure we have a token before
     starting the child. */

  /* This can be inefficient.  There's a decent chance that this job won't
     actually have to run any subprocesses: the command script may be empty
     or otherwise optimized away.  It would be nice if we could defer
     obtaining a token until just before we need it, in start_job_command.
     To do that we'd need to keep track of whether we'd already obtained a
     token (since start_job_command is called for each line of the job, not
     just once).  Also more thought needs to go into the entire algorithm;
     this is where the old parallel job code waits, so...  */

  else if (jobserver_enabled ())
    while (1)
      {
        int got_token;

        DB (DB_JOBS, ("Need a job token; we %shave children\n",
                      children ? "" : "don't "));

        /* If we don't already have a job started, use our "free" token.  */
        if (!jobserver_tokens)
          break;

        /* Prepare for jobserver token acquisition.  */
        jobserver_pre_acquire ();

        /* Reap anything that's currently waiting.  */
        reap_children (0, 0);

        /* Kick off any jobs we have waiting for an opportunity that
           can run now (i.e., waiting for load). */
        start_waiting_jobs ();

        /* If our "free" slot is available, use it; we don't need a token.  */
        if (!jobserver_tokens)
          break;

        /* There must be at least one child already, or we have no business
           waiting for a token. */
        if (!children)
          O (fatal, NILF, "INTERNAL: no children as we go to sleep on read\n");

        /* Get a token.  */
        got_token = jobserver_acquire (waiting_jobs != NULL);

        /* If we got one, we're done here.  */
        if (got_token == 1)
          {
            DB (DB_JOBS, (_("Obtained token for child %p (%s).\n"),
                          c, c->file->name));
            break;
          }
      }
#endif

  ++jobserver_tokens;

  /* Trace the build.
     Use message here so that changes to working directories are logged.  */
  if (trace_flag)
    {
      char *newer = allocated_variable_expand_for_file ("$?", c->file);
      const char *nm;

      if (! cmds->fileinfo.filenm)
        nm = _("<builtin>");
      else
        {
          char *n = alloca (strlen (cmds->fileinfo.filenm) + 1 + 11 + 1);
          sprintf (n, "%s:%lu", cmds->fileinfo.filenm, cmds->fileinfo.lineno);
          nm = n;
        }

      if (newer[0] == '\0')
        OSS (message, 0,
             _("%s: target '%s' does not exist"), nm, c->file->name);
      else
        OSSS (message, 0,
              _("%s: update target '%s' due to: %s"), nm, c->file->name, newer);

      free (newer);
    }

  /* The job is now primed.  Start it running.
     (This will notice if there is in fact no recipe.)  */
  start_waiting_job (c);

  if (job_slots == 1 || not_parallel)
    /* Since there is only one job slot, make things run linearly.
       Wait for the child to die, setting the state to 'cs_finished'.  */
    while (file->command_state == cs_running)
      reap_children (1, 0);

  OUTPUT_UNSET ();
  return;
}

/* Move CHILD's pointers to the next command for it to execute.
   Returns nonzero if there is another command.  */

static int
job_next_command (struct child *child)
{
  while (child->command_ptr == 0 || *child->command_ptr == '\0')
    {
      /* There are no more lines in the expansion of this line.  */
      if (child->command_line == child->file->cmds->ncommand_lines)
        {
          /* There are no more lines to be expanded.  */
          child->command_ptr = 0;
          child->file->cmds->fileinfo.offset = 0;
          return 0;
        }
      else
        /* Get the next line to run.  */
        child->command_ptr = child->command_lines[child->command_line++];
    }

  child->file->cmds->fileinfo.offset = child->command_line - 1;
  return 1;
}

/* Determine if the load average on the system is too high to start a new job.

   On systems which provide /proc/loadavg (e.g., Linux), we use an idea
   provided by Sven C. Dack <sven.c.dack@sky.com>: retrieve the current number
   of processes the kernel is running and, if it's greater than the requested
   load we don't allow another job to start.  We allow a job to start with
   equal processes since one of those will be for make itself, which will then
   pause waiting for jobs to clear.

   Otherwise, we obtain the system load average and compare that.

   The system load average is only recomputed once every N (N>=1) seconds.
   However, a very parallel make can easily start tens or even hundreds of
   jobs in a second, which brings the system to its knees for a while until
   that first batch of jobs clears out.

   To avoid this we use a weighted algorithm to try to account for jobs which
   have been started since the last second, and guess what the load average
   would be now if it were computed.

   This algorithm was provided by Thomas Riedl <thomas.riedl@siemens.com>,
   based on load average being recomputed once per second, which is
   (apparently) how Solaris operates.  Linux recomputes only once every 5
   seconds, but Linux is handled by the /proc/loadavg algorithm above.

   Thomas writes:

!      calculate something load-oid and add to the observed sys.load,
!      so that latter can catch up:
!      - every job started increases jobctr;
!      - every dying job decreases a positive jobctr;
!      - the jobctr value gets zeroed every change of seconds,
!        after its value*weight_b is stored into the 'backlog' value last_sec
!      - weight_a times the sum of jobctr and last_sec gets
!        added to the observed sys.load.
!
!      The two weights have been tried out on 24 and 48 proc. Sun Solaris-9
!      machines, using a several-thousand-jobs-mix of cpp, cc, cxx and smallish
!      sub-shelled commands (rm, echo, sed...) for tests.
!      lowering the 'direct influence' factor weight_a (e.g. to 0.1)
!      resulted in significant excession of the load limit, raising it
!      (e.g. to 0.5) took bad to small, fast-executing jobs and didn't
!      reach the limit in most test cases.
!
!      lowering the 'history influence' weight_b (e.g. to 0.1) resulted in
!      exceeding the limit for longer-running stuff (compile jobs in
!      the .5 to 1.5 sec. range),raising it (e.g. to 0.5) overrepresented
!      small jobs' effects.

 */

#define LOAD_WEIGHT_A           0.25
#define LOAD_WEIGHT_B           0.25

static int
load_too_high (void)
{
#if defined(__MSDOS__) || defined(VMS) || defined(_AMIGA) || defined(__riscos__)
  return 1;
#else
  static double last_sec;
  static time_t last_now;

  /* This is disabled by default for now, because it will behave badly if the
     user gives a value > the number of cores; in that situation the load will
     never be exceeded, this function always returns false, and we'll start
     all the jobs.  Also, it's not quite right to limit jobs to the number of
     cores not busy since a job takes some time to start etc.  Maybe that's
     OK, I'm not sure exactly how to handle that, but for sure we need to
     clamp this value at the number of cores before this can be enabled.
   */
#define PROC_FD_INIT -1
  static int proc_fd = PROC_FD_INIT;

  double load, guess;
  time_t now;

#ifdef WINDOWS32
  /* sub_proc.c is limited in the number of objects it can wait for. */
  if (process_table_full ())
    return 1;
#endif

  if (max_load_average < 0)
    return 0;

  /* If we haven't tried to open /proc/loadavg, try now.  */
#define LOADAVG "/proc/loadavg"
  if (proc_fd == -2)
    {
      EINTRLOOP (proc_fd, open (LOADAVG, O_RDONLY));
      if (proc_fd < 0)
        DB (DB_JOBS, ("Using system load detection method.\n"));
      else
        {
          DB (DB_JOBS, ("Using " LOADAVG " load detection method.\n"));
          fd_noinherit (proc_fd);
        }
    }

  /* Try to read /proc/loadavg if we managed to open it.  */
  if (proc_fd >= 0)
    {
      int r;

      EINTRLOOP (r, lseek (proc_fd, 0, SEEK_SET));
      if (r >= 0)
        {
#define PROC_LOADAVG_SIZE 64
          char avg[PROC_LOADAVG_SIZE+1];

          EINTRLOOP (r, read (proc_fd, avg, PROC_LOADAVG_SIZE));
          if (r >= 0)
            {
              const char *p;

              /* The syntax of /proc/loadavg is:
                    <1m> <5m> <15m> <running>/<total> <pid>
                 The load is considered too high if there are more jobs
                 running than the requested average.  */

              avg[r] = '\0';
              p = strchr (avg, ' ');
              if (p)
                p = strchr (p+1, ' ');
              if (p)
                p = strchr (p+1, ' ');

              if (p && ISDIGIT(p[1]))
                {
                  int cnt = atoi (p+1);
                  DB (DB_JOBS, ("Running: system = %d / make = %u (max requested = %f)\n",
                                cnt, job_slots_used, max_load_average));
                  return (double)cnt > max_load_average;
                }

              DB (DB_JOBS, ("Failed to parse " LOADAVG ": %s\n", avg));
            }
        }

      /* If we got here, something went wrong.  Give up on this method.  */
      if (r < 0)
        DB (DB_JOBS, ("Failed to read " LOADAVG ": %s\n", strerror (errno)));

      close (proc_fd);
      proc_fd = -1;
    }

  /* Find the real system load average.  */
  make_access ();
  if (getloadavg (&load, 1) != 1)
    {
      static int lossage = -1;
      /* Complain only once for the same error.  */
      if (lossage == -1 || errno != lossage)
        {
          if (errno == 0)
            /* An errno value of zero means getloadavg is just unsupported.  */
            O (error, NILF,
               _("cannot enforce load limits on this operating system"));
          else
            perror_with_name (_("cannot enforce load limit: "), "getloadavg");
        }
      lossage = errno;
      load = 0;
    }
  user_access ();

  /* If we're in a new second zero the counter and correct the backlog
     value.  Only keep the backlog for one extra second; after that it's 0.  */
  now = time (NULL);
  if (last_now < now)
    {
      if (last_now == now - 1)
        last_sec = LOAD_WEIGHT_B * job_counter;
      else
        last_sec = 0.0;

      job_counter = 0;
      last_now = now;
    }

  /* Try to guess what the load would be right now.  */
  guess = load + (LOAD_WEIGHT_A * (job_counter + last_sec));

  DB (DB_JOBS, ("Estimated system load = %f (actual = %f) (max requested = %f)\n",
                guess, load, max_load_average));

  return guess >= max_load_average;
#endif
}

/* Start jobs that are waiting for the load to be lower.  */

void
start_waiting_jobs (void)
{
  struct child *job;

  if (waiting_jobs == 0)
    return;

  do
    {
      /* Check for recently deceased descendants.  */
      reap_children (0, 0);

      /* Take a job off the waiting list.  */
      job = waiting_jobs;
      waiting_jobs = job->next;

      /* Try to start that job.  We break out of the loop as soon
         as start_waiting_job puts one back on the waiting list.  */
    }
  while (start_waiting_job (job) && waiting_jobs != 0);

  return;
}

#ifndef WINDOWS32

/* EMX: Start a child process. This function returns the new pid.  */
# if defined __EMX__
pid_t
child_execute_job (struct childbase *child, int good_stdin, char **argv)
{
  pid_t pid;
  int fdin = good_stdin ? FD_STDIN : get_bad_stdin ();
  int fdout = FD_STDOUT;
  int fderr = FD_STDERR;
  int save_fdin = -1;
  int save_fdout = -1;
  int save_fderr = -1;

  /* Divert child output if we want to capture output.  */
  if (child->output.syncout)
    {
      if (child->output.out >= 0)
        fdout = child->output.out;
      if (child->output.err >= 0)
        fderr = child->output.err;
    }

  /* For each FD which needs to be redirected first make a dup of the standard
     FD to save and mark it close on exec so our child won't see it.  Then
     dup2() the standard FD to the redirect FD, and also mark the redirect FD
     as close on exec. */
  if (fdin != FD_STDIN)
    {
      save_fdin = dup (FD_STDIN);
      if (save_fdin < 0)
        O (fatal, NILF, _("no more file handles: could not duplicate stdin\n"));
      fd_noinherit (save_fdin);

      dup2 (fdin, FD_STDIN);
      fd_noinherit (fdin);
    }

  if (fdout != FD_STDOUT)
    {
      save_fdout = dup (FD_STDOUT);
      if (save_fdout < 0)
        O (fatal, NILF,
           _("no more file handles: could not duplicate stdout\n"));
      fd_noinherit (save_fdout);

      dup2 (fdout, FD_STDOUT);
      fd_noinherit (fdout);
    }

  if (fderr != FD_STDERR)
    {
      if (fderr != fdout)
        {
          save_fderr = dup (FD_STDERR);
          if (save_fderr < 0)
            O (fatal, NILF,
               _("no more file handles: could not duplicate stderr\n"));
          fd_noinherit (save_fderr);
        }

      dup2 (fderr, FD_STDERR);
      fd_noinherit (fderr);
    }

  /* Run the command.  */
  pid = exec_command (argv, child->environment);

  /* Restore stdout/stdin/stderr of the parent and close temporary FDs.  */
  if (save_fdin >= 0)
    {
      if (dup2 (save_fdin, FD_STDIN) != FD_STDIN)
        O (fatal, NILF, _("Could not restore stdin\n"));
      else
        close (save_fdin);
    }

  if (save_fdout >= 0)
    {
      if (dup2 (save_fdout, FD_STDOUT) != FD_STDOUT)
        O (fatal, NILF, _("Could not restore stdout\n"));
      else
        close (save_fdout);
    }

  if (save_fderr >= 0)
    {
      if (dup2 (save_fderr, FD_STDERR) != FD_STDERR)
        O (fatal, NILF, _("Could not restore stderr\n"));
      else
        close (save_fderr);
    }

  if (pid < 0)
    OSS (error, NILF, "%s: %s", argv[0], strerror (errno));

  return pid;
}

#elif !defined (_AMIGA) && !defined (__MSDOS__) && !defined (VMS)

/* POSIX:
   Create a child process executing the command in ARGV.
   Returns the PID or -1.  */
pid_t
child_execute_job (struct childbase *child, int good_stdin, char **argv)
{
  const int fdin = good_stdin ? FD_STDIN : get_bad_stdin ();
  int fdout = FD_STDOUT;
  int fderr = FD_STDERR;
  pid_t pid;
  int r;
#if defined(USE_POSIX_SPAWN)
  char *cmd;
  posix_spawnattr_t attr;
  posix_spawn_file_actions_t fa;
  short flags = 0;
#endif

  /* Divert child output if we want to capture it.  */
  if (child->output.syncout)
    {
      if (child->output.out >= 0)
        fdout = child->output.out;
      if (child->output.err >= 0)
        fderr = child->output.err;
    }

#if !defined(USE_POSIX_SPAWN)

  pid = vfork();
  if (pid != 0)
    return pid;

  /* We are the child.  */
  unblock_all_sigs ();

#ifdef SET_STACK_SIZE
  /* Reset limits, if necessary.  */
  if (stack_limit.rlim_cur)
    setrlimit (RLIMIT_STACK, &stack_limit);
#endif

  /* For any redirected FD, dup2() it to the standard FD.
     They are all marked close-on-exec already.  */
  if (fdin >= 0 && fdin != FD_STDIN)
    EINTRLOOP (r, dup2 (fdin, FD_STDIN));
  if (fdout != FD_STDOUT)
    EINTRLOOP (r, dup2 (fdout, FD_STDOUT));
  if (fderr != FD_STDERR)
    EINTRLOOP (r, dup2 (fderr, FD_STDERR));

  /* Run the command.  */
  exec_command (argv, child->environment);

#else /* USE_POSIX_SPAWN */

  if ((r = posix_spawnattr_init (&attr)) != 0)
    goto done;

  if ((r = posix_spawn_file_actions_init (&fa)) != 0)
    {
      posix_spawnattr_destroy (&attr);
      goto done;
    }

  /* Unblock all signals.  */
#ifdef HAVE_POSIX_SPAWNATTR_SETSIGMASK
  {
    sigset_t mask;
    sigemptyset (&mask);
    r = posix_spawnattr_setsigmask (&attr, &mask);
    if (r != 0)
      goto cleanup;
    flags |= POSIX_SPAWN_SETSIGMASK;
  }
#endif /* have posix_spawnattr_setsigmask() */

  /* USEVFORK can give significant speedup on systems where it's available.  */
#ifdef POSIX_SPAWN_USEVFORK
  flags |= POSIX_SPAWN_USEVFORK;
#endif

  /* For any redirected FD, dup2() it to the standard FD.
     They are all marked close-on-exec already.  */
  if (fdin >= 0 && fdin != FD_STDIN)
    if ((r = posix_spawn_file_actions_adddup2 (&fa, fdin, FD_STDIN)) != 0)
      goto cleanup;
  if (fdout != FD_STDOUT)
    if ((r = posix_spawn_file_actions_adddup2 (&fa, fdout, FD_STDOUT)) != 0)
      goto cleanup;
  if (fderr != FD_STDERR)
    if ((r = posix_spawn_file_actions_adddup2 (&fa, fderr, FD_STDERR)) != 0)
      goto cleanup;

  /* Be the user, permanently.  */
  flags |= POSIX_SPAWN_RESETIDS;

  /* Apply the spawn flags.  */
  if ((r = posix_spawnattr_setflags (&attr, flags)) != 0)
    goto cleanup;

  /* Look up the program on the child's PATH, if needed.  */
  {
    const char *p = NULL;
    char **pp;

    for (pp = child->environment; *pp != NULL; ++pp)
      if ((*pp)[0] == 'P' && (*pp)[1] == 'A' && (*pp)[2] == 'T'
          && (*pp)[3] == 'H' &&(*pp)[4] == '=')
        {
          p = (*pp) + 5;
          break;
        }

    cmd = (char *)find_in_given_path (argv[0], p, 0);
  }

  if (!cmd)
    {
      r = errno;
      goto cleanup;
    }

  /* Start the program.  */
  while ((r = posix_spawn (&pid, cmd, &fa, &attr, argv,
                           child->environment)) == EINTR)
    ;

  /* posix_spawn() doesn't provide sh fallback like exec() does; implement
     it here.  POSIX doesn't specify the path to sh so use the default.  */

  if (r == ENOEXEC)
    {
      char **nargv;
      char **pp;
      size_t l = 0;

      for (pp = argv; *pp != NULL; ++pp)
        ++l;

      nargv = xmalloc (sizeof (char *) * (l + 3));
      nargv[0] = (char *)default_shell;
      nargv[1] = cmd;
      memcpy (&nargv[2], &argv[1], sizeof (char *) * l);

      while ((r = posix_spawn (&pid, nargv[0], &fa, &attr, nargv,
                               child->environment)) == EINTR)
        ;

      free (nargv);
    }

  if (r == 0)
    {
      /* Spawn succeeded but may fail later: remember the command.  */
      free (child->cmd_name);
      if (cmd != argv[0])
        child->cmd_name = cmd;
      else
        child->cmd_name = xstrdup(cmd);
    }

 cleanup:
  posix_spawn_file_actions_destroy (&fa);
  posix_spawnattr_destroy (&attr);

 done:
  if (r != 0)
    pid = -1;

#endif /* USE_POSIX_SPAWN */

  if (pid < 0)
    OSS (error, NILF, "%s: %s", argv[0], strerror (r));

  return pid;
}
#endif /* !AMIGA && !__MSDOS__ && !VMS */
#endif /* !WINDOWS32 */

#ifndef _AMIGA
/* Replace the current process with one running the command in ARGV,
   with environment ENVP.  This function does not return.  */

/* EMX: This function returns the pid of the child process.  */
# ifdef __EMX__
pid_t
# else
void
# endif
exec_command (char **argv, char **envp)
{
#ifdef VMS
  /* to work around a problem with signals and execve: ignore them */
#ifdef SIGCHLD
  signal (SIGCHLD,SIG_IGN);
#endif
  /* Run the program.  */
  execve (argv[0], argv, envp);
  OSS (error, NILF, "%s: %s", argv[0], strerror (errno));
  _exit (EXIT_FAILURE);
#else
#ifdef WINDOWS32
  HANDLE hPID;
  HANDLE hWaitPID;
  int exit_code = EXIT_FAILURE;

  /* make sure CreateProcess() has Path it needs */
  sync_Path_environment ();

  /* launch command */
  hPID = process_easy (argv, envp, -1, -1);

  /* make sure launch ok */
  if (hPID == INVALID_HANDLE_VALUE)
    {
      int i;
      fprintf (stderr, _("process_easy() failed to launch process (e=%ld)\n"),
               process_last_err (hPID));
      for (i = 0; argv[i]; i++)
          fprintf (stderr, "%s ", argv[i]);
      fprintf (stderr, _("\nCounted %d args in failed launch\n"), i);
      exit (EXIT_FAILURE);
    }

  /* wait and reap last child */
  hWaitPID = process_wait_for_any (1, 0);
  while (hWaitPID)
    {
      /* was an error found on this process? */
      int err = process_last_err (hWaitPID);

      /* get exit data */
      exit_code = process_exit_code (hWaitPID);

      if (err)
          fprintf (stderr, "make (e=%d, rc=%d): %s",
                   err, exit_code, map_windows32_error_to_string (err));

      /* cleanup process */
      process_cleanup (hWaitPID);

      /* expect to find only last pid, warn about other pids reaped */
      if (hWaitPID == hPID)
          break;
      else
        {
          char *pidstr = xstrdup (pid2str ((pid_t)hWaitPID));

          fprintf (stderr,
                   _("make reaped child pid %s, still waiting for pid %s\n"),
                   pidstr, pid2str ((pid_t)hPID));
          free (pidstr);
        }
    }

  /* return child's exit code as our exit code */
  exit (exit_code);

#else  /* !WINDOWS32 */

# ifdef __EMX__
  pid_t pid;
# endif

  /* Be the user, permanently.  */
  child_access ();

# ifdef __EMX__
  /* Run the program.  */
  pid = spawnvpe (P_NOWAIT, argv[0], argv, envp);
  if (pid >= 0)
    return pid;

  /* the file might have a strange shell extension */
  if (errno == ENOENT)
    errno = ENOEXEC;

# else
  /* Run the program.  */
  environ = envp;
  execvp (argv[0], argv);

# endif /* !__EMX__ */

  switch (errno)
    {
    case ENOENT:
      OSS (error, NILF, "%s: %s", argv[0], strerror (errno));
      break;
    case ENOEXEC:
      {
        /* The file was not a program.  Try it as a shell script.  */
        const char *shell;
        char **new_argv;
        int argc;
        int i=1;

# ifdef __EMX__
        /* Do not use $SHELL from the environment */
        struct variable *p = lookup_variable ("SHELL", 5);
        if (p)
          shell = p->value;
        else
          shell = 0;
# else
        shell = getenv ("SHELL");
# endif
        if (shell == 0)
          shell = default_shell;

        argc = 1;
        while (argv[argc] != 0)
          ++argc;

# ifdef __EMX__
        if (!unixy_shell)
          ++argc;
# endif

        new_argv = alloca ((1 + argc + 1) * sizeof (char *));
        new_argv[0] = (char *)shell;

# ifdef __EMX__
        if (!unixy_shell)
          {
            new_argv[1] = "/c";
            ++i;
            --argc;
          }
# endif

        new_argv[i] = argv[0];
        while (argc > 0)
          {
            new_argv[i + argc] = argv[argc];
            --argc;
          }

# ifdef __EMX__
        pid = spawnvpe (P_NOWAIT, shell, new_argv, envp);
        if (pid >= 0)
          break;
# else
        execvp (shell, new_argv);
# endif
        OSS (error, NILF, "%s: %s", new_argv[0], strerror (errno));
        break;
      }

# ifdef __EMX__
    case EINVAL:
      /* this nasty error was driving me nuts :-( */
      O (error, NILF, _("spawnvpe: environment space might be exhausted"));
      /* FALLTHROUGH */
# endif

    default:
      OSS (error, NILF, "%s: %s", argv[0], strerror (errno));
      break;
    }

# ifdef __EMX__
  return pid;
# else
  _exit (127);
# endif
#endif /* !WINDOWS32 */
#endif /* !VMS */
}
#else /* On Amiga */
void
exec_command (char **argv)
{
  MyExecute (argv);
}

void clean_tmp (void)
{
  DeleteFile (amiga_bname);
}

#endif /* On Amiga */

#ifndef VMS
/* Figure out the argument list necessary to run LINE as a command.  Try to
   avoid using a shell.  This routine handles only ' quoting, and " quoting
   when no backslash, $ or ' characters are seen in the quotes.  Starting
   quotes may be escaped with a backslash.  If any of the characters in
   sh_chars is seen, or any of the builtin commands listed in sh_cmds
   is the first word of a line, the shell is used.

   If RESTP is not NULL, *RESTP is set to point to the first newline in LINE.
   If *RESTP is NULL, newlines will be ignored.

   SHELL is the shell to use, or nil to use the default shell.
   IFS is the value of $IFS, or nil (meaning the default).

   FLAGS is the value of lines_flags for this command line.  It is
   used in the WINDOWS32 port to check whether + or $(MAKE) were found
   in this command line, in which case the effect of just_print_flag
   is overridden.  */

static char **
construct_command_argv_internal (char *line, char **restp, const char *shell,
                                 const char *shellflags, const char *ifs,
                                 int flags, char **batch_filename UNUSED)
{
#ifdef __MSDOS__
  /* MSDOS supports both the stock DOS shell and ports of Unixy shells.
     We call 'system' for anything that requires ''slow'' processing,
     because DOS shells are too dumb.  When $SHELL points to a real
     (unix-style) shell, 'system' just calls it to do everything.  When
     $SHELL points to a DOS shell, 'system' does most of the work
     internally, calling the shell only for its internal commands.
     However, it looks on the $PATH first, so you can e.g. have an
     external command named 'mkdir'.

     Since we call 'system', certain characters and commands below are
     actually not specific to COMMAND.COM, but to the DJGPP implementation
     of 'system'.  In particular:

       The shell wildcard characters are in DOS_CHARS because they will
       not be expanded if we call the child via 'spawnXX'.

       The ';' is in DOS_CHARS, because our 'system' knows how to run
       multiple commands on a single line.

       DOS_CHARS also include characters special to 4DOS/NDOS, so we
       won't have to tell one from another and have one more set of
       commands and special characters.  */
  static const char *sh_chars_dos = "*?[];|<>%^&()";
  static const char *sh_cmds_dos[] =
    { "break", "call", "cd", "chcp", "chdir", "cls", "copy", "ctty", "date",
      "del", "dir", "echo", "erase", "exit", "for", "goto", "if", "md",
      "mkdir", "path", "pause", "prompt", "rd", "rmdir", "rem", "ren",
      "rename", "set", "shift", "time", "type", "ver", "verify", "vol", ":",
      0 };

  static const char *sh_chars_sh = "#;\"*?[]&|<>(){}$`^";
  static const char *sh_cmds_sh[] =
    { "cd", "echo", "eval", "exec", "exit", "login", "logout", "set", "umask",
      "wait", "while", "for", "case", "if", ":", ".", "break", "continue",
      "export", "read", "readonly", "shift", "times", "trap", "switch",
      "unset", "ulimit", "command", 0 };

  const char *sh_chars;
  const char **sh_cmds;

#elif defined (__EMX__)
  static const char *sh_chars_dos = "*?[];|<>%^&()";
  static const char *sh_cmds_dos[] =
    { "break", "call", "cd", "chcp", "chdir", "cls", "copy", "ctty", "date",
      "del", "dir", "echo", "erase", "exit", "for", "goto", "if", "md",
      "mkdir", "path", "pause", "prompt", "rd", "rmdir", "rem", "ren",
      "rename", "set", "shift", "time", "type", "ver", "verify", "vol", ":",
      0 };

  static const char *sh_chars_os2 = "*?[];|<>%^()\"'&";
  static const char *sh_cmds_os2[] =
    { "call", "cd", "chcp", "chdir", "cls", "copy", "date", "del", "detach",
      "dir", "echo", "endlocal", "erase", "exit", "for", "goto", "if", "keys",
      "md", "mkdir", "move", "path", "pause", "prompt", "rd", "rem", "ren",
      "rename", "rmdir", "set", "setlocal", "shift", "start", "time", "type",
      "ver", "verify", "vol", ":", 0 };

  static const char *sh_chars_sh = "#;\"*?[]&|<>(){}$`^~'";
  static const char *sh_cmds_sh[] =
    { "echo", "cd", "eval", "exec", "exit", "login", "logout", "set", "umask",
      "wait", "while", "for", "case", "if", ":", ".", "break", "continue",
      "export", "read", "readonly", "shift", "times", "trap", "switch",
      "unset", "command", 0 };

  const char *sh_chars;
  const char **sh_cmds;

#elif defined (_AMIGA)
  static const char *sh_chars = "#;\"|<>()?*$`";
  static const char *sh_cmds[] =
    { "cd", "eval", "if", "delete", "echo", "copy", "rename", "set", "setenv",
      "date", "makedir", "skip", "else", "endif", "path", "prompt", "unset",
      "unsetenv", "version", "command", 0 };

#elif defined (WINDOWS32)
  /* We used to have a double quote (") in sh_chars_dos[] below, but
     that caused any command line with quoted file names be run
     through a temporary batch file, which introduces command-line
     limit of 4K charcaters imposed by cmd.exe.  Since CreateProcess
     can handle quoted file names just fine, removing the quote lifts
     the limit from a very frequent use case, because using quoted
     file names is commonplace on MS-Windows.  */
  static const char *sh_chars_dos = "|&<>";
  static const char *sh_cmds_dos[] =
    { "assoc", "break", "call", "cd", "chcp", "chdir", "cls", "color", "copy",
      "ctty", "date", "del", "dir", "echo", "echo.", "endlocal", "erase",
      "exit", "for", "ftype", "goto", "if", "if", "md", "mkdir", "move",
      "path", "pause", "prompt", "rd", "rem", "ren", "rename", "rmdir",
      "set", "setlocal", "shift", "time", "title", "type", "ver", "verify",
      "vol", ":", 0 };

  static const char *sh_chars_sh = "#;\"*?[]&|<>(){}$`^";
  static const char *sh_cmds_sh[] =
    { "cd", "eval", "exec", "exit", "login", "logout", "set", "umask", "wait",
      "while", "for", "case", "if", ":", ".", "break", "continue", "export",
      "read", "readonly", "shift", "times", "trap", "switch", "test", "command",
#ifdef BATCH_MODE_ONLY_SHELL
      "echo",
#endif
      0 };

  const char *sh_chars;
  const char **sh_cmds;
#elif defined(__riscos__)
  static const char *sh_chars = "";
  static const char *sh_cmds[] = { 0 };
#else  /* must be UNIX-ish */
  static const char *sh_chars = "#;\"*?[]&|<>(){}$`^~!";
  static const char *sh_cmds[] =
    { ".", ":", "alias", "bg", "break", "case", "cd", "command", "continue",
      "eval", "exec", "exit", "export", "fc", "fg", "for", "getopts", "hash",
      "if", "jobs", "login", "logout", "read", "readonly", "return", "set",
      "shift", "test", "times", "trap", "type", "ulimit", "umask", "unalias",
      "unset", "wait", "while", 0 };

# ifdef HAVE_DOS_PATHS
  /* This is required if the MSYS/Cygwin ports (which do not define
     WINDOWS32) are compiled with HAVE_DOS_PATHS defined, which uses
     sh_chars_sh directly (see below).  The value must be identical
     to that of sh_chars immediately above.  */
  static const char *sh_chars_sh =  "#;\"*?[]&|<>(){}$`^~!";
# endif  /* HAVE_DOS_PATHS */
#endif
  size_t i;
  char *p;
#ifndef NDEBUG
  char *end;
#endif
  char *ap;
  const char *cap;
  const char *cp;
  int instring, word_has_equals, seen_nonequals, last_argument_was_empty;
  char **new_argv = 0;
  char *argstr = 0;
#ifdef WINDOWS32
  int slow_flag = 0;

  if (!unixy_shell)
    {
      sh_cmds = sh_cmds_dos;
      sh_chars = sh_chars_dos;
    }
  else
    {
      sh_cmds = sh_cmds_sh;
      sh_chars = sh_chars_sh;
    }
#endif /* WINDOWS32 */

  if (restp != NULL)
    *restp = NULL;

  /* Make sure not to bother processing an empty line but stop at newline.  */
  while (ISBLANK (*line))
    ++line;
  if (*line == '\0')
    return 0;

  if (shellflags == 0)
    shellflags = posix_pedantic ? "-ec" : "-c";

  /* See if it is safe to parse commands internally.  */
  if (shell == 0)
    shell = default_shell;
#ifdef WINDOWS32
  else if (strcmp (shell, default_shell))
  {
    char *s1 = _fullpath (NULL, shell, 0);
    char *s2 = _fullpath (NULL, default_shell, 0);

    slow_flag = strcmp ((s1 ? s1 : ""), (s2 ? s2 : ""));

    free (s1);
    free (s2);
  }
  if (slow_flag)
    goto slow;
#else  /* not WINDOWS32 */
#if defined (__MSDOS__) || defined (__EMX__)
  else if (strcasecmp (shell, default_shell))
    {
      extern int _is_unixy_shell (const char *_path);

      DB (DB_BASIC, (_("$SHELL changed (was '%s', now '%s')\n"),
                     default_shell, shell));
      unixy_shell = _is_unixy_shell (shell);
      /* we must allocate a copy of shell: construct_command_argv() will free
       * shell after this function returns.  */
      default_shell = xstrdup (shell);
    }
  if (unixy_shell)
    {
      sh_chars = sh_chars_sh;
      sh_cmds  = sh_cmds_sh;
    }
  else
    {
      sh_chars = sh_chars_dos;
      sh_cmds  = sh_cmds_dos;
# ifdef __EMX__
      if (_osmode == OS2_MODE)
        {
          sh_chars = sh_chars_os2;
          sh_cmds = sh_cmds_os2;
        }
# endif
    }
#else  /* !__MSDOS__ */
  else if (strcmp (shell, default_shell))
    goto slow;
#endif /* !__MSDOS__ && !__EMX__ */
#endif /* not WINDOWS32 */

  if (ifs)
    for (cap = ifs; *cap != '\0'; ++cap)
      if (*cap != ' ' && *cap != '\t' && *cap != '\n')
        goto slow;

  if (shellflags)
    if (shellflags[0] != '-'
        || ((shellflags[1] != 'c' || shellflags[2] != '\0')
            && (shellflags[1] != 'e' || shellflags[2] != 'c' || shellflags[3] != '\0')))
      goto slow;

  i = strlen (line) + 1;

  /* More than 1 arg per character is impossible.  */
  new_argv = xmalloc (i * sizeof (char *));

  /* All the args can fit in a buffer as big as LINE is.   */
  ap = new_argv[0] = argstr = xmalloc (i);
#ifndef NDEBUG
  end = ap + i;
#endif

  /* I is how many complete arguments have been found.  */
  i = 0;
  instring = word_has_equals = seen_nonequals = last_argument_was_empty = 0;
  for (p = line; *p != '\0'; ++p)
    {
      assert (ap <= end);

      if (instring)
        {
          /* Inside a string, just copy any char except a closing quote
             or a backslash-newline combination.  */
          if (*p == instring)
            {
              instring = 0;
              if (ap == new_argv[0] || *(ap-1) == '\0')
                last_argument_was_empty = 1;
            }
          else if (*p == '\\' && p[1] == '\n')
            {
              /* Backslash-newline is handled differently depending on what
                 kind of string we're in: inside single-quoted strings you
                 keep them; in double-quoted strings they disappear.  For
                 DOS/Windows/OS2, if we don't have a POSIX shell, we keep the
                 pre-POSIX behavior of removing the backslash-newline.  */
              if (instring == '"'
#if defined (__MSDOS__) || defined (__EMX__) || defined (WINDOWS32)
                  || !unixy_shell
#endif
                  )
                ++p;
              else
                {
                  *(ap++) = *(p++);
                  *(ap++) = *p;
                }
            }
          else if (*p == '\n' && restp != NULL)
            {
              /* End of the command line.  */
              *restp = p;
              goto end_of_line;
            }
          /* Backslash, $, and ` are special inside double quotes.
             If we see any of those, punt.
             But on MSDOS, if we use COMMAND.COM, double and single
             quotes have the same effect.  */
          else if (instring == '"' && strchr ("\\$`", *p) != 0 && unixy_shell)
            goto slow;
#ifdef WINDOWS32
          /* Quoted wildcard characters must be passed quoted to the
             command, so give up the fast route.  */
          else if (instring == '"' && strchr ("*?", *p) != 0 && !unixy_shell)
            goto slow;
          else if (instring == '"' && strncmp (p, "\\\"", 2) == 0)
            *ap++ = *++p;
#endif
          else
            *ap++ = *p;
        }
      else if (strchr (sh_chars, *p) != 0)
        /* Not inside a string, but it's a special char.  */
        goto slow;
      else if (one_shell && *p == '\n')
        /* In .ONESHELL mode \n is a separator like ; or && */
        goto slow;
#ifdef  __MSDOS__
      else if (*p == '.' && p[1] == '.' && p[2] == '.' && p[3] != '.')
        /* '...' is a wildcard in DJGPP.  */
        goto slow;
#endif
      else
        /* Not a special char.  */
        switch (*p)
          {
          case '=':
            /* Equals is a special character in leading words before the
               first word with no equals sign in it.  This is not the case
               with sh -k, but we never get here when using nonstandard
               shell flags.  */
            if (! seen_nonequals && unixy_shell)
              goto slow;
            word_has_equals = 1;
            *ap++ = '=';
            break;

          case '\\':
            /* Backslash-newline has special case handling, ref POSIX.
               We're in the fastpath, so emulate what the shell would do.  */
            if (p[1] == '\n')
              {
                /* Throw out the backslash and newline.  */
                ++p;

                /* At the beginning of the argument, skip any whitespace other
                   than newline before the start of the next word.  */
                if (ap == new_argv[i])
                  while (ISBLANK (p[1]))
                    ++p;
              }
#ifdef WINDOWS32
            /* Backslash before whitespace is not special if our shell
               is not Unixy.  */
            else if (ISSPACE (p[1]) && !unixy_shell)
              {
                *ap++ = *p;
                break;
              }
#endif
            else if (p[1] != '\0')
              {
#ifdef HAVE_DOS_PATHS
                /* Only remove backslashes before characters special to Unixy
                   shells.  All other backslashes are copied verbatim, since
                   they are probably DOS-style directory separators.  This
                   still leaves a small window for problems, but at least it
                   should work for the vast majority of naive users.  */

#ifdef __MSDOS__
                /* A dot is only special as part of the "..."
                   wildcard.  */
                if (strneq (p + 1, ".\\.\\.", 5))
                  {
                    *ap++ = '.';
                    *ap++ = '.';
                    p += 4;
                  }
                else
#endif
                  if (p[1] != '\\' && p[1] != '\''
                      && !ISSPACE (p[1])
                      && strchr (sh_chars_sh, p[1]) == 0)
                    /* back up one notch, to copy the backslash */
                    --p;
#endif  /* HAVE_DOS_PATHS */

                /* Copy and skip the following char.  */
                *ap++ = *++p;
              }
            break;

          case '\'':
          case '"':
            instring = *p;
            break;

          case '\n':
            if (restp != NULL)
              {
                /* End of the command line.  */
                *restp = p;
                goto end_of_line;
              }
            else
              /* Newlines are not special.  */
              *ap++ = '\n';
            break;

          case ' ':
          case '\t':
            /* We have the end of an argument.
               Terminate the text of the argument.  */
            *ap++ = '\0';
            new_argv[++i] = ap;
            last_argument_was_empty = 0;

            /* Update SEEN_NONEQUALS, which tells us if every word
               heretofore has contained an '='.  */
            seen_nonequals |= ! word_has_equals;
            if (word_has_equals && ! seen_nonequals)
              /* An '=' in a word before the first
                 word without one is magical.  */
              goto slow;
            word_has_equals = 0; /* Prepare for the next word.  */

            /* If this argument is the command name,
               see if it is a built-in shell command.
               If so, have the shell handle it.  */
            if (i == 1)
              {
                int j;
                for (j = 0; sh_cmds[j] != 0; ++j)
                  {
                    if (streq (sh_cmds[j], new_argv[0]))
                      goto slow;
#if defined(__EMX__) || defined(WINDOWS32)
                    /* Non-Unix shells are case insensitive.  */
                    if (!unixy_shell
                        && strcasecmp (sh_cmds[j], new_argv[0]) == 0)
                      goto slow;
#endif
                  }
              }

            /* Skip whitespace chars, but not newlines.  */
            while (ISBLANK (p[1]))
              ++p;
            break;

          default:
            *ap++ = *p;
            break;
          }
    }
 end_of_line:

  if (instring)
    /* Let the shell deal with an unterminated quote.  */
    goto slow;

  /* Terminate the last argument and the argument list.  */

  *ap = '\0';
  if (new_argv[i][0] != '\0' || last_argument_was_empty)
    ++i;
  new_argv[i] = 0;

  if (i == 1)
    {
      int j;
      for (j = 0; sh_cmds[j] != 0; ++j)
        if (streq (sh_cmds[j], new_argv[0]))
          goto slow;
    }

  if (new_argv[0] == 0)
    {
      /* Line was empty.  */
      free (argstr);
      free (new_argv);
      return 0;
    }

  return new_argv;

 slow:;
  /* We must use the shell.  */

  if (new_argv != 0)
    {
      /* Free the old argument list we were working on.  */
      free (argstr);
      free (new_argv);
    }

#ifdef __MSDOS__
  execute_by_shell = 1; /* actually, call 'system' if shell isn't unixy */
#endif

#ifdef _AMIGA
  {
    char *ptr;
    char *buffer;
    char *dptr;

    buffer = xmalloc (strlen (line)+1);

    ptr = line;
    for (dptr=buffer; *ptr; )
    {
      if (*ptr == '\\' && ptr[1] == '\n')
        ptr += 2;
      else if (*ptr == '@') /* Kludge: multiline commands */
      {
        ptr += 2;
        *dptr++ = '\n';
      }
      else
        *dptr++ = *ptr++;
    }
    *dptr = 0;

    new_argv = xmalloc (2 * sizeof (char *));
    new_argv[0] = buffer;
    new_argv[1] = 0;
  }
#else   /* Not Amiga  */
#ifdef WINDOWS32
  /*
   * Not eating this whitespace caused things like
   *
   *    sh -c "\n"
   *
   * which gave the shell fits. I think we have to eat
   * whitespace here, but this code should be considered
   * suspicious if things start failing....
   */

  /* Make sure not to bother processing an empty line.  */
  NEXT_TOKEN (line);
  if (*line == '\0')
    return 0;
#endif /* WINDOWS32 */

  {
    /* SHELL may be a multi-word command.  Construct a command line
       "$(SHELL) $(.SHELLFLAGS) LINE", with all special chars in LINE escaped.
       Then recurse, expanding this command line to get the final
       argument list.  */

    char *new_line;
    size_t shell_len = strlen (shell);
    size_t line_len = strlen (line);
    size_t sflags_len = shellflags ? strlen (shellflags) : 0;
#ifdef WINDOWS32
    char *command_ptr = NULL; /* used for batch_mode_shell mode */
#endif

# ifdef __EMX__ /* is this necessary? */
    if (!unixy_shell && shellflags)
      shellflags[0] = '/'; /* "/c" */
# endif

    /* In .ONESHELL mode we are allowed to throw the entire current
        recipe string at a single shell and trust that the user
        has configured the shell and shell flags, and formatted
        the string, appropriately. */
    if (one_shell)
      {
        /* If the shell is Bourne compatible, we must remove and ignore
           interior special chars [@+-] because they're meaningless to
           the shell itself. If, however, we're in .ONESHELL mode and
           have changed SHELL to something non-standard, we should
           leave those alone because they could be part of the
           script. In this case we must also leave in place
           any leading [@+-] for the same reason.  */

        /* Remove and ignore interior prefix chars [@+-] because they're
             meaningless given a single shell. */
#if defined __MSDOS__ || defined (__EMX__)
        if (unixy_shell)     /* the test is complicated and we already did it */
#else
        if (is_bourne_compatible_shell (shell)
#ifdef WINDOWS32
            /* If we didn't find any sh.exe, don't behave is if we did!  */
            && !no_default_sh_exe
#endif
            )
#endif
          {
            const char *f = line;
            char *t = line;

            /* Copy the recipe, removing and ignoring interior prefix chars
               [@+-]: they're meaningless in .ONESHELL mode.  */
            while (f[0] != '\0')
              {
                int esc = 0;

                /* This is the start of a new recipe line.  Skip whitespace
                   and prefix characters but not newlines.  */
                while (ISBLANK (*f) || *f == '-' || *f == '@' || *f == '+')
                  ++f;

                /* Copy until we get to the next logical recipe line.  */
                while (*f != '\0')
                  {
                    *(t++) = *(f++);
                    if (f[-1] == '\\')
                      esc = !esc;
                    else
                      {
                        /* On unescaped newline, we're done with this line.  */
                        if (f[-1] == '\n' && ! esc)
                          break;

                        /* Something else: reset the escape sequence.  */
                        esc = 0;
                      }
                  }
              }
            *t = '\0';
          }
#ifdef WINDOWS32
        else    /* non-Posix shell (cmd.exe etc.) */
          {
            const char *f = line;
            char *t = line;
            char *tstart = t;
            int temp_fd;
            FILE* batch = NULL;
            int id = GetCurrentProcessId ();
            PATH_VAR(fbuf);

            /* Generate a file name for the temporary batch file.  */
            sprintf (fbuf, "make%d", id);
            *batch_filename = create_batch_file (fbuf, 0, &temp_fd);
            DB (DB_JOBS, (_("Creating temporary batch file %s\n"),
                          *batch_filename));

            /* Create a FILE object for the batch file, and write to it the
               commands to be executed.  Put the batch file in TEXT mode.  */
            _setmode (temp_fd, _O_TEXT);
            batch = _fdopen (temp_fd, "wt");
            fputs ("@echo off\n", batch);
            DB (DB_JOBS, (_("Batch file contents:\n\t@echo off\n")));

            /* Copy the recipe, removing and ignoring interior prefix chars
               [@+-]: they're meaningless in .ONESHELL mode.  */
            while (*f != '\0')
              {
                /* This is the start of a new recipe line.  Skip whitespace
                   and prefix characters but not newlines.  */
                while (ISBLANK (*f) || *f == '-' || *f == '@' || *f == '+')
                  ++f;

                /* Copy until we get to the next logical recipe line.  */
                while (*f != '\0')
                  {
                    /* Remove the escaped newlines in the command, and the
                       blanks that follow them.  Windows shells cannot handle
                       escaped newlines.  */
                    if (*f == '\\' && f[1] == '\n')
                      {
                        f += 2;
                        while (ISBLANK (*f))
                          ++f;
                      }
                    *(t++) = *(f++);
                    /* On an unescaped newline, we're done with this
                       line.  */
                    if (f[-1] == '\n')
                      break;
                  }
                /* Write another line into the batch file.  */
                if (t > tstart)
                  {
                    char c = *t;
                    *t = '\0';
                    fputs (tstart, batch);
                    DB (DB_JOBS, ("\t%s", tstart));
                    tstart = t;
                    *t = c;
                  }
              }
            DB (DB_JOBS, ("\n"));
            fclose (batch);

            /* Create an argv list for the shell command line that
               will run the batch file.  */
            new_argv = xmalloc (2 * sizeof (char *));
            new_argv[0] = xstrdup (*batch_filename);
            new_argv[1] = NULL;
            return new_argv;
          }
#endif /* WINDOWS32 */
        /* Create an argv list for the shell command line.  */
        {
          int n = 0;

          new_argv = xmalloc ((4 + sflags_len/2) * sizeof (char *));
          new_argv[n++] = xstrdup (shell);

          /* Chop up the shellflags (if any) and assign them.  */
          if (! shellflags)
            new_argv[n++] = xstrdup ("");
          else
            {
              const char *s = shellflags;
              char *t;
              size_t len;
              while ((t = find_next_token (&s, &len)) != 0)
                new_argv[n++] = xstrndup (t, len);
            }

          /* Set the command to invoke.  */
          new_argv[n++] = line;
          new_argv[n++] = NULL;
        }
        return new_argv;
      }

    new_line = xmalloc ((shell_len*2) + 1 + sflags_len + 1
                        + (line_len*2) + 1);
    ap = new_line;
    /* Copy SHELL, escaping any characters special to the shell.  If
       we don't escape them, construct_command_argv_internal will
       recursively call itself ad nauseam, or until stack overflow,
       whichever happens first.  */
    for (cp = shell; *cp != '\0'; ++cp)
      {
        if (strchr (sh_chars, *cp) != 0)
          *(ap++) = '\\';
        *(ap++) = *cp;
      }
    *(ap++) = ' ';
    if (shellflags)
      memcpy (ap, shellflags, sflags_len);
    ap += sflags_len;
    *(ap++) = ' ';
#ifdef WINDOWS32
    command_ptr = ap;
#endif
    for (p = line; *p != '\0'; ++p)
      {
        if (restp != NULL && *p == '\n')
          {
            *restp = p;
            break;
          }
        else if (*p == '\\' && p[1] == '\n')
          {
            /* POSIX says we keep the backslash-newline.  If we don't have a
               POSIX shell on DOS/Windows/OS2, mimic the pre-POSIX behavior
               and remove the backslash/newline.  */
#if defined (__MSDOS__) || defined (__EMX__) || defined (WINDOWS32)
# define PRESERVE_BSNL  unixy_shell
#else
# define PRESERVE_BSNL  1
#endif
            if (PRESERVE_BSNL)
              {
                *(ap++) = '\\';
                /* Only non-batch execution needs another backslash,
                   because it will be passed through a recursive
                   invocation of this function.  */
                if (!batch_mode_shell)
                  *(ap++) = '\\';
                *(ap++) = '\n';
              }
            ++p;
            continue;
          }

        /* DOS shells don't know about backslash-escaping.  */
        if (unixy_shell && !batch_mode_shell &&
            (*p == '\\' || *p == '\'' || *p == '"'
             || ISSPACE (*p)
             || strchr (sh_chars, *p) != 0))
          *ap++ = '\\';
#ifdef __MSDOS__
        else if (unixy_shell && strneq (p, "...", 3))
          {
            /* The case of '...' wildcard again.  */
            strcpy (ap, "\\.\\.\\");
            ap += 5;
            p  += 2;
          }
#endif
        *ap++ = *p;
      }
    if (ap == new_line + shell_len + sflags_len + 2)
      {
        /* Line was empty.  */
        free (new_line);
        return 0;
      }
    *ap = '\0';

#ifdef WINDOWS32
    /* Some shells do not work well when invoked as 'sh -c xxx' to run a
       command line (e.g. Cygnus GNUWIN32 sh.exe on WIN32 systems).  In these
       cases, run commands via a script file.  */
    if (just_print_flag && !(flags & COMMANDS_RECURSE))
      {
        /* Need to allocate new_argv, although it's unused, because
           start_job_command will want to free it and its 0'th element.  */
        new_argv = xmalloc (2 * sizeof (char *));
        new_argv[0] = xstrdup ("");
        new_argv[1] = NULL;
      }
    else if ((no_default_sh_exe || batch_mode_shell) && batch_filename)
      {
        int temp_fd;
        FILE* batch = NULL;
        int id = GetCurrentProcessId ();
        PATH_VAR (fbuf);

        /* create a file name */
        sprintf (fbuf, "make%d", id);
        *batch_filename = create_batch_file (fbuf, unixy_shell, &temp_fd);

        DB (DB_JOBS, (_("Creating temporary batch file %s\n"),
                      *batch_filename));

        /* Create a FILE object for the batch file, and write to it the
           commands to be executed.  Put the batch file in TEXT mode.  */
        _setmode (temp_fd, _O_TEXT);
        batch = _fdopen (temp_fd, "wt");
        if (!unixy_shell)
          fputs ("@echo off\n", batch);
        fputs (command_ptr, batch);
        fputc ('\n', batch);
        fclose (batch);
        DB (DB_JOBS, (_("Batch file contents:%s\n\t%s\n"),
                      !unixy_shell ? "\n\t@echo off" : "", command_ptr));

        /* create argv */
        new_argv = xmalloc (3 * sizeof (char *));
        if (unixy_shell)
          {
            new_argv[0] = xstrdup (shell);
            new_argv[1] = *batch_filename; /* only argv[0] gets freed later */
          }
        else
          {
            new_argv[0] = xstrdup (*batch_filename);
            new_argv[1] = NULL;
          }
        new_argv[2] = NULL;
      }
    else
#endif /* WINDOWS32 */

    if (unixy_shell)
      new_argv = construct_command_argv_internal (new_line, 0, 0, 0, 0,
                                                  flags, 0);

#ifdef __EMX__
    else if (!unixy_shell)
      {
        /* new_line is local, must not be freed therefore
           We use line here instead of new_line because we run the shell
           manually.  */
        size_t line_len = strlen (line);
        char *p = new_line;
        char *q = new_line;
        memcpy (new_line, line, line_len + 1);
        /* Replace all backslash-newline combination and also following tabs.
           Important: stop at the first '\n' because that's what the loop above
           did. The next line starting at restp[0] will be executed during the
           next call of this function. */
        while (*q != '\0' && *q != '\n')
          {
            if (q[0] == '\\' && q[1] == '\n')
              q += 2; /* remove '\\' and '\n' */
            else
              *p++ = *q++;
          }
        *p = '\0';

# ifndef NO_CMD_DEFAULT
        if (strnicmp (new_line, "echo", 4) == 0
            && (new_line[4] == ' ' || new_line[4] == '\t'))
          {
            /* the builtin echo command: handle it separately */
            size_t echo_len = line_len - 5;
            char *echo_line = new_line + 5;

            /* special case: echo 'x="y"'
               cmd works this way: a string is printed as is, i.e., no quotes
               are removed. But autoconf uses a command like echo 'x="y"' to
               determine whether make works. autoconf expects the output x="y"
               so we will do exactly that.
               Note: if we do not allow cmd to be the default shell
               we do not need this kind of voodoo */
            if (echo_line[0] == '\''
                && echo_line[echo_len - 1] == '\''
                && strncmp (echo_line + 1, "ac_maketemp=",
                            strlen ("ac_maketemp=")) == 0)
              {
                /* remove the enclosing quotes */
                memmove (echo_line, echo_line + 1, echo_len - 2);
                echo_line[echo_len - 2] = '\0';
              }
          }
# endif

        {
          /* Let the shell decide what to do. Put the command line into the
             2nd command line argument and hope for the best ;-)  */
          size_t sh_len = strlen (shell);

          /* exactly 3 arguments + NULL */
          new_argv = xmalloc (4 * sizeof (char *));
          /* Exactly strlen(shell) + strlen("/c") + strlen(line) + 3 times
             the trailing '\0' */
          new_argv[0] = xmalloc (sh_len + line_len + 5);
          memcpy (new_argv[0], shell, sh_len + 1);
          new_argv[1] = new_argv[0] + sh_len + 1;
          memcpy (new_argv[1], "/c", 3);
          new_argv[2] = new_argv[1] + 3;
          memcpy (new_argv[2], new_line, line_len + 1);
          new_argv[3] = NULL;
        }
      }
#elif defined(__MSDOS__)
    else
      {
        /* With MSDOS shells, we must construct the command line here
           instead of recursively calling ourselves, because we
           cannot backslash-escape the special characters (see above).  */
        new_argv = xmalloc (sizeof (char *));
        line_len = strlen (new_line) - shell_len - sflags_len - 2;
        new_argv[0] = xmalloc (line_len + 1);
        strncpy (new_argv[0],
                 new_line + shell_len + sflags_len + 2, line_len);
        new_argv[0][line_len] = '\0';
      }
#else
    else
      fatal (NILF, CSTRLEN (__FILE__) + INTSTR_LENGTH,
             _("%s (line %d) Bad shell context (!unixy && !batch_mode_shell)\n"),
            __FILE__, __LINE__);
#endif

    free (new_line);
  }
#endif  /* ! AMIGA */

  return new_argv;
}
#endif /* !VMS */

/* Figure out the argument list necessary to run LINE as a command.  Try to
   avoid using a shell.  This routine handles only ' quoting, and " quoting
   when no backslash, $ or ' characters are seen in the quotes.  Starting
   quotes may be escaped with a backslash.  If any of the characters in
   sh_chars is seen, or any of the builtin commands listed in sh_cmds
   is the first word of a line, the shell is used.

   If RESTP is not NULL, *RESTP is set to point to the first newline in LINE.
   If *RESTP is NULL, newlines will be ignored.

   FILE is the target whose commands these are.  It is used for
   variable expansion for $(SHELL) and $(IFS).  */

char **
construct_command_argv (char *line, char **restp, struct file *file,
                        int cmd_flags, char **batch_filename)
{
  char *shell, *ifs, *shellflags;
  char **argv;

  {
    /* Turn off --warn-undefined-variables while we expand SHELL and IFS.  */
    int save = warn_undefined_variables_flag;
    warn_undefined_variables_flag = 0;

    shell = allocated_variable_expand_for_file ("$(SHELL)", file);
#ifdef WINDOWS32
    /*
     * Convert to forward slashes so that construct_command_argv_internal()
     * is not confused.
     */
    if (shell)
      {
        char *p = w32ify (shell, 0);
        strcpy (shell, p);
      }
#endif
#ifdef __EMX__
    {
      static const char *unixroot = NULL;
      static const char *last_shell = "";
      static int init = 0;
      if (init == 0)
        {
          unixroot = getenv ("UNIXROOT");
          /* unixroot must be NULL or not empty */
          if (unixroot && unixroot[0] == '\0') unixroot = NULL;
          init = 1;
        }

      /* if we have an unixroot drive and if shell is not default_shell
         (which means it's either cmd.exe or the test has already been
         performed) and if shell is an absolute path without drive letter,
         try whether it exists e.g.: if "/bin/sh" does not exist use
         "$UNIXROOT/bin/sh" instead.  */
      if (unixroot && shell && strcmp (shell, last_shell) != 0
          && (shell[0] == '/' || shell[0] == '\\'))
        {
          /* trying a new shell, check whether it exists */
          size_t size = strlen (shell);
          char *buf = xmalloc (size + 7);
          memcpy (buf, shell, size);
          memcpy (buf + size, ".exe", 5); /* including the trailing '\0' */
          if (access (shell, F_OK) != 0 && access (buf, F_OK) != 0)
            {
              /* try the same for the unixroot drive */
              memmove (buf + 2, buf, size + 5);
              buf[0] = unixroot[0];
              buf[1] = unixroot[1];
              if (access (buf, F_OK) == 0)
                /* we have found a shell! */
                /* free(shell); */
                shell = buf;
              else
                free (buf);
            }
          else
            free (buf);
        }
    }
#endif /* __EMX__ */

    shellflags = allocated_variable_expand_for_file ("$(.SHELLFLAGS)", file);
    ifs = allocated_variable_expand_for_file ("$(IFS)", file);

    warn_undefined_variables_flag = save;
  }

  argv = construct_command_argv_internal (line, restp, shell, shellflags, ifs,
                                          cmd_flags, batch_filename);

  free (shell);
  free (shellflags);
  free (ifs);

  return argv;
}

#if !defined(HAVE_DUP2) && !defined(_AMIGA)
int
dup2 (int old, int new)
{
  int fd;

  (void) close (new);
  EINTRLOOP (fd, dup (old));
  if (fd != new)
    {
      (void) close (fd);
      errno = EMFILE;
      return -1;
    }

  return fd;
}
#endif /* !HAVE_DUP2 && !_AMIGA */

/* On VMS systems, include special VMS functions.  */

#ifdef VMS
#include "vmsjobs.c"
#endif
