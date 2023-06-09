#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
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

#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4

#define F_DUPFD_CLOEXEC F_DUPFD_CLOEXEC
#define F_GETLEASE      F_GETLEASE
#define F_GETLK         F_GETLK
#define F_GETLK64       F_GETLK64
#define F_GETOWN        F_GETOWN
#define F_GETPATH       F_GETPATH
#define F_GETPIPE_SZ    F_GETPIPE_SZ
#define F_GETSIG        F_GETSIG
#define F_MAXFD         F_MAXFD
#define F_NOCACHE       F_NOCACHE
#define F_NOTIFY        F_NOTIFY
#define F_RDLCK         F_RDLCK
#define F_SETLEASE      F_SETLEASE
#define F_SETLK         F_SETLK
#define F_SETLK64       F_SETLK64
#define F_SETLKW        F_SETLKW
#define F_SETLKW64      F_SETLKW64
#define F_SETOWN        F_SETOWN
#define F_SETPIPE_SZ    F_SETPIPE_SZ
#define F_SETSIG        F_SETSIG
#define F_UNLCK         F_UNLCK
#define F_WRLCK         F_WRLCK

#define __tmpcosmo_F_DUPFD_CLOEXEC -15823938
#define __tmpcosmo_F_GETLEASE      -15823862
#define __tmpcosmo_F_GETLK         -15823916
#define __tmpcosmo_F_GETLK64       -15823846
#define __tmpcosmo_F_GETOWN        -15824116
#define __tmpcosmo_F_GETPATH       -15824128
#define __tmpcosmo_F_GETPIPE_SZ    -15824006
#define __tmpcosmo_F_GETSIG        -15824112
#define __tmpcosmo_F_MAXFD         -15823896
#define __tmpcosmo_F_NOCACHE       -15824048
#define __tmpcosmo_F_NOTIFY        -15823898
#define __tmpcosmo_F_RDLCK         -15823826
#define __tmpcosmo_F_SETLEASE      -15823884
#define __tmpcosmo_F_SETLK         -15824088
#define __tmpcosmo_F_SETLK64       -15824154
#define __tmpcosmo_F_SETLKW        -15824096
#define __tmpcosmo_F_SETLKW64      -15824104
#define __tmpcosmo_F_SETOWN        -15823874
#define __tmpcosmo_F_SETPIPE_SZ    -15823958
#define __tmpcosmo_F_SETSIG        -15823832
#define __tmpcosmo_F_UNLCK         -15824148
#define __tmpcosmo_F_WRLCK         -15824058

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_ */
