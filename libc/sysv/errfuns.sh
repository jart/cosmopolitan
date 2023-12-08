/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│ vi: set et ft=sh ts=2 sts=2 sw=2 fenc=utf-8                              :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚────────────────────────────────────────────────────────────────'>/dev/null #*/
dir=libc/sysv/errfuns
. libc/sysv/gen.sh

errfun eperm EPERM
errfun enoent ENOENT
errfun esrch ESRCH
errfun eintr EINTR
errfun eio EIO
errfun enxio ENXIO
errfun e2big E2BIG
errfun enoexec ENOEXEC
errfun ebadf EBADF
errfun echild ECHILD
errfun eagain EAGAIN
errfun enomem ENOMEM
errfun eacces EACCES
errfun efault EFAULT
errfun enotblk ENOTBLK
errfun ebusy EBUSY
errfun eexist EEXIST
errfun exdev EXDEV
errfun enodev ENODEV
errfun enotdir ENOTDIR
errfun eisdir EISDIR
errfun einval EINVAL
errfun enfile ENFILE
errfun emfile EMFILE
errfun enotty ENOTTY
errfun enotsup ENOTSUP
errfun etxtbsy ETXTBSY
errfun efbig EFBIG
errfun enospc ENOSPC
errfun espipe ESPIPE
errfun erofs EROFS
errfun emlink EMLINK
errfun epipe EPIPE
errfun edom EDOM
errfun erange ERANGE
errfun edeadlk EDEADLK
errfun enametoolong ENAMETOOLONG
errfun enolck ENOLCK
errfun enosys ENOSYS
errfun enotempty ENOTEMPTY
errfun eloop ELOOP
errfun enomsg ENOMSG
errfun eidrm EIDRM
errfun echrng ECHRNG
errfun el2nsync EL2NSYNC
errfun el3hlt EL3HLT
errfun el3rst EL3RST
errfun elnrng ELNRNG
errfun eunatch EUNATCH
errfun enocsi ENOCSI
errfun el2hlt EL2HLT
errfun ebade EBADE
errfun ebadr EBADR
errfun exfull EXFULL
errfun enoano ENOANO
errfun ebadrqc EBADRQC
errfun ebadslt EBADSLT
errfun enostr ENOSTR
errfun enodata ENODATA
errfun etime ETIME
errfun enosr ENOSR
errfun enonet ENONET
errfun enopkg ENOPKG
errfun eremote EREMOTE
errfun enolink ENOLINK
errfun eadv EADV
errfun esrmnt ESRMNT
errfun ecomm ECOMM
errfun eproto EPROTO
errfun emultihop EMULTIHOP
errfun edotdot EDOTDOT
errfun ebadmsg EBADMSG
errfun eoverflow EOVERFLOW
errfun enotuniq ENOTUNIQ
errfun ebadfd EBADFD
errfun eremchg EREMCHG
errfun elibacc ELIBACC
errfun elibbad ELIBBAD
errfun elibscn ELIBSCN
errfun elibmax ELIBMAX
errfun elibexec ELIBEXEC
errfun eilseq EILSEQ
errfun erestart ERESTART
errfun estrpipe ESTRPIPE
errfun eusers EUSERS
errfun enotsock ENOTSOCK
errfun edestaddrreq EDESTADDRREQ
errfun emsgsize EMSGSIZE
errfun eprototype EPROTOTYPE
errfun enoprotoopt ENOPROTOOPT
errfun eprotonosupport EPROTONOSUPPORT
errfun esocktnosupport ESOCKTNOSUPPORT
errfun eopnotsupp EOPNOTSUPP
errfun epfnosupport EPFNOSUPPORT
errfun eafnosupport EAFNOSUPPORT
errfun eaddrinuse EADDRINUSE
errfun eaddrnotavail EADDRNOTAVAIL
errfun enetdown ENETDOWN
errfun enetunreach ENETUNREACH
errfun enetreset ENETRESET
errfun econnaborted ECONNABORTED
errfun econnreset ECONNRESET
errfun enobufs ENOBUFS
errfun eisconn EISCONN
errfun enotconn ENOTCONN
errfun eshutdown ESHUTDOWN
errfun etoomanyrefs ETOOMANYREFS
errfun etimedout ETIMEDOUT
errfun econnrefused ECONNREFUSED
errfun ehostdown EHOSTDOWN
errfun ehostunreach EHOSTUNREACH
errfun ealready EALREADY
errfun einprogress EINPROGRESS
errfun estale ESTALE
errfun euclean EUCLEAN
errfun enotnam ENOTNAM
errfun enavail ENAVAIL
errfun eisnam EISNAM
errfun eremoteio EREMOTEIO
errfun edquot EDQUOT
errfun enomedium ENOMEDIUM
errfun emediumtype EMEDIUMTYPE
errfun ecanceled ECANCELED
errfun enokey ENOKEY
errfun ekeyexpired EKEYEXPIRED
errfun ekeyrevoked EKEYREVOKED
errfun ekeyrejected EKEYREJECTED
errfun eownerdead EOWNERDEAD
errfun enotrecoverable ENOTRECOVERABLE
errfun erfkill ERFKILL
errfun ehwpoison EHWPOISON
