/* Declarations for operating system interfaces for GNU Make.
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

#define IO_UNKNOWN              0x0001
#define IO_COMBINED_OUTERR      0x0002
#define IO_STDIN_OK             0x0004
#define IO_STDOUT_OK            0x0008
#define IO_STDERR_OK            0x0010

#if defined(VMS) || defined(_AMIGA) || defined(__MSDOS__)
# define check_io_state()  (IO_STDIN_OK|IO_STDOUT_OK|IO_STDERR_OK)
# define fd_inherit(_i)    (0)
# define fd_noinherit(_i)  (0)
# define fd_set_append(_i) (void)(0)
# define os_anontmp()      (-1)
#else

/* Determine the state of stdin/stdout/stderr.  */
unsigned int check_io_state (void);

/* Set a file descriptor to close/not close in a subprocess.  */
void fd_inherit (int);
void fd_noinherit (int);

/* If the file descriptor is for a file put it into append mode.  */
void fd_set_append (int);

/* Return a file descriptor for a new anonymous temp file, or -1.  */
int os_anontmp (void);
#endif

/* This section provides OS-specific functions to support the jobserver.  */

#ifdef MAKE_JOBSERVER

/* Returns 1 if the jobserver is enabled, else 0.  */
unsigned int jobserver_enabled (void);

/* Called in the parent make to set up the jobserver initially.  */
unsigned int jobserver_setup (int job_slots, const char *style);

/* Called in a child instance to connect to the jobserver.
   Return 1 if we got a valid auth, else 0.  */
unsigned int jobserver_parse_auth (const char* auth);

/* Returns an allocated buffer used to pass to child instances.  */
char *jobserver_get_auth (void);

/* Returns a pointer to a static string used to indicate that the child
   cannot access the jobserver, or NULL if it always can.  */
const char *jobserver_get_invalid_auth (void);

/* Clear this instance's jobserver configuration.
   This method might be invoked from a signal handler.  */
void jobserver_clear (void);

/* Recover all the jobserver tokens and return the number we got.
   Will also run jobserver_clear() as a side-effect.  */
unsigned int jobserver_acquire_all (void);

/* Release a jobserver token.  If it fails and is_fatal is 1, fatal.  */
void jobserver_release (int is_fatal);

/* Notify the jobserver that a child exited.  */
void jobserver_signal (void);

/* Get ready to start a non-recursive child.  */
void jobserver_pre_child (int);

/* Complete starting a non-recursive child.  */
void jobserver_post_child (int);

/* Set up to acquire a new token.  */
void jobserver_pre_acquire (void);

/* Wait until we can acquire a jobserver token.
   TIMEOUT is 1 if we have other jobs waiting for the load to go down;
   in this case we won't wait forever, so we can check the load.
   Returns 1 if we got a token, or 0 if we stopped waiting due to a child
   exiting or a timeout.    */
unsigned int jobserver_acquire (int timeout);

#else

#define jobserver_enabled()             (0)
#define jobserver_setup(_slots, _style) (0)
#define jobserver_parse_auth(_auth)     (0)
#define jobserver_get_auth()            (NULL)
#define jobserver_get_invalid_auth()    (NULL)
#define jobserver_clear()               (void)(0)
#define jobserver_release(_fatal)       (void)(0)
#define jobserver_acquire_all()         (0)
#define jobserver_signal()              (void)(0)
#define jobserver_pre_child(_r)         (void)(0)
#define jobserver_post_child(_r)        (void)(0)
#define jobserver_pre_acquire()         (void)(0)
#define jobserver_acquire(_tmout)       (0)

#endif  /* MAKE_JOBSERVER */

#ifndef NO_OUTPUT_SYNC

/* Returns 1 if output sync is enabled, else 0.  */
unsigned int osync_enabled (void);

/* Called in the parent make to set up output sync initially.  */
void osync_setup (void);

/* Returns an allocated buffer containing output sync info to pass to child
   instances, or NULL if not needed.  */
char *osync_get_mutex (void);

/* Called in a child instance to obtain info on the output sync mutex.
   Return 1 if we got a valid mutex, else 0.  */
unsigned int osync_parse_mutex (const char *mutex);

/* Clean up this instance's output sync facilities.
   This method might be invoked from a signal handler.  */
void osync_clear (void);

/* Acquire the output sync lock.  This will wait until available.
   Returns 0 if there was an error getting the semaphore.  */
unsigned int osync_acquire (void);

/* Release the output sync lock.  */
void osync_release (void);

#else

#define osync_enabled()       (0)
#define osync_setup()         (void)(0)
#define osync_get_mutex()     (0)
#define osync_parse_mutex(_s) (0)
#define osync_clear()         (void)(0)
#define osync_acquire()       (1)
#define osync_release()       (void)(0)

#endif  /* NO_OUTPUT_SYNC */

/* Create a "bad" file descriptor for stdin when parallel jobs are run.  */
#if defined(VMS) || defined(WINDOWS32) || defined(_AMIGA) || defined(__MSDOS__)
# define get_bad_stdin() (-1)
#else
int get_bad_stdin (void);
#endif
