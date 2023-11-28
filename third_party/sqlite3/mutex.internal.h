#ifndef COSMOPOLITAN_THIRD_PARTY_SQLITE3_MUTEX_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_SQLITE3_MUTEX_INTERNAL_H_
COSMOPOLITAN_C_START_

#if !SQLITE_THREADSAFE
#define SQLITE_MUTEX_OMIT
#endif
#if SQLITE_THREADSAFE && !defined(SQLITE_MUTEX_NOOP)
#if SQLITE_OS_UNIX
#define SQLITE_MUTEX_PTHREADS
#elif SQLITE_OS_WIN
#define SQLITE_MUTEX_W32
#else
#define SQLITE_MUTEX_NOOP
#endif
#endif

#ifdef SQLITE_MUTEX_OMIT
/*
** If this is a no-op implementation, implement everything as macros.
*/
#define sqlite3_mutex_alloc(X) ((sqlite3_mutex*)8)
#define sqlite3_mutex_free(X)
#define sqlite3_mutex_enter(X)
#define sqlite3_mutex_try(X) SQLITE_OK
#define sqlite3_mutex_leave(X)
#define sqlite3_mutex_held(X)    ((void)(X), 1)
#define sqlite3_mutex_notheld(X) ((void)(X), 1)
#define sqlite3MutexAlloc(X)     ((sqlite3_mutex*)8)
#define sqlite3MutexInit()       SQLITE_OK
#define sqlite3MutexEnd()
#define MUTEX_LOGIC(X)
#else
#define MUTEX_LOGIC(X) X
int sqlite3_mutex_held(sqlite3_mutex*);
#endif /* defined(SQLITE_MUTEX_OMIT) */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_SQLITE3_MUTEX_INTERNAL_H_ */
