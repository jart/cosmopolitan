#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long F_DUPFD;
extern const long F_DUPFD_CLOEXEC;
extern const long F_FULLFSYNC;
extern const long F_GETFD;
extern const long F_GETFL;
extern const long F_GETLEASE;
extern const long F_GETLK64;
extern const long F_GETLK;
extern const long F_GETOWN;
extern const long F_GETOWN_EX;
extern const long F_GETPIPE_SZ;
extern const long F_GETSIG;
extern const long F_LOCK;
extern const long F_NOCACHE;
extern const long F_NOTIFY;
extern const long F_OFD_GETLK;
extern const long F_OFD_SETLK;
extern const long F_OFD_SETLKW;
extern const long F_RDLCK;
extern const long F_SETFD;
extern const long F_SETFL;
extern const long F_SETLEASE;
extern const long F_SETLK64;
extern const long F_SETLK;
extern const long F_SETLKW64;
extern const long F_SETLKW;
extern const long F_SETOWN;
extern const long F_SETOWN_EX;
extern const long F_SETPIPE_SZ;
extern const long F_SETSIG;
extern const long F_TEST;
extern const long F_TLOCK;
extern const long F_ULOCK;
extern const long F_UNLCK;
extern const long F_WRLCK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define F_DUPFD LITERALLY(0)
#define F_GETFD LITERALLY(1)
#define F_SETFD LITERALLY(2)
#define F_GETFL LITERALLY(3)
#define F_SETFL LITERALLY(4)

#define F_DUPFD_CLOEXEC SYMBOLIC(F_DUPFD_CLOEXEC)
#define F_FULLFSYNC     SYMBOLIC(F_FULLFSYNC)
#define F_GETLEASE      SYMBOLIC(F_GETLEASE)
#define F_GETLK         SYMBOLIC(F_GETLK)
#define F_GETLK64       SYMBOLIC(F_GETLK64)
#define F_GETOWN        SYMBOLIC(F_GETOWN)
#define F_GETOWN_EX     SYMBOLIC(F_GETOWN_EX)
#define F_GETPIPE_SZ    SYMBOLIC(F_GETPIPE_SZ)
#define F_GETSIG        SYMBOLIC(F_GETSIG)
#define F_LOCK          SYMBOLIC(F_LOCK)
#define F_NOCACHE       SYMBOLIC(F_NOCACHE)
#define F_NOTIFY        SYMBOLIC(F_NOTIFY)
#define F_OFD_GETLK     SYMBOLIC(F_OFD_GETLK)
#define F_OFD_SETLK     SYMBOLIC(F_OFD_SETLK)
#define F_OFD_SETLKW    SYMBOLIC(F_OFD_SETLKW)
#define F_RDLCK         SYMBOLIC(F_RDLCK)
#define F_SETLEASE      SYMBOLIC(F_SETLEASE)
#define F_SETLK         SYMBOLIC(F_SETLK)
#define F_SETLK64       SYMBOLIC(F_SETLK64)
#define F_SETLKW        SYMBOLIC(F_SETLKW)
#define F_SETLKW64      SYMBOLIC(F_SETLKW64)
#define F_SETOWN        SYMBOLIC(F_SETOWN)
#define F_SETOWN_EX     SYMBOLIC(F_SETOWN_EX)
#define F_SETPIPE_SZ    SYMBOLIC(F_SETPIPE_SZ)
#define F_SETSIG        SYMBOLIC(F_SETSIG)
#define F_TEST          SYMBOLIC(F_TEST)
#define F_TLOCK         SYMBOLIC(F_TLOCK)
#define F_ULOCK         SYMBOLIC(F_ULOCK)
#define F_UNLCK         SYMBOLIC(F_UNLCK)
#define F_WRLCK         SYMBOLIC(F_WRLCK)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_ */
