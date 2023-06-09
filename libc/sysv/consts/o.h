#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
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

#define O_RDONLY  0
#define O_WRONLY  1
#define O_RDWR    2
#define O_ACCMODE 3

#define O_APPEND     O_APPEND
#define O_ASYNC      O_ASYNC
#define O_CLOEXEC    O_CLOEXEC
#define O_COMPRESSED O_COMPRESSED
#define O_CREAT      O_CREAT
#define O_DIRECT     O_DIRECT
#define O_DIRECTORY  O_DIRECTORY
#define O_EXCL       O_EXCL
#define O_EXEC       O_EXEC
#define O_INDEXED    O_INDEXED
#define O_LARGEFILE  O_LARGEFILE
#define O_NDELAY     O_NDELAY
#define O_NOATIME    O_NOATIME
#define O_NOCTTY     O_NOCTTY
#define O_NOFOLLOW   O_NOFOLLOW
#define O_NONBLOCK   O_NONBLOCK
#define O_RANDOM     O_RANDOM
#define O_SEQUENTIAL O_SEQUENTIAL
#define O_SYNC       O_SYNC
#define O_TRUNC      O_TRUNC

#define __tmpcosmo_O_APPEND     -801373305
#define __tmpcosmo_O_ASYNC      -957973766
#define __tmpcosmo_O_CLOEXEC    1580591778
#define __tmpcosmo_O_COMPRESSED 1922797055
#define __tmpcosmo_O_CREAT      -796151748
#define __tmpcosmo_O_DIRECT     -1042229894
#define __tmpcosmo_O_DIRECTORY  -614889241
#define __tmpcosmo_O_EXCL       1874611659
#define __tmpcosmo_O_EXEC       -727286831
#define __tmpcosmo_O_INDEXED    -234653513
#define __tmpcosmo_O_LARGEFILE  -1307734823
#define __tmpcosmo_O_NDELAY     -923014394
#define __tmpcosmo_O_NOATIME    1127867239
#define __tmpcosmo_O_NOCTTY     -107116141
#define __tmpcosmo_O_NOFOLLOW   795975523
#define __tmpcosmo_O_NONBLOCK   549686796
#define __tmpcosmo_O_RANDOM     -2052765995
#define __tmpcosmo_O_SEQUENTIAL 854419792
#define __tmpcosmo_O_SYNC       -282123817
#define __tmpcosmo_O_TRUNC      924355570

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_ */
