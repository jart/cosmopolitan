#ifndef COSMOPOLITAN_LIBC_ERRNO_H_
#define COSMOPOLITAN_LIBC_ERRNO_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview System error codes.
 * @see libc/sysv/consts.sh for numbers
 */

#if defined(__GNUC__) && defined(__x86_64__) && defined(__MNO_RED_ZONE__) && \
    !defined(__STRICT_ANSI__)
#define errno                                                           \
  (*({                                                                  \
    errno_t *_ep;                                                       \
    asm("call\t__errno_location" : "=a"(_ep) : /* no inputs */ : "cc"); \
    _ep;                                                                \
  }))
#elif defined(__GNUC__) && defined(__aarch64__) && \
    !defined(__STRICT_ANSI__) && !defined(__cplusplus)
#define errno                             \
  (*({                                    \
    errno_t *_ep;                         \
    asm("sub\t%0,x28,#1092" : "=r"(_ep)); \
    _ep;                                  \
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
 * Inappropriate file type or format. (BSD only)
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

#define __tmpcosmo_E2BIG           -15823698
#define __tmpcosmo_EACCES          -15823580
#define __tmpcosmo_EADDRINUSE      -15823756
#define __tmpcosmo_EADDRNOTAVAIL   -15823592
#define __tmpcosmo_EADV            -15823574
#define __tmpcosmo_EAFNOSUPPORT    -15823748
#define __tmpcosmo_EAGAIN          -15823506
#define __tmpcosmo_EALREADY        -15823530
#define __tmpcosmo_EAUTH           -15823702
#define __tmpcosmo_EBADARCH        -15823738
#define __tmpcosmo_EBADE           -15823740
#define __tmpcosmo_EBADEXEC        -15823684
#define __tmpcosmo_EBADF           -15823744
#define __tmpcosmo_EBADFD          -15823554
#define __tmpcosmo_EBADMACHO       -15823618
#define __tmpcosmo_EBADMSG         -15823650
#define __tmpcosmo_EBADR           -15823570
#define __tmpcosmo_EBADRPC         -15823626
#define __tmpcosmo_EBADRQC         -15823688
#define __tmpcosmo_EBADSLT         -15823788
#define __tmpcosmo_EBUSY           -15823550
#define __tmpcosmo_ECANCELED       -15823676
#define __tmpcosmo_ECHILD          -15823662
#define __tmpcosmo_ECHRNG          -15823722
#define __tmpcosmo_ECOMM           -15823634
#define __tmpcosmo_ECONNABORTED    -15823616
#define __tmpcosmo_ECONNREFUSED    -15823556
#define __tmpcosmo_ECONNRESET      -15823548
#define __tmpcosmo_EDEADLK         -15823718
#define __tmpcosmo_EDESTADDRREQ    -15823658
#define __tmpcosmo_EDEVERR         -15823518
#define __tmpcosmo_EDOM            -15823798
#define __tmpcosmo_EDOTDOT         -15823726
#define __tmpcosmo_EDQUOT          -15823620
#define __tmpcosmo_EEXIST          -15823594
#define __tmpcosmo_EFAULT          -15823686
#define __tmpcosmo_EFBIG           -15823768
#define __tmpcosmo_EFTYPE          -15823568
#define __tmpcosmo_EHOSTDOWN       -15823596
#define __tmpcosmo_EHOSTUNREACH    -15823742
#define __tmpcosmo_EHWPOISON       -15823680
#define __tmpcosmo_EIDRM           -15823644
#define __tmpcosmo_EILSEQ          -15823540
#define __tmpcosmo_EINPROGRESS     -15823720
#define __tmpcosmo_EINTR           -15823710
#define __tmpcosmo_EINVAL          -15823624
#define __tmpcosmo_EIO             -15823544
#define __tmpcosmo_EISCONN         -15823704
#define __tmpcosmo_EISDIR          -15823758
#define __tmpcosmo_EISNAM          -15823682
#define __tmpcosmo_EKEYEXPIRED     -15823520
#define __tmpcosmo_EKEYREJECTED    -15823712
#define __tmpcosmo_EKEYREVOKED     -15823780
#define __tmpcosmo_EL2HLT          -15823510
#define __tmpcosmo_EL2NSYNC        -15823670
#define __tmpcosmo_EL3HLT          -15823792
#define __tmpcosmo_EL3RST          -15823654
#define __tmpcosmo_ELIBACC         -15823708
#define __tmpcosmo_ELIBBAD         -15823564
#define __tmpcosmo_ELIBEXEC        -15823696
#define __tmpcosmo_ELIBMAX         -15823724
#define __tmpcosmo_ELIBSCN         -15823786
#define __tmpcosmo_ELNRNG          -15823732
#define __tmpcosmo_ELOOP           -15823672
#define __tmpcosmo_EMEDIUMTYPE     -15823508
#define __tmpcosmo_EMFILE          -15823762
#define __tmpcosmo_EMLINK          -15823694
#define __tmpcosmo_EMSGSIZE        -15823536
#define __tmpcosmo_EMULTIHOP       -15823750
#define __tmpcosmo_ENAMETOOLONG    -15823600
#define __tmpcosmo_ENAVAIL         -15823656
#define __tmpcosmo_ENEEDAUTH       -15823766
#define __tmpcosmo_ENETDOWN        -15823730
#define __tmpcosmo_ENETRESET       -15823604
#define __tmpcosmo_ENETUNREACH     -15823524
#define __tmpcosmo_ENFILE          -15823700
#define __tmpcosmo_ENOANO          -15823734
#define __tmpcosmo_ENOATTR         -15823606
#define __tmpcosmo_ENOBUFS         -15823628
#define __tmpcosmo_ENOCSI          -15823760
#define __tmpcosmo_ENODATA         -15823516
#define __tmpcosmo_ENODEV          -15823774
#define __tmpcosmo_ENOENT          -15823590
#define __tmpcosmo_ENOEXEC         -15823512
#define __tmpcosmo_ENOKEY          -15823764
#define __tmpcosmo_ENOLCK          -15823782
#define __tmpcosmo_ENOLINK         -15823538
#define __tmpcosmo_ENOMEDIUM       -15823598
#define __tmpcosmo_ENOMEM          -15823514
#define __tmpcosmo_ENOMSG          -15823796
#define __tmpcosmo_ENONET          -15823642
#define __tmpcosmo_ENOPKG          -15823664
#define __tmpcosmo_ENOPOLICY       -15823716
#define __tmpcosmo_ENOPROTOOPT     -15823608
#define __tmpcosmo_ENOSPC          -15823646
#define __tmpcosmo_ENOSR           -15823558
#define __tmpcosmo_ENOSTR          -15823706
#define __tmpcosmo_ENOSYS          -15823636
#define __tmpcosmo_ENOTBLK         -15823640
#define __tmpcosmo_ENOTCONN        -15823778
#define __tmpcosmo_ENOTDIR         -15823648
#define __tmpcosmo_ENOTEMPTY       -15823552
#define __tmpcosmo_ENOTNAM         -15823532
#define __tmpcosmo_ENOTRECOVERABLE -15823746
#define __tmpcosmo_ENOTSOCK        -15823582
#define __tmpcosmo_ENOTSUP         -15823602
#define __tmpcosmo_ENOTTY          -15823528
#define __tmpcosmo_ENOTUNIQ        -15823790
#define __tmpcosmo_ENXIO           -15823622
#define __tmpcosmo_EOPNOTSUPP      -15823588
#define __tmpcosmo_EOVERFLOW       -15823736
#define __tmpcosmo_EOWNERDEAD      -15823562
#define __tmpcosmo_EPERM           -15823754
#define __tmpcosmo_EPFNOSUPPORT    -15823690
#define __tmpcosmo_EPIPE           -15823534
#define __tmpcosmo_EPROCLIM        -15823610
#define __tmpcosmo_EPROCUNAVAIL    -15823546
#define __tmpcosmo_EPROGMISMATCH   -15823572
#define __tmpcosmo_EPROGUNAVAIL    -15823526
#define __tmpcosmo_EPROTO          -15823678
#define __tmpcosmo_EPROTONOSUPPORT -15823576
#define __tmpcosmo_EPROTOTYPE      -15823614
#define __tmpcosmo_EPWROFF         -15823692
#define __tmpcosmo_ERANGE          -15823772
#define __tmpcosmo_EREMCHG         -15823666
#define __tmpcosmo_EREMOTE         -15823560
#define __tmpcosmo_EREMOTEIO       -15823794
#define __tmpcosmo_ERESTART        -15823728
#define __tmpcosmo_ERFKILL         -15823612
#define __tmpcosmo_EROFS           -15823566
#define __tmpcosmo_ERPCMISMATCH    -15823542
#define __tmpcosmo_ESHLIBVERS      -15823584
#define __tmpcosmo_ESHUTDOWN       -15823660
#define __tmpcosmo_ESOCKTNOSUPPORT -15823776
#define __tmpcosmo_ESPIPE          -15823652
#define __tmpcosmo_ESRCH           -15823674
#define __tmpcosmo_ESRMNT          -15823714
#define __tmpcosmo_ESTALE          -15823632
#define __tmpcosmo_ESTRPIPE        -15823770
#define __tmpcosmo_ETIME           -15823630
#define __tmpcosmo_ETIMEDOUT       -15823522
#define __tmpcosmo_ETOOMANYREFS    -15823586
#define __tmpcosmo_ETXTBSY         -15823638
#define __tmpcosmo_EUCLEAN         -15823578
#define __tmpcosmo_EUNATCH         -15823504
#define __tmpcosmo_EUSERS          -15823668
#define __tmpcosmo_EXDEV           -15823752
#define __tmpcosmo_EXFULL          -15823784

extern errno_t __errno;
errno_t *__errno_location(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ERRNO_H_ */
