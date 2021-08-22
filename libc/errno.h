#ifndef COSMOPOLITAN_LIBC_ERRNO_H_
#define COSMOPOLITAN_LIBC_ERRNO_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview System error codes.
 * @see libc/sysv/consts.sh for numbers
 */

extern errno_t errno;

/**
 * System call unavailable.
 * @note kNtErrorInvalidFunction on NT
 */
extern const long ENOSYS;

/**
 * Operation not permitted.
 * @note kNtErrorInvalidAccess on NT
 */
extern const long EPERM;

/**
 * No such file or directory.
 */
extern const long ENOENT;

/**
 * No such process.
 */
extern const long ESRCH;

/**
 * The greatest of all errnos.
 */
extern const long EINTR;

/**
 * Unix consensus.
 */
extern const long EIO;

/**
 * No such device or address.
 */
extern const long ENXIO;

/**
 * Argument list too long.
 */
extern const long E2BIG;

/**
 * Exec format error.
 */
extern const long ENOEXEC;

/**
 * Bad file descriptor.
 */
extern const long EBADF;

/**
 * No child process.
 */
extern const long ECHILD;

/**
 * Resource temporarily unavailable (e.g. SO_RCVTIMEO expired, too many
 * processes, too much memory locked, read or write with O_NONBLOCK needs
 * polling, etc.).
 */
extern const long EAGAIN;

/**
 * We require more vespene gas.
 */
extern const long ENOMEM;

/**
 * Permission denied.
 */
extern const long EACCES;

/**
 * Pointer passed to system call that would otherwise segfault.
 */
extern const long EFAULT;

/**
 * Block device required.
 */
extern const long ENOTBLK;

/**
 * Device or resource busy.
 */
extern const long EBUSY;

/**
 * File exists.
 */
extern const long EEXIST;

/**
 * Improper link.
 */
extern const long EXDEV;

/**
 * No such device.
 */
extern const long ENODEV;

/**
 * Not a directory.
 */
extern const long ENOTDIR;

/**
 * Is a a directory.
 */
extern const long EISDIR;

/**
 * Invalid argument.
 */
extern const long EINVAL;

/**
 * Too many open files in system.
 */
extern const long ENFILE;

/**
 * Too many open files.
 */
extern const long EMFILE;

/**
 * Inappropriate i/o control operation.
 */
extern const long ENOTTY;

/**
 * Won't open executable that's executing in write mode.
 */
extern const long ETXTBSY;

/**
 * File too large.
 */
extern const long EFBIG;

/**
 * No space left on device.
 */
extern const long ENOSPC;

/**
 * Disk quota exceeded.
 */
extern const long EDQUOT;

/**
 * Invalid seek.
 */
extern const long ESPIPE;

/**
 * Read-only filesystem.
 */
extern const long EROFS;

/**
 * Too many links.
 */
extern const long EMLINK;

/**
 * Broken pipe.
 */
extern const long EPIPE;

/**
 * Mathematics argument out of domain of function.
 */
extern const long EDOM;

/**
 * Result too large.
 */
extern const long ERANGE;

/**
 * Resource deadlock avoided.
 */
extern const long EDEADLK;

/**
 * Filename too long.
 */
extern const long ENAMETOOLONG;

/**
 * No locks available.
 */
extern const long ENOLCK;

/**
 * Directory not empty.
 */
extern const long ENOTEMPTY;

/**
 * Too many levels of symbolic links.
 */
extern const long ELOOP;

/**
 * No message error.
 */
extern const long ENOMSG;

/**
 * Identifier removed.
 */
extern const long EIDRM;

/**
 * Timer expired.
 */
extern const long ETIME;

/**
 * Protocol error.
 */
extern const long EPROTO;

/**
 * Overflow error.
 */
extern const long EOVERFLOW;

/**
 * Unicode decoding error.
 */
extern const long EILSEQ;

/**
 * Too many users.
 */
extern const long EUSERS;

