/* Declarations for operating system interfaces for GNU Make.
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


/* This section provides OS-specific functions to support the jobserver.  */

#ifdef MAKE_JOBSERVER

/* Returns 1 if the jobserver is enabled, else 0.  */
unsigned int jobserver_enabled (void);

/* Called in the master instance to set up the jobserver initially.  */
unsigned int jobserver_setup (int job_slots);

/* Called in a child instance to connect to the jobserver.  */
unsigned int jobserver_parse_auth (const char* auth);

/* Returns an allocated buffer used to pass to child instances.  */
char *jobserver_get_auth (void);

/* Clear this instance's jobserver configuration.  */
void jobserver_clear (void);

/* Recover all the jobserver tokens and return the number we got.  */
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

#define jobserver_enabled()         (0)
#define jobserver_setup(_slots)     (0)
#define jobserver_parse_auth(_auth) (0)
#define jobserver_get_auth()        (NULL)
#define jobserver_clear()           (void)(0)
#define jobserver_release(_fatal)   (void)(0)
#define jobserver_acquire_all()     (0)
#define jobserver_signal()          (void)(0)
#define jobserver_pre_child(_r)     (void)(0)
#define jobserver_post_child(_r)    (void)(0)
#define jobserver_pre_acquire()     (void)(0)
#define jobserver_acquire(_tmout)   (0)

#endif

/* Create a "bad" file descriptor for stdin when parallel jobs are run.  */
#if defined(VMS) || defined(WINDOWS32) || defined(_AMIGA) || defined(__MSDOS__)
# define get_bad_stdin() (-1)
#else
int get_bad_stdin (void);
#endif

/* Set a file descriptor to close/not close in a subprocess.  */
#if defined(VMS) || defined(_AMIGA) || defined(__MSDOS__)
# define fd_inherit(_i)   0
# define fd_noinherit(_i) 0
#else
void fd_inherit (int);
void fd_noinherit (int);
#endif
