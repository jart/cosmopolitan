#ifndef COSMOPOLITAN_THIRD_PARTY_AWK_AWKGRAM_TAB_H_
#define COSMOPOLITAN_THIRD_PARTY_AWK_AWKGRAM_TAB_H_
#include "third_party/awk/awk.h"
COSMOPOLITAN_C_START_

#define FIRSTTOKEN 257
#define PROGRAM    258
#define PASTAT     259
#define PASTAT2    260
#define XBEGIN     261
#define XEND       262
#define NL         263
#define ARRAY      264
#define MATCH      265
#define NOTMATCH   266
#define MATCHOP    267
#define FINAL      268
#define DOT        269
#define ALL        270
#define CCL        271
#define NCCL       272
#define CHAR       273
#define OR         274
#define STAR       275
#define QUEST      276
#define PLUS       277
#define EMPTYRE    278
#define ZERO       279
#define AND        280
#define BOR        281
#define APPEND     282
#define EQ         283
#define GE         284
#define GT         285
#define LE         286
#define LT         287
#define NE         288
#define IN         289
#define ARG        290
#define BLTIN      291
#define BREAK      292
#define CLOSE      293
#define CONTINUE   294
#define DELETE     295
#define DO         296
#define EXIT       297
#define FOR        298
#define FUNC       299
#define SUB        300
#define GSUB       301
#define IF         302
#define INDEX      303
#define LSUBSTR    304
#define MATCHFCN   305
#define NEXT       306
#define NEXTFILE   307
#define ADD        308
#define MINUS      309
#define MULT       310
#define DIVIDE     311
#define MOD        312
#define ASSIGN     313
#define ASGNOP     314
#define ADDEQ      315
#define SUBEQ      316
#define MULTEQ     317
#define DIVEQ      318
#define MODEQ      319
#define POWEQ      320
#define PRINT      321
#define PRINTF     322
#define SPRINTF    323
#define ELSE       324
#define INTEST     325
#define CONDEXPR   326
#define POSTINCR   327
#define PREINCR    328
#define POSTDECR   329
#define PREDECR    330
#define VAR        331
#define IVAR       332
#define VARNF      333
#define CALL       334
#define NUMBER     335
#define STRING     336
#define REGEXPR    337
#define GETLINE    338
#define RETURN     339
#define SPLIT      340
#define SUBSTR     341
#define WHILE      342
#define CAT        343
#define NOT        344
#define UMINUS     345
#define UPLUS      346
#define POWER      347
#define DECR       348
#define INCR       349
#define INDIRECT   350
#define LASTTOKEN  351
#ifndef YYSTYPE_DEFINED
#define YYSTYPE_DEFINED
typedef union {
  Node *p;
  Cell *cp;
  int i;
  char *s;
} YYSTYPE;
#endif /* YYSTYPE_DEFINED */
extern YYSTYPE yylval;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_AWK_AWKGRAM_TAB_H_ */
