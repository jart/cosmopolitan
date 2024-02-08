/* command.h -- The structures used internally to represent commands, and
   the extern declarations of the functions used to create them. */

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

#if !defined (_COMMAND_H_)
#define _COMMAND_H_

#include "stdc.h"

/* Instructions describing what kind of thing to do for a redirection. */
enum r_instruction {
  r_output_direction, r_input_direction, r_inputa_direction,
  r_appending_to, r_reading_until, r_reading_string,
  r_duplicating_input, r_duplicating_output, r_deblank_reading_until,
  r_close_this, r_err_and_out, r_input_output, r_output_force,
  r_duplicating_input_word, r_duplicating_output_word,
  r_move_input, r_move_output, r_move_input_word, r_move_output_word,
  r_append_err_and_out
};

/* Redirection flags; values for rflags */
#define REDIR_VARASSIGN		0x01

/* Redirection errors. */
#define AMBIGUOUS_REDIRECT  -1
#define NOCLOBBER_REDIRECT  -2
#define RESTRICTED_REDIRECT -3	/* can only happen in restricted shells. */
#define HEREDOC_REDIRECT    -4  /* here-doc temp file can't be created */
#define BADVAR_REDIRECT	    -5  /* something wrong with {varname}redir */

#define CLOBBERING_REDIRECT(ri) \
  (ri == r_output_direction || ri == r_err_and_out)

#define OUTPUT_REDIRECT(ri) \
  (ri == r_output_direction || ri == r_input_output || ri == r_err_and_out || ri == r_append_err_and_out)

#define INPUT_REDIRECT(ri) \
  (ri == r_input_direction || ri == r_inputa_direction || ri == r_input_output)

#define WRITE_REDIRECT(ri) \
  (ri == r_output_direction || \
	ri == r_input_output || \
	ri == r_err_and_out || \
	ri == r_appending_to || \
	ri == r_append_err_and_out || \
	ri == r_output_force)

/* redirection needs translation */
#define TRANSLATE_REDIRECT(ri) \
  (ri == r_duplicating_input_word || ri == r_duplicating_output_word || \
   ri == r_move_input_word || ri == r_move_output_word)

/* Command Types: */
enum command_type { cm_for, cm_case, cm_while, cm_if, cm_simple, cm_select,
		    cm_connection, cm_function_def, cm_until, cm_group,
		    cm_arith, cm_cond, cm_arith_for, cm_subshell, cm_coproc };

/* Possible values for the `flags' field of a WORD_DESC. */
#define W_HASDOLLAR	(1 << 0)	/* Dollar sign present. */
#define W_QUOTED	(1 << 1)	/* Some form of quote character is present. */
#define W_ASSIGNMENT	(1 << 2)	/* This word is a variable assignment. */
#define W_SPLITSPACE	(1 << 3)	/* Split this word on " " regardless of IFS */
#define W_NOSPLIT	(1 << 4)	/* Do not perform word splitting on this word because ifs is empty string. */
#define W_NOGLOB	(1 << 5)	/* Do not perform globbing on this word. */
#define W_NOSPLIT2	(1 << 6)	/* Don't split word except for $@ expansion (using spaces) because context does not allow it. */
#define W_TILDEEXP	(1 << 7)	/* Tilde expand this assignment word */
#define W_DOLLARAT	(1 << 8)	/* UNUSED - $@ and its special handling */
#define W_ARRAYREF	(1 << 9)	/* word is a valid array reference */
#define W_NOCOMSUB	(1 << 10)	/* Don't perform command substitution on this word */
#define W_ASSIGNRHS	(1 << 11)	/* Word is rhs of an assignment statement */
#define W_NOTILDE	(1 << 12)	/* Don't perform tilde expansion on this word */
#define W_NOASSNTILDE	(1 << 13)	/* don't do tilde expansion like an assignment statement */
#define W_EXPANDRHS	(1 << 14)	/* Expanding word in ${paramOPword} */
#define W_COMPASSIGN	(1 << 15)	/* Compound assignment */
#define W_ASSNBLTIN	(1 << 16)	/* word is a builtin command that takes assignments */
#define W_ASSIGNARG	(1 << 17)	/* word is assignment argument to command */
#define W_HASQUOTEDNULL	(1 << 18)	/* word contains a quoted null character */
#define W_DQUOTE	(1 << 19)	/* UNUSED - word should be treated as if double-quoted */
#define W_NOPROCSUB	(1 << 20)	/* don't perform process substitution */
#define W_SAWQUOTEDNULL	(1 << 21)	/* word contained a quoted null that was removed */
#define W_ASSIGNASSOC	(1 << 22)	/* word looks like associative array assignment */
#define W_ASSIGNARRAY	(1 << 23)	/* word looks like a compound indexed array assignment */
#define W_ARRAYIND	(1 << 24)	/* word is an array index being expanded */
#define W_ASSNGLOBAL	(1 << 25)	/* word is a global assignment to declare (declare/typeset -g) */
#define W_NOBRACE	(1 << 26)	/* Don't perform brace expansion */
#define W_COMPLETE	(1 << 27)	/* word is being expanded for completion */
#define W_CHKLOCAL	(1 << 28)	/* check for local vars on assignment */
#define W_FORCELOCAL	(1 << 29)	/* force assignments to be to local variables, non-fatal on assignment errors */
/* UNUSED		(1 << 30)	*/

