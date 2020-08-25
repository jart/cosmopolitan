#ifndef _DEBUG_H
#define _DEBUG_H
#include "third_party/ctags/general.h"
/* must always come first */
#include "third_party/ctags/entry.h"

/*
 *   Macros
 */

#ifdef DEBUG
#define debug(level)      ((Option.debugLevel & (long)(level)) != 0)
#define DebugStatement(x) x
#define PrintStatus(x) \
  if (debug(DEBUG_STATUS)) printf x;
#define Assert(c) assert(c)
#else
#define DebugStatement(x)
#define PrintStatus(x)
#define Assert(c)
#ifndef NDEBUG
#define NDEBUG
#endif
#endif

/*
 *   Data declarations
 */

/*  Defines the debugging levels.
 */
enum eDebugLevels {
  DEBUG_READ = 0x01,   /* echo raw (filtered) characters */
  DEBUG_PARSE = 0x02,  /* echo parsing results */
  DEBUG_STATUS = 0x04, /* echo file status information */
  DEBUG_OPTION = 0x08, /* echo option parsing */
  DEBUG_CPP = 0x10,    /* echo characters out of pre-processor */
  DEBUG_RAW = 0x20     /* echo raw (filtered) characters */
};

/*
 *   Function prototypes
 */
extern void lineBreak(void);
extern void debugPrintf(const enum eDebugLevels level, const char *const format,
                        ...) __printf__(2, 3);
extern void debugPutc(const int level, const int c);
extern void debugParseNest(const boolean increase, const unsigned int level);
extern void debugCppNest(const boolean begin, const unsigned int level);
extern void debugCppIgnore(const boolean ignore);
extern void debugEntry(const tagEntryInfo *const tag);

#endif /* _DEBUG_H */
