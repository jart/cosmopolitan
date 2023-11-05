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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/iovec.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/thread.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#ifdef __x86_64__

struct WinsockBlockResources {
  int64_t handle;
  struct NtOverlapped *overlap;
};

static void UnwindWinsockBlock(void *arg) {
  struct WinsockBlockResources *wbr = arg;
  uint32_t got, flags;
  CancelIoEx(wbr->handle, wbr->overlap);
  WSAGetOverlappedResult(wbr->handle, wbr->overlap, &got, true, &flags);
  WSACloseEvent(wbr->overlap->hEvent);
}

static void CancelWinsockBlock(int64_t handle, struct NtOverlapped *overlap) {
  if (!CancelIoEx(handle, overlap)) {
    unassert(WSAGetLastError() == kNtErrorNotFound);
  }
}

textwindows ssize_t
__winsock_block(int64_t handle, uint32_t flags, bool nonblock,
                uint32_t srwtimeout, sigset_t waitmask,
                int StartSocketOp(int64_t handle, struct NtOverlapped *overlap,
                                  uint32_t *flags, void *arg),
                void *arg) {

  int rc;
  int sig = 0;
  uint32_t status;
  uint32_t exchanged;
  int olderror = errno;
  bool eagained = false;
  bool canceled = false;
  int handler_was_called;
  struct PosixThread *pt;

RestartOperation:
  struct NtOverlapped overlap = {.hEvent = WSACreateEvent()};
  struct WinsockBlockResources wbr = {handle, &overlap};
  pthread_cleanup_push(UnwindWinsockBlock, &wbr);
  rc = StartSocketOp(handle, &overlap, &flags, arg);
  if (rc && WSAGetLastError() == kNtErrorIoPending) {
    if (nonblock) {
      CancelWinsockBlock(handle, &overlap);
      eagained = true;
    } else if (_check_cancel()) {
      CancelWinsockBlock(handle, &overlap);
      canceled = true;
    } else if ((sig = __sig_get(waitmask))) {
      CancelWinsockBlock(handle, &overlap);
    } else {
      pt = _pthread_self();
      pt->pt_blkmask = waitmask;
      pt->pt_iohandle = handle;
      pt->pt_ioverlap = &overlap;
      atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_IO,
                            memory_order_release);
      status = WSAWaitForMultipleEvents(1, &overlap.hEvent, 0,
                                        srwtimeout ? srwtimeout : -1u, 0);
      atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
      if (status == kNtWaitTimeout) {
        // SO_RCVTIMEO or SO_SNDTIMEO elapsed
        CancelWinsockBlock(handle, &overlap);
        eagained = true;
      }
    }
    rc = 0;
  }
  if (!rc) {
    rc = WSAGetOverlappedResult(handle, &overlap, &exchanged, true, &flags)
             ? 0
             : -1;
  }
  pthread_cleanup_pop(false);
  WSACloseEvent(overlap.hEvent);

  if (canceled) {
    return ecanceled();
  }
  if (sig) {
    handler_was_called = __sig_relay(sig, SI_KERNEL, waitmask);
    if (_check_cancel() == -1) return -1;
  } else {
    handler_was_called = 0;
  }
  if (!rc) {
    errno = olderror;
    return exchanged;
  }
  if (WSAGetLastError() == kNtErrorOperationAborted) {
    if (eagained) return eagain();
    if (!handler_was_called && (sig = __sig_get(waitmask))) {
      handler_was_called = __sig_relay(sig, SI_KERNEL, waitmask);
      if (_check_cancel() == -1) return -1;
    }
    if (handler_was_called != 1) {
      goto RestartOperation;
    }
    return eintr();
  }
  return __winsockerr();
}

#endif /* __x86_64__ */
