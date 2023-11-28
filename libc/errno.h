#ifndef COSMOPOLITAN_LIBC_ERRNO_H_
#define COSMOPOLITAN_LIBC_ERRNO_H_
COSMOPOLITAN_C_START_

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
 *
 * @see libc/sysv/consts.sh for assigned numbers
 * @see libc/sysv/dos2errno.sh for multimapped numbers
 */

#if defined(__GNUC__) && defined(__aarch64__) && !defined(__STRICT_ANSI__) && \
    !defined(__cplusplus)
/* this header is included by 700+ files; therefore we */
/* hand-roll &__get_tls()->tib_errno to avoid #include */
/* cosmopolitan uses x28 as the tls register b/c apple */
#define errno                                  \
  (*({                                         \
    errno_t *__ep;                             \
    asm("sub\t%0,x28,#192-0x3c" : "=r"(__ep)); \
    __ep;                                      \
  }))
#else
#define errno (*__errno_location())
#endif

/**
 * System call unavailable.
 * @note kNtErrorInvalidFunction on NT
 */
extern const errno_t ENOSYS;

/**
 * Operation not permitted.
 * @note kNtErrorInvalidAccess on NT
 */
extern const errno_t EPERM;

/**
 * No such file or directory.
 */
extern const errno_t ENOENT;

/**
 * No such process.
 */
extern const errno_t ESRCH;

/**
 * The greatest of all errnos.
 */
extern const errno_t EINTR;

/**
 * Unix consensus.
 */
extern const errno_t EIO;

/**
 * No such device or address.
 */
extern const errno_t ENXIO;

/**
 * Argument list too errno_t.
 */
extern const errno_t E2BIG;

/**
 * Exec format error.
 */
extern const errno_t ENOEXEC;

/**
 * Bad file descriptor.
 */
extern const errno_t EBADF;

/**
 * No child process.
 */
extern const errno_t ECHILD;

/**
 * Resource temporarily unavailable (e.g. SO_RCVTIMEO expired, too many
 * processes, too much memory locked, read or write with O_NONBLOCK needs
 * polling, etc.).
 */
extern const errno_t EAGAIN;

/**
 * We require more vespene gas.
 */
extern const errno_t ENOMEM;

/**
 * Permission denied.
 */
extern const errno_t EACCES;

/**
 * Pointer passed to system call that would otherwise segfault.
 */
extern const errno_t EFAULT;

/**
 * Block device required.
 */
extern const errno_t ENOTBLK;

/**
 * Device or resource busy.
 */
extern const errno_t EBUSY;

/**
 * File exists.
 */
extern const errno_t EEXIST;

/**
 * Improper link.
 */
extern const errno_t EXDEV;

/**
 * No such device.
 */
extern const errno_t ENODEV;

/**
 * Not a directory.
 */
extern const errno_t ENOTDIR;

/**
 * Is a a directory.
 */
extern const errno_t EISDIR;

/**
 * Invalid argument.
 */
extern const errno_t EINVAL;

/**
 * Too many open files in system.
 */
extern const errno_t ENFILE;

/**
 * Too many open files.
 */
extern const errno_t EMFILE;

/**
 * Inappropriate i/o control operation.
 */
extern const errno_t ENOTTY;

/**
 * Won't open executable that's executing in write mode.
 */
extern const errno_t ETXTBSY;

/**
 * File too large.
 */
extern const errno_t EFBIG;

/**
 * No space left on device.
 */
extern const errno_t ENOSPC;

/**
 * Disk quota exceeded.
 */
extern const errno_t EDQUOT;

/**
 * Invalid seek.
 */
extern const errno_t ESPIPE;

/**
 * Read-only filesystem.
 */
extern const errno_t EROFS;

/**
 * Too many links.
 */
extern const errno_t EMLINK;

/**
 * Broken pipe.
 */
extern const errno_t EPIPE;

/**
 * Mathematics argument out of domain of function.
 */
extern const errno_t EDOM;

/**
 * Result too large.
 */
extern const errno_t ERANGE;

/**
 * Resource deadlock avoided.
 */
extern const errno_t EDEADLK;

/**
 * Filename too errno_t.
 */
extern const errno_t ENAMETOOLONG;

/**
 * No locks available.
 */
extern const errno_t ENOLCK;

/**
 * Directory not empty.
 */
extern const errno_t ENOTEMPTY;

/**
 * Too many levels of symbolic links.
 */
extern const errno_t ELOOP;

/**
 * No message error.
 */
extern const errno_t ENOMSG;

/**
 * Identifier removed.
 */
extern const errno_t EIDRM;

/**
 * Timer expired.
 */
extern const errno_t ETIME;

/**
 * Protocol error.
 */
extern const errno_t EPROTO;

/**
 * Overflow error.
 */
extern const errno_t EOVERFLOW;

/**
 * Unicode decoding error.
 */
extern const errno_t EILSEQ;

/**
 * Too many users.
 */
extern const errno_t EUSERS;

/**
 * Not a socket.
 */
extern const errno_t ENOTSOCK;

/**
 * Destination address required.
 */
