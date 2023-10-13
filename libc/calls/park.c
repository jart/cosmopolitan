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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

static textwindows int _park_thread(uint32_t msdelay, sigset_t waitmask,
                                    bool restartable) {
  int rc;
  int64_t sem;
  sigset_t om;
  uint32_t wi;
  struct PosixThread *pt;
  pt = _pthread_self();
  pt->pt_flags &= ~PT_RESTARTABLE;
  if (restartable) pt->pt_flags |= PT_RESTARTABLE;
  pt->pt_semaphore = sem = CreateSemaphore(0, 0, 1, 0);
  pthread_cleanup_push((void *)CloseHandle, (void *)sem);
  atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_SEM, memory_order_release);
  om = __sig_beginwait(waitmask);
  if ((rc = _check_cancel()) != -1 && (rc = _check_signal(restartable)) != -1) {
    unassert((wi = WaitForSingleObject(sem, msdelay)) != -1u);
    if (wi != kNtWaitTimeout) {
      _check_signal(false);
      rc = eintr();
      _check_cancel();
    } else if ((rc = _check_signal(restartable))) {
      _check_cancel();
    }
  }
  __sig_finishwait(om);
  atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_CPU, memory_order_release);
  pt->pt_flags &= ~PT_RESTARTABLE;
  pthread_cleanup_pop(true);
  pt->pt_semaphore = 0;
  return rc;
}

textwindows int _park_norestart(uint32_t msdelay, sigset_t waitmask) {
  return _park_thread(msdelay, waitmask, false);
}

textwindows int _park_restartable(uint32_t msdelay, sigset_t waitmask) {
  return _park_thread(msdelay, waitmask, true);
}

#endif /* __x86_64__ */
