/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/errno.h"
#include "libc/nt/errors.h"
#include "libc/sock/sock.h"

/**
 * Translates Windows error using superset of consts.sh.
 */
textwindows errno_t MapDosErrorToErrno(uint32_t error) {
  switch (error) {
    case kNtErrorModNotFound:
      return ENOSYS;
    case kNtErrorBadCommand:
      return EACCES;
    case kNtErrorBadLength:
      return EACCES;
    case kNtErrorBadNetpath:
      return ENOENT;
    case kNtErrorBadNetName:
      return ENOENT;
    case kNtErrorBadNetResp:
      return ENETDOWN;
    case kNtErrorBadPathname:
      return ENOENT;
    case kNtErrorCannotMake:
      return EACCES;
    case kNtErrorCommitmentLimit:
      return ENOMEM;
    case kNtErrorConnectionAborted:
      return ECONNABORTED;
    case kNtErrorConnectionActive:
      return EISCONN;
    case kNtErrorConnectionRefused:
      return ECONNREFUSED;
    case kNtErrorCrc:
      return EACCES;
    case kNtErrorDirNotEmpty:
      return ENOTEMPTY;
    case kNtErrorDupName:
      return EADDRINUSE;
    case kNtErrorFilenameExcedRange:
      return ENOENT;
    case kNtErrorFileNotFound:
      return ENOENT;
    case kNtErrorGenFailure:
      return EACCES;
    case kNtErrorGracefulDisconnect:
      return EPIPE;
    case kNtErrorHostDown:
      return EHOSTUNREACH;
    case kNtErrorHostUnreachable:
      return EHOSTUNREACH;
    case kNtErrorInsufficientBuffer:
      return EFAULT;
    case kNtErrorInvalidAddress:
      return EADDRNOTAVAIL;
    case kNtErrorInvalidFunction:
      return EINVAL;
    case kNtErrorInvalidNetname:
      return EADDRNOTAVAIL;
    case kNtErrorInvalidUserBuffer:
      return EMSGSIZE;
    case kNtErrorIoPending:
      return EINPROGRESS;
    case kNtErrorLockViolation:
      return EACCES;
    case kNtErrorMoreData:
      return EMSGSIZE;
    case kNtErrorNetnameDeleted:
      return ECONNABORTED;
    case kNtErrorNetworkAccessDenied:
      return EACCES;
    case kNtErrorNetworkBusy:
      return ENETDOWN;
    case kNtErrorNetworkUnreachable:
      return ENETUNREACH;
    case kNtErrorNoaccess:
      return EFAULT;
    case kNtErrorNonpagedSystemResources:
      return ENOMEM;
    case kNtErrorNotEnoughMemory:
      return ENOMEM;
    case kNtErrorNotEnoughQuota:
      return ENOMEM;
    case kNtErrorNotFound:
      return ENOENT;
    case kNtErrorNotLocked:
      return EACCES;
    case kNtErrorNotReady:
      return EACCES;
    case kNtErrorNotSupported:
      return ENOTSUP;
    case kNtErrorNoMoreFiles:
      return ENOENT;
    case kNtErrorNoSystemResources:
      return ENOMEM;
    case kNtErrorOperationAborted:
      return EINTR;
    case kNtErrorOutOfPaper:
      return EACCES;
    case kNtErrorPagedSystemResources:
      return ENOMEM;
    case kNtErrorPagefileQuota:
      return ENOMEM;
    case kNtErrorPathNotFound:
      return ENOENT;
    case kNtErrorPipeNotConnected:
      return EPIPE;
    case kNtErrorPortUnreachable:
      return ECONNRESET;
    case kNtErrorProtocolUnreachable:
      return ENETUNREACH;
    case kNtErrorRemNotList:
      return ECONNREFUSED;
    case kNtErrorRequestAborted:
      return EINTR;
    case kNtErrorReqNotAccep:
      return EWOULDBLOCK;
    case kNtErrorSectorNotFound:
      return EACCES;
    case kNtErrorSemTimeout:
      return ETIMEDOUT;
    case kNtErrorSharingViolation:
      return EACCES;
    case kNtErrorTooManyNames:
      return ENOMEM;
    case kNtErrorUnexpNetErr:
      return ECONNABORTED;
    case kNtErrorWorkingSetQuota:
      return ENOMEM;
    case kNtErrorWriteProtect:
      return EACCES;
    case kNtErrorWrongDisk:
      return EACCES;
    case WSAEACCES:
      return EACCES;
    case WSAEDISCON:
      return EPIPE;
    case WSAEFAULT:
      return EFAULT;
    case WSAEINPROGRESS:
      return EBUSY;
    case WSAEINVAL:
      return EINVAL;
    case WSAEPROCLIM:
      return ENOMEM;
    case WSAESHUTDOWN:
      return EPIPE;
    case WSANOTINITIALISED:
      return ENETDOWN;
    case WSASYSNOTREADY:
      return ENETDOWN;
    case WSAVERNOTSUPPORTED:
      return ENOSYS;
    default:
      return error;
  }
}
