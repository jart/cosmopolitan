/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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

#line 177 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