/* Flags for the `pflags' argument to param_expand() and various
   parameter_brace_expand_xxx functions; also used for string_list_dollar_at */
#define PF_NOCOMSUB	0x01	/* Do not perform command substitution */
#define PF_IGNUNBOUND	0x02	/* ignore unbound vars even if -u set */
#define PF_NOSPLIT2	0x04	/* same as W_NOSPLIT2 */
#define PF_ASSIGNRHS	0x08	/* same as W_ASSIGNRHS */
#define PF_COMPLETE	0x10	/* same as W_COMPLETE, sets SX_COMPLETE */
#define PF_EXPANDRHS	0x20	/* same as W_EXPANDRHS */
#define PF_ALLINDS	0x40	/* array, act as if [@] was supplied */

/* Possible values for subshell_environment */
#define SUBSHELL_ASYNC	0x01	/* subshell caused by `command &' */
#define SUBSHELL_PAREN	0x02	/* subshell caused by ( ... ) */
#define SUBSHELL_COMSUB	0x04	/* subshell caused by `command` or $(command) */
#define SUBSHELL_FORK	0x08	/* subshell caused by executing a disk command */
#define SUBSHELL_PIPE	0x10	/* subshell from a pipeline element */
#define SUBSHELL_PROCSUB 0x20	/* subshell caused by <(command) or >(command) */
#define SUBSHELL_COPROC	0x40	/* subshell from a coproc pipeline */
#define SUBSHELL_RESETTRAP 0x80	/* subshell needs to reset trap strings on first call to trap */
#define SUBSHELL_IGNTRAP 0x100  /* subshell should reset trapped signals from trap_handler */

/* A structure which represents a word. */
typedef struct word_desc {
  char *word;		/* Zero terminated string. */
  int flags;		/* Flags associated with this word. */
} WORD_DESC;

/* A linked list of words. */
typedef struct word_list {
  struct word_list *next;
  WORD_DESC *word;
} WORD_LIST;


/* **************************************************************** */
/*								    */
/*			Shell Command Structs			    */
/*								    */
/* **************************************************************** */

/* What a redirection descriptor looks like.  If the redirection instruction
   is ri_duplicating_input or ri_duplicating_output, use DEST, otherwise
   use the file in FILENAME.  Out-of-range descriptors are identified by a
   negative DEST. */

typedef union {
  int dest;			/* Place to redirect REDIRECTOR to, or ... */
  WORD_DESC *filename;		/* filename to redirect to. */
} REDIRECTEE;

/* Structure describing a redirection.  If REDIRECTOR is negative, the parser
   (or translator in redir.c) encountered an out-of-range file descriptor. */
typedef struct redirect {
  struct redirect *next;	/* Next element, or NULL. */
  REDIRECTEE redirector;	/* Descriptor or varname to be redirected. */
  int rflags;			/* Private flags for this redirection */
  int flags;			/* Flag value for `open'. */
  enum r_instruction  instruction; /* What to do with the information. */
  REDIRECTEE redirectee;	/* File descriptor or filename */
  char *here_doc_eof;		/* The word that appeared in <<foo. */
} REDIRECT;

/* An element used in parsing.  A single word or a single redirection.
   This is an ephemeral construct. */
typedef struct element {
  WORD_DESC *word;
  REDIRECT *redirect;
} ELEMENT;

