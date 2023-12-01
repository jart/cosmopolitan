/* GNU Make remote job exportation interface to the Customs daemon.
   THIS CODE IS NOT SUPPORTED BY THE GNU PROJECT.
   Please do not send bug reports or questions about it to
   the Make maintainers.

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
#include "debug.h"

#if HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#include <netdb.h>

#include "customs.h"

char *remote_description = "Customs";

/* File name of the Customs 'export' client command.
   A full path name can be used to avoid some path-searching overhead.  */
#define EXPORT_COMMAND  "/usr/local/bin/export"

/* ExportPermit gotten by start_remote_job_p, and used by start_remote_job.  */
static ExportPermit permit;

/* Normalized path name of the current directory.  */
static char *normalized_cwd;

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
start_remote_job_p (int first_p)
{
  static int inited = 0;
  int status;
  int njobs;

  if (!inited)
    {
      /* Allow the user to turn off job exportation (useful while he is
         debugging Customs, for example).  */
      if (getenv ("GNU_MAKE_NO_CUSTOMS") != 0)
        {
          inited = -1;
          return 0;
        }

      if (ISDB (DB_JOBS))
        Rpc_Debug (1);

      /* Ping the daemon once to see if it is there.  */
      inited = Customs_Ping () == RPC_SUCCESS ? 1 : -1;

      if (starting_directory == 0)
        /* main couldn't figure it out.  */
        inited = -1;
      else
        {
          /* Normalize the current directory path name to something
             that should work on all machines exported to.  */

          normalized_cwd = xmalloc (GET_PATH_MAX);
          strcpy (normalized_cwd, starting_directory);
          if (Customs_NormPath (normalized_cwd, GET_PATH_MAX) < 0)
            /* Path normalization failure means using Customs
               won't work, but it's not really an error.  */
            inited = -1;
        }
    }

  if (inited < 0)
    return 0;

  njobs = job_slots_used;
  if (!first_p)
    njobs -= 1;         /* correction for being called from reap_children() */

  /* the first job should run locally, or, if the -l flag is given, we use
     that as clue as to how many local jobs should be scheduled locally */
  if (max_load_average < 0 && njobs == 0 || njobs < max_load_average)
     return 0;

  status = Customs_Host (EXPORT_SAME, &permit);
  if (status != RPC_SUCCESS)
    {
      DB (DB_JOBS, (_("Customs won't export: %s\n"),
                    Rpc_ErrorMessage (status)));
      return 0;
    }

  return !CUSTOMS_FAIL (&permit.addr);
}

/* Start a remote job running the command in ARGV, with environment from
   ENVP.  It gets standard input from STDIN_FD.  On failure, return
   nonzero.  On success, return zero, and set *USED_STDIN to nonzero if it
   will actually use STDIN_FD, zero if not, set *ID_PTR to a unique
   identification, and set *IS_REMOTE to nonzero if the job is remote, zero
   if it is local (meaning *ID_PTR is a process ID).  */

int
start_remote_job (char **argv, char **envp, int stdin_fd,
                  int *is_remote, pid_t *id_ptr, int *used_stdin)
{
  char waybill[MAX_DATA_SIZE], msg[128];
  struct hostent *host;
  struct timeval timeout;
  struct sockaddr_in sin;
  int len;
  int retsock, retport, sock;
  Rpc_Stat status;
  pid_t pid;

  /* Create the return socket.  */
  retsock = Rpc_UdpCreate (True, 0);
  if (retsock < 0)
    {
      O (error, NILF, "exporting: Couldn't create return socket.");
      return 1;
    }

  /* Get the return socket's port number.  */
  len = sizeof (sin);
  if (getsockname (retsock, (struct sockaddr *) &sin, &len) < 0)
    {
      (void) close (retsock);
      perror_with_name ("exporting: ", "getsockname");
      return 1;
    }
  retport = sin.sin_port;

  /* Create the TCP socket for talking to the remote child.  */
  sock = Rpc_TcpCreate (False, 0);

  /* Create a WayBill to give to the server.  */
  len = Customs_MakeWayBill (&permit, normalized_cwd, argv[0], argv,
                             envp, retport, waybill);

  /* Modify the waybill for the child's uid/gid.  */
  {
    WayBill *wb = (WayBill *) waybill;
    wb->ruid = wb->euid;
    wb->rgid = wb->egid;
  }

  /* Send the request to the server, timing out in 20 seconds.  */
  timeout.tv_usec = 0;
  timeout.tv_sec = 20;
  sin.sin_family = AF_INET;
  sin.sin_port = htons (Customs_Port ());
  sin.sin_addr = permit.addr;
  status = Rpc_Call (sock, &sin, (Rpc_Proc) CUSTOMS_IMPORT,
                     len, (Rpc_Opaque) waybill,
                     sizeof (msg), (Rpc_Opaque) msg,
                     1, &timeout);

  host = gethostbyaddr ((char *)&permit.addr, sizeof(permit.addr), AF_INET);

  {
    const char *hnm = host ? host->h_name : inet_ntoa (permit.addr);
    size_t hlen = strlen (hnm);

    if (status != RPC_SUCCESS)
      {
        const char *err = Rpc_ErrorMessage (status);
        (void) close (retsock);
        (void) close (sock);
        error (NILF, hlen + strlen (err),
               "exporting to %s: %s", hnm, err);
        return 1;
      }
    else if (msg[0] != 'O' || msg[1] != 'k' || msg[2] != '\0')
      {
        (void) close (retsock);
        (void) close (sock);
        error (NILF, hlen + strlen (msg), "exporting to %s: %s", hnm, msg);
        return 1;
      }
    else
      {
        error (NILF, hlen + INTSTR_LENGTH,
               "*** exported to %s (id %u)", hnm, permit.id);
      }

    fflush (stdout);
    fflush (stderr);
  }

  pid = vfork ();
  if (pid < 0)
    {
      /* The fork failed!  */
      perror_with_name ("fork", "");
      return 1;
    }
  else if (pid == 0)
    {
      /* Child side.  Run 'export' to handle the connection.  */
      static char sock_buf[INTSTR_LENGTH], retsock_buf[INTSTR_LENGTH];
      static char id_buf[INTSTR_LENGTH];
      static char *new_argv[6] =
        { EXPORT_COMMAND, "-id", sock_buf, retsock_buf, id_buf, 0 };

      /* Set up the arguments.  */
      (void) sprintf (sock_buf, "%d", sock);
      (void) sprintf (retsock_buf, "%d", retsock);
      (void) sprintf (id_buf, "%x", permit.id);

      /* Get the right stdin.  */
      if (stdin_fd != 0)
        (void) dup2 (stdin_fd, 0);

      /* Unblock signals in the child.  */
      unblock_all_sigs ();

      /* Run the command.  */
      exec_command (new_argv, envp);
    }

  /* Parent side.  Return the 'export' process's ID.  */
  (void) close (retsock);
  (void) close (sock);
  *is_remote = 0;
  *id_ptr = pid;
  *used_stdin = 1;
  return 0;
}

/* Get the status of a dead remote child.  Block waiting for one to die
   if BLOCK is nonzero.  Set *EXIT_CODE_PTR to the exit status, *SIGNAL_PTR
   to the termination signal or zero if it exited normally, and *COREDUMP_PTR
   nonzero if it dumped core.  Return the ID of the child that died,
   0 if we would have to block and !BLOCK, or < 0 if there were none.  */

int
remote_status (int *exit_code_ptr, int *signal_ptr, int *coredump_ptr,
               int block)
{
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
remote_kill (pid_t id, int sig)
{
  return -1;
}
