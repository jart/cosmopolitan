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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/lockcmpxchgp.h"
#include "libc/nexgen32e/threaded.h"

_Alignas(64) static int rlock;

// return 0 on success, or tid of other owner
static privileged inline int AcquireInterruptPollLock(void) {
  // any thread can poll for interrupts
  // but it's wasteful to have every single thread doing it
  int me, owner = 0;
  if (__threaded) {
    me = gettid();
    if (!_lockcmpxchgp(&rlock, &owner, me) && owner == me) {
      owner = 0;
    }
  }
  return owner;
}

static textwindows inline void ReleaseInterruptPollLock(void) {
  int zero = 0;
  __atomic_store(&rlock, &zero, __ATOMIC_RELAXED);
}

textwindows bool _check_interrupts(bool restartable, struct Fd *fd) {
  bool res;
  if (__time_critical) return false;
  if (AcquireInterruptPollLock()) return false;
  if (weaken(_check_sigalrm)) weaken(_check_sigalrm)();
  if (weaken(_check_sigchld)) weaken(_check_sigchld)();
  if (fd && weaken(_check_sigwinch)) weaken(_check_sigwinch)(fd);
  res = weaken(__sig_check) && weaken(__sig_check)(restartable);
  ReleaseInterruptPollLock();
  return res;
}