/* Possible values for command->flags. */
#define CMD_WANT_SUBSHELL  0x01	/* User wants a subshell: ( command ) */
#define CMD_FORCE_SUBSHELL 0x02	/* Shell needs to force a subshell. */
#define CMD_INVERT_RETURN  0x04	/* Invert the exit value. */
#define CMD_IGNORE_RETURN  0x08	/* Ignore the exit value.  For set -e. */
#define CMD_NO_FUNCTIONS   0x10 /* Ignore functions during command lookup. */
#define CMD_INHIBIT_EXPANSION 0x20 /* Do not expand the command words. */
#define CMD_NO_FORK	   0x40	/* Don't fork; just call execve */
#define CMD_TIME_PIPELINE  0x80 /* Time a pipeline */
#define CMD_TIME_POSIX	   0x100 /* time -p; use POSIX.2 time output spec. */
#define CMD_AMPERSAND	   0x200 /* command & */
#define CMD_STDIN_REDIR	   0x400 /* async command needs implicit </dev/null */
#define CMD_COMMAND_BUILTIN 0x0800 /* command executed by `command' builtin */
#define CMD_COPROC_SUBSHELL 0x1000
#define CMD_LASTPIPE	    0x2000
#define CMD_STDPATH	    0x4000	/* use standard path for command lookup */
#define CMD_TRY_OPTIMIZING  0x8000	/* try to optimize this simple command */

/* What a command looks like. */
typedef struct command {
  enum command_type type;	/* FOR CASE WHILE IF CONNECTION or SIMPLE. */
  int flags;			/* Flags controlling execution environment. */
  int line;			/* line number the command starts on */
  REDIRECT *redirects;		/* Special redirects for FOR CASE, etc. */
  union {
    struct for_com *For;
    struct case_com *Case;
    struct while_com *While;
    struct if_com *If;
    struct connection *Connection;
    struct simple_com *Simple;
    struct function_def *Function_def;
    struct group_com *Group;
#if defined (SELECT_COMMAND)
    struct select_com *Select;
#endif
#if defined (DPAREN_ARITHMETIC)
    struct arith_com *Arith;
#endif
#if defined (COND_COMMAND)
    struct cond_com *Cond;
#endif
#if defined (ARITH_FOR_COMMAND)
    struct arith_for_com *ArithFor;
#endif
    struct subshell_com *Subshell;
    struct coproc_com *Coproc;
  } value;
} COMMAND;

/* Structure used to represent the CONNECTION type. */
typedef struct connection {
  int ignore;			/* Unused; simplifies make_command (). */
  COMMAND *first;		/* Pointer to the first command. */
  COMMAND *second;		/* Pointer to the second command. */
  int connector;		/* What separates this command from others. */
} CONNECTION;

/* Structures used to represent the CASE command. */

/* Values for FLAGS word in a PATTERN_LIST */
#define CASEPAT_FALLTHROUGH	0x01
#define CASEPAT_TESTNEXT	0x02

/* Pattern/action structure for CASE_COM. */
typedef struct pattern_list {
  struct pattern_list *next;	/* Clause to try in case this one failed. */
  WORD_LIST *patterns;		/* Linked list of patterns to test. */
  COMMAND *action;		/* Thing to execute if a pattern matches. */
  int flags;
} PATTERN_LIST;

/* The CASE command. */
typedef struct case_com {
  int flags;			/* See description of CMD flags. */
  int line;			/* line number the `case' keyword appears on */
  WORD_DESC *word;		/* The thing to test. */
  PATTERN_LIST *clauses;	/* The clauses to test against, or NULL. */
} CASE_COM;

/* FOR command. */
typedef struct for_com {
  int flags;		/* See description of CMD flags. */
  int line;		/* line number the `for' keyword appears on */
  WORD_DESC *name;	/* The variable name to get mapped over. */
  WORD_LIST *map_list;	/* The things to map over.  This is never NULL. */
  COMMAND *action;	/* The action to execute.
			   During execution, NAME is bound to successive
			   members of MAP_LIST. */
} FOR_COM;

#if defined (ARITH_FOR_COMMAND)
typedef struct arith_for_com {
  int flags;
  int line;	/* generally used for error messages */
  WORD_LIST *init;
  WORD_LIST *test;
  WORD_LIST *step;
  COMMAND *action;
} ARITH_FOR_COM;
#endif

