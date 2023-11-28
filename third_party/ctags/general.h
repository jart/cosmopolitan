/*
*   $Id: general.h 508 2007-05-03 03:20:59Z dhiebert $
*
*   Copyright (c) 1998-2003, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   Provides the general (non-ctags-specific) environment assumed by all.
*/
#ifndef _GENERAL_H
#define _GENERAL_H
#include "third_party/ctags/config.h"

/* Define standard error destination
 */
#ifndef errout
# define errout	stderr
#endif

/* Define regex if supported */
#if (defined (HAVE_REGCOMP) && !defined (REGCOMP_BROKEN))
# define HAVE_REGEX 1
#endif

/*  This is a helpful internal feature of later versions (> 2.7) of GCC
 *  to prevent warnings about unused variables.
 */
#if (__GNUC__ > 2  ||  (__GNUC__ == 2  &&  __GNUC_MINOR__ >= 7)) && !defined (__GNUG__)
# define __unused  __attribute__((__unused__))
# define __printf(s,f)  __attribute__((__format__ (__printf__, s, f)))
#else
# define __unused
# define __printf(s,f)
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
# ifdef __cplusplus
typedef bool boolean;
#define FALSE false
#define TRUE true
# else
typedef enum { FALSE, TRUE } boolean;
# endif
#endif

#endif  /* _GENERAL_H */

/* vi:set tabstop=4 shiftwidth=4: */
