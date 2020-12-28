/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/notice.inc"
#include "libc/macros.h"
.source	__FILE__

/	Embeds ASCII names for errno constants into binary.
	.section .rodata,"aS",@progbits
kErrnoNames:
/	<SORTED-ORDER-C>
	.asciz	"2BIG"
	.asciz	"ACCES"
	.asciz	"ADDRINUSE"
	.asciz	"ADDRNOTAVAIL"
	.asciz	"ADV"
	.asciz	"AFNOSUPPORT"
	.asciz	"AGAIN"
	.asciz	"ALREADY"
	.asciz	"BADE"
	.asciz	"BADF"
	.asciz	"BADFD"
	.asciz	"BADMSG"
	.asciz	"BADR"
	.asciz	"BADRQC"
	.asciz	"BADSLT"
	.asciz	"BFONT"
	.asciz	"BUSY"
	.asciz	"CANCELED"
	.asciz	"CHILD"
	.asciz	"CHRNG"
	.asciz	"COMM"
	.asciz	"CONNABORTED"
	.asciz	"CONNREFUSED"
	.asciz	"CONNRESET"
	.asciz	"DEADLK"
	.asciz	"DESTADDRREQ"
	.asciz	"DOM"
	.asciz	"DOTDOT"
	.asciz	"DQUOT"
	.asciz	"EXIST"
	.asciz	"FAULT"
	.asciz	"FBIG"
	.asciz	"HOSTDOWN"
	.asciz	"HOSTUNREACH"
	.asciz	"HWPOISON"
	.asciz	"IDRM"
	.asciz	"ILSEQ"
	.asciz	"INPROGRESS"
	.asciz	"INTR"
	.asciz	"INVAL"
	.asciz	"IO"
	.asciz	"ISCONN"
	.asciz	"ISDIR"
	.asciz	"ISNAM"
	.asciz	"KEYEXPIRED"
	.asciz	"KEYREJECTED"
	.asciz	"KEYREVOKED"
	.asciz	"L2HLT"
	.asciz	"L2NSYNC"
	.asciz	"L3HLT"
	.asciz	"L3RST"
	.asciz	"LIBACC"
	.asciz	"LIBBAD"
	.asciz	"LIBEXEC"
	.asciz	"LIBMAX"
	.asciz	"LIBSCN"
	.asciz	"LNRNG"
	.asciz	"LOOP"
	.asciz	"MEDIUMTYPE"
	.asciz	"MFILE"
	.asciz	"MLINK"
	.asciz	"MSGSIZE"
	.asciz	"MULTIHOP"
	.asciz	"NAMETOOLONG"
	.asciz	"NAVAIL"
	.asciz	"NETDOWN"
	.asciz	"NETRESET"
	.asciz	"NETUNREACH"
	.asciz	"NFILE"
	.asciz	"NOANO"
	.asciz	"NOBUFS"
	.asciz	"NOCSI"
	.asciz	"NODATA"
	.asciz	"NODEV"
	.asciz	"NOENT"
	.asciz	"NOEXEC"
	.asciz	"NOKEY"
	.asciz	"NOLCK"
	.asciz	"NOLINK"
	.asciz	"NOMEDIUM"
	.asciz	"NOMEM"
	.asciz	"NOMSG"
	.asciz	"NONET"
	.asciz	"NOPKG"
	.asciz	"NOPROTOOPT"
	.asciz	"NOSPC"
	.asciz	"NOSR"
	.asciz	"NOSTR"
	.asciz	"NOSYS"
	.asciz	"NOTBLK"
	.asciz	"NOTCONN"
	.asciz	"NOTDIR"
	.asciz	"NOTEMPTY"
	.asciz	"NOTNAM"
	.asciz	"NOTRECOVERABLE"
	.asciz	"NOTSOCK"
	.asciz	"NOTSUP"
	.asciz	"NOTTY"
	.asciz	"NOTUNIQ"
	.asciz	"NXIO"
	.asciz	"OPNOTSUPP"
	.asciz	"OVERFLOW"
	.asciz	"OWNERDEAD"
	.asciz	"PERM"
	.asciz	"PFNOSUPPORT"
	.asciz	"PIPE"
	.asciz	"PROTO"
	.asciz	"PROTONOSUPPORT"
	.asciz	"PROTOTYPE"
	.asciz	"RANGE"
	.asciz	"REMCHG"
	.asciz	"REMOTE"
	.asciz	"REMOTEIO"
	.asciz	"RESTART"
	.asciz	"RFKILL"
	.asciz	"ROFS"
	.asciz	"SHUTDOWN"
	.asciz	"SOCKTNOSUPPORT"
	.asciz	"SPIPE"
	.asciz	"SRCH"
	.asciz	"SRMNT"
	.asciz	"STALE"
	.asciz	"STRPIPE"
	.asciz	"TIME"
	.asciz	"TIMEDOUT"
	.asciz	"TOOMANYREFS"
	.asciz	"TXTBSY"
	.asciz	"UCLEAN"
	.asciz	"UNATCH"
	.asciz	"USERS"
	.asciz	"XDEV"
	.asciz	"XFULL"
