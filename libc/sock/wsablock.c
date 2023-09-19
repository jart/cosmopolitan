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
#include "libc/calls/bo.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
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
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"

textwindows int __wsablock(struct Fd *f, struct NtOverlapped *overlapped,
                           uint32_t *flags, int sigops, uint32_t timeout) {
  bool nonblock;
  int e, rc, err;
  uint32_t i, got;
  uint32_t waitfor;
  struct PosixThread *pt;
  struct timespec now, remain, interval, deadline;

  if (WSAGetLastError() != kNtErrorIoPending) {
    // our i/o operation never happened because it failed
    return __winsockerr();
  }

  // our i/o operation is in flight and it needs to block
  nonblock = !!(f->flags & O_NONBLOCK);
  pt = _pthread_self();
  pt->abort_errno = EAGAIN;
  interval = timespec_frommillis(__SIG_IO_INTERVAL_MS);
  deadline = timeout
                 ? timespec_add(timespec_real(), timespec_frommillis(timeout))
                 : timespec_max;
  e = errno;
BlockingOperation:
  if (!nonblock) {
    pt->ioverlap = overlapped;
    pt->iohandle = f->handle;
  }
  if (nonblock) {
    CancelIoEx(f->handle, overlapped);
  } else if (_check_interrupts(sigops)) {
  Interrupted:
    pt->abort_errno = errno;  // EINTR or ECANCELED
    CancelIoEx(f->handle, overlapped);
  } else {
    for (;;) {
      now = timespec_real();
      if (timespec_cmp(now, deadline) >= 0) {
        CancelIoEx(f->handle, overlapped);
        nonblock = true;
        break;
      }
      remain = timespec_sub(deadline, now);
      if (timespec_cmp(remain, interval) >= 0) {
        waitfor = __SIG_IO_INTERVAL_MS;
      } else {
        waitfor = timespec_tomillis(remain);
      }
      i = WSAWaitForMultipleEvents(1, &overlapped->hEvent, true, waitfor, true);
      if (i == kNtWaitFailed) {
        // Failure should be an impossible condition, but MSDN lists
        // WSAENETDOWN and WSA_NOT_ENOUGH_MEMORY as possible errors.
        pt->abort_errno = WSAGetLastError();
        CancelIoEx(f->handle, overlapped);
        nonblock = true;
        break;
      } else if (i == kNtWaitTimeout) {
        if (_check_interrupts(sigops)) {
          goto Interrupted;
        }
        continue;
      } else {
        break;
      }
    }
  }
  pt->ioverlap = 0;
  pt->iohandle = 0;

  // overlapped is allocated on stack by caller, so it's important that
  // we wait for win32 to acknowledge that it's done using that memory.
  if (WSAGetOverlappedResult(f->handle, overlapped, &got, nonblock, flags)) {
    rc = got;
  } else {
    rc = -1;
    err = WSAGetLastError();
    if (err == kNtErrorOperationAborted) {
      errno = pt->abort_errno;
    } else if (err == kNtErrorIoIncomplete) {
      errno = e;
      goto BlockingOperation;
    }
  }
  return rc;
}
