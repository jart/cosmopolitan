#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const unsigned O_ACCMODE;
extern const unsigned O_APPEND;
extern const unsigned O_ASYNC;
extern const unsigned O_CLOEXEC;
extern const unsigned O_COMPRESSED;
extern const unsigned O_CREAT;
extern const unsigned O_DIRECT;
extern const unsigned O_DIRECTORY;
extern const unsigned O_DSYNC;
extern const unsigned O_EXCL;
extern const unsigned O_EXEC;
extern const unsigned O_EXLOCK;
extern const unsigned O_INDEXED;
extern const unsigned O_LARGEFILE;
extern const unsigned O_NDELAY;
extern const unsigned O_NOATIME;
extern const unsigned O_NOCTTY;
extern const unsigned O_NOFOLLOW;
extern const unsigned O_NOFOLLOW_ANY;
extern const unsigned O_NONBLOCK;
extern const unsigned O_PATH;
extern const unsigned O_RANDOM;
extern const unsigned O_RDONLY;
extern const unsigned O_RDWR;
extern const unsigned O_RSYNC;
extern const unsigned O_SEARCH;
extern const unsigned O_SEQUENTIAL;
extern const unsigned O_SHLOCK;
extern const unsigned O_SPARSE;
extern const unsigned O_SYNC;
extern const unsigned O_TMPFILE;
extern const unsigned O_TRUNC;
extern const unsigned O_TTY_INIT;
extern const unsigned O_VERIFY;
extern const unsigned O_WRONLY;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define O_RDONLY  LITERALLY(0)
#define O_WRONLY  LITERALLY(1)
#define O_RDWR    LITERALLY(2)
#define O_ACCMODE LITERALLY(3)

#define O_APPEND       SYMBOLIC(O_APPEND)
#define O_ASYNC        SYMBOLIC(O_ASYNC)
#define O_CLOEXEC      SYMBOLIC(O_CLOEXEC)
#define O_COMPRESSED   SYMBOLIC(O_COMPRESSED)
#define O_CREAT        SYMBOLIC(O_CREAT)
#define O_DIRECT       SYMBOLIC(O_DIRECT)
#define O_DIRECTORY    SYMBOLIC(O_DIRECTORY)
#define O_DSYNC        SYMBOLIC(O_DSYNC)
#define O_EXCL         SYMBOLIC(O_EXCL)
#define O_EXEC         SYMBOLIC(O_EXEC)
#define O_EXLOCK       SYMBOLIC(O_EXLOCK)
#define O_INDEXED      SYMBOLIC(O_INDEXED)
#define O_LARGEFILE    SYMBOLIC(O_LARGEFILE)
#define O_NDELAY       SYMBOLIC(O_NDELAY)
#define O_NOATIME      SYMBOLIC(O_NOATIME)
#define O_NOCTTY       SYMBOLIC(O_NOCTTY)
#define O_NOFOLLOW     SYMBOLIC(O_NOFOLLOW)
#define O_NOFOLLOW_ANY SYMBOLIC(O_NOFOLLOW_ANY)
#define O_NONBLOCK     SYMBOLIC(O_NONBLOCK)
#define O_PATH         SYMBOLIC(O_PATH)
#define O_RANDOM       SYMBOLIC(O_RANDOM)
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

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_ */
