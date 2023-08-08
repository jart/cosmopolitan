/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/enum/wsa.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static textwindows void __wsablock_abort(int64_t handle,
                                         struct NtOverlapped *overlapped) {
  unassert(CancelIoEx(handle, overlapped) ||
           GetLastError() == kNtErrorNotFound);
}

textwindows int __wsablock(struct Fd *fd, struct NtOverlapped *overlapped,
                           uint32_t *flags, int sigops, uint32_t timeout) {
  uint32_t i, got;
  int rc, abort_errno;
  if (WSAGetLastError() != kNtErrorIoPending) {
    // our i/o operation never happened because it failed
    return __winsockerr();
  }
  // our i/o operation is in flight and it needs to block
  abort_errno = EAGAIN;
  if (fd->flags & O_NONBLOCK) {
    __wsablock_abort(fd->handle, overlapped);
  } else if (_check_interrupts(sigops, g_fds.p)) {
  Interrupted:
    abort_errno = errno;  // EINTR or ECANCELED
    __wsablock_abort(fd->handle, overlapped);
  } else {
    for (;;) {
      i = WSAWaitForMultipleEvents(1, &overlapped->hEvent, true,
                                   __SIG_POLLING_INTERVAL_MS, true);
      if (i == kNtWaitFailed || i == kNtWaitTimeout) {
        if (_check_interrupts(sigops, g_fds.p)) {
          goto Interrupted;
        }
        if (i == kNtWaitFailed) {
          // Failure should be an impossible condition, but MSDN lists
          // WSAENETDOWN and WSA_NOT_ENOUGH_MEMORY as possible errors,
          // which we're going to hope are ephemeral.
          SleepEx(__SIG_POLLING_INTERVAL_MS, false);
        }
        if (timeout) {
          if (timeout <= __SIG_POLLING_INTERVAL_MS) {
            __wsablock_abort(fd->handle, overlapped);
            break;
          }
          timeout -= __SIG_POLLING_INTERVAL_MS;
        }
      } else {
        break;
      }
    }
  }
  // overlapped is allocated on stack by caller, so it's important that
  // we wait for win32 to acknowledge that it's done using that memory.
  if (WSAGetOverlappedResult(fd->handle, overlapped, &got, true, flags)) {
    return got;
  } else if (WSAGetLastError() == kNtErrorOperationAborted) {
    errno = abort_errno;
    return -1;
  } else {
    return -1;
  }
}
