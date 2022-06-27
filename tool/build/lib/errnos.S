/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
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
#include "libc/macros.internal.h"

	.macro	.e	local:req linux:req
	.globl	\local
	.long	\local-kLinuxErrnos
	.byte	\linux
	.endm

//	Lookup table translating errnos between systems.
//
//	@see	libc/sysv/systemfive.S
	.rodata
	.align	4
kLinuxErrnos:
	.e	EPERM,1
	.e	ENOENT,2
	.e	ESRCH,3
	.e	EINTR,4
	.e	EIO,5
	.e	ENXIO,6
	.e	E2BIG,7
	.e	ENOEXEC,8
	.e	EBADF,9
	.e	ECHILD,10
	.e	EAGAIN,11
	.e	ENOMEM,12
	.e	EACCES,13
	.e	EFAULT,14
	.e	ENOTBLK,15
	.e	EBUSY,16
	.e	EEXIST,17
	.e	EXDEV,18
	.e	ENODEV,19
	.e	ENOTDIR,20
	.e	EISDIR,21
	.e	EINVAL,22
	.e	ENFILE,23
	.e	EMFILE,24
	.e	ENOTTY,25
	.e	ETXTBSY,26
	.e	EFBIG,27
	.e	ENOSPC,28
	.e	ESPIPE,29
	.e	EROFS,30
	.e	EMLINK,31
	.e	EPIPE,32
	.e	EDOM,33
	.e	ERANGE,34
	.e	EDEADLK,35
	.e	ENAMETOOLONG,36
	.e	ENOLCK,37
	.e	ENOSYS,38
	.e	ENOTEMPTY,39
	.e	ELOOP,40
	.e	ENOMSG,42
	.e	EIDRM,43
	.e	EUSERS,87
	.e	ENOTSOCK,88
	.e	EDESTADDRREQ,89
	.e	EMSGSIZE,90
	.e	EPROTOTYPE,91
	.e	ENOPROTOOPT,92
	.e	EPROTONOSUPPORT,93
	.e	ESOCKTNOSUPPORT,94
	.e	EOPNOTSUPP,95
	.e	EPFNOSUPPORT,96
	.e	EAFNOSUPPORT,97
	.e	EADDRINUSE,98
	.e	EADDRNOTAVAIL,99
	.e	EL2NSYNC,45
	.e	EL3HLT,46
	.e	EL3RST,47
	.e	ELNRNG,48
	.e	ETIME,62
	.e	ENONET,64
	.e	EREMOTE,66
	.e	EPROTO,71
	.e	EBADMSG,74
	.e	EOVERFLOW,75
	.e	EILSEQ,84
	.e	ERESTART,85
	.e	ENETDOWN,100
	.e	ENETUNREACH,101
	.e	ENETRESET,102
	.e	ECONNABORTED,103
	.e	ECONNRESET,104
	.e	ENOBUFS,105
	.e	EISCONN,106
	.e	ENOTCONN,107
	.e	ESHUTDOWN,108
	.e	ETOOMANYREFS,109
	.e	ETIMEDOUT,110
	.e	ECONNREFUSED,111
	.e	EHOSTDOWN,112
	.e	EHOSTUNREACH,113
	.e	EALREADY,114
	.e	EINPROGRESS,115
	.e	ESTALE,116
	.e	EDQUOT,122
	.e	ECANCELED,125
	.e	EOWNERDEAD,130
	.e	ENOTRECOVERABLE,131
	.long	0
	.byte	0
	.endobj	kLinuxErrnos,globl
