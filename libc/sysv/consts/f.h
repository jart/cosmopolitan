#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int F_BARRIERFSYNC;
extern const int F_DUPFD;
extern const int F_DUPFD_CLOEXEC;
extern const int F_FULLFSYNC;
extern const int F_GETFD;
extern const int F_GETFL;
extern const int F_GETLEASE;
extern const int F_GETLK64;
extern const int F_GETLK;
extern const int F_GETNOSIGPIPE;
extern const int F_GETOWN;
extern const int F_GETPATH;
extern const int F_GETPIPE_SZ;
extern const int F_GETSIG;
extern const int F_LOCK;
extern const int F_MAXFD;
extern const int F_NOCACHE;
extern const int F_NOTIFY;
extern const int F_OFD_GETLK;
extern const int F_OFD_SETLK;
extern const int F_OFD_SETLKW;
extern const int F_RDLCK;
extern const int F_SETFD;
extern const int F_SETFL;
extern const int F_SETLEASE;
extern const int F_SETLK64;
extern const int F_SETLK;
extern const int F_SETLKW64;
extern const int F_SETLKW;
extern const int F_SETNOSIGPIPE;
extern const int F_SETOWN;
extern const int F_SETPIPE_SZ;
extern const int F_SETSIG;
extern const int F_UNLCK;
extern const int F_WRLCK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define F_DUPFD LITERALLY(0)
#define F_GETFD LITERALLY(1)
#define F_SETFD LITERALLY(2)
#define F_GETFL LITERALLY(3)
#define F_SETFL LITERALLY(4)

#define F_DUPFD_CLOEXEC SYMBOLIC(F_DUPFD_CLOEXEC)
#define F_GETLEASE      SYMBOLIC(F_GETLEASE)
#define F_GETLK         SYMBOLIC(F_GETLK)
#define F_GETLK64       SYMBOLIC(F_GETLK64)
#define F_GETOWN        SYMBOLIC(F_GETOWN)
#define F_GETPATH       SYMBOLIC(F_GETPATH)
#define F_GETPIPE_SZ    SYMBOLIC(F_GETPIPE_SZ)
#define F_GETSIG        SYMBOLIC(F_GETSIG)
#define F_MAXFD         SYMBOLIC(F_MAXFD)
#define F_NOCACHE       SYMBOLIC(F_NOCACHE)
#define F_NOTIFY        SYMBOLIC(F_NOTIFY)
#define F_RDLCK         SYMBOLIC(F_RDLCK)
#define F_SETLEASE      SYMBOLIC(F_SETLEASE)
#define F_SETLK         SYMBOLIC(F_SETLK)
#define F_SETLK64       SYMBOLIC(F_SETLK64)
#define F_SETLKW        SYMBOLIC(F_SETLKW)
#define F_SETLKW64      SYMBOLIC(F_SETLKW64)
#define F_SETOWN        SYMBOLIC(F_SETOWN)
#define F_SETPIPE_SZ    SYMBOLIC(F_SETPIPE_SZ)
#define F_SETSIG        SYMBOLIC(F_SETSIG)
#define F_UNLCK         SYMBOLIC(F_UNLCK)
#define F_WRLCK         SYMBOLIC(F_WRLCK)

/* avoid leading #ifdef configurations astray */
/* #define F_FULLFSYNC SYMBOLIC(F_FULLFSYNC) */
/* #define F_BARRIERFSYNC SYMBOLIC(F_BARRIERFSYNC) */
/* #define F_OFD_GETLK     SYMBOLIC(F_OFD_GETLK) */
/* #define F_OFD_SETLK     SYMBOLIC(F_OFD_SETLK) */
/* #define F_OFD_SETLKW    SYMBOLIC(F_OFD_SETLKW) */
/* #define F_SETNOSIGPIPE  SYMBOLIC(F_SETNOSIGPIPE) */
/* #define F_GETNOSIGPIPE  SYMBOLIC(F_GETNOSIGPIPE) */

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_ */