#if defined (SELECT_COMMAND)
/* KSH SELECT command. */
typedef struct select_com {
  int flags;		/* See description of CMD flags. */
  int line;		/* line number the `select' keyword appears on */
  WORD_DESC *name;	/* The variable name to get mapped over. */
  WORD_LIST *map_list;	/* The things to map over.  This is never NULL. */
  COMMAND *action;	/* The action to execute.
			   During execution, NAME is bound to the member of
			   MAP_LIST chosen by the user. */
} SELECT_COM;
#endif /* SELECT_COMMAND */

/* IF command. */
typedef struct if_com {
  int flags;			/* See description of CMD flags. */
  COMMAND *test;		/* Thing to test. */
  COMMAND *true_case;		/* What to do if the test returned non-zero. */
  COMMAND *false_case;		/* What to do if the test returned zero. */
} IF_COM;

/* WHILE command. */
typedef struct while_com {
  int flags;			/* See description of CMD flags. */
  COMMAND *test;		/* Thing to test. */
  COMMAND *action;		/* Thing to do while test is non-zero. */
} WHILE_COM;

#if defined (DPAREN_ARITHMETIC)
/* The arithmetic evaluation command, ((...)).  Just a set of flags and
   a WORD_LIST, of which the first element is the only one used, for the
   time being. */
typedef struct arith_com {
  int flags;
  int line;
  WORD_LIST *exp;
} ARITH_COM;
#endif /* DPAREN_ARITHMETIC */

/* The conditional command, [[...]].  This is a binary tree -- we slipped
   a recursive-descent parser into the YACC grammar to parse it. */
#define COND_AND	1
#define COND_OR		2
#define COND_UNARY	3
#define COND_BINARY	4
#define COND_TERM	5
#define COND_EXPR	6

typedef struct cond_com {
  int flags;
  int line;
  int type;
  WORD_DESC *op;
  struct cond_com *left, *right;
} COND_COM;

/* The "simple" command.  Just a collection of words and redirects. */
typedef struct simple_com {
  int flags;			/* See description of CMD flags. */
  int line;			/* line number the command starts on */
  WORD_LIST *words;		/* The program name, the arguments,
				   variable assignments, etc. */
  REDIRECT *redirects;		/* Redirections to perform. */
} SIMPLE_COM;

/* The "function definition" command. */
typedef struct function_def {
  int flags;			/* See description of CMD flags. */
  int line;			/* Line number the function def starts on. */
  WORD_DESC *name;		/* The name of the function. */
  COMMAND *command;		/* The parsed execution tree. */
  char *source_file;		/* file in which function was defined, if any */
} FUNCTION_DEF;

/* A command that is `grouped' allows pipes and redirections to affect all
   commands in the group. */
typedef struct group_com {
  int ignore;			/* See description of CMD flags. */
  COMMAND *command;
} GROUP_COM;

typedef struct subshell_com {
  int flags;
  int line;
  COMMAND *command;
} SUBSHELL_COM;

#define COPROC_RUNNING	0x01
#define COPROC_DEAD	0x02

typedef struct coproc {
  char *c_name;
  pid_t c_pid;
  int c_rfd;
  int c_wfd;
  int c_rsave;
  int c_wsave;
  int c_flags;
  int c_status;
  int c_lock;
} Coproc;

typedef struct coproc_com {
  int flags;
  char *name;
  COMMAND *command;
} COPROC_COM;

extern COMMAND *global_command;
extern Coproc sh_coproc;

/* Possible command errors */
#define CMDERR_DEFAULT	0
#define CMDERR_BADTYPE	1
#define CMDERR_BADCONN	2
#define CMDERR_BADJUMP	3

#define CMDERR_LAST	3

/* Forward declarations of functions declared in copy_cmd.c. */

extern FUNCTION_DEF *copy_function_def_contents PARAMS((FUNCTION_DEF *, FUNCTION_DEF *));
extern FUNCTION_DEF *copy_function_def PARAMS((FUNCTION_DEF *));

extern WORD_DESC *copy_word PARAMS((WORD_DESC *));
extern WORD_LIST *copy_word_list PARAMS((WORD_LIST *));
extern REDIRECT *copy_redirect PARAMS((REDIRECT *));
extern REDIRECT *copy_redirects PARAMS((REDIRECT *));
extern COMMAND *copy_command PARAMS((COMMAND *));

#endif /* _COMMAND_H_ */
