#ifndef YY_YY_AWKGRAM_TAB_H_INCLUDED
# define YY_YY_AWKGRAM_TAB_H_INCLUDED
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
    FIRSTTOKEN = 258,              /* FIRSTTOKEN  */
    PROGRAM = 259,                 /* PROGRAM  */
    PASTAT = 260,                  /* PASTAT  */
    PASTAT2 = 261,                 /* PASTAT2  */
    XBEGIN = 262,                  /* XBEGIN  */
    XEND = 263,                    /* XEND  */
    NL = 264,                      /* NL  */
    ARRAY = 265,                   /* ARRAY  */
    MATCH = 266,                   /* MATCH  */
    NOTMATCH = 267,                /* NOTMATCH  */
    MATCHOP = 268,                 /* MATCHOP  */
    FINAL = 269,                   /* FINAL  */
    DOT = 270,                     /* DOT  */
    ALL = 271,                     /* ALL  */
    CCL = 272,                     /* CCL  */
    NCCL = 273,                    /* NCCL  */
    CHAR = 274,                    /* CHAR  */
    OR = 275,                      /* OR  */
    STAR = 276,                    /* STAR  */
    QUEST = 277,                   /* QUEST  */
    PLUS = 278,                    /* PLUS  */
    EMPTYRE = 279,                 /* EMPTYRE  */
    ZERO = 280,                    /* ZERO  */
    AND = 281,                     /* AND  */
    BOR = 282,                     /* BOR  */
    APPEND = 283,                  /* APPEND  */
    EQ = 284,                      /* EQ  */
    GE = 285,                      /* GE  */
    GT = 286,                      /* GT  */
    LE = 287,                      /* LE  */
    LT = 288,                      /* LT  */
    NE = 289,                      /* NE  */
    IN = 290,                      /* IN  */
    ARG = 291,                     /* ARG  */
    BLTIN = 292,                   /* BLTIN  */
    BREAK = 293,                   /* BREAK  */
    CLOSE = 294,                   /* CLOSE  */
    CONTINUE = 295,                /* CONTINUE  */
    DELETE = 296,                  /* DELETE  */
    DO = 297,                      /* DO  */
    EXIT = 298,                    /* EXIT  */
    FOR = 299,                     /* FOR  */
    FUNC = 300,                    /* FUNC  */
    SUB = 301,                     /* SUB  */
    GSUB = 302,                    /* GSUB  */
    IF = 303,                      /* IF  */
    INDEX = 304,                   /* INDEX  */
    LSUBSTR = 305,                 /* LSUBSTR  */
    MATCHFCN = 306,                /* MATCHFCN  */
    NEXT = 307,                    /* NEXT  */
    NEXTFILE = 308,                /* NEXTFILE  */
    ADD = 309,                     /* ADD  */
    MINUS = 310,                   /* MINUS  */
    MULT = 311,                    /* MULT  */
    DIVIDE = 312,                  /* DIVIDE  */
    MOD = 313,                     /* MOD  */
    ASSIGN = 314,                  /* ASSIGN  */
    ASGNOP = 315,                  /* ASGNOP  */
    ADDEQ = 316,                   /* ADDEQ  */
    SUBEQ = 317,                   /* SUBEQ  */
    MULTEQ = 318,                  /* MULTEQ  */
    DIVEQ = 319,                   /* DIVEQ  */
    MODEQ = 320,                   /* MODEQ  */
    POWEQ = 321,                   /* POWEQ  */
    PRINT = 322,                   /* PRINT  */
    PRINTF = 323,                  /* PRINTF  */
    SPRINTF = 324,                 /* SPRINTF  */
    ELSE = 325,                    /* ELSE  */
    INTEST = 326,                  /* INTEST  */
    CONDEXPR = 327,                /* CONDEXPR  */
    POSTINCR = 328,                /* POSTINCR  */
    PREINCR = 329,                 /* PREINCR  */
    POSTDECR = 330,                /* POSTDECR  */
    PREDECR = 331,                 /* PREDECR  */
    VAR = 332,                     /* VAR  */
    IVAR = 333,                    /* IVAR  */
    VARNF = 334,                   /* VARNF  */
    CALL = 335,                    /* CALL  */
    NUMBER = 336,                  /* NUMBER  */
    STRING = 337,                  /* STRING  */
    REGEXPR = 338,                 /* REGEXPR  */
    GETLINE = 339,                 /* GETLINE  */
    RETURN = 340,                  /* RETURN  */
    SPLIT = 341,                   /* SPLIT  */
    SUBSTR = 342,                  /* SUBSTR  */
    WHILE = 343,                   /* WHILE  */
    CAT = 344,                     /* CAT  */
    NOT = 345,                     /* NOT  */
    UMINUS = 346,                  /* UMINUS  */
    UPLUS = 347,                   /* UPLUS  */
    POWER = 348,                   /* POWER  */
    DECR = 349,                    /* DECR  */
    INCR = 350,                    /* INCR  */
    INDIRECT = 351,                /* INDIRECT  */
    LASTTOKEN = 352                /* LASTTOKEN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 41 "awkgram.y"

	Node	*p;
	Cell	*cp;
	int	i;
	char	*s;

#line 168 "awkgram.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_AWKGRAM_TAB_H_INCLUDED  */
