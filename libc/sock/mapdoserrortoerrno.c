/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
