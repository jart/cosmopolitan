/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8                               :vi│
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

# # DOS ERRNO CANONICAL MAPPINGS
# #
# # These are defined by consts.sh.
# # They're here too as a reminder.
# #
# dos	kNtErrorInvalidFunction         ENOSYS		# in consts.sh
# dos	kNtErrorFileNotFound            ENOENT		# in consts.sh
# dos	kNtErrorPathNotFound            ENOTDIR		# in consts.sh
# dos	kNtErrorTooManyOpenFiles	EMFILE		# in consts.sh
# dos	kNtErrorTooManyDescriptors	ENFILE          # in consts.sh
# dos	kNtErrorTooManyLinks            EMLINK		# in consts.sh
# dos	kNtErrorAccessDenied            EACCES		# in consts.sh
# dos	kNtErrorInvalidHandle           EBADF		# in consts.sh
# dos	kNtErrorInvalidAccess           EPERM		# in consts.sh
# dos	kNtErrorNotEnoughQuota          EDQUOT		# in consts.sh
# dos	kNtErrorSeek                    ESPIPE		# in consts.sh
# dos	kNtErrorNotDosDisk              ENOTBLK		# in consts.sh
# dos	kNtErrorFileExists              EEXIST		# in consts.sh
# dos	kNtErrorInvalidParameter	EINVAL		# in consts.sh
# dos	kNtErrorOutofmemory             ENOMEM		# in consts.sh
# dos	kNtErrorBrokenPipe              EPIPE		# in consts.sh
# dos	kNtErrorWaitNoChildren          ECHILD		# in consts.sh
# dos	kNtErrorPathBusy                ETXTBSY		# in consts.sh
# dos	kNtErrorBusy                    EBUSY		# in consts.sh
# dos	kNtErrorAlreadyExists           EEXIST		# in consts.sh
# dos	kNtErrorBadExeFormat            ENOEXEC		# in consts.sh
# dos	kNtErrorFileTooLarge            EFBIG		# in consts.sh
# dos	kNtErrorDirectoryNotSupported	EISDIR          # in consts.sh
# dos	kNtErrorInvalidAddress          EFAULT		# in consts.sh
# dos	kNtErrorThreadNotInProcess	ESRCH           # in consts.sh
# dos	kNtErrorNoMediaInDrive          ENXIO		# in consts.sh
# dos	kNtErrorIoDevice                EIO		# in consts.sh
# dos	kNtErrorSerialNoDevice          ENOTTY		# in consts.sh
# dos	kNtErrorPossibleDeadlock	EDEADLK         # in consts.sh
# dos	kNtErrorBadDevice               ENODEV		# in consts.sh
# dos	kNtErrorInvalidCommandLine	E2BIG           # in consts.sh
# dos	kNtErrorFileReadOnly            EROFS		# in consts.sh
# dos	kNtErrorNoData                  ENODATA		# in consts.sh
# dos	WSAEPROCLIM                     EPROCLIM	# in consts.sh
# dos	WSAESHUTDOWN                    ESHUTDOWN	# in consts.sh
# dos	WSAEINPROGRESS                  EINPROGRESS	# in consts.sh
# dos	WSAENETDOWN                     ENETDOWN	# in consts.sh
# dos	WSAENETUNREACH                  ENETUNREACH	# in consts.sh
# dos	WSAENETRESET                    ENETRESET	# in consts.sh
# dos	WSAEUSERS                       EUSERS		# in consts.sh
# dos	WSAENOTSOCK                     ENOTSOCK	# in consts.sh
# dos	WSAEDESTADDRREQ                 EDESTADDRREQ	# in consts.sh
# dos	WSAEMSGSIZE                     EMSGSIZE	# in consts.sh
# dos	WSAEPROTOTYPE                   EPROTOTYPE	# in consts.sh
# dos	WSAENOPROTOOPT                  ENOPROTOOPT	# in consts.sh
# dos	WSAEPROTONOSUPPORT              EPROTONOSUPPORT	# in consts.sh
# dos	WSAESOCKTNOSUPPORT              ESOCKTNOSUPPORT	# in consts.sh
# dos	WSAEOPNOTSUPP                   ENOTSUP		# in consts.sh
# dos	WSAEOPNOTSUPP                   EOPNOTSUPP	# in consts.sh
# dos	WSAEPFNOSUPPORT                 EPFNOSUPPORT	# in consts.sh
# dos	WSAEAFNOSUPPORT                 EAFNOSUPPORT	# in consts.sh
# dos	WSAEADDRINUSE                   EADDRINUSE	# in consts.sh
# dos	WSAEADDRNOTAVAIL                EADDRNOTAVAIL	# in consts.sh
# dos	WSAECONNABORTED                 ECONNABORTED	# in consts.sh
# dos	WSAECONNRESET                   ECONNRESET	# in consts.sh
# dos	WSAENOBUFS                      ENOBUFS		# in consts.sh
# dos	WSAEISCONN                      EISCONN		# in consts.sh
# dos	WSAENOTCONN                     ENOTCONN	# in consts.sh
# dos	WSAESHUTDOWN                    ESHUTDOWN	# in consts.sh
# dos	WSAETOOMANYREFS                 ETOOMANYREFS	# in consts.sh
# dos	WSAETIMEDOUT                    ETIMEDOUT	# in consts.sh
# dos	WSAECONNREFUSED                 ECONNREFUSED	# in consts.sh
# dos	WSAEHOSTDOWN                    EHOSTDOWN	# in consts.sh
# dos	WSAEHOSTUNREACH                 EHOSTUNREACH	# in consts.sh
# dos	WSAEALREADY                     EALREADY	# in consts.sh
# dos	WSAESTALE                       ESTALE		# in consts.sh
# dos	WSAEREMOTE                      EREMOTE		# in consts.sh
# dos	WSAEINTR                        EINTR		# in consts.sh

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
dos	kNtErrorInsufficientBuffer	EFAULT
dos	kNtErrorNoaccess                EFAULT
dos	kNtErrorInvalidAddress          EADDRNOTAVAIL
dos	kNtErrorNotAReparsePoint	EINVAL
dos	kNtErrorInvalidFunction         EINVAL
dos	kNtErrorInvalidNetname          EADDRNOTAVAIL
dos	kNtErrorInvalidUserBuffer	EMSGSIZE
dos	kNtErrorIoPending               EINPROGRESS
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
dos	WSAEACCES                       EACCES
dos	WSAEDISCON                      EPIPE
dos	WSAEFAULT                       EFAULT
dos	WSAEINVAL                       EINVAL
dos	WSAEDQUOT                       EDQUOT
dos	WSAEPROCLIM                     ENOMEM
dos	WSANOTINITIALISED               ENETDOWN
dos	WSASYSNOTREADY                  ENETDOWN
dos	WSAVERNOTSUPPORTED              ENOSYS
