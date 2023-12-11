/* make_cmd.h -- Declarations of functions found in make_cmd.c */

/* Copyright (C) 1993-2009,2020  Free Software Foundation, Inc.

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

#if !defined (_MAKE_CMD_H_)
#define _MAKE_CMD_H_

#include "stdc.h"

extern int here_doc_first_line;

extern void cmd_init PARAMS((void));

extern WORD_DESC *alloc_word_desc PARAMS((void));
extern WORD_DESC *make_bare_word PARAMS((const char *));
extern WORD_DESC *make_word_flags PARAMS((WORD_DESC *, const char *));
extern WORD_DESC *make_word PARAMS((const char *));
extern WORD_DESC *make_word_from_token PARAMS((int));

extern WORD_LIST *make_word_list PARAMS((WORD_DESC *, WORD_LIST *));

#define add_string_to_list(s, l) make_word_list (make_word(s), (l))

extern COMMAND *make_command PARAMS((enum command_type, SIMPLE_COM *));
extern COMMAND *command_connect PARAMS((COMMAND *, COMMAND *, int));
extern COMMAND *make_for_command PARAMS((WORD_DESC *, WORD_LIST *, COMMAND *, int));
extern COMMAND *make_group_command PARAMS((COMMAND *));
extern COMMAND *make_case_command PARAMS((WORD_DESC *, PATTERN_LIST *, int));
extern PATTERN_LIST *make_pattern_list PARAMS((WORD_LIST *, COMMAND *));
extern COMMAND *make_if_command PARAMS((COMMAND *, COMMAND *, COMMAND *));
extern COMMAND *make_while_command PARAMS((COMMAND *, COMMAND *));
extern COMMAND *make_until_command PARAMS((COMMAND *, COMMAND *));
extern COMMAND *make_bare_simple_command PARAMS((void));
extern COMMAND *make_simple_command PARAMS((ELEMENT, COMMAND *));
extern void make_here_document PARAMS((REDIRECT *, int));
extern REDIRECT *make_redirection PARAMS((REDIRECTEE, enum r_instruction, REDIRECTEE, int));
extern COMMAND *make_function_def PARAMS((WORD_DESC *, COMMAND *, int, int));
extern COMMAND *clean_simple_command PARAMS((COMMAND *));

extern COMMAND *make_arith_command PARAMS((WORD_LIST *));

extern COMMAND *make_select_command PARAMS((WORD_DESC *, WORD_LIST *, COMMAND *, int));

#if defined (COND_COMMAND)
extern COND_COM *make_cond_node PARAMS((int, WORD_DESC *, COND_COM *, COND_COM *));
extern COMMAND *make_cond_command PARAMS((COND_COM *));
#endif

extern COMMAND *make_arith_for_command PARAMS((WORD_LIST *, COMMAND *, int));

extern COMMAND *make_subshell_command PARAMS((COMMAND *));
extern COMMAND *make_coproc_command PARAMS((char *, COMMAND *));

extern COMMAND *connect_async_list PARAMS((COMMAND *, COMMAND *, int));

#endif /* !_MAKE_CMD_H */
