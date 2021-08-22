#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long O_ACCMODE;
extern const long O_APPEND;
extern const long O_ASYNC;
extern const long O_CLOEXEC;
extern const long O_CREAT;
extern const long O_DIRECT;
extern const long O_DIRECTORY;
extern const long O_DSYNC;
extern const long O_EXCL;
extern const long O_EXEC;
extern const long O_EXLOCK;
extern const long O_LARGEFILE;
extern const long O_NDELAY;
extern const long O_NOATIME;
extern const long O_NOCTTY;
extern const long O_NOFOLLOW;
extern const long O_NOFOLLOW_ANY;
extern const long O_NONBLOCK;
extern const long O_PATH;
extern const long O_RANDOM;
extern const long O_RDONLY;
extern const long O_RDWR;
extern const long O_RSYNC;
extern const long O_SEARCH;
extern const long O_SEQUENTIAL;
extern const long O_SHLOCK;
extern const long O_SPARSE;
extern const long O_SYNC;
extern const long O_TMPFILE;
extern const long O_TRUNC;
extern const long O_TTY_INIT;
extern const long O_VERIFY;
extern const long O_WRONLY;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define O_ACCMODE      SYMBOLIC(O_ACCMODE)
#define O_APPEND       SYMBOLIC(O_APPEND)
#define O_ASYNC        SYMBOLIC(O_ASYNC)
#define O_CLOEXEC      SYMBOLIC(O_CLOEXEC)
#define O_CREAT        SYMBOLIC(O_CREAT)
#define O_DIRECT       SYMBOLIC(O_DIRECT)
#define O_DIRECTORY    SYMBOLIC(O_DIRECTORY)
#define O_DSYNC        SYMBOLIC(O_DSYNC)
#define O_EXCL         SYMBOLIC(O_EXCL)
#define O_EXEC         SYMBOLIC(O_EXEC)
#define O_EXLOCK       SYMBOLIC(O_EXLOCK)
#define O_LARGEFILE    SYMBOLIC(O_LARGEFILE)
#define O_NDELAY       SYMBOLIC(O_NDELAY)
#define O_NOATIME      SYMBOLIC(O_NOATIME)
#define O_NOCTTY       SYMBOLIC(O_NOCTTY)
#define O_NOFOLLOW     SYMBOLIC(O_NOFOLLOW)
#define O_NOFOLLOW_ANY SYMBOLIC(O_NOFOLLOW_ANY)
#define O_NONBLOCK     SYMBOLIC(O_NONBLOCK)
#define O_PATH         SYMBOLIC(O_PATH)
#define O_RANDOM       SYMBOLIC(O_RANDOM)
#define O_RDONLY       SYMBOLIC(O_RDONLY)
#define O_RDWR         SYMBOLIC(O_RDWR)
#define O_RSYNC        SYMBOLIC(O_RSYNC)
#define O_SEARCH       SYMBOLIC(O_SEARCH)
#define O_SEQUENTIAL   SYMBOLIC(O_SEQUENTIAL)
#define O_SHLOCK       SYMBOLIC(O_SHLOCK)
#define O_SPARSE       SYMBOLIC(O_SPARSE)
#define O_SYNC         SYMBOLIC(O_SYNC)
#define O_TMPFILE      SYMBOLIC(O_TMPFILE)
#define O_TRUNC        SYMBOLIC(O_TRUNC)
#define O_TTY_INIT     SYMBOLIC(O_TTY_INIT)
#define O_VERIFY       SYMBOLIC(O_VERIFY)
#define O_WRONLY       SYMBOLIC(O_WRONLY)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_ */
