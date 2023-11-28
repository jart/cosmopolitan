#ifndef COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_
#define COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_

/**
 * @fileoverview Types we'd prefer hadn't been invented.
 */

typedef int64_t blkcnt_t;
typedef uint8_t cc_t;
typedef int64_t clock_t; /* uint64_t on xnu */
typedef uint64_t dev_t;  /* int32_t on xnu */
typedef uint64_t fsblkcnt_t;
typedef int64_t fsfilcnt_t; /* uint32_t on xnu */
typedef uint32_t gid_t;
typedef int32_t id_t; /* int32_t on linux/freebsd/etc. */
typedef uint32_t in_addr_t;
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
typedef uint64_t ino_t;
typedef int32_t key_t;
typedef int64_t loff_t;
typedef uint32_t mode_t; /* uint16_t on xnu */
typedef uint64_t nfds_t;
typedef int64_t off_t;
typedef int32_t pid_t;
typedef int64_t register_t;
typedef uint16_t sa_family_t; /* bsd:uint8_t */
typedef uint32_t socklen_t;
typedef uint32_t speed_t;
typedef uint32_t suseconds_t;
typedef uint64_t useconds_t;   /* uint32_t on xnu */
typedef int64_t syscall_arg_t; /* uint64_t on xnu */
typedef uint32_t tcflag_t;
typedef int64_t time_t;
typedef void *timer_t;
typedef uint32_t uid_t;
typedef uint64_t rlim_t; /* int64_t on bsd */
typedef int32_t clockid_t;

#ifdef __x86_64__
typedef int64_t blksize_t; /* int32_t on xnu */
typedef uint64_t nlink_t;
#elif defined(__aarch64__)
typedef int32_t blksize_t;
typedef uint32_t nlink_t; /* uint16_t on xnu */
#endif

#define TIME_T_MAX __INT64_MAX__
#define TIME_T_MIN (-TIME_T_MAX - 1)

#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
#define F_GETLK64         F_GETLK
#define F_SETLK64         F_SETLK
#define F_SETLKW64        F_SETLKW
#define RLIM64_INFINITY   RLIM_INFINITY
#define RLIM64_SAVED_CUR  RLIM_SAVED_CUR
#define RLIM64_SAVED_MAX  RLIM_SAVED_MAX
#define alphasort64       alphasort
#define blkcnt64_t        blkcnt_t
#define dirent64          dirent
#define flock64           flock
#define fsfilcnt64_t      fsfilcnt_t
#define fstat64           fstat
#define fstatat64         fstatat
#define fstatfs64         fstatfs
#define fstatvfs64        fstatvfs
#define getrlimit64       getrlimit
#define ino64_t           ino_t
#define lockf64           lockf
#define lstat64           lstat
#define mmap64            mmap
#define off64_t           off_t
#define open64            open
#define openat64          openat
#define posix_fadvise64   posix_fadvise
#define posix_fallocate64 posix_fallocate
#define readdir64         readdir
#define readdir64_r       readdir_r
#define rlim64_t          rlim_t
#define rlimit64          rlimit
#define scandir64         scandir
#define sendfile64        sendfile
#define setrlimit64       setrlimit
#define stat64            stat
#define statfs64          statfs
#define statvfs64         statvfs
#define versionsort64     versionsort
#endif

#endif /* COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_ */
