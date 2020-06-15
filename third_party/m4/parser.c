#include "libc/limits.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar = (YYEMPTY))
#define yyerrok (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYPREFIX "yy"

/* clang-format off */
/* #line 2 "parser.y" */
/* $OpenBSD: parser.y,v 1.7 2012/04/12 17:00:11 espie Exp $ */
/*
 * Copyright (c) 2004 Marc Espie <espie@cvs.openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#define YYSTYPE	int32_t
extern int32_t end_result;
extern int yylex(void);
extern int yyerror(const char *);
/* #line 36 "parser.c" */
#define NUMBER 257
#define ERROR 258
#define LOR 259
#define LAND 260
#define EQ 261
#define NE 262
#define LE 263
#define GE 264
#define LSHIFT 265
#define RSHIFT 266
#define EXPONENT 267
#define UMINUS 268
#define UPLUS 269
#define YYERRCODE 256
const short yylhs[] =
	{                                        -1,
    0,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,
};
const short yylen[] =
	{                                         2,
    1,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    2,    2,    2,    2,    1,
};
const short yydefred[] =
	{                                      0,
   26,    0,    0,    0,    0,    0,    0,    0,   23,   22,
   24,   25,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   21,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,
};
const short yydgoto[] =
	{                                       7,
    8,
};
const short yysindex[] =
	{                                     95,
    0,   95,   95,   95,   95,   95,    0,  397,    0,    0,
    0,    0,  383,   95,   95,   95,   95,   95,   95,   95,
   95,   95,   95,   95,   95,   95,   95,   95,   95,   95,
   95,   95,    0,  428,  471,  482,  185,  437,  493,  493,
  -10,  -10,  -10,  -10,  -23,  -23,  -34,  -34, -267, -267,
 -267, -267,};
const short yyrindex[] =
	{                                      0,
    0,    0,    0,    0,    0,    0,    0,    2,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   11,   62,   23,  101,  308,  201,  243,
  124,  130,  144,  155,   79,  116,   51,   67,    1,   12,
   28,   40,};
