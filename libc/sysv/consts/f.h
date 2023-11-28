#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
COSMOPOLITAN_C_START_

/*
 * full set of fcntl() commands
 * many are only provided by a single platform
 * will be equal to -1 when not available on host
 */
extern const int F_BARRIERFSYNC;
extern const int F_DUPFD;
extern const int F_DUPFD_CLOEXEC;
extern const int F_FULLFSYNC;
extern const int F_GETFD;
extern const int F_GETFL;
extern const int F_GETLEASE;
extern const int F_GETLK;
extern const int F_GETNOSIGPIPE;
extern const int F_GETOWN;
extern const int F_GETPATH;
extern const int F_GETPIPE_SZ;
extern const int F_GETSIG;
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
extern const int F_SETLK;
extern const int F_SETLKW;
extern const int F_SETNOSIGPIPE;
extern const int F_SETOWN;
extern const int F_SETPIPE_SZ;
extern const int F_SETSIG;
extern const int F_UNLCK;
extern const int F_WRLCK;

COSMOPOLITAN_C_END_

/*
 * portable fcntl() commands
 */
#define F_DUPFD         0
#define F_GETFD         1
#define F_SETFD         2
#define F_GETFL         3
#define F_SETFL         4
#define F_DUPFD_CLOEXEC F_DUPFD_CLOEXEC

/*
 * posix advisory locks
 * polyfilled poorly on windows
 */
#define F_SETLK  F_SETLK
#define F_SETLKW F_SETLKW
#define F_GETLK  F_GETLK
#define F_RDLCK  F_RDLCK
#define F_UNLCK  F_UNLCK
#define F_WRLCK  F_WRLCK

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_ */
