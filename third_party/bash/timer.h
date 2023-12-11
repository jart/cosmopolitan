/* timer.h -- data structures used by the shell timers in lib/sh/timers.c */

/* Copyright (C) 2021 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "bashjmp.h"
typedef struct _shtimer
{
  struct timeval tmout;

  int fd;
  int flags;

  int alrmflag;					/* should be set by alrm_handler */

  SigHandler *alrm_handler;
  SigHandler *old_handler;

  procenv_t jmpenv;

  int (*tm_handler) (struct _shtimer *);	/* called on timeout if set */
  PTR_T	*data;					/* reserved */
} sh_timer;

#define SHTIMER_ALARM	0x01			/* mutually exclusive */
#define SHTIMER_SELECT	0x02
#define SHTIMER_LONGJMP	0x04

#define SHTIMER_SIGSET	0x100
#define SHTIMER_ALRMSET	0x200

extern sh_timer *shtimer_alloc (void);
extern void shtimer_flush (sh_timer *);
extern void shtimer_dispose (sh_timer *);

extern void shtimer_set (sh_timer *, time_t, long);
extern void shtimer_unset (sh_timer *);

extern void shtimer_cleanup (sh_timer *);
extern void shtimer_clear (sh_timer *);

extern int shtimer_chktimeout (sh_timer *);

extern int shtimer_select (sh_timer *);
extern int shtimer_alrm (sh_timer *);
