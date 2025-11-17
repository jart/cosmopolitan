#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_

#define F_GETFL 3
#define F_SETFL 4

#define F_GETFD 1
#define F_SETFD 2

#define FD_CLOEXEC 1

#define F_DUPFD 0

#define F_DUPFD_CLOEXEC 0x0406

#define F_SETLK  F_SETLK
#define F_SETLKW F_SETLKW
#define F_GETLK  F_GETLK

#define F_RDLCK F_RDLCK
#define F_WRLCK F_WRLCK
#define F_UNLCK 2

COSMOPOLITAN_C_START_

extern const int F_GETLK;
extern const int F_RDLCK;
extern const int F_SETLK;
extern const int F_SETLKW;
extern const int F_WRLCK;

int fcntl(int fd, int cmd, ...) libcesque;

int __fcntl_getfl(int) libcesque;
int __fcntl_getfd(int) libcesque;
int __fcntl_setfl(int, ...) libcesque;
int __fcntl_setfd(int, ...) libcesque;
int __fcntl_dupfd(int, ...) libcesque;
int __fcntl_dupfd_cloexec(int, ...) libcesque;
int __fcntl_lock(int, int, ...) libcesque;
int __fcntl_misc(int, int, ...) libcesque;

#if defined(__OPTIMIZE__) && !defined(__cplusplus) && \
    (defined(__GNUC__) || defined(__llvm__))
/* Undiamond fcntl() to avoid linking POSIX advisory locks */
#define fcntl(fd, cmd, ...)                                \
  (__extension__({                                         \
    int _rc;                                               \
    if (__fcntl_equivalent(cmd, F_GETFL)) {                \
      _rc = __fcntl_getfl(fd);                             \
    } else if (__fcntl_equivalent(cmd, F_GETFD)) {         \
      _rc = __fcntl_getfd(fd);                             \
    } else if (__fcntl_equivalent(cmd, F_SETFL)) {         \
      _rc = __fcntl_setfl(fd, ##__VA_ARGS__);              \
    } else if (__fcntl_equivalent(cmd, F_SETFD)) {         \
      _rc = __fcntl_setfd(fd, ##__VA_ARGS__);              \
    } else if (__fcntl_equivalent(cmd, F_DUPFD)) {         \
      _rc = __fcntl_dupfd(fd, ##__VA_ARGS__);              \
    } else if (__fcntl_equivalent(cmd, F_DUPFD_CLOEXEC)) { \
      _rc = __fcntl_dupfd_cloexec(fd, ##__VA_ARGS__);      \
    } else if (__fcntl_equivalent(cmd, F_GETLK) ||         \
               __fcntl_equivalent(cmd, F_SETLK) ||         \
               __fcntl_equivalent(cmd, F_SETLKW)) {        \
      _rc = __fcntl_lock(fd, cmd, ##__VA_ARGS__);          \
    } else {                                               \
      _rc = fcntl(fd, cmd, ##__VA_ARGS__);                 \
    }                                                      \
    _rc;                                                   \
  }))
#define __fcntl_equivalent(X, Y) \
  (__builtin_constant_p((X) == (Y)) && ((X) == (Y)))
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_F_H_ */
