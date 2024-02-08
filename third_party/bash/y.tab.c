/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 21 "/usr/local/src/chet/src/bash/src/parse.y"

#include "config.h"

#include "bashtypes.h"
#include "bashansi.h"

#include "filecntl.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#if defined (HAVE_LOCALE_H)
#  include <locale.h>
#endif

#include <stdio.h>
#include "chartypes.h"
#include <signal.h>

#include "memalloc.h"

#include "bashintl.h"

#define NEED_STRFTIME_DECL	/* used in externs.h */

#include "shell.h"
#include "execute_cmd.h"
#include "typemax.h"		/* SIZE_MAX if needed */
#include "trap.h"
#include "flags.h"
#include "parser.h"
#include "mailcheck.h"
#include "test.h"
#include "builtins.h"
#include "common.h"
#include "builtext.h"

#include "shmbutil.h"

#if defined (READLINE)
#  include "bashline.h"
#  include "third_party/readline/readline.h"
#endif /* READLINE */

#if defined (HISTORY)
#  include "bashhist.h"
#  include "third_party/readline/history.h"
#endif /* HISTORY */

#if defined (JOB_CONTROL)
#  include "jobs.h"
#else
extern int cleanup_dead_jobs PARAMS((void));
#endif /* JOB_CONTROL */

#if defined (ALIAS)
#  include "alias.h"
#else
typedef void *alias_t;
#endif /* ALIAS */

#if defined (PROMPT_STRING_DECODE)
#  ifndef _MINIX
#    include <sys/param.h>
#  endif
#  include <time.h>
#  if defined (TM_IN_SYS_TIME)
#    include <sys/types.h>
#    include <sys/time.h>
#  endif /* TM_IN_SYS_TIME */
#  include "maxpath.h"
#endif /* PROMPT_STRING_DECODE */

#define RE_READ_TOKEN	-99
#define NO_EXPANSION	-100

#define END_ALIAS	-2

#ifdef DEBUG
#  define YYDEBUG 1
#else
#  define YYDEBUG 0
#endif

#if defined (HANDLE_MULTIBYTE)
#  define last_shell_getc_is_singlebyte \
	((shell_input_line_index > 1) \
		? shell_input_line_property[shell_input_line_index - 1] \
		: 1)
#  define MBTEST(x)	((x) && last_shell_getc_is_singlebyte)
#else
#  define last_shell_getc_is_singlebyte	1
#  define MBTEST(x)	((x))
#endif

#define EXTEND_SHELL_INPUT_LINE_PROPERTY() \
do { \
    if (shell_input_line_len + 2 > shell_input_line_propsize) \
      { \
	shell_input_line_propsize = shell_input_line_len + 2; \
	shell_input_line_property = (char *)xrealloc (shell_input_line_property, \
				    shell_input_line_propsize); \
      } \
} while (0)

#if defined (EXTENDED_GLOB)
extern int extended_glob;
#endif

#if defined (TRANSLATABLE_STRINGS)
extern int dump_translatable_strings, dump_po_strings;
extern int singlequote_translations;
#endif /* TRANSLATABLE_STRINGS */

#if !defined (errno)
extern int errno;
#endif

/* **************************************************************** */
/*								    */
/*		    "Forward" declarations			    */
/*								    */
/* **************************************************************** */

#ifdef DEBUG
static void debug_parser PARAMS((int));
#endif

static int yy_getc PARAMS((void));
static int yy_ungetc PARAMS((int));

#if defined (READLINE)
static int yy_readline_get PARAMS((void));
static int yy_readline_unget PARAMS((int));
#endif

static int yy_string_get PARAMS((void));
static int yy_string_unget PARAMS((int));
static int yy_stream_get PARAMS((void));
static int yy_stream_unget PARAMS((int));

static int shell_getc PARAMS((int));
static void shell_ungetc PARAMS((int));
static void discard_until PARAMS((int));

static void push_string PARAMS((char *, int, alias_t *));
static void pop_string PARAMS((void));
static void free_string_list PARAMS((void));

static char *read_a_line PARAMS((int));

static int reserved_word_acceptable PARAMS((int));
static int yylex PARAMS((void));

static void push_heredoc PARAMS((REDIRECT *));
static char *mk_alexpansion PARAMS((char *));
static int alias_expand_token PARAMS((char *));
static int time_command_acceptable PARAMS((void));
static int special_case_tokens PARAMS((char *));
static int read_token PARAMS((int));
static char *parse_matched_pair PARAMS((int, int, int, int *, int));
static char *parse_comsub PARAMS((int, int, int, int *, int));
#if defined (ARRAY_VARS)
static char *parse_compound_assignment PARAMS((int *));
#endif
#if defined (DPAREN_ARITHMETIC) || defined (ARITH_FOR_COMMAND)
static int parse_dparen PARAMS((int));
static int parse_arith_cmd PARAMS((char **, int));
#endif
#if defined (COND_COMMAND)
static void cond_error PARAMS((void));
static COND_COM *cond_expr PARAMS((void));
static COND_COM *cond_or PARAMS((void));
static COND_COM *cond_and PARAMS((void));
static COND_COM *cond_term PARAMS((void));
static int cond_skip_newlines PARAMS((void));
static COMMAND *parse_cond_command PARAMS((void));
#endif
#if defined (ARRAY_VARS)
static int token_is_assignment PARAMS((char *, int));
static int token_is_ident PARAMS((char *, int));
#endif
static int read_token_word PARAMS((int));
static void discard_parser_constructs PARAMS((int));

static char *error_token_from_token PARAMS((int));
static char *error_token_from_text PARAMS((void));
static void print_offending_line PARAMS((void));
static void report_syntax_error PARAMS((char *));

static void handle_eof_input_unit PARAMS((void));
static void prompt_again PARAMS((int));
#if 0
static void reset_readline_prompt PARAMS((void));
#endif
static void print_prompt PARAMS((void));

#if defined (HANDLE_MULTIBYTE)
static void set_line_mbstate PARAMS((void));
static char *shell_input_line_property = NULL;
static size_t shell_input_line_propsize = 0;
#else
#  define set_line_mbstate()
#endif

extern int yyerror PARAMS((const char *));

#ifdef DEBUG
extern int yydebug;
#endif

/* Default prompt strings */
char *primary_prompt = PPROMPT;
char *secondary_prompt = SPROMPT;

/* PROMPT_STRING_POINTER points to one of these, never to an actual string. */
char *ps1_prompt, *ps2_prompt;

/* Displayed after reading a command but before executing it in an interactive shell */
char *ps0_prompt;

/* Handle on the current prompt string.  Indirectly points through
   ps1_ or ps2_prompt. */
char **prompt_string_pointer = (char **)NULL;
char *current_prompt_string;

/* Non-zero means we expand aliases in commands. */
int expand_aliases = 0;

/* If non-zero, the decoded prompt string undergoes parameter and
   variable substitution, command substitution, arithmetic substitution,
   string expansion, process substitution, and quote removal in
   decode_prompt_string. */
int promptvars = 1;

/* If non-zero, $'...' and $"..." are expanded when they appear within
   a ${...} expansion, even when the expansion appears within double
   quotes. */
int extended_quote = 1;

/* The number of lines read from input while creating the current command. */
int current_command_line_count;

/* The number of lines in a command saved while we run parse_and_execute */
int saved_command_line_count;

/* The token that currently denotes the end of parse. */
int shell_eof_token;

/* The token currently being read. */
int current_token;

/* The current parser state. */
int parser_state;

/* Variables to manage the task of reading here documents, because we need to
   defer the reading until after a complete command has been collected. */
static REDIRECT *redir_stack[HEREDOC_MAX];
int need_here_doc;

/* Where shell input comes from.  History expansion is performed on each
   line when the shell is interactive. */
static char *shell_input_line = (char *)NULL;
static size_t shell_input_line_index;
static size_t shell_input_line_size;	/* Amount allocated for shell_input_line. */
static size_t shell_input_line_len;	/* strlen (shell_input_line) */

/* Either zero or EOF. */
static int shell_input_line_terminator;

/* The line number in a script on which a function definition starts. */
static int function_dstart;

/* The line number in a script on which a function body starts. */
static int function_bstart;

/* The line number in a script at which an arithmetic for command starts. */
static int arith_for_lineno;

/* The decoded prompt string.  Used if READLINE is not defined or if
   editing is turned off.  Analogous to current_readline_prompt. */
static char *current_decoded_prompt;

/* The last read token, or NULL.  read_token () uses this for context
   checking. */
static int last_read_token;

/* The token read prior to last_read_token. */
static int token_before_that;

/* The token read prior to token_before_that. */
static int two_tokens_ago;

static int global_extglob;

/* The line number in a script where the word in a `case WORD', `select WORD'
   or `for WORD' begins.  This is a nested command maximum, since the array
   index is decremented after a case, select, or for command is parsed. */
#define MAX_CASE_NEST	128
static int word_lineno[MAX_CASE_NEST+1];
static int word_top = -1;

/* If non-zero, it is the token that we want read_token to return
   regardless of what text is (or isn't) present to be read.  This
   is reset by read_token.  If token_to_read == WORD or
   ASSIGNMENT_WORD, yylval.word should be set to word_desc_to_read. */
static int token_to_read;
static WORD_DESC *word_desc_to_read;

static REDIRECTEE source;
static REDIRECTEE redir;

static FILE *yyoutstream;
static FILE *yyerrstream;

#line 388 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IF = 258,                      /* IF  */
    THEN = 259,                    /* THEN  */
    ELSE = 260,                    /* ELSE  */
    ELIF = 261,                    /* ELIF  */
    FI = 262,                      /* FI  */
    CASE = 263,                    /* CASE  */
    ESAC = 264,                    /* ESAC  */
    FOR = 265,                     /* FOR  */
    SELECT = 266,                  /* SELECT  */
    WHILE = 267,                   /* WHILE  */
    UNTIL = 268,                   /* UNTIL  */
    DO = 269,                      /* DO  */
    DONE = 270,                    /* DONE  */
    FUNCTION = 271,                /* FUNCTION  */
    COPROC = 272,                  /* COPROC  */
    COND_START = 273,              /* COND_START  */
    COND_END = 274,                /* COND_END  */
    COND_ERROR = 275,              /* COND_ERROR  */
    IN = 276,                      /* IN  */
    BANG = 277,                    /* BANG  */
    TIME = 278,                    /* TIME  */
    TIMEOPT = 279,                 /* TIMEOPT  */
    TIMEIGN = 280,                 /* TIMEIGN  */
    WORD = 281,                    /* WORD  */
    ASSIGNMENT_WORD = 282,         /* ASSIGNMENT_WORD  */
    REDIR_WORD = 283,              /* REDIR_WORD  */
    NUMBER = 284,                  /* NUMBER  */
    ARITH_CMD = 285,               /* ARITH_CMD  */
    ARITH_FOR_EXPRS = 286,         /* ARITH_FOR_EXPRS  */
    COND_CMD = 287,                /* COND_CMD  */
    AND_AND = 288,                 /* AND_AND  */
    OR_OR = 289,                   /* OR_OR  */
    GREATER_GREATER = 290,         /* GREATER_GREATER  */
    LESS_LESS = 291,               /* LESS_LESS  */
    LESS_AND = 292,                /* LESS_AND  */
    LESS_LESS_LESS = 293,          /* LESS_LESS_LESS  */
    GREATER_AND = 294,             /* GREATER_AND  */
    SEMI_SEMI = 295,               /* SEMI_SEMI  */
    SEMI_AND = 296,                /* SEMI_AND  */
    SEMI_SEMI_AND = 297,           /* SEMI_SEMI_AND  */
    LESS_LESS_MINUS = 298,         /* LESS_LESS_MINUS  */
    AND_GREATER = 299,             /* AND_GREATER  */
    AND_GREATER_GREATER = 300,     /* AND_GREATER_GREATER  */
    LESS_GREATER = 301,            /* LESS_GREATER  */
    GREATER_BAR = 302,             /* GREATER_BAR  */
    BAR_AND = 303,                 /* BAR_AND  */
    DOLPAREN = 304,                /* DOLPAREN  */
    yacc_EOF = 305                 /* yacc_EOF  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define IF 258
#define THEN 259
#define ELSE 260
#define ELIF 261
#define FI 262
#define CASE 263
#define ESAC 264
#define FOR 265
#define SELECT 266
#define WHILE 267
#define UNTIL 268
#define DO 269
#define DONE 270
#define FUNCTION 271
#define COPROC 272
#define COND_START 273
#define COND_END 274
#define COND_ERROR 275
#define IN 276
#define BANG 277
#define TIME 278
#define TIMEOPT 279
#define TIMEIGN 280
#define WORD 281
#define ASSIGNMENT_WORD 282
#define REDIR_WORD 283
#define NUMBER 284
#define ARITH_CMD 285
#define ARITH_FOR_EXPRS 286
#define COND_CMD 287
#define AND_AND 288
#define OR_OR 289
#define GREATER_GREATER 290
#define LESS_LESS 291
#define LESS_AND 292
#define LESS_LESS_LESS 293
#define GREATER_AND 294
#define SEMI_SEMI 295
#define SEMI_AND 296
#define SEMI_SEMI_AND 297
#define LESS_LESS_MINUS 298
#define AND_GREATER 299
#define AND_GREATER_GREATER 300
#define LESS_GREATER 301
#define GREATER_BAR 302
#define BAR_AND 303
#define DOLPAREN 304
#define yacc_EOF 305

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 338 "/usr/local/src/chet/src/bash/src/parse.y"

  WORD_DESC *word;		/* the word that we read. */
  int number;			/* the number that we read. */
  WORD_LIST *word_list;
  COMMAND *command;
  REDIRECT *redirect;
  ELEMENT element;
  PATTERN_LIST *pattern;

#line 551 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IF = 3,                         /* IF  */
  YYSYMBOL_THEN = 4,                       /* THEN  */
  YYSYMBOL_ELSE = 5,                       /* ELSE  */
  YYSYMBOL_ELIF = 6,                       /* ELIF  */
  YYSYMBOL_FI = 7,                         /* FI  */
  YYSYMBOL_CASE = 8,                       /* CASE  */
  YYSYMBOL_ESAC = 9,                       /* ESAC  */
  YYSYMBOL_FOR = 10,                       /* FOR  */
  YYSYMBOL_SELECT = 11,                    /* SELECT  */
  YYSYMBOL_WHILE = 12,                     /* WHILE  */
  YYSYMBOL_UNTIL = 13,                     /* UNTIL  */
  YYSYMBOL_DO = 14,                        /* DO  */
  YYSYMBOL_DONE = 15,                      /* DONE  */
  YYSYMBOL_FUNCTION = 16,                  /* FUNCTION  */
  YYSYMBOL_COPROC = 17,                    /* COPROC  */
  YYSYMBOL_COND_START = 18,                /* COND_START  */
  YYSYMBOL_COND_END = 19,                  /* COND_END  */
  YYSYMBOL_COND_ERROR = 20,                /* COND_ERROR  */
  YYSYMBOL_IN = 21,                        /* IN  */
  YYSYMBOL_BANG = 22,                      /* BANG  */
  YYSYMBOL_TIME = 23,                      /* TIME  */
  YYSYMBOL_TIMEOPT = 24,                   /* TIMEOPT  */
  YYSYMBOL_TIMEIGN = 25,                   /* TIMEIGN  */
  YYSYMBOL_WORD = 26,                      /* WORD  */
  YYSYMBOL_ASSIGNMENT_WORD = 27,           /* ASSIGNMENT_WORD  */
  YYSYMBOL_REDIR_WORD = 28,                /* REDIR_WORD  */
  YYSYMBOL_NUMBER = 29,                    /* NUMBER  */
  YYSYMBOL_ARITH_CMD = 30,                 /* ARITH_CMD  */
  YYSYMBOL_ARITH_FOR_EXPRS = 31,           /* ARITH_FOR_EXPRS  */
  YYSYMBOL_COND_CMD = 32,                  /* COND_CMD  */
  YYSYMBOL_AND_AND = 33,                   /* AND_AND  */
  YYSYMBOL_OR_OR = 34,                     /* OR_OR  */
  YYSYMBOL_GREATER_GREATER = 35,           /* GREATER_GREATER  */
  YYSYMBOL_LESS_LESS = 36,                 /* LESS_LESS  */
  YYSYMBOL_LESS_AND = 37,                  /* LESS_AND  */
  YYSYMBOL_LESS_LESS_LESS = 38,            /* LESS_LESS_LESS  */
  YYSYMBOL_GREATER_AND = 39,               /* GREATER_AND  */
  YYSYMBOL_SEMI_SEMI = 40,                 /* SEMI_SEMI  */
  YYSYMBOL_SEMI_AND = 41,                  /* SEMI_AND  */
  YYSYMBOL_SEMI_SEMI_AND = 42,             /* SEMI_SEMI_AND  */
  YYSYMBOL_LESS_LESS_MINUS = 43,           /* LESS_LESS_MINUS  */
  YYSYMBOL_AND_GREATER = 44,               /* AND_GREATER  */
  YYSYMBOL_AND_GREATER_GREATER = 45,       /* AND_GREATER_GREATER  */
  YYSYMBOL_LESS_GREATER = 46,              /* LESS_GREATER  */
  YYSYMBOL_GREATER_BAR = 47,               /* GREATER_BAR  */
  YYSYMBOL_BAR_AND = 48,                   /* BAR_AND  */
  YYSYMBOL_DOLPAREN = 49,                  /* DOLPAREN  */
  YYSYMBOL_50_ = 50,                       /* '&'  */
  YYSYMBOL_51_ = 51,                       /* ';'  */
  YYSYMBOL_52_n_ = 52,                     /* '\n'  */
  YYSYMBOL_yacc_EOF = 53,                  /* yacc_EOF  */
  YYSYMBOL_54_ = 54,                       /* '|'  */
  YYSYMBOL_55_ = 55,                       /* '>'  */
  YYSYMBOL_56_ = 56,                       /* '<'  */
  YYSYMBOL_57_ = 57,                       /* '-'  */
  YYSYMBOL_58_ = 58,                       /* '{'  */
  YYSYMBOL_59_ = 59,                       /* '}'  */
  YYSYMBOL_60_ = 60,                       /* '('  */
  YYSYMBOL_61_ = 61,                       /* ')'  */
  YYSYMBOL_YYACCEPT = 62,                  /* $accept  */
  YYSYMBOL_inputunit = 63,                 /* inputunit  */
  YYSYMBOL_word_list = 64,                 /* word_list  */
  YYSYMBOL_redirection = 65,               /* redirection  */
  YYSYMBOL_simple_command_element = 66,    /* simple_command_element  */
  YYSYMBOL_redirection_list = 67,          /* redirection_list  */
  YYSYMBOL_simple_command = 68,            /* simple_command  */
  YYSYMBOL_command = 69,                   /* command  */
  YYSYMBOL_shell_command = 70,             /* shell_command  */
  YYSYMBOL_for_command = 71,               /* for_command  */
  YYSYMBOL_arith_for_command = 72,         /* arith_for_command  */
  YYSYMBOL_select_command = 73,            /* select_command  */
  YYSYMBOL_case_command = 74,              /* case_command  */
  YYSYMBOL_function_def = 75,              /* function_def  */
  YYSYMBOL_function_body = 76,             /* function_body  */
  YYSYMBOL_subshell = 77,                  /* subshell  */
  YYSYMBOL_comsub = 78,                    /* comsub  */
  YYSYMBOL_coproc = 79,                    /* coproc  */
  YYSYMBOL_if_command = 80,                /* if_command  */
  YYSYMBOL_group_command = 81,             /* group_command  */
  YYSYMBOL_arith_command = 82,             /* arith_command  */
  YYSYMBOL_cond_command = 83,              /* cond_command  */
  YYSYMBOL_elif_clause = 84,               /* elif_clause  */
  YYSYMBOL_case_clause = 85,               /* case_clause  */
  YYSYMBOL_pattern_list = 86,              /* pattern_list  */
  YYSYMBOL_case_clause_sequence = 87,      /* case_clause_sequence  */
  YYSYMBOL_pattern = 88,                   /* pattern  */
  YYSYMBOL_compound_list = 89,             /* compound_list  */
  YYSYMBOL_list0 = 90,                     /* list0  */
  YYSYMBOL_list1 = 91,                     /* list1  */
  YYSYMBOL_simple_list_terminator = 92,    /* simple_list_terminator  */
  YYSYMBOL_list_terminator = 93,           /* list_terminator  */
  YYSYMBOL_newline_list = 94,              /* newline_list  */
  YYSYMBOL_simple_list = 95,               /* simple_list  */
  YYSYMBOL_simple_list1 = 96,              /* simple_list1  */
  YYSYMBOL_pipeline_command = 97,          /* pipeline_command  */
  YYSYMBOL_pipeline = 98,                  /* pipeline  */
  YYSYMBOL_timespec = 99                   /* timespec  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  121
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   740

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  174
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  349

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   305


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      52,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    50,     2,
      60,    61,     2,     2,     2,    57,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    51,
      56,     2,    55,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    58,    54,    59,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    53
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   395,   395,   406,   414,   423,   438,   455,   465,   467,
     471,   477,   483,   489,   495,   501,   507,   513,   519,   525,
     531,   537,   543,   549,   555,   561,   568,   575,   582,   589,
     596,   603,   609,   615,   621,   627,   633,   639,   645,   651,
     657,   663,   669,   675,   681,   687,   693,   699,   705,   711,
     717,   723,   729,   735,   743,   745,   747,   751,   755,   766,
     768,   772,   774,   776,   792,   794,   798,   800,   802,   804,
     806,   808,   810,   812,   814,   816,   818,   822,   827,   832,
     837,   842,   847,   852,   857,   864,   870,   876,   882,   890,
     895,   900,   905,   910,   915,   920,   925,   932,   937,   942,
     949,   951,   953,   955,   959,   961,   992,   999,  1003,  1009,
    1014,  1031,  1036,  1053,  1060,  1062,  1064,  1069,  1073,  1077,
    1081,  1083,  1085,  1089,  1090,  1094,  1096,  1098,  1100,  1104,
    1106,  1108,  1110,  1112,  1114,  1118,  1120,  1129,  1135,  1141,
    1142,  1149,  1153,  1155,  1157,  1164,  1166,  1173,  1177,  1178,
    1181,  1183,  1185,  1189,  1190,  1199,  1214,  1232,  1249,  1251,
    1253,  1260,  1263,  1267,  1269,  1275,  1281,  1301,  1324,  1326,
    1349,  1353,  1355,  1357,  1359
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IF", "THEN", "ELSE",
  "ELIF", "FI", "CASE", "ESAC", "FOR", "SELECT", "WHILE", "UNTIL", "DO",
  "DONE", "FUNCTION", "COPROC", "COND_START", "COND_END", "COND_ERROR",
  "IN", "BANG", "TIME", "TIMEOPT", "TIMEIGN", "WORD", "ASSIGNMENT_WORD",
  "REDIR_WORD", "NUMBER", "ARITH_CMD", "ARITH_FOR_EXPRS", "COND_CMD",
  "AND_AND", "OR_OR", "GREATER_GREATER", "LESS_LESS", "LESS_AND",
  "LESS_LESS_LESS", "GREATER_AND", "SEMI_SEMI", "SEMI_AND",
  "SEMI_SEMI_AND", "LESS_LESS_MINUS", "AND_GREATER", "AND_GREATER_GREATER",
  "LESS_GREATER", "GREATER_BAR", "BAR_AND", "DOLPAREN", "'&'", "';'",
  "'\\n'", "yacc_EOF", "'|'", "'>'", "'<'", "'-'", "'{'", "'}'", "'('",
  "')'", "$accept", "inputunit", "word_list", "redirection",
  "simple_command_element", "redirection_list", "simple_command",
  "command", "shell_command", "for_command", "arith_for_command",
  "select_command", "case_command", "function_def", "function_body",
  "subshell", "comsub", "coproc", "if_command", "group_command",
  "arith_command", "cond_command", "elif_clause", "case_clause",
  "pattern_list", "case_clause_sequence", "pattern", "compound_list",
  "list0", "list1", "simple_list_terminator", "list_terminator",
  "newline_list", "simple_list", "simple_list1", "pipeline_command",
  "pipeline", "timespec", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-152)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     328,    80,  -152,   -11,    -1,     3,  -152,  -152,    15,   637,
      -5,   433,   149,   -28,  -152,   187,   684,  -152,    18,    28,
     130,    38,   139,    50,    52,    60,    65,    74,  -152,  -152,
    -152,    89,   104,  -152,  -152,    97,  -152,  -152,   246,  -152,
     670,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,   146,   211,  -152,     1,   433,  -152,  -152,
     135,   484,  -152,    59,    61,    90,   167,   171,    10,    71,
     246,   670,   144,  -152,  -152,  -152,  -152,  -152,   165,  -152,
     142,   179,   192,   140,   194,   160,   227,   245,   252,   253,
     260,   261,   262,   162,   269,   178,   270,   272,   273,   274,
     277,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,   168,   379,  -152,  -152,   173,
     244,  -152,  -152,  -152,  -152,   670,  -152,  -152,  -152,  -152,
    -152,   535,   535,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
     205,  -152,    14,  -152,    36,  -152,  -152,  -152,  -152,    84,
    -152,  -152,  -152,   249,   670,  -152,   670,   670,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,   484,   484,   203,   203,   586,   586,
     145,  -152,  -152,  -152,  -152,  -152,  -152,     0,  -152,   119,
    -152,   291,   248,    66,    88,  -152,   119,  -152,   296,   297,
      35,  -152,   670,   670,    35,  -152,  -152,     1,     1,  -152,
    -152,  -152,   306,   484,   484,   484,   484,   484,   305,   169,
    -152,     7,  -152,  -152,   302,  -152,   131,  -152,   265,  -152,
    -152,  -152,  -152,  -152,  -152,   304,   131,  -152,   266,  -152,
    -152,  -152,    35,  -152,   313,   317,  -152,  -152,  -152,   225,
     225,   225,  -152,  -152,  -152,  -152,   206,    25,  -152,  -152,
     307,   -42,   319,   276,  -152,  -152,  -152,    95,  -152,   322,
     283,   332,   284,  -152,  -152,   102,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,    45,   323,  -152,  -152,  -152,   106,
    -152,  -152,  -152,  -152,  -152,  -152,   109,  -152,  -152,   264,
    -152,  -152,  -152,   484,  -152,  -152,   333,   293,  -152,  -152,
     338,   300,  -152,  -152,  -152,   484,   345,   303,  -152,  -152,
     346,   309,  -152,  -152,  -152,  -152,  -152,  -152,  -152
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,   153,     0,     0,     0,   153,   153,     0,     0,
       0,     0,   171,    54,    55,     0,     0,   118,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   153,     4,
       7,     0,     0,   153,   153,     0,    56,    59,    61,   170,
      62,    66,    76,    70,    67,    64,    72,     3,    65,    71,
      73,    74,    75,     0,   155,   162,   163,     0,     5,     6,
       0,     0,   153,   153,     0,   153,     0,     0,     0,    54,
     113,   109,     0,   151,   150,   152,   167,   164,   172,   173,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    16,    25,    40,    34,    49,    31,    43,    37,    46,
      28,    52,    53,    22,    19,     0,     0,    10,    11,     0,
       0,     1,    54,    60,    57,    63,   148,   149,     2,   153,
     153,   156,   157,   153,   153,   166,   165,   153,   154,   137,
     138,   147,     0,   153,     0,   153,   153,   153,   153,     0,
     153,   153,   153,   153,   104,   102,   111,   110,   119,   174,
     153,    18,    27,    42,    36,    51,    33,    45,    39,    48,
      30,    24,    21,    14,    15,    17,    26,    41,    35,    50,
      32,    44,    38,    47,    29,    23,    20,    12,    13,   107,
     108,   117,   106,    58,     0,     0,   160,   161,     0,     0,
       0,   153,   153,   153,   153,   153,   153,     0,   153,     0,
     153,     0,     0,     0,     0,   153,     0,   153,     0,     0,
       0,   153,   105,   112,     0,   158,   159,   169,   168,   153,
     153,   114,     0,     0,     0,   140,   141,   139,     0,   123,
     153,     0,   153,   153,     0,     8,     0,   153,     0,    87,
      88,   153,   153,   153,   153,     0,     0,   153,     0,    68,
      69,   103,     0,   100,     0,     0,   116,   142,   143,   144,
     145,   146,    99,   129,   131,   133,   124,     0,    97,   135,
       0,     0,     0,     0,    77,     9,   153,     0,    78,     0,
       0,     0,     0,    89,   153,     0,    90,   101,   115,   153,
     130,   132,   134,    98,     0,     0,   153,    79,    80,     0,
     153,   153,    85,    86,    91,    92,     0,   153,   153,   120,
     153,   136,   125,   126,   153,   153,     0,     0,   153,   153,
       0,     0,   153,   122,   127,   128,     0,     0,    83,    84,
       0,     0,    95,    96,   121,    81,    82,    93,    94
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -152,  -152,   112,   -29,   -14,   -64,   360,  -152,    -8,  -152,
    -152,  -152,  -152,  -152,  -151,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,    13,  -152,   136,  -152,    98,    -2,  -152,    30,
    -152,   -54,   -26,  -152,  -123,     6,    78,  -152
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    35,   246,    36,    37,   125,    38,    39,    40,    41,
      42,    43,    44,    45,   155,    46,    47,    48,    49,    50,
      51,    52,   232,   238,   239,   240,   281,   120,   139,   140,
     128,    76,    61,    53,    54,   141,    56,    57
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      60,    71,   116,   135,    66,    67,    55,   157,   196,   197,
     147,   124,   305,     2,   242,    62,   278,    77,     3,   306,
       4,     5,     6,     7,   123,    63,   115,    72,    10,    65,
      64,   119,    80,   279,   303,   206,   142,   144,     2,   149,
      17,    68,   124,     3,   101,     4,     5,     6,     7,   133,
     208,   279,   138,    10,   102,   134,   123,   209,   243,   138,
     154,   156,   152,   136,   106,    17,   138,   280,    33,   261,
     153,   225,   226,   263,     2,   145,   110,   138,   111,     3,
     251,     4,     5,     6,     7,   280,   112,   138,   138,    10,
     222,   113,   223,    33,   210,    34,   193,   121,   215,   305,
     114,    17,   253,   194,   195,   216,   320,   198,   199,   310,
     143,   297,    73,    74,    75,   117,   317,   207,   138,   146,
     324,   213,   214,   328,   252,   124,   220,   124,   193,    33,
     118,    34,    58,    59,   224,   200,   138,    55,    55,   137,
     138,   148,   217,   211,   212,   245,   254,   138,   218,   219,
     229,   230,   231,   311,   138,   247,   103,   285,   138,   104,
     318,   138,   257,   158,   325,   107,   163,   329,   108,   164,
      73,    74,    75,    78,    79,   233,   234,   235,   236,   237,
     241,   150,    73,    74,    75,   151,   167,   105,   177,   168,
     159,   178,   286,   193,   193,   262,   109,   165,   126,   127,
      55,    55,   294,   160,   181,   161,   244,   182,   248,   273,
     274,   275,   154,   255,   277,   258,   154,   169,   162,   179,
     166,   287,    81,    82,    83,    84,    85,   264,   265,   189,
      86,   295,   191,    87,    88,   183,   129,   130,   201,   202,
     282,   283,    89,    90,   129,   130,   300,   301,   302,   289,
     290,   291,   292,   170,   154,   203,   204,   205,   201,   202,
     309,   131,   132,   267,   268,   269,   270,   271,   316,   332,
     230,   171,   122,    14,    15,    16,   227,   228,   172,   173,
     323,    18,    19,    20,    21,    22,   174,   175,   176,    23,
      24,    25,    26,    27,   335,   180,   184,   319,   185,   186,
     187,    31,    32,   188,   322,   192,   249,   250,   326,   327,
     221,   259,   260,   266,   272,   330,   331,   284,   334,   293,
     298,   299,   336,   337,   288,   296,   340,   341,   256,     1,
     344,     2,   333,   279,   307,   308,     3,   312,     4,     5,
       6,     7,   313,   315,     8,     9,    10,   314,   338,   321,
      11,    12,   339,   342,    13,    14,    15,    16,    17,   343,
     345,   347,   346,    18,    19,    20,    21,    22,   348,    70,
       0,    23,    24,    25,    26,    27,   276,    28,   304,     0,
      29,    30,     2,    31,    32,     0,    33,     3,    34,     4,
       5,     6,     7,     0,     0,     8,     9,    10,     0,     0,
       0,    11,    12,     0,     0,    13,    14,    15,    16,    17,
       0,     0,     0,     0,    18,    19,    20,    21,    22,     0,
       0,     0,    23,    24,    25,    26,    27,     0,     0,     0,
       0,   138,     0,     0,    31,    32,     2,    33,     0,    34,
     190,     3,     0,     4,     5,     6,     7,     0,     0,     8,
       9,    10,     0,     0,     0,    11,    12,     0,     0,    13,
      14,    15,    16,    17,     0,     0,     0,     0,    18,    19,
      20,    21,    22,     0,     0,     0,    23,    24,    25,    26,
      27,     0,     0,     0,    73,    74,    75,     2,    31,    32,
       0,    33,     3,    34,     4,     5,     6,     7,     0,     0,
       8,     9,    10,     0,     0,     0,    11,    12,     0,     0,
      13,    14,    15,    16,    17,     0,     0,     0,     0,    18,
      19,    20,    21,    22,     0,     0,     0,    23,    24,    25,
      26,    27,     0,     0,     0,     0,   138,     0,     2,    31,
      32,     0,    33,     3,    34,     4,     5,     6,     7,     0,
       0,     8,     9,    10,     0,     0,     0,    11,    12,     0,
       0,    13,    14,    15,    16,    17,     0,     0,     0,     0,
      18,    19,    20,    21,    22,     0,     0,     0,    23,    24,
      25,    26,    27,     0,     0,     0,     0,     0,     0,     2,
      31,    32,     0,    33,     3,    34,     4,     5,     6,     7,
       0,     0,     8,     9,    10,     0,     0,     0,     0,     0,
       0,     0,    13,    14,    15,    16,    17,     0,     0,     0,
       0,    18,    19,    20,    21,    22,     0,     0,     0,    23,
      24,    25,    26,    27,     0,     0,     0,     0,   138,     0,
       2,    31,    32,     0,    33,     3,    34,     4,     5,     6,
       7,     0,     0,     0,     0,    10,     0,     0,     0,     0,
       0,     0,     0,    69,    14,    15,    16,    17,     0,     0,
       0,     0,    18,    19,    20,    21,    22,     0,     0,     0,
      23,    24,    25,    26,    27,     0,     0,     0,     0,     0,
       0,     0,    31,    32,     0,    33,     0,    34,    15,    16,
       0,     0,     0,     0,     0,    18,    19,    20,    21,    22,
       0,     0,     0,    23,    24,    25,    26,    27,     0,    91,
      92,    93,    94,    95,     0,    31,    32,    96,     0,     0,
      97,    98,     0,     0,     0,     0,     0,     0,     0,    99,
     100
};

static const yytype_int16 yycheck[] =
{
       2,     9,    28,    57,     6,     7,     0,    71,   131,   132,
      64,    40,    54,     3,    14,    26,     9,    11,     8,    61,
      10,    11,    12,    13,    38,    26,    28,    32,    18,    26,
      31,    33,    60,    26,     9,    21,    62,    63,     3,    65,
      30,    26,    71,     8,    26,    10,    11,    12,    13,    48,
      14,    26,    52,    18,    26,    54,    70,    21,    58,    52,
      68,    69,    52,    57,    26,    30,    52,    60,    58,   220,
      60,   194,   195,   224,     3,    14,    26,    52,    26,     8,
      14,    10,    11,    12,    13,    60,    26,    52,    52,    18,
     154,    26,   156,    58,    58,    60,   125,     0,    14,    54,
      26,    30,    14,   129,   130,    21,    61,   133,   134,    14,
      51,   262,    51,    52,    53,    26,    14,   143,    52,    58,
      14,   147,   148,    14,    58,   154,   152,   156,   157,    58,
      26,    60,    52,    53,   160,   137,    52,   131,   132,     4,
      52,    51,    58,   145,   146,    26,    58,    52,   150,   151,
       5,     6,     7,    58,    52,   209,    26,    26,    52,    29,
      58,    52,   216,    19,    58,    26,    26,    58,    29,    29,
      51,    52,    53,    24,    25,   201,   202,   203,   204,   205,
     206,    14,    51,    52,    53,    14,    26,    57,    26,    29,
      25,    29,   246,   222,   223,   221,    57,    57,    52,    53,
     194,   195,   256,    61,    26,    26,   208,    29,   210,    40,
      41,    42,   220,   215,   240,   217,   224,    57,    26,    57,
      26,   247,    35,    36,    37,    38,    39,   229,   230,    61,
      43,   257,    59,    46,    47,    57,    33,    34,    33,    34,
     242,   243,    55,    56,    33,    34,    40,    41,    42,   251,
     252,   253,   254,    26,   262,    50,    51,    52,    33,    34,
     286,    50,    51,   233,   234,   235,   236,   237,   294,     5,
       6,    26,    26,    27,    28,    29,   198,   199,    26,    26,
     306,    35,    36,    37,    38,    39,    26,    26,    26,    43,
      44,    45,    46,    47,   320,    26,    26,   299,    26,    26,
      26,    55,    56,    26,   306,    61,    15,    59,   310,   311,
      61,    15,    15,     7,     9,   317,   318,    15,   320,    15,
       7,     4,   324,   325,    59,    59,   328,   329,   216,     1,
     332,     3,   319,    26,    15,    59,     8,    15,    10,    11,
      12,    13,    59,    59,    16,    17,    18,    15,    15,    26,
      22,    23,    59,    15,    26,    27,    28,    29,    30,    59,
      15,    15,    59,    35,    36,    37,    38,    39,    59,     9,
      -1,    43,    44,    45,    46,    47,   240,    49,   280,    -1,
      52,    53,     3,    55,    56,    -1,    58,     8,    60,    10,
      11,    12,    13,    -1,    -1,    16,    17,    18,    -1,    -1,
      -1,    22,    23,    -1,    -1,    26,    27,    28,    29,    30,
      -1,    -1,    -1,    -1,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    55,    56,     3,    58,    -1,    60,
      61,     8,    -1,    10,    11,    12,    13,    -1,    -1,    16,
      17,    18,    -1,    -1,    -1,    22,    23,    -1,    -1,    26,
      27,    28,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    43,    44,    45,    46,
      47,    -1,    -1,    -1,    51,    52,    53,     3,    55,    56,
      -1,    58,     8,    60,    10,    11,    12,    13,    -1,    -1,
      16,    17,    18,    -1,    -1,    -1,    22,    23,    -1,    -1,
      26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    43,    44,    45,
      46,    47,    -1,    -1,    -1,    -1,    52,    -1,     3,    55,
      56,    -1,    58,     8,    60,    10,    11,    12,    13,    -1,
      -1,    16,    17,    18,    -1,    -1,    -1,    22,    23,    -1,
      -1,    26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      55,    56,    -1,    58,     8,    60,    10,    11,    12,    13,
      -1,    -1,    16,    17,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    26,    27,    28,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    38,    39,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    -1,    -1,    -1,    -1,    52,    -1,
       3,    55,    56,    -1,    58,     8,    60,    10,    11,    12,
      13,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    27,    28,    29,    30,    -1,    -1,
      -1,    -1,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    58,    -1,    60,    28,    29,
      -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    -1,    35,
      36,    37,    38,    39,    -1,    55,    56,    43,    -1,    -1,
      46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     8,    10,    11,    12,    13,    16,    17,
      18,    22,    23,    26,    27,    28,    29,    30,    35,    36,
      37,    38,    39,    43,    44,    45,    46,    47,    49,    52,
      53,    55,    56,    58,    60,    63,    65,    66,    68,    69,
      70,    71,    72,    73,    74,    75,    77,    78,    79,    80,
      81,    82,    83,    95,    96,    97,    98,    99,    52,    53,
      89,    94,    26,    26,    31,    26,    89,    89,    26,    26,
      68,    70,    32,    51,    52,    53,    93,    97,    24,    25,
      60,    35,    36,    37,    38,    39,    43,    46,    47,    55,
      56,    35,    36,    37,    38,    39,    43,    46,    47,    55,
      56,    26,    26,    26,    29,    57,    26,    26,    29,    57,
      26,    26,    26,    26,    26,    89,    94,    26,    26,    89,
      89,     0,    26,    66,    65,    67,    52,    53,    92,    33,
      34,    50,    51,    48,    54,    93,    97,     4,    52,    90,
      91,    97,    94,    51,    94,    14,    58,    93,    51,    94,
      14,    14,    52,    60,    70,    76,    70,    67,    19,    25,
      61,    26,    26,    26,    29,    57,    26,    26,    29,    57,
      26,    26,    26,    26,    26,    26,    26,    26,    29,    57,
      26,    26,    29,    57,    26,    26,    26,    26,    26,    61,
      61,    59,    61,    65,    94,    94,    96,    96,    94,    94,
      89,    33,    34,    50,    51,    52,    21,    94,    14,    21,
      58,    89,    89,    94,    94,    14,    21,    58,    89,    89,
      94,    61,    67,    67,    94,    96,    96,    98,    98,     5,
       6,     7,    84,    94,    94,    94,    94,    94,    85,    86,
      87,    94,    14,    58,    89,    26,    64,    93,    89,    15,
      59,    14,    58,    14,    58,    89,    64,    93,    89,    15,
      15,    76,    94,    76,    89,    89,     7,    91,    91,    91,
      91,    91,     9,    40,    41,    42,    86,    94,     9,    26,
      60,    88,    89,    89,    15,    26,    93,    94,    59,    89,
      89,    89,    89,    15,    93,    94,    59,    76,     7,     4,
      40,    41,    42,     9,    88,    54,    61,    15,    59,    94,
      14,    58,    15,    59,    15,    59,    94,    14,    58,    89,
      61,    26,    89,    94,    14,    58,    89,    89,    14,    58,
      89,    89,     5,    84,    89,    94,    89,    89,    15,    59,
      89,    89,    15,    59,    89,    15,    59,    15,    59
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    62,    63,    63,    63,    63,    63,    63,    64,    64,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    66,    66,    66,    67,    67,    68,
      68,    69,    69,    69,    69,    69,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    71,    71,    71,
      71,    71,    71,    71,    71,    72,    72,    72,    72,    73,
      73,    73,    73,    73,    73,    73,    73,    74,    74,    74,
      75,    75,    75,    75,    76,    76,    77,    78,    78,    79,
      79,    79,    79,    79,    80,    80,    80,    81,    82,    83,
      84,    84,    84,    85,    85,    86,    86,    86,    86,    87,
      87,    87,    87,    87,    87,    88,    88,    89,    89,    90,
      90,    90,    91,    91,    91,    91,    91,    91,    92,    92,
      93,    93,    93,    94,    94,    95,    95,    95,    96,    96,
      96,    96,    96,    97,    97,    97,    97,    97,    98,    98,
      98,    99,    99,    99,    99
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     2,     2,     1,     1,     2,
       2,     2,     3,     3,     3,     3,     2,     3,     3,     2,
       3,     3,     2,     3,     3,     2,     3,     3,     2,     3,
       3,     2,     3,     3,     2,     3,     3,     2,     3,     3,
       2,     3,     3,     2,     3,     3,     2,     3,     3,     2,
       3,     3,     2,     2,     1,     1,     1,     1,     2,     1,
       2,     1,     1,     2,     1,     1,     1,     1,     5,     5,
       1,     1,     1,     1,     1,     1,     1,     6,     6,     7,
       7,    10,    10,     9,     9,     7,     7,     5,     5,     6,
       6,     7,     7,    10,    10,     9,     9,     6,     7,     6,
       5,     6,     3,     5,     1,     2,     3,     3,     3,     2,
       3,     3,     4,     2,     5,     7,     6,     3,     1,     3,
       4,     6,     5,     1,     2,     4,     4,     5,     5,     2,
       3,     2,     3,     2,     3,     1,     3,     2,     2,     3,
       3,     3,     4,     4,     4,     4,     4,     1,     1,     1,
       1,     1,     1,     0,     2,     1,     2,     2,     4,     4,
       3,     3,     1,     1,     2,     2,     2,     2,     4,     4,
       1,     1,     2,     2,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* inputunit: simple_list simple_list_terminator  */
