#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long F_DUPFD;
hidden extern const long F_DUPFD_CLOEXEC;
hidden extern const long F_GETFD;
hidden extern const long F_GETFL;
hidden extern const long F_GETLEASE;
hidden extern const long F_GETLK64;
hidden extern const long F_GETLK;
hidden extern const long F_GETOWN;
hidden extern const long F_GETOWN_EX;
hidden extern const long F_GETPIPE_SZ;
hidden extern const long F_GETSIG;
hidden extern const long F_LOCK;
hidden extern const long F_NOTIFY;
hidden extern const long F_OFD_GETLK;
hidden extern const long F_OFD_SETLK;
hidden extern const long F_OFD_SETLKW;
hidden extern const long F_RDLCK;
hidden extern const long F_SETFD;
hidden extern const long F_SETFL;
hidden extern const long F_SETLEASE;
hidden extern const long F_SETLK64;
hidden extern const long F_SETLK;
hidden extern const long F_SETLKW64;
hidden extern const long F_SETLKW;
hidden extern const long F_SETOWN;
hidden extern const long F_SETOWN_EX;
hidden extern const long F_SETPIPE_SZ;
hidden extern const long F_SETSIG;
hidden extern const long F_TEST;
hidden extern const long F_TLOCK;
hidden extern const long F_ULOCK;
hidden extern const long F_UNLCK;
hidden extern const long F_WRLCK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define F_GETFD LITERALLY(1)
#define F_SETFD LITERALLY(2)
#define F_GETFL LITERALLY(3)
#define F_SETFL LITERALLY(4)

#define F_DUPFD SYMBOLIC(F_DUPFD)
#define F_DUPFD_CLOEXEC SYMBOLIC(F_DUPFD_CLOEXEC)
#define F_GETLEASE SYMBOLIC(F_GETLEASE)
#define F_GETLK SYMBOLIC(F_GETLK)
#define F_GETLK64 SYMBOLIC(F_GETLK64)
#define F_GETOWN SYMBOLIC(F_GETOWN)
#define F_GETOWN_EX SYMBOLIC(F_GETOWN_EX)
#define F_GETPIPE_SZ SYMBOLIC(F_GETPIPE_SZ)
#define F_GETSIG SYMBOLIC(F_GETSIG)
#define F_LOCK SYMBOLIC(F_LOCK)
#define F_NOTIFY SYMBOLIC(F_NOTIFY)
#define F_OFD_GETLK SYMBOLIC(F_OFD_GETLK)
#define F_OFD_SETLK SYMBOLIC(F_OFD_SETLK)
#define F_OFD_SETLKW SYMBOLIC(F_OFD_SETLKW)
#define F_RDLCK SYMBOLIC(F_RDLCK)
#define F_SETLEASE SYMBOLIC(F_SETLEASE)
#define F_SETLK SYMBOLIC(F_SETLK)
#define F_SETLK64 SYMBOLIC(F_SETLK64)
#define F_SETLKW SYMBOLIC(F_SETLKW)
#define F_SETLKW64 SYMBOLIC(F_SETLKW64)
#define F_SETOWN SYMBOLIC(F_SETOWN)
#define F_SETOWN_EX SYMBOLIC(F_SETOWN_EX)
#define F_SETPIPE_SZ SYMBOLIC(F_SETPIPE_SZ)
#define F_SETSIG SYMBOLIC(F_SETSIG)
#define F_TEST SYMBOLIC(F_TEST)
#define F_TLOCK SYMBOLIC(F_TLOCK)
#define F_ULOCK SYMBOLIC(F_ULOCK)
#define F_UNLCK SYMBOLIC(F_UNLCK)
#define F_WRLCK SYMBOLIC(F_WRLCK)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_ */
