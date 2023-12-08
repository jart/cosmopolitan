/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/thread.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#ifdef __x86_64__

textwindows ssize_t
__winsock_block(int64_t handle, uint32_t flags, bool nonblock,
                uint32_t srwtimeout, sigset_t waitmask,
                int StartSocketOp(int64_t handle, struct NtOverlapped *overlap,
                                  uint32_t *flags, void *arg),
                void *arg) {

RestartOperation:
  int rc, sig, reason = 0;
  uint32_t status, exchanged;
  if (_check_cancel() == -1) return -1;  // ECANCELED
  if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
    goto HandleInterrupt;
  }

  struct NtOverlapped overlap = {.hEvent = WSACreateEvent()};
  rc = StartSocketOp(handle, &overlap, &flags, arg);
  if (rc && WSAGetLastError() == kNtErrorIoPending) {
    if (nonblock) {
      CancelIoEx(handle, &overlap);
      reason = EAGAIN;
    } else {
      struct PosixThread *pt;
      pt = _pthread_self();
      pt->pt_blkmask = waitmask;
      pt->pt_iohandle = handle;
      pt->pt_ioverlap = &overlap;
      atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_IO,
                            memory_order_release);
      status = WSAWaitForMultipleEvents(1, &overlap.hEvent, 0,
                                        srwtimeout ? srwtimeout : -1u, 0);
      atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
      if (status) {
        if (status == kNtWaitTimeout) {
          reason = EAGAIN;  // SO_RCVTIMEO or SO_SNDTIMEO elapsed
        } else {
          reason = WSAGetLastError();  // ENETDOWN or ENOBUFS
        }
        CancelIoEx(handle, &overlap);
      }
    }
    rc = 0;
  }
  if (!rc) {
    rc = WSAGetOverlappedResult(handle, &overlap, &exchanged, true, &flags)
             ? 0
             : -1;
  }
  WSACloseEvent(overlap.hEvent);

  if (!rc) {
    return exchanged;
  }
  if (WSAGetLastError() == kNtErrorOperationAborted) {
    if (reason) {
      errno = reason;
      return -1;
    }
    if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
    HandleInterrupt:
      int handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
      if (_check_cancel() == -1) return -1;
      if (handler_was_called != 1) goto RestartOperation;
    }
    return eintr();
  }
  return __winsockerr();
}

#endif /* __x86_64__ */