/	</SORTED-ORDER-C>
	.byte	0
	.endobj	kErrnoNames,globl,hidden
	.previous

/	Pulls errno constants into linkage.
/
/	@assume	linker relocates these in sorted order
/	@assume	linker invoked as LC_ALL=C ld ...
/	@see	libc/sysv/systemfive.S
/	@see	libc/sysv/consts/syscon.h
	.yoink	E2BIG
	.yoink	EACCES
	.yoink	EADDRINUSE
	.yoink	EADDRNOTAVAIL
	.yoink	EADV
	.yoink	EAFNOSUPPORT
	.yoink	EAGAIN
	.yoink	EALREADY
	.yoink	EBADE
	.yoink	EBADF
	.yoink	EBADFD
	.yoink	EBADMSG
	.yoink	EBADR
	.yoink	EBADRQC
	.yoink	EBADSLT
	.yoink	EBFONT
	.yoink	EBUSY
	.yoink	ECANCELED
	.yoink	ECHILD
	.yoink	ECHRNG
	.yoink	ECOMM
	.yoink	ECONNABORTED
	.yoink	ECONNREFUSED
	.yoink	ECONNRESET
	.yoink	EDEADLK
	.yoink	EDESTADDRREQ
	.yoink	EDOM
	.yoink	EDOTDOT
	.yoink	EDQUOT
	.yoink	EEXIST
	.yoink	EFAULT
	.yoink	EFBIG
	.yoink	EHOSTDOWN
	.yoink	EHOSTUNREACH
	.yoink	EHWPOISON
	.yoink	EIDRM
	.yoink	EILSEQ
	.yoink	EINPROGRESS
	.yoink	EINTR
	.yoink	EINVAL
	.yoink	EIO
	.yoink	EISCONN
	.yoink	EISDIR
	.yoink	EISNAM
	.yoink	EKEYEXPIRED
	.yoink	EKEYREJECTED
	.yoink	EKEYREVOKED
	.yoink	EL2HLT
	.yoink	EL2NSYNC
	.yoink	EL3HLT
	.yoink	EL3RST
	.yoink	ELIBACC
	.yoink	ELIBBAD
	.yoink	ELIBEXEC
	.yoink	ELIBMAX
	.yoink	ELIBSCN
	.yoink	ELNRNG
	.yoink	ELOOP
	.yoink	EMEDIUMTYPE
	.yoink	EMFILE
	.yoink	EMLINK
	.yoink	EMSGSIZE
	.yoink	EMULTIHOP
	.yoink	ENAMETOOLONG
	.yoink	ENAVAIL
	.yoink	ENETDOWN
	.yoink	ENETRESET
	.yoink	ENETUNREACH
	.yoink	ENFILE
	.yoink	ENOANO
	.yoink	ENOBUFS
	.yoink	ENOCSI
	.yoink	ENODATA
	.yoink	ENODEV
	.yoink	ENOENT
	.yoink	ENOEXEC
	.yoink	ENOKEY
	.yoink	ENOLCK
	.yoink	ENOLINK
	.yoink	ENOMEDIUM
	.yoink	ENOMEM
	.yoink	ENOMSG
	.yoink	ENONET
	.yoink	ENOPKG
	.yoink	ENOPROTOOPT
	.yoink	ENOSPC
	.yoink	ENOSR
	.yoink	ENOSTR
	.yoink	ENOSYS
	.yoink	ENOTBLK
	.yoink	ENOTCONN
	.yoink	ENOTDIR
	.yoink	ENOTEMPTY
	.yoink	ENOTNAM
	.yoink	ENOTRECOVERABLE
	.yoink	ENOTSOCK
	.yoink	ENOTSUP
	.yoink	ENOTTY
	.yoink	ENOTUNIQ
	.yoink	ENXIO
	.yoink	EOPNOTSUPP
	.yoink	EOVERFLOW
	.yoink	EOWNERDEAD
	.yoink	EPERM
	.yoink	EPFNOSUPPORT
	.yoink	EPIPE
	.yoink	EPROTO
	.yoink	EPROTONOSUPPORT
	.yoink	EPROTOTYPE
	.yoink	ERANGE
	.yoink	EREMCHG
	.yoink	EREMOTE
	.yoink	EREMOTEIO
	.yoink	ERESTART
	.yoink	ERFKILL
	.yoink	EROFS
	.yoink	ESHUTDOWN
	.yoink	ESOCKTNOSUPPORT
	.yoink	ESPIPE
	.yoink	ESRCH
	.yoink	ESRMNT
	.yoink	ESTALE
	.yoink	ESTRPIPE
	.yoink	ETIME
	.yoink	ETIMEDOUT
	.yoink	ETOOMANYREFS
	.yoink	ETXTBSY
	.yoink	EUCLEAN
	.yoink	EUNATCH
	.yoink	EUSERS
	.yoink	EXDEV
	.yoink	EXFULL

	.type	kErrnoStart,@object
	.type	kErrnoEnd,@object
	.globl	kErrnoStart, kErrnoEnd, EXFULL, E2BIG
	.hidden	kErrnoStart, kErrnoEnd
	kErrnoStart = E2BIG
	kErrnoEnd = EXFULL + 8
