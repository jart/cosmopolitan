#ifndef COSMOPOLITAN_LIBC_ERRNO_H_
#define COSMOPOLITAN_LIBC_ERRNO_H_

/**
 * @fileoverview System Five error codes.
 *
 * This file defines the `errno` global variable. When system calls
 * (e.g. read(), write(), etc.) fail they return -1 to indicate the
 * failure, and that is *the only* error return value. System calls
 * also update `errno` too whenever -1 is returned (otherwise errno
 * isn't changed) to be a non-zero value holding one of the numbers
 * below, in order to indicate why the system call failed.
 *
 * There is only one exception to the above rule; some system calls
 * are documented with the `@returnserrno` tag, which means they'll
 * return the error number rather than stuffing it in a global. You
 * can usually spot these system calls easily since most of them'll
 * have names like `posix_foo()` or `pthread_bar()`.
 */

#define EPERM           1   /* Operation not permitted */
#define ENOENT          2   /* No such file or directory */
#define ESRCH           3   /* No such process */
#define EINTR           4   /* The greatest of all errnos */
#define EIO             5   /* I/O error */
#define ENXIO           6   /* No such device or address */
#define E2BIG           7   /* Arg list too long */
#define ENOEXEC         8   /* Exec format error */
#define EBADF           9   /* Bad file descriptor */
#define ECHILD          10  /* No child process */
#define EAGAIN          11  /* Resource temporarily unavailable */
#define ENOMEM          12  /* Out of memory */
#define EACCES          13  /* Permission denied */
#define EFAULT          14  /* Invalid memory address */
#define ENOTBLK         15  /* Block device required */
#define EBUSY           16  /* Device or resource busy */
#define EEXIST          17  /* File exists */
#define EXDEV           18  /* Improper link */
#define ENODEV          19  /* No such device */
#define ENOTDIR         20  /* Not a directory */
#define EISDIR          21  /* Is a directory */
#define EINVAL          22  /* Invalid argument */
#define ENFILE          23  /* Too many open files in system */
#define EMFILE          24  /* Too many open files */
#define ENOTTY          25  /* Inappropriate i/o control operation */
#define ETXTBSY         26  /* Text file busy */
#define EFBIG           27  /* File too large */
#define ENOSPC          28  /* No space left on device */
#define ESPIPE          29  /* Invalid seek */
#define EROFS           30  /* Read-only filesystem */
#define EMLINK          31  /* Too many links */
#define EPIPE           32  /* Broken pipe */
#define EDOM            33  /* Argument outside function domain */
#define ERANGE          34  /* Result too large */
#define EDEADLK         35  /* Resource deadlock avoided */
#define ENAMETOOLONG    36  /* Filename too long */
#define ENOLCK          37  /* No locks available */
#define ENOSYS          38  /* Function not implemented */
#define ENOTEMPTY       39  /* Directory not empty */
#define ELOOP           40  /* Too many levels of symbolic links */
#define ENOMSG          42  /* No message error */
#define EIDRM           43  /* Identifier removed */
#define ENOSTR          60  /* No string */
#define ENODATA         61  /* No data */
#define ETIME           62  /* Timer expired */
#define ENOSR           63  /* Out of streams resources */
#define ENONET          64  /* No network */
#define EREMOTE         66  /* Remote error */
#define ENOLINK         67  /* Link severed */
#define EPROTO          71  /* Protocol error */
#define EMULTIHOP       72  /* Multihop attempted */
#define EBADMSG         74  /* Bad message */
#define EOVERFLOW       75  /* Overflow error */
#define EBADFD          77  /* File descriptor in bad state */
#define EFTYPE          79  /* Inappropriate file type or format */
#define EILSEQ          84  /* Unicode decoding error */
#define ERESTART        85  /* Please restart syscall */
#define EUSERS          87  /* Too many users */
#define ENOTSOCK        88  /* Not a socket */
#define EDESTADDRREQ    89  /* Destination address required */
#define EMSGSIZE        90  /* Message too long */
#define EPROTOTYPE      91  /* Protocol wrong type for socket */
#define ENOPROTOOPT     92  /* Protocol not available */
#define EPROTONOSUPPORT 93  /* Protocol not supported */
#define ESOCKTNOSUPPORT 94  /* Socket type not supported */
#define ENOTSUP         95  /* Operation not supported */
#define EPFNOSUPPORT    96  /* Protocol family not supported */
#define EAFNOSUPPORT    97  /* Address family not supported */
#define EADDRINUSE      98  /* Address already in use */
#define EADDRNOTAVAIL   99  /* Address not available */
#define ENETDOWN        100 /* Network is down */
#define ENETUNREACH     101 /* Host is unreachable */
#define ENETRESET       102 /* Connection reset by network */
#define ECONNABORTED    103 /* Connection reset before accept */
#define ECONNRESET      104 /* Connection reset by client */
#define ENOBUFS         105 /* No buffer space available */
#define EISCONN         106 /* Socket is connected */
#define ENOTCONN        107 /* Transport endpoint is not connected */
#define ESHUTDOWN       108 /* Transport endpoint shutdown */
#define ETOOMANYREFS    109 /* Too many references: cannot splice */
#define ETIMEDOUT       110 /* Connection timed out */
#define ECONNREFUSED    111 /* Connection refused error */
#define EHOSTDOWN       112 /* Host down error */
#define EHOSTUNREACH    113 /* Host unreachable error */
#define EALREADY        114 /* Connection already in progress */
#define EINPROGRESS     115 /* Operation already in progress */
#define ESTALE          116 /* Stale error */
#define EDQUOT          122 /* Disk quota exceeded */
#define ENOMEDIUM       123 /* No medium found */
#define EMEDIUMTYPE     124 /* Wrong medium type */
#define ECANCELED       125 /* Operation canceled */
#define EOWNERDEAD      130 /* Owner died */
#define ENOTRECOVERABLE 131 /* State not recoverable */
#define ERFKILL         132 /* Operation not possible due to RF-kill */
#define EHWPOISON       133 /* Memory page has hardware error */

#define EWOULDBLOCK EAGAIN
#define EOPNOTSUPP  ENOTSUP

#ifndef __ASSEMBLER__
COSMOPOLITAN_C_START_

#if defined(__GNUC__) && defined(__aarch64__) && !defined(__cplusplus)
/* this header is included by 700+ files; therefore we */
/* hand-roll &__get_tls()->tib_errno to avoid #include */
/* cosmopolitan uses x28 as the tls register b/c apple */
#define errno                                       \
  (*__extension__({                                 \
    errno_t *__ep;                                  \
    __asm__("sub\t%0,x28,#1024-0x3c" : "=r"(__ep)); \
    __ep;                                           \
  }))
#else
#define errno (*__errno_location())
#endif

extern errno_t __errno;
errno_t *__errno_location(void) dontthrow pureconst;

#if defined(_COSMO_SOURCE) || defined(_GNU_SOURCE)
extern char *program_invocation_short_name;
extern char *program_invocation_name;
#endif

COSMOPOLITAN_C_END_
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_ERRNO_H_ */