#line 396 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  /* Case of regular command.  Discard the error
			     safety net,and return the command just parsed. */
			  global_command = (yyvsp[-1].command);
			  eof_encountered = 0;
			  /* discard_parser_constructs (0); */
			  if (parser_state & PST_CMDSUBST)
			    parser_state |= PST_EOFTOKEN;
			  YYACCEPT;
			}
#line 1954 "y.tab.c"
    break;

  case 3: /* inputunit: comsub  */
#line 407 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  /* This is special; look at the production and how
			     parse_comsub sets token_to_read */
			  global_command = (yyvsp[0].command);
			  eof_encountered = 0;
			  YYACCEPT;
			}
#line 1966 "y.tab.c"
    break;

  case 4: /* inputunit: '\n'  */
#line 415 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  /* Case of regular command, but not a very
			     interesting one.  Return a NULL command. */
			  global_command = (COMMAND *)NULL;
			  if (parser_state & PST_CMDSUBST)
			    parser_state |= PST_EOFTOKEN;
			  YYACCEPT;
			}
#line 1979 "y.tab.c"
    break;

  case 5: /* inputunit: error '\n'  */
#line 424 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  /* Error during parsing.  Return NULL command. */
			  global_command = (COMMAND *)NULL;
			  eof_encountered = 0;
			  /* discard_parser_constructs (1); */
			  if (interactive && parse_and_execute_level == 0)
			    {
			      YYACCEPT;
			    }
			  else
			    {
			      YYABORT;
			    }
			}
#line 1998 "y.tab.c"
    break;

  case 6: /* inputunit: error yacc_EOF  */
#line 439 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  /* EOF after an error.  Do ignoreeof or not.  Really
			     only interesting in non-interactive shells */
			  global_command = (COMMAND *)NULL;
			  if (last_command_exit_value == 0)
			    last_command_exit_value = EX_BADUSAGE;	/* force error return */
			  if (interactive && parse_and_execute_level == 0)
			    {
			      handle_eof_input_unit ();
			      YYACCEPT;
			    }
			  else
			    {
			      YYABORT;
			    }
			}
#line 2019 "y.tab.c"
    break;

  case 7: /* inputunit: yacc_EOF  */
#line 456 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  /* Case of EOF seen by itself.  Do ignoreeof or
			     not. */
			  global_command = (COMMAND *)NULL;
			  handle_eof_input_unit ();
			  YYACCEPT;
			}
#line 2031 "y.tab.c"
    break;

  case 8: /* word_list: WORD  */
#line 466 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.word_list) = make_word_list ((yyvsp[0].word), (WORD_LIST *)NULL); }
#line 2037 "y.tab.c"
    break;

  case 9: /* word_list: word_list WORD  */
#line 468 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.word_list) = make_word_list ((yyvsp[0].word), (yyvsp[-1].word_list)); }
#line 2043 "y.tab.c"
    break;

  case 10: /* redirection: '>' WORD  */
#line 472 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 1;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_output_direction, redir, 0);
			}
#line 2053 "y.tab.c"
    break;

  case 11: /* redirection: '<' WORD  */
#line 478 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 0;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_input_direction, redir, 0);
			}
#line 2063 "y.tab.c"
    break;

  case 12: /* redirection: NUMBER '>' WORD  */
#line 484 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_output_direction, redir, 0);
			}
#line 2073 "y.tab.c"
    break;

  case 13: /* redirection: NUMBER '<' WORD  */
#line 490 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_input_direction, redir, 0);
			}
#line 2083 "y.tab.c"
    break;

  case 14: /* redirection: REDIR_WORD '>' WORD  */
#line 496 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_output_direction, redir, REDIR_VARASSIGN);
			}
#line 2093 "y.tab.c"
    break;

  case 15: /* redirection: REDIR_WORD '<' WORD  */
#line 502 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_input_direction, redir, REDIR_VARASSIGN);
			}
#line 2103 "y.tab.c"
    break;

  case 16: /* redirection: GREATER_GREATER WORD  */
#line 508 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 1;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_appending_to, redir, 0);
			}
#line 2113 "y.tab.c"
    break;

  case 17: /* redirection: NUMBER GREATER_GREATER WORD  */
#line 514 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_appending_to, redir, 0);
			}
#line 2123 "y.tab.c"
    break;

  case 18: /* redirection: REDIR_WORD GREATER_GREATER WORD  */
#line 520 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_appending_to, redir, REDIR_VARASSIGN);
			}
#line 2133 "y.tab.c"
    break;

  case 19: /* redirection: GREATER_BAR WORD  */
#line 526 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 1;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_output_force, redir, 0);
			}
#line 2143 "y.tab.c"
    break;

  case 20: /* redirection: NUMBER GREATER_BAR WORD  */
#line 532 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_output_force, redir, 0);
			}
#line 2153 "y.tab.c"
    break;

  case 21: /* redirection: REDIR_WORD GREATER_BAR WORD  */
#line 538 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_output_force, redir, REDIR_VARASSIGN);
			}
#line 2163 "y.tab.c"
    break;

  case 22: /* redirection: LESS_GREATER WORD  */
#line 544 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 0;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_input_output, redir, 0);
			}
#line 2173 "y.tab.c"
    break;

  case 23: /* redirection: NUMBER LESS_GREATER WORD  */
#line 550 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_input_output, redir, 0);
			}
#line 2183 "y.tab.c"
    break;

  case 24: /* redirection: REDIR_WORD LESS_GREATER WORD  */
#line 556 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_input_output, redir, REDIR_VARASSIGN);
			}
#line 2193 "y.tab.c"
    break;

  case 25: /* redirection: LESS_LESS WORD  */
#line 562 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 0;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_reading_until, redir, 0);
			  push_heredoc ((yyval.redirect));
			}
#line 2204 "y.tab.c"
    break;

  case 26: /* redirection: NUMBER LESS_LESS WORD  */
#line 569 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_reading_until, redir, 0);
			  push_heredoc ((yyval.redirect));
			}
#line 2215 "y.tab.c"
    break;

  case 27: /* redirection: REDIR_WORD LESS_LESS WORD  */
#line 576 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_reading_until, redir, REDIR_VARASSIGN);
			  push_heredoc ((yyval.redirect));
			}
#line 2226 "y.tab.c"
    break;

  case 28: /* redirection: LESS_LESS_MINUS WORD  */
#line 583 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 0;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_deblank_reading_until, redir, 0);
			  push_heredoc ((yyval.redirect));
			}
#line 2237 "y.tab.c"
    break;

  case 29: /* redirection: NUMBER LESS_LESS_MINUS WORD  */
#line 590 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_deblank_reading_until, redir, 0);
			  push_heredoc ((yyval.redirect));
			}
#line 2248 "y.tab.c"
    break;

  case 30: /* redirection: REDIR_WORD LESS_LESS_MINUS WORD  */
#line 597 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_deblank_reading_until, redir, REDIR_VARASSIGN);
			  push_heredoc ((yyval.redirect));
			}
#line 2259 "y.tab.c"
    break;

  case 31: /* redirection: LESS_LESS_LESS WORD  */
#line 604 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 0;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_reading_string, redir, 0);
			}
#line 2269 "y.tab.c"
    break;

  case 32: /* redirection: NUMBER LESS_LESS_LESS WORD  */
#line 610 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_reading_string, redir, 0);
			}
#line 2279 "y.tab.c"
    break;

  case 33: /* redirection: REDIR_WORD LESS_LESS_LESS WORD  */
#line 616 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_reading_string, redir, REDIR_VARASSIGN);
			}
#line 2289 "y.tab.c"
    break;

  case 34: /* redirection: LESS_AND NUMBER  */
#line 622 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 0;
			  redir.dest = (yyvsp[0].number);
			  (yyval.redirect) = make_redirection (source, r_duplicating_input, redir, 0);
			}
#line 2299 "y.tab.c"
    break;

  case 35: /* redirection: NUMBER LESS_AND NUMBER  */
#line 628 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.dest = (yyvsp[0].number);
			  (yyval.redirect) = make_redirection (source, r_duplicating_input, redir, 0);
			}
#line 2309 "y.tab.c"
    break;

  case 36: /* redirection: REDIR_WORD LESS_AND NUMBER  */
#line 634 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.dest = (yyvsp[0].number);
			  (yyval.redirect) = make_redirection (source, r_duplicating_input, redir, REDIR_VARASSIGN);
			}
#line 2319 "y.tab.c"
    break;

  case 37: /* redirection: GREATER_AND NUMBER  */
#line 640 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 1;
			  redir.dest = (yyvsp[0].number);
			  (yyval.redirect) = make_redirection (source, r_duplicating_output, redir, 0);
			}
#line 2329 "y.tab.c"
    break;

  case 38: /* redirection: NUMBER GREATER_AND NUMBER  */
#line 646 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.dest = (yyvsp[0].number);
			  (yyval.redirect) = make_redirection (source, r_duplicating_output, redir, 0);
			}
#line 2339 "y.tab.c"
    break;

  case 39: /* redirection: REDIR_WORD GREATER_AND NUMBER  */
#line 652 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.dest = (yyvsp[0].number);
			  (yyval.redirect) = make_redirection (source, r_duplicating_output, redir, REDIR_VARASSIGN);
			}
#line 2349 "y.tab.c"
    break;

  case 40: /* redirection: LESS_AND WORD  */
#line 658 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 0;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_duplicating_input_word, redir, 0);
			}
#line 2359 "y.tab.c"
    break;

  case 41: /* redirection: NUMBER LESS_AND WORD  */
#line 664 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_duplicating_input_word, redir, 0);
			}
#line 2369 "y.tab.c"
    break;

  case 42: /* redirection: REDIR_WORD LESS_AND WORD  */
#line 670 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_duplicating_input_word, redir, REDIR_VARASSIGN);
			}
#line 2379 "y.tab.c"
    break;

  case 43: /* redirection: GREATER_AND WORD  */
#line 676 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 1;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_duplicating_output_word, redir, 0);
			}
#line 2389 "y.tab.c"
    break;

  case 44: /* redirection: NUMBER GREATER_AND WORD  */
#line 682 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_duplicating_output_word, redir, 0);
			}
#line 2399 "y.tab.c"
    break;

  case 45: /* redirection: REDIR_WORD GREATER_AND WORD  */
#line 688 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_duplicating_output_word, redir, REDIR_VARASSIGN);
			}
#line 2409 "y.tab.c"
    break;

  case 46: /* redirection: GREATER_AND '-'  */
#line 694 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 1;
			  redir.dest = 0;
			  (yyval.redirect) = make_redirection (source, r_close_this, redir, 0);
			}
#line 2419 "y.tab.c"
    break;

  case 47: /* redirection: NUMBER GREATER_AND '-'  */
#line 700 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.dest = 0;
			  (yyval.redirect) = make_redirection (source, r_close_this, redir, 0);
			}
#line 2429 "y.tab.c"
    break;

  case 48: /* redirection: REDIR_WORD GREATER_AND '-'  */
#line 706 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.dest = 0;
			  (yyval.redirect) = make_redirection (source, r_close_this, redir, REDIR_VARASSIGN);
			}
#line 2439 "y.tab.c"
    break;

  case 49: /* redirection: LESS_AND '-'  */
#line 712 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 0;
			  redir.dest = 0;
			  (yyval.redirect) = make_redirection (source, r_close_this, redir, 0);
			}
#line 2449 "y.tab.c"
    break;

  case 50: /* redirection: NUMBER LESS_AND '-'  */
#line 718 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = (yyvsp[-2].number);
			  redir.dest = 0;
			  (yyval.redirect) = make_redirection (source, r_close_this, redir, 0);
			}
#line 2459 "y.tab.c"
    break;

  case 51: /* redirection: REDIR_WORD LESS_AND '-'  */
#line 724 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.filename = (yyvsp[-2].word);
			  redir.dest = 0;
			  (yyval.redirect) = make_redirection (source, r_close_this, redir, REDIR_VARASSIGN);
			}
#line 2469 "y.tab.c"
    break;

  case 52: /* redirection: AND_GREATER WORD  */
#line 730 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 1;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_err_and_out, redir, 0);
			}
#line 2479 "y.tab.c"
    break;

  case 53: /* redirection: AND_GREATER_GREATER WORD  */
#line 736 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  source.dest = 1;
			  redir.filename = (yyvsp[0].word);
			  (yyval.redirect) = make_redirection (source, r_append_err_and_out, redir, 0);
			}
#line 2489 "y.tab.c"
    break;

  case 54: /* simple_command_element: WORD  */
#line 744 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.element).word = (yyvsp[0].word); (yyval.element).redirect = 0; }
#line 2495 "y.tab.c"
    break;

  case 55: /* simple_command_element: ASSIGNMENT_WORD  */
#line 746 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.element).word = (yyvsp[0].word); (yyval.element).redirect = 0; }
#line 2501 "y.tab.c"
    break;

  case 56: /* simple_command_element: redirection  */
#line 748 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.element).redirect = (yyvsp[0].redirect); (yyval.element).word = 0; }
#line 2507 "y.tab.c"
    break;

  case 57: /* redirection_list: redirection  */
#line 752 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.redirect) = (yyvsp[0].redirect);
			}
#line 2515 "y.tab.c"
    break;

  case 58: /* redirection_list: redirection_list redirection  */
#line 756 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  register REDIRECT *t;

			  for (t = (yyvsp[-1].redirect); t->next; t = t->next)
			    ;
			  t->next = (yyvsp[0].redirect);
			  (yyval.redirect) = (yyvsp[-1].redirect);
			}
#line 2528 "y.tab.c"
    break;

  case 59: /* simple_command: simple_command_element  */
#line 767 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_simple_command ((yyvsp[0].element), (COMMAND *)NULL); }
#line 2534 "y.tab.c"
    break;

  case 60: /* simple_command: simple_command simple_command_element  */
#line 769 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_simple_command ((yyvsp[0].element), (yyvsp[-1].command)); }
#line 2540 "y.tab.c"
    break;

  case 61: /* command: simple_command  */
#line 773 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = clean_simple_command ((yyvsp[0].command)); }
#line 2546 "y.tab.c"
    break;

  case 62: /* command: shell_command  */
#line 775 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2552 "y.tab.c"
    break;

  case 63: /* command: shell_command redirection_list  */
#line 777 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  COMMAND *tc;

			  tc = (yyvsp[-1].command);
			  if (tc && tc->redirects)
			    {
			      register REDIRECT *t;
			      for (t = tc->redirects; t->next; t = t->next)
				;
			      t->next = (yyvsp[0].redirect);
			    }
			  else if (tc)
			    tc->redirects = (yyvsp[0].redirect);
			  (yyval.command) = (yyvsp[-1].command);
			}
#line 2572 "y.tab.c"
    break;

  case 64: /* command: function_def  */
#line 793 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2578 "y.tab.c"
    break;

  case 65: /* command: coproc  */
#line 795 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2584 "y.tab.c"
    break;

  case 66: /* shell_command: for_command  */
#line 799 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2590 "y.tab.c"
    break;

  case 67: /* shell_command: case_command  */
#line 801 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2596 "y.tab.c"
    break;

  case 68: /* shell_command: WHILE compound_list DO compound_list DONE  */
#line 803 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_while_command ((yyvsp[-3].command), (yyvsp[-1].command)); }
#line 2602 "y.tab.c"
    break;

  case 69: /* shell_command: UNTIL compound_list DO compound_list DONE  */
#line 805 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_until_command ((yyvsp[-3].command), (yyvsp[-1].command)); }
#line 2608 "y.tab.c"
    break;

  case 70: /* shell_command: select_command  */
#line 807 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2614 "y.tab.c"
    break;

  case 71: /* shell_command: if_command  */
#line 809 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2620 "y.tab.c"
    break;

  case 72: /* shell_command: subshell  */
#line 811 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2626 "y.tab.c"
    break;

  case 73: /* shell_command: group_command  */
#line 813 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2632 "y.tab.c"
    break;

  case 74: /* shell_command: arith_command  */
#line 815 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2638 "y.tab.c"
    break;

  case 75: /* shell_command: cond_command  */
#line 817 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2644 "y.tab.c"
    break;

  case 76: /* shell_command: arith_for_command  */
#line 819 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2650 "y.tab.c"
    break;

  case 77: /* for_command: FOR WORD newline_list DO compound_list DONE  */
#line 823 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_for_command ((yyvsp[-4].word), add_string_to_list ("\"$@\"", (WORD_LIST *)NULL), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2659 "y.tab.c"
    break;

  case 78: /* for_command: FOR WORD newline_list '{' compound_list '}'  */
#line 828 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_for_command ((yyvsp[-4].word), add_string_to_list ("\"$@\"", (WORD_LIST *)NULL), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2668 "y.tab.c"
    break;

  case 79: /* for_command: FOR WORD ';' newline_list DO compound_list DONE  */
#line 833 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_for_command ((yyvsp[-5].word), add_string_to_list ("\"$@\"", (WORD_LIST *)NULL), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2677 "y.tab.c"
    break;

  case 80: /* for_command: FOR WORD ';' newline_list '{' compound_list '}'  */
#line 838 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_for_command ((yyvsp[-5].word), add_string_to_list ("\"$@\"", (WORD_LIST *)NULL), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2686 "y.tab.c"
    break;

  case 81: /* for_command: FOR WORD newline_list IN word_list list_terminator newline_list DO compound_list DONE  */
#line 843 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_for_command ((yyvsp[-8].word), REVERSE_LIST ((yyvsp[-5].word_list), WORD_LIST *), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2695 "y.tab.c"
    break;

  case 82: /* for_command: FOR WORD newline_list IN word_list list_terminator newline_list '{' compound_list '}'  */
#line 848 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_for_command ((yyvsp[-8].word), REVERSE_LIST ((yyvsp[-5].word_list), WORD_LIST *), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2704 "y.tab.c"
    break;

  case 83: /* for_command: FOR WORD newline_list IN list_terminator newline_list DO compound_list DONE  */
#line 853 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_for_command ((yyvsp[-7].word), (WORD_LIST *)NULL, (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2713 "y.tab.c"
    break;

  case 84: /* for_command: FOR WORD newline_list IN list_terminator newline_list '{' compound_list '}'  */
#line 858 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_for_command ((yyvsp[-7].word), (WORD_LIST *)NULL, (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2722 "y.tab.c"
    break;

  case 85: /* arith_for_command: FOR ARITH_FOR_EXPRS list_terminator newline_list DO compound_list DONE  */
#line 865 "/usr/local/src/chet/src/bash/src/parse.y"
                                {
				  (yyval.command) = make_arith_for_command ((yyvsp[-5].word_list), (yyvsp[-1].command), arith_for_lineno);
				  if ((yyval.command) == 0) YYERROR;
				  if (word_top > 0) word_top--;
				}
#line 2732 "y.tab.c"
    break;

  case 86: /* arith_for_command: FOR ARITH_FOR_EXPRS list_terminator newline_list '{' compound_list '}'  */
#line 871 "/usr/local/src/chet/src/bash/src/parse.y"
                                {
				  (yyval.command) = make_arith_for_command ((yyvsp[-5].word_list), (yyvsp[-1].command), arith_for_lineno);
				  if ((yyval.command) == 0) YYERROR;
				  if (word_top > 0) word_top--;
				}
#line 2742 "y.tab.c"
    break;

  case 87: /* arith_for_command: FOR ARITH_FOR_EXPRS DO compound_list DONE  */
#line 877 "/usr/local/src/chet/src/bash/src/parse.y"
                                {
				  (yyval.command) = make_arith_for_command ((yyvsp[-3].word_list), (yyvsp[-1].command), arith_for_lineno);
				  if ((yyval.command) == 0) YYERROR;
				  if (word_top > 0) word_top--;
				}
#line 2752 "y.tab.c"
    break;

  case 88: /* arith_for_command: FOR ARITH_FOR_EXPRS '{' compound_list '}'  */
#line 883 "/usr/local/src/chet/src/bash/src/parse.y"
                                {
				  (yyval.command) = make_arith_for_command ((yyvsp[-3].word_list), (yyvsp[-1].command), arith_for_lineno);
				  if ((yyval.command) == 0) YYERROR;
				  if (word_top > 0) word_top--;
				}
#line 2762 "y.tab.c"
    break;

  case 89: /* select_command: SELECT WORD newline_list DO compound_list DONE  */
#line 891 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_select_command ((yyvsp[-4].word), add_string_to_list ("\"$@\"", (WORD_LIST *)NULL), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2771 "y.tab.c"
    break;

  case 90: /* select_command: SELECT WORD newline_list '{' compound_list '}'  */