/**
 * Not a socket.
 */
extern const long ENOTSOCK;

/**
 * Destination address required.
 */
extern const long EDESTADDRREQ;

/**
 * Message too long.
 */
extern const long EMSGSIZE;

/**
 * Protocol wrong type for socket.
 */
extern const long EPROTOTYPE;

/**
 * Protocol not available.
 */
extern const long ENOPROTOOPT;

/**
 * Protocol not supported.
 */
extern const long EPROTONOSUPPORT;

/**
 * Socket type not supported.
 */
extern const long ESOCKTNOSUPPORT;

/**
 * Operation not supported.
 */
extern const long ENOTSUP;

/**
 * Socket operation not supported.
 */
extern const long EOPNOTSUPP;

/**
 * Protocol family not supported.
 */
extern const long EPFNOSUPPORT;

/**
 * Address family not supported.
 */
extern const long EAFNOSUPPORT;

/**
 * Address already in use.
 */
extern const long EADDRINUSE;

/**
 * Address not available.
 */
extern const long EADDRNOTAVAIL;

/**
 * Network is down.
 */
extern const long ENETDOWN;

/**
 * Host is unreachable.
 */
extern const long ENETUNREACH;

/**
 * Connection reset by network.
 */
extern const long ENETRESET;

/**
 * Connection reset before accept.
 */
extern const long ECONNABORTED;

/**
 * Connection reset by client.
 */
extern const long ECONNRESET;

/**
 * No buffer space available.
 */
extern const long ENOBUFS;

/**
 * Socket is connected.
 */
extern const long EISCONN;

/**
 * Socket is not connected.
 */
extern const long ENOTCONN;

/**
 * Cannot send after transport endpoint shutdown.
 */
extern const long ESHUTDOWN;

/**
 * Too many references: cannot splice.
 */
extern const long ETOOMANYREFS;

/**
 * Connection timed out.
 */
extern const long ETIMEDOUT;

/**
 * Connection refused error.
 */
extern const long ECONNREFUSED;

/**
 * Host down error.
 */
extern const long EHOSTDOWN;

/**
 * Host unreachable error.
 */
extern const long EHOSTUNREACH;

/**
 * Connection already in progress.
 */
extern const long EALREADY;

/**
 * Operation already in progress.
 */
extern const long EINPROGRESS;

/**
 * Stale error.
 */
extern const long ESTALE;

/**
 * Remote error.
 */
extern const long EREMOTE;

/**
 * Bad message.
 */
extern const long EBADMSG;

/**
 * Operation canceled.
 */
extern const long ECANCELED;

/**
 * Owner died.
 */
extern const long EOWNERDEAD;

/**
 * State not recoverable.
 */
extern const long ENOTRECOVERABLE;

/**
 * No network.
 */
extern const long ENONET;

/**
 * Please restart syscall.
 */
extern const long ERESTART;

/**
 * Out of streams resources.
 */
extern const long ENOSR;

/**
 * No string.
 */
extern const long ENOSTR;

/**
 * No data.
 */
extern const long ENODATA;

/**
 * Multihop attempted.
 */
extern const long EMULTIHOP;

/**
 * Link severed.
 */
extern const long ENOLINK;

/**
 * No medium found.
 */
extern const long ENOMEDIUM;

/**
 * Wrong medium type.
 */
extern const long EMEDIUMTYPE;

/**
 * Inappropriate file type or format. (BSD only)
 */
extern const long EFTYPE;

extern const long EAUTH;
extern const long EBADARCH;
extern const long EBADEXEC;
extern const long EBADMACHO;
extern const long EBADRPC;
extern const long EDEVERR;
extern const long ENEEDAUTH;
extern const long ENOATTR;
extern const long ENOPOLICY;
extern const long EPROCLIM;
extern const long EPROCUNAVAIL;
extern const long EPROGMISMATCH;
extern const long EPROGUNAVAIL;
extern const long EPWROFF;
extern const long ERPCMISMATCH;
extern const long ESHLIBVERS;

