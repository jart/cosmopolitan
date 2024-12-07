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
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/events.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"

#ifdef __x86_64__

// returns 0 if deadline is reached
// raises EINTR if a signal delivery interrupted wait operation
// raises ECANCELED if this POSIX thread was canceled in masked mode
textwindows static int _park_thread(struct timespec deadline, sigset_t waitmask,
                                    bool restartable) {
  for (;;) {
    uint32_t handl = 0;
    intptr_t hands[2];

    // create event object
    intptr_t sigev;
    if (!(sigev = CreateEvent(0, 0, 0, 0)))
      return __winerr();
    hands[handl++] = sigev;

    // create high precision timer if needed
    if (memcmp(&deadline, &timespec_max, sizeof(struct timespec))) {
      intptr_t hTimer;
      if ((hTimer = CreateWaitableTimer(NULL, true, NULL))) {
        int64_t due = TimeSpecToWindowsTime(deadline);
        if (SetWaitableTimer(hTimer, &due, 0, NULL, NULL, false)) {
          hands[handl++] = hTimer;
        } else {
          CloseHandle(hTimer);
        }
      }
    }

    // perform wait operation
    struct PosixThread *pt = _pthread_self();
    pt->pt_event = sigev;
    pt->pt_blkmask = waitmask;
    atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_EVENT,
                          memory_order_release);
    //!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!//
    int sig = 0;
    uint32_t wi = 0;
    if (!_is_canceled() &&
        !(_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))))
      wi = WaitForMultipleObjects(handl, hands, false, -1u);
    //!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!//
    atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
    for (int i = 0; i < handl; ++i)
      CloseHandle(hands[i]);

    // recursion is now safe
    if (wi == 1)
      return 0;
    if (wi == -1u)
      return __winerr();
    int handler_was_called = 0;
    if (!sig) {
      if (_check_cancel())
        return -1;
      if (_weaken(__sig_get))
        sig = _weaken(__sig_get)(waitmask);
    }
    if (sig)
      handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
    if (_check_cancel())
      return -1;
    if (handler_was_called & SIG_HANDLED_NO_RESTART)
      return eintr();
    if (handler_was_called & SIG_HANDLED_SA_RESTART)
      if (!restartable)
        return eintr();
  }
}

textwindows int _park_norestart(struct timespec deadline, sigset_t waitmask) {
  return _park_thread(deadline, waitmask, false);
}

textwindows int _park_restartable(struct timespec deadline, sigset_t waitmask) {
  return _park_thread(deadline, waitmask, true);
}

#endif /* __x86_64__ */
