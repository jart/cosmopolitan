/* shell.h -- The data structures used by the shell */

/* Copyright (C) 1993-2021 Free Software Foundation, Inc.

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

#include "command.h"
#include "syntax.h"
#include "general.h"
#include "error.h"
#include "variables.h"
#include "arrayfunc.h"
#include "quit.h"
#include "maxpath.h"
#include "unwind_prot.h"
#include "dispose_cmd.h"
#include "make_cmd.h"
#include "ocache.h"
#include "subst.h"
#include "sig.h"
#include "pathnames.h"
#include "externs.h"

extern int EOF_Reached;

#define NO_PIPE -1
#define REDIRECT_BOTH -2

#define NO_VARIABLE -1

/* Values that can be returned by execute_command (). */
#define EXECUTION_FAILURE 1
#define EXECUTION_SUCCESS 0

/* Usage messages by builtins result in a return status of 2. */
#define EX_BADUSAGE	2

#define EX_MISCERROR	2

/* Special exit statuses used by the shell, internally and externally. */
#define EX_RETRYFAIL	124
#define EX_WEXPCOMSUB	125
#define EX_BINARY_FILE	126
#define EX_NOEXEC	126
#define EX_NOINPUT	126
#define EX_NOTFOUND	127

#define EX_SHERRBASE	256	/* all special error values are > this. */

#define EX_BADSYNTAX	257	/* shell syntax error */
#define EX_USAGE	258	/* syntax error in usage */
#define EX_REDIRFAIL	259	/* redirection failed */
#define EX_BADASSIGN	260	/* variable assignment error */
#define EX_EXPFAIL	261	/* word expansion failed */
#define EX_DISKFALLBACK	262	/* fall back to disk command from builtin */

/* Flag values that control parameter pattern substitution. */
#define MATCH_ANY	0x000
#define MATCH_BEG	0x001
#define MATCH_END	0x002

#define MATCH_TYPEMASK	0x003

#define MATCH_GLOBREP	0x010
#define MATCH_QUOTED	0x020
#define MATCH_ASSIGNRHS	0x040
#define MATCH_STARSUB	0x080
#define MATCH_EXPREP	0x100	/* for pattern substitution, expand replacement */

/* Some needed external declarations. */
extern char **shell_environment;
extern WORD_LIST *rest_of_args;

/* Generalized global variables. */
extern char *command_execution_string;

extern int debugging_mode;
extern int executing, login_shell;
extern int interactive, interactive_shell;
extern int startup_state;
extern int reading_shell_script;
extern int shell_initialized;
extern int bash_argv_initialized;
extern int subshell_environment;
extern int current_command_number;
extern int indirection_level;
extern int shell_compatibility_level;
extern int running_under_emacs;

extern int posixly_correct;
extern int no_line_editing;

extern char *shell_name;
extern char *current_host_name;

extern int subshell_argc;
extern char **subshell_argv;
extern char **subshell_envp;

/* variables managed using shopt */
extern int hup_on_exit;
extern int check_jobs_at_exit;
extern int autocd;
extern int check_window_size;

/* from version.c */
extern int build_version, patch_level;
extern char *dist_version, *release_status;

extern int locale_mb_cur_max;
extern int locale_utf8locale;

/* Structure to pass around that holds a bitmap of file descriptors
   to close, and the size of that structure.  Used in execute_cmd.c. */
struct fd_bitmap {
  int size;
  char *bitmap;
};

#define FD_BITMAP_SIZE 32

#define CTLESC '\001'
#define CTLNUL '\177'

/* Information about the current user. */
struct user_info {
  uid_t uid, euid;
  gid_t gid, egid;
  char *user_name;
  char *shell;		/* shell from the password file */
  char *home_dir;
};

extern struct user_info current_user;

/* Force gcc to not clobber X on a longjmp().  Old versions of gcc mangle
   this badly. */
#if (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ > 8)
#  define USE_VAR(x)	((void) &(x))
#else
#  define USE_VAR(x)
#endif

#define HEREDOC_MAX 16

/* Structure in which to save partial parsing state when doing things like
   PROMPT_COMMAND and bash_execute_unix_command execution. */

typedef struct _sh_parser_state_t
{
  /* parsing state */
  int parser_state;
  int *token_state;

  char *token;
  size_t token_buffer_size;
  int eof_token;

  /* input line state -- line number saved elsewhere */
  int input_line_terminator;
  int eof_encountered;
  int eol_lookahead;

#if defined (HANDLE_MULTIBYTE)
  /* Nothing right now for multibyte state, but might want something later. */
#endif

  char **prompt_string_pointer;

  /* history state affecting or modified by the parser */
  int current_command_line_count;
#if defined (HISTORY)
  int remember_on_history;
  int history_expansion_inhibited;
#endif

  /* execution state possibly modified by the parser */
  int last_command_exit_value;
#if defined (ARRAY_VARS)
  ARRAY *pipestatus;
#endif
  sh_builtin_func_t *last_shell_builtin, *this_shell_builtin;

  /* flags state affecting the parser */
  int expand_aliases;
  int echo_input_at_read;
  int need_here_doc;
  int here_doc_first_line;

  int esacs_needed;
  int expecting_in;

  /* structures affecting the parser */
  void *pushed_strings;
  REDIRECT *redir_stack[HEREDOC_MAX];
} sh_parser_state_t;

typedef struct _sh_input_line_state_t
{
  char *input_line;
  size_t input_line_index;
  size_t input_line_size;
  size_t input_line_len;
#if defined (HANDLE_MULTIBYTE)
  char *input_property;
  size_t input_propsize;
#endif
} sh_input_line_state_t;

/* Let's try declaring these here. */
extern void shell_ungets PARAMS((char *));
extern void rewind_input_string PARAMS((void));

extern char *parser_remaining_input PARAMS((void));

extern sh_parser_state_t *save_parser_state PARAMS((sh_parser_state_t *));
extern void restore_parser_state PARAMS((sh_parser_state_t *));

extern sh_input_line_state_t *save_input_line_state PARAMS((sh_input_line_state_t *));
extern void restore_input_line_state PARAMS((sh_input_line_state_t *));