extern const errno_t EDESTADDRREQ;

/**
 * Message too errno_t.
 */
extern const errno_t EMSGSIZE;

/**
 * Protocol wrong type for socket.
 */
extern const errno_t EPROTOTYPE;

/**
 * Protocol not available.
 */
extern const errno_t ENOPROTOOPT;

/**
 * Protocol not supported.
 */
extern const errno_t EPROTONOSUPPORT;

/**
 * Socket type not supported.
 */
extern const errno_t ESOCKTNOSUPPORT;

/**
 * Operation not supported.
 */
extern const errno_t ENOTSUP;

/**
 * Socket operation not supported.
 */
extern const errno_t EOPNOTSUPP;

/**
 * Protocol family not supported.
 */
extern const errno_t EPFNOSUPPORT;

/**
 * Address family not supported.
 */
extern const errno_t EAFNOSUPPORT;

/**
 * Address already in use.
 */
extern const errno_t EADDRINUSE;

/**
 * Address not available.
 */
extern const errno_t EADDRNOTAVAIL;

/**
 * Network is down.
 */
extern const errno_t ENETDOWN;

/**
 * Host is unreachable.
 */
extern const errno_t ENETUNREACH;

/**
 * Connection reset by network.
 */
extern const errno_t ENETRESET;

/**
 * Connection reset before accept.
 */
extern const errno_t ECONNABORTED;

/**
 * Connection reset by client.
 */
extern const errno_t ECONNRESET;

/**
 * No buffer space available.
 */
extern const errno_t ENOBUFS;

/**
 * Socket is connected.
 */
extern const errno_t EISCONN;

/**
 * Socket is not connected.
 */
extern const errno_t ENOTCONN;

/**
 * Cannot send after transport endpoint shutdown.
 */
extern const errno_t ESHUTDOWN;

/**
 * Too many references: cannot splice.
 */
extern const errno_t ETOOMANYREFS;

/**
 * Connection timed out.
 */
extern const errno_t ETIMEDOUT;

/**
 * Connection refused error.
 */
extern const errno_t ECONNREFUSED;

/**
 * Host down error.
 */
extern const errno_t EHOSTDOWN;

/**
 * Host unreachable error.
 */
extern const errno_t EHOSTUNREACH;

/**
 * Connection already in progress.
 */
extern const errno_t EALREADY;

/**
 * Operation already in progress.
 */
extern const errno_t EINPROGRESS;

/**
 * Stale error.
 */
extern const errno_t ESTALE;

/**
 * Remote error.
 */
extern const errno_t EREMOTE;

/**
 * Bad message.
 */
extern const errno_t EBADMSG;

/**
 * Operation canceled.
 */
extern const errno_t ECANCELED;

/**
 * Owner died.
 */
extern const errno_t EOWNERDEAD;

/**
 * State not recoverable.
 */
extern const errno_t ENOTRECOVERABLE;

/**
 * No network.
 */
extern const errno_t ENONET;

/**
 * Please restart syscall.
 */
extern const errno_t ERESTART;

/**
 * Out of streams resources.
 */
extern const errno_t ENOSR;

/**
 * No string.
 */
extern const errno_t ENOSTR;

/**
 * No data.
 */
extern const errno_t ENODATA;

/**
 * Multihop attempted.
 */
extern const errno_t EMULTIHOP;

/**
 * Link severed.
 */
extern const errno_t ENOLINK;

/**
 * No medium found.
 */
extern const errno_t ENOMEDIUM;

/**
 * Wrong medium type.
 */
extern const errno_t EMEDIUMTYPE;

/**
 * Inappropriate file type or format.
 */
extern const errno_t EFTYPE;

extern const errno_t EAUTH;
extern const errno_t EBADARCH;
extern const errno_t EBADEXEC;
extern const errno_t EBADMACHO;
extern const errno_t EBADRPC;
extern const errno_t EDEVERR;
extern const errno_t ENEEDAUTH;
extern const errno_t ENOATTR;
extern const errno_t ENOPOLICY;
extern const errno_t EPROCLIM;
extern const errno_t EPROCUNAVAIL;
extern const errno_t EPROGMISMATCH;
extern const errno_t EPROGUNAVAIL;
extern const errno_t EPWROFF;
extern const errno_t ERPCMISMATCH;
extern const errno_t ESHLIBVERS;
extern const errno_t EADV;
extern const errno_t EBADE;
extern const errno_t EBADFD;
extern const errno_t EBADR;
extern const errno_t EBADRQC;
extern const errno_t EBADSLT;
extern const errno_t ECHRNG;
extern const errno_t ECOMM;
extern const errno_t EDOTDOT;
extern const errno_t EHWPOISON;
extern const errno_t EISNAM;
extern const errno_t EKEYEXPIRED;
extern const errno_t EKEYREJECTED;
extern const errno_t EKEYREVOKED;
extern const errno_t EL2HLT;
extern const errno_t EL2NSYNC;
extern const errno_t EL3HLT;
extern const errno_t EL3RST;
extern const errno_t ELIBACC;
extern const errno_t ELIBBAD;
extern const errno_t ELIBEXEC;
extern const errno_t ELIBMAX;
extern const errno_t ELIBSCN;
extern const errno_t ELNRNG;
extern const errno_t ENAVAIL;
extern const errno_t ENOANO;
extern const errno_t ENOCSI;
extern const errno_t ENOKEY;
extern const errno_t ENOPKG;
extern const errno_t ENOTNAM;
extern const errno_t ENOTUNIQ;
extern const errno_t EREMCHG;
extern const errno_t EREMOTEIO;
extern const errno_t ERFKILL;
extern const errno_t ESRMNT;
extern const errno_t ESTRPIPE;
extern const errno_t EUCLEAN;
extern const errno_t EUNATCH;
extern const errno_t EXFULL;

