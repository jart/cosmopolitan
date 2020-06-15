#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long O_ACCMODE;
hidden extern const long O_APPEND;
hidden extern const long O_ASYNC;
hidden extern const long O_CLOEXEC;
hidden extern const long O_CREAT;
hidden extern const long O_DIRECT;
hidden extern const long O_DIRECTORY;
hidden extern const long O_DSYNC;
hidden extern const long O_EXCL;
hidden extern const long O_EXEC;
hidden extern const long O_LARGEFILE;
hidden extern const long O_NDELAY;
hidden extern const long O_NOATIME;
hidden extern const long O_NOCTTY;
hidden extern const long O_NOFOLLOW;
hidden extern const long O_NONBLOCK;
hidden extern const long O_PATH;
hidden extern const long O_RDONLY;
hidden extern const long O_RDWR;
hidden extern const long O_RSYNC;
hidden extern const long O_SPARSE;
hidden extern const long O_SYNC;
hidden extern const long O_TMPFILE;
hidden extern const long O_TRUNC;
hidden extern const long O_TTY_INIT;
hidden extern const long O_WRONLY;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define O_ACCMODE SYMBOLIC(O_ACCMODE)
#define O_APPEND SYMBOLIC(O_APPEND)
#define O_ASYNC SYMBOLIC(O_ASYNC)
#define O_CLOEXEC SYMBOLIC(O_CLOEXEC)
#define O_CREAT SYMBOLIC(O_CREAT)
#define O_DIRECT SYMBOLIC(O_DIRECT)
#define O_DIRECTORY SYMBOLIC(O_DIRECTORY)
#define O_DSYNC SYMBOLIC(O_DSYNC)
#define O_EXCL SYMBOLIC(O_EXCL)
#define O_EXEC SYMBOLIC(O_EXEC)
#define O_LARGEFILE SYMBOLIC(O_LARGEFILE)
#define O_NDELAY SYMBOLIC(O_NDELAY)
#define O_NOATIME SYMBOLIC(O_NOATIME)
#define O_NOCTTY SYMBOLIC(O_NOCTTY)
#define O_NOFOLLOW SYMBOLIC(O_NOFOLLOW)
#define O_NONBLOCK SYMBOLIC(O_NONBLOCK)
#define O_PATH SYMBOLIC(O_PATH)
#define O_RDONLY SYMBOLIC(O_RDONLY)
#define O_RDWR SYMBOLIC(O_RDWR)
#define O_RSYNC SYMBOLIC(O_RSYNC)
#define O_SPARSE SYMBOLIC(O_SPARSE)
#define O_SYNC SYMBOLIC(O_SYNC)
#define O_TMPFILE SYMBOLIC(O_TMPFILE)
#define O_TRUNC SYMBOLIC(O_TRUNC)
#define O_TTY_INIT SYMBOLIC(O_TTY_INIT)
#define O_WRONLY SYMBOLIC(O_WRONLY)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_ */
