/* parser.h -- Everything you wanted to know about the parser, but were
   afraid to ask. */

/* Copyright (C) 1995-2021 Free Software Foundation, Inc.

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

#if !defined (_PARSER_H_)
#  define _PARSER_H_

#  include "command.h"
#  include "input.h"

/* Possible states for the parser that require it to do special things. */
#define PST_CASEPAT	0x000001	/* in a case pattern list */
#define PST_ALEXPNEXT	0x000002	/* expand next word for aliases */
#define PST_ALLOWOPNBRC	0x000004	/* allow open brace for function def */
#define PST_NEEDCLOSBRC	0x000008	/* need close brace */
#define PST_DBLPAREN	0x000010	/* double-paren parsing - unused */
#define PST_SUBSHELL	0x000020	/* ( ... ) subshell */
#define PST_CMDSUBST	0x000040	/* $( ... ) command substitution */
#define PST_CASESTMT	0x000080	/* parsing a case statement */
#define PST_CONDCMD	0x000100	/* parsing a [[...]] command */
#define PST_CONDEXPR	0x000200	/* parsing the guts of [[...]] */
#define PST_ARITHFOR	0x000400	/* parsing an arithmetic for command - unused */
#define PST_ALEXPAND	0x000800	/* OK to expand aliases - unused */
#define PST_EXTPAT	0x001000	/* parsing an extended shell pattern */
#define PST_COMPASSIGN	0x002000	/* parsing x=(...) compound assignment */
#define PST_ASSIGNOK	0x004000	/* assignment statement ok in this context */
#define PST_EOFTOKEN	0x008000	/* yylex checks against shell_eof_token */
#define PST_REGEXP	0x010000	/* parsing an ERE/BRE as a single word */
#define PST_HEREDOC	0x020000	/* reading body of here-document */
#define PST_REPARSE	0x040000	/* re-parsing in parse_string_to_word_list */
#define PST_REDIRLIST	0x080000	/* parsing a list of redirections preceding a simple command name */
#define PST_COMMENT	0x100000	/* parsing a shell comment; used by aliases */
#define PST_ENDALIAS	0x200000	/* just finished expanding and consuming an alias */
#define PST_NOEXPAND	0x400000	/* don't expand anything in read_token_word; for command substitution */
#define PST_NOERROR	0x800000	/* don't print error messages in yyerror */

/* Definition of the delimiter stack.  Needed by parse.y and bashhist.c. */
struct dstack {
/* DELIMITERS is a stack of the nested delimiters that we have
   encountered so far. */
  char *delimiters;

/* Offset into the stack of delimiters. */
  int delimiter_depth;

/* How many slots are allocated to DELIMITERS. */
  int delimiter_space;
};

/* States we can be in while scanning a ${...} expansion.  Shared between
   parse.y and subst.c */
#define DOLBRACE_PARAM	0x01
#define DOLBRACE_OP	0x02
#define DOLBRACE_WORD	0x04

#define DOLBRACE_QUOTE	0x40	/* single quote is special in double quotes */
#define DOLBRACE_QUOTE2	0x80	/* single quote is semi-special in double quotes */

/* variable declarations from parse.y */
extern struct dstack dstack;

extern char *primary_prompt;
extern char *secondary_prompt;

extern char *current_prompt_string;

extern char *ps1_prompt;
extern char *ps2_prompt;
extern char *ps0_prompt;

extern int expand_aliases;
extern int current_command_line_count;
extern int saved_command_line_count;
extern int shell_eof_token;
extern int current_token;
extern int parser_state;
extern int need_here_doc;

extern int ignoreeof;
extern int eof_encountered;
extern int eof_encountered_limit;

extern int line_number, line_number_base;

#endif /* _PARSER_H_ */
