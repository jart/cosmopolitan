/* Windows32-based operating system interface for GNU Make.
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
#include <string.h>

#include <windows.h>
#include <process.h>
#include <io.h>
#include "pathstuff.h"
#include "sub_proc.h"
#include "w32err.h"
#include "os.h"
#include "debug.h"

/* This section provides OS-specific functions to support the jobserver.  */

static char jobserver_semaphore_name[MAX_PATH + 1];
static HANDLE jobserver_semaphore = NULL;

unsigned int
jobserver_setup (int slots)
{
  /* sub_proc.c is limited in the number of objects it can wait for. */

  if (slots > process_table_usable_size())
    {
      slots = process_table_usable_size();
      DB (DB_JOBS, (_("Jobserver slots limited to %d\n"), slots));
    }

  sprintf (jobserver_semaphore_name, "gmake_semaphore_%d", _getpid ());

  jobserver_semaphore = CreateSemaphore (
      NULL,                           /* Use default security descriptor */
      slots,                          /* Initial count */
      slots,                          /* Maximum count */
      jobserver_semaphore_name);      /* Semaphore name */

  if (jobserver_semaphore == NULL)
    {
      DWORD err = GetLastError ();
      const char *estr = map_windows32_error_to_string (err);
      ONS (fatal, NILF,
           _("creating jobserver semaphore: (Error %ld: %s)"), err, estr);
    }

  return 1;
}

unsigned int
jobserver_parse_auth (const char *auth)
{
  jobserver_semaphore = OpenSemaphore (
      SEMAPHORE_ALL_ACCESS,   /* Semaphore access setting */
      FALSE,                  /* Child processes DON'T inherit */
      auth);                  /* Semaphore name */

  if (jobserver_semaphore == NULL)
    {
      DWORD err = GetLastError ();
      const char *estr = map_windows32_error_to_string (err);
      fatal (NILF, strlen (auth) + INTSTR_LENGTH + strlen (estr),
             _("internal error: unable to open jobserver semaphore '%s': (Error %ld: %s)"),
             auth, err, estr);
    }
  DB (DB_JOBS, (_("Jobserver client (semaphore %s)\n"), auth));

  return 1;
}

char *
jobserver_get_auth ()
{
  return xstrdup (jobserver_semaphore_name);
}

unsigned int
jobserver_enabled ()
{
  return jobserver_semaphore != NULL;
}

/* Close jobserver semaphore */
void
jobserver_clear ()
{
  if (jobserver_semaphore != NULL)
    {
      CloseHandle (jobserver_semaphore);
      jobserver_semaphore = NULL;
    }
}

void
jobserver_release (int is_fatal)
{
  if (! ReleaseSemaphore (
          jobserver_semaphore,    /* handle to semaphore */
          1,                      /* increase count by one */
          NULL))                  /* not interested in previous count */
    {
      if (is_fatal)
        {
          DWORD err = GetLastError ();
          const char *estr = map_windows32_error_to_string (err);
          ONS (fatal, NILF,
               _("release jobserver semaphore: (Error %ld: %s)"), err, estr);
        }
      perror_with_name ("release_jobserver_semaphore", "");
    }
}

unsigned int
jobserver_acquire_all ()
{
  unsigned int tokens = 0;
  while (1)
    {
      DWORD dwEvent = WaitForSingleObject (
          jobserver_semaphore,    /* Handle to semaphore */
          0);                     /* DON'T wait on semaphore */

      if (dwEvent != WAIT_OBJECT_0)
        return tokens;

      ++tokens;
    }
}

void
jobserver_signal ()
{
}

void jobserver_pre_child (int recursive)
{
}

void jobserver_post_child (int recursive)
{
}

void
jobserver_pre_acquire ()
{
}

/* Returns 1 if we got a token, or 0 if a child has completed.
   The Windows implementation doesn't support load detection.  */
unsigned int
jobserver_acquire (int timeout)
{
    HANDLE *handles;
    DWORD dwHandleCount;
    DWORD dwEvent;

    handles = xmalloc(process_table_actual_size() * sizeof(HANDLE));

    /* Add jobserver semaphore to first slot. */
    handles[0] = jobserver_semaphore;

    /* Build array of handles to wait for.  */
    dwHandleCount = 1 + process_set_handles (&handles[1]);

    dwEvent = process_wait_for_multiple_objects (
        dwHandleCount,  /* number of objects in array */
        handles,        /* array of objects */
        FALSE,          /* wait for any object */
        INFINITE);      /* wait until object is signalled */

    free(handles);

    if (dwEvent == WAIT_FAILED)
      {
        DWORD err = GetLastError ();
        const char *estr = map_windows32_error_to_string (err);
        ONS (fatal, NILF,
             _("semaphore or child process wait: (Error %ld: %s)"),
             err, estr);
      }

    /* WAIT_OBJECT_0 indicates that the semaphore was signalled.  */
    return dwEvent == WAIT_OBJECT_0;
}

void
fd_inherit(int fd)
{
  HANDLE fh = (HANDLE)_get_osfhandle(fd);

  if (fh && fh != INVALID_HANDLE_VALUE)
        SetHandleInformation(fh, HANDLE_FLAG_INHERIT, 1);
}

void
fd_noinherit(int fd)
{
  HANDLE fh = (HANDLE)_get_osfhandle(fd);

  if (fh && fh != INVALID_HANDLE_VALUE)
        SetHandleInformation(fh, HANDLE_FLAG_INHERIT, 0);
}
