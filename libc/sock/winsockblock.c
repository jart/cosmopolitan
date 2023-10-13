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
                uint32_t srwtimeout, sigset_t wait_signal_mask,
                int StartSocketOp(int64_t handle, struct NtOverlapped *overlap,
                                  uint32_t *flags, void *arg),
                void *arg) {

  int rc;
  uint64_t m;
  uint32_t status;
  uint32_t exchanged;
  int olderror = errno;
  bool eagained = false;
  bool eintered = false;
  bool canceled = false;
  struct PosixThread *pt;
  struct NtOverlapped overlap = {.hEvent = WSACreateEvent()};
  struct WinsockBlockResources wbr = {handle, &overlap};

  pthread_cleanup_push(UnwindWinsockBlock, &wbr);
  rc = StartSocketOp(handle, &overlap, &flags, arg);
  if (rc && WSAGetLastError() == kNtErrorIoPending) {
  BlockingOperation:
    pt = _pthread_self();
    pt->pt_iohandle = handle;
    pt->pt_ioverlap = &overlap;
    pt->pt_flags |= PT_RESTARTABLE;
    atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_IO, memory_order_release);
    m = __sig_beginwait(wait_signal_mask);
    if (nonblock) {
      CancelWinsockBlock(handle, &overlap);
      eagained = true;
    } else if (_check_cancel()) {
      CancelWinsockBlock(handle, &overlap);
      canceled = true;
    } else if (_check_signal(true)) {
      CancelWinsockBlock(handle, &overlap);
      eintered = true;
    } else {
      status = WSAWaitForMultipleEvents(1, &overlap.hEvent, 0,
                                        srwtimeout ? srwtimeout : -1u, 0);
      if (status == kNtWaitTimeout) {
        // rcvtimeo or sndtimeo elapsed
        CancelWinsockBlock(handle, &overlap);
        eagained = true;
      } else if (status == kNtWaitFailed) {
        // Failure should be an impossible condition, but MSDN lists
        // WSAENETDOWN and WSA_NOT_ENOUGH_MEMORY as possible errors.
        CancelWinsockBlock(handle, &overlap);
        eintered = true;
      }
    }
    __sig_finishwait(m);
    atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_CPU,
                          memory_order_release);
    pt->pt_flags &= ~PT_RESTARTABLE;
    pt->pt_ioverlap = 0;
    pt->pt_iohandle = 0;
    rc = 0;
  }
  if (!rc) {
    bool32 should_wait = canceled || eagained;
    bool32 ok = WSAGetOverlappedResult(handle, &overlap, &exchanged,
                                       should_wait, &flags);
    if (!ok && WSAGetLastError() == kNtErrorIoIncomplete) {
      goto BlockingOperation;
    }
    rc = ok ? 0 : -1;
  }
  WSACloseEvent(overlap.hEvent);
  pthread_cleanup_pop(false);

  if (canceled) {
    return ecanceled();
  }
  if (!rc) {
    errno = olderror;
    return exchanged;
  }
  if (eagained) {
    return eagain();
  }
  if (GetLastError() == kNtErrorOperationAborted) {
    if (_check_cancel() == -1) return ecanceled();
    if (!eintered && _check_signal(false)) return eintr();
  }
  if (eintered) {
    return eintr();
  }
  return __winsockerr();
}

#endif /* __x86_64__ */
