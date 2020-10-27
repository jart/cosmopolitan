#ifndef COSMOPOLITAN_LIBC_ERRNO_H_
#define COSMOPOLITAN_LIBC_ERRNO_H_

/**
 * @fileoverview System error codes.
 * @see libc/sysv/consts.sh for numbers
 */

#define EPERM           EPERM            // operation not permitted
#define ENOENT          ENOENT           // no such file or directory
#define ESRCH           ESRCH            // no such process
#define EINTR           EINTR            // interrupted system call
#define EIO             EIO              // input/output error
#define ENXIO           ENXIO            // no such device or address
#define E2BIG           E2BIG            // argument list too long
#define ENOEXEC         ENOEXEC          // exec format error
#define EBADF           EBADF            // bad file descriptor
#define ECHILD          ECHILD           // no child processes
#define EAGAIN          EAGAIN           // resource temporarily unavailable
#define ENOMEM          ENOMEM           // not enough space
#define EACCES          EACCES           // permission denied
#define EFAULT          EFAULT           // bad address
#define ENOTBLK         ENOTBLK          // block device required
#define EBUSY           EBUSY            // device or resource busy
#define EEXIST          EEXIST           // file exists
#define EXDEV           EXDEV            // improper link
#define ENODEV          ENODEV           // no such device
#define ENOTDIR         ENOTDIR          // not a directory
#define EISDIR          EISDIR           // is a directory
#define EINVAL          EINVAL           // invalid argument
#define ENFILE          ENFILE           // too many open files in system
#define EMFILE          EMFILE           // too many open files
#define ENOTTY          ENOTTY           // inappropriate I/O control op
#define ETXTBSY         ETXTBSY          // text file busy
#define EFBIG           EFBIG            // file too large
#define ENOSPC          ENOSPC           // no space left on device
#define ESPIPE          ESPIPE           // invalid seek
#define EROFS           EROFS            // read-only filesystem
#define EMLINK          EMLINK           // too many links
#define EPIPE           EPIPE            // broken pipe
#define EDOM            EDOM             // argument out of function domain
#define ERANGE          ERANGE           // result too large
#define EDEADLK         EDEADLK          // resource deadlock avoided
#define ENAMETOOLONG    ENAMETOOLONG     // filename too long
#define ENOLCK          ENOLCK           // no locks available
#define ENOSYS          ENOSYS           // system call not implemented
#define ENOTEMPTY       ENOTEMPTY        // directory not empty
#define ELOOP           ELOOP            // too many levels of symbolic links
#define ENOMSG          ENOMSG           // no message of the desired type
#define EIDRM           EIDRM            // identifier removed
#define ECHRNG          ECHRNG           // channel number out of range
#define EL2NSYNC        EL2NSYNC         // level 2 not synchronized
#define EL3HLT          EL3HLT           // level 3 halted
#define EL3RST          EL3RST           // level 3 halted
#define ELNRNG          ELNRNG           // link number out of range
#define EUNATCH         EUNATCH          // protocol driver not attached
#define ENOCSI          ENOCSI           // no csi structure available
#define EL2HLT          EL2HLT           // level 2 halted
#define EBADE           EBADE            // invalid exchange
#define EBADR           EBADR            // invalid request descriptor
#define EXFULL          EXFULL           // exchange full
#define ENOANO          ENOANO           // no anode
#define EBADRQC         EBADRQC          // invalid request code
#define EBADSLT         EBADSLT          // invalid slot
#define ENOSTR          ENOSTR           // no string
#define ENODATA         ENODATA          // no data
#define ETIME           ETIME            // timer expired
#define ENOSR           ENOSR            // out of streams resources
#define ENONET          ENONET           // no network
#define ENOPKG          ENOPKG           // package not installed
#define EREMOTE         EREMOTE          // object is remote
#define ENOLINK         ENOLINK          // link severed
#define EADV            EADV             // todo
#define ESRMNT          ESRMNT           // todo
#define ECOMM           ECOMM            // communication error on send
#define EPROTO          EPROTO           // protocol error
#define EMULTIHOP       EMULTIHOP        // multihop attempted
#define EDOTDOT         EDOTDOT          // todo
#define EBADMSG         EBADMSG          // bad message
#define EOVERFLOW       EOVERFLOW        // value too large for type
#define ENOTUNIQ        ENOTUNIQ         // name not unique on network
#define EBADFD          EBADFD           // fd in bad *state* (cf. EBADF)
#define EREMCHG         EREMCHG          // remote address changed
#define ELIBACC         ELIBACC          // cannot access dso
#define ELIBBAD         ELIBBAD          // corrupted shared library
#define ELIBSCN         ELIBSCN          // a.out section corrupted
#define ELIBMAX         ELIBMAX          // too many shared libraries
#define ELIBEXEC        ELIBEXEC         // cannot exec a dso directly
#define EILSEQ          EILSEQ           // invalid wide character
#define ERESTART        ERESTART         // please restart syscall
#define ESTRPIPE        ESTRPIPE         // streams pipe error
#define EUSERS          EUSERS           // too many users
#define ENOTSOCK        ENOTSOCK         // not a socket
#define EDESTADDRREQ    EDESTADDRREQ     // dest address needed
#define EMSGSIZE        EMSGSIZE         // message too long
#define EPROTOTYPE      EPROTOTYPE       // protocol wrong for socket
#define ENOPROTOOPT     ENOPROTOOPT      // protocol not available
#define EPROTONOSUPPORT EPROTONOSUPPORT  // protocol not supported
#define ESOCKTNOSUPPORT ESOCKTNOSUPPORT  // socket type not supported
#define EOPNOTSUPP      EOPNOTSUPP       // operation not supported on socket
#define EPFNOSUPPORT    EPFNOSUPPORT     // protocol family not supported
#define EAFNOSUPPORT    EAFNOSUPPORT     // address family not supported
#define EADDRINUSE      EADDRINUSE       // address already in use
#define EADDRNOTAVAIL   EADDRNOTAVAIL    // address not available
#define ENETDOWN        ENETDOWN         // network is down
#define ENETUNREACH     ENETUNREACH      // network unreachable
#define ENETRESET       ENETRESET        // connection aborted by network
#define ECONNABORTED    ECONNABORTED     // connection aborted
#define ECONNRESET      ECONNRESET       // connection reset
#define ENOBUFS         ENOBUFS          // no buffer space available
#define EISCONN         EISCONN          // socket is connected
#define ENOTCONN        ENOTCONN         // the socket is not connected
#define ESHUTDOWN       ESHUTDOWN        // no send after endpoint shutdown
#define ETOOMANYREFS    ETOOMANYREFS     // too many refs
#define ETIMEDOUT       ETIMEDOUT        // connection timed out
#define ECONNREFUSED    ECONNREFUSED     // connection refused
#define EHOSTDOWN       EHOSTDOWN        // host is down
#define EHOSTUNREACH    EHOSTUNREACH     // host is unreachable
#define EALREADY        EALREADY         // connection already in progress
#define EINPROGRESS     EINPROGRESS      // operation in progress
#define ESTALE          ESTALE           // stale file handle
#define EUCLEAN         EUCLEAN          // structure needs cleaning
#define ENOTNAM         ENOTNAM          // todo
#define ENAVAIL         ENAVAIL          // todo
#define EISNAM          EISNAM           // is a named type file
#define EREMOTEIO       EREMOTEIO        // remote i/o error
#define EDQUOT          EDQUOT           // disk quota exceeded
#define ENOMEDIUM       ENOMEDIUM        // no medium found
#define EMEDIUMTYPE     EMEDIUMTYPE      // wrong medium type
#define ECANCELED       ECANCELED        // operation canceled
#define ENOKEY          ENOKEY           // required key not available
#define EKEYEXPIRED     EKEYEXPIRED      // key has expired
#define EKEYREVOKED     EKEYREVOKED      // key has been revoked
#define EKEYREJECTED    EKEYREJECTED     // key was rejected by service
#define EOWNERDEAD      EOWNERDEAD       // owner died
#define ENOTRECOVERABLE ENOTRECOVERABLE  // state not recoverable
#define ERFKILL         ERFKILL          // can't op b/c RF-kill
#define EHWPOISON       EHWPOISON        // mempage has h/w error
#define EWOULDBLOCK     EAGAIN           // poll fd and try again
#define ENOTSUP         ENOTSUP

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern errno_t errno;

