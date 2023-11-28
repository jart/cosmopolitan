#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_

#define O_RDONLY  0
#define O_WRONLY  1
#define O_RDWR    2
#define O_ACCMODE 3

COSMOPOLITAN_C_START_

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
extern const unsigned O_NOATIME;
extern const unsigned O_NOCTTY;
extern const unsigned O_NOFOLLOW;
extern const unsigned O_NOFOLLOW_ANY;
extern const unsigned O_NONBLOCK;
extern const unsigned O_PATH;
extern const unsigned O_RANDOM;
extern const unsigned O_RSYNC;
extern const unsigned O_SEARCH;
extern const unsigned O_SEQUENTIAL;
extern const unsigned O_SHLOCK;
extern const unsigned O_SYNC;
extern const unsigned O_TMPFILE; /* use tmpfd() or tmpfile() */
extern const unsigned O_TRUNC;
extern const unsigned O_TTY_INIT;
extern const unsigned O_UNLINK;
extern const unsigned O_VERIFY;

#define O_APPEND     O_APPEND
#define O_ASYNC      O_ASYNC
#define O_CLOEXEC    O_CLOEXEC
#define O_COMPRESSED O_COMPRESSED
#define O_CREAT      O_CREAT
#define O_DIRECTORY  O_DIRECTORY
#define O_EXCL       O_EXCL
#define O_EXEC       O_EXEC
#define O_INDEXED    O_INDEXED
#define O_LARGEFILE  O_LARGEFILE
#define O_NDELAY     O_NONBLOCK
#define O_NOATIME    O_NOATIME
#define O_NOCTTY     O_NOCTTY
#define O_NOFOLLOW   O_NOFOLLOW
#define O_NONBLOCK   O_NONBLOCK
#define O_RANDOM     O_RANDOM
#define O_SEQUENTIAL O_SEQUENTIAL
#define O_SYNC       O_SYNC
#define O_TRUNC      O_TRUNC
#define O_UNLINK     O_UNLINK

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_O_H_ */