#line 896 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_select_command ((yyvsp[-4].word), add_string_to_list ("\"$@\"", (WORD_LIST *)NULL), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2780 "y.tab.c"
    break;

  case 91: /* select_command: SELECT WORD ';' newline_list DO compound_list DONE  */
#line 901 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_select_command ((yyvsp[-5].word), add_string_to_list ("\"$@\"", (WORD_LIST *)NULL), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2789 "y.tab.c"
    break;

  case 92: /* select_command: SELECT WORD ';' newline_list '{' compound_list '}'  */
#line 906 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_select_command ((yyvsp[-5].word), add_string_to_list ("\"$@\"", (WORD_LIST *)NULL), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2798 "y.tab.c"
    break;

  case 93: /* select_command: SELECT WORD newline_list IN word_list list_terminator newline_list DO compound_list DONE  */
#line 911 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_select_command ((yyvsp[-8].word), REVERSE_LIST ((yyvsp[-5].word_list), WORD_LIST *), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2807 "y.tab.c"
    break;

  case 94: /* select_command: SELECT WORD newline_list IN word_list list_terminator newline_list '{' compound_list '}'  */
#line 916 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_select_command ((yyvsp[-8].word), REVERSE_LIST ((yyvsp[-5].word_list), WORD_LIST *), (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2816 "y.tab.c"
    break;

  case 95: /* select_command: SELECT WORD newline_list IN list_terminator newline_list DO compound_list DONE  */
#line 921 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_select_command ((yyvsp[-7].word), (WORD_LIST *)NULL, (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2825 "y.tab.c"
    break;

  case 96: /* select_command: SELECT WORD newline_list IN list_terminator newline_list '{' compound_list '}'  */
#line 926 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_select_command ((yyvsp[-7].word), (WORD_LIST *)NULL, (yyvsp[-1].command), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2834 "y.tab.c"
    break;

  case 97: /* case_command: CASE WORD newline_list IN newline_list ESAC  */
#line 933 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_case_command ((yyvsp[-4].word), (PATTERN_LIST *)NULL, word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2843 "y.tab.c"
    break;

  case 98: /* case_command: CASE WORD newline_list IN case_clause_sequence newline_list ESAC  */
#line 938 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_case_command ((yyvsp[-5].word), (yyvsp[-2].pattern), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2852 "y.tab.c"
    break;

  case 99: /* case_command: CASE WORD newline_list IN case_clause ESAC  */
#line 943 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_case_command ((yyvsp[-4].word), (yyvsp[-1].pattern), word_lineno[word_top]);
			  if (word_top > 0) word_top--;
			}
#line 2861 "y.tab.c"
    break;

  case 100: /* function_def: WORD '(' ')' newline_list function_body  */
#line 950 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_function_def ((yyvsp[-4].word), (yyvsp[0].command), function_dstart, function_bstart); }
#line 2867 "y.tab.c"
    break;

  case 101: /* function_def: FUNCTION WORD '(' ')' newline_list function_body  */
#line 952 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_function_def ((yyvsp[-4].word), (yyvsp[0].command), function_dstart, function_bstart); }
#line 2873 "y.tab.c"
    break;

  case 102: /* function_def: FUNCTION WORD function_body  */
#line 954 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_function_def ((yyvsp[-1].word), (yyvsp[0].command), function_dstart, function_bstart); }
#line 2879 "y.tab.c"
    break;

  case 103: /* function_def: FUNCTION WORD '\n' newline_list function_body  */
#line 956 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_function_def ((yyvsp[-3].word), (yyvsp[0].command), function_dstart, function_bstart); }
#line 2885 "y.tab.c"
    break;

  case 104: /* function_body: shell_command  */
#line 960 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 2891 "y.tab.c"
    break;

  case 105: /* function_body: shell_command redirection_list  */
#line 962 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  COMMAND *tc;

			  tc = (yyvsp[-1].command);
			  /* According to Posix.2 3.9.5, redirections
			     specified after the body of a function should
			     be attached to the function and performed when
			     the function is executed, not as part of the
			     function definition command. */
			  /* XXX - I don't think it matters, but we might
			     want to change this in the future to avoid
			     problems differentiating between a function
			     definition with a redirection and a function
			     definition containing a single command with a
			     redirection.  The two are semantically equivalent,
			     though -- the only difference is in how the
			     command printing code displays the redirections. */
			  if (tc && tc->redirects)
			    {
			      register REDIRECT *t;
			      for (t = tc->redirects; t->next; t = t->next)
				;
			      t->next = (yyvsp[0].redirect);
			    }
			  else if (tc)
			    tc->redirects = (yyvsp[0].redirect);
			  (yyval.command) = (yyvsp[-1].command);
			}
#line 2924 "y.tab.c"
    break;

  case 106: /* subshell: '(' compound_list ')'  */
#line 993 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_subshell_command ((yyvsp[-1].command));
			  (yyval.command)->flags |= CMD_WANT_SUBSHELL;
			}
#line 2933 "y.tab.c"
    break;

  case 107: /* comsub: DOLPAREN compound_list ')'  */
#line 1000 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = (yyvsp[-1].command);
			}
#line 2941 "y.tab.c"
    break;

  case 108: /* comsub: DOLPAREN newline_list ')'  */
#line 1004 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = (COMMAND *)NULL;
			}
#line 2949 "y.tab.c"
    break;

  case 109: /* coproc: COPROC shell_command  */
#line 1010 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_coproc_command ("COPROC", (yyvsp[0].command));
			  (yyval.command)->flags |= CMD_WANT_SUBSHELL|CMD_COPROC_SUBSHELL;
			}
#line 2958 "y.tab.c"
    break;

  case 110: /* coproc: COPROC shell_command redirection_list  */
#line 1015 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  COMMAND *tc;

			  tc = (yyvsp[-1].command);
			  if (tc && tc->redirects)
			    {
			      register REDIRECT *t;
			      for (t = tc->redirects; t->next; t = t->next)
				;
			      t->next = (yyvsp[0].redirect);
			    }
			  else if (tc)
			    tc->redirects = (yyvsp[0].redirect);
			  (yyval.command) = make_coproc_command ("COPROC", (yyvsp[-1].command));
			  (yyval.command)->flags |= CMD_WANT_SUBSHELL|CMD_COPROC_SUBSHELL;
			}
#line 2979 "y.tab.c"
    break;

  case 111: /* coproc: COPROC WORD shell_command  */
#line 1032 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_coproc_command ((yyvsp[-1].word)->word, (yyvsp[0].command));
			  (yyval.command)->flags |= CMD_WANT_SUBSHELL|CMD_COPROC_SUBSHELL;
			}
#line 2988 "y.tab.c"
    break;

  case 112: /* coproc: COPROC WORD shell_command redirection_list  */
#line 1037 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  COMMAND *tc;

			  tc = (yyvsp[-1].command);
			  if (tc && tc->redirects)
			    {
			      register REDIRECT *t;
			      for (t = tc->redirects; t->next; t = t->next)
				;
			      t->next = (yyvsp[0].redirect);
			    }
			  else if (tc)
			    tc->redirects = (yyvsp[0].redirect);
			  (yyval.command) = make_coproc_command ((yyvsp[-2].word)->word, (yyvsp[-1].command));
			  (yyval.command)->flags |= CMD_WANT_SUBSHELL|CMD_COPROC_SUBSHELL;
			}
#line 3009 "y.tab.c"
    break;

  case 113: /* coproc: COPROC simple_command  */
#line 1054 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = make_coproc_command ("COPROC", clean_simple_command ((yyvsp[0].command)));
			  (yyval.command)->flags |= CMD_WANT_SUBSHELL|CMD_COPROC_SUBSHELL;
			}
#line 3018 "y.tab.c"
    break;

  case 114: /* if_command: IF compound_list THEN compound_list FI  */
#line 1061 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_if_command ((yyvsp[-3].command), (yyvsp[-1].command), (COMMAND *)NULL); }
#line 3024 "y.tab.c"
    break;

  case 115: /* if_command: IF compound_list THEN compound_list ELSE compound_list FI  */
#line 1063 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_if_command ((yyvsp[-5].command), (yyvsp[-3].command), (yyvsp[-1].command)); }
#line 3030 "y.tab.c"
    break;

  case 116: /* if_command: IF compound_list THEN compound_list elif_clause FI  */
#line 1065 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_if_command ((yyvsp[-4].command), (yyvsp[-2].command), (yyvsp[-1].command)); }
#line 3036 "y.tab.c"
    break;

  case 117: /* group_command: '{' compound_list '}'  */
#line 1070 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_group_command ((yyvsp[-1].command)); }
#line 3042 "y.tab.c"
    break;

  case 118: /* arith_command: ARITH_CMD  */
#line 1074 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_arith_command ((yyvsp[0].word_list)); }
#line 3048 "y.tab.c"
    break;

  case 119: /* cond_command: COND_START COND_CMD COND_END  */
#line 1078 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[-1].command); }
#line 3054 "y.tab.c"
    break;

  case 120: /* elif_clause: ELIF compound_list THEN compound_list  */
#line 1082 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_if_command ((yyvsp[-2].command), (yyvsp[0].command), (COMMAND *)NULL); }
#line 3060 "y.tab.c"
    break;

  case 121: /* elif_clause: ELIF compound_list THEN compound_list ELSE compound_list  */
#line 1084 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_if_command ((yyvsp[-4].command), (yyvsp[-2].command), (yyvsp[0].command)); }
#line 3066 "y.tab.c"
    break;

  case 122: /* elif_clause: ELIF compound_list THEN compound_list elif_clause  */
#line 1086 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = make_if_command ((yyvsp[-3].command), (yyvsp[-1].command), (yyvsp[0].command)); }
#line 3072 "y.tab.c"
    break;

  case 124: /* case_clause: case_clause_sequence pattern_list  */
#line 1091 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyvsp[0].pattern)->next = (yyvsp[-1].pattern); (yyval.pattern) = (yyvsp[0].pattern); }
#line 3078 "y.tab.c"
    break;

  case 125: /* pattern_list: newline_list pattern ')' compound_list  */
#line 1095 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.pattern) = make_pattern_list ((yyvsp[-2].word_list), (yyvsp[0].command)); }
#line 3084 "y.tab.c"
    break;

  case 126: /* pattern_list: newline_list pattern ')' newline_list  */
#line 1097 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.pattern) = make_pattern_list ((yyvsp[-2].word_list), (COMMAND *)NULL); }
#line 3090 "y.tab.c"
    break;

  case 127: /* pattern_list: newline_list '(' pattern ')' compound_list  */
#line 1099 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.pattern) = make_pattern_list ((yyvsp[-2].word_list), (yyvsp[0].command)); }
#line 3096 "y.tab.c"
    break;

  case 128: /* pattern_list: newline_list '(' pattern ')' newline_list  */
#line 1101 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.pattern) = make_pattern_list ((yyvsp[-2].word_list), (COMMAND *)NULL); }
#line 3102 "y.tab.c"
    break;

  case 129: /* case_clause_sequence: pattern_list SEMI_SEMI  */
#line 1105 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.pattern) = (yyvsp[-1].pattern); }
#line 3108 "y.tab.c"
    break;

  case 130: /* case_clause_sequence: case_clause_sequence pattern_list SEMI_SEMI  */
#line 1107 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyvsp[-1].pattern)->next = (yyvsp[-2].pattern); (yyval.pattern) = (yyvsp[-1].pattern); }
#line 3114 "y.tab.c"
    break;

  case 131: /* case_clause_sequence: pattern_list SEMI_AND  */
#line 1109 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyvsp[-1].pattern)->flags |= CASEPAT_FALLTHROUGH; (yyval.pattern) = (yyvsp[-1].pattern); }
#line 3120 "y.tab.c"
    break;

  case 132: /* case_clause_sequence: case_clause_sequence pattern_list SEMI_AND  */
#line 1111 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyvsp[-1].pattern)->flags |= CASEPAT_FALLTHROUGH; (yyvsp[-1].pattern)->next = (yyvsp[-2].pattern); (yyval.pattern) = (yyvsp[-1].pattern); }
#line 3126 "y.tab.c"
    break;

  case 133: /* case_clause_sequence: pattern_list SEMI_SEMI_AND  */
#line 1113 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyvsp[-1].pattern)->flags |= CASEPAT_TESTNEXT; (yyval.pattern) = (yyvsp[-1].pattern); }
#line 3132 "y.tab.c"
    break;

  case 134: /* case_clause_sequence: case_clause_sequence pattern_list SEMI_SEMI_AND  */
#line 1115 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyvsp[-1].pattern)->flags |= CASEPAT_TESTNEXT; (yyvsp[-1].pattern)->next = (yyvsp[-2].pattern); (yyval.pattern) = (yyvsp[-1].pattern); }
#line 3138 "y.tab.c"
    break;

  case 135: /* pattern: WORD  */
#line 1119 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.word_list) = make_word_list ((yyvsp[0].word), (WORD_LIST *)NULL); }
#line 3144 "y.tab.c"
    break;

  case 136: /* pattern: pattern '|' WORD  */
#line 1121 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.word_list) = make_word_list ((yyvsp[0].word), (yyvsp[-2].word_list)); }
#line 3150 "y.tab.c"
    break;

  case 137: /* compound_list: newline_list list0  */
#line 1130 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = (yyvsp[0].command);
			  if (need_here_doc && last_read_token == '\n')
			    gather_here_documents ();
			 }
#line 3160 "y.tab.c"
    break;

  case 138: /* compound_list: newline_list list1  */
#line 1136 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = (yyvsp[0].command);
			}
#line 3168 "y.tab.c"
    break;

  case 140: /* list0: list1 '&' newline_list  */
#line 1143 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  if ((yyvsp[-2].command)->type == cm_connection)
			    (yyval.command) = connect_async_list ((yyvsp[-2].command), (COMMAND *)NULL, '&');
			  else
			    (yyval.command) = command_connect ((yyvsp[-2].command), (COMMAND *)NULL, '&');
			}
#line 3179 "y.tab.c"
    break;

  case 142: /* list1: list1 AND_AND newline_list list1  */
#line 1154 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), AND_AND); }
#line 3185 "y.tab.c"
    break;

  case 143: /* list1: list1 OR_OR newline_list list1  */
#line 1156 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), OR_OR); }
#line 3191 "y.tab.c"
    break;

  case 144: /* list1: list1 '&' newline_list list1  */
#line 1158 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  if ((yyvsp[-3].command)->type == cm_connection)
			    (yyval.command) = connect_async_list ((yyvsp[-3].command), (yyvsp[0].command), '&');
			  else
			    (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), '&');
			}
#line 3202 "y.tab.c"
    break;

  case 145: /* list1: list1 ';' newline_list list1  */
#line 1165 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), ';'); }
#line 3208 "y.tab.c"
    break;

  case 146: /* list1: list1 '\n' newline_list list1  */
#line 1167 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  if (parser_state & PST_CMDSUBST)
			    (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), '\n');
			  else
			    (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), ';');
			}
#line 3219 "y.tab.c"
    break;

  case 147: /* list1: pipeline_command  */
#line 1174 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 3225 "y.tab.c"
    break;

  case 150: /* list_terminator: '\n'  */
#line 1182 "/usr/local/src/chet/src/bash/src/parse.y"
                { (yyval.number) = '\n'; }
#line 3231 "y.tab.c"
    break;

  case 151: /* list_terminator: ';'  */
#line 1184 "/usr/local/src/chet/src/bash/src/parse.y"
                { (yyval.number) = ';'; }
#line 3237 "y.tab.c"
    break;

  case 152: /* list_terminator: yacc_EOF  */
#line 1186 "/usr/local/src/chet/src/bash/src/parse.y"
                { (yyval.number) = yacc_EOF; }
#line 3243 "y.tab.c"
    break;

  case 155: /* simple_list: simple_list1  */
#line 1200 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = (yyvsp[0].command);
			  if (need_here_doc)
			    gather_here_documents ();	/* XXX */
			  if ((parser_state & PST_CMDSUBST) && current_token == shell_eof_token)
			    {
INTERNAL_DEBUG (("LEGACY: parser: command substitution simple_list1 -> simple_list"));
			      global_command = (yyvsp[0].command);
			      eof_encountered = 0;
			      if (bash_input.type == st_string)
				rewind_input_string ();
			      YYACCEPT;
			    }
			}
#line 3262 "y.tab.c"
    break;

  case 156: /* simple_list: simple_list1 '&'  */
#line 1215 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  if ((yyvsp[-1].command)->type == cm_connection)
			    (yyval.command) = connect_async_list ((yyvsp[-1].command), (COMMAND *)NULL, '&');
			  else
			    (yyval.command) = command_connect ((yyvsp[-1].command), (COMMAND *)NULL, '&');
			  if (need_here_doc)
			    gather_here_documents (); /* XXX */
			  if ((parser_state & PST_CMDSUBST) && current_token == shell_eof_token)
			    {
INTERNAL_DEBUG (("LEGACY: parser: command substitution simple_list1 '&' -> simple_list"));
			      global_command = (yyvsp[-1].command);
			      eof_encountered = 0;
			      if (bash_input.type == st_string)
				rewind_input_string ();
			      YYACCEPT;
			    }
			}
#line 3284 "y.tab.c"
    break;

  case 157: /* simple_list: simple_list1 ';'  */
#line 1233 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  (yyval.command) = (yyvsp[-1].command);
			  if (need_here_doc)
			    gather_here_documents ();	/* XXX */
			  if ((parser_state & PST_CMDSUBST) && current_token == shell_eof_token)
			    {
INTERNAL_DEBUG (("LEGACY: parser: command substitution simple_list1 ';' -> simple_list"));
			      global_command = (yyvsp[-1].command);
			      eof_encountered = 0;
			      if (bash_input.type == st_string)
				rewind_input_string ();
			      YYACCEPT;
			    }
			}
#line 3303 "y.tab.c"
    break;

  case 158: /* simple_list1: simple_list1 AND_AND newline_list simple_list1  */
#line 1250 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), AND_AND); }
#line 3309 "y.tab.c"
    break;

  case 159: /* simple_list1: simple_list1 OR_OR newline_list simple_list1  */
#line 1252 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), OR_OR); }
#line 3315 "y.tab.c"
    break;

  case 160: /* simple_list1: simple_list1 '&' simple_list1  */
#line 1254 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  if ((yyvsp[-2].command)->type == cm_connection)
			    (yyval.command) = connect_async_list ((yyvsp[-2].command), (yyvsp[0].command), '&');
			  else
			    (yyval.command) = command_connect ((yyvsp[-2].command), (yyvsp[0].command), '&');
			}
#line 3326 "y.tab.c"
    break;

  case 161: /* simple_list1: simple_list1 ';' simple_list1  */
#line 1261 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = command_connect ((yyvsp[-2].command), (yyvsp[0].command), ';'); }
#line 3332 "y.tab.c"
    break;

  case 162: /* simple_list1: pipeline_command  */
#line 1264 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 3338 "y.tab.c"
    break;

  case 163: /* pipeline_command: pipeline  */
#line 1268 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 3344 "y.tab.c"
    break;

  case 164: /* pipeline_command: BANG pipeline_command  */
#line 1270 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  if ((yyvsp[0].command))
			    (yyvsp[0].command)->flags ^= CMD_INVERT_RETURN;	/* toggle */
			  (yyval.command) = (yyvsp[0].command);
			}
#line 3354 "y.tab.c"
    break;

  case 165: /* pipeline_command: timespec pipeline_command  */
#line 1276 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  if ((yyvsp[0].command))
			    (yyvsp[0].command)->flags |= (yyvsp[-1].number);
			  (yyval.command) = (yyvsp[0].command);
			}
#line 3364 "y.tab.c"
    break;

  case 166: /* pipeline_command: timespec list_terminator  */
#line 1282 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  ELEMENT x;

			  /* Boy, this is unclean.  `time' by itself can
			     time a null command.  We cheat and push a
			     newline back if the list_terminator was a newline
			     to avoid the double-newline problem (one to
			     terminate this, one to terminate the command) */
			  x.word = 0;
			  x.redirect = 0;
			  (yyval.command) = make_simple_command (x, (COMMAND *)NULL);
			  (yyval.command)->flags |= (yyvsp[-1].number);
			  /* XXX - let's cheat and push a newline back */
			  if ((yyvsp[0].number) == '\n')
			    token_to_read = '\n';
			  else if ((yyvsp[0].number) == ';')
			    token_to_read = ';';
			  parser_state &= ~PST_REDIRLIST;	/* make_simple_command sets this */
			}
#line 3388 "y.tab.c"
    break;

  case 167: /* pipeline_command: BANG list_terminator  */
#line 1302 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  ELEMENT x;

			  /* This is just as unclean.  Posix says that `!'
			     by itself should be equivalent to `false'.
			     We cheat and push a
			     newline back if the list_terminator was a newline
			     to avoid the double-newline problem (one to
			     terminate this, one to terminate the command) */
			  x.word = 0;
			  x.redirect = 0;
			  (yyval.command) = make_simple_command (x, (COMMAND *)NULL);
			  (yyval.command)->flags |= CMD_INVERT_RETURN;
			  /* XXX - let's cheat and push a newline back */
			  if ((yyvsp[0].number) == '\n')
			    token_to_read = '\n';
			  if ((yyvsp[0].number) == ';')
			    token_to_read = ';';
			  parser_state &= ~PST_REDIRLIST;	/* make_simple_command sets this */
			}
#line 3413 "y.tab.c"
    break;

  case 168: /* pipeline: pipeline '|' newline_list pipeline  */
#line 1325 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), '|'); }
#line 3419 "y.tab.c"
    break;

  case 169: /* pipeline: pipeline BAR_AND newline_list pipeline  */
#line 1327 "/usr/local/src/chet/src/bash/src/parse.y"
                        {
			  /* Make cmd1 |& cmd2 equivalent to cmd1 2>&1 | cmd2 */
			  COMMAND *tc;
			  REDIRECTEE rd, sd;
			  REDIRECT *r;

			  tc = (yyvsp[-3].command)->type == cm_simple ? (COMMAND *)(yyvsp[-3].command)->value.Simple : (yyvsp[-3].command);
			  sd.dest = 2;
			  rd.dest = 1;
			  r = make_redirection (sd, r_duplicating_output, rd, 0);
			  if (tc->redirects)
			    {
			      register REDIRECT *t;
			      for (t = tc->redirects; t->next; t = t->next)
				;
			      t->next = r;
			    }
			  else
			    tc->redirects = r;

			  (yyval.command) = command_connect ((yyvsp[-3].command), (yyvsp[0].command), '|');
			}
#line 3446 "y.tab.c"
    break;

  case 170: /* pipeline: command  */
#line 1350 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.command) = (yyvsp[0].command); }
#line 3452 "y.tab.c"
    break;

  case 171: /* timespec: TIME  */
#line 1354 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.number) = CMD_TIME_PIPELINE; }
#line 3458 "y.tab.c"
    break;

  case 172: /* timespec: TIME TIMEOPT  */
#line 1356 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.number) = CMD_TIME_PIPELINE|CMD_TIME_POSIX; }
#line 3464 "y.tab.c"
    break;

  case 173: /* timespec: TIME TIMEIGN  */
#line 1358 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.number) = CMD_TIME_PIPELINE|CMD_TIME_POSIX; }
#line 3470 "y.tab.c"
    break;

  case 174: /* timespec: TIME TIMEOPT TIMEIGN  */
#line 1360 "/usr/local/src/chet/src/bash/src/parse.y"
                        { (yyval.number) = CMD_TIME_PIPELINE|CMD_TIME_POSIX; }
#line 3476 "y.tab.c"
    break;


#line 3480 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1362 "/usr/local/src/chet/src/bash/src/parse.y"


/* Initial size to allocate for tokens, and the
   amount to grow them by. */
#define TOKEN_DEFAULT_INITIAL_SIZE 496
#define TOKEN_DEFAULT_GROW_SIZE 512

/* Should we call prompt_again? */
#define SHOULD_PROMPT() \
  (interactive && (bash_input.type == st_stdin || bash_input.type == st_stream))

#if defined (ALIAS)
#  define expanding_alias() (pushed_string_list && pushed_string_list->expander)
#else
#  define expanding_alias() 0
#endif

/* Global var is non-zero when end of file has been reached. */
int EOF_Reached = 0;

#ifdef DEBUG
static void
debug_parser (i)
     int i;
{
#if YYDEBUG != 0
  yydebug = i;
  yyoutstream = stdout;
  yyerrstream = stderr;
#endif
}
#endif

/* yy_getc () returns the next available character from input or EOF.
   yy_ungetc (c) makes `c' the next character to read.
   init_yy_io (get, unget, type, location) makes the function GET the
   installed function for getting the next character, makes UNGET the
   installed function for un-getting a character, sets the type of stream
   (either string or file) from TYPE, and makes LOCATION point to where
   the input is coming from. */

/* Unconditionally returns end-of-file. */
int
return_EOF ()
{
  return (EOF);
}

/* Variable containing the current get and unget functions.
   See ./input.h for a clearer description. */
BASH_INPUT bash_input;

/* Set all of the fields in BASH_INPUT to NULL.  Free bash_input.name if it
   is non-null, avoiding a memory leak. */
void
initialize_bash_input ()
{
  bash_input.type = st_none;
  FREE (bash_input.name);
  bash_input.name = (char *)NULL;
  bash_input.location.file = (FILE *)NULL;
  bash_input.location.string = (char *)NULL;
  bash_input.getter = (sh_cget_func_t *)NULL;
  bash_input.ungetter = (sh_cunget_func_t *)NULL;
}

/* Set the contents of the current bash input stream from
   GET, UNGET, TYPE, NAME, and LOCATION. */
void
init_yy_io (get, unget, type, name, location)
     sh_cget_func_t *get;
     sh_cunget_func_t *unget;
     enum stream_type type;
     const char *name;
     INPUT_STREAM location;
{
  bash_input.type = type;
  FREE (bash_input.name);
  bash_input.name = name ? savestring (name) : (char *)NULL;

  /* XXX */
#if defined (CRAY)
  memcpy((char *)&bash_input.location.string, (char *)&location.string, sizeof(location));
#else
  bash_input.location = location;
#endif
  bash_input.getter = get;
  bash_input.ungetter = unget;
}

char *
yy_input_name ()
{
  return (bash_input.name ? bash_input.name : "stdin");
}

/* Call this to get the next character of input. */
static int
yy_getc ()
{
  return (*(bash_input.getter)) ();
}

/* Call this to unget C.  That is, to make C the next character
   to be read. */
static int
yy_ungetc (c)
     int c;
{
  return (*(bash_input.ungetter)) (c);
}

#if defined (BUFFERED_INPUT)
#ifdef INCLUDE_UNUSED
int
input_file_descriptor ()
{
  switch (bash_input.type)
    {
    case st_stream:
      return (fileno (bash_input.location.file));
    case st_bstream:
      return (bash_input.location.buffered_fd);
    case st_stdin:
    default:
      return (fileno (stdin));
    }
}
#endif
#endif /* BUFFERED_INPUT */

/* **************************************************************** */
/*								    */
/*		  Let input be read from readline ().		    */
/*								    */
/* **************************************************************** */

#if defined (READLINE)
char *current_readline_prompt = (char *)NULL;
char *current_readline_line = (char *)NULL;
int current_readline_line_index = 0;

static int
yy_readline_get ()
{
  SigHandler *old_sigint;
  int line_len;
  unsigned char c;

  if (current_readline_line == 0)
    {
      if (bash_readline_initialized == 0)
	initialize_readline ();

#if defined (JOB_CONTROL)
      if (job_control)
	give_terminal_to (shell_pgrp, 0);
#endif /* JOB_CONTROL */

      old_sigint = IMPOSSIBLE_TRAP_HANDLER;
      if (signal_is_ignored (SIGINT) == 0)
	{
	  old_sigint = (SigHandler *)set_signal_handler (SIGINT, sigint_sighandler);
	}

      sh_unset_nodelay_mode (fileno (rl_instream));	/* just in case */
      current_readline_line = readline (current_readline_prompt ?
      					  current_readline_prompt : "");

      CHECK_TERMSIG;
      if (signal_is_ignored (SIGINT) == 0)
	{
	  if (old_sigint != IMPOSSIBLE_TRAP_HANDLER)
	    set_signal_handler (SIGINT, old_sigint);
	}

#if 0
      /* Reset the prompt to the decoded value of prompt_string_pointer. */
      reset_readline_prompt ();
#endif

      if (current_readline_line == 0)
	return (EOF);

      current_readline_line_index = 0;
      line_len = strlen (current_readline_line);

      current_readline_line = (char *)xrealloc (current_readline_line, 2 + line_len);
      current_readline_line[line_len++] = '\n';
      current_readline_line[line_len] = '\0';
    }

  if (current_readline_line[current_readline_line_index] == 0)
    {
      free (current_readline_line);
      current_readline_line = (char *)NULL;
      return (yy_readline_get ());
    }
  else
    {
      c = current_readline_line[current_readline_line_index++];
      return (c);
    }
}

static int
yy_readline_unget (c)
     int c;
{
  if (current_readline_line_index && current_readline_line)
    current_readline_line[--current_readline_line_index] = c;
  return (c);
}

void
with_input_from_stdin ()
{
  INPUT_STREAM location;

  if (bash_input.type != st_stdin && stream_on_stack (st_stdin) == 0)
    {
      location.string = current_readline_line;
      init_yy_io (yy_readline_get, yy_readline_unget,
		  st_stdin, "readline stdin", location);
    }
}

/* Will we be collecting another input line and printing a prompt? This uses
   different conditions than SHOULD_PROMPT(), since readline allows a user to
   embed a newline in the middle of the line it collects, which the parser
   will interpret as a line break and command delimiter. */
int
parser_will_prompt ()
{
  return (current_readline_line == 0 || current_readline_line[current_readline_line_index] == 0);
}
  
#else  /* !READLINE */

void
with_input_from_stdin ()
{
  with_input_from_stream (stdin, "stdin");
}
#endif	/* !READLINE */

/* **************************************************************** */
/*								    */
/*   Let input come from STRING.  STRING is zero terminated.	    */
/*								    */
/* **************************************************************** */

static int
yy_string_get ()
{
  register char *string;
  register unsigned char c;

  string = bash_input.location.string;

  /* If the string doesn't exist, or is empty, EOF found. */
  if (string && *string)
    {
      c = *string++;
      bash_input.location.string = string;
      return (c);
    }
  else
    return (EOF);
}

static int
yy_string_unget (c)
     int c;
{
  *(--bash_input.location.string) = c;
  return (c);
}

void
with_input_from_string (string, name)
     char *string;
     const char *name;
{
  INPUT_STREAM location;

  location.string = string;
  init_yy_io (yy_string_get, yy_string_unget, st_string, name, location);
}

/* Count the number of characters we've consumed from bash_input.location.string
   and read into shell_input_line, but have not returned from shell_getc.
   That is the true input location.  Rewind bash_input.location.string by
   that number of characters, so it points to the last character actually
   consumed by the parser. */
void
rewind_input_string ()
{
  int xchars;

  /* number of unconsumed characters in the input -- XXX need to take newlines
     into account, e.g., $(...\n) */
  xchars = shell_input_line_len - shell_input_line_index;
  if (bash_input.location.string[-1] == '\n')
    xchars++;

  /* XXX - how to reflect bash_input.location.string back to string passed to
     parse_and_execute or xparse_dolparen? xparse_dolparen needs to know how
     far into the string we parsed.  parse_and_execute knows where bash_input.
     location.string is, and how far from orig_string that is -- that's the
     number of characters the command consumed. */

  /* bash_input.location.string - xchars should be where we parsed to */
  /* need to do more validation on xchars value for sanity -- test cases. */
  bash_input.location.string -= xchars;
}

/* **************************************************************** */
/*								    */
/*		     Let input come from STREAM.		    */
/*								    */
/* **************************************************************** */

/* These two functions used to test the value of the HAVE_RESTARTABLE_SYSCALLS
   define, and just use getc/ungetc if it was defined, but since bash
   installs most of its signal handlers without the SA_RESTART flag, some
   signals received during a read(2) will not cause the read to be restarted.
   We will need to restart it ourselves. */

static int
yy_stream_get ()
{
  int result;

  result = EOF;
  if (bash_input.location.file)
    {
      /* XXX - don't need terminate_immediately; getc_with_restart checks
	 for terminating signals itself if read returns < 0 */
      result = getc_with_restart (bash_input.location.file);
    }
  return (result);
}

