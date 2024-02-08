/* execute_cmd.h - functions from execute_cmd.c. */

/* Copyright (C) 1993-2017 Free Software Foundation, Inc.

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

#if !defined (_EXECUTE_CMD_H_)
#define _EXECUTE_CMD_H_

#include "stdc.h"

#if defined (ARRAY_VARS)
struct func_array_state
  {
    ARRAY *funcname_a;
    SHELL_VAR *funcname_v;
    ARRAY *source_a;
    SHELL_VAR *source_v;
    ARRAY *lineno_a;
    SHELL_VAR *lineno_v;
  };
#endif

/* Placeholder for later expansion to include more execution state */
/* XXX - watch out for pid_t */
struct execstate
  {
    pid_t pid;
    int subshell_env;
  };
	

/* Variables declared in execute_cmd.c, used by many other files */
extern int return_catch_flag;
extern int return_catch_value;
extern volatile int last_command_exit_value;
extern int last_command_exit_signal;
extern int builtin_ignoring_errexit;
extern int executing_builtin;
extern int executing_list;
extern int comsub_ignore_return;
extern int subshell_level;
extern int match_ignore_case;
extern int executing_command_builtin;
extern int funcnest, funcnest_max;
extern int evalnest, evalnest_max;
extern int sourcenest, sourcenest_max;
extern int stdin_redir;
extern int line_number_for_err_trap;

extern char *the_printed_command_except_trap;

extern char *this_command_name;
extern SHELL_VAR *this_shell_function;

/* Functions declared in execute_cmd.c, used by many other files */

extern struct fd_bitmap *new_fd_bitmap PARAMS((int));
extern void dispose_fd_bitmap PARAMS((struct fd_bitmap *));
extern void close_fd_bitmap PARAMS((struct fd_bitmap *));
extern int executing_line_number PARAMS((void));
extern int execute_command PARAMS((COMMAND *));
extern int execute_command_internal PARAMS((COMMAND *, int, int, int, struct fd_bitmap *));
extern int shell_execve PARAMS((char *, char **, char **));
extern void setup_async_signals PARAMS((void));
extern void async_redirect_stdin PARAMS((void));

extern void undo_partial_redirects PARAMS((void));
extern void dispose_partial_redirects PARAMS((void));
extern void dispose_exec_redirects PARAMS((void));

extern int execute_shell_function PARAMS((SHELL_VAR *, WORD_LIST *));

extern struct coproc *getcoprocbypid PARAMS((pid_t));
extern struct coproc *getcoprocbyname PARAMS((const char *));

extern void coproc_init PARAMS((struct coproc *));
extern struct coproc *coproc_alloc PARAMS((char *, pid_t));
extern void coproc_dispose PARAMS((struct coproc *));
extern void coproc_flush PARAMS((void));
extern void coproc_close PARAMS((struct coproc *));
extern void coproc_closeall PARAMS((void));
extern void coproc_reap PARAMS((void));
extern pid_t coproc_active PARAMS((void));

extern void coproc_rclose PARAMS((struct coproc *, int));
extern void coproc_wclose PARAMS((struct coproc *, int));
extern void coproc_fdclose PARAMS((struct coproc *, int));

extern void coproc_checkfd PARAMS((struct coproc *, int));
extern void coproc_fdchk PARAMS((int));

extern void coproc_pidchk PARAMS((pid_t, int));

extern void coproc_fdsave PARAMS((struct coproc *));
extern void coproc_fdrestore PARAMS((struct coproc *));

extern void coproc_setvars PARAMS((struct coproc *));
extern void coproc_unsetvars PARAMS((struct coproc *));

#if defined (PROCESS_SUBSTITUTION)
extern void close_all_files PARAMS((void));
#endif

#if defined (ARRAY_VARS)
extern void restore_funcarray_state PARAMS((struct func_array_state *));
#endif

#endif /* _EXECUTE_CMD_H_ */
