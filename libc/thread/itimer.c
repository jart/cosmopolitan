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
#include "libc/sysv/consts/itimer.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/itimerval.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/cosmo.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/itimer.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread2.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

#define STACK_SIZE 65536

textwindows dontinstrument static uint32_t __itimer_worker(void *arg) {
  struct CosmoTib tls;
  char *sp = __builtin_frame_address(0);
  __bootstrap_tls(&tls, sp);
  __maps_track(
      (char *)(((uintptr_t)sp + __pagesize - 1) & -__pagesize) - STACK_SIZE,
      STACK_SIZE, PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS | MAP_NOFORK);
  for (;;) {
    bool dosignal = false;
    struct timeval now, waituntil;
    __itimer_lock();
    now = timeval_real();
    if (timeval_iszero(__itimer.it.it_value)) {
      waituntil = timeval_max;
    } else {
      if (timeval_cmp(now, __itimer.it.it_value) < 0) {
        waituntil = __itimer.it.it_value;
      } else {
        if (timeval_iszero(__itimer.it.it_interval)) {
          __itimer.it.it_value = timeval_zero;
          waituntil = timeval_max;
        } else {
          do {
            __itimer.it.it_value =
                timeval_add(__itimer.it.it_value, __itimer.it.it_interval);
          } while (timeval_cmp(now, __itimer.it.it_value) > 0);
          waituntil = __itimer.it.it_value;
        }
        dosignal = true;
      }
    }
    __itimer_unlock();
    if (dosignal)
      __sig_generate(SIGALRM, SI_TIMER);
    __itimer_lock();
    struct timespec deadline = timeval_totimespec(waituntil);
    _pthread_cond_timedwait(&__itimer.cond, &__itimer.lock, &deadline);
    __itimer_unlock();
  }
  return 0;
}

textwindows static void __itimer_setup(void) {
  __itimer.thread = CreateThread(0, STACK_SIZE, __itimer_worker, 0,
                                 kNtStackSizeParamIsAReservation, 0);
}

textwindows int sys_setitimer_nt(int which, const struct itimerval *neu,
                                 struct itimerval *old) {
  struct itimerval config;
  cosmo_once(&__itimer.once, __itimer_setup);
  if (which != ITIMER_REAL || (neu && (!timeval_isvalid(neu->it_value) ||
                                       !timeval_isvalid(neu->it_interval))))
    return einval();
  if (neu)
    // POSIX defines setitimer() with the restrict keyword but let's
    // accommodate the usage setitimer(ITIMER_REAL, &it, &it) anyway
    config = *neu;
  BLOCK_SIGNALS;
  __itimer_lock();
  if (old) {
    old->it_interval = __itimer.it.it_interval;
    old->it_value = timeval_subz(__itimer.it.it_value, timeval_real());
  }
  if (neu) {
    if (!timeval_iszero(config.it_value))
      config.it_value = timeval_add(config.it_value, timeval_real());
    __itimer.it = config;
    _pthread_cond_signal(&__itimer.cond);
  }
  __itimer_unlock();
  ALLOW_SIGNALS;
  return 0;
}

#endif /* __x86_64__ */