static int
yy_stream_unget (c)
     int c;
{
  return (ungetc_with_restart (c, bash_input.location.file));
}

void
with_input_from_stream (stream, name)
     FILE *stream;
     const char *name;
{
  INPUT_STREAM location;

  location.file = stream;
  init_yy_io (yy_stream_get, yy_stream_unget, st_stream, name, location);
}

typedef struct stream_saver {
  struct stream_saver *next;
  BASH_INPUT bash_input;
  int line;
#if defined (BUFFERED_INPUT)
  BUFFERED_STREAM *bstream;
#endif /* BUFFERED_INPUT */
} STREAM_SAVER;

/* The globally known line number. */
int line_number = 0;

/* The line number offset set by assigning to LINENO.  Not currently used. */
int line_number_base = 0;

#if defined (COND_COMMAND)
static int cond_lineno;
static int cond_token;
#endif

STREAM_SAVER *stream_list = (STREAM_SAVER *)NULL;

void
push_stream (reset_lineno)
     int reset_lineno;
{
  STREAM_SAVER *saver = (STREAM_SAVER *)xmalloc (sizeof (STREAM_SAVER));

  xbcopy ((char *)&bash_input, (char *)&(saver->bash_input), sizeof (BASH_INPUT));

#if defined (BUFFERED_INPUT)
  saver->bstream = (BUFFERED_STREAM *)NULL;
  /* If we have a buffered stream, clear out buffers[fd]. */
  if (bash_input.type == st_bstream && bash_input.location.buffered_fd >= 0)
    saver->bstream = set_buffered_stream (bash_input.location.buffered_fd,
    					  (BUFFERED_STREAM *)NULL);
#endif /* BUFFERED_INPUT */

  saver->line = line_number;
  bash_input.name = (char *)NULL;
  saver->next = stream_list;
  stream_list = saver;
  EOF_Reached = 0;
  if (reset_lineno)
    line_number = 0;
}

void
pop_stream ()
{
  if (!stream_list)
    EOF_Reached = 1;
  else
    {
      STREAM_SAVER *saver = stream_list;

      EOF_Reached = 0;
      stream_list = stream_list->next;

      init_yy_io (saver->bash_input.getter,
		  saver->bash_input.ungetter,
		  saver->bash_input.type,
		  saver->bash_input.name,
		  saver->bash_input.location);

#if defined (BUFFERED_INPUT)
      /* If we have a buffered stream, restore buffers[fd]. */
      /* If the input file descriptor was changed while this was on the
	 save stack, update the buffered fd to the new file descriptor and
	 re-establish the buffer <-> bash_input fd correspondence. */
      if (bash_input.type == st_bstream && bash_input.location.buffered_fd >= 0)
	{
	  if (bash_input_fd_changed)
	    {
	      bash_input_fd_changed = 0;
	      if (default_buffered_input >= 0)
		{
		  bash_input.location.buffered_fd = default_buffered_input;
		  saver->bstream->b_fd = default_buffered_input;
		  SET_CLOSE_ON_EXEC (default_buffered_input);
		}
	    }
	  /* XXX could free buffered stream returned as result here. */
	  set_buffered_stream (bash_input.location.buffered_fd, saver->bstream);
	}
#endif /* BUFFERED_INPUT */

      line_number = saver->line;

      FREE (saver->bash_input.name);
      free (saver);
    }
}

/* Return 1 if a stream of type TYPE is saved on the stack. */
int
stream_on_stack (type)
     enum stream_type type;
{
  register STREAM_SAVER *s;

  for (s = stream_list; s; s = s->next)
    if (s->bash_input.type == type)
      return 1;
  return 0;
}

/* Save the current token state and return it in a malloced array. */
int *
save_token_state ()
{
  int *ret;

  ret = (int *)xmalloc (4 * sizeof (int));
  ret[0] = last_read_token;
  ret[1] = token_before_that;
  ret[2] = two_tokens_ago;
  ret[3] = current_token;
  return ret;
}

void
restore_token_state (ts)
     int *ts;
{
  if (ts == 0)
    return;
  last_read_token = ts[0];
  token_before_that = ts[1];
  two_tokens_ago = ts[2];
  current_token = ts[3];
}

/*
 * This is used to inhibit alias expansion and reserved word recognition
 * inside case statement pattern lists.  A `case statement pattern list' is:
 *
 *	everything between the `in' in a `case word in' and the next ')'
 *	or `esac'
 *	everything between a `;;' and the next `)' or `esac'
 */

#define END_OF_ALIAS 0

/*
 * Pseudo-global variables used in implementing token-wise alias expansion.
 */

/*
 * Pushing and popping strings.  This works together with shell_getc to
 * implement alias expansion on a per-token basis.
 */

#define PSH_ALIAS	0x01
#define PSH_DPAREN	0x02
#define PSH_SOURCE	0x04
#define PSH_ARRAY	0x08

typedef struct string_saver {
  struct string_saver *next;
  int expand_alias;  /* Value to set expand_alias to when string is popped. */
  char *saved_line;
#if defined (ALIAS)
  alias_t *expander;   /* alias that caused this line to be pushed. */
#endif
  size_t saved_line_size, saved_line_index, saved_line_len;
  int saved_line_terminator;
  int flags;
} STRING_SAVER;

STRING_SAVER *pushed_string_list = (STRING_SAVER *)NULL;

/*
 * Push the current shell_input_line onto a stack of such lines and make S
 * the current input.  Used when expanding aliases.  EXPAND is used to set
 * the value of expand_next_token when the string is popped, so that the
 * word after the alias in the original line is handled correctly when the
 * alias expands to multiple words.  TOKEN is the token that was expanded
 * into S; it is saved and used to prevent infinite recursive expansion.
 */
static void
push_string (s, expand, ap)
     char *s;
     int expand;
     alias_t *ap;
{
  STRING_SAVER *temp = (STRING_SAVER *)xmalloc (sizeof (STRING_SAVER));

  temp->expand_alias = expand;
  temp->saved_line = shell_input_line;
  temp->saved_line_size = shell_input_line_size;
  temp->saved_line_len = shell_input_line_len;
  temp->saved_line_index = shell_input_line_index;
  temp->saved_line_terminator = shell_input_line_terminator;
  temp->flags = 0;
#if defined (ALIAS)
  temp->expander = ap;
  if (ap)
    temp->flags = PSH_ALIAS;
#endif
  temp->next = pushed_string_list;
  pushed_string_list = temp;

#if defined (ALIAS)
  if (ap)
    ap->flags |= AL_BEINGEXPANDED;
#endif

  shell_input_line = s;
  shell_input_line_size = shell_input_line_len = STRLEN (s);
  shell_input_line_index = 0;
  shell_input_line_terminator = '\0';
#if 0
  parser_state &= ~PST_ALEXPNEXT;	/* XXX */
#endif

  set_line_mbstate ();
}

/*
 * Make the top of the pushed_string stack be the current shell input.
 * Only called when there is something on the stack.  Called from shell_getc
 * when it thinks it has consumed the string generated by an alias expansion
 * and needs to return to the original input line.
 */
static void
pop_string ()
{
  STRING_SAVER *t;

  FREE (shell_input_line);
  shell_input_line = pushed_string_list->saved_line;
  shell_input_line_index = pushed_string_list->saved_line_index;
  shell_input_line_size = pushed_string_list->saved_line_size;
  shell_input_line_len = pushed_string_list->saved_line_len;
  shell_input_line_terminator = pushed_string_list->saved_line_terminator;

#if defined (ALIAS)
  if (pushed_string_list->expand_alias)
    parser_state |= PST_ALEXPNEXT;
  else
    parser_state &= ~PST_ALEXPNEXT;
#endif

  t = pushed_string_list;
  pushed_string_list = pushed_string_list->next;

#if defined (ALIAS)
  if (t->expander)
    t->expander->flags &= ~AL_BEINGEXPANDED;
#endif

  free ((char *)t);

  set_line_mbstate ();
}

static void
free_string_list ()
{
  register STRING_SAVER *t, *t1;

  for (t = pushed_string_list; t; )
    {
      t1 = t->next;
      FREE (t->saved_line);
#if defined (ALIAS)
      if (t->expander)
	t->expander->flags &= ~AL_BEINGEXPANDED;
#endif
      free ((char *)t);
      t = t1;
    }
  pushed_string_list = (STRING_SAVER *)NULL;
}

void
free_pushed_string_input ()
{
#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  free_string_list ();
#endif
}

int
parser_expanding_alias ()
{
  return (expanding_alias ());
}

void
parser_save_alias ()
{
#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  push_string ((char *)NULL, 0, (alias_t *)NULL);
  pushed_string_list->flags = PSH_SOURCE;	/* XXX - for now */
#else
  ;
#endif
}

void
parser_restore_alias ()
{
#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  if (pushed_string_list)
    pop_string ();
#else
  ;
#endif
}

#if defined (ALIAS)
/* Before freeing AP, make sure that there aren't any cases of pointer
   aliasing that could cause us to reference freed memory later on. */
void
clear_string_list_expander (ap)
     alias_t *ap;
{
  register STRING_SAVER *t;

  for (t = pushed_string_list; t; t = t->next)
    {
      if (t->expander && t->expander == ap)
	t->expander = 0;
    }
}
#endif

void
clear_shell_input_line ()
{
  if (shell_input_line)
    shell_input_line[shell_input_line_index = 0] = '\0';
}

/* Return a line of text, taken from wherever yylex () reads input.
   If there is no more input, then we return NULL.  If REMOVE_QUOTED_NEWLINE
   is non-zero, we remove unquoted \<newline> pairs.  This is used by
   read_secondary_line to read here documents. */
static char *
read_a_line (remove_quoted_newline)
     int remove_quoted_newline;
{
  static char *line_buffer = (char *)NULL;
  static int buffer_size = 0;
  int indx, c, peekc, pass_next;

#if defined (READLINE)
  if (no_line_editing && SHOULD_PROMPT ())
#else
  if (SHOULD_PROMPT ())
#endif
    print_prompt ();

  pass_next = indx = 0;
  while (1)
    {
      /* Allow immediate exit if interrupted during input. */
      QUIT;

      c = yy_getc ();

      /* Ignore null bytes in input. */
      if (c == 0)
	continue;

      /* If there is no more input, then we return NULL. */
      if (c == EOF)
	{
	  if (interactive && bash_input.type == st_stream)
	    clearerr (stdin);
	  if (indx == 0)
	    return ((char *)NULL);
	  c = '\n';
	}

      /* `+2' in case the final character in the buffer is a newline or we
	 have to handle CTLESC or CTLNUL. */
      RESIZE_MALLOCED_BUFFER (line_buffer, indx, 2, buffer_size, 128);

      /* IF REMOVE_QUOTED_NEWLINES is non-zero, we are reading a
	 here document with an unquoted delimiter.  In this case,
	 the line will be expanded as if it were in double quotes.
	 We allow a backslash to escape the next character, but we
	 need to treat the backslash specially only if a backslash
	 quoting a backslash-newline pair appears in the line. */
      if (pass_next)
	{
	  line_buffer[indx++] = c;
	  pass_next = 0;
	}
      else if (c == '\\' && remove_quoted_newline)
	{
	  QUIT;
	  peekc = yy_getc ();
	  if (peekc == '\n')
	    {
	      line_number++;
	      continue;	/* Make the unquoted \<newline> pair disappear. */
	    }
	  else
	    {
	      yy_ungetc (peekc);
	      pass_next = 1;
	      line_buffer[indx++] = c;		/* Preserve the backslash. */
	    }
	}
      else
	{
	  /* remove_quoted_newline is non-zero if the here-document delimiter
	     is unquoted. In this case, we will be expanding the lines and
	     need to make sure CTLESC and CTLNUL in the input are quoted. */
	  if (remove_quoted_newline && (c == CTLESC || c == CTLNUL))
	    line_buffer[indx++] = CTLESC;
	  line_buffer[indx++] = c;
	}

      if (c == '\n')
	{
	  line_buffer[indx] = '\0';
	  return (line_buffer);
	}
    }
}

/* Return a line as in read_a_line (), but insure that the prompt is
   the secondary prompt.  This is used to read the lines of a here
   document.  REMOVE_QUOTED_NEWLINE is non-zero if we should remove
   newlines quoted with backslashes while reading the line.  It is
   non-zero unless the delimiter of the here document was quoted. */
char *
read_secondary_line (remove_quoted_newline)
     int remove_quoted_newline;
{
  char *ret;
  int n, c;

  prompt_string_pointer = &ps2_prompt;
  if (SHOULD_PROMPT ())
    prompt_again (0);
  ret = read_a_line (remove_quoted_newline);
#if defined (HISTORY)
  if (ret && remember_on_history && (parser_state & PST_HEREDOC))
    {
      /* To make adding the here-document body right, we need to rely on
	 history_delimiting_chars() returning \n for the first line of the
	 here-document body and the null string for the second and subsequent
	 lines, so we avoid double newlines.
	 current_command_line_count == 2 for the first line of the body. */

      current_command_line_count++;
      maybe_add_history (ret);
    }
#endif /* HISTORY */
  return ret;
}

/* **************************************************************** */
/*								    */
/*				YYLEX ()			    */
/*								    */
/* **************************************************************** */

/* Reserved words.  These are only recognized as the first word of a
   command. */
STRING_INT_ALIST word_token_alist[] = {
  { "if", IF },
  { "then", THEN },
  { "else", ELSE },
  { "elif", ELIF },
  { "fi", FI },
  { "case", CASE },
  { "esac", ESAC },
  { "for", FOR },
#if defined (SELECT_COMMAND)
  { "select", SELECT },
#endif
  { "while", WHILE },
  { "until", UNTIL },
  { "do", DO },
  { "done", DONE },
  { "in", IN },
  { "function", FUNCTION },
#if defined (COMMAND_TIMING)
  { "time", TIME },
#endif
  { "{", '{' },
  { "}", '}' },
  { "!", BANG },
#if defined (COND_COMMAND)
  { "[[", COND_START },
  { "]]", COND_END },
#endif
#if defined (COPROCESS_SUPPORT)
  { "coproc", COPROC },
#endif
  { (char *)NULL, 0}
};

/* other tokens that can be returned by read_token() */
STRING_INT_ALIST other_token_alist[] = {
  /* Multiple-character tokens with special values */
  { "--", TIMEIGN },
  { "-p", TIMEOPT },
  { "&&", AND_AND },
  { "||", OR_OR },
  { ">>", GREATER_GREATER },
  { "<<", LESS_LESS },
  { "<&", LESS_AND },
  { ">&", GREATER_AND },
  { ";;", SEMI_SEMI },
  { ";&", SEMI_AND },
  { ";;&", SEMI_SEMI_AND },
  { "<<-", LESS_LESS_MINUS },
  { "<<<", LESS_LESS_LESS },
  { "&>", AND_GREATER },
  { "&>>", AND_GREATER_GREATER },
  { "<>", LESS_GREATER },
  { ">|", GREATER_BAR },
  { "|&", BAR_AND },
  { "EOF", yacc_EOF },
  /* Tokens whose value is the character itself */
  { ">", '>' },
  { "<", '<' },
  { "-", '-' },
  { "{", '{' },
  { "}", '}' },
  { ";", ';' },
  { "(", '(' },
  { ")", ')' },
  { "|", '|' },
  { "&", '&' },
  { "newline", '\n' },
  { (char *)NULL, 0}
};

/* others not listed here:
	WORD			look at yylval.word
	ASSIGNMENT_WORD		look at yylval.word
	NUMBER			look at yylval.number
	ARITH_CMD		look at yylval.word_list
	ARITH_FOR_EXPRS		look at yylval.word_list
	COND_CMD		look at yylval.command
*/

/* These are used by read_token_word, but appear up here so that shell_getc
   can use them to decide when to add otherwise blank lines to the history. */

/* The primary delimiter stack. */
struct dstack dstack = {  (char *)NULL, 0, 0 };

/* A temporary delimiter stack to be used when decoding prompt strings.
   This is needed because command substitutions in prompt strings (e.g., PS2)
   can screw up the parser's quoting state. */
static struct dstack temp_dstack = { (char *)NULL, 0, 0 };

/* Macro for accessing the top delimiter on the stack.  Returns the
   delimiter or zero if none. */
#define current_delimiter(ds) \
  (ds.delimiter_depth ? ds.delimiters[ds.delimiter_depth - 1] : 0)

#define push_delimiter(ds, character) \
  do \
    { \
      if (ds.delimiter_depth + 2 > ds.delimiter_space) \
	ds.delimiters = (char *)xrealloc \
	  (ds.delimiters, (ds.delimiter_space += 10) * sizeof (char)); \
      ds.delimiters[ds.delimiter_depth] = character; \
      ds.delimiter_depth++; \
    } \
  while (0)

#define pop_delimiter(ds)	ds.delimiter_depth--

/* Return the next shell input character.  This always reads characters
   from shell_input_line; when that line is exhausted, it is time to
   read the next line.  This is called by read_token when the shell is
   processing normal command input. */

/* This implements one-character lookahead/lookbehind across physical input
   lines, to avoid something being lost because it's pushed back with
   shell_ungetc when we're at the start of a line. */
static int eol_ungetc_lookahead = 0;

static int unquoted_backslash = 0;

