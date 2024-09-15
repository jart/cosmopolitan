/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.h"
#include "libc/nt/events.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#ifdef __x86_64__

textwindows static int sys_sigtimedwait_nt_check(sigset_t syncsigs,
                                                 siginfo_t *opt_info,
                                                 sigset_t waitmask) {
  int sig;
  if (_check_cancel() == -1)
    return -1;
  if ((sig = __sig_get(waitmask))) {
    if ((1ull << (sig - 1)) & syncsigs) {
      if (opt_info) {
        memset(opt_info, 0, sizeof(*opt_info));
        opt_info->si_signo = sig;
        opt_info->si_code = SI_TKILL;
        opt_info->si_uid = sys_getuid_nt();
      }
      return sig;
    }
    int handler_was_called = __sig_relay(sig, SI_TKILL, waitmask);
    if (_check_cancel() == -1)
      return -1;
    if (handler_was_called)
      return eintr();
  }
  return 0;
}

textwindows static int sys_sigtimedwait_nt_impl(sigset_t syncsigs,
                                                siginfo_t *opt_info,
                                                struct timespec deadline,
                                                sigset_t waitmask,
                                                intptr_t semaphore) {
  for (;;) {
    int sig;
    if ((sig = sys_sigtimedwait_nt_check(syncsigs, opt_info, waitmask)))
      return sig;
    struct timespec now = sys_clock_gettime_monotonic_nt();
    if (timespec_cmp(now, deadline) >= 0)
      return eagain();
    struct timespec remain = timespec_sub(deadline, now);
    int64_t millis = timespec_tomillis(remain);
    uint32_t waitms = MIN(millis, 0xffffffffu);
    uint32_t wi = WaitForSingleObject(semaphore, waitms);
    if (wi == -1u)
      return __winerr();
    if (wi)
      return eagain();
  }
}

textwindows int sys_sigtimedwait_nt(const sigset_t *set, siginfo_t *opt_info,
                                    const struct timespec *opt_timeout) {
  int rc;
  intptr_t sev;
  struct PosixThread *pt;
  struct timespec deadline;
  sigset_t syncsigs, waitmask;
  BLOCK_SIGNALS;
  if (opt_timeout) {
    deadline = timespec_add(sys_clock_gettime_monotonic_nt(), *opt_timeout);
  } else {
    deadline = timespec_max;
  }
  if ((sev = CreateEvent(0, 0, 0, 0))) {
    syncsigs = *set & ~(1ull << (SIGTHR - 1));  // internal to pthreads
    waitmask = ~syncsigs & _SigMask;
    pt = _pthread_self();
    pt->pt_event = sev;
    pt->pt_blkmask = waitmask;
    atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_EVENT,
                          memory_order_release);
    rc = sys_sigtimedwait_nt_impl(syncsigs, opt_info, deadline, waitmask, sev);
    atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
    CloseHandle(sev);
  } else {
    rc = __winerr();
  }
  ALLOW_SIGNALS;
  return rc;
}

#endif /* __x86_64__ */
