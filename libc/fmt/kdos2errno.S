/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/nt/errors.h"
#include "libc/macros.internal.h"

//	@fileoverview data structure for __dos2errno()

	.macro	.e doscode systemv
	.short	\doscode
	.long	\systemv - kDos2Errno
	.endm

	.section .rodata
kDos2Errno:
	.e	kNtErrorModNotFound,ENOSYS
	.e	kNtErrorBadCommand,EACCES
	.e	kNtErrorBadLength,EACCES
	.e	kNtErrorBadNetpath,ENOENT
	.e	kNtErrorBadNetName,ENOENT
	.e	kNtErrorBadNetResp,ENETDOWN
	.e	kNtErrorBadPathname,ENOENT
	.e	kNtErrorCannotMake,EACCES
	.e	kNtErrorCommitmentLimit,ENOMEM
	.e	kNtErrorConnectionAborted,ECONNABORTED
	.e	kNtErrorConnectionActive,EISCONN
	.e	kNtErrorConnectionRefused,ECONNREFUSED
	.e	kNtErrorCrc,EACCES
	.e	kNtErrorDirNotEmpty,ENOTEMPTY
	.e	kNtErrorDupName,EADDRINUSE
	.e	kNtErrorFilenameExcedRange,ENOENT
	.e	kNtErrorGenFailure,EACCES
	.e	kNtErrorGracefulDisconnect,EPIPE
	.e	kNtErrorHostDown,EHOSTUNREACH
	.e	kNtErrorHostUnreachable,EHOSTUNREACH
	.e	kNtErrorInsufficientBuffer,EFAULT
	.e	kNtErrorInvalidAddress,EADDRNOTAVAIL
	.e	kNtErrorInvalidFunction,EINVAL
	.e	kNtErrorInvalidNetname,EADDRNOTAVAIL
	.e	kNtErrorInvalidUserBuffer,EMSGSIZE
	.e	kNtErrorIoPending,EINPROGRESS
	.e	kNtErrorLockViolation,EACCES
	.e	kNtErrorMoreData,EMSGSIZE
	.e	kNtErrorNetnameDeleted,ECONNABORTED
	.e	kNtErrorNetworkAccessDenied,EACCES
	.e	kNtErrorNetworkBusy,ENETDOWN
	.e	kNtErrorNetworkUnreachable,ENETUNREACH
	.e	kNtErrorNoaccess,EFAULT
	.e	kNtErrorNonpagedSystemResources,ENOMEM
	.e	kNtErrorNotEnoughMemory,ENOMEM
	.e	kNtErrorNotEnoughQuota,ENOMEM
	.e	kNtErrorNotFound,ENOENT
	.e	kNtErrorNotLocked,EACCES
	.e	kNtErrorNotReady,EACCES
	.e	kNtErrorNotSupported,ENOTSUP
	.e	kNtErrorNoMoreFiles,ENOENT
	.e	kNtErrorNoSystemResources,ENOMEM
	.e	kNtErrorOperationAborted,EINTR
	.e	kNtErrorOutOfPaper,EACCES
	.e	kNtErrorPagedSystemResources,ENOMEM
	.e	kNtErrorPagefileQuota,ENOMEM
	.e	kNtErrorPipeNotConnected,EPIPE
	.e	kNtErrorPortUnreachable,ECONNRESET
	.e	kNtErrorProtocolUnreachable,ENETUNREACH
	.e	kNtErrorRemNotList,ECONNREFUSED
	.e	kNtErrorRequestAborted,EINTR
	.e	kNtErrorReqNotAccep,EWOULDBLOCK
	.e	kNtErrorSectorNotFound,EACCES
	.e	kNtErrorSemTimeout,ETIMEDOUT
	.e	kNtErrorSharingViolation,EACCES
	.e	kNtErrorTooManyNames,ENOMEM
	.e	kNtErrorUnexpNetErr,ECONNABORTED
	.e	kNtErrorWorkingSetQuota,ENOMEM
	.e	kNtErrorWriteProtect,EACCES
	.e	kNtErrorWrongDisk,EACCES
	.e	WSAEACCES,EACCES
	.e	WSAEDISCON,EPIPE
	.e	WSAEFAULT,EFAULT
	.e	WSAEINPROGRESS,EBUSY
	.e	WSAEINVAL,EINVAL
	.e	WSAEPROCLIM,ENOMEM
	.e	WSAESHUTDOWN,EPIPE
	.e	WSANOTINITIALISED,ENETDOWN
	.e	WSASYSNOTREADY,ENETDOWN
	.e	WSAVERNOTSUPPORTED,ENOSYS
	.short	0
	.endobj	kDos2Errno,globl,hidden
