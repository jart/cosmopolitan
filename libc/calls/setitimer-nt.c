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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

static struct itimerval g_setitimer;

textwindows void _check_sigalrm(void) {
  struct timeval now;
  if (timeval_iszero(g_setitimer.it_value)) return;
  now = timeval_real();
  if (timeval_cmp(now, g_setitimer.it_value) < 0) return;
  if (timeval_iszero(g_setitimer.it_interval)) {
    g_setitimer.it_value = timeval_zero;
  } else {
    do {
      g_setitimer.it_value =
          timeval_add(g_setitimer.it_value, g_setitimer.it_interval);
    } while (timeval_cmp(now, g_setitimer.it_value) > 0);
  }
  __sig_add(0, SIGALRM, SI_TIMER);
}

textwindows void sys_setitimer_nt_reset(void) {
  // this function is called by fork(), because
  // timers aren't inherited by forked subprocesses
  bzero(&g_setitimer, sizeof(g_setitimer));
}

textwindows int sys_setitimer_nt(int which, const struct itimerval *neu,
                                 struct itimerval *old) {
  struct itimerval config;
  if (which != ITIMER_REAL || (neu && (!timeval_isvalid(neu->it_value) ||
                                       !timeval_isvalid(neu->it_interval)))) {
    return einval();
  }
  if (neu) {
    // POSIX defines setitimer() with the restrict keyword but let's
    // accommodate the usage setitimer(ITIMER_REAL, &it, &it) anyway
    config = *neu;
  }
  if (old) {
    old->it_interval = g_setitimer.it_interval;
    old->it_value = timeval_subz(g_setitimer.it_value, timeval_real());
  }
  if (neu) {
    if (!timeval_iszero(config.it_value)) {
      config.it_value = timeval_add(config.it_value, timeval_real());
    }
    g_setitimer = config;
  }
  return 0;
}

#endif /* __x86_64__ */