static int
shell_getc (remove_quoted_newline)
     int remove_quoted_newline;
{
  register int i;
  int c, truncating, last_was_backslash;
  unsigned char uc;

  QUIT;

  last_was_backslash = 0;
  if (sigwinch_received)
    {
      sigwinch_received = 0;
      get_new_window_size (0, (int *)0, (int *)0);
    }
      
  if (eol_ungetc_lookahead)
    {
      c = eol_ungetc_lookahead;
      eol_ungetc_lookahead = 0;
      return (c);
    }

#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  /* If shell_input_line[shell_input_line_index] == 0, but there is
     something on the pushed list of strings, then we don't want to go
     off and get another line.  We let the code down below handle it. */

  if (!shell_input_line || ((!shell_input_line[shell_input_line_index]) &&
			    (pushed_string_list == (STRING_SAVER *)NULL)))
#else /* !ALIAS && !DPAREN_ARITHMETIC */
  if (!shell_input_line || !shell_input_line[shell_input_line_index])
#endif /* !ALIAS && !DPAREN_ARITHMETIC */
    {
      line_number++;

      /* Let's not let one really really long line blow up memory allocation */
      if (shell_input_line && shell_input_line_size >= 32768)
	{
	  free (shell_input_line);
	  shell_input_line = 0;
	  shell_input_line_size = 0;
	}

    restart_read:

      /* Allow immediate exit if interrupted during input. */
      QUIT;

      i = truncating = 0;
      shell_input_line_terminator = 0;

      /* If the shell is interactive, but not currently printing a prompt
         (interactive_shell && interactive == 0), we don't want to print
         notifies or cleanup the jobs -- we want to defer it until we do
         print the next prompt. */
      if (interactive_shell == 0 || SHOULD_PROMPT())
	{
#if defined (JOB_CONTROL)
      /* This can cause a problem when reading a command as the result
	 of a trap, when the trap is called from flush_child.  This call
	 had better not cause jobs to disappear from the job table in
	 that case, or we will have big trouble. */
	  notify_and_cleanup ();
#else /* !JOB_CONTROL */
	  cleanup_dead_jobs ();
#endif /* !JOB_CONTROL */
	}

#if defined (READLINE)
      if (no_line_editing && SHOULD_PROMPT())
#else
      if (SHOULD_PROMPT())
#endif
	print_prompt ();

      if (bash_input.type == st_stream)
	clearerr (stdin);

      while (1)
	{
	  c = yy_getc ();

	  /* Allow immediate exit if interrupted during input. */
	  QUIT;

	  if (c == '\0')
	    {
	      /* If we get EOS while parsing a string, treat it as EOF so we
		 don't just keep looping. Happens very rarely */
	      if (bash_input.type == st_string)
		{
		  if (i == 0)
		    shell_input_line_terminator = EOF;
		  shell_input_line[i] = '\0';
		  c = EOF;
		  break;
		}
	      continue;
	    }

	  /* Theoretical overflow */
	  /* If we can't put 256 bytes more into the buffer, allocate
	     everything we can and fill it as full as we can. */
	  /* XXX - we ignore rest of line using `truncating' flag */
	  if (shell_input_line_size > (SIZE_MAX - 256))
	    {
	      size_t n;

	      n = SIZE_MAX - i;	/* how much more can we put into the buffer? */
	      if (n <= 2)	/* we have to save 1 for the newline added below */
		{
		  if (truncating == 0)
		    internal_warning(_("shell_getc: shell_input_line_size (%zu) exceeds SIZE_MAX (%lu): line truncated"), shell_input_line_size, (unsigned long)SIZE_MAX);
		  shell_input_line[i] = '\0';
		  truncating = 1;
		}
	      if (shell_input_line_size < SIZE_MAX)
		{
		  shell_input_line_size = SIZE_MAX;
		  shell_input_line = xrealloc (shell_input_line, shell_input_line_size);
		}
	    }
	  else
	    RESIZE_MALLOCED_BUFFER (shell_input_line, i, 2, shell_input_line_size, 256);

	  if (c == EOF)
	    {
	      if (bash_input.type == st_stream)
		clearerr (stdin);

	      if (i == 0)
		shell_input_line_terminator = EOF;

	      shell_input_line[i] = '\0';
	      break;
	    }

	  if (truncating == 0 || c == '\n')
	    shell_input_line[i++] = c;

	  if (c == '\n')
	    {
	      shell_input_line[--i] = '\0';
	      current_command_line_count++;
	      break;
	    }

	  last_was_backslash = last_was_backslash == 0 && c == '\\';
	}

      shell_input_line_index = 0;
      shell_input_line_len = i;		/* == strlen (shell_input_line) */

      set_line_mbstate ();

#if defined (HISTORY)
      if (remember_on_history && shell_input_line && shell_input_line[0])
	{
	  char *expansions;
#  if defined (BANG_HISTORY)
	  /* If the current delimiter is a single quote, we should not be
	     performing history expansion, even if we're on a different
	     line from the original single quote. */
	  if (current_delimiter (dstack) == '\'')
	    history_quoting_state = '\'';
	  else if (current_delimiter (dstack) == '"')
	    history_quoting_state = '"';
	  else
	    history_quoting_state = 0;
#  endif
	  /* Calling with a third argument of 1 allows remember_on_history to
	     determine whether or not the line is saved to the history list */
	  expansions = pre_process_line (shell_input_line, 1, 1);
#  if defined (BANG_HISTORY)
	  history_quoting_state = 0;
#  endif
	  if (expansions != shell_input_line)
	    {
	      free (shell_input_line);
	      shell_input_line = expansions;
	      shell_input_line_len = shell_input_line ?
					strlen (shell_input_line) : 0;
	      if (shell_input_line_len == 0)
		current_command_line_count--;

	      /* We have to force the xrealloc below because we don't know
		 the true allocated size of shell_input_line anymore. */
	      shell_input_line_size = shell_input_line_len;

	      set_line_mbstate ();
	    }
	}
      /* Try to do something intelligent with blank lines encountered while
	 entering multi-line commands.  XXX - this is grotesque */
      else if (remember_on_history && shell_input_line &&
	       shell_input_line[0] == '\0' &&
	       current_command_line_count > 1)
	{
	  if (current_delimiter (dstack))
	    /* We know shell_input_line[0] == 0 and we're reading some sort of
	       quoted string.  This means we've got a line consisting of only
	       a newline in a quoted string.  We want to make sure this line
	       gets added to the history. */
	    maybe_add_history (shell_input_line);
	  else
	    {
	      char *hdcs;
	      hdcs = history_delimiting_chars (shell_input_line);
	      if (hdcs && hdcs[0] == ';')
		maybe_add_history (shell_input_line);
	    }
	}

#endif /* HISTORY */

      if (shell_input_line)
	{
	  /* Lines that signify the end of the shell's input should not be
	     echoed.  We should not echo lines while parsing command
	     substitutions with recursive calls into the parsing engine; those
	     should only be echoed once when we read the word.  That is the
	     reason for the test against shell_eof_token, which is set to a
	     right paren when parsing the contents of command substitutions. */
	  if (echo_input_at_read && (shell_input_line[0] ||
				       shell_input_line_terminator != EOF) &&
				     shell_eof_token == 0)
	    fprintf (stderr, "%s\n", shell_input_line);
	}
      else
	{
	  shell_input_line_size = 0;
	  prompt_string_pointer = &current_prompt_string;
	  if (SHOULD_PROMPT ())
	    prompt_again (0);
	  goto restart_read;
	}

      /* Add the newline to the end of this string, iff the string does
	 not already end in an EOF character.  */
      if (shell_input_line_terminator != EOF)
	{
	  if (shell_input_line_size < SIZE_MAX-3 && (shell_input_line_len+3 > shell_input_line_size))
	    shell_input_line = (char *)xrealloc (shell_input_line,
					1 + (shell_input_line_size += 2));

	  /* Don't add a newline to a string that ends with a backslash if we're
	     going to be removing quoted newlines, since that will eat the
	     backslash.  Add another backslash instead (will be removed by
	     word expansion). */
	  if (bash_input.type == st_string && expanding_alias() == 0 && last_was_backslash && c == EOF && remove_quoted_newline)
	    shell_input_line[shell_input_line_len] = '\\';
	  else
	    shell_input_line[shell_input_line_len] = '\n';
	  shell_input_line[shell_input_line_len + 1] = '\0';

#if defined (HANDLE_MULTIBYTE)
	  /* This is kind of an abstraction violation, but there's no need to
	     go through the entire shell_input_line again with a call to
	     set_line_mbstate(). */
	  EXTEND_SHELL_INPUT_LINE_PROPERTY();
	  shell_input_line_property[shell_input_line_len] = 1;
#endif
	}
    }

next_alias_char:
  if (shell_input_line_index == 0)
    unquoted_backslash = 0;

  uc = shell_input_line[shell_input_line_index];

  if (uc)
    {
      unquoted_backslash = unquoted_backslash == 0 && uc == '\\';
      shell_input_line_index++;
    }

#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  /* If UC is NULL, we have reached the end of the current input string.  If
     pushed_string_list is non-empty, it's time to pop to the previous string
     because we have fully consumed the result of the last alias expansion.
     Do it transparently; just return the next character of the string popped
     to. */
  /* If pushed_string_list != 0 but pushed_string_list->expander == 0 (not
     currently tested) and the flags value is not PSH_SOURCE, we are not
     parsing an alias, we have just saved one (push_string, when called by
     the parse_dparen code) In this case, just go on as well.  The PSH_SOURCE
     case is handled below. */

  /* If we're at the end of an alias expansion add a space to make sure that
     the alias remains marked as being in use while we expand its last word.
     This makes sure that pop_string doesn't mark the alias as not in use
     before the string resulting from the alias expansion is tokenized and
     checked for alias expansion, preventing recursion.  At this point, the
     last character in shell_input_line is the last character of the alias
     expansion.  We test that last character to determine whether or not to
     return the space that will delimit the token and postpone the pop_string.
     This set of conditions duplicates what used to be in mk_alexpansion ()
     below, with the addition that we don't add a space if we're currently
     reading a quoted string or in a shell comment. */
#ifndef OLD_ALIAS_HACK
  if (uc == 0 && pushed_string_list && pushed_string_list->flags != PSH_SOURCE &&
      pushed_string_list->flags != PSH_DPAREN &&
      (parser_state & PST_COMMENT) == 0 &&
      (parser_state & PST_ENDALIAS) == 0 &&	/* only once */
      shell_input_line_index > 0 &&
      shellblank (shell_input_line[shell_input_line_index-1]) == 0 &&
      shell_input_line[shell_input_line_index-1] != '\n' &&
      unquoted_backslash == 0 &&
      shellmeta (shell_input_line[shell_input_line_index-1]) == 0 &&
      (current_delimiter (dstack) != '\'' && current_delimiter (dstack) != '"'))
    {
      parser_state |= PST_ENDALIAS;
      /* We need to do this to make sure last_shell_getc_is_singlebyte returns
	 true, since we are returning a single-byte space. */
      if (shell_input_line_index == shell_input_line_len && last_shell_getc_is_singlebyte == 0)
	{
#if 0
	  EXTEND_SHELL_INPUT_LINE_PROPERTY();
	  shell_input_line_property[shell_input_line_len++] = 1;
	  /* extend shell_input_line to accommodate the shell_ungetc that
	     read_token_word() will perform, since we're extending the index */
	  RESIZE_MALLOCED_BUFFER (shell_input_line, shell_input_line_index, 2, shell_input_line_size, 16);
          shell_input_line[++shell_input_line_index] = '\0';	/* XXX */
#else
	  shell_input_line_property[shell_input_line_index - 1] = 1;
#endif
	}
      return ' ';	/* END_ALIAS */
    }
#endif

pop_alias:
#endif /* ALIAS || DPAREN_ARITHMETIC */
  /* This case works for PSH_DPAREN as well as the shell_ungets() case that uses
     push_string */
  if (uc == 0 && pushed_string_list && pushed_string_list->flags != PSH_SOURCE)
    {
      parser_state &= ~PST_ENDALIAS;
      pop_string ();
      uc = shell_input_line[shell_input_line_index];
      if (uc)
	shell_input_line_index++;
    }

  if MBTEST(uc == '\\' && remove_quoted_newline && shell_input_line[shell_input_line_index] == '\n')
    {
	if (SHOULD_PROMPT ())
	  prompt_again (0);
	line_number++;

	/* What do we do here if we're expanding an alias whose definition
	   includes an escaped newline?  If that's the last character in the
	   alias expansion, we just pop the pushed string list (recall that
	   we inhibit the appending of a space if newline is the last
	   character).  If it's not the last character, we need to consume the
	   quoted newline and move to the next character in the expansion. */
#if defined (ALIAS)
	if (expanding_alias () && shell_input_line[shell_input_line_index+1] == '\0')
	  {
	    uc = 0;
	    goto pop_alias;
	  }
	else if (expanding_alias () && shell_input_line[shell_input_line_index+1] != '\0')
	  {
	    shell_input_line_index++;	/* skip newline */
	    goto next_alias_char;	/* and get next character */
	  }
	else
#endif 
	  goto restart_read;
    }

  if (uc == 0 && shell_input_line_terminator == EOF)
    return ((shell_input_line_index != 0) ? '\n' : EOF);

#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  /* We already know that we are not parsing an alias expansion because of the
     check for expanding_alias() above.  This knows how parse_and_execute
     handles switching to st_string input while an alias is being expanded,
     hence the check for pushed_string_list without pushed_string_list->expander
     and the check for PSH_SOURCE as pushed_string_list->flags.
     parse_and_execute and parse_string both change the input type to st_string
     and place the string to be parsed and executed into location.string, so
     we should not stop reading that until the pointer is '\0'.
     The check for shell_input_line_terminator may be superfluous.

     This solves the problem of `.' inside a multi-line alias with embedded
     newlines executing things out of order. */
  if (uc == 0 && bash_input.type == st_string && *bash_input.location.string &&
      pushed_string_list && pushed_string_list->flags == PSH_SOURCE &&
      shell_input_line_terminator == 0)
    {
      shell_input_line_index = 0;
      goto restart_read;
    }
#endif

  return (uc);
}

/* Put C back into the input for the shell.  This might need changes for
   HANDLE_MULTIBYTE around EOLs.  Since we (currently) never push back a
   character different than we read, shell_input_line_property doesn't need
   to change when manipulating shell_input_line.  The define for
   last_shell_getc_is_singlebyte should take care of it, though. */
static void
shell_ungetc (c)
     int c;
{
  if (shell_input_line && shell_input_line_index)
    shell_input_line[--shell_input_line_index] = c;
  else
    eol_ungetc_lookahead = c;
}

/* Push S back into shell_input_line; updating shell_input_line_index */
void
shell_ungets (s)
     char *s;
{
  size_t slen, chars_left;

  slen = strlen (s);

  if (shell_input_line[shell_input_line_index] == '\0')
    {
      /* Easy, just overwrite shell_input_line. This is preferred because it
	 saves on set_line_mbstate () and other overhead like push_string */
      if (shell_input_line_size <= slen)
	RESIZE_MALLOCED_BUFFER (shell_input_line, shell_input_line_index, slen + 1, shell_input_line_size, 64);
      strcpy (shell_input_line, s);
      shell_input_line_index = 0;
      shell_input_line_len = slen;
      shell_input_line_terminator = 0;
    }
  else if (shell_input_line_index >= slen)
    {
      /* Just as easy, just back up shell_input_line_index, but it means we
	 will re-process some characters in set_line_mbstate(). Need to
	 watch pushing back newlines here. */
      while (slen > 0)
        shell_input_line[--shell_input_line_index] = s[--slen];
    }
  else if (s[slen - 1] == '\n')
    {
      push_string (savestring (s), 0, (alias_t *)NULL);
      /* push_string does set_line_mbstate () */
      return;
    }
  else
    {
      /* Harder case: pushing back input string that's longer than what we've
	 consumed from shell_input_line so far. */
      INTERNAL_DEBUG (("shell_ungets: not at end of shell_input_line"));

      chars_left = shell_input_line_len - shell_input_line_index;
      if (shell_input_line_size <= (slen + chars_left))
	RESIZE_MALLOCED_BUFFER (shell_input_line, shell_input_line_index, chars_left + slen + 1, shell_input_line_size, 64);
      memmove (shell_input_line + slen, shell_input_line + shell_input_line_index, shell_input_line_len - shell_input_line_index);
      strcpy (shell_input_line, s);
      shell_input_line_index = 0;
      shell_input_line_len = strlen (shell_input_line);	/* chars_left + slen? */
    }

#if defined (HANDLE_MULTIBYTE)
  set_line_mbstate ();	/* XXX */
#endif
}

char *
parser_remaining_input ()
{
  if (shell_input_line == 0)
    return 0;
  if ((int)shell_input_line_index < 0 || shell_input_line_index >= shell_input_line_len)
    return "";	/* XXX */
  return (shell_input_line + shell_input_line_index);
}

#ifdef INCLUDE_UNUSED
/* Back the input pointer up by one, effectively `ungetting' a character. */
static void
shell_ungetchar ()
{
  if (shell_input_line && shell_input_line_index)
    shell_input_line_index--;
}
#endif

/* Discard input until CHARACTER is seen, then push that character back
   onto the input stream. */
static void
discard_until (character)
     int character;
{
  int c;

  while ((c = shell_getc (0)) != EOF && c != character)
    ;

  if (c != EOF)
    shell_ungetc (c);
}

void
execute_variable_command (command, vname)
     char *command, *vname;
{
  char *last_lastarg;
  sh_parser_state_t ps;

  save_parser_state (&ps);
  last_lastarg = get_string_value ("_");
  if (last_lastarg)
    last_lastarg = savestring (last_lastarg);

  parse_and_execute (savestring (command), vname, SEVAL_NONINT|SEVAL_NOHIST);

  restore_parser_state (&ps);
  bind_variable ("_", last_lastarg, 0);
  FREE (last_lastarg);

  if (token_to_read == '\n')	/* reset_parser was called */
    token_to_read = 0;
}

void
push_token (x)
     int x;
{
  two_tokens_ago = token_before_that;
  token_before_that = last_read_token;
  last_read_token = current_token;

  current_token = x;
}

/* Place to remember the token.  We try to keep the buffer
   at a reasonable size, but it can grow. */
static char *token = (char *)NULL;

/* Current size of the token buffer. */
static size_t token_buffer_size;

/* Command to read_token () explaining what we want it to do. */
#define READ 0
#define RESET 1
#define prompt_is_ps1 \
      (!prompt_string_pointer || prompt_string_pointer == &ps1_prompt)

/* Function for yyparse to call.  yylex keeps track of
   the last two tokens read, and calls read_token.  */
static int
yylex ()
{
  if (interactive && (current_token == 0 || current_token == '\n'))
    {
      /* Before we print a prompt, we might have to check mailboxes.
	 We do this only if it is time to do so. Notice that only here
	 is the mail alarm reset; nothing takes place in check_mail ()
	 except the checking of mail.  Please don't change this. */
      if (prompt_is_ps1 && parse_and_execute_level == 0 && time_to_check_mail ())
	{
	  check_mail ();
	  reset_mail_timer ();
	}

      /* Avoid printing a prompt if we're not going to read anything, e.g.
	 after resetting the parser with read_token (RESET). */
      if (token_to_read == 0 && SHOULD_PROMPT ())
	prompt_again (0);
    }

  two_tokens_ago = token_before_that;
  token_before_that = last_read_token;
  last_read_token = current_token;
  current_token = read_token (READ);

  if ((parser_state & PST_EOFTOKEN) && current_token == shell_eof_token)
    {
      /* placeholder for any special handling. */
      return (current_token);
    }

  if (current_token < 0)
#if defined (YYERRCODE) && !defined (YYUNDEF)
    current_token = YYERRCODE;
#else
    current_token = YYerror;
#endif

  return (current_token);
}

/* When non-zero, we have read the required tokens
   which allow ESAC to be the next one read. */
static int esacs_needed_count;

/* When non-zero, we can read IN as an acceptable token, regardless of how
   many newlines we read. */
static int expecting_in_token;

static void
push_heredoc (r)
     REDIRECT *r;
{
  if (need_here_doc >= HEREDOC_MAX)
    {
      last_command_exit_value = EX_BADUSAGE;
      need_here_doc = 0;
      report_syntax_error (_("maximum here-document count exceeded"));
      reset_parser ();
      exit_shell (last_command_exit_value);
    }
  redir_stack[need_here_doc++] = r;
}

void
gather_here_documents ()
{
  int r;

  r = 0;
  here_doc_first_line = 1;
  while (need_here_doc > 0)
    {
      parser_state |= PST_HEREDOC;
      make_here_document (redir_stack[r++], line_number);
      parser_state &= ~PST_HEREDOC;
      need_here_doc--;
      redir_stack[r - 1] = 0;		/* XXX */
    }
  here_doc_first_line = 0;		/* just in case */
}

/* When non-zero, an open-brace used to create a group is awaiting a close
   brace partner. */
static int open_brace_count;

/* In the following three macros, `token' is always last_read_token */

/* Are we in the middle of parsing a redirection where we are about to read
   a word?  This is used to make sure alias expansion doesn't happen in the
   middle of a redirection, even though we're parsing a simple command. */
#define parsing_redirection(token) \
  (token == '<' || token == '>' || \
   token == GREATER_GREATER || token == GREATER_BAR || \
   token == LESS_GREATER || token == LESS_LESS_MINUS || \
   token == LESS_LESS || token == LESS_LESS_LESS || \
   token == LESS_AND || token == GREATER_AND || token == AND_GREATER)

/* Is `token' one that will allow a WORD to be read in a command position?
   We can read a simple command name on which we should attempt alias expansion
   or we can read an assignment statement. */
#define command_token_position(token) \
  (((token) == ASSIGNMENT_WORD) || \
   ((parser_state&PST_REDIRLIST) && parsing_redirection(token) == 0) || \
   ((token) != SEMI_SEMI && (token) != SEMI_AND && (token) != SEMI_SEMI_AND && reserved_word_acceptable(token)))

/* Are we in a position where we can read an assignment statement? */
#define assignment_acceptable(token) \
  (command_token_position(token) && ((parser_state & PST_CASEPAT) == 0))

/* Check to see if TOKEN is a reserved word and return the token
   value if it is. */
#define CHECK_FOR_RESERVED_WORD(tok) \
  do { \
    if (!dollar_present && !quoted && \
	reserved_word_acceptable (last_read_token)) \
      { \
	int i; \
	for (i = 0; word_token_alist[i].word != (char *)NULL; i++) \
	  if (STREQ (tok, word_token_alist[i].word)) \
	    { \
	      if ((parser_state & PST_CASEPAT) && (word_token_alist[i].token != ESAC)) \
		break; \
	      if (word_token_alist[i].token == TIME && time_command_acceptable () == 0) \
		break; \
	      if ((parser_state & PST_CASEPAT) && last_read_token == '|' && word_token_alist[i].token == ESAC) \
		break; /* Posix grammar rule 4 */ \
	      if ((parser_state & PST_CASEPAT) && last_read_token == '(' && word_token_alist[i].token == ESAC) /*)*/ \
		break; /* phantom Posix grammar rule 4 */ \
	      if (word_token_alist[i].token == ESAC) { \
		parser_state &= ~(PST_CASEPAT|PST_CASESTMT); \
		esacs_needed_count--; \
	      } else if (word_token_alist[i].token == CASE) \
		parser_state |= PST_CASESTMT; \
	      else if (word_token_alist[i].token == COND_END) \
		parser_state &= ~(PST_CONDCMD|PST_CONDEXPR); \
	      else if (word_token_alist[i].token == COND_START) \
		parser_state |= PST_CONDCMD; \
	      else if (word_token_alist[i].token == '{') \
		open_brace_count++; \
	      else if (word_token_alist[i].token == '}' && open_brace_count) \
		open_brace_count--; \
	      return (word_token_alist[i].token); \
	    } \
      } \
  } while (0)

#if defined (ALIAS)

    /* OK, we have a token.  Let's try to alias expand it, if (and only if)
       it's eligible.

       It is eligible for expansion if EXPAND_ALIASES is set, and
       the token is unquoted and the last token read was a command
       separator (or expand_next_token is set), and we are currently
       processing an alias (pushed_string_list is non-empty) and this
       token is not the same as the current or any previously
       processed alias.

       Special cases that disqualify:
	 In a pattern list in a case statement (parser_state & PST_CASEPAT). */

static char *
mk_alexpansion (s)
     char *s;
{
  int l;
  char *r;

  l = strlen (s);
  r = xmalloc (l + 2);
  strcpy (r, s);
#ifdef OLD_ALIAS_HACK
  /* If the last character in the alias is a newline, don't add a trailing
     space to the expansion.  Works with shell_getc above. */
  /* Need to do something about the case where the alias expansion contains
     an unmatched quoted string, since appending this space affects the
     subsequent output. */
  if (l > 0 && r[l - 1] != ' ' && r[l - 1] != '\n' && shellmeta(r[l - 1]) == 0)
    r[l++] = ' ';
#endif
  r[l] = '\0';
  return r;
}

static int
alias_expand_token (tokstr)
     char *tokstr;
{
  char *expanded;
  alias_t *ap;

#if 0
  if (((parser_state & PST_ALEXPNEXT) || command_token_position (last_read_token)) &&
	(parser_state & PST_CASEPAT) == 0)
#else
  if ((parser_state & PST_ALEXPNEXT) || assignment_acceptable (last_read_token))
#endif
    {
      ap = find_alias (tokstr);

      /* Currently expanding this token. */
      if (ap && (ap->flags & AL_BEINGEXPANDED))
	return (NO_EXPANSION);

#ifdef OLD_ALIAS_HACK
      /* mk_alexpansion puts an extra space on the end of the alias expansion,
	 so the lookahead by the parser works right (the alias needs to remain
	 `in use' while parsing its last word to avoid alias recursion for
	 something like "alias echo=echo").  If this gets changed, make sure
	 the code in shell_getc that deals with reaching the end of an
	 expanded alias is changed with it. */
#endif
      expanded = ap ? mk_alexpansion (ap->value) : (char *)NULL;

      if (expanded)
	{
	  push_string (expanded, ap->flags & AL_EXPANDNEXT, ap);
	  return (RE_READ_TOKEN);
	}
      else
	/* This is an eligible token that does not have an expansion. */
	return (NO_EXPANSION);
    }
  return (NO_EXPANSION);
}
#endif /* ALIAS */

static int
time_command_acceptable ()
{
#if defined (COMMAND_TIMING)
  int i;

  if (posixly_correct && shell_compatibility_level > 41)
    {
      /* Quick check of the rest of the line to find the next token.  If it
	 begins with a `-', Posix says to not return `time' as the token.
	 This was interp 267. */
      i = shell_input_line_index;
      while (i < shell_input_line_len && (shell_input_line[i] == ' ' || shell_input_line[i] == '\t'))
        i++;
      if (shell_input_line[i] == '-')
	return 0;
    }

  switch (last_read_token)
    {
    case 0:
    case ';':
    case '\n':
      if (token_before_that == '|')
	return (0);
      /* FALLTHROUGH */
    case AND_AND:
    case OR_OR:
    case '&':
    case WHILE:
    case DO:
    case UNTIL:
    case IF:
    case THEN:
    case ELIF:
    case ELSE:
    case '{':		/* } */
    case '(':		/* )( */
    case ')':		/* only valid in case statement */
    case BANG:		/* ! time pipeline */
    case TIME:		/* time time pipeline */
    case TIMEOPT:	/* time -p time pipeline */
    case TIMEIGN:	/* time -p -- ... */
      return 1;
    default:
      return 0;
    }
#else
  return 0;
#endif /* COMMAND_TIMING */
}

/* Handle special cases of token recognition:
	IN is recognized if the last token was WORD and the token
	before that was FOR or CASE or SELECT.

	DO is recognized if the last token was WORD and the token
	before that was FOR or SELECT.

	ESAC is recognized if the last token caused `esacs_needed_count'
	to be set

	`{' is recognized if the last token as WORD and the token
	before that was FUNCTION, or if we just parsed an arithmetic
	`for' command.

	`}' is recognized if there is an unclosed `{' present.

	`-p' is returned as TIMEOPT if the last read token was TIME.
	`--' is returned as TIMEIGN if the last read token was TIME or TIMEOPT.

	']]' is returned as COND_END if the parser is currently parsing
	a conditional expression ((parser_state & PST_CONDEXPR) != 0)

	`time' is returned as TIME if and only if it is immediately
	preceded by one of `;', `\n', `||', `&&', or `&'.
*/

static int
special_case_tokens (tokstr)
     char *tokstr;
{
  /* Posix grammar rule 6 */
  if ((last_read_token == WORD) &&
#if defined (SELECT_COMMAND)
      ((token_before_that == FOR) || (token_before_that == CASE) || (token_before_that == SELECT)) &&
#else
      ((token_before_that == FOR) || (token_before_that == CASE)) &&
#endif
      (tokstr[0] == 'i' && tokstr[1] == 'n' && tokstr[2] == 0))
    {
      if (token_before_that == CASE)
	{
	  parser_state |= PST_CASEPAT;
	  esacs_needed_count++;
	}
      if (expecting_in_token)
	expecting_in_token--;
      return (IN);
    }

  /* XXX - leaving above code intact for now, but it should eventually be
     removed in favor of this clause. */
  /* Posix grammar rule 6 */
  if (expecting_in_token && (last_read_token == WORD || last_read_token == '\n') &&
      (tokstr[0] == 'i' && tokstr[1] == 'n' && tokstr[2] == 0))
    {
      if (parser_state & PST_CASESTMT)
	{
	  parser_state |= PST_CASEPAT;
	  esacs_needed_count++;
	}
      expecting_in_token--;
      return (IN);
    }
  /* Posix grammar rule 6, third word in FOR: for i; do command-list; done */
  else if (expecting_in_token && (last_read_token == '\n' || last_read_token == ';') &&
    (tokstr[0] == 'd' && tokstr[1] == 'o' && tokstr[2] == '\0'))
    {
      expecting_in_token--;
      return (DO);
    }

  /* for i do; command-list; done */
  if (last_read_token == WORD &&
#if defined (SELECT_COMMAND)
      (token_before_that == FOR || token_before_that == SELECT) &&
#else
      (token_before_that == FOR) &&
#endif
      (tokstr[0] == 'd' && tokstr[1] == 'o' && tokstr[2] == '\0'))
    {
      if (expecting_in_token)
	expecting_in_token--;
      return (DO);
    }

  /* Ditto for ESAC in the CASE case.
     Specifically, this handles "case word in esac", which is a legal
     construct, certainly because someone will pass an empty arg to the
     case construct, and we don't want it to barf.  Of course, we should
     insist that the case construct has at least one pattern in it, but
     the designers disagree. */
  if (esacs_needed_count)
    {
      if (last_read_token == IN && STREQ (tokstr, "esac"))
	{
	  esacs_needed_count--;
	  parser_state &= ~PST_CASEPAT;
	  return (ESAC);
	}
    }

  /* The start of a shell function definition. */
  if (parser_state & PST_ALLOWOPNBRC)
    {
      parser_state &= ~PST_ALLOWOPNBRC;
      if (tokstr[0] == '{' && tokstr[1] == '\0')		/* } */
	{
	  open_brace_count++;
	  function_bstart = line_number;
	  return ('{');					/* } */
	}
    }

  /* We allow a `do' after a for ((...)) without an intervening
     list_terminator */
  if (last_read_token == ARITH_FOR_EXPRS && tokstr[0] == 'd' && tokstr[1] == 'o' && !tokstr[2])
    return (DO);
  if (last_read_token == ARITH_FOR_EXPRS && tokstr[0] == '{' && tokstr[1] == '\0')	/* } */
    {
      open_brace_count++;
      return ('{');			/* } */
    }

  if (open_brace_count && reserved_word_acceptable (last_read_token) && tokstr[0] == '}' && !tokstr[1])
    {
      open_brace_count--;		/* { */
      return ('}');
    }

#if defined (COMMAND_TIMING)
  /* Handle -p after `time'. */
  if (last_read_token == TIME && tokstr[0] == '-' && tokstr[1] == 'p' && !tokstr[2])
    return (TIMEOPT);
  /* Handle -- after `time'. */
  if (last_read_token == TIME && tokstr[0] == '-' && tokstr[1] == '-' && !tokstr[2])
    return (TIMEIGN);
  /* Handle -- after `time -p'. */
  if (last_read_token == TIMEOPT && tokstr[0] == '-' && tokstr[1] == '-' && !tokstr[2])
    return (TIMEIGN);
#endif

#if defined (COND_COMMAND) /* [[ */
  if ((parser_state & PST_CONDEXPR) && tokstr[0] == ']' && tokstr[1] == ']' && tokstr[2] == '\0')
    return (COND_END);
#endif

  return (-1);
}

/* Called from shell.c when Control-C is typed at top level.  Or
   by the error rule at top level. */
void
reset_parser ()
{
  dstack.delimiter_depth = 0;	/* No delimiters found so far. */
  open_brace_count = 0;

#if defined (EXTENDED_GLOB)
  /* Reset to global value of extended glob */
  if (parser_state & (PST_EXTPAT|PST_CMDSUBST))
    extended_glob = global_extglob;
#endif

  parser_state = 0;
  here_doc_first_line = 0;

#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  if (pushed_string_list)
    free_string_list ();
#endif /* ALIAS || DPAREN_ARITHMETIC */

  /* This is where we resynchronize to the next newline on error/reset */
  if (shell_input_line)
    {
      free (shell_input_line);
      shell_input_line = (char *)NULL;
      shell_input_line_size = shell_input_line_index = 0;
    }

  FREE (word_desc_to_read);
  word_desc_to_read = (WORD_DESC *)NULL;

  eol_ungetc_lookahead = 0;

  /* added post-bash-5.1 */
  need_here_doc = 0;
  redir_stack[0] = 0;
  esacs_needed_count = expecting_in_token = 0;

  current_token = '\n';		/* XXX */
  last_read_token = '\n';
  token_to_read = '\n';
}

void
reset_readahead_token ()
{
  if (token_to_read == '\n')
    token_to_read = 0;
}

/* Read the next token.  Command can be READ (normal operation) or
   RESET (to normalize state). */
static int
read_token (command)
     int command;
{
  int character;		/* Current character. */
  int peek_char;		/* Temporary look-ahead character. */
  int result;			/* The thing to return. */

  if (command == RESET)
    {
      reset_parser ();
      return ('\n');
    }

  if (token_to_read)
    {
      result = token_to_read;
      if (token_to_read == WORD || token_to_read == ASSIGNMENT_WORD)
	{
	  yylval.word = word_desc_to_read;
	  word_desc_to_read = (WORD_DESC *)NULL;
	}
      token_to_read = 0;
      return (result);
    }

#if defined (COND_COMMAND)
  if ((parser_state & (PST_CONDCMD|PST_CONDEXPR)) == PST_CONDCMD)
    {
      cond_lineno = line_number;
      parser_state |= PST_CONDEXPR;
      yylval.command = parse_cond_command ();
      if (cond_token != COND_END)
	{
	  cond_error ();
	  return (-1);
	}
      token_to_read = COND_END;
      parser_state &= ~(PST_CONDEXPR|PST_CONDCMD);
      return (COND_CMD);
    }
#endif

#if defined (ALIAS)
  /* This is a place to jump back to once we have successfully expanded a
     token with an alias and pushed the string with push_string () */
 re_read_token:
#endif /* ALIAS */

  /* Read a single word from input.  Start by skipping blanks. */
  while ((character = shell_getc (1)) != EOF && shellblank (character))
    ;

  if (character == EOF)
    {
      EOF_Reached = 1;
      return (yacc_EOF);
    }

  /* If we hit the end of the string and we're not expanding an alias (e.g.,
     we are eval'ing a string that is an incomplete command), return EOF */
  if (character == '\0' && bash_input.type == st_string && expanding_alias() == 0)
    {
      INTERNAL_DEBUG (("shell_getc: bash_input.location.string = `%s'", bash_input.location.string));
      EOF_Reached = 1;
      return (yacc_EOF);
    }

  if MBTEST(character == '#' && (!interactive || interactive_comments))
    {
      /* A comment.  Discard until EOL or EOF, and then return a newline. */
      parser_state |= PST_COMMENT;
      discard_until ('\n');
      shell_getc (0);
      parser_state &= ~PST_COMMENT;
      character = '\n';	/* this will take the next if statement and return. */
    }

  if MBTEST(character == '\n')
    {
      /* If we're about to return an unquoted newline, we can go and collect
	 the text of any pending here document. */
      if (need_here_doc)
	gather_here_documents ();

#if defined (ALIAS)
      parser_state &= ~PST_ALEXPNEXT;
#endif /* ALIAS */

      parser_state &= ~PST_ASSIGNOK;

      return (character);
    }

  if (parser_state & PST_REGEXP)
    goto tokword;

  /* Shell meta-characters. */
  if MBTEST(shellmeta (character))
    {
#if defined (ALIAS)
      /* Turn off alias tokenization iff this character sequence would
	 not leave us ready to read a command. */
      if (character == '<' || character == '>')
	parser_state &= ~PST_ALEXPNEXT;
#endif /* ALIAS */

      parser_state &= ~PST_ASSIGNOK;

      /* If we are parsing a command substitution and we have read a character
	 that marks the end of it, don't bother to skip over quoted newlines
	 when we read the next token. We're just interested in a character
	 that will turn this into a two-character token, so we let the higher
	 layers deal with quoted newlines following the command substitution. */
      if ((parser_state & PST_CMDSUBST) && character == shell_eof_token)
	peek_char = shell_getc (0);
      else
	peek_char = shell_getc (1);

      if MBTEST(character == peek_char)
	{
	  switch (character)
	    {
	    case '<':
	      /* If '<' then we could be at "<<" or at "<<-".  We have to
		 look ahead one more character. */
	      peek_char = shell_getc (1);
	      if MBTEST(peek_char == '-')
		return (LESS_LESS_MINUS);
	      else if MBTEST(peek_char == '<')
		return (LESS_LESS_LESS);
	      else
		{
		  shell_ungetc (peek_char);
		  return (LESS_LESS);
		}

	    case '>':
	      return (GREATER_GREATER);

	    case ';':
	      parser_state |= PST_CASEPAT;
#if defined (ALIAS)
	      parser_state &= ~PST_ALEXPNEXT;
#endif /* ALIAS */

	      peek_char = shell_getc (1);
	      if MBTEST(peek_char == '&')
		return (SEMI_SEMI_AND);
	      else
		{
		  shell_ungetc (peek_char);
		  return (SEMI_SEMI);
		}

	    case '&':
	      return (AND_AND);

	    case '|':
	      return (OR_OR);

#if defined (DPAREN_ARITHMETIC) || defined (ARITH_FOR_COMMAND)
	    case '(':		/* ) */
	      result = parse_dparen (character);
	      if (result == -2)
	        break;
	      else
	        return result;
#endif
	    }
	}
      else if MBTEST(character == '<' && peek_char == '&')
	return (LESS_AND);
      else if MBTEST(character == '>' && peek_char == '&')
	return (GREATER_AND);
      else if MBTEST(character == '<' && peek_char == '>')
	return (LESS_GREATER);
      else if MBTEST(character == '>' && peek_char == '|')
	return (GREATER_BAR);
      else if MBTEST(character == '&' && peek_char == '>')
	{
	  peek_char = shell_getc (1);
	  if MBTEST(peek_char == '>')
	    return (AND_GREATER_GREATER);
	  else
	    {
	      shell_ungetc (peek_char);
	      return (AND_GREATER);
	    }
	}
      else if MBTEST(character == '|' && peek_char == '&')
	return (BAR_AND);
      else if MBTEST(character == ';' && peek_char == '&')
	{
	  parser_state |= PST_CASEPAT;
#if defined (ALIAS)
	  parser_state &= ~PST_ALEXPNEXT;
#endif /* ALIAS */
	  return (SEMI_AND);
	}

      shell_ungetc (peek_char);

      /* If we look like we are reading the start of a function
	 definition, then let the reader know about it so that
	 we will do the right thing with `{'. */
      if MBTEST(character == ')' && last_read_token == '(' && token_before_that == WORD)
	{
	  parser_state |= PST_ALLOWOPNBRC;
#if defined (ALIAS)
	  parser_state &= ~PST_ALEXPNEXT;
#endif /* ALIAS */
	  function_dstart = line_number;
	}

      /* case pattern lists may be preceded by an optional left paren.  If
	 we're not trying to parse a case pattern list, the left paren
	 indicates a subshell. */
      if MBTEST(character == '(' && (parser_state & PST_CASEPAT) == 0) /* ) */
	parser_state |= PST_SUBSHELL;
      /*(*/
      else if MBTEST((parser_state & PST_CASEPAT) && character == ')')
	parser_state &= ~PST_CASEPAT;
      /*(*/
      else if MBTEST((parser_state & PST_SUBSHELL) && character == ')')
	parser_state &= ~PST_SUBSHELL;

#if defined (PROCESS_SUBSTITUTION)
      /* Check for the constructs which introduce process substitution.
	 Shells running in `posix mode' don't do process substitution. */
      if MBTEST((character != '>' && character != '<') || peek_char != '(') /*)*/
#endif /* PROCESS_SUBSTITUTION */
	return (character);
    }

  /* Hack <&- (close stdin) case.  Also <&N- (dup and close). */
  if MBTEST(character == '-' && (last_read_token == LESS_AND || last_read_token == GREATER_AND))
    return (character);

tokword:
  /* Okay, if we got this far, we have to read a word.  Read one,
     and then check it against the known ones. */
  result = read_token_word (character);
#if defined (ALIAS)
  if (result == RE_READ_TOKEN)
    goto re_read_token;
#endif
  return result;
}

/*
 * Match a $(...) or other grouping construct.  This has to handle embedded
 * quoted strings ('', ``, "") and nested constructs.  It also must handle
 * reprompting the user, if necessary, after reading a newline, and returning
 * correct error values if it reads EOF.
 */
#define P_FIRSTCLOSE	0x0001
#define P_ALLOWESC	0x0002
#define P_DQUOTE	0x0004
#define P_COMMAND	0x0008	/* parsing a command, so look for comments */
#define P_BACKQUOTE	0x0010	/* parsing a backquoted command substitution */
#define P_ARRAYSUB	0x0020	/* parsing a [...] array subscript for assignment */
#define P_DOLBRACE	0x0040	/* parsing a ${...} construct */

/* Lexical state while parsing a grouping construct or $(...). */
#define LEX_WASDOL	0x0001
#define LEX_CKCOMMENT	0x0002
#define LEX_INCOMMENT	0x0004
#define LEX_PASSNEXT	0x0008
#define LEX_RESWDOK	0x0010
#define LEX_CKCASE	0x0020
#define LEX_INCASE	0x0040
#define LEX_INHEREDOC	0x0080
#define LEX_HEREDELIM	0x0100		/* reading here-doc delimiter */
#define LEX_STRIPDOC	0x0200		/* <<- strip tabs from here doc delim */
#define LEX_QUOTEDDOC	0x0400		/* here doc with quoted delim */
#define LEX_INWORD	0x0800
#define LEX_GTLT	0x1000
#define LEX_CKESAC	0x2000		/* check esac after in -- for later */
#define LEX_CASEWD	0x4000		/* word after case */
#define LEX_PATLIST	0x8000		/* case statement pattern list */

#define COMSUB_META(ch)		((ch) == ';' || (ch) == '&' || (ch) == '|')

#define CHECK_NESTRET_ERROR() \
  do { \
    if (nestret == &matched_pair_error) \
      { \
	free (ret); \
	return &matched_pair_error; \
      } \
  } while (0)

#define APPEND_NESTRET() \
  do { \
    if (nestlen) \
      { \
	RESIZE_MALLOCED_BUFFER (ret, retind, nestlen, retsize, 64); \
	strcpy (ret + retind, nestret); \
	retind += nestlen; \
      } \
  } while (0)

static char matched_pair_error;

static char *
parse_matched_pair (qc, open, close, lenp, flags)
     int qc;	/* `"' if this construct is within double quotes */
     int open, close;
     int *lenp, flags;
{
  int count, ch, prevch, tflags;
  int nestlen, ttranslen, start_lineno;
  char *ret, *nestret, *ttrans;
  int retind, retsize, rflags;
  int dolbrace_state;

  dolbrace_state = (flags & P_DOLBRACE) ? DOLBRACE_PARAM : 0;

/*itrace("parse_matched_pair[%d]: open = %c close = %c flags = %d", line_number, open, close, flags);*/
  count = 1;
  tflags = 0;

  if ((flags & P_COMMAND) && qc != '`' && qc != '\'' && qc != '"' && (flags & P_DQUOTE) == 0)
    tflags |= LEX_CKCOMMENT;

  /* RFLAGS is the set of flags we want to pass to recursive calls. */
  rflags = (qc == '"') ? P_DQUOTE : (flags & P_DQUOTE);

  ret = (char *)xmalloc (retsize = 64);
  retind = 0;

  start_lineno = line_number;
  ch = EOF;		/* just in case */
  while (count)
    {
      prevch = ch;
      ch = shell_getc (qc != '\'' && (tflags & (LEX_PASSNEXT)) == 0);

      if (ch == EOF)
	{
	  free (ret);
	  parser_error (start_lineno, _("unexpected EOF while looking for matching `%c'"), close);
	  EOF_Reached = 1;	/* XXX */
	  return (&matched_pair_error);
	}

      /* Possible reprompting. */
      if MBTEST(ch == '\n' && SHOULD_PROMPT ())
	prompt_again (0);

      /* Don't bother counting parens or doing anything else if in a comment
	 or part of a case statement */
      if (tflags & LEX_INCOMMENT)
	{
	  /* Add this character. */
	  RESIZE_MALLOCED_BUFFER (ret, retind, 1, retsize, 64);
	  ret[retind++] = ch;

	  if MBTEST(ch == '\n')
	    tflags &= ~LEX_INCOMMENT;

	  continue;
	}

      /* Not exactly right yet, should handle shell metacharacters, too.  If
	 any changes are made to this test, make analogous changes to subst.c:
	 extract_delimited_string(). */
      else if MBTEST((tflags & LEX_CKCOMMENT) && (tflags & LEX_INCOMMENT) == 0 && ch == '#' && (retind == 0 || ret[retind-1] == '\n' || shellblank (ret[retind - 1])))
	tflags |= LEX_INCOMMENT;

      if (tflags & LEX_PASSNEXT)		/* last char was backslash */
	{
	  tflags &= ~LEX_PASSNEXT;
	  /* XXX - PST_NOEXPAND? */
	  if MBTEST(qc != '\'' && ch == '\n')	/* double-quoted \<newline> disappears. */
	    {
	      if (retind > 0)
		retind--;	/* swallow previously-added backslash */
	      continue;
	    }

	  RESIZE_MALLOCED_BUFFER (ret, retind, 2, retsize, 64);
	  if MBTEST(ch == CTLESC)
	    ret[retind++] = CTLESC;
	  ret[retind++] = ch;
	  continue;
	}
      /* If we're reparsing the input (e.g., from parse_string_to_word_list),
	 we've already prepended CTLESC to single-quoted results of $'...'.
	 We may want to do this for other CTLESC-quoted characters in
	 reparse, too. */
      else if MBTEST((parser_state & PST_REPARSE) && open == '\'' && (ch == CTLESC || ch == CTLNUL))
	{
	  RESIZE_MALLOCED_BUFFER (ret, retind, 1, retsize, 64);
	  ret[retind++] = ch;
	  continue;
	}
      else if MBTEST(ch == CTLESC || ch == CTLNUL)	/* special shell escapes */
	{
	  RESIZE_MALLOCED_BUFFER (ret, retind, 2, retsize, 64);
	  ret[retind++] = CTLESC;
	  ret[retind++] = ch;
	  continue;
	}
      else if MBTEST(ch == close)		/* ending delimiter */
	count--;
      /* handle nested ${...} specially. */
      else if MBTEST(open != close && (tflags & LEX_WASDOL) && open == '{' && ch == open) /* } */
	count++;
      else if MBTEST(((flags & P_FIRSTCLOSE) == 0) && ch == open)	/* nested begin */
	count++;

      /* Add this character. */
      RESIZE_MALLOCED_BUFFER (ret, retind, 1, retsize, 64);
      ret[retind++] = ch;

      /* If we just read the ending character, don't bother continuing. */
      if (count == 0)
	break;

      if (open == '\'')			/* '' inside grouping construct */
	{
	  if MBTEST((flags & P_ALLOWESC) && ch == '\\')
	    tflags |= LEX_PASSNEXT;
	  continue;
	}

      if MBTEST(ch == '\\')			/* backslashes */
	tflags |= LEX_PASSNEXT;

      /* Based on which dolstate is currently in (param, op, or word),
	 decide what the op is.  We're really only concerned if it's % or
	 #, so we can turn on a flag that says whether or not we should
	 treat single quotes as special when inside a double-quoted
	 ${...}. This logic must agree with subst.c:extract_dollar_brace_string
	 since they share the same defines. */
      /* FLAG POSIX INTERP 221 */
      if (flags & P_DOLBRACE)
        {
          /* ${param%[%]word} */
	  if MBTEST(dolbrace_state == DOLBRACE_PARAM && ch == '%' && retind > 1)
	    dolbrace_state = DOLBRACE_QUOTE;
          /* ${param#[#]word} */
	  else if MBTEST(dolbrace_state == DOLBRACE_PARAM && ch == '#' && retind > 1)
	    dolbrace_state = DOLBRACE_QUOTE;
          /* ${param/[/]pat/rep} */
	  else if MBTEST(dolbrace_state == DOLBRACE_PARAM && ch == '/' && retind > 1)
	    dolbrace_state = DOLBRACE_QUOTE2;	/* XXX */
          /* ${param^[^]pat} */
	  else if MBTEST(dolbrace_state == DOLBRACE_PARAM && ch == '^' && retind > 1)
	    dolbrace_state = DOLBRACE_QUOTE;
          /* ${param,[,]pat} */
	  else if MBTEST(dolbrace_state == DOLBRACE_PARAM && ch == ',' && retind > 1)
	    dolbrace_state = DOLBRACE_QUOTE;
	  else if MBTEST(dolbrace_state == DOLBRACE_PARAM && strchr ("#%^,~:-=?+/", ch) != 0)
	    dolbrace_state = DOLBRACE_OP;
	  else if MBTEST(dolbrace_state == DOLBRACE_OP && strchr ("#%^,~:-=?+/", ch) == 0)
	    dolbrace_state = DOLBRACE_WORD;
        }

      /* The big hammer.  Single quotes aren't special in double quotes.  The
         problem is that Posix used to say the single quotes are semi-special:
         within a double-quoted ${...} construct "an even number of
         unescaped double-quotes or single-quotes, if any, shall occur." */
      /* This was changed in Austin Group Interp 221 */
      if MBTEST(posixly_correct && shell_compatibility_level > 41 && dolbrace_state != DOLBRACE_QUOTE && dolbrace_state != DOLBRACE_QUOTE2 && (flags & P_DQUOTE) && (flags & P_DOLBRACE) && ch == '\'')
	continue;

      /* Could also check open == '`' if we want to parse grouping constructs
	 inside old-style command substitution. */
      if (open != close)		/* a grouping construct */
	{
	  if MBTEST(shellquote (ch))
	    {
	      /* '', ``, or "" inside $(...) or other grouping construct. */
	      push_delimiter (dstack, ch);
	      if MBTEST((tflags & LEX_WASDOL) && ch == '\'')	/* $'...' inside group */
		nestret = parse_matched_pair (ch, ch, ch, &nestlen, P_ALLOWESC|rflags);
	      else
		nestret = parse_matched_pair (ch, ch, ch, &nestlen, rflags);
	      pop_delimiter (dstack);
	      CHECK_NESTRET_ERROR ();

	      if MBTEST((tflags & LEX_WASDOL) && ch == '\'' && (extended_quote || (rflags & P_DQUOTE) == 0 || dolbrace_state == DOLBRACE_QUOTE || dolbrace_state == DOLBRACE_QUOTE2))
		{
		  /* Translate $'...' here. */
		  /* PST_NOEXPAND */
		  ttrans = ansiexpand (nestret, 0, nestlen - 1, &ttranslen);
		  free (nestret);

		  /* If we're parsing a double-quoted brace expansion and we are
		     not in a place where single quotes are treated specially,
		     make sure we single-quote the results of the ansi
		     expansion because quote removal should remove them later */
		  /* FLAG POSIX INTERP 221 */
		  if ((shell_compatibility_level > 42) && (rflags & P_DQUOTE) && (dolbrace_state == DOLBRACE_QUOTE2 || dolbrace_state == DOLBRACE_QUOTE) && (flags & P_DOLBRACE))
		    {
		      nestret = sh_single_quote (ttrans);
		      free (ttrans);
		      nestlen = strlen (nestret);
		    }
#if 0 /* TAG:bash-5.3 */
		  /* This single-quotes PARAM in ${PARAM OP WORD} when PARAM
		     contains a $'...' even when extended_quote is set. */
		  else if ((rflags & P_DQUOTE) && (dolbrace_state == DOLBRACE_PARAM) && (flags & P_DOLBRACE))
		    {
		      nestret = sh_single_quote (ttrans);
		      free (ttrans);
		      nestlen = strlen (nestret);
		    }
#endif
		  else if ((rflags & P_DQUOTE) == 0)
		    {
		      nestret = sh_single_quote (ttrans);
		      free (ttrans);
		      nestlen = strlen (nestret);
		    }
		  else
		    {
		      /* Should we quote CTLESC here? */
		      nestret = ttrans;
		      nestlen = ttranslen;
		    }
		  retind -= 2;		/* back up before the $' */
		}
#if defined (TRANSLATABLE_STRINGS)
	      else if MBTEST((tflags & LEX_WASDOL) && ch == '"' && (extended_quote || (rflags & P_DQUOTE) == 0))
		{
		  /* Locale expand $"..." here. */
		  /* PST_NOEXPAND */
		  ttrans = locale_expand (nestret, 0, nestlen - 1, start_lineno, &ttranslen);
		  free (nestret);

		  /* If we're supposed to single-quote translated strings,
		     check whether the translated result is different from
		     the original and single-quote the string if it is. */
		  if (singlequote_translations &&
		        ((nestlen - 1) != ttranslen || STREQN (nestret, ttrans, ttranslen) == 0))
		    {
		      if ((rflags & P_DQUOTE) == 0)
			nestret = sh_single_quote (ttrans);
		      else if ((rflags & P_DQUOTE) && (dolbrace_state == DOLBRACE_QUOTE2) && (flags & P_DOLBRACE))
			nestret = sh_single_quote (ttrans);
		      else
			/* single quotes aren't special, use backslash instead */
			nestret = sh_backslash_quote_for_double_quotes (ttrans, 0);
		    }
		  else
		    nestret = sh_mkdoublequoted (ttrans, ttranslen, 0);
		  free (ttrans);
		  nestlen = strlen (nestret);
		  retind -= 2;		/* back up before the $" */
		}
#endif /* TRANSLATABLE_STRINGS */

	      APPEND_NESTRET ();
	      FREE (nestret);
	    }
	  else if ((flags & (P_ARRAYSUB|P_DOLBRACE)) && (tflags & LEX_WASDOL) && (ch == '(' || ch == '{' || ch == '['))	/* ) } ] */
	    goto parse_dollar_word;
#if defined (PROCESS_SUBSTITUTION)
	  /* XXX - technically this should only be recognized at the start of
	     a word */
	  else if ((flags & (P_ARRAYSUB|P_DOLBRACE)) && (tflags & LEX_GTLT) && (ch == '('))	/* ) */
	    goto parse_dollar_word;
#endif
	}
      /* Parse an old-style command substitution within double quotes as a
	 single word. */
      /* XXX - sh and ksh93 don't do this - XXX */
      else if MBTEST(open == '"' && ch == '`')
	{
	  nestret = parse_matched_pair (0, '`', '`', &nestlen, rflags);

	  CHECK_NESTRET_ERROR ();
	  APPEND_NESTRET ();

	  FREE (nestret);
	}
      else if MBTEST(open != '`' && (tflags & LEX_WASDOL) && (ch == '(' || ch == '{' || ch == '['))	/* ) } ] */
	/* check for $(), $[], or ${} inside quoted string. */
	{
parse_dollar_word:
	  if (open == ch)	/* undo previous increment */
	    count--;
	  if (ch == '(')		/* ) */
	    nestret = parse_comsub (0, '(', ')', &nestlen, (rflags|P_COMMAND) & ~P_DQUOTE);
	  else if (ch == '{')		/* } */
	    nestret = parse_matched_pair (0, '{', '}', &nestlen, P_FIRSTCLOSE|P_DOLBRACE|rflags);
	  else if (ch == '[')		/* ] */
	    nestret = parse_matched_pair (0, '[', ']', &nestlen, rflags);

	  CHECK_NESTRET_ERROR ();
	  APPEND_NESTRET ();

	  FREE (nestret);
	}
#if defined (PROCESS_SUBSTITUTION)
      if MBTEST((ch == '<' || ch == '>') && (tflags & LEX_GTLT) == 0)
	tflags |= LEX_GTLT;
      else
	tflags &= ~LEX_GTLT;
#endif
      if MBTEST(ch == '$' && (tflags & LEX_WASDOL) == 0)
	tflags |= LEX_WASDOL;
      else
	tflags &= ~LEX_WASDOL;
    }

  ret[retind] = '\0';
  if (lenp)
    *lenp = retind;
/*itrace("parse_matched_pair[%d]: returning %s", line_number, ret);*/
  return ret;
}

#if defined (DEBUG)
static void
dump_tflags (flags)
     int flags;
{
  int f;

  f = flags;
  fprintf (stderr, "%d -> ", f);
  if (f & LEX_WASDOL)
    {
      f &= ~LEX_WASDOL;
      fprintf (stderr, "LEX_WASDOL%s", f ? "|" : "");
    }
  if (f & LEX_CKCOMMENT)
    {
      f &= ~LEX_CKCOMMENT;
      fprintf (stderr, "LEX_CKCOMMENT%s", f ? "|" : "");
    }
  if (f & LEX_INCOMMENT)
    {
      f &= ~LEX_INCOMMENT;
      fprintf (stderr, "LEX_INCOMMENT%s", f ? "|" : "");
    }
  if (f & LEX_PASSNEXT)
    {
      f &= ~LEX_PASSNEXT;
      fprintf (stderr, "LEX_PASSNEXT%s", f ? "|" : "");
    }
  if (f & LEX_RESWDOK)
    {
      f &= ~LEX_RESWDOK;
      fprintf (stderr, "LEX_RESWDOK%s", f ? "|" : "");
    }
  if (f & LEX_CKCASE)
    {
      f &= ~LEX_CKCASE;
      fprintf (stderr, "LEX_CKCASE%s", f ? "|" : "");
    }
  if (f & LEX_CKESAC)
    {
      f &= ~LEX_CKESAC;
      fprintf (stderr, "LEX_CKESAC%s", f ? "|" : "");
    }
  if (f & LEX_INCASE)
    {
      f &= ~LEX_INCASE;
      fprintf (stderr, "LEX_INCASE%s", f ? "|" : "");
    }
  if (f & LEX_CASEWD)
    {
      f &= ~LEX_CASEWD;
      fprintf (stderr, "LEX_CASEWD%s", f ? "|" : "");
    }
  if (f & LEX_PATLIST)
    {
      f &= ~LEX_PATLIST;
      fprintf (stderr, "LEX_PATLIST%s", f ? "|" : "");
    }
  if (f & LEX_INHEREDOC)
    {
      f &= ~LEX_INHEREDOC;
      fprintf (stderr, "LEX_INHEREDOC%s", f ? "|" : "");
    }
  if (f & LEX_HEREDELIM)
    {
      f &= ~LEX_HEREDELIM;
      fprintf (stderr, "LEX_HEREDELIM%s", f ? "|" : "");
    }
  if (f & LEX_STRIPDOC)
    {
      f &= ~LEX_STRIPDOC;
      fprintf (stderr, "LEX_WASDOL%s", f ? "|" : "");
    }
  if (f & LEX_QUOTEDDOC)
    {
      f &= ~LEX_QUOTEDDOC;
      fprintf (stderr, "LEX_QUOTEDDOC%s", f ? "|" : "");
    }
  if (f & LEX_INWORD)
    {
      f &= ~LEX_INWORD;
      fprintf (stderr, "LEX_INWORD%s", f ? "|" : "");
    }

  fprintf (stderr, "\n");
  fflush (stderr);
}
#endif

/* Parse a $(...) command substitution.  This reads input from the current
   input stream. */
static char *
parse_comsub (qc, open, close, lenp, flags)
     int qc;	/* `"' if this construct is within double quotes */
     int open, close;
     int *lenp, flags;
{
  int peekc, r;
  int start_lineno, local_extglob, was_extpat;
  char *ret, *tcmd;
  int retlen;
  sh_parser_state_t ps;
  STRING_SAVER *saved_strings;
  COMMAND *saved_global, *parsed_command;

  /* Posix interp 217 says arithmetic expressions have precedence, so
     assume $(( introduces arithmetic expansion and parse accordingly. */
  if (open == '(')		/*)*/
    {
      peekc = shell_getc (1);
      shell_ungetc (peekc);
      if (peekc == '(')		/*)*/
	return (parse_matched_pair (qc, open, close, lenp, 0));
    }

/*itrace("parse_comsub: qc = `%c' open = %c close = %c", qc, open, close);*/

  /*debug_parser(1);*/
  start_lineno = line_number;

  save_parser_state (&ps);

  was_extpat = (parser_state & PST_EXTPAT);

  /* State flags we don't want to persist into command substitutions. */
  parser_state &= ~(PST_REGEXP|PST_EXTPAT|PST_CONDCMD|PST_CONDEXPR|PST_COMPASSIGN);
  /* Could do PST_CASESTMT too, but that also affects history. Setting
     expecting_in_token below should take care of the parsing requirements.
     Unsetting PST_REDIRLIST isn't strictly necessary because of how we set
     token_to_read below, but we do it anyway. */
  parser_state &= ~(PST_CASEPAT|PST_ALEXPNEXT|PST_SUBSHELL|PST_REDIRLIST);
  /* State flags we want to set for this run through the parser. */
  parser_state |= PST_CMDSUBST|PST_EOFTOKEN|PST_NOEXPAND;

  /* leave pushed_string_list alone, since we might need to consume characters
     from it to satisfy this command substitution (in some perverse case). */
  shell_eof_token = close;

  saved_global = global_command;		/* might not be necessary */
  global_command = (COMMAND *)NULL;

  /* These are reset by reset_parser() */
  need_here_doc = 0;
  esacs_needed_count = expecting_in_token = 0;

  /* We want to expand aliases on this pass if we're in posix mode, since the
     standard says you have to take aliases into account when looking for the
     terminating right paren. Otherwise, we defer until execution time for
     backwards compatibility. */
  if (expand_aliases)
    expand_aliases = posixly_correct != 0;
#if defined (EXTENDED_GLOB)
  /* If (parser_state & PST_EXTPAT), we're parsing an extended pattern for a
     conditional command and have already set global_extglob appropriately. */
  if (shell_compatibility_level <= 51 && was_extpat == 0)
    {
      local_extglob = global_extglob = extended_glob;
      extended_glob = 1;
    }
#endif

  current_token = '\n';				/* XXX */
  token_to_read = DOLPAREN;			/* let's trick the parser */

  r = yyparse ();

  if (need_here_doc > 0)
    {
      internal_warning ("command substitution: %d unterminated here-document%s", need_here_doc, (need_here_doc == 1) ? "" : "s");
      gather_here_documents ();	/* XXX check compatibility level? */
    }

#if defined (EXTENDED_GLOB)
  if (shell_compatibility_level <= 51 && was_extpat == 0)
    extended_glob = local_extglob;
#endif

  parsed_command = global_command;

  if (EOF_Reached)
    {
      shell_eof_token = ps.eof_token;
      expand_aliases = ps.expand_aliases;

      /* yyparse() has already called yyerror() and reset_parser() */
      return (&matched_pair_error);
    }
  else if (r != 0)
    {
      /* parser_error (start_lineno, _("could not parse command substitution")); */
      /* Non-interactive shells exit on parse error in a command substitution. */
      if (last_command_exit_value == 0)
	last_command_exit_value = EXECUTION_FAILURE;
      set_exit_status (last_command_exit_value);
      if (interactive_shell == 0)
	jump_to_top_level (FORCE_EOF);	/* This is like reader_loop() */
      else
	{
	  shell_eof_token = ps.eof_token;
	  expand_aliases = ps.expand_aliases;

	  jump_to_top_level (DISCARD);	/* XXX - return (&matched_pair_error)? */
	}
    }

  if (current_token != shell_eof_token)
    {
INTERNAL_DEBUG(("current_token (%d) != shell_eof_token (%c)", current_token, shell_eof_token));
      token_to_read = current_token;

      /* If we get here we can check eof_encountered and if it's 1 but the
	 previous EOF_Reached test didn't succeed, we can assume that the shell
	 is interactive and ignoreeof is set. We might want to restore the
	 parser state in this case. */
      shell_eof_token = ps.eof_token;
      expand_aliases = ps.expand_aliases;

      return (&matched_pair_error);
    }

  /* We don't want to restore the old pushed string list, since we might have
     used it to consume additional input from an alias while parsing this
     command substitution. */
  saved_strings = pushed_string_list;
  restore_parser_state (&ps);
  pushed_string_list = saved_strings;

  tcmd = print_comsub (parsed_command);		/* returns static memory */
  retlen = strlen (tcmd);
  if (tcmd[0] == '(')			/* ) need a space to prevent arithmetic expansion */
    retlen++;
  ret = xmalloc (retlen + 2);
  if (tcmd[0] == '(')			/* ) */
    {
      ret[0] = ' ';
      strcpy (ret + 1, tcmd);
    }
  else
    strcpy (ret, tcmd);
  ret[retlen++] = ')';
  ret[retlen] = '\0';

  dispose_command (parsed_command);
  global_command = saved_global;

  if (lenp)
    *lenp = retlen;

/*itrace("parse_comsub:%d: returning `%s'", line_number, ret);*/
  return ret;
}

/* Recursively call the parser to parse a $(...) command substitution. This is
   called by the word expansion code and so does not have to reset as much
   parser state before calling yyparse(). */
char *
xparse_dolparen (base, string, indp, flags)
     char *base;
     char *string;
     int *indp;
     int flags;
{
  sh_parser_state_t ps;
  sh_input_line_state_t ls;
  int orig_ind, nc, sflags, start_lineno;
  char *ret, *ep, *ostring;

/*debug_parser(1);*/
  orig_ind = *indp;
  ostring = string;
  start_lineno = line_number;

  if (*string == 0)
    {
      if (flags & SX_NOALLOC) 
	return (char *)NULL;

      ret = xmalloc (1);
      ret[0] = '\0';
      return ret;
    }

/*itrace("xparse_dolparen: size = %d shell_input_line = `%s' string=`%s'", shell_input_line_size, shell_input_line, string);*/

  sflags = SEVAL_NONINT|SEVAL_NOHIST|SEVAL_NOFREE;
  if (flags & SX_NOLONGJMP)
    sflags |= SEVAL_NOLONGJMP;

  save_parser_state (&ps);
  save_input_line_state (&ls);

#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  pushed_string_list = (STRING_SAVER *)NULL;
#endif
  /*(*/
  parser_state |= PST_CMDSUBST|PST_EOFTOKEN;	/* allow instant ')' */ /*(*/
  shell_eof_token = ')';
  if (flags & SX_COMPLETE)
    parser_state |= PST_NOERROR;

  /* Don't expand aliases on this pass at all. Either parse_comsub() does it
     at parse time, in which case this string already has aliases expanded,
     or command_substitute() does it in the child process executing the
     command substitution and we want to defer it completely until then. The
     old value will be restored by restore_parser_state(). */
  expand_aliases = 0;
#if defined (EXTENDED_GLOB)
  global_extglob = extended_glob;		/* for reset_parser() */
#endif

  token_to_read = DOLPAREN;			/* let's trick the parser */

  nc = parse_string (string, "command substitution", sflags, (COMMAND **)NULL, &ep);

  /* Should we save and restore the bison/yacc lookahead token (yychar) here?
     Or only if it's not YYEMPTY? */
  if (current_token == shell_eof_token)
    yyclearin;		/* might want to clear lookahead token unconditionally */

  reset_parser ();	/* resets extended_glob too */
  /* reset_parser() clears shell_input_line and associated variables, including
     parser_state, so we want to reset things, then restore what we need. */
  restore_input_line_state (&ls);
  restore_parser_state (&ps);

  token_to_read = 0;

  /* If parse_string returns < 0, we need to jump to top level with the
     negative of the return value. We abandon the rest of this input line
     first */
  if (nc < 0)
    {
      clear_shell_input_line ();	/* XXX */
      if (bash_input.type != st_string)	/* paranoia */
	parser_state &= ~(PST_CMDSUBST|PST_EOFTOKEN);
      if ((flags & SX_NOLONGJMP) == 0)
	jump_to_top_level (-nc);	/* XXX */
    }

  /* Need to find how many characters parse_string() consumed, update
     *indp, if flags != 0, copy the portion of the string parsed into RET
     and return it.  If flags & 1 (SX_NOALLOC) we can return NULL. */

  /*(*/
  if (ep[-1] != ')')
    {
#if 0
      if (ep[-1] != '\n')
	itrace("xparse_dolparen:%d: ep[-1] != RPAREN (%d), ep = `%s'", line_number, ep[-1], ep);
#endif

      while (ep > ostring && ep[-1] == '\n') ep--;
    }

  nc = ep - ostring;
  *indp = ep - base - 1;

  /*((*/
#if 0
  if (base[*indp] != ')')
    itrace("xparse_dolparen:%d: base[%d] != RPAREN (%d), base = `%s'", line_number, *indp, base[*indp], base);
  if (*indp < orig_ind)
    itrace("xparse_dolparen:%d: *indp (%d) < orig_ind (%d), orig_string = `%s'", line_number, *indp, orig_ind, ostring);
#endif

  if (base[*indp] != ')' && (flags & SX_NOLONGJMP) == 0)
    {
      /*(*/
      if ((flags & SX_NOERROR) == 0)
	parser_error (start_lineno, _("unexpected EOF while looking for matching `%c'"), ')');
      jump_to_top_level (DISCARD);
    }

  if (flags & SX_NOALLOC) 
    return (char *)NULL;

  if (nc == 0)
    {
      ret = xmalloc (1);
      ret[0] = '\0';
    }
  else
    ret = substring (ostring, 0, nc - 1);

  return ret;
}

/* Recursively call the parser to parse the string from a $(...) command
   substitution to a COMMAND *. This is called from command_substitute() and
   has the same parser state constraints as xparse_dolparen(). */
COMMAND *
parse_string_to_command (string, flags)
     char *string;
     int flags;
{
  sh_parser_state_t ps;
  sh_input_line_state_t ls;
  int nc, sflags;
  size_t slen;
  char *ret, *ep;
  COMMAND *cmd;

  if (*string == 0)
    return (COMMAND *)NULL;

  ep = string;
  slen = STRLEN (string);

/*itrace("parse_string_to_command: size = %d shell_input_line = `%s' string=`%s'", shell_input_line_size, shell_input_line, string);*/

  sflags = SEVAL_NONINT|SEVAL_NOHIST|SEVAL_NOFREE;
  if (flags & SX_NOLONGJMP)
    sflags |= SEVAL_NOLONGJMP;

  save_parser_state (&ps);
  save_input_line_state (&ls);

#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  pushed_string_list = (STRING_SAVER *)NULL;
#endif
  if (flags & SX_COMPLETE)
    parser_state |= PST_NOERROR;

  expand_aliases = 0;

  cmd = 0;
  nc = parse_string (string, "command substitution", sflags, &cmd, &ep);

  reset_parser ();
  /* reset_parser() clears shell_input_line and associated variables, including
     parser_state, so we want to reset things, then restore what we need. */
  restore_input_line_state (&ls);
  restore_parser_state (&ps);

  /* If parse_string returns < 0, we need to jump to top level with the
     negative of the return value. We abandon the rest of this input line
     first */
  if (nc < 0)
    {
      clear_shell_input_line ();	/* XXX */
      if ((flags & SX_NOLONGJMP) == 0)
        jump_to_top_level (-nc);	/* XXX */
    }

  /* Need to check how many characters parse_string() consumed, make sure it's
     the entire string. */
  if (nc < slen)
    {
      dispose_command (cmd);
      return (COMMAND *)NULL;
    }

  return cmd;
}

#if defined (DPAREN_ARITHMETIC) || defined (ARITH_FOR_COMMAND)
/* Parse a double-paren construct.  It can be either an arithmetic
   command, an arithmetic `for' command, or a nested subshell.  Returns
   the parsed token, -1 on error, or -2 if we didn't do anything and
   should just go on. */
static int
parse_dparen (c)
     int c;
{
  int cmdtyp, sline;
  char *wval;
  WORD_DESC *wd;

#if defined (ARITH_FOR_COMMAND)
  if (last_read_token == FOR)
    {
      if (word_top < MAX_CASE_NEST)
	word_top++;
      arith_for_lineno = word_lineno[word_top] = line_number;
      cmdtyp = parse_arith_cmd (&wval, 0);
      if (cmdtyp == 1)
	{
	  wd = alloc_word_desc ();
	  wd->word = wval;
	  yylval.word_list = make_word_list (wd, (WORD_LIST *)NULL);
	  return (ARITH_FOR_EXPRS);
	}
      else
	return -1;		/* ERROR */
    }
#endif

#if defined (DPAREN_ARITHMETIC)
  if (reserved_word_acceptable (last_read_token))
    {
      sline = line_number;

      cmdtyp = parse_arith_cmd (&wval, 0);
      if (cmdtyp == 1)	/* arithmetic command */
	{
	  wd = alloc_word_desc ();
	  wd->word = wval;
	  wd->flags = W_QUOTED|W_NOSPLIT|W_NOGLOB|W_NOTILDE|W_NOPROCSUB;
	  yylval.word_list = make_word_list (wd, (WORD_LIST *)NULL);
	  return (ARITH_CMD);
	}
      else if (cmdtyp == 0)	/* nested subshell */
	{
	  push_string (wval, 0, (alias_t *)NULL);
	  pushed_string_list->flags = PSH_DPAREN;
	  if ((parser_state & PST_CASEPAT) == 0)
	    parser_state |= PST_SUBSHELL;
	  return (c);
	}
      else			/* ERROR */
	return -1;
    }
#endif

  return -2;			/* XXX */
}

/* We've seen a `(('.  Look for the matching `))'.  If we get it, return 1.
   If not, assume it's a nested subshell for backwards compatibility and
   return 0.  In any case, put the characters we've consumed into a locally-
   allocated buffer and make *ep point to that buffer.  Return -1 on an
   error, for example EOF. */
static int
parse_arith_cmd (ep, adddq)
     char **ep;
     int adddq;
{
  int exp_lineno, rval, c;
  char *ttok, *tokstr;
  int ttoklen;

  exp_lineno = line_number;
  ttok = parse_matched_pair (0, '(', ')', &ttoklen, 0);
  rval = 1;
  if (ttok == &matched_pair_error)
    return -1;
  /* Check that the next character is the closing right paren.  If
     not, this is a syntax error. ( */
  c = shell_getc (0);
  if MBTEST(c != ')')
    rval = 0;

  tokstr = (char *)xmalloc (ttoklen + 4);

  /* if ADDDQ != 0 then (( ... )) -> "..." */
  if (rval == 1 && adddq)	/* arith cmd, add double quotes */
    {
      tokstr[0] = '"';
      strncpy (tokstr + 1, ttok, ttoklen - 1);
      tokstr[ttoklen] = '"';
      tokstr[ttoklen+1] = '\0';
    }
  else if (rval == 1)		/* arith cmd, don't add double quotes */
    {
      strncpy (tokstr, ttok, ttoklen - 1);
      tokstr[ttoklen-1] = '\0';
    }
  else				/* nested subshell */
    {
      tokstr[0] = '(';
      strncpy (tokstr + 1, ttok, ttoklen - 1);
      tokstr[ttoklen] = ')';
      tokstr[ttoklen+1] = c;
      tokstr[ttoklen+2] = '\0';
    }

  *ep = tokstr;
  FREE (ttok);
  return rval;
}
#endif /* DPAREN_ARITHMETIC || ARITH_FOR_COMMAND */

#if defined (COND_COMMAND)
static void
cond_error ()
{
  char *etext;

  if (EOF_Reached && cond_token != COND_ERROR)		/* [[ */
    parser_error (cond_lineno, _("unexpected EOF while looking for `]]'"));
  else if (cond_token != COND_ERROR)
    {
      if (etext = error_token_from_token (cond_token))
	{
	  parser_error (cond_lineno, _("syntax error in conditional expression: unexpected token `%s'"), etext);
	  free (etext);
	}
      else
	parser_error (cond_lineno, _("syntax error in conditional expression"));
    }
}

static COND_COM *
cond_expr ()
{
  return (cond_or ());  
}

static COND_COM *
cond_or ()
{
  COND_COM *l, *r;

  l = cond_and ();
  if (cond_token == OR_OR)
    {
      r = cond_or ();
      l = make_cond_node (COND_OR, (WORD_DESC *)NULL, l, r);
    }
  return l;
}

static COND_COM *
cond_and ()
{
  COND_COM *l, *r;

  l = cond_term ();
  if (cond_token == AND_AND)
    {
      r = cond_and ();
      l = make_cond_node (COND_AND, (WORD_DESC *)NULL, l, r);
    }
  return l;
}

static int
cond_skip_newlines ()
{
  while ((cond_token = read_token (READ)) == '\n')
    {
      if (SHOULD_PROMPT ())
	prompt_again (0);
    }
  return (cond_token);
}

#define COND_RETURN_ERROR() \
  do { cond_token = COND_ERROR; return ((COND_COM *)NULL); } while (0)

static COND_COM *
cond_term ()
{
  WORD_DESC *op;
  COND_COM *term, *tleft, *tright;
  int tok, lineno, local_extglob;
  char *etext;

  /* Read a token.  It can be a left paren, a `!', a unary operator, or a
     word that should be the first argument of a binary operator.  Start by
     skipping newlines, since this is a compound command. */
  tok = cond_skip_newlines ();
  lineno = line_number;
  if (tok == COND_END)
    {
      COND_RETURN_ERROR ();
    }
  else if (tok == '(')
    {
      term = cond_expr ();
      if (cond_token != ')')
	{
	  if (term)
	    dispose_cond_node (term);		/* ( */
	  if (etext = error_token_from_token (cond_token))
	    {
	      parser_error (lineno, _("unexpected token `%s', expected `)'"), etext);
	      free (etext);
	    }
	  else
	    parser_error (lineno, _("expected `)'"));
	  COND_RETURN_ERROR ();
	}
      term = make_cond_node (COND_EXPR, (WORD_DESC *)NULL, term, (COND_COM *)NULL);
      (void)cond_skip_newlines ();
    }
  else if (tok == BANG || (tok == WORD && (yylval.word->word[0] == '!' && yylval.word->word[1] == '\0')))
    {
      if (tok == WORD)
	dispose_word (yylval.word);	/* not needed */
      term = cond_term ();
      if (term)
	term->flags ^= CMD_INVERT_RETURN;
    }
  else if (tok == WORD && yylval.word->word[0] == '-' && yylval.word->word[1] && yylval.word->word[2] == 0 && test_unop (yylval.word->word))
    {
      op = yylval.word;
      tok = read_token (READ);
      if (tok == WORD)
	{
	  tleft = make_cond_node (COND_TERM, yylval.word, (COND_COM *)NULL, (COND_COM *)NULL);
	  term = make_cond_node (COND_UNARY, op, tleft, (COND_COM *)NULL);
	}
      else
	{
	  dispose_word (op);
	  if (etext = error_token_from_token (tok))
	    {
	      parser_error (line_number, _("unexpected argument `%s' to conditional unary operator"), etext);
	      free (etext);
	    }
	  else
	    parser_error (line_number, _("unexpected argument to conditional unary operator"));
	  COND_RETURN_ERROR ();
	}

      (void)cond_skip_newlines ();
    }
  else if (tok == WORD)		/* left argument to binary operator */
    {
      /* lhs */
      tleft = make_cond_node (COND_TERM, yylval.word, (COND_COM *)NULL, (COND_COM *)NULL);

      /* binop */
      /* tok = cond_skip_newlines (); ? */
      tok = read_token (READ);
      if (tok == WORD && test_binop (yylval.word->word))
	{
	  op = yylval.word;
	  if (op->word[0] == '=' && (op->word[1] == '\0' || (op->word[1] == '=' && op->word[2] == '\0')))
	    parser_state |= PST_EXTPAT;
	  else if (op->word[0] == '!' && op->word[1] == '=' && op->word[2] == '\0')
	    parser_state |= PST_EXTPAT;
	}
#if defined (COND_REGEXP)
      else if (tok == WORD && STREQ (yylval.word->word, "=~"))
	{
	  op = yylval.word;
	  parser_state |= PST_REGEXP;
	}
#endif
      else if (tok == '<' || tok == '>')
	op = make_word_from_token (tok);  /* ( */
      /* There should be a check before blindly accepting the `)' that we have
	 seen the opening `('. */
      else if (tok == COND_END || tok == AND_AND || tok == OR_OR || tok == ')')
	{
	  /* Special case.  [[ x ]] is equivalent to [[ -n x ]], just like
	     the test command.  Similarly for [[ x && expr ]] or
	     [[ x || expr ]] or [[ (x) ]]. */
	  op = make_word ("-n");
	  term = make_cond_node (COND_UNARY, op, tleft, (COND_COM *)NULL);
	  cond_token = tok;
	  return (term);
	}
      else
	{
	  if (etext = error_token_from_token (tok))
	    {
	      parser_error (line_number, _("unexpected token `%s', conditional binary operator expected"), etext);
	      free (etext);
	    }
	  else
	    parser_error (line_number, _("conditional binary operator expected"));
	  dispose_cond_node (tleft);
	  COND_RETURN_ERROR ();
	}

      /* rhs */
      local_extglob = extended_glob;
      if (parser_state & PST_EXTPAT)
	extended_glob = 1;
      tok = read_token (READ);
      if (parser_state & PST_EXTPAT)
	extended_glob = local_extglob;
      parser_state &= ~(PST_REGEXP|PST_EXTPAT);

      if (tok == WORD)
	{
	  tright = make_cond_node (COND_TERM, yylval.word, (COND_COM *)NULL, (COND_COM *)NULL);
	  term = make_cond_node (COND_BINARY, op, tleft, tright);
	}
      else
	{
	  if (etext = error_token_from_token (tok))
	    {
	      parser_error (line_number, _("unexpected argument `%s' to conditional binary operator"), etext);
	      free (etext);
	    }
	  else
	    parser_error (line_number, _("unexpected argument to conditional binary operator"));
	  dispose_cond_node (tleft);
	  dispose_word (op);
	  COND_RETURN_ERROR ();
	}

      (void)cond_skip_newlines ();
    }
  else
    {
      if (tok < 256)
	parser_error (line_number, _("unexpected token `%c' in conditional command"), tok);
      else if (etext = error_token_from_token (tok))
	{
	  parser_error (line_number, _("unexpected token `%s' in conditional command"), etext);
	  free (etext);
	}
      else
	parser_error (line_number, _("unexpected token %d in conditional command"), tok);
      COND_RETURN_ERROR ();
    }
  return (term);
}      

/* This is kind of bogus -- we slip a mini recursive-descent parser in
   here to handle the conditional statement syntax. */
static COMMAND *
parse_cond_command ()
{
  COND_COM *cexp;

  global_extglob = extended_glob;
  cexp = cond_expr ();
  return (make_cond_command (cexp));
}
#endif

#if defined (ARRAY_VARS)
/* When this is called, it's guaranteed that we don't care about anything
   in t beyond i.  We use a buffer with room for the characters we add just
   in case assignment() ends up doing something like parsing a command
   substitution that will reallocate atoken.  We don't want to write beyond
   the end of an allocated buffer. */
static int
token_is_assignment (t, i)
     char *t;
     int i;
{
  int r;
  char *atoken;

  atoken = xmalloc (i + 3);
  memcpy (atoken, t, i);
  atoken[i] = '=';
  atoken[i+1] = '\0';

  r = assignment (atoken, (parser_state & PST_COMPASSIGN) != 0);

  free (atoken);

  /* XXX - check that r == i to avoid returning false positive for
     t containing `=' before t[i]. */
  return (r > 0 && r == i);
}

/* XXX - possible changes here for `+=' */
static int
token_is_ident (t, i)
     char *t;
     int i;
{
  unsigned char c;
  int r;

  c = t[i];
  t[i] = '\0';
  r = legal_identifier (t);
  t[i] = c;
  return r;
}
#endif

static int
read_token_word (character)
     int character;
{
  /* The value for YYLVAL when a WORD is read. */
  WORD_DESC *the_word;

  /* Index into the token that we are building. */
  int token_index;

  /* ALL_DIGITS becomes zero when we see a non-digit. */
  int all_digit_token;

  /* DOLLAR_PRESENT becomes non-zero if we see a `$'. */
  int dollar_present;

  /* COMPOUND_ASSIGNMENT becomes non-zero if we are parsing a compound
     assignment. */
  int compound_assignment;

  /* QUOTED becomes non-zero if we see one of ("), ('), (`), or (\). */
  int quoted;

  /* Non-zero means to ignore the value of the next character, and just
     to add it no matter what. */
  int pass_next_character;

  /* The current delimiting character. */
  int cd;
  int result, peek_char;
  char *ttok, *ttrans;
  int ttoklen, ttranslen;
  intmax_t lvalue;

  if (token_buffer_size < TOKEN_DEFAULT_INITIAL_SIZE)
    token = (char *)xrealloc (token, token_buffer_size = TOKEN_DEFAULT_INITIAL_SIZE);

  token_index = 0;
  all_digit_token = DIGIT (character);
  dollar_present = quoted = pass_next_character = compound_assignment = 0;

  for (;;)
    {
      if (character == EOF)
	goto got_token;

      if (pass_next_character)
	{
	  pass_next_character = 0;
	  goto got_escaped_character;
	}

      cd = current_delimiter (dstack);

      /* Handle backslashes.  Quote lots of things when not inside of
	 double-quotes, quote some things inside of double-quotes. */
      if MBTEST(character == '\\')
	{
	  if (parser_state & PST_NOEXPAND)
	    {
	      pass_next_character++;
	      quoted = 1;
	      goto got_character;
	    }
	      
	  peek_char = shell_getc (0);

	  /* Backslash-newline is ignored in all cases except
	     when quoted with single quotes. */
	  if MBTEST(peek_char == '\n')
	    {
	      character = '\n';
	      goto next_character;
	    }
	  else
	    {
	      shell_ungetc (peek_char);

	      /* If the next character is to be quoted, note it now. */
	      if MBTEST(cd == 0 || cd == '`' ||
		  (cd == '"' && peek_char >= 0 && (sh_syntaxtab[peek_char] & CBSDQUOTE)))
		pass_next_character++;

	      quoted = 1;
	      goto got_character;
	    }
	}

      /* Parse a matched pair of quote characters. */
      if MBTEST(shellquote (character))
	{
	  push_delimiter (dstack, character);
	  ttok = parse_matched_pair (character, character, character, &ttoklen, (character == '`') ? P_COMMAND : 0);
	  pop_delimiter (dstack);
	  if (ttok == &matched_pair_error)
	    return -1;		/* Bail immediately. */
	  RESIZE_MALLOCED_BUFFER (token, token_index, ttoklen + 2,
				  token_buffer_size, TOKEN_DEFAULT_GROW_SIZE);
	  token[token_index++] = character;
	  strcpy (token + token_index, ttok);
	  token_index += ttoklen;
	  all_digit_token = 0;
	  if (character != '`')
	    quoted = 1;
	  dollar_present |= (character == '"' && strchr (ttok, '$') != 0);
	  FREE (ttok);
	  goto next_character;
	}

#ifdef COND_REGEXP
      /* When parsing a regexp as a single word inside a conditional command,
	 we need to special-case characters special to both the shell and
	 regular expressions.  Right now, that is only '(' and '|'. */ /*)*/
      if MBTEST((parser_state & PST_REGEXP) && (character == '(' || character == '|'))		/*)*/
	{
	  if (character == '|')
	    goto got_character;

	  push_delimiter (dstack, character);
	  ttok = parse_matched_pair (cd, '(', ')', &ttoklen, 0);
	  pop_delimiter (dstack);
	  if (ttok == &matched_pair_error)
	    return -1;		/* Bail immediately. */
	  RESIZE_MALLOCED_BUFFER (token, token_index, ttoklen + 2,
				  token_buffer_size, TOKEN_DEFAULT_GROW_SIZE);
	  token[token_index++] = character;
	  strcpy (token + token_index, ttok);
	  token_index += ttoklen;
	  FREE (ttok);
	  dollar_present = all_digit_token = 0;
	  goto next_character;
	}
#endif /* COND_REGEXP */

#ifdef EXTENDED_GLOB
      /* Parse a ksh-style extended pattern matching specification. */
      if MBTEST(extended_glob && PATTERN_CHAR (character))
	{
	  peek_char = shell_getc (1);
	  if MBTEST(peek_char == '(')		/* ) */
	    {
	      push_delimiter (dstack, peek_char);
	      ttok = parse_matched_pair (cd, '(', ')', &ttoklen, 0);
	      pop_delimiter (dstack);
	      if (ttok == &matched_pair_error)
		return -1;		/* Bail immediately. */
	      RESIZE_MALLOCED_BUFFER (token, token_index, ttoklen + 3,
				      token_buffer_size,
				      TOKEN_DEFAULT_GROW_SIZE);
	      token[token_index++] = character;
	      token[token_index++] = peek_char;
	      strcpy (token + token_index, ttok);
	      token_index += ttoklen;
	      FREE (ttok);
	      dollar_present = all_digit_token = 0;
	      goto next_character;
	    }
	  else
	    shell_ungetc (peek_char);
	}
#endif /* EXTENDED_GLOB */

      /* If the delimiter character is not single quote, parse some of
	 the shell expansions that must be read as a single word. */
      if MBTEST(shellexp (character))
	{
	  peek_char = shell_getc (1);
	  /* $(...), <(...), >(...), $((...)), ${...}, and $[...] constructs */
	  if MBTEST(peek_char == '(' ||
		((peek_char == '{' || peek_char == '[') && character == '$'))	/* ) ] } */
	    {
	      if (peek_char == '{')		/* } */
		ttok = parse_matched_pair (cd, '{', '}', &ttoklen, P_FIRSTCLOSE|P_DOLBRACE);
	      else if (peek_char == '(')		/* ) */
		{
		  /* XXX - push and pop the `(' as a delimiter for use by
		     the command-oriented-history code.  This way newlines
		     appearing in the $(...) string get added to the
		     history literally rather than causing a possibly-
		     incorrect `;' to be added. ) */
		  push_delimiter (dstack, peek_char);
		  ttok = parse_comsub (cd, '(', ')', &ttoklen, P_COMMAND);
		  pop_delimiter (dstack);
		}
	      else
		ttok = parse_matched_pair (cd, '[', ']', &ttoklen, 0);
	      if (ttok == &matched_pair_error)
		return -1;		/* Bail immediately. */
	      RESIZE_MALLOCED_BUFFER (token, token_index, ttoklen + 3,
				      token_buffer_size,
				      TOKEN_DEFAULT_GROW_SIZE);
	      token[token_index++] = character;
	      token[token_index++] = peek_char;
	      strcpy (token + token_index, ttok);
	      token_index += ttoklen;
	      FREE (ttok);
	      dollar_present = 1;
	      all_digit_token = 0;
	      goto next_character;
	    }
	  /* This handles $'...' and $"..." new-style quoted strings. */
#if defined (TRANSLATABLE_STRINGS)
	  else if MBTEST(character == '$' && (peek_char == '\'' || peek_char == '"'))
#else
	  else if MBTEST(character == '$' && peek_char == '\'')
#endif
	    {
	      int first_line;

	      first_line = line_number;
	      push_delimiter (dstack, peek_char);
	      ttok = parse_matched_pair (peek_char, peek_char, peek_char,
					 &ttoklen,
					 (peek_char == '\'') ? P_ALLOWESC : 0);
	      pop_delimiter (dstack);
	      if (ttok == &matched_pair_error)
		return -1;
	      if (peek_char == '\'')
		{
		  /* PST_NOEXPAND */
		  ttrans = ansiexpand (ttok, 0, ttoklen - 1, &ttranslen);
		  free (ttok);

		  /* Insert the single quotes and correctly quote any
		     embedded single quotes (allowed because P_ALLOWESC was
		     passed to parse_matched_pair). */
		  ttok = sh_single_quote (ttrans);
		  free (ttrans);
		  ttranslen = strlen (ttok);
		  ttrans = ttok;
		}
#if defined (TRANSLATABLE_STRINGS)
	      else
		{
		  /* PST_NOEXPAND */
		  /* Try to locale-expand the converted string. */
		  ttrans = locale_expand (ttok, 0, ttoklen - 1, first_line, &ttranslen);
		  free (ttok);

		  /* Add the double quotes back (or single quotes if the user
		     has set that option). */
		  if (singlequote_translations &&
		        ((ttoklen - 1) != ttranslen || STREQN (ttok, ttrans, ttranslen) == 0))
		    ttok = sh_single_quote (ttrans);
		  else
		    ttok = sh_mkdoublequoted (ttrans, ttranslen, 0);

		  free (ttrans);
		  ttrans = ttok;
		  ttranslen = strlen (ttrans);
		}
#endif /* TRANSLATABLE_STRINGS */

	      RESIZE_MALLOCED_BUFFER (token, token_index, ttranslen + 1,
				      token_buffer_size,
				      TOKEN_DEFAULT_GROW_SIZE);
	      strcpy (token + token_index, ttrans);
	      token_index += ttranslen;
	      FREE (ttrans);
	      quoted = 1;
	      all_digit_token = 0;
	      goto next_character;
	    }
	  /* This could eventually be extended to recognize all of the
	     shell's single-character parameter expansions, and set flags.*/
	  else if MBTEST(character == '$' && peek_char == '$')
	    {
	      RESIZE_MALLOCED_BUFFER (token, token_index, 3,
				      token_buffer_size,
				      TOKEN_DEFAULT_GROW_SIZE);
	      token[token_index++] = '$';
	      token[token_index++] = peek_char;
	      dollar_present = 1;
	      all_digit_token = 0;
	      goto next_character;
	    }
	  else
	    shell_ungetc (peek_char);
	}

#if defined (ARRAY_VARS)
      /* Identify possible array subscript assignment; match [...].  If
	 parser_state&PST_COMPASSIGN, we need to parse [sub]=words treating
	 `sub' as if it were enclosed in double quotes. */
      else if MBTEST(character == '[' &&		/* ] */
		     ((token_index > 0 && assignment_acceptable (last_read_token) && token_is_ident (token, token_index)) ||
		      (token_index == 0 && (parser_state&PST_COMPASSIGN))))
        {
	  ttok = parse_matched_pair (cd, '[', ']', &ttoklen, P_ARRAYSUB);
	  if (ttok == &matched_pair_error)
	    return -1;		/* Bail immediately. */
	  RESIZE_MALLOCED_BUFFER (token, token_index, ttoklen + 2,
				  token_buffer_size,
				  TOKEN_DEFAULT_GROW_SIZE);
	  token[token_index++] = character;
	  strcpy (token + token_index, ttok);
	  token_index += ttoklen;
	  FREE (ttok);
	  all_digit_token = 0;
	  goto next_character;
        }
      /* Identify possible compound array variable assignment. */
      else if MBTEST(character == '=' && token_index > 0 && (assignment_acceptable (last_read_token) || (parser_state & PST_ASSIGNOK)) && token_is_assignment (token, token_index))
	{
	  peek_char = shell_getc (1);
	  if MBTEST(peek_char == '(')		/* ) */
	    {
	      ttok = parse_compound_assignment (&ttoklen);

	      RESIZE_MALLOCED_BUFFER (token, token_index, ttoklen + 4,
				      token_buffer_size,
				      TOKEN_DEFAULT_GROW_SIZE);

	      token[token_index++] = '=';
	      token[token_index++] = '(';
	      if (ttok)
		{
		  strcpy (token + token_index, ttok);
		  token_index += ttoklen;
		}
	      token[token_index++] = ')';
	      FREE (ttok);
	      all_digit_token = 0;
	      compound_assignment = 1;
#if 1
	      goto next_character;
#else
	      goto got_token;		/* ksh93 seems to do this */
#endif
	    }
	  else
	    shell_ungetc (peek_char);
	}
#endif

      /* When not parsing a multi-character word construct, shell meta-
	 characters break words. */
      if MBTEST(shellbreak (character))
	{
	  shell_ungetc (character);
	  goto got_token;
	}

got_character:
      if MBTEST(character == CTLESC || character == CTLNUL)
	{
	  RESIZE_MALLOCED_BUFFER (token, token_index, 2, token_buffer_size,
				  TOKEN_DEFAULT_GROW_SIZE);
	  token[token_index++] = CTLESC;
	}
      else
got_escaped_character:
	RESIZE_MALLOCED_BUFFER (token, token_index, 1, token_buffer_size,
				TOKEN_DEFAULT_GROW_SIZE);

      token[token_index++] = character;

      all_digit_token &= DIGIT (character);
      dollar_present |= character == '$';

    next_character:
      if (character == '\n' && SHOULD_PROMPT ())
	prompt_again (0);

      /* We want to remove quoted newlines (that is, a \<newline> pair)
	 unless we are within single quotes or pass_next_character is
	 set (the shell equivalent of literal-next). */
      cd = current_delimiter (dstack);
      character = shell_getc (cd != '\'' && pass_next_character == 0);
    }	/* end for (;;) */

got_token:

  /* Calls to RESIZE_MALLOCED_BUFFER ensure there is sufficient room. */
  token[token_index] = '\0';

  /* Check to see what thing we should return.  If the last_read_token
     is a `<', or a `&', or the character which ended this token is
     a '>' or '<', then, and ONLY then, is this input token a NUMBER.
     Otherwise, it is just a word, and should be returned as such. */
  if MBTEST(all_digit_token && (character == '<' || character == '>' ||
		    last_read_token == LESS_AND ||
		    last_read_token == GREATER_AND))
      {
	if (legal_number (token, &lvalue) && (int)lvalue == lvalue)
	  {
	    yylval.number = lvalue;
	    return (NUMBER);
	  }
      }

  /* Check for special case tokens. */
  result = (last_shell_getc_is_singlebyte) ? special_case_tokens (token) : -1;
  if (result >= 0)
    return result;

#if defined (ALIAS)
  /* Posix.2 does not allow reserved words to be aliased, so check for all
     of them, including special cases, before expanding the current token
     as an alias. */
  if MBTEST(posixly_correct)
    CHECK_FOR_RESERVED_WORD (token);

  /* Aliases are expanded iff EXPAND_ALIASES is non-zero, and quoting
     inhibits alias expansion. */
  if (expand_aliases && quoted == 0)
    {
      result = alias_expand_token (token);
      if (result == RE_READ_TOKEN)
	return (RE_READ_TOKEN);
      else if (result == NO_EXPANSION)
	parser_state &= ~PST_ALEXPNEXT;
    }

  /* If not in Posix.2 mode, check for reserved words after alias
     expansion. */
  if MBTEST(posixly_correct == 0)
#endif
    CHECK_FOR_RESERVED_WORD (token);

  the_word = alloc_word_desc ();
  the_word->word = (char *)xmalloc (1 + token_index);
  the_word->flags = 0;
  strcpy (the_word->word, token);
  if (dollar_present)
    the_word->flags |= W_HASDOLLAR;
  if (quoted)
    the_word->flags |= W_QUOTED;		/*(*/
  if (compound_assignment && token[token_index-1] == ')')
    the_word->flags |= W_COMPASSIGN;
  /* A word is an assignment if it appears at the beginning of a
     simple command, or after another assignment word.  This is
     context-dependent, so it cannot be handled in the grammar. */
  if (assignment (token, (parser_state & PST_COMPASSIGN) != 0))
    {
      the_word->flags |= W_ASSIGNMENT;
      /* Don't perform word splitting on assignment statements. */
      if (assignment_acceptable (last_read_token) || (parser_state & PST_COMPASSIGN) != 0)
	{
	  the_word->flags |= W_NOSPLIT;
	  if (parser_state & PST_COMPASSIGN)
	    the_word->flags |= W_NOGLOB;	/* XXX - W_NOBRACE? */
	}
    }

  if (command_token_position (last_read_token))
    {
      struct builtin *b;
      b = builtin_address_internal (token, 0);
      if (b && (b->flags & ASSIGNMENT_BUILTIN))
	parser_state |= PST_ASSIGNOK;
      else if (STREQ (token, "eval") || STREQ (token, "let"))
	parser_state |= PST_ASSIGNOK;
    }

  yylval.word = the_word;

  /* should we check that quoted == 0 as well? */
  if MBTEST(token[0] == '{' && token[token_index-1] == '}' &&
      (character == '<' || character == '>'))
    {
      /* can use token; already copied to the_word */
      token[token_index-1] = '\0';
#if defined (ARRAY_VARS)
      if (legal_identifier (token+1) || valid_array_reference (token+1, 0))
#else
      if (legal_identifier (token+1))
#endif
	{
	  strcpy (the_word->word, token+1);
/* itrace("read_token_word: returning REDIR_WORD for %s", the_word->word); */
	  yylval.word = the_word;	/* accommodate recursive call */
	  return (REDIR_WORD);
	}
      else
        /* valid_array_reference can call the parser recursively; need to
	   make sure that yylval.word doesn't change if we are going to
	   return WORD or ASSIGNMENT_WORD */
        yylval.word = the_word;
    }

  result = ((the_word->flags & (W_ASSIGNMENT|W_NOSPLIT)) == (W_ASSIGNMENT|W_NOSPLIT))
		? ASSIGNMENT_WORD : WORD;

  switch (last_read_token)
    {
    case FUNCTION:
      parser_state |= PST_ALLOWOPNBRC;
      function_dstart = line_number;
      break;
    case CASE:
    case SELECT:
    case FOR:
      if (word_top < MAX_CASE_NEST)
	word_top++;
      word_lineno[word_top] = line_number;
      expecting_in_token++;
      break;
    }

  return (result);
}

/* Return 1 if TOKSYM is a token that after being read would allow
   a reserved word to be seen, else 0. */
static int
reserved_word_acceptable (toksym)
     int toksym;
{
  switch (toksym)
    {
    case '\n':
    case ';':
    case '(':
    case ')':
    case '|':
    case '&':
    case '{':
    case '}':		/* XXX */
    case AND_AND:
    case ARITH_CMD:
    case BANG:
    case BAR_AND:
    case COND_END:
    case DO:
    case DONE:
    case ELIF:
    case ELSE:
    case ESAC:
    case FI:
    case IF:
    case OR_OR:
    case SEMI_SEMI:
    case SEMI_AND:
    case SEMI_SEMI_AND:
    case THEN:
    case TIME:
    case TIMEOPT:
    case TIMEIGN:
    case COPROC:
    case UNTIL:
    case WHILE:
    case 0:
    case DOLPAREN:
      return 1;
    default:
#if defined (COPROCESS_SUPPORT)
      if (last_read_token == WORD && token_before_that == COPROC)
	return 1;
#endif
      if (last_read_token == WORD && token_before_that == FUNCTION)
	return 1;
      return 0;
    }
}
    
/* Return the index of TOKEN in the alist of reserved words, or -1 if
   TOKEN is not a shell reserved word. */
int
find_reserved_word (tokstr)
     char *tokstr;
{
  int i;
  for (i = 0; word_token_alist[i].word; i++)
    if (STREQ (tokstr, word_token_alist[i].word))
      return i;
  return -1;
}

/* An interface to let the rest of the shell (primarily the completion
   system) know what the parser is expecting. */
int
parser_in_command_position ()
{
  return (command_token_position (last_read_token));
}

#if 0
#if defined (READLINE)
/* Called after each time readline is called.  This insures that whatever
   the new prompt string is gets propagated to readline's local prompt
   variable. */
static void
reset_readline_prompt ()
{
  char *temp_prompt;

  if (prompt_string_pointer)
    {
      temp_prompt = (*prompt_string_pointer)
			? decode_prompt_string (*prompt_string_pointer)
			: (char *)NULL;

      if (temp_prompt == 0)
	{
	  temp_prompt = (char *)xmalloc (1);
	  temp_prompt[0] = '\0';
	}

      FREE (current_readline_prompt);
      current_readline_prompt = temp_prompt;
    }
}
#endif /* READLINE */
#endif /* 0 */

#if defined (HISTORY)
/* A list of tokens which can be followed by newlines, but not by
   semi-colons.  When concatenating multiple lines of history, the
   newline separator for such tokens is replaced with a space. */
static const int no_semi_successors[] = {
  '\n', '{', '(', ')', ';', '&', '|',
  CASE, DO, ELSE, IF, SEMI_SEMI, SEMI_AND, SEMI_SEMI_AND, THEN, UNTIL,
  WHILE, AND_AND, OR_OR, IN,
  0
};

/* If we are not within a delimited expression, try to be smart
   about which separators can be semi-colons and which must be
   newlines.  Returns the string that should be added into the
   history entry.  LINE is the line we're about to add; it helps
   make some more intelligent decisions in certain cases. */
char *
history_delimiting_chars (line)
     const char *line;
{
  static int last_was_heredoc = 0;	/* was the last entry the start of a here document? */
  register int i;

  if ((parser_state & PST_HEREDOC) == 0)
    last_was_heredoc = 0;

  if (dstack.delimiter_depth != 0)
    return ("\n");

  /* We look for current_command_line_count == 2 because we are looking to
     add the first line of the body of the here document (the second line
     of the command).  We also keep LAST_WAS_HEREDOC as a private sentinel
     variable to note when we think we added the first line of a here doc
     (the one with a "<<" somewhere in it) */
  if (parser_state & PST_HEREDOC)
    {
      if (last_was_heredoc)
	{
	  last_was_heredoc = 0;
	  return "\n";
	}
      return (here_doc_first_line ? "\n" : "");
    }

  if (parser_state & PST_COMPASSIGN)
    return (" ");

  /* First, handle some special cases. */
  /*(*/
  /* If we just read `()', assume it's a function definition, and don't
     add a semicolon.  If the token before the `)' was not `(', and we're
     not in the midst of parsing a case statement, assume it's a
     parenthesized command and add the semicolon. */
  /*)(*/
  if (token_before_that == ')')
    {
      if (two_tokens_ago == '(')	/*)*/	/* function def */
	return " ";
      /* This does not work for subshells inside case statement
	 command lists.  It's a suboptimal solution. */
      else if (parser_state & PST_CASESTMT)	/* case statement pattern */
	return " ";
      else
	return "; ";				/* (...) subshell */
    }
  else if (token_before_that == WORD && two_tokens_ago == FUNCTION)
    return " ";		/* function def using `function name' without `()' */

  /* If we're not in a here document, but we think we're about to parse one,
     and we would otherwise return a `;', return a newline to delimit the
     line with the here-doc delimiter */
  else if ((parser_state & PST_HEREDOC) == 0 && current_command_line_count > 1 && last_read_token == '\n' && strstr (line, "<<"))
    {
      last_was_heredoc = 1;
      return "\n";
    }
  else if ((parser_state & PST_HEREDOC) == 0 && current_command_line_count > 1 && need_here_doc > 0)
    return "\n";
  else if (token_before_that == WORD && two_tokens_ago == FOR)
    {
      /* Tricky.  `for i\nin ...' should not have a semicolon, but
	 `for i\ndo ...' should.  We do what we can. */
      for (i = shell_input_line_index; whitespace (shell_input_line[i]); i++)
	;
      if (shell_input_line[i] && shell_input_line[i] == 'i' && shell_input_line[i+1] == 'n')
	return " ";
      return ";";
    }
  else if (two_tokens_ago == CASE && token_before_that == WORD && (parser_state & PST_CASESTMT))
    return " ";

  for (i = 0; no_semi_successors[i]; i++)
    {
      if (token_before_that == no_semi_successors[i])
	return (" ");
    }

  /* Assume that by this point we are reading lines in a multi-line command.
     If we have multiple consecutive blank lines we want to return only one
     semicolon. */
  if (line_isblank (line))
    return (current_command_line_count > 1 && last_read_token == '\n' && token_before_that != '\n') ? "; " : "";

  return ("; ");
}
#endif /* HISTORY */

/* Issue a prompt, or prepare to issue a prompt when the next character
   is read. */
static void
prompt_again (force)
     int force;
{
  char *temp_prompt;

  if (interactive == 0 || expanding_alias ())	/* XXX */
    return;

  ps1_prompt = get_string_value ("PS1");
  ps2_prompt = get_string_value ("PS2");

  ps0_prompt = get_string_value ("PS0");

  if (!prompt_string_pointer)
    prompt_string_pointer = &ps1_prompt;

  temp_prompt = *prompt_string_pointer
			? decode_prompt_string (*prompt_string_pointer)
			: (char *)NULL;

  if (temp_prompt == 0)
    {
      temp_prompt = (char *)xmalloc (1);
      temp_prompt[0] = '\0';
    }

  current_prompt_string = *prompt_string_pointer;
  prompt_string_pointer = &ps2_prompt;

#if defined (READLINE)
  if (!no_line_editing)
    {
      FREE (current_readline_prompt);
      current_readline_prompt = temp_prompt;
    }
  else
#endif	/* READLINE */
    {
      FREE (current_decoded_prompt);
      current_decoded_prompt = temp_prompt;
    }
}

int
get_current_prompt_level ()
{
  return ((current_prompt_string && current_prompt_string == ps2_prompt) ? 2 : 1);
}

void
set_current_prompt_level (x)
     int x;
{
  prompt_string_pointer = (x == 2) ? &ps2_prompt : &ps1_prompt;
  current_prompt_string = *prompt_string_pointer;
}
      
static void
print_prompt ()
{
  fprintf (stderr, "%s", current_decoded_prompt);
  fflush (stderr);
}

#if defined (HISTORY)
  /* The history library increments the history offset as soon as it stores
     the first line of a potentially multi-line command, so we compensate
     here by returning one fewer when appropriate. */
static int
prompt_history_number (pmt)
     char *pmt;
{
  int ret;

  ret = history_number ();
  if (ret == 1)
    return ret;

  if (pmt == ps1_prompt)	/* are we expanding $PS1? */
    return ret;
  else if (pmt == ps2_prompt && command_oriented_history == 0)
    return ret;			/* not command oriented history */
  else if (pmt == ps2_prompt && command_oriented_history && current_command_first_line_saved)
    return ret - 1;
  else
    return ret - 1;		/* PS0, PS4, ${var@P}, PS2 other cases */
}
#endif

/* Return a string which will be printed as a prompt.  The string
   may contain special characters which are decoded as follows:

	\a	bell (ascii 07)
	\d	the date in Day Mon Date format
	\e	escape (ascii 033)
	\h	the hostname up to the first `.'
	\H	the hostname
	\j	the number of active jobs
	\l	the basename of the shell's tty device name
	\n	CRLF
	\r	CR
	\s	the name of the shell
	\t	the time in 24-hour hh:mm:ss format
	\T	the time in 12-hour hh:mm:ss format
	\@	the time in 12-hour hh:mm am/pm format
	\A	the time in 24-hour hh:mm format
	\D{fmt}	the result of passing FMT to strftime(3)
	\u	your username
	\v	the version of bash (e.g., 2.00)
	\V	the release of bash, version + patchlevel (e.g., 2.00.0)
	\w	the current working directory
	\W	the last element of $PWD
	\!	the history number of this command
	\#	the command number of this command
	\$	a $ or a # if you are root
	\nnn	character code nnn in octal
	\\	a backslash
	\[	begin a sequence of non-printing chars
	\]	end a sequence of non-printing chars
*/
#define PROMPT_GROWTH 48
char *
decode_prompt_string (string)
     char *string;
{
  WORD_LIST *list;
  char *result, *t, *orig_string;
  struct dstack save_dstack;
  int last_exit_value, last_comsub_pid;
#if defined (PROMPT_STRING_DECODE)
  size_t result_size;
  size_t result_index;
  int c, n, i;
  char *temp, *t_host, octal_string[4];
  struct tm *tm;  
  time_t the_time;
  char timebuf[128];
  char *timefmt;

  result = (char *)xmalloc (result_size = PROMPT_GROWTH);
  result[result_index = 0] = 0;
  temp = (char *)NULL;
  orig_string = string;

  while (c = *string++)
    {
      if (posixly_correct && c == '!')
	{
	  if (*string == '!')
	    {
	      temp = savestring ("!");
	      goto add_string;
	    }
	  else
	    {
#if !defined (HISTORY)
		temp = savestring ("1");
#else /* HISTORY */
		temp = itos (prompt_history_number (orig_string));
#endif /* HISTORY */
		string--;	/* add_string increments string again. */
		goto add_string;
	    }
	}
      if (c == '\\')
	{
	  c = *string;

	  switch (c)
	    {
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	      strncpy (octal_string, string, 3);
	      octal_string[3] = '\0';

	      n = read_octal (octal_string);
	      temp = (char *)xmalloc (3);

	      if (n == CTLESC || n == CTLNUL)
		{
		  temp[0] = CTLESC;
		  temp[1] = n;
		  temp[2] = '\0';
		}
	      else if (n == -1)
		{
		  temp[0] = '\\';
		  temp[1] = '\0';
		}
	      else
		{
		  temp[0] = n;
		  temp[1] = '\0';
		}

	      for (c = 0; n != -1 && c < 3 && ISOCTAL (*string); c++)
		string++;

	      c = 0;		/* tested at add_string: */
	      goto add_string;

	    case 'd':
	    case 't':
	    case 'T':
	    case '@':
	    case 'A':
	      /* Make the current time/date into a string. */
	      (void) time (&the_time);
#if defined (HAVE_TZSET)
	      sv_tz ("TZ");		/* XXX -- just make sure */
#endif
	      tm = localtime (&the_time);

	      if (c == 'd')
		n = strftime (timebuf, sizeof (timebuf), "%a %b %d", tm);
	      else if (c == 't')
		n = strftime (timebuf, sizeof (timebuf), "%H:%M:%S", tm);
	      else if (c == 'T')
		n = strftime (timebuf, sizeof (timebuf), "%I:%M:%S", tm);
	      else if (c == '@')
		n = strftime (timebuf, sizeof (timebuf), "%I:%M %p", tm);
	      else if (c == 'A')
		n = strftime (timebuf, sizeof (timebuf), "%H:%M", tm);

	      if (n == 0)
		timebuf[0] = '\0';
	      else
		timebuf[sizeof(timebuf) - 1] = '\0';

	      temp = savestring (timebuf);
	      goto add_string;

	    case 'D':		/* strftime format */
	      if (string[1] != '{')		/* } */
		goto not_escape;

	      (void) time (&the_time);
	      tm = localtime (&the_time);
	      string += 2;			/* skip { */
	      timefmt = xmalloc (strlen (string) + 3);
	      for (t = timefmt; *string && *string != '}'; )
		*t++ = *string++;
	      *t = '\0';
	      c = *string;	/* tested at add_string */
	      if (timefmt[0] == '\0')
		{
		  timefmt[0] = '%';
		  timefmt[1] = 'X';	/* locale-specific current time */
		  timefmt[2] = '\0';
		}
	      n = strftime (timebuf, sizeof (timebuf), timefmt, tm);
	      free (timefmt);

	      if (n == 0)
		timebuf[0] = '\0';
	      else
		timebuf[sizeof(timebuf) - 1] = '\0';

	      if (promptvars || posixly_correct)
		/* Make sure that expand_prompt_string is called with a
		   second argument of Q_DOUBLE_QUOTES if we use this
		   function here. */
		temp = sh_backslash_quote_for_double_quotes (timebuf, 0);
	      else
		temp = savestring (timebuf);
	      goto add_string;
	      
	    case 'n':
	      temp = (char *)xmalloc (3);
	      temp[0] = no_line_editing ? '\n' : '\r';
	      temp[1] = no_line_editing ? '\0' : '\n';
	      temp[2] = '\0';
	      goto add_string;

	    case 's':
	      temp = base_pathname (shell_name);
	      /* Try to quote anything the user can set in the file system */
	      if (promptvars || posixly_correct)
		{
		  char *t;
		  t = sh_strvis (temp);
		  temp = sh_backslash_quote_for_double_quotes (t, 0);
		  free (t);
		}
	      else
		temp = sh_strvis (temp);
	      goto add_string;

	    case 'v':
	    case 'V':
	      temp = (char *)xmalloc (16);
	      if (c == 'v')
		strcpy (temp, dist_version);
	      else
		sprintf (temp, "%s.%d", dist_version, patch_level);
	      goto add_string;

	    case 'w':
	    case 'W':
	      {
		/* Use the value of PWD because it is much more efficient. */
		char t_string[PATH_MAX];
		int tlen;

		temp = get_string_value ("PWD");

		if (temp == 0)
		  {
		    if (getcwd (t_string, sizeof(t_string)) == 0)
		      {
			t_string[0] = '.';
			tlen = 1;
		      }
		    else
		      tlen = strlen (t_string);
		  }
		else
		  {
		    tlen = sizeof (t_string) - 1;
		    strncpy (t_string, temp, tlen);
		  }
		t_string[tlen] = '\0';

#if defined (MACOSX)
		/* Convert from "fs" format to "input" format */
		temp = fnx_fromfs (t_string, strlen (t_string));
		if (temp != t_string)
		  strcpy (t_string, temp);
#endif

#define ROOT_PATH(x)	((x)[0] == '/' && (x)[1] == 0)
#define DOUBLE_SLASH_ROOT(x)	((x)[0] == '/' && (x)[1] == '/' && (x)[2] == 0)
		/* Abbreviate \W as ~ if $PWD == $HOME */
		if (c == 'W' && (((t = get_string_value ("HOME")) == 0) || STREQ (t, t_string) == 0))
		  {
		    if (ROOT_PATH (t_string) == 0 && DOUBLE_SLASH_ROOT (t_string) == 0)
		      {
			t = strrchr (t_string, '/');
			if (t)
			  memmove (t_string, t + 1, strlen (t));	/* strlen(t) to copy NULL */
		      }
		  }
#undef ROOT_PATH
#undef DOUBLE_SLASH_ROOT
		else
		  {
		    /* polite_directory_format is guaranteed to return a string
		       no longer than PATH_MAX - 1 characters. */
		    temp = polite_directory_format (t_string);
		    if (temp != t_string)
		      strcpy (t_string, temp);
		  }

		temp = trim_pathname (t_string, PATH_MAX - 1);
		/* If we're going to be expanding the prompt string later,
		   quote the directory name. */
		if (promptvars || posixly_correct)
		  /* Make sure that expand_prompt_string is called with a
		     second argument of Q_DOUBLE_QUOTES if we use this
		     function here. */
		  {
		    char *t;
		    t = sh_strvis (t_string);
		    temp = sh_backslash_quote_for_double_quotes (t, 0);
		    free (t);
		  }
		else
		  temp = sh_strvis (t_string);

		goto add_string;
	      }

	    case 'u':
	      if (current_user.user_name == 0)
		get_current_user_info ();
	      temp = savestring (current_user.user_name);
	      goto add_string;

	    case 'h':
	    case 'H':
	      t_host = savestring (current_host_name);
	      if (c == 'h' && (t = (char *)strchr (t_host, '.')))
		*t = '\0';
	      if (promptvars || posixly_correct)
		/* Make sure that expand_prompt_string is called with a
		   second argument of Q_DOUBLE_QUOTES if we use this
		   function here. */
		temp = sh_backslash_quote_for_double_quotes (t_host, 0);
	      else
		temp = savestring (t_host);
	      free (t_host);
	      goto add_string;

	    case '#':
	      n = current_command_number;
	      /* If we have already incremented current_command_number (PS4,
		 ${var@P}), compensate */
	      if (orig_string != ps0_prompt && orig_string != ps1_prompt && orig_string != ps2_prompt)
		n--;
	      temp = itos (n);
	      goto add_string;

	    case '!':
#if !defined (HISTORY)
	      temp = savestring ("1");
#else /* HISTORY */
	      temp = itos (prompt_history_number (orig_string));
#endif /* HISTORY */
	      goto add_string;

	    case '$':
	      t = temp = (char *)xmalloc (3);
	      if ((promptvars || posixly_correct) && (current_user.euid != 0))
		*t++ = '\\';
	      *t++ = current_user.euid == 0 ? '#' : '$';
	      *t = '\0';
	      goto add_string;

	    case 'j':
	      temp = itos (count_all_jobs ());
	      goto add_string;

	    case 'l':
#if defined (HAVE_TTYNAME)
	      temp = (char *)ttyname (fileno (stdin));
	      t = temp ? base_pathname (temp) : "tty";
	      temp = savestring (t);
#else
	      temp = savestring ("tty");
#endif /* !HAVE_TTYNAME */
	      goto add_string;

#if defined (READLINE)
	    case '[':
	    case ']':
	      if (no_line_editing)
		{
		  string++;
		  break;
		}
	      temp = (char *)xmalloc (3);
	      n = (c == '[') ? RL_PROMPT_START_IGNORE : RL_PROMPT_END_IGNORE;
	      i = 0;
	      if (n == CTLESC || n == CTLNUL)
		temp[i++] = CTLESC;
	      temp[i++] = n;
	      temp[i] = '\0';
	      goto add_string;
#endif /* READLINE */

	    case '\\':
	    case 'a':
	    case 'e':
	    case 'r':
	      temp = (char *)xmalloc (2);
	      if (c == 'a')
		temp[0] = '\07';
	      else if (c == 'e')
		temp[0] = '\033';
	      else if (c == 'r')
		temp[0] = '\r';
	      else			/* (c == '\\') */
	        temp[0] = c;
	      temp[1] = '\0';
	      goto add_string;

	    default:
not_escape:
	      temp = (char *)xmalloc (3);
	      temp[0] = '\\';
	      temp[1] = c;
	      temp[2] = '\0';

	    add_string:
	      if (c)
		string++;
	      result =
		sub_append_string (temp, result, &result_index, &result_size);
	      temp = (char *)NULL; /* Freed in sub_append_string (). */
	      result[result_index] = '\0';
	      break;
	    }
	}
      else
	{
	  RESIZE_MALLOCED_BUFFER (result, result_index, 3, result_size, PROMPT_GROWTH);
	  /* dequote_string should take care of removing this if we are not
	     performing the rest of the word expansions. */
	  if (c == CTLESC || c == CTLNUL)
	    result[result_index++] = CTLESC;
	  result[result_index++] = c;
	  result[result_index] = '\0';
	}
    }
#else /* !PROMPT_STRING_DECODE */
  result = savestring (string);
#endif /* !PROMPT_STRING_DECODE */

  /* Save the delimiter stack and point `dstack' to temp space so any
     command substitutions in the prompt string won't result in screwing
     up the parser's quoting state. */
  save_dstack = dstack;
  dstack = temp_dstack;
  dstack.delimiter_depth = 0;

  /* Perform variable and parameter expansion and command substitution on
     the prompt string. */
  if (promptvars || posixly_correct)
    {
      last_exit_value = last_command_exit_value;
      last_comsub_pid = last_command_subst_pid;
      list = expand_prompt_string (result, Q_DOUBLE_QUOTES, 0);
      free (result);
      result = string_list (list);
      dispose_words (list);
      last_command_exit_value = last_exit_value;
      last_command_subst_pid = last_comsub_pid;
    }
  else
    {
      t = dequote_string (result);
      free (result);
      result = t;
    }

  dstack = save_dstack;

  return (result);
}

/************************************************
 *						*
 *		ERROR HANDLING			*
 *						*
 ************************************************/

/* Report a syntax error, and restart the parser.  Call here for fatal
   errors. */
int
yyerror (msg)
     const char *msg;
{
  if ((parser_state & PST_NOERROR) == 0)
    report_syntax_error ((char *)NULL);
  reset_parser ();
  return (0);
}

static char *
error_token_from_token (tok)
     int tok;
{
  char *t;

  if (t = find_token_in_alist (tok, word_token_alist, 0))
    return t;

  if (t = find_token_in_alist (tok, other_token_alist, 0))
    return t;

  t = (char *)NULL;
  /* This stuff is dicy and needs closer inspection */
  switch (current_token)
    {
    case WORD:
    case ASSIGNMENT_WORD:
      if (yylval.word)
	t = savestring (yylval.word->word);
      break;
    case NUMBER:
      t = itos (yylval.number);
      break;
    case ARITH_CMD:
      if (yylval.word_list)
        t = string_list (yylval.word_list);
      break;
    case ARITH_FOR_EXPRS:
      if (yylval.word_list)
	t = string_list_internal (yylval.word_list, " ; ");
      break;
    case COND_CMD:
      t = (char *)NULL;		/* punt */
      break;
    }

  return t;
}

static char *
error_token_from_text ()
{
  char *msg, *t;
  int token_end, i;

  t = shell_input_line;
  i = shell_input_line_index;
  token_end = 0;
  msg = (char *)NULL;

  if (i && t[i] == '\0')
    i--;

  while (i && (whitespace (t[i]) || t[i] == '\n'))
    i--;

  if (i)
    token_end = i + 1;

  while (i && (member (t[i], " \n\t;|&") == 0))
    i--;

  while (i != token_end && (whitespace (t[i]) || t[i] == '\n'))
    i++;

  /* Return our idea of the offending token. */
  if (token_end || (i == 0 && token_end == 0))
    {
      if (token_end)
	msg = substring (t, i, token_end);
      else	/* one-character token */
	{
	  msg = (char *)xmalloc (2);
	  msg[0] = t[i];
	  msg[1] = '\0';
	}
    }

  return (msg);
}

static void
print_offending_line ()
{
  char *msg;
  int token_end;

  msg = savestring (shell_input_line);
  token_end = strlen (msg);
  while (token_end && msg[token_end - 1] == '\n')
    msg[--token_end] = '\0';

  parser_error (line_number, "`%s'", msg);
  free (msg);
}

/* Report a syntax error with line numbers, etc.
   Call here for recoverable errors.  If you have a message to print,
   then place it in MESSAGE, otherwise pass NULL and this will figure
   out an appropriate message for you. */
static void
report_syntax_error (message)
     char *message;
{
  char *msg, *p;

  if (message)
    {
      parser_error (line_number, "%s", message);
      if (interactive && EOF_Reached)
	EOF_Reached = 0;
      last_command_exit_value = (executing_builtin && parse_and_execute_level) ? EX_BADSYNTAX : EX_BADUSAGE;
      set_pipestatus_from_exit (last_command_exit_value);
      return;
    }

  /* If the line of input we're reading is not null, try to find the
     objectionable token.  First, try to figure out what token the
     parser's complaining about by looking at current_token. */
  if (current_token != 0 && EOF_Reached == 0 && (msg = error_token_from_token (current_token)))
    {
      if (ansic_shouldquote (msg))
	{
	  p = ansic_quote (msg, 0, NULL);
	  free (msg);
	  msg = p;
	}
      parser_error (line_number, _("syntax error near unexpected token `%s'"), msg);
      free (msg);

      if (interactive == 0)
	print_offending_line ();

      last_command_exit_value = (executing_builtin && parse_and_execute_level) ? EX_BADSYNTAX : EX_BADUSAGE;
      set_pipestatus_from_exit (last_command_exit_value);
      return;
    }

  /* If looking at the current token doesn't prove fruitful, try to find the
     offending token by analyzing the text of the input line near the current
     input line index and report what we find. */
  if (shell_input_line && *shell_input_line)
    {
      msg = error_token_from_text ();
      if (msg)
	{
	  parser_error (line_number, _("syntax error near `%s'"), msg);
	  free (msg);
	}

      /* If not interactive, print the line containing the error. */
      if (interactive == 0)
        print_offending_line ();
    }
  else
    {
      if (EOF_Reached && shell_eof_token && current_token != shell_eof_token)
	parser_error (line_number, _("unexpected EOF while looking for matching `%c'"), shell_eof_token);
      else
	{
	  msg = EOF_Reached ? _("syntax error: unexpected end of file") : _("syntax error");
	  parser_error (line_number, "%s", msg);
	}

      /* When the shell is interactive, this file uses EOF_Reached
	 only for error reporting.  Other mechanisms are used to
	 decide whether or not to exit. */
      if (interactive && EOF_Reached)
	EOF_Reached = 0;
    }

  last_command_exit_value = (executing_builtin && parse_and_execute_level) ? EX_BADSYNTAX : EX_BADUSAGE;
  set_pipestatus_from_exit (last_command_exit_value);
}

/* ??? Needed function. ??? We have to be able to discard the constructs
   created during parsing.  In the case of error, we want to return
   allocated objects to the memory pool.  In the case of no error, we want
   to throw away the information about where the allocated objects live.
   (dispose_command () will actually free the command.) */
static void
discard_parser_constructs (error_p)
     int error_p;
{
}

/************************************************
 *						*
 *		EOF HANDLING			*
 *						*
 ************************************************/

/* Do that silly `type "bye" to exit' stuff.  You know, "ignoreeof". */

/* A flag denoting whether or not ignoreeof is set. */
int ignoreeof = 0;

/* The number of times that we have encountered an EOF character without
   another character intervening.  When this gets above the limit, the
   shell terminates. */
int eof_encountered = 0;

/* The limit for eof_encountered. */
int eof_encountered_limit = 10;

/* If we have EOF as the only input unit, this user wants to leave
   the shell.  If the shell is not interactive, then just leave.
   Otherwise, if ignoreeof is set, and we haven't done this the
   required number of times in a row, print a message. */
static void
handle_eof_input_unit ()
{
  if (interactive)
    {
      /* shell.c may use this to decide whether or not to write out the
	 history, among other things.  We use it only for error reporting
	 in this file. */
      if (EOF_Reached)
	EOF_Reached = 0;

      /* If the user wants to "ignore" eof, then let her do so, kind of. */
      if (ignoreeof)
	{
	  if (eof_encountered < eof_encountered_limit)
	    {
	      fprintf (stderr, _("Use \"%s\" to leave the shell.\n"),
		       login_shell ? "logout" : "exit");
	      eof_encountered++;
	      /* Reset the parsing state. */
	      last_read_token = current_token = '\n';
	      /* Reset the prompt string to be $PS1. */
	      prompt_string_pointer = (char **)NULL;
	      prompt_again (0);
	      return;
	    }
	}

      /* In this case EOF should exit the shell.  Do it now. */
      reset_parser ();

      last_shell_builtin = this_shell_builtin;
      this_shell_builtin = exit_builtin;
      exit_builtin ((WORD_LIST *)NULL);
    }
  else
    {
      /* We don't write history files, etc., for non-interactive shells. */
      EOF_Reached = 1;
    }
}

/************************************************
 *						*
 *	STRING PARSING FUNCTIONS		*
 *						*
 ************************************************/

/* It's very important that these two functions treat the characters
   between ( and ) identically. */

static WORD_LIST parse_string_error;

/* Take a string and run it through the shell parser, returning the
   resultant word list.  Used by compound array assignment. */
WORD_LIST *
parse_string_to_word_list (s, flags, whom)
     char *s;
     int flags;
     const char *whom;
{
  WORD_LIST *wl;
  int tok, orig_current_token, orig_line_number;
  int orig_parser_state;
  sh_parser_state_t ps;
  int ea;

  orig_line_number = line_number;
  save_parser_state (&ps);

#if defined (HISTORY)
  bash_history_disable ();
#endif

  push_stream (1);
  if (ea = expanding_alias ())
    parser_save_alias ();

  /* WORD to avoid parsing reserved words as themselves and just parse them as
     WORDs. */
  last_read_token = WORD;

  current_command_line_count = 0;
  echo_input_at_read = expand_aliases = 0;

  with_input_from_string (s, whom);
  wl = (WORD_LIST *)NULL;

  if (flags & 1)
    {
      orig_parser_state = parser_state;		/* XXX - not needed? */
      /* State flags we don't want to persist into compound assignments. */
      parser_state &= ~PST_NOEXPAND;	/* parse_comsub sentinel */
      /* State flags we want to set for this run through the tokenizer. */
      parser_state |= PST_COMPASSIGN|PST_REPARSE;
    }

  while ((tok = read_token (READ)) != yacc_EOF)
    {
      if (tok == '\n' && *bash_input.location.string == '\0')
	break;
      if (tok == '\n')		/* Allow newlines in compound assignments */
	continue;
      if (tok != WORD && tok != ASSIGNMENT_WORD)
	{
	  line_number = orig_line_number + line_number - 1;
	  orig_current_token = current_token;
	  current_token = tok;
	  yyerror (NULL);	/* does the right thing */
	  current_token = orig_current_token;
	  if (wl)
	    dispose_words (wl);
	  wl = &parse_string_error;
	  break;
	}
      wl = make_word_list (yylval.word, wl);
    }
  
  last_read_token = '\n';
  pop_stream ();

  if (ea)
    parser_restore_alias ();

  restore_parser_state (&ps);

  if (flags & 1)
    parser_state = orig_parser_state;	/* XXX - not needed? */

  if (wl == &parse_string_error)
    {
      set_exit_status (EXECUTION_FAILURE);
      if (interactive_shell == 0 && posixly_correct)
	jump_to_top_level (FORCE_EOF);
      else
	jump_to_top_level (DISCARD);
    }

  return (REVERSE_LIST (wl, WORD_LIST *));
}

static char *
parse_compound_assignment (retlenp)
     int *retlenp;
{
  WORD_LIST *wl, *rl;
  int tok, orig_line_number, assignok;
  sh_parser_state_t ps;
  char *ret;

  orig_line_number = line_number;
  save_parser_state (&ps);

  /* WORD to avoid parsing reserved words as themselves and just parse them as
     WORDs. Plus it means we won't be in a command position and so alias
     expansion won't happen. */
  last_read_token = WORD;

  token = (char *)NULL;
  token_buffer_size = 0;
  wl = (WORD_LIST *)NULL;	/* ( */

  assignok = parser_state&PST_ASSIGNOK;		/* XXX */

  /* State flags we don't want to persist into compound assignments. */
  parser_state &= ~(PST_NOEXPAND|PST_CONDCMD|PST_CONDEXPR|PST_REGEXP|PST_EXTPAT);
  /* State flags we want to set for this run through the tokenizer. */
  parser_state |= PST_COMPASSIGN;

  esacs_needed_count = expecting_in_token = 0;

  while ((tok = read_token (READ)) != ')')
    {
      if (tok == '\n')			/* Allow newlines in compound assignments */
	{
	  if (SHOULD_PROMPT ())
	    prompt_again (0);
	  continue;
	}
      if (tok != WORD && tok != ASSIGNMENT_WORD)
	{
	  current_token = tok;	/* for error reporting */
	  if (tok == yacc_EOF)	/* ( */
	    parser_error (orig_line_number, _("unexpected EOF while looking for matching `)'"));
	  else
	    yyerror(NULL);	/* does the right thing */
	  if (wl)
	    dispose_words (wl);
	  wl = &parse_string_error;
	  break;
	}
      wl = make_word_list (yylval.word, wl);
    }

  restore_parser_state (&ps);

  if (wl == &parse_string_error)
    {
      set_exit_status (EXECUTION_FAILURE);
      last_read_token = '\n';	/* XXX */
      if (interactive_shell == 0 && posixly_correct)
	jump_to_top_level (FORCE_EOF);
      else
	jump_to_top_level (DISCARD);
    }

  if (wl)
    {
      rl = REVERSE_LIST (wl, WORD_LIST *);
      ret = string_list (rl);
      dispose_words (rl);
    }
  else
    ret = (char *)NULL;

  if (retlenp)
    *retlenp = (ret && *ret) ? strlen (ret) : 0;

  if (assignok)
    parser_state |= PST_ASSIGNOK;

  return ret;
}

/************************************************
 *						*
 *   SAVING AND RESTORING PARTIAL PARSE STATE   *
 *						*
 ************************************************/

sh_parser_state_t *
save_parser_state (ps)
     sh_parser_state_t *ps;
{
  if (ps == 0)
    ps = (sh_parser_state_t *)xmalloc (sizeof (sh_parser_state_t));
  if (ps == 0)
    return ((sh_parser_state_t *)NULL);

  ps->parser_state = parser_state;
  ps->token_state = save_token_state ();

  ps->input_line_terminator = shell_input_line_terminator;
  ps->eof_encountered = eof_encountered;
  ps->eol_lookahead = eol_ungetc_lookahead;

  ps->prompt_string_pointer = prompt_string_pointer;

  ps->current_command_line_count = current_command_line_count;

#if defined (HISTORY)
  ps->remember_on_history = remember_on_history;
#  if defined (BANG_HISTORY)
  ps->history_expansion_inhibited = history_expansion_inhibited;
#  endif
#endif

  ps->last_command_exit_value = last_command_exit_value;
#if defined (ARRAY_VARS)
  ps->pipestatus = save_pipestatus_array ();
#endif
    
  ps->last_shell_builtin = last_shell_builtin;
  ps->this_shell_builtin = this_shell_builtin;

  ps->expand_aliases = expand_aliases;
  ps->echo_input_at_read = echo_input_at_read;
  ps->need_here_doc = need_here_doc;
  ps->here_doc_first_line = here_doc_first_line;

  ps->esacs_needed = esacs_needed_count;
  ps->expecting_in = expecting_in_token;

  if (need_here_doc == 0)
    ps->redir_stack[0] = 0;
  else
    memcpy (ps->redir_stack, redir_stack, sizeof (redir_stack[0]) * HEREDOC_MAX);

#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  ps->pushed_strings = pushed_string_list;
#endif

  ps->eof_token = shell_eof_token;
  ps->token = token;
  ps->token_buffer_size = token_buffer_size;
  /* Force reallocation on next call to read_token_word */
  token = 0;
  token_buffer_size = 0;

  return (ps);
}

void
restore_parser_state (ps)
     sh_parser_state_t *ps;
{
  int i;

  if (ps == 0)
    return;

  parser_state = ps->parser_state;
  if (ps->token_state)
    {
      restore_token_state (ps->token_state);
      free (ps->token_state);
    }

  shell_input_line_terminator = ps->input_line_terminator;
  eof_encountered = ps->eof_encountered;
  eol_ungetc_lookahead = ps->eol_lookahead;

  prompt_string_pointer = ps->prompt_string_pointer;

  current_command_line_count = ps->current_command_line_count;

#if defined (HISTORY)
  remember_on_history = ps->remember_on_history;
#  if defined (BANG_HISTORY)
  history_expansion_inhibited = ps->history_expansion_inhibited;
#  endif
#endif

  last_command_exit_value = ps->last_command_exit_value;
#if defined (ARRAY_VARS)
  restore_pipestatus_array (ps->pipestatus);
#endif

  last_shell_builtin = ps->last_shell_builtin;
  this_shell_builtin = ps->this_shell_builtin;

  expand_aliases = ps->expand_aliases;
  echo_input_at_read = ps->echo_input_at_read;
  need_here_doc = ps->need_here_doc;
  here_doc_first_line = ps->here_doc_first_line;

  esacs_needed_count = ps->esacs_needed;
  expecting_in_token = ps->expecting_in;

#if 0
  for (i = 0; i < HEREDOC_MAX; i++)
    redir_stack[i] = ps->redir_stack[i];
#else
  if (need_here_doc == 0)
    redir_stack[0] = 0;
  else
    memcpy (redir_stack, ps->redir_stack, sizeof (redir_stack[0]) * HEREDOC_MAX);
#endif

#if defined (ALIAS) || defined (DPAREN_ARITHMETIC)
  pushed_string_list = (STRING_SAVER *)ps->pushed_strings;
#endif

  FREE (token);
  token = ps->token;
  token_buffer_size = ps->token_buffer_size;
  shell_eof_token = ps->eof_token;
}

sh_input_line_state_t *
save_input_line_state (ls)
     sh_input_line_state_t *ls;
{
  if (ls == 0)
    ls = (sh_input_line_state_t *)xmalloc (sizeof (sh_input_line_state_t));
  if (ls == 0)
    return ((sh_input_line_state_t *)NULL);

  ls->input_line = shell_input_line;
  ls->input_line_size = shell_input_line_size;
  ls->input_line_len = shell_input_line_len;
  ls->input_line_index = shell_input_line_index;

#if defined (HANDLE_MULTIBYTE)
  ls->input_property = shell_input_line_property;
  ls->input_propsize = shell_input_line_propsize;
#endif

  /* force reallocation */
  shell_input_line = 0;
  shell_input_line_size = shell_input_line_len = shell_input_line_index = 0;

#if defined (HANDLE_MULTIBYTE)
  shell_input_line_property = 0;
  shell_input_line_propsize = 0;
#endif

  return ls;
}

void
restore_input_line_state (ls)
     sh_input_line_state_t *ls;
{
  FREE (shell_input_line);
  shell_input_line = ls->input_line;
  shell_input_line_size = ls->input_line_size;
  shell_input_line_len = ls->input_line_len;
  shell_input_line_index = ls->input_line_index;

#if defined (HANDLE_MULTIBYTE)
  FREE (shell_input_line_property);
  shell_input_line_property = ls->input_property;
  shell_input_line_propsize = ls->input_propsize;
#endif

#if 0
  set_line_mbstate ();
#endif
}

/************************************************
 *						*
 *	MULTIBYTE CHARACTER HANDLING		*
 *						*
 ************************************************/

#if defined (HANDLE_MULTIBYTE)

/* We don't let the property buffer get larger than this unless the line is */
#define MAX_PROPSIZE 32768

static void
set_line_mbstate ()
{
  int c;
  size_t i, previ, len;
  mbstate_t mbs, prevs;
  size_t mbclen;
  int ilen;

  if (shell_input_line == NULL)
    return;
  len = STRLEN (shell_input_line);	/* XXX - shell_input_line_len ? */
  if (len == 0)
    return;
  if (shell_input_line_propsize >= MAX_PROPSIZE && len < MAX_PROPSIZE>>1)
    {
      free (shell_input_line_property);
      shell_input_line_property = 0;
      shell_input_line_propsize = 0;
    }
  if (len+1 > shell_input_line_propsize)
    {
      shell_input_line_propsize = len + 1;
      shell_input_line_property = (char *)xrealloc (shell_input_line_property, shell_input_line_propsize);
    }

  if (locale_mb_cur_max == 1)
    {
      memset (shell_input_line_property, 1, len);
      return;
    }

  /* XXX - use whether or not we are in a UTF-8 locale to avoid calls to
     mbrlen */
  if (locale_utf8locale == 0)
    memset (&prevs, '\0', sizeof (mbstate_t));

  for (i = previ = 0; i < len; i++)
    {
      if (locale_utf8locale == 0)
	mbs = prevs;

      c = shell_input_line[i];
      if (c == EOF)
	{
	  size_t j;
	  for (j = i; j < len; j++)
	    shell_input_line_property[j] = 1;
	  break;
	}

      if (locale_utf8locale)
	{
	  if ((unsigned char)shell_input_line[previ] < 128)	/* i != previ */
	    mbclen = 1;
	  else
	    {
	      ilen = utf8_mblen (shell_input_line + previ, i - previ + 1);
	      mbclen = (ilen == -1) ? (size_t)-1
				    : ((ilen == -2) ? (size_t)-2 : (size_t)ilen);
	    }
	}
      else
	mbclen = mbrlen (shell_input_line + previ, i - previ + 1, &mbs);

      if (mbclen == 1 || mbclen == (size_t)-1)
	{
	  mbclen = 1;
	  previ = i + 1;
	}
      else if (mbclen == (size_t)-2)
        mbclen = 0;
      else if (mbclen > 1)
	{
	  mbclen = 0;
	  previ = i + 1;
	  if (locale_utf8locale == 0)
	    prevs = mbs;
	}
      else
	{
	  size_t j;
	  for (j = i; j < len; j++)
	    shell_input_line_property[j] = 1;
	  break;
	}

      shell_input_line_property[i] = mbclen;
    }
}
#endif /* HANDLE_MULTIBYTE */
