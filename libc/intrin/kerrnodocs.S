/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/macros.internal.h"

	.macro	.e e s
	.long	\e - kErrnoDocs
	.long	.L\@ - kErrnoDocs
	.rodata.str1.1
.L\@:	.asciz	"\s"
	.previous
	.endm

	.section .rodata
	.balign 4
	.underrun
kErrnoDocs:
	.e	EINVAL,"Invalid argument"
	.e	ENOSYS,"Function not implemented"
	.e	EPERM,"Operation not permitted"
	.e	ENOENT,"No such file or directory"
	.e	ESRCH,"No such process"
	.e	EINTR,"Interrupted system call"
	.e	EIO,"I/O error"
	.e	ENXIO,"No such device or address"
	.e	E2BIG,"Arg list too long"
	.e	ENOEXEC,"Exec format error"
	.e	EBADF,"Bad file number"
	.e	ECHILD,"No child processes"
	.e	EAGAIN,"Try again"
	.e	ENOMEM,"Out of memory"
	.e	EACCES,"Permission denied"
	.e	EFAULT,"Bad address"
	.e	ENOTBLK,"Block device required"
	.e	EBUSY,"Device or resource busy"
	.e	EEXIST,"File exists"
	.e	EXDEV,"Cross-device link"
	.e	ENODEV,"No such device"
	.e	ENOTDIR,"Not a directory"
	.e	EISDIR,"Is a directory"
	.e	ENFILE,"File table overflow"
	.e	EMFILE,"Too many open files"
	.e	ENOTTY,"Not a typewriter"
	.e	ETXTBSY,"Text file busy"
	.e	EFBIG,"File too large"
	.e	ENOSPC,"No space left on device"
	.e	EDQUOT,"Quota exceeded"
	.e	ESPIPE,"Illegal seek"
	.e	EROFS,"Read-only file system"
	.e	EMLINK,"Too many links"
	.e	EPIPE,"Broken pipe"
	.e	EDOM,"Math argument out of domain of func"
	.e	ERANGE,"Math result not representable"
	.e	EDEADLK,"Resource deadlock would occur"
	.e	ENAMETOOLONG,"File name too long"
	.e	ENOLCK,"No record locks available"
	.e	ENOTEMPTY,"Directory not empty"
	.e	ELOOP,"Too many symbolic links encountered"
	.e	ENOMSG,"No message of desired type"
	.e	EIDRM,"Identifier removed"
	.e	EPROTO,"Protocol error"
	.e	EOVERFLOW,"Value too large for defined data type"
	.e	EILSEQ,"Illegal byte sequence"
	.e	EUSERS,"Too many users"
	.e	ENOTSOCK,"Socket operation on non-socket"
	.e	EDESTADDRREQ,"Destination address required"
	.e	EMSGSIZE,"Message too long"
	.e	EPROTOTYPE,"Protocol wrong type for socket"
	.e	ENOPROTOOPT,"Protocol not available"
	.e	EPROTONOSUPPORT,"Protocol not supported"
	.e	ESOCKTNOSUPPORT,"Socket type not supported"
	.e	ENOTSUP,"Operation not supported"
	.e	EOPNOTSUPP,"Operation not supported on transport endpoint"
	.e	EPFNOSUPPORT,"Protocol family not supported"
	.e	EAFNOSUPPORT,"Address family not supported by protocol"
	.e	EADDRINUSE,"Address already in use"
	.e	EADDRNOTAVAIL,"Cannot assign requested address"
	.e	ENETDOWN,"Network is down"
	.e	ENETUNREACH,"Network is unreachable"
	.e	ENETRESET,"Network dropped connection because of reset"
	.e	ECONNABORTED,"Software caused connection abort"
	.e	ECONNRESET,"Connection reset by peer"
	.e	ENOBUFS,"No buffer space available"
	.e	EISCONN,"Transport endpoint is already connected"
	.e	ENOTCONN,"Transport endpoint is not connected"
	.e	ESHUTDOWN,"Cannot send after transport endpoint shutdown"
	.e	ETOOMANYREFS,"Too many references: cannot splice"
	.e	ETIMEDOUT,"Connection timed out"
	.e	ETIME,"Timer expired"
	.e	ECONNREFUSED,"Connection refused"
	.e	EHOSTDOWN,"Host is down"
	.e	EHOSTUNREACH,"No route to host"
	.e	EALREADY,"Operation already in progress"
	.e	EINPROGRESS,"Operation now in progress"
	.e	ESTALE,"Stale NFS file handle"
	.e	EREMOTE,"Object is remote"
	.e	EBADMSG,"Not a data message"
	.e	ECANCELED,"Operation Canceled"
	.e	EOWNERDEAD,"Owner died"
	.e	ENOTRECOVERABLE,"State not recoverable"
	.e	ENONET,"Machine is not on the network"
	.e	ERESTART,"Interrupted system call should be restarted"
	.e	EBADFD,"File descriptor in bad state"
	.long	MAGNUM_TERMINATOR
	.endobj	kErrnoDocs,globl,hidden
	.overrun
