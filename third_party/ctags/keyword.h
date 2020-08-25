#ifndef _KEYWORD_H
#define _KEYWORD_H
#include "third_party/ctags/general.h"
/* must always come first */
#include "third_party/ctags/parse.h"

/*
 *   FUNCTION PROTOTYPES
 */
extern void addKeyword(const char *const string, langType language, int value);
extern int lookupKeyword(const char *const string, langType language);
extern void freeKeywordTable(void);
#ifdef DEBUG
extern void printKeywordTable(void);
#endif
extern int analyzeToken(vString *const name, langType language);

#endif /* _KEYWORD_H */
