/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#ifdef __x86_64__

// returns 0 on timeout or spurious wakeup
// raises EINTR if a signal delivery interrupted wait operation
// raises ECANCELED if this POSIX thread was canceled in masked mode
static textwindows int _park_thread(uint32_t msdelay, sigset_t waitmask,
                                    bool restartable) {
  int sig, handler_was_called;
  if (_check_cancel() == -1) return -1;
  if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
    goto HandleSignal;
  }
  int expect = 0;
  atomic_int futex = 0;
  struct PosixThread *pt = _pthread_self();
  pt->pt_blkmask = waitmask;
  atomic_store_explicit(&pt->pt_blocker, &futex, memory_order_release);
  bool32 ok = WaitOnAddress(&futex, &expect, sizeof(int), msdelay);
  atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
  if (ok && _weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
  HandleSignal:
    handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
    if (_check_cancel() == -1) return -1;
    if (!restartable || (handler_was_called & SIG_HANDLED_NO_RESTART)) {
      return eintr();
    }
  }
  return 0;
}

textwindows int _park_norestart(uint32_t msdelay, sigset_t waitmask) {
  return _park_thread(msdelay, waitmask, false);
}

textwindows int _park_restartable(uint32_t msdelay, sigset_t waitmask) {
  return _park_thread(msdelay, waitmask, true);
}

#endif /* __x86_64__ */
