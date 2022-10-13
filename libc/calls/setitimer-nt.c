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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/check.h"
#include "libc/math.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

/**
 * @fileoverview Heartbreaking polyfill for SIGALRM on NT.
 *
 * Threads are used to trigger the SIGALRM handler, which should
 * hopefully be an unfancy function like this:
 *
 *   void OnAlarm(int sig, struct siginfo *si, struct ucontext *uc) {
 *     g_alarmed = true;
 *   }
 *
 * This is needed because WIN32 provides no obvious solutions for
 * interrupting i/o operations on the standard input handle.
 */

static bool __hastimer;
static bool __singleshot;
static long double __lastalrm;
static long double __interval;

textwindows void _check_sigalrm(void) {
  // TODO(jart): use a different timing source
  // TODO(jart): synchronize across intervals?
  long double now, elapsed;
  if (!__hastimer) return;
  now = nowl();
  elapsed = now - __lastalrm;
  if (elapsed > __interval) {
    __sig_add(0, SIGALRM, SI_TIMER);
    if (__singleshot) {
      __hastimer = false;
    } else {
      __lastalrm = now;
    }
  }
}

textwindows int sys_setitimer_nt(int which, const struct itimerval *newvalue,
                                 struct itimerval *out_opt_oldvalue) {
  long double elapsed, untilnext;

  if (which != ITIMER_REAL ||
      (newvalue && (!(0 <= newvalue->it_value.tv_usec &&
                      newvalue->it_value.tv_usec < 1000000) ||
                    !(0 <= newvalue->it_interval.tv_usec &&
                      newvalue->it_interval.tv_usec < 1000000)))) {
    return einval();
  }

  if (out_opt_oldvalue) {
    if (__hastimer) {
      elapsed = nowl() - __lastalrm;
      if (elapsed > __interval) {
        untilnext = 0;
      } else {
        untilnext = __interval - elapsed;
      }
      out_opt_oldvalue->it_interval.tv_sec = __interval;
      out_opt_oldvalue->it_interval.tv_usec = 1 / 1e6 * fmodl(__interval, 1);
      out_opt_oldvalue->it_value.tv_sec = untilnext;
      out_opt_oldvalue->it_value.tv_usec = 1 / 1e6 * fmodl(untilnext, 1);
    } else {
      out_opt_oldvalue->it_interval.tv_sec = 0;
      out_opt_oldvalue->it_interval.tv_usec = 0;
      out_opt_oldvalue->it_value.tv_sec = 0;
      out_opt_oldvalue->it_value.tv_usec = 0;
    }
  }

  if (newvalue) {
    if (newvalue->it_interval.tv_sec || newvalue->it_interval.tv_usec ||
        newvalue->it_value.tv_sec || newvalue->it_value.tv_usec) {
      __hastimer = true;
      if (newvalue->it_interval.tv_sec || newvalue->it_interval.tv_usec) {
        __singleshot = false;
        __interval = newvalue->it_interval.tv_sec +
                     1 / 1e6 * newvalue->it_interval.tv_usec;
      } else {
        __singleshot = true;
        __interval =
            newvalue->it_value.tv_sec + 1 / 1e6 * newvalue->it_value.tv_usec;
      }
      __lastalrm = nowl();
    } else {
      __hastimer = false;
    }
  }

  return 0;
}