extern const long EADV;
extern const long EBADE;
extern const long EBADFD;
extern const long EBADR;
extern const long EBADRQC;
extern const long EBADSLT;
extern const long ECHRNG;
extern const long ECOMM;
extern const long EDOTDOT;
extern const long EHWPOISON;
extern const long EISNAM;
extern const long EKEYEXPIRED;
extern const long EKEYREJECTED;
extern const long EKEYREVOKED;
extern const long EL2HLT;
extern const long EL2NSYNC;
extern const long EL3HLT;
extern const long EL3RST;
extern const long ELIBACC;
extern const long ELIBBAD;
extern const long ELIBEXEC;
extern const long ELIBMAX;
extern const long ELIBSCN;
extern const long ELNRNG;
extern const long ENAVAIL;
extern const long ENOANO;
extern const long ENOCSI;
extern const long ENOKEY;
extern const long ENOPKG;
extern const long ENOTNAM;
extern const long ENOTUNIQ;
extern const long EREMCHG;
extern const long EREMOTEIO;
extern const long ERFKILL;
extern const long ESRMNT;
extern const long ESTRPIPE;
extern const long EUCLEAN;
extern const long EUNATCH;
extern const long EXFULL;

#define E2BIG           E2BIG
#define EACCES          EACCES
#define EADDRINUSE      EADDRINUSE
#define EADDRNOTAVAIL   EADDRNOTAVAIL
#define EADV            EADV
#define EAFNOSUPPORT    EAFNOSUPPORT
#define EAGAIN          EAGAIN
#define EALREADY        EALREADY
#define EAUTH           EAUTH
#define EBADARCH        EBADARCH
#define EBADE           EBADE
#define EBADEXEC        EBADEXEC
#define EBADF           EBADF
#define EBADFD          EBADFD
#define EBADMACHO       EBADMACHO
#define EBADMSG         EBADMSG
#define EBADR           EBADR
#define EBADRPC         EBADRPC
#define EBADRQC         EBADRQC
#define EBADSLT         EBADSLT
#define EBUSY           EBUSY
#define ECANCELED       ECANCELED
#define ECHILD          ECHILD
#define ECHRNG          ECHRNG
#define ECOMM           ECOMM
#define ECONNABORTED    ECONNABORTED
#define ECONNREFUSED    ECONNREFUSED
#define ECONNRESET      ECONNRESET
#define EDEADLK         EDEADLK
#define EDESTADDRREQ    EDESTADDRREQ
#define EDEVERR         EDEVERR
#define EDOM            EDOM
#define EDOTDOT         EDOTDOT
#define EDQUOT          EDQUOT
#define EEXIST          EEXIST
#define EFAULT          EFAULT
#define EFBIG           EFBIG
#define EFTYPE          EFTYPE
#define EHOSTDOWN       EHOSTDOWN
#define EHOSTUNREACH    EHOSTUNREACH
#define EHWPOISON       EHWPOISON
#define EIDRM           EIDRM
#define EILSEQ          EILSEQ
#define EINPROGRESS     EINPROGRESS
#define EINTR           EINTR
#define EINVAL          EINVAL
#define EIO             EIO
#define EISCONN         EISCONN
#define EISDIR          EISDIR
#define EISNAM          EISNAM
#define EKEYEXPIRED     EKEYEXPIRED
#define EKEYREJECTED    EKEYREJECTED
#define EKEYREVOKED     EKEYREVOKED
#define EL2HLT          EL2HLT
#define EL2NSYNC        EL2NSYNC
#define EL3HLT          EL3HLT
#define EL3RST          EL3RST
#define ELIBACC         ELIBACC
#define ELIBBAD         ELIBBAD
#define ELIBEXEC        ELIBEXEC
#define ELIBMAX         ELIBMAX
#define ELIBSCN         ELIBSCN
#define ELNRNG          ELNRNG
#define ELOOP           ELOOP
#define EMEDIUMTYPE     EMEDIUMTYPE
#define EMFILE          EMFILE
#define EMLINK          EMLINK
#define EMSGSIZE        EMSGSIZE
#define EMULTIHOP       EMULTIHOP
#define ENAMETOOLONG    ENAMETOOLONG
#define ENAVAIL         ENAVAIL
#define ENEEDAUTH       ENEEDAUTH
#define ENETDOWN        ENETDOWN
#define ENETRESET       ENETRESET
#define ENETUNREACH     ENETUNREACH
#define ENFILE          ENFILE
#define ENOANO          ENOANO
#define ENOATTR         ENOATTR
#define ENOBUFS         ENOBUFS
#define ENOCSI          ENOCSI
#define ENODATA         ENODATA
#define ENODEV          ENODEV
#define ENOENT          ENOENT
#define ENOEXEC         ENOEXEC
#define ENOKEY          ENOKEY
#define ENOLCK          ENOLCK
#define ENOLINK         ENOLINK
#define ENOMEDIUM       ENOMEDIUM
#define ENOMEM          ENOMEM
#define ENOMSG          ENOMSG
#define ENONET          ENONET
#define ENOPKG          ENOPKG
#define ENOPOLICY       ENOPOLICY
#define ENOPROTOOPT     ENOPROTOOPT
#define ENOSPC          ENOSPC
#define ENOSR           ENOSR
#define ENOSTR          ENOSTR
#define ENOSYS          ENOSYS
#define ENOTBLK         ENOTBLK
#define ENOTCONN        ENOTCONN
#define ENOTDIR         ENOTDIR
#define ENOTEMPTY       ENOTEMPTY
#define ENOTNAM         ENOTNAM
#define ENOTRECOVERABLE ENOTRECOVERABLE
#define ENOTSOCK        ENOTSOCK
#define ENOTSUP         ENOTSUP
#define ENOTTY          ENOTTY
#define ENOTUNIQ        ENOTUNIQ
#define ENXIO           ENXIO
#define EOPNOTSUPP      EOPNOTSUPP
#define EOVERFLOW       EOVERFLOW
#define EOWNERDEAD      EOWNERDEAD
#define EPERM           EPERM
#define EPFNOSUPPORT    EPFNOSUPPORT
#define EPIPE           EPIPE
#define EPROCLIM        EPROCLIM
#define EPROCUNAVAIL    EPROCUNAVAIL
#define EPROGMISMATCH   EPROGMISMATCH
#define EPROGUNAVAIL    EPROGUNAVAIL
#define EPROTO          EPROTO
#define EPROTONOSUPPORT EPROTONOSUPPORT
#define EPROTOTYPE      EPROTOTYPE
#define EPWROFF         EPWROFF
#define ERANGE          ERANGE
#define EREMCHG         EREMCHG
#define EREMOTE         EREMOTE
#define EREMOTEIO       EREMOTEIO
#define ERESTART        ERESTART
#define ERFKILL         ERFKILL
#define EROFS           EROFS
#define ERPCMISMATCH    ERPCMISMATCH
#define ESHLIBVERS      ESHLIBVERS
#define ESHUTDOWN       ESHUTDOWN
#define ESOCKTNOSUPPORT ESOCKTNOSUPPORT
#define ESPIPE          ESPIPE
#define ESRCH           ESRCH
#define ESRMNT          ESRMNT
#define ESTALE          ESTALE
#define ESTRPIPE        ESTRPIPE
#define ETIME           ETIME
#define ETIMEDOUT       ETIMEDOUT
#define ETOOMANYREFS    ETOOMANYREFS
#define ETXTBSY         ETXTBSY
#define EUCLEAN         EUCLEAN
#define EUNATCH         EUNATCH
#define EUSERS          EUSERS
#define EWOULDBLOCK     EAGAIN
#define EXDEV           EXDEV
#define EXFULL          EXFULL

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ERRNO_H_ */
