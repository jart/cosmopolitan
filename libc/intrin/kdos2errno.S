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
#include "libc/nt/errors.h"
#include "libc/macros.internal.h"

//	@fileoverview data structure for __dos2errno()

	.macro	.e doscode systemv
	.short	\doscode
	.long	\systemv - kDos2Errno
	.endm

	.section .rodata
	.underrun
kDos2Errno:
//	.e	kNtErrorInvalidFunction,ENOSYS		# in consts.sh
//	.e	kNtErrorFileNotFound,ENOENT		# in consts.sh
//	.e	kNtErrorPathNotFound,ENOTDIR		# in consts.sh
//	.e	kNtErrorTooManyOpenFiles,EMFILE		# in consts.sh
//	.e	kNtErrorTooManyDescriptors,ENFILE	# in consts.sh
//	.e	kNtErrorTooManyLinks,EMLINK		# in consts.sh
//	.e	kNtErrorAccessDenied,EACCES		# in consts.sh
//	.e	kNtErrorInvalidHandle,EBADF		# in consts.sh
//	.e	kNtErrorInvalidAccess,EPERM		# in consts.sh
//	.e	kNtErrorNotEnoughQuota,EDQUOT		# in consts.sh
//	.e	kNtErrorSeek,ESPIPE			# in consts.sh
//	.e	kNtErrorNotDosDisk,ENOTBLK		# in consts.sh
//	.e	kNtErrorFileExists,EEXIST		# in consts.sh
//	.e	kNtErrorInvalidParameter,EINVAL		# in consts.sh
//	.e	kNtErrorOutofmemory,ENOMEM		# in consts.sh
//	.e	kNtErrorBrokenPipe,EPIPE		# in consts.sh
//	.e	kNtErrorWaitNoChildren,ECHILD		# in consts.sh
//	.e	kNtErrorPathBusy,ETXTBSY		# in consts.sh
//	.e	kNtErrorBusy,EBUSY			# in consts.sh
//	.e	kNtErrorAlreadyExists,EEXIST		# in consts.sh
//	.e	kNtErrorBadExeFormat,ENOEXEC		# in consts.sh
//	.e	kNtErrorFileTooLarge,EFBIG		# in consts.sh
//	.e	kNtErrorDirectoryNotSupported,EISDIR	# in consts.sh
//	.e	kNtErrorInvalidAddress,EFAULT		# in consts.sh
//	.e	kNtErrorThreadNotInProcess,ESRCH	# in consts.sh
//	.e	kNtErrorNoMediaInDrive,ENXIO		# in consts.sh
//	.e	kNtErrorIoDevice,EIO			# in consts.sh
//	.e	kNtErrorSerialNoDevice,ENOTTY		# in consts.sh
//	.e	kNtErrorPossibleDeadlock,EDEADLK	# in consts.sh
//	.e	kNtErrorBadDevice,ENODEV		# in consts.sh
//	.e	kNtErrorInvalidCommandLine,E2BIG	# in consts.sh
//	.e	kNtErrorFileReadOnly,EROFS		# in consts.sh
//	.e	kNtErrorNoData,ENODATA			# in consts.sh
//	.e	WSAEPROCLIM,EPROCLIM			# in consts.sh
//	.e	WSAESHUTDOWN,ESHUTDOWN			# in consts.sh
//	.e	WSAEINPROGRESS,EINPROGRESS		# in consts.sh
//	.e	WSAENETDOWN,ENETDOWN			# in consts.sh
//	.e	WSAENETUNREACH,ENETUNREACH		# in consts.sh
//	.e	WSAENETRESET,ENETRESET			# in consts.sh
//	.e	WSAEUSERS,EUSERS			# in consts.sh
//	.e	WSAENOTSOCK,ENOTSOCK			# in consts.sh
//	.e	WSAEDESTADDRREQ,EDESTADDRREQ		# in consts.sh
//	.e	WSAEMSGSIZE,EMSGSIZE			# in consts.sh
//	.e	WSAEPROTOTYPE,EPROTOTYPE		# in consts.sh
//	.e	WSAENOPROTOOPT,ENOPROTOOPT		# in consts.sh
//	.e	WSAEPROTONOSUPPORT,EPROTONOSUPPORT	# in consts.sh
//	.e	WSAESOCKTNOSUPPORT,ESOCKTNOSUPPORT	# in consts.sh
//	.e	WSAEOPNOTSUPP,ENOTSUP			# in consts.sh
//	.e	WSAEOPNOTSUPP,EOPNOTSUPP		# in consts.sh
//	.e	WSAEPFNOSUPPORT,EPFNOSUPPORT		# in consts.sh
//	.e	WSAEAFNOSUPPORT,EAFNOSUPPORT		# in consts.sh
//	.e	WSAEADDRINUSE,EADDRINUSE		# in consts.sh
//	.e	WSAEADDRNOTAVAIL,EADDRNOTAVAIL		# in consts.sh
//	.e	WSAECONNABORTED,ECONNABORTED		# in consts.sh
//	.e	WSAECONNRESET,ECONNRESET		# in consts.sh
//	.e	WSAENOBUFS,ENOBUFS			# in consts.sh
//	.e	WSAEISCONN,EISCONN			# in consts.sh
//	.e	WSAENOTCONN,ENOTCONN			# in consts.sh
//	.e	WSAESHUTDOWN,ESHUTDOWN			# in consts.sh
//	.e	WSAETOOMANYREFS,ETOOMANYREFS		# in consts.sh
//	.e	WSAETIMEDOUT,ETIMEDOUT			# in consts.sh
//	.e	WSAECONNREFUSED,ECONNREFUSED		# in consts.sh
//	.e	WSAEHOSTDOWN,EHOSTDOWN			# in consts.sh
//	.e	WSAEHOSTUNREACH,EHOSTUNREACH		# in consts.sh
//	.e	WSAEALREADY,EALREADY			# in consts.sh
//	.e	WSAESTALE,ESTALE			# in consts.sh
//	.e	WSAEREMOTE,EREMOTE			# in consts.sh
//	.e	WSAEINTR,EINTR				# in consts.sh
	.e	kNtErrorModNotFound,ENOSYS
	.e	kNtErrorBadCommand,EACCES
	.e	kNtErrorBadLength,EACCES
	.e	kNtErrorBadNetpath,ENOENT
	.e	kNtErrorBadNetName,ENOENT
	.e	kNtErrorBadPathname,ENOENT
	.e	kNtErrorBadNetResp,ENETDOWN
	.e	kNtErrorFileExists,EEXIST
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
	.e	kNtErrorNoaccess,EFAULT
	.e	kNtErrorInvalidAddress,EADDRNOTAVAIL
	.e	kNtErrorNotAReparsePoint,EINVAL
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
	.e	WSAEINVAL,EINVAL
	.e	WSAEDQUOT,EDQUOT
	.e	WSAEPROCLIM,ENOMEM
	.e	WSANOTINITIALISED,ENETDOWN
	.e	WSASYSNOTREADY,ENETDOWN
	.e	WSAVERNOTSUPPORTED,ENOSYS
	.short	0
	.endobj	kDos2Errno,globl,hidden
	.overrun
