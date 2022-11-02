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

#include "third_party/make/makeint.inc"
/**/
#include "third_party/make/debug.h"
#include "third_party/make/filedef.h"
#include "third_party/make/job.h"
/**/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/pledge.h"
#include "libc/calls/pledge.internal.h"
#include "libc/calls/struct/bpf.h"
#include "libc/calls/struct/filter.h"
#include "libc/calls/struct/seccomp.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"
#include "libc/log/rop.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/audit.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/libcxx/math.h"
#include "third_party/make/commands.h"
#include "third_party/make/dep.h"
#include "third_party/make/os.h"
#include "third_party/make/variable.h"
// clang-format off

#ifdef WINDOWS32
const char *default_shell = "sh.exe";
int no_default_sh_exe = 1;
int batch_mode_shell = 1;
HANDLE main_thread;
#else
const char *default_shell = "/bin/sh";
int batch_mode_shell = 0;
#endif

#define WAIT_NOHANG(status)  waitpid (-1, (status), WNOHANG)

#define WAIT_T int

/* Different systems have different requirements for pid_t.
   Plus we have to support gettext string translation... Argh.  */
static const char *
pid2str (pid_t pid)
{
  static char pidstring[100];
  sprintf (pidstring, "%lu", (unsigned long) pid);
  return pidstring;
}

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

