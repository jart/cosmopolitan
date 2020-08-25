#ifndef _GENERAL_H
#define _GENERAL_H
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "third_party/ctags/config.h"

/* Define standard error destination
 */
#ifndef errout
#define errout stderr
#endif

/* Define regex if supported */
#if (defined(HAVE_REGCOMP) && !defined(REGCOMP_BROKEN))
#define HAVE_REGEX 1
#endif

/*  This is a helpful internal feature of later versions (> 2.7) of GCC
 *  to prevent warnings about unused variables.
 */
#if (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)) && \
    !defined(__GNUG__)
#define __unused__
#define __printf__(s, f) __attribute__((__format__(printf, s, f)))
#else
#define __unused__
#define __printf__(s, f)
#endif

/*
 *  Portability macros
 */
#if !defined(HAVE_STRCASECMP) && !defined(strcasecmp)
#ifdef HAVE_STRICMP
#define strcasecmp(s1, s2) stricmp(s1, s2)
#else
#define strcasecmp(s1, s2) struppercmp(s1, s2)
#endif
#endif

#if !defined(HAVE_STRNCASECMP) && !defined(strncasecmp)
#ifdef HAVE_STRNICMP
#define strncasecmp(s1, s2, n) strnicmp(s1, s2, n)
#else
#define strncasecmp(s1, s2, n) strnuppercmp(s1, s2, n)
#endif
#endif

/*
 *   DATA DECLARATIONS
 */

#undef FALSE
#undef TRUE
#ifdef VAXC
typedef enum { FALSE, TRUE } booleanType;
typedef int boolean;
#else
#ifdef __cplusplus
typedef bool boolean;
#define FALSE false
#define TRUE  true
#else
typedef enum { FALSE, TRUE } boolean;
#endif
#endif

#if !defined(HAVE_FGETPOS) && !defined(fpos_t)
#define fpos_t long
#endif

/*
 *   FUNCTION PROTOTYPES
 */

#if defined(NEED_PROTO_REMOVE) && defined(HAVE_REMOVE)
extern int remove(const char *);
#endif

#if defined(NEED_PROTO_UNLINK) && !defined(HAVE_REMOVE)
extern void *unlink(const char *);
#endif

#ifdef NEED_PROTO_GETENV
extern char *getenv(const char *);
#endif

/* vi:set tabstop=4 shiftwidth=4: */
#endif /* _GENERAL_H */
