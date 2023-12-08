/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/sysv/consts/timer.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

static textwindows int sys_clock_nanosleep_nt_impl(int clock,
                                                   struct timespec abs,
                                                   sigset_t waitmask) {
  uint32_t msdelay;
  struct timespec now;
  for (;;) {
    if (sys_clock_gettime_nt(clock, &now)) return -1;
    if (timespec_cmp(now, abs) >= 0) return 0;
    msdelay = timespec_tomillis(timespec_sub(abs, now));
    if (_park_norestart(msdelay, waitmask)) return -1;
  }
}

textwindows int sys_clock_nanosleep_nt(int clock, int flags,
                                       const struct timespec *req,
                                       struct timespec *rem) {
  int rc;
  struct timespec abs, now;
  sigset_t m = __sig_block();
  if (flags & TIMER_ABSTIME) {
    abs = *req;
  } else {
    if ((rc = sys_clock_gettime_nt(clock, &now))) goto BailOut;
    abs = timespec_add(now, *req);
  }
  rc = sys_clock_nanosleep_nt_impl(clock, abs, m);
  if (rc == -1 && rem && errno == EINTR) {
    sys_clock_gettime_nt(clock, &now);
    *rem = timespec_subz(abs, now);
  }
BailOut:
  __sig_unblock(m);
  return rc;
}

#endif /* __x86_64__ */
