/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│ vi: set noet ft=sh ts=8 sts=8 sw=8 fenc=utf-8                            :vi │
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
dir=libc/sysv/dos2errno
. libc/sysv/gen.sh

# DOS ERRNO MULTIMAPPINGS
#
# These mappings are defined in a decentralized section which are
# grouped at linktime into kDos2Errno. (libc/intrin/kdos2errno.S)
#
dos	kNtErrorInvalidTargetHandle	EBADF
dos	kNtErrorDirectAccessHandle	EBADF
dos	kNtErrorModNotFound             ENOSYS
dos	kNtErrorBadCommand              EACCES
dos	kNtErrorBadLength               EACCES
dos	kNtErrorBadNetpath              ENOENT
dos	kNtErrorBadNetName              ENOENT
dos	kNtErrorBadPathname             ENOENT
dos	kNtErrorBadNetResp              ENETDOWN
dos	kNtErrorFileExists              EEXIST
dos	kNtErrorCannotMake              EACCES
dos	kNtErrorCommitmentLimit         ENOMEM
dos	kNtErrorConnectionAborted	ECONNABORTED
dos	kNtErrorConnectionActive	EISCONN
dos	kNtErrorConnectionRefused	ECONNREFUSED
dos	kNtErrorCrc                     EACCES
dos	kNtErrorDirNotEmpty             ENOTEMPTY
dos	kNtErrorDupName                 EADDRINUSE
dos	kNtErrorFilenameExcedRange	ENAMETOOLONG
dos	kNtErrorGenFailure              EACCES
dos	kNtErrorGracefulDisconnect	EPIPE
dos	kNtErrorHostDown                EHOSTUNREACH
dos	kNtErrorHostUnreachable         EHOSTUNREACH
dos	kNtErrorInsufficientBuffer	ENOBUFS
dos	kNtErrorNoaccess                EFAULT
dos	kNtErrorInvalidAddress          EADDRNOTAVAIL
dos	kNtErrorNotAReparsePoint	EINVAL
dos	kNtErrorInvalidFunction         EINVAL
dos	kNtErrorNegativeSeek		EINVAL
dos	kNtErrorInvalidName		EINVAL
dos	kNtErrorInvalidNetname          EADDRNOTAVAIL
dos	kNtErrorInvalidUserBuffer	EMSGSIZE
dos	kNtErrorIoPending		EINPROGRESS
dos	kNtErrorLockViolation           EAGAIN
dos	kNtErrorMoreData                EMSGSIZE
dos	kNtErrorNetnameDeleted          ECONNABORTED
dos	kNtErrorNetworkAccessDenied	EACCES
dos	kNtErrorNetworkBusy             ENETDOWN
dos	kNtErrorNetworkUnreachable	ENETUNREACH
dos	kNtErrorNonpagedSystemResources	ENOMEM
dos	kNtErrorNotEnoughMemory         ENOMEM
dos	kNtErrorNotEnoughQuota          ENOMEM
dos	kNtErrorNotFound                ENOENT
dos	kNtErrorNotReady                EACCES
dos	kNtErrorNotSupported            ENOTSUP
dos	kNtErrorNoMoreFiles             ENOENT
dos	kNtErrorNoSystemResources	ENOMEM
dos	kNtErrorOperationAborted	EINTR
dos	kNtErrorOutOfPaper              EACCES
dos	kNtErrorPagedSystemResources	ENOMEM
dos	kNtErrorPagefileQuota           ENOMEM
dos	kNtErrorPipeNotConnected	EPIPE
dos	kNtErrorPortUnreachable         ECONNRESET
dos	kNtErrorProtocolUnreachable	ENETUNREACH
dos	kNtErrorRemNotList              ECONNREFUSED
dos	kNtErrorRequestAborted          EINTR
dos	kNtErrorReqNotAccep             EWOULDBLOCK
dos	kNtErrorSectorNotFound          EACCES
dos	kNtErrorSemTimeout              ETIMEDOUT
dos	kNtErrorSharingViolation	EACCES
dos	kNtErrorTooManyNames            ENOMEM
dos	kNtErrorUnexpNetErr             ECONNABORTED
dos	kNtErrorWorkingSetQuota         ENOMEM
dos	kNtErrorWriteProtect            EACCES
dos	kNtErrorWrongDisk               EACCES
dos	kNtErrorExeMarkedInvalid	ENOEXEC
dos	kNtErrorExeMachineTypeMismatch	ENOEXEC
dos	WSAEACCES                       EACCES
dos	WSAEDISCON                      EPIPE
dos	WSAEFAULT                       EFAULT
dos	WSAEINVAL                       EINVAL
dos	WSAEDQUOT                       EDQUOT
dos	WSAEPROCLIM                     ENOMEM
dos	WSANOTINITIALISED               ENETDOWN
dos	WSASYSNOTREADY                  ENETDOWN
dos	WSAVERNOTSUPPORTED              ENOSYS
dos	WSAETIMEDOUT			ETIMEDOUT