hidden extern const long EPERM;
hidden extern const long ENOENT;
hidden extern const long ESRCH;
hidden extern const long EINTR;
hidden extern const long EIO;
hidden extern const long ENXIO;
hidden extern const long E2BIG;
hidden extern const long ENOEXEC;
hidden extern const long EBADF;
hidden extern const long ECHILD;
hidden extern const long EAGAIN;
hidden extern const long ENOMEM;
hidden extern const long EACCES;
hidden extern const long EFAULT;
hidden extern const long ENOTBLK;
hidden extern const long EBUSY;
hidden extern const long EEXIST;
hidden extern const long EXDEV;
hidden extern const long ENODEV;
hidden extern const long ENOTDIR;
hidden extern const long EISDIR;
hidden extern const long EINVAL;
hidden extern const long ENFILE;
hidden extern const long EMFILE;
hidden extern const long ENOTTY;
hidden extern const long ETXTBSY;
hidden extern const long EFBIG;
hidden extern const long ENOSPC;
hidden extern const long ESPIPE;
hidden extern const long EROFS;
hidden extern const long EMLINK;
hidden extern const long EPIPE;
hidden extern const long EDOM;
hidden extern const long ERANGE;
hidden extern const long EDEADLK;
hidden extern const long ENAMETOOLONG;
hidden extern const long ENOLCK;
hidden extern const long ENOSYS;
hidden extern const long ENOTEMPTY;
hidden extern const long ELOOP;
hidden extern const long ENOMSG;
hidden extern const long EIDRM;
hidden extern const long ECHRNG;
hidden extern const long EL2NSYNC;
hidden extern const long EL3HLT;
hidden extern const long EL3RST;
hidden extern const long ELNRNG;
hidden extern const long EUNATCH;
hidden extern const long ENOCSI;
hidden extern const long EL2HLT;
hidden extern const long EBADE;
hidden extern const long EBADR;
hidden extern const long EXFULL;
hidden extern const long ENOANO;
hidden extern const long EBADRQC;
hidden extern const long EBADSLT;
hidden extern const long ENOSTR;
hidden extern const long ENODATA;
hidden extern const long ETIME;
hidden extern const long ENOSR;
hidden extern const long ENONET;
hidden extern const long ENOPKG;
hidden extern const long EREMOTE;
hidden extern const long ENOLINK;
hidden extern const long EADV;
hidden extern const long ESRMNT;
hidden extern const long ECOMM;
hidden extern const long EPROTO;
hidden extern const long EMULTIHOP;
hidden extern const long EDOTDOT;
hidden extern const long EBADMSG;
hidden extern const long EOVERFLOW;
hidden extern const long ENOTUNIQ;
hidden extern const long EBADFD;
hidden extern const long EREMCHG;
hidden extern const long ELIBACC;
hidden extern const long ELIBBAD;
hidden extern const long ELIBSCN;
hidden extern const long ELIBMAX;
hidden extern const long ELIBEXEC;
hidden extern const long EILSEQ;
hidden extern const long ERESTART;
hidden extern const long ESTRPIPE;
hidden extern const long EUSERS;
hidden extern const long ENOTSOCK;
hidden extern const long EDESTADDRREQ;
hidden extern const long EMSGSIZE;
hidden extern const long EPROTOTYPE;
hidden extern const long ENOPROTOOPT;
hidden extern const long EPROTONOSUPPORT;
hidden extern const long ESOCKTNOSUPPORT;
hidden extern const long EOPNOTSUPP;
hidden extern const long EPFNOSUPPORT;
hidden extern const long EAFNOSUPPORT;
hidden extern const long EADDRINUSE;
hidden extern const long EADDRNOTAVAIL;
hidden extern const long ENETDOWN;
hidden extern const long ENETUNREACH;
hidden extern const long ENETRESET;
hidden extern const long ECONNABORTED;
hidden extern const long ECONNRESET;
hidden extern const long ENOBUFS;
hidden extern const long EISCONN;
hidden extern const long ENOTCONN;
hidden extern const long ESHUTDOWN;
hidden extern const long ETOOMANYREFS;
hidden extern const long ETIMEDOUT;
hidden extern const long ECONNREFUSED;
hidden extern const long EHOSTDOWN;
hidden extern const long EHOSTUNREACH;
hidden extern const long EALREADY;
hidden extern const long EINPROGRESS;
hidden extern const long ESTALE;
hidden extern const long EUCLEAN;
hidden extern const long ENOTNAM;
hidden extern const long ENAVAIL;
hidden extern const long EISNAM;
hidden extern const long EREMOTEIO;
hidden extern const long EDQUOT;
hidden extern const long ENOMEDIUM;
hidden extern const long EMEDIUMTYPE;
hidden extern const long ECANCELED;
hidden extern const long ENOKEY;
hidden extern const long EKEYEXPIRED;
hidden extern const long EKEYREVOKED;
hidden extern const long EKEYREJECTED;
hidden extern const long EOWNERDEAD;
hidden extern const long ENOTRECOVERABLE;
hidden extern const long ERFKILL;
hidden extern const long EHWPOISON;
hidden extern const long ENOTSUP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ERRNO_H_ */
