/*
** 2013 November 25
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This file contains pre-processor directives related to operating system
** detection and/or setup.
*/
#ifndef SQLITE_OS_SETUP_H
#define SQLITE_OS_SETUP_H

/*
** Figure out if we are dealing with Unix, Windows, or some other operating
** system.
**
** After the following block of preprocess macros, all of 
**
**    SQLITE_OS_KV
**    SQLITE_OS_OTHER
**    SQLITE_OS_UNIX
**    SQLITE_OS_WIN
**
** will defined to either 1 or 0. One of them will be 1. The others will be 0.
** If none of the macros are initially defined, then select either
** SQLITE_OS_UNIX or SQLITE_OS_WIN depending on the target platform.
**
** If SQLITE_OS_OTHER=1 is specified at compile-time, then the application
** must provide its own VFS implementation together with sqlite3_os_init()
** and sqlite3_os_end() routines.
*/
#if !defined(SQLITE_OS_KV) && !defined(SQLITE_OS_OTHER) && \
       !defined(SQLITE_OS_UNIX) && !defined(SQLITE_OS_WIN)
#  if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || \
          defined(__MINGW32__) || defined(__BORLANDC__)
#    define SQLITE_OS_WIN 1
#    define SQLITE_OS_UNIX 0
#  else
#    define SQLITE_OS_WIN 0
#    define SQLITE_OS_UNIX 1
#  endif
#endif
#if SQLITE_OS_OTHER+1>1
#  undef SQLITE_OS_KV
#  define SQLITE_OS_KV 0
#  undef SQLITE_OS_UNIX
#  define SQLITE_OS_UNIX 0
#  undef SQLITE_OS_WIN
#  define SQLITE_OS_WIN 0
#endif
#if SQLITE_OS_KV+1>1
#  undef SQLITE_OS_OTHER
#  define SQLITE_OS_OTHER 0
#  undef SQLITE_OS_UNIX
#  define SQLITE_OS_UNIX 0
#  undef SQLITE_OS_WIN
#  define SQLITE_OS_WIN 0
#  define SQLITE_OMIT_LOAD_EXTENSION 1
#  define SQLITE_OMIT_WAL 1
#  define SQLITE_OMIT_DEPRECATED 1
#  undef SQLITE_TEMP_STORE
#  define SQLITE_TEMP_STORE 3  /* Always use memory for temporary storage */
#  define SQLITE_DQS 0
#  define SQLITE_OMIT_SHARED_CACHE 1
#  define SQLITE_OMIT_AUTOINIT 1
#endif
#if SQLITE_OS_UNIX+1>1
#  undef SQLITE_OS_KV
#  define SQLITE_OS_KV 0
#  undef SQLITE_OS_OTHER
#  define SQLITE_OS_OTHER 0
#  undef SQLITE_OS_WIN
#  define SQLITE_OS_WIN 0
#endif
#if SQLITE_OS_WIN+1>1
#  undef SQLITE_OS_KV
#  define SQLITE_OS_KV 0
#  undef SQLITE_OS_OTHER
#  define SQLITE_OS_OTHER 0
#  undef SQLITE_OS_UNIX
#  define SQLITE_OS_UNIX 0
#endif


#endif /* SQLITE_OS_SETUP_H */