/* determines whether path looks to be a Bourne-like shell. */
int
is_bourne_compatible_shell (const char *path)
{
  /* List of known POSIX (or POSIX-ish) shells.  */
  static const char *unix_shells[] = {
    "build/bootstrap/cocmd.com",
    "false",
    "dash",
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


/* [jart] manage temporary directories per rule */

bool
parse_bool (const char *s)
{
  while (isspace (*s))
    ++s;
  if (isdigit (*s))
    return !! atoi (s);
  return _startswithi (s, "true");
}

const char *
get_target_variable (const char *name,
                     size_t length,
                     struct file *file,
                     const char *dflt)
{
  const struct variable *var;
  if ((file &&
       ((var = lookup_variable_in_set (name, length,
                                       file->variables->set)) ||
        (file->pat_variables &&
         (var = lookup_variable_in_set (name, length,
                                        file->pat_variables->set))))) ||
      (var = lookup_variable (name, length)))
    return variable_expand (var->value);
  else
    return dflt;
}

const char *
get_tmpdir (struct file *file)
{
  return get_target_variable (STRING_SIZE_TUPLE ("TMPDIR"), file, 0);
}

char *
new_tmpdir (const char *tmp, struct file *file)
{
  const char *s;
  int c, e, i, j;
  char *dir, *tmpdir;
  char cwd[PATH_MAX];
  char path[PATH_MAX];

  /* create temporary directory in tmp */
  i = 0;

  /* ensure tmpdir will be absolute */
  if (tmp[0] != '/')
    {
      if (getcwd(cwd, sizeof(cwd)))
        {
          for (j = 0; cwd[j]; ++j)
            if (i < PATH_MAX)
              path[i++] = cwd[j];
          if (i && path[i - 1] != '/')
            if (i < PATH_MAX)
              path[i++] = '/';
        }
      else
        DB (DB_JOBS, (_("Failed to get current directory\n")));
    }

  /* copy old tmpdir */
  for (j = 0; tmp[j]; ++j)
    if (i < PATH_MAX)
      path[i++] = tmp[j];

  /* append slash */
  if (i && path[i - 1] != '/')
    if (i < PATH_MAX)
      path[i++] = '/';

  /* append target name safely */
  for (j = 0; (c = file->name[j]); ++j)
    {
      if (isalnum(c))
        c = tolower(c);
      else
        c = '_';
      if (i < PATH_MAX)
        path[i++] = c;
    }

  /* copy random template */
  s = ".XXXXXX";
  for (j = 0; s[j]; ++j)
    if (i < PATH_MAX)
      path[i++] = s[j];

  /* add nul terminator */
  if (i + 11 < PATH_MAX)
    path[i] = 0;
  else
    {
      DB (DB_JOBS, (_("Creating TMPDIR in %s for %s is too long\n"),
                    tmp, file->name));
      return 0;
    }

  /* create temp directory with random data */
  e = errno;
  if (!(tmpdir = mkdtemp (path)) && errno == ENOENT)
    {
      /* create parent directories if necessary */
      char *dir;
      errno = e;
      dir = xstrdup (path);
      if (!makedirs (dirname (dir), 0700))
        tmpdir = mkdtemp (path);
      free (dir);
    }

  /* returned string must be free'd */
  if (tmpdir)
    {
      tmpdir = xstrdup (tmpdir);
      DB (DB_JOBS, (_("Created TMPDIR %s\n"), path));
    }
  else
    DB (DB_JOBS, (_("Creating TMPDIR %s failed %s\n"),
                  path, strerror (errno)));

  return tmpdir;
}

bool
get_file_timestamp (struct file *file)
{
  int e;
  struct stat st;
  EINTRLOOP (e, stat (file->name, &st));
  if (e == 0)
    return FILE_TIMESTAMP_STAT_MODTIME (file->name, st);
  else
    return NONEXISTENT_MTIME;
}

void
delete_tmpdir (struct child *c)
{
  if (!c->tmpdir) return;

  DB (DB_JOBS, (_("Deleting TMPDIR %s\n"), c->tmpdir));

  if (!isdirectory (c->tmpdir))
    DB (DB_JOBS, (_("Warning TMPDIR %s doesn't exist\n"), c->tmpdir));

  errno = 0;
  if (rmrf (c->tmpdir))
    DB (DB_JOBS, (_("Deleting TMPDIR %s failed %s\n"),
                  c->tmpdir, strerror(errno)));

  free (c->tmpdir);
  c->tmpdir = 0;
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
  WAIT_T status;
  /* Initially, assume we have some.  */
  int reap_more = 1;

  /* As long as:

       We have at least one child outstanding OR a shell function in progress,
         AND
       We're blocking for a complete child OR there are more children to reap

     we'll keep reaping children.  */

  while ((children != 0 || shell_function_pid != 0)
         && (block || reap_more))
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
        remote_status_lose:
          pfatal_with_name ("remote_status");
        }
      else
        {
          /* No remote children.  Check for local children.  */
          if (any_local)
            {
              if (!block)
                pid = WAIT_NOHANG (&status);
              else
                EINTRLOOP (pid, wait (&status));
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
            {
              delete_child_targets (c);
              delete_tmpdir (c);
            }
          else if (c->file->touched &&
                   c->file->touched != get_file_timestamp (c->file))
            /* If file was created just so it could be sandboxed, then
               delete that file even if .DELETE_ON_ERROR isn't used,
               but only if the command hasn't modified it.  */
            unlink (c->file->name);
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
                {
                  /* We failed to start the commands.  */
                  delete_child_targets (c);
                  delete_tmpdir (c);
                }
            }
          else
            {
              /* There are no more commands.  We got through them all
                 without an unignored error.  Now the target has been
                 successfully updated.  */
              c->file->update_status = us_success;
              delete_tmpdir (c);
            }
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

  free (child->tmpdir);
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
# define FREE_ARGV(_a) do{ if (_a) { free ((_a)[0]); free (_a); } }while(0)
  char **argv;

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
    argv = construct_command_argv (p, &end, child->file,
                                   child->file->cmds->lines_flags[child->command_line - 1],
                                   &child->sh_batch_file);
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
          child->file->update_status = us_question;
          notice_finished_file (child->file);
          return;
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

  if (
      (argv[0] && is_bourne_compatible_shell (argv[0]))
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

  /* Set up the environment for the child.  */
  if (child->environment == 0)
    child->environment = target_environment (child->file);

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
    {
      /* Fork the child process.  */
      char **parent_environ;
    run_local:
      block_sigs ();
      child->remote = 0;
      parent_environ = environ;
      jobserver_pre_child (flags & COMMANDS_RECURSE);
      child->pid = child_execute_job ((struct childbase *)child,
                                      child->good_stdin, argv, true);
      environ = parent_environ; /* Restore value child may have clobbered.  */
      jobserver_post_child (flags & COMMANDS_RECURSE);
    }

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
  struct variable *var;
  struct child *c;
  unsigned int i;
  char **lines;

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

  /* [jart] manage temporary directories per rule */
  if ((c->tmpdir = get_tmpdir (file)) &&
      (c->tmpdir = new_tmpdir (c->tmpdir, file)))
    {
      var = define_variable_for_file ("TMPDIR", 6, c->tmpdir,
                                      o_override, 0, file);
      var->export = v_export;
    }

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
  double load, guess;
  time_t now;

  if (max_load_average < 0)
    return 0;

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


bool
get_perm_prefix (const char *path, char out_perm[5], const char **out_path)
{
  int c, n;
  for (n = 0;;)
    switch ((c = *path++)) {
    case 'r':
    case 'w':
    case 'c':
    case 'x':
      out_perm[n++] = c;
      out_perm[n] = 0;
      break;
    case ':':
      if (n)
        {
          *out_path = path;
          return true;
        }
      else
        return false;
    default:
      return false;
    }
}

/* Adds path to sandbox, returning true if found. */
int
Unveil (const char *path, const char *perm)
{
  int e;
  char *fp[2];
  char permprefix[5];

  /* if path is like `rwcx:o/tmp` then `rwcx` will override perm */
  if (path && get_perm_prefix (path, permprefix, &path))
    perm = permprefix;

  fp[0] = 0;
  fp[1] = 0;
  if (path && path[0] == '~' &&
      (fp[1] = tilde_expand ((fp[0] = xstrdup (path)))))
    path = fp[1];

  DB (DB_JOBS, (_("Unveiling %s with permissions %s\n"), path, perm));

  e = errno;
  if (unveil (path, perm) != -1)
    {
      free(fp[0]);
      free(fp[1]);
      return 0;
    }

  /* path not found isn't really much of an error */
  if (errno == ENOENT)
    {
      free(fp[0]);
      free(fp[1]);
      errno = e;
      return 0;
    }

  /* otherwise fail */
  OSS (error, NILF, "%s: unveil() failed %s", path, strerror (errno));
  free(fp[0]);
  free(fp[1]);
  return -1;
}

int
unveil_variable (const struct variable *var)
{
  char *val, *tok, *state, *start;
  if (!var) return 0;
  start = val = xstrdup (variable_expand (var->value));
  while ((tok = strtok_r (start, " \t\r\n", &state)))
    {
      RETURN_ON_ERROR (Unveil (tok, "r"));
      start = 0;
    }
  free(val);
  return 0;
 OnError:
  return -1;
}

int
get_base_cpu_freq_mhz (void)
{
  return KCPUIDS(16H, EAX) & 0x7fff;
}

int
set_limit (int r, long lo, long hi)
{
  struct rlimit old;
  struct rlimit lim = {lo, hi};
  if (!setrlimit (r, &lim))
    return 0;
  if (getrlimit (r, &old))
    return -1;
  lim.rlim_cur = MIN (lim.rlim_cur, old.rlim_max);
  lim.rlim_max = MIN (lim.rlim_max, old.rlim_max);
  return setrlimit (r, &lim);
}

int
set_cpu_limit (int secs)
{
  int mhz, lim;
  if (secs <= 0) return 0;
  if (!(mhz = get_base_cpu_freq_mhz())) return eopnotsupp();
  lim = ceil(3100. / mhz * secs);
  return set_limit (RLIMIT_CPU, lim, lim + 1);
}

static struct sysinfo g_sysinfo;

__attribute__((__constructor__)) static void
get_sysinfo (void)
{
  int e = errno;
  sysinfo (&g_sysinfo);
  errno = e;
}

static bool internet;
static char *promises;

/* POSIX:
   Create a child process executing the command in ARGV.
   Returns the PID or -1.  */
pid_t
child_execute_job (struct childbase *child,
                   int good_stdin,
                   char **argv,
                   bool is_build_rule)
{
  const int fdin = good_stdin ? FD_STDIN : get_bad_stdin ();
  struct dep *d;
  bool strict;
  bool sandboxed;
  bool unsandboxed;
  struct child *c;
  unsigned long ipromises;
  char pathbuf[PATH_MAX];
  char outpathbuf[PATH_MAX];
  int fdout = FD_STDOUT;
  int fderr = FD_STDERR;
  pid_t pid;
  int e, r;
  char *s;

  /* Divert child output if we want to capture it.  */
  if (child->output.syncout)
    {
      if (child->output.out >= 0)
        fdout = child->output.out;
      if (child->output.err >= 0)
        fderr = child->output.err;
    }

  pid = fork();
  if (pid != 0)
    return pid;

  /* We are the child.  */
  unblock_all_sigs ();

  /* Reset limits, if necessary.  */
  if (stack_limit.rlim_cur)
    setrlimit (RLIMIT_STACK, &stack_limit);

  /* Tell build rules apart from $(shell foo).  */
  if (is_build_rule) {
    c = (struct child *)child;
  } else {
    c = 0;
  }

  if (c)
    {
      strict = parse_bool (get_target_variable
                           (STRING_SIZE_TUPLE (".STRICT"),
                            c->file, "0"));
      internet = !strict ||
                 parse_bool (get_target_variable
                             (STRING_SIZE_TUPLE (".INTERNET"),
                              c->file, "0"));
      unsandboxed = !strict ||
                    parse_bool (get_target_variable
                                (STRING_SIZE_TUPLE (".UNSANDBOXED"),
                                 c->file, "0"));
    }
  else
    {
      strict = false;
      internet = true;
      unsandboxed = true;
    }

  sandboxed = !unsandboxed;

  if (sandboxed)
    {
      promises = emptytonull (get_target_variable
                              (STRING_SIZE_TUPLE (".PLEDGE"),
                               c ? c->file : 0, 0));
      if (promises)
        promises = xstrdup (promises);
      if (ParsePromises (promises, &ipromises))
        {
          OSS (error, NILF, "%s: invalid .PLEDGE string: %s",
               argv[0], strerror (errno));
          _Exit (127);
        }
    }
  else
    {
      promises = NULL;
      ipromises = 0;
    }

  DB (DB_JOBS, 
      (_("Executing %s for %s%s%s%s\n"),
       argv[0], c ? c->file->name : "$(shell)",
       sandboxed ? " with sandboxing" : " without sandboxing",
       strict ? " in .STRICT mode" : "",
       internet ? " with internet access" : ""));

  /* [jart] Set cpu seconds quota.  */
  if (RLIMIT_CPU < RLIM_NLIMITS &&
      (s = get_target_variable (STRING_SIZE_TUPLE (".CPU"),
                                c ? c->file : 0, 0)))
    {
      int secs;
      secs = atoi (s);
      if (!set_cpu_limit (secs))
        DB (DB_JOBS, (_("Set cpu limit of %d seconds\n"), secs));
      else
        DB (DB_JOBS, (_("Failed to set CPU limit: %s\n"), strerror (errno)));
    }

  /* [jart] Set virtual memory quota.  */
  if (RLIMIT_AS < RLIM_NLIMITS &&
      (s = get_target_variable (STRING_SIZE_TUPLE (".MEMORY"),
                                c ? c->file : 0, 0)))
    {
      long bytes;
      char buf[16];
      errno = 0;
      if (!strchr (s, '%'))
        bytes = sizetol (s, 1024);
      else
        bytes = strtod (s, 0) / 100. * g_sysinfo.totalram;
      if (bytes > 0)
        {
          if (!set_limit (RLIMIT_AS, bytes, bytes))
            DB (DB_JOBS, (_("Set virtual memory limit of %sb\n"),
                          (sizefmt (buf, bytes, 1024), buf)));
          else
            DB (DB_JOBS, (_("Failed to set virtual memory: %s\n"),
                          strerror (errno)));
        }
      else if (errno)
        {
          OSS (error, NILF, "%s: .MEMORY invalid: %s",
               argv[0], strerror (errno));
          _Exit (127);
        }
    }

  /* [jart] Set resident memory quota.  */
  if (RLIMIT_RSS < RLIM_NLIMITS &&
      (s = get_target_variable (STRING_SIZE_TUPLE (".RSS"),
                                c ? c->file : 0, 0)))
    {
      long bytes;
      char buf[16];
      errno = 0;
      if (!strchr (s, '%'))
        bytes = sizetol (s, 1024);
      else
        bytes = strtod (s, 0) / 100. * g_sysinfo.totalram;
      if (bytes > 0)
        {
          if (!set_limit (RLIMIT_RSS, bytes, bytes))
            DB (DB_JOBS, (_("Set resident memory limit of %sb\n"),
                          (sizefmt (buf, bytes, 1024), buf)));
          else
            DB (DB_JOBS, (_("Failed to set resident memory: %s\n"),
                          strerror (errno)));
        }
      else if (errno)
        {
          OSS (error, NILF, "%s: .RSS invalid: %s",
               argv[0], strerror (errno));
          _Exit (127);
        }
    }

  /* [jart] Set file size limit.  */
  if (RLIMIT_FSIZE < RLIM_NLIMITS &&
      (s = get_target_variable (STRING_SIZE_TUPLE (".FSIZE"),
                                c ? c->file : 0, 0)))
    {
      long bytes;
      char buf[16];
      errno = 0;
      if ((bytes = sizetol (s, 1000)) > 0)
        {
          if (!set_limit (RLIMIT_FSIZE, bytes, bytes * 1.5))
            DB (DB_JOBS, (_("Set file size limit of %sb\n"),
                          (sizefmt (buf, bytes, 1000), buf)));
          else
            DB (DB_JOBS, (_("Failed to set file size limit: %s\n"),
                          strerror (errno)));
        }
      else if (errno)
        {
          OSS (error, NILF, "%s: .FSIZE invalid: %s",
               argv[0], strerror (errno));
          _Exit (127);
        }
    }

  /* [jart] Set core dump limit.  */
  if (RLIMIT_CORE < RLIM_NLIMITS &&
      (s = get_target_variable (STRING_SIZE_TUPLE (".MAXCORE"),
                                c ? c->file : 0, 0)))
    {
      long bytes;
      char buf[16];
      errno = 0;
      if ((bytes = sizetol (s, 1000)) > 0)
        {
          if (!set_limit (RLIMIT_CORE, bytes, bytes))
            DB (DB_JOBS, (_("Set core dump limit of %sb\n"),
                          (sizefmt (buf, bytes, 1000), buf)));
          else
            DB (DB_JOBS, (_("Failed to set core dump limit: %s\n"),
                          strerror (errno)));
        }
      else if (errno)
        {
          OSS (error, NILF, "%s: .MAXCORE invalid: %s",
               argv[0], strerror (errno));
          _Exit (127);
        }
    }

  /* [jart] Set process limit.  */
  if (RLIMIT_NPROC < RLIM_NLIMITS &&
      (s = get_target_variable (STRING_SIZE_TUPLE (".NPROC"),
                                c ? c->file : 0, 0)))
    {
      int procs;
      if ((procs = atoi (s)) > 0)
        {
          if (!set_limit (RLIMIT_NPROC,
                          procs + g_sysinfo.procs,
                          procs + g_sysinfo.procs))
            DB (DB_JOBS, (_("Set process limit to %d + %d preexisting\n"),
                          procs, g_sysinfo.procs));
          else
            DB (DB_JOBS, (_("Failed to set process limit: %s\n"),
                          strerror (errno)));
        }
    }

  /* [jart] Set file descriptor limit.  */
  if (RLIMIT_NOFILE < RLIM_NLIMITS &&
      (s = get_target_variable (STRING_SIZE_TUPLE (".NOFILE"),
                                c ? c->file : 0, 0)))
    {
      int fds;
      if ((fds = atoi (s)) > 0)
        {
          if (!set_limit (RLIMIT_NOFILE, fds, fds))
            DB (DB_JOBS, (_("Set file descriptor limit to %d\n"), fds));
          else
            DB (DB_JOBS, (_("Failed to set process limit: %s\n"),
                          strerror (errno)));
        }
    }

  /* [jart] Prevent builds from talking to the Internet.  */
  if (internet)
    DB (DB_JOBS, (_("Allowing Internet access\n")));
  else if (!(~ipromises & (1ul << PROMISE_INET)) &&
           !(~ipromises & (1ul << PROMISE_DNS)))
    DB (DB_JOBS, (_("Internet access will be blocked by pledge\n")));
  else
    {
      e = errno;
      if (!nointernet())
        DB (DB_JOBS, (_("Blocked Internet access with seccomp ptrace\n")));
      else
        {
          if (errno = EPERM)
            {
              errno = e;
              DB (DB_JOBS, (_("Can't block Internet if already traced\n")));
            }
          else if (errno == ENOSYS)
            {
              errno = e;
              DB (DB_JOBS, (_("Need SECCOMP ptrace() to block Internet\n")));
            }
          else
            {
              OSS (error, NILF, "%s: failed to block internet access: %s",
                   argv[0], strerror (errno));
              _Exit (127);
            }
        }
    }

  /* [jart] Resolve command into executable path.  */
  if (!strict || !sandboxed)
    {
      if ((s = commandv (argv[0], pathbuf, sizeof (pathbuf))))
        argv[0] = s;
      else
        {
          OSS (error, NILF, "%s: command not found on $PATH: %s",
               argv[0], strerror (errno));
          _Exit (127);
        }
    }

  /* [jart] Sandbox build rule commands based on prerequisites.  */
  if (c)
    {
      errno = 0;
      if (sandboxed)
        {
          /*
           * permit launching actually portable executables
           *
           * we assume launching make.com already did the expensive
           * work of extracting the ape loader program, via /bin/sh
           * and we won't need to do that again, since sys_execve()
           * will pass ape binaries directly to the ape loader, but
           * only if the ape loader exists on a well-known path.
           */
          e = errno;
          DB (DB_JOBS, (_("Unveiling %s with permissions %s\n"),
                        "/usr/bin/ape", "rx"));
          if (unveil ("/usr/bin/ape", "rx") == -1)
          {
            char *s, *t;
            errno = e;
            if ((s = getenv ("TMPDIR")))
            {
              t = xjoinpaths (s, ".ape");
              RETURN_ON_ERROR (Unveil (t, "rx"));
              free (t);
            }
            if ((s = getenv ("HOME")))
            {
              t = xjoinpaths (s, ".ape");
              RETURN_ON_ERROR (Unveil (t, "rx"));
              free (t);
            }
          }

          /* Unveil executable.  */
          RETURN_ON_ERROR (Unveil (argv[0], "rx"));

          /* Unveil temporary directory.  */
          if (c->tmpdir)
            RETURN_ON_ERROR (Unveil (c->tmpdir, "rwcx"));

          /* Unveil .PLEDGE = vminfo.  */
          if (promises && (~ipromises & (1ul << PROMISE_VMINFO)))
            {
              RETURN_ON_ERROR (Unveil ("/proc/stat", "r"));
              RETURN_ON_ERROR (Unveil ("/proc/meminfo", "r"));
              RETURN_ON_ERROR (Unveil ("/proc/cpuinfo", "r"));
              RETURN_ON_ERROR (Unveil ("/proc/diskstats", "r"));
              RETURN_ON_ERROR (Unveil ("/proc/self/maps", "r"));
              RETURN_ON_ERROR (Unveil ("/sys/devices/system/cpu", "r"));
            }

          /* Unveil .PLEDGE = tty.  */
          if (promises && (~ipromises & (1ul << PROMISE_TTY)))
            {
              RETURN_ON_ERROR (Unveil (ttyname(0), "rw"));
              RETURN_ON_ERROR (Unveil ("/dev/tty", "rw"));
              RETURN_ON_ERROR (Unveil ("/dev/console", "rw"));
              RETURN_ON_ERROR (Unveil ("/etc/terminfo", "r"));
              RETURN_ON_ERROR (Unveil ("/usr/lib/terminfo", "r"));
              RETURN_ON_ERROR (Unveil ("/usr/share/terminfo", "r"));
            }

          /* Unveil .PLEDGE = dns.  */
          if (promises && (~ipromises & (1ul << PROMISE_DNS)))
            {
              RETURN_ON_ERROR (Unveil ("/etc/hosts", "r"));
              RETURN_ON_ERROR (Unveil ("/etc/hostname", "r"));
              RETURN_ON_ERROR (Unveil ("/etc/services", "r"));
              RETURN_ON_ERROR (Unveil ("/etc/protocols", "r"));
              RETURN_ON_ERROR (Unveil ("/etc/resolv.conf", "r"));
            }

          /* Unveil .PLEDGE = inet.  */
          if (promises && (~ipromises & (1ul << PROMISE_INET)))
            RETURN_ON_ERROR (Unveil ("/etc/ssl/certs/ca-certificates.crt", "r"));

          /* Unveil .PLEDGE = rpath.  */
          if (promises && (~ipromises & (1ul << PROMISE_RPATH)))
            RETURN_ON_ERROR (Unveil ("/proc/filesystems", "r"));

          /*
           * unveils target output file
           *
           * landlock operates per inode so it can't whitelist missing
           * paths. so we create the output file manually, and prevent
           * creation so that it can't be deleted by the command which
           * must truncate when writing its output.
           */
          if (!c->file->phony &&
              strlen(c->file->name) < PATH_MAX)
            {
              int fd, rc, err;
              if (c->file->last_mtime == NONEXISTENT_MTIME)
                {
                  strcpy (outpathbuf, c->file->name);
                  err = errno;
                  if (makedirs (dirname (outpathbuf), 0777) == -1)
                    errno = err;
                  fd = open (c->file->name, O_RDWR | O_CREAT, 0777);
                  if (fd != -1)
                    close (fd);
                  else if (errno == EEXIST)
                    errno = err;
                  else
                    {
                      OSS (error, NILF, "%s: touch target failed %s",
                           c->file->name, strerror (errno));
                      _Exit (127);
                    }
                  c->file->touched = get_file_timestamp (c->file);
                }
              DB (DB_JOBS, (_("Unveiling %s with permissions %s\n"),
                            c->file->name, "rwx"));
              if (unveil (c->file->name, "rwx") && errno != ENOSYS)
                {
                  OSS (error, NILF, "%s: unveil target failed %s",
                       c->file->name, strerror (errno));
                  _Exit (127);
                }
            }

          /*
           * unveil target prerequisites
           *
           * directories get special treatment:
           *
           *   - libc/nt
           *     shall unveil everything beneath dir
           *
           *   - libc/nt/
           *     no sandboxing due to trailing slash
           *     intended to be timestamp check only
           */
          for (d = c->file->deps; d; d = d->next)
            {
              size_t n;
              n = strlen (d->file->name);
              if (n && d->file->name[n - 1] == '/')
                continue;
              RETURN_ON_ERROR (Unveil (d->file->name, "rx"));
              if (n > 4 && READ32LE(d->file->name + n - 4) == READ32LE(".com"))
                {
                  s = xstrcat (d->file->name, ".dbg");
                  RETURN_ON_ERROR (Unveil (s, "rx"));
                  free (s);
                }
            }

          /* unveil explicit .UNVEIL entries */
          RETURN_ON_ERROR
            (unveil_variable
             (lookup_variable
              (STRING_SIZE_TUPLE (".UNVEIL"))));
          RETURN_ON_ERROR
            (unveil_variable
             (lookup_variable_in_set
              (STRING_SIZE_TUPLE (".UNVEIL"),
               c->file->variables->set)));
          if (c->file->pat_variables)
            RETURN_ON_ERROR
              (unveil_variable
               (lookup_variable_in_set
                (STRING_SIZE_TUPLE (".UNVEIL"),
                 c->file->pat_variables->set)));

          /* commit sandbox */
          RETURN_ON_ERROR (Unveil (0, 0));
        }
    }

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

 OnError:
  _Exit (127);
}


/* Replace the current process with one running the command in ARGV,
   with environment ENVP.  This function does not return.  */
void
exec_command (char **argv, char **envp)
{
  /* Be the user, permanently.  */
  child_access ();

  /* Restrict system calls.  */
  if (promises)
    {
      __pledge_mode = PLEDGE_PENALTY_RETURN_EPERM;
      DB (DB_JOBS, (_("Pledging %s\n"), promises));
      promises = xstrcat (promises, " prot_exec exec");
      if (pledge (promises, promises))
        {
          OSS (error, NILF, "pledge(%s) failed: %s",
               promises, strerror (errno));
          _Exit (127);
        }
    }

  /* Run the program.  */
  environ = envp;
  execv (argv[0], argv);

  if(errno == ENOENT)
    OSS (error, NILF, "%s: command doesn't exist: %s",
         argv[0], strerror (errno));
  else if(errno == ENOEXEC)
  {
    /* The file was not a program.  Try it as a shell script.  */
    const char *shell;
    char **new_argv;
    int argc;
    int i=1;

    shell = getenv ("SHELL");
    if (shell == 0)
      shell = default_shell;

    argc = 1;
    while (argv[argc] != 0)
      ++argc;

    new_argv = alloca ((1 + argc + 1) * sizeof (char *));
    new_argv[0] = (char *)shell;

    new_argv[i] = argv[0];
    while (argc > 0)
    {
      new_argv[i + argc] = argv[argc];
      --argc;
    }

    execvp (shell, new_argv);
    OSS (error, NILF, "%s: execvp shell failed: %s",
         new_argv[0], strerror (errno));
  }

  OSS (error, NILF, "%s: execv failed: %s",
       argv[0], strerror (errno));

  _Exit (127);
}


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
  static const char *sh_chars = "#;\"*?[]&|<>(){}$`^~!";
  static const char *sh_cmds[] =
    { ".", ":", "alias", "bg", "break", "case", "cd", "command", "continue",
      "eval", "exec", "exit", "export", "fc", "fg", "for", "getopts", "hash",
      "if", "jobs", "login", "logout", "read", "readonly", "return", "set",
      "shift", "test", "times", "trap", "type", "ulimit", "umask", "unalias",
      "unset", "wait", "while", 0 };
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
  
  /* [jart] remove code that forces slow path if not using /bin/sh */

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
      // assert (ap <= end);

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
          else
            *ap++ = *p;
        }
      else if (strchr (sh_chars, *p) != 0)
        /* Not inside a string, but it's a special char.  */
        goto slow;
      else if (one_shell && *p == '\n')
        /* In .ONESHELL mode \n is a separator like ; or && */
        goto slow;
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
            else if (p[1] != '\0')
              {
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

  {
    /* SHELL may be a multi-word command.  Construct a command line
       "$(SHELL) $(.SHELLFLAGS) LINE", with all special chars in LINE escaped.
       Then recurse, expanding this command line to get the final
       argument list.  */

    char *new_line;
    size_t shell_len = strlen (shell);
    size_t line_len = strlen (line);
    size_t sflags_len = shellflags ? strlen (shellflags) : 0;

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
        if (is_bourne_compatible_shell (shell))
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
    else
      fatal (NILF, CSTRLEN (__FILE__) + INTSTR_LENGTH,
             _("%s (line %d) Bad shell context (!unixy && !batch_mode_shell)\n"),
            __FILE__, __LINE__);
    free (new_line);
  }

  return new_argv;
}

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
