/* bashjmp.h -- wrapper for setjmp.h with necessary bash definitions. */

/* Copyright (C) 1987-2021 Free Software Foundation, Inc.

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

#ifndef _BASHJMP_H_
#define _BASHJMP_H_

#include "posixjmp.h"

extern procenv_t	top_level;
extern procenv_t	subshell_top_level;
extern procenv_t	return_catch;	/* used by `return' builtin */
extern procenv_t	wait_intr_buf;

extern int no_longjmp_on_fatal_error;

#define SHFUNC_RETURN()	sh_longjmp (return_catch, 1)

#define COPY_PROCENV(old, save) \
	xbcopy ((char *)old, (char *)save, sizeof (procenv_t));

/* Values for the second argument to longjmp/siglongjmp. */
#define NOT_JUMPED	0	/* Not returning from a longjmp. */
#define FORCE_EOF	1	/* We want to stop parsing. */
#define DISCARD		2	/* Discard current command. */
#define EXITPROG	3	/* Unconditionally exit the program now. */
#define ERREXIT		4	/* Exit due to error condition */	
#define SIGEXIT		5	/* Exit due to fatal terminating signal */
#define EXITBLTIN	6	/* Exit due to the exit builtin. */

#endif /* _BASHJMP_H_ */
