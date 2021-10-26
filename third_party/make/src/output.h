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

struct output
  {
    int out;
    int err;
    unsigned int syncout:1;     /* True if we want to synchronize output.  */
 };

extern struct output *output_context;
extern unsigned int stdio_traced;

#define FD_STDIN  (fileno (stdin))
#define FD_STDOUT (fileno (stdout))
#define FD_STDERR (fileno (stderr))

#define OUTPUT_SET(_new)    do{ output_context = (_new)->syncout ? (_new) : NULL; }while(0)
#define OUTPUT_UNSET()      do{ output_context = NULL; }while(0)

#define OUTPUT_TRACED()     do{ stdio_traced = 1; }while(0)
#define OUTPUT_IS_TRACED()  (!!stdio_traced)

/* Write a buffer directly to the given file descriptor.
   This handles errors etc.  */
int output_write (int fd, const void *buffer, size_t len);

/* Initialize and close a child output structure: if NULL do this program's
   output (this should only be done once).  */
void output_init (struct output *out);
void output_close (struct output *out);

/* In situations where output may be about to be displayed but we're not
   sure if we've set it up yet, call this.  */
void output_start (void);

/* Show a message on stdout or stderr.  Will start the output if needed.  */
void outputs (int is_err, const char *msg);

#if defined(HAVE_FCNTL_H)
# include <fcntl.h>
#elif defined(HAVE_SYS_FILE_H)
# include <sys/file.h>
#endif

#ifdef NO_OUTPUT_SYNC
# define RECORD_SYNC_MUTEX(m) \
    O (error, NILF,                                                    \
       _("-O[TYPE] (--output-sync[=TYPE]) is not configured for this build."));
#else
int output_tmpfd (void);
/* Dump any child output content to stdout, and reset it.  */
void output_dump (struct output *out);

# ifdef WINDOWS32
/* For emulations in w32/compat/posixfcn.c.  */
#  define F_GETFD 1
#  define F_SETLKW 2
/* Implementation note: None of the values of l_type below can be zero
   -- they are compared with a static instance of the struct, so zero
   means unknown/invalid, see w32/compat/posixfcn.c. */
#  define F_WRLCK 1
#  define F_UNLCK 2

struct flock
  {
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t l_pid;
  };

/* This type is actually a HANDLE, but we want to avoid including
   windows.h as much as possible.  */
typedef intptr_t sync_handle_t;

/* Public functions emulated/provided in posixfcn.c.  */
int fcntl (intptr_t fd, int cmd, ...);
intptr_t create_mutex (void);
int same_stream (FILE *f1, FILE *f2);

#  define RECORD_SYNC_MUTEX(m) record_sync_mutex(m)
void record_sync_mutex (const char *str);
void prepare_mutex_handle_string (intptr_t hdl);
# else  /* !WINDOWS32 */

typedef int sync_handle_t;      /* file descriptor */

#  define RECORD_SYNC_MUTEX(m) (void)(m)

# endif
#endif  /* !NO_OUTPUT_SYNC */
