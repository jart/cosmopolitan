/* Template for the remote job exportation interface to GNU Make.
Copyright (C) 1988-2023 Free Software Foundation, Inc.
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
#include "filedef.h"
#include "job.h"
#include "commands.h"


char *remote_description = 0;

/* Call once at startup even if no commands are run.  */

void
remote_setup (void)
{
}

/* Called before exit.  */

void
remote_cleanup (void)
{
}

/* Return nonzero if the next job should be done remotely.  */

int
start_remote_job_p (int first_p UNUSED)
{
  return 0;
}

/* Start a remote job running the command in ARGV,
   with environment from ENVP.  It gets standard input from STDIN_FD.  On
   failure, return nonzero.  On success, return zero, and set *USED_STDIN
   to nonzero if it will actually use STDIN_FD, zero if not, set *ID_PTR to
   a unique identification, and set *IS_REMOTE to zero if the job is local,
   nonzero if it is remote (meaning *ID_PTR is a process ID).  */

int
start_remote_job (char **argv UNUSED, char **envp UNUSED, int stdin_fd UNUSED,
                  int *is_remote UNUSED, pid_t *id_ptr UNUSED,
                  int *used_stdin UNUSED)
{
  return -1;
}

/* Get the status of a dead remote child.  Block waiting for one to die
   if BLOCK is nonzero.  Set *EXIT_CODE_PTR to the exit status, *SIGNAL_PTR
   to the termination signal or zero if it exited normally, and *COREDUMP_PTR
   nonzero if it dumped core.  Return the ID of the child that died,
   0 if we would have to block and !BLOCK, or < 0 if there were none.  */

int
remote_status (int *exit_code_ptr UNUSED, int *signal_ptr UNUSED,
               int *coredump_ptr UNUSED, int block UNUSED)
{
  errno = ECHILD;
  return -1;
}

/* Block asynchronous notification of remote child death.
   If this notification is done by raising the child termination
   signal, do not block that signal.  */
void
block_remote_children (void)
{
  return;
}

/* Restore asynchronous notification of remote child death.
   If this is done by raising the child termination signal,
   do not unblock that signal.  */
void
unblock_remote_children (void)
{
  return;
}

/* Send signal SIG to child ID.  Return 0 if successful, -1 if not.  */
int
remote_kill (pid_t id UNUSED, int sig UNUSED)
{
  return -1;
}
