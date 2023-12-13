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

	.macro	.e e
	.long	\e - kErrnoNames
	.long	.L\@ - kErrnoNames
	.rodata.str1.1
.L\@:	.string	"\e"
	.previous
	.endm

	.section .rodata
	.balign	4
	.underrun
kErrnoNames:
	.e	EINVAL
	.e	ENOSYS
	.e	EPERM
	.e	ENOENT
	.e	ESRCH
	.e	EINTR
	.e	EIO
	.e	ENXIO
	.e	E2BIG
	.e	ENOEXEC
	.e	EBADF
	.e	ECHILD
	.e	EAGAIN
	.e	ENOMEM
	.e	EACCES
	.e	EFAULT
	.e	ENOTBLK
	.e	EBUSY
	.e	EEXIST
	.e	EXDEV
	.e	ENODEV
	.e	ENOTDIR
	.e	EISDIR
	.e	ENFILE
	.e	EMFILE
	.e	ENOTTY
	.e	ETXTBSY
	.e	EFBIG
	.e	ENOSPC
	.e	EDQUOT
	.e	ESPIPE
	.e	EROFS
	.e	EMLINK
	.e	EPIPE
	.e	EDOM
	.e	ERANGE
	.e	EDEADLK
	.e	ENAMETOOLONG
	.e	ENOLCK
	.e	ENOTEMPTY
	.e	ELOOP
	.e	ENOMSG
	.e	EIDRM
	.e	EPROTO
	.e	EOVERFLOW
	.e	EILSEQ
	.e	EUSERS
	.e	ENOTSOCK
	.e	EDESTADDRREQ
	.e	EMSGSIZE
	.e	EPROTOTYPE
	.e	ENOPROTOOPT
	.e	EPROTONOSUPPORT
	.e	ESOCKTNOSUPPORT
	.e	ENOTSUP
	.e	EOPNOTSUPP
	.e	EPFNOSUPPORT
	.e	EAFNOSUPPORT
	.e	EADDRINUSE
	.e	EADDRNOTAVAIL
	.e	ENETDOWN
	.e	ENETUNREACH
	.e	ENETRESET
	.e	ECONNABORTED
	.e	ECONNRESET
	.e	ENOBUFS
	.e	EISCONN
	.e	ENOTCONN
	.e	ESHUTDOWN
	.e	ETOOMANYREFS
	.e	ETIMEDOUT
	.e	ETIME
	.e	ECONNREFUSED
	.e	EHOSTDOWN
	.e	EHOSTUNREACH
	.e	EALREADY
	.e	EINPROGRESS
	.e	ESTALE
	.e	EREMOTE
	.e	EBADMSG
	.e	ECANCELED
	.e	EOWNERDEAD
	.e	ENOTRECOVERABLE
	.e	ENONET
	.e	ERESTART
	.e	ENODATA
	.e	EBADFD
	.long	MAGNUM_TERMINATOR
	.endobj	kErrnoNames,globl,hidden
	.overrun