const short yygindex[] =
	{                                      0,
  582,
};
#define YYTABLESIZE 760
const short yytable[] =
	{                                      32,
    5,    1,   31,    0,    0,    0,    0,   29,    0,    0,
   20,    6,   30,   31,    0,    0,    0,    0,   29,   27,
    0,   28,   18,   30,    0,    0,   31,    7,    0,    0,
    0,   29,   27,    0,   28,    0,   30,    5,    5,    4,
    0,    5,    5,    5,    0,    5,    0,    5,    6,    6,
    2,   20,    6,    6,    6,    0,    6,    0,    6,    0,
    5,   19,    5,   18,    7,    7,    3,    0,    7,    7,
    7,    6,    7,    6,    7,    0,    4,    4,    8,    0,
    4,    4,    4,    0,    4,    0,    4,    7,    2,    7,
    0,    2,    0,    2,    5,    2,    0,    0,    0,    4,
   17,    4,   19,    0,    3,    6,    0,    3,    0,    3,
    2,    3,    2,    0,    0,    9,    8,    0,    0,    8,
    0,    7,    0,   10,    5,    0,    3,    4,    3,   12,
    0,    0,    0,    4,    6,    6,    0,    2,    8,    3,
    8,   17,    0,   11,    2,    0,   18,    0,    0,    0,
    0,    7,    0,    9,   13,    0,    9,    0,    0,    0,
    3,   10,    0,    4,   10,    0,    0,   12,    0,    0,
   12,    0,    8,    0,    2,    9,    0,    9,    0,    0,
    0,   11,    0,   10,   11,   10,    0,    0,    0,   12,
    3,   12,   13,    0,   17,   13,    0,    0,    0,    0,
   14,    0,    8,   11,    0,   11,    0,    0,    0,    9,
    0,    0,    0,    0,   13,    0,   13,   10,    0,    0,
    5,   31,   18,   12,   17,    0,   29,   27,    0,   28,
    0,   30,   32,    0,    0,    0,    0,   11,   14,    9,
    0,   14,   15,   32,   21,    0,   23,   10,   13,    0,
    0,    0,    0,   12,   25,   26,   32,    0,    0,    5,
    5,    5,    5,    5,    5,    5,    5,   11,    0,   20,
    6,    6,    6,    6,    6,    6,    6,    6,   13,    0,
   15,   18,   18,   15,    0,    0,    7,    7,    7,    7,
    7,    7,    7,    7,   14,    0,    0,    0,    4,    4,
    4,    4,    4,    4,    4,    4,    0,   16,    0,    2,
    2,    2,    2,    2,    2,    2,    2,    0,    0,    0,
   19,   19,    0,    0,   14,    3,    3,    3,    3,    3,
    3,    3,    3,    0,    0,    0,   15,    8,    8,    8,
    8,    8,    8,    8,    8,   16,    0,    0,   16,    0,
    0,    1,    0,    0,    0,    0,    0,    0,    0,   17,
   17,    0,    0,    0,    0,    0,   15,    0,    0,    0,
    0,    0,    0,    0,    9,    9,    9,    9,    9,    9,
    9,    9,   10,   10,   10,   10,   10,   10,   12,   12,
   12,   12,   12,   12,    0,    0,    0,    0,    0,    0,
    0,   16,   11,   11,   11,   11,   11,   11,    0,    0,
    0,    0,    0,   13,   13,   13,   13,   13,   13,   31,
   18,    0,    0,   33,   29,   27,    0,   28,    0,   30,
    0,   16,    0,   31,   18,    0,    0,    0,   29,   27,
    0,   28,   21,   30,   23,   19,   20,   22,   24,   25,
   26,   32,    0,    0,    0,    0,   21,    0,   23,   14,
   14,   14,   14,    0,   31,   18,    0,    0,    0,   29,
   27,    0,   28,   31,   30,    0,   17,    0,   29,   27,
    0,   28,    0,   30,    0,    0,    0,   21,    0,   23,
   17,    0,    0,    0,    0,    0,   21,    0,   23,    0,
    0,   15,   15,   15,   15,    0,   16,   31,   18,    0,
    0,    0,   29,   27,    0,   28,    0,   30,   31,   18,
   16,   17,    0,   29,   27,    0,   28,    0,   30,   31,
   21,    0,   23,    0,   29,   27,    0,   28,    0,   30,
    0,   21,    0,   23,    0,    0,    0,    0,    0,    0,
    0,   16,   21,    0,   23,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   17,    0,   16,   16,    0,    0,
    0,    0,    0,    0,    0,   17,    0,    0,    0,    0,
    0,    0,    0,    9,   10,   11,   12,   13,    0,    0,
    0,    0,    0,    0,   16,   34,   35,   36,   37,   38,
   39,   40,   41,   42,   43,   44,   45,   46,   47,   48,
   49,   50,   51,   52,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   14,   15,   19,   20,   22,   24,   25,   26,   32,
    0,    0,    0,    0,    0,   14,   15,   19,   20,   22,
   24,   25,   26,   32,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   15,   19,   20,
   22,   24,   25,   26,   32,    0,    0,   19,   20,   22,
   24,   25,   26,   32,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   19,   20,   22,   24,   25,   26,   32,    0,    0,
    0,    0,   19,   20,   22,   24,   25,   26,   32,    0,
    0,    0,    0,    0,    0,   22,   24,   25,   26,   32,
};
const short yycheck[] =
	{                                     267,
    0,    0,   37,   -1,   -1,   -1,   -1,   42,   -1,   -1,
    0,    0,   47,   37,   -1,   -1,   -1,   -1,   42,   43,
   -1,   45,    0,   47,   -1,   -1,   37,    0,   -1,   -1,
   -1,   42,   43,   -1,   45,   -1,   47,   37,   38,    0,
   -1,   41,   42,   43,   -1,   45,   -1,   47,   37,   38,
    0,   41,   41,   42,   43,   -1,   45,   -1,   47,   -1,
   60,    0,   62,   41,   37,   38,    0,   -1,   41,   42,
   43,   60,   45,   62,   47,   -1,   37,   38,    0,   -1,
   41,   42,   43,   -1,   45,   -1,   47,   60,   38,   62,
   -1,   41,   -1,   43,   94,   45,   -1,   -1,   -1,   60,
    0,   62,   41,   -1,   38,   94,   -1,   41,   -1,   43,
   60,   45,   62,   -1,   -1,    0,   38,   -1,   -1,   41,
   -1,   94,   -1,    0,  124,   -1,   60,   33,   62,    0,
   -1,   -1,   -1,   94,   40,  124,   -1,   43,   60,   45,
   62,   41,   -1,    0,   94,   -1,  124,   -1,   -1,   -1,
   -1,  124,   -1,   38,    0,   -1,   41,   -1,   -1,   -1,
   94,   38,   -1,  124,   41,   -1,   -1,   38,   -1,   -1,
   41,   -1,   94,   -1,  124,   60,   -1,   62,   -1,   -1,
   -1,   38,   -1,   60,   41,   62,   -1,   -1,   -1,   60,
  124,   62,   38,   -1,   94,   41,   -1,   -1,   -1,   -1,
    0,   -1,  124,   60,   -1,   62,   -1,   -1,   -1,   94,
   -1,   -1,   -1,   -1,   60,   -1,   62,   94,   -1,   -1,
  126,   37,   38,   94,  124,   -1,   42,   43,   -1,   45,
   -1,   47,  267,   -1,   -1,   -1,   -1,   94,   38,  124,
   -1,   41,    0,  267,   60,   -1,   62,  124,   94,   -1,
   -1,   -1,   -1,  124,  265,  266,  267,   -1,   -1,  259,
  260,  261,  262,  263,  264,  265,  266,  124,   -1,  259,
  259,  260,  261,  262,  263,  264,  265,  266,  124,   -1,
   38,  259,  260,   41,   -1,   -1,  259,  260,  261,  262,
  263,  264,  265,  266,   94,   -1,   -1,   -1,  259,  260,
  261,  262,  263,  264,  265,  266,   -1,    0,   -1,  259,
  260,  261,  262,  263,  264,  265,  266,   -1,   -1,   -1,
  259,  260,   -1,   -1,  124,  259,  260,  261,  262,  263,
  264,  265,  266,   -1,   -1,   -1,   94,  259,  260,  261,
  262,  263,  264,  265,  266,   38,   -1,   -1,   41,   -1,
   -1,  257,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  259,
  260,   -1,   -1,   -1,   -1,   -1,  124,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  259,  260,  261,  262,  263,  264,
  265,  266,  259,  260,  261,  262,  263,  264,  259,  260,
  261,  262,  263,  264,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   94,  259,  260,  261,  262,  263,  264,   -1,   -1,
   -1,   -1,   -1,  259,  260,  261,  262,  263,  264,   37,
   38,   -1,   -1,   41,   42,   43,   -1,   45,   -1,   47,
   -1,  124,   -1,   37,   38,   -1,   -1,   -1,   42,   43,
   -1,   45,   60,   47,   62,  261,  262,  263,  264,  265,
  266,  267,   -1,   -1,   -1,   -1,   60,   -1,   62,  259,
  260,  261,  262,   -1,   37,   38,   -1,   -1,   -1,   42,
   43,   -1,   45,   37,   47,   -1,   94,   -1,   42,   43,
   -1,   45,   -1,   47,   -1,   -1,   -1,   60,   -1,   62,
   94,   -1,   -1,   -1,   -1,   -1,   60,   -1,   62,   -1,
   -1,  259,  260,  261,  262,   -1,  124,   37,   38,   -1,
   -1,   -1,   42,   43,   -1,   45,   -1,   47,   37,   38,
  124,   94,   -1,   42,   43,   -1,   45,   -1,   47,   37,
   60,   -1,   62,   -1,   42,   43,   -1,   45,   -1,   47,
   -1,   60,   -1,   62,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  124,   60,   -1,   62,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   94,   -1,  259,  260,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   94,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,    2,    3,    4,    5,    6,   -1,   -1,
   -1,   -1,   -1,   -1,  124,   14,   15,   16,   17,   18,
   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
   29,   30,   31,   32,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  259,  260,  261,  262,  263,  264,  265,  266,  267,
   -1,   -1,   -1,   -1,   -1,  259,  260,  261,  262,  263,
  264,  265,  266,  267,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  260,  261,  262,
  263,  264,  265,  266,  267,   -1,   -1,  261,  262,  263,
  264,  265,  266,  267,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  261,  262,  263,  264,  265,  266,  267,   -1,   -1,
   -1,   -1,  261,  262,  263,  264,  265,  266,  267,   -1,
   -1,   -1,   -1,   -1,   -1,  263,  264,  265,  266,  267,
};
#define YYFINAL 7
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 269
#if YYDEBUG
const char * const yyname[] =
	{
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,"'%'","'&'",0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,
0,0,0,0,0,"'<'",0,"'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'|'",0,
"'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,"NUMBER","ERROR","LOR","LAND","EQ","NE","LE","GE",
"LSHIFT","RSHIFT","EXPONENT","UMINUS","UPLUS",
};
const char * const yyrule[] =
	{"$accept : top",
"top : expr",
"expr : expr '+' expr",
"expr : expr '-' expr",
"expr : expr EXPONENT expr",
"expr : expr '*' expr",
"expr : expr '/' expr",
"expr : expr '%' expr",
"expr : expr LSHIFT expr",
"expr : expr RSHIFT expr",
"expr : expr '<' expr",
"expr : expr '>' expr",
"expr : expr LE expr",
"expr : expr GE expr",
"expr : expr EQ expr",
"expr : expr NE expr",
"expr : expr '&' expr",
"expr : expr '^' expr",
"expr : expr '|' expr",
"expr : expr LAND expr",
"expr : expr LOR expr",
"expr : '(' expr ')'",
"expr : '-' expr",
"expr : '+' expr",
"expr : '!' expr",
"expr : '~' expr",
"expr : NUMBER",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
/* LINTUSED */
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
unsigned int yystacksize;
int yyparse(void);
/* #line 83 "parser.y" */

/* #line 326 "parser.c" */
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(void)
{
    unsigned int newsize;
    long sslen;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    sslen = yyssp - yyss;
#ifdef SIZE_MAX
#define YY_SIZE_MAX SIZE_MAX
#else
#define YY_SIZE_MAX 0xffffffffU
#endif
    if (newsize && YY_SIZE_MAX / newsize < sizeof *newss)
        goto bail;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss); /* overflow check above */
    if (newss == NULL)
        goto bail;
    yyss = newss;
    yyssp = newss + sslen;
    if (newsize && YY_SIZE_MAX / newsize < sizeof *newvs)
        goto bail;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs); /* overflow check above */
    if (newvs == NULL)
        goto bail;
    yyvs = newvs;
    yyvsp = newvs + sslen;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