#define E2BIG           E2BIG
#define EACCES          EACCES
#define EADDRINUSE      EADDRINUSE
#define EADDRNOTAVAIL   EADDRNOTAVAIL
#define EAFNOSUPPORT    EAFNOSUPPORT
#define EAGAIN          EAGAIN
#define EALREADY        EALREADY
#define EBADF           EBADF
#define EBADMSG         EBADMSG
#define EBUSY           EBUSY
#define ECANCELED       ECANCELED
#define ECHILD          ECHILD
#define ECONNABORTED    ECONNABORTED
#define ECONNREFUSED    ECONNREFUSED
#define ECONNRESET      ECONNRESET
#define EDEADLK         EDEADLK
#define EDESTADDRREQ    EDESTADDRREQ
#define EDOM            EDOM
#define EDQUOT          EDQUOT
#define EEXIST          EEXIST
#define EFAULT          EFAULT
#define EFBIG           EFBIG
#define EFTYPE          EFTYPE
#define EHOSTDOWN       EHOSTDOWN
#define EHOSTUNREACH    EHOSTUNREACH
#define EIDRM           EIDRM
#define EILSEQ          EILSEQ
#define EINPROGRESS     EINPROGRESS
#define EINTR           EINTR
#define EINVAL          EINVAL
#define EIO             EIO
#define EISCONN         EISCONN
#define EISDIR          EISDIR
#define ELOOP           ELOOP
#define EMEDIUMTYPE     EMEDIUMTYPE
#define EMFILE          EMFILE
#define EMLINK          EMLINK
#define EMSGSIZE        EMSGSIZE
#define EMULTIHOP       EMULTIHOP
#define ENAMETOOLONG    ENAMETOOLONG
#define ENETDOWN        ENETDOWN
#define ENETRESET       ENETRESET
#define ENETUNREACH     ENETUNREACH
#define ENFILE          ENFILE
#define ENOBUFS         ENOBUFS
#define ENODATA         ENODATA
#define ENODEV          ENODEV
#define ENOENT          ENOENT
#define ENOEXEC         ENOEXEC
#define ENOLCK          ENOLCK
#define ENOLINK         ENOLINK
#define ENOMEDIUM       ENOMEDIUM
#define ENOMEM          ENOMEM
#define ENOMSG          ENOMSG
#define ENONET          ENONET
#define ENOPROTOOPT     ENOPROTOOPT
#define ENOSPC          ENOSPC
#define ENOSR           ENOSR
#define ENOSTR          ENOSTR
#define ENOSYS          ENOSYS
#define ENOTBLK         ENOTBLK
#define ENOTCONN        ENOTCONN
#define ENOTDIR         ENOTDIR
#define ENOTEMPTY       ENOTEMPTY
#define ENOTRECOVERABLE ENOTRECOVERABLE
#define ENOTSOCK        ENOTSOCK
#define ENOTSUP         ENOTSUP
#define ENOTTY          ENOTTY
#define ENXIO           ENXIO
#define EOPNOTSUPP      EOPNOTSUPP
#define EOVERFLOW       EOVERFLOW
#define EOWNERDEAD      EOWNERDEAD
#define EPERM           EPERM
#define EPFNOSUPPORT    EPFNOSUPPORT
#define EPIPE           EPIPE
#define EPROTO          EPROTO
#define EPROTONOSUPPORT EPROTONOSUPPORT
#define EPROTOTYPE      EPROTOTYPE
#define ERANGE          ERANGE
#define EREMOTE         EREMOTE
#define ERESTART        ERESTART
#define EROFS           EROFS
#define ESHUTDOWN       ESHUTDOWN
#define ESOCKTNOSUPPORT ESOCKTNOSUPPORT
#define ESPIPE          ESPIPE
#define ESRCH           ESRCH
#define ESTALE          ESTALE
#define ETIME           ETIME
#define ETIMEDOUT       ETIMEDOUT
#define ETOOMANYREFS    ETOOMANYREFS
#define ETXTBSY         ETXTBSY
#define EUSERS          EUSERS
#define EWOULDBLOCK     EAGAIN
#define EXDEV           EXDEV

extern errno_t __errno;
errno_t *__errno_location(void) dontthrow pureconst;

#if defined(_COSMO_SOURCE) || defined(_GNU_SOURCE)
extern char *program_invocation_short_name;
extern char *program_invocation_name;
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ERRNO_H_ */
