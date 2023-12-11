/* unionwait.h -- definitions for using a `union wait' on systems without
   one. */

/* Copyright (C) 1996 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _UNIONWAIT_H
#define _UNIONWAIT_H

#if !defined (WORDS_BIGENDIAN)
union wait
  {
    int	w_status;		/* used in syscall */

    /* Terminated process status. */
    struct
      {
	unsigned short
	  w_Termsig  : 7,	/* termination signal */
	  w_Coredump : 1,	/* core dump indicator */
	  w_Retcode  : 8,	/* exit code if w_termsig==0 */
	  w_Fill1    : 16;	/* high 16 bits unused */
      } w_T;

    /* Stopped process status.  Returned
       only for traced children unless requested
       with the WUNTRACED option bit. */
    struct
      {
	unsigned short
	  w_Stopval : 8,	/* == W_STOPPED if stopped */
	  w_Stopsig : 8,	/* actually zero on XENIX */
	  w_Fill2   : 16;	/* high 16 bits unused */
      } w_S;
  };

#else  /* WORDS_BIGENDIAN */

/* This is for big-endian machines like the IBM RT, HP 9000, or Sun-3 */

union wait
  {
    int	w_status;		/* used in syscall */

    /* Terminated process status. */
    struct
      {
	unsigned short w_Fill1    : 16;	/* high 16 bits unused */
	unsigned       w_Retcode  : 8;	/* exit code if w_termsig==0 */
	unsigned       w_Coredump : 1;	/* core dump indicator */
	unsigned       w_Termsig  : 7;	/* termination signal */
      } w_T;

    /* Stopped process status.  Returned
       only for traced children unless requested
       with the WUNTRACED option bit. */
    struct
      {
	unsigned short w_Fill2   : 16;	/* high 16 bits unused */
	unsigned       w_Stopsig : 8;	/* signal that stopped us */
	unsigned       w_Stopval : 8;	/* == W_STOPPED if stopped */
      } w_S;
  };

#endif /* WORDS_BIGENDIAN */

#define w_termsig  w_T.w_Termsig
#define w_coredump w_T.w_Coredump
#define w_retcode  w_T.w_Retcode
#define w_stopval  w_S.w_Stopval
#define w_stopsig  w_S.w_Stopsig

#define WSTOPPED       0177
#define WIFSTOPPED(x)  ((x).w_stopval == WSTOPPED)
#define WIFEXITED(x)   ((x).w_stopval != WSTOPPED && (x).w_termsig == 0)
#define WIFSIGNALED(x) ((x).w_stopval != WSTOPPED && (x).w_termsig != 0)

#define WTERMSIG(x)    ((x).w_termsig)
#define WSTOPSIG(x)    ((x).w_stopsig)
#define WEXITSTATUS(x) ((x).w_retcode)
#define WIFCORED(x)    ((x).w_coredump)

#endif /* _UNIONWAIT_H */