bail:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return -1;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse(void)
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif /* YYDEBUG */

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yyvsp[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
/* #line 42 "parser.y" */
{ end_result = yyvsp[0]; }
break;
case 2:
/* #line 44 "parser.y" */
{ yyval = yyvsp[-2] + yyvsp[0]; }
break;
case 3:
/* #line 45 "parser.y" */
{ yyval = yyvsp[-2] - yyvsp[0]; }
break;
case 4:
/* #line 46 "parser.y" */
{ yyval = pow(yyvsp[-2], yyvsp[0]); }
break;
case 5:
/* #line 47 "parser.y" */
{ yyval = yyvsp[-2] * yyvsp[0]; }
break;
case 6:
/* #line 48 "parser.y" */
{
		if (yyvsp[0] == 0) {
			yyerror("division by zero");
			exit(1);
		}
		yyval = yyvsp[-2] / yyvsp[0];
	}
break;
case 7:
/* #line 55 "parser.y" */
{ 
		if (yyvsp[0] == 0) {
			yyerror("modulo zero");
			exit(1);
		}
		yyval = yyvsp[-2] % yyvsp[0];
	}
break;
case 8:
/* #line 62 "parser.y" */
{ yyval = yyvsp[-2] << yyvsp[0]; }
break;
case 9:
/* #line 63 "parser.y" */
{ yyval = yyvsp[-2] >> yyvsp[0]; }
break;
case 10:
/* #line 64 "parser.y" */
{ yyval = yyvsp[-2] < yyvsp[0]; }
break;
case 11:
/* #line 65 "parser.y" */
{ yyval = yyvsp[-2] > yyvsp[0]; }
break;
case 12:
/* #line 66 "parser.y" */
{ yyval = yyvsp[-2] <= yyvsp[0]; }
break;
case 13:
/* #line 67 "parser.y" */
{ yyval = yyvsp[-2] >= yyvsp[0]; }
break;
case 14:
/* #line 68 "parser.y" */
{ yyval = yyvsp[-2] == yyvsp[0]; }
break;
case 15:
/* #line 69 "parser.y" */
{ yyval = yyvsp[-2] != yyvsp[0]; }
break;
case 16:
/* #line 70 "parser.y" */
{ yyval = yyvsp[-2] & yyvsp[0]; }
break;
case 17:
/* #line 71 "parser.y" */
{ yyval = yyvsp[-2] ^ yyvsp[0]; }
break;
case 18:
/* #line 72 "parser.y" */
{ yyval = yyvsp[-2] | yyvsp[0]; }
break;
case 19:
/* #line 73 "parser.y" */
{ yyval = yyvsp[-2] && yyvsp[0]; }
break;
case 20:
/* #line 74 "parser.y" */
{ yyval = yyvsp[-2] || yyvsp[0]; }
break;
case 21:
/* #line 75 "parser.y" */
{ yyval = yyvsp[-1]; }
break;
case 22:
/* #line 76 "parser.y" */
{ yyval = -yyvsp[0]; }
break;
case 23:
/* #line 77 "parser.y" */
{ yyval = yyvsp[0]; }
break;
case 24:
/* #line 78 "parser.y" */
{ yyval = !yyvsp[0]; }
break;
case 25:
/* #line 79 "parser.y" */
{ yyval = ~yyvsp[0]; }
break;
/* #line 632 "parser.c" */
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return (1);
yyaccept:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return (0);
}
