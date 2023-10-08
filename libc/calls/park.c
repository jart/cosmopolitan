/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

// each thread has its own pt_futex which is used by both posix signals
// and posix thread cancelation to "park" blocking operations that dont
// need win32 overlapped i/o. the delay is advisory and may be -1 which
// means wait forever. these functions don't guarantee to wait the full
// duration. other threads wanting to deliver a signal, can wake parked
// futexes without releasing them, just to stir up activity. if a futex
// is both woken and released then the cancelation point shall generate
// an eintr. we also abstract checking for signals & thread cancelation

static textwindows int _park_wait(uint32_t msdelay, bool restartable,
                                  struct PosixThread *pt) {
  int got, expect = 0;
  if (_check_cancel() == -1) return -1;
  if (_check_signal(restartable) == -1) return -1;
  WaitOnAddress(&pt->pt_futex, &expect, sizeof(expect), msdelay);
  got = atomic_load_explicit(&pt->pt_futex, memory_order_acquire);
  return got != expect ? eintr() : 0;
}

static textwindows int _park_thread(uint32_t msdelay, sigset_t waitmask,
                                    bool restartable) {
  int rc;
  sigset_t om;
  struct PosixThread *pt;
  pt = _pthread_self();
  pt->pt_flags &= ~PT_RESTARTABLE;
  if (restartable) pt->pt_flags |= PT_RESTARTABLE;
  atomic_store_explicit(&pt->pt_futex, 0, memory_order_release);
  atomic_store_explicit(&pt->pt_blocker, &pt->pt_futex, memory_order_release);
  om = __sig_beginwait(waitmask);
  rc = _park_wait(msdelay, restartable, pt);
  if (rc == -1 && errno == EINTR) _check_cancel();
  __sig_finishwait(om);
  atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_CPU, memory_order_release);
  pt->pt_flags &= ~PT_RESTARTABLE;
  return rc;
}

textwindows int _park_norestart(uint32_t msdelay, sigset_t waitmask) {
  return _park_thread(msdelay, waitmask, false);
}

textwindows int _park_restartable(uint32_t msdelay, sigset_t waitmask) {
  return _park_thread(msdelay, waitmask, true);
}

#endif /* __x86_64__ */
