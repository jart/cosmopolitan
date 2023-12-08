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
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/itimer.internal.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/mu.h"
#ifdef __x86_64__

struct IntervalTimer __itimer;

static textwindows dontinstrument uint32_t __itimer_worker(void *arg) {
  struct CosmoTib tls;
  __bootstrap_tls(&tls, __builtin_frame_address(0));
  for (;;) {
    bool dosignal = false;
    struct timeval now, waituntil;
    nsync_mu_lock(&__itimer.lock);
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
    nsync_mu_unlock(&__itimer.lock);
    if (dosignal) {
      __sig_generate(SIGALRM, SI_TIMER);
    }
    nsync_mu_lock(&__itimer.lock);
    nsync_cv_wait_with_deadline(&__itimer.cond, &__itimer.lock,
                                timeval_totimespec(waituntil), 0);
    nsync_mu_unlock(&__itimer.lock);
  }
  return 0;
}

static textwindows void __itimer_setup(void) {
  __itimer.thread = CreateThread(0, 65536, __itimer_worker, 0,
                                 kNtStackSizeParamIsAReservation, 0);
}

textwindows void __itimer_wipe(void) {
  // this function is called by fork(), because
  // timers aren't inherited by forked subprocesses
  bzero(&__itimer, sizeof(__itimer));
}

textwindows int sys_setitimer_nt(int which, const struct itimerval *neu,
                                 struct itimerval *old) {
  struct itimerval config;
  cosmo_once(&__itimer.once, __itimer_setup);
  if (which != ITIMER_REAL || (neu && (!timeval_isvalid(neu->it_value) ||
                                       !timeval_isvalid(neu->it_interval)))) {
    return einval();
  }
  if (neu) {
    // POSIX defines setitimer() with the restrict keyword but let's
    // accommodate the usage setitimer(ITIMER_REAL, &it, &it) anyway
    config = *neu;
  }
  BLOCK_SIGNALS;
  nsync_mu_lock(&__itimer.lock);
  if (old) {
    old->it_interval = __itimer.it.it_interval;
    old->it_value = timeval_subz(__itimer.it.it_value, timeval_real());
  }
  if (neu) {
    if (!timeval_iszero(config.it_value)) {
      config.it_value = timeval_add(config.it_value, timeval_real());
    }
    __itimer.it = config;
    nsync_cv_signal(&__itimer.cond);
  }
  nsync_mu_unlock(&__itimer.lock);
  ALLOW_SIGNALS;
  return 0;
}

#endif /* __x86_64__ */
