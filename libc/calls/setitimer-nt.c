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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/errfuns.h"

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

static struct ItimerNt {
  int64_t ith;
  uint32_t tid;
  struct itimerval itv;
} g_itimernt;

static uint32_t ItimerWorker(void *arg) {
  do {
    if (!WaitForSingleObject(g_itimernt.ith, -1)) {
      __winalarm(NULL, 0, 0);
    }
  } while (g_itimernt.ith && g_itimernt.tid == GetCurrentThreadId());
  return 0;
}

textwindows int setitimer$nt(int which, const struct itimerval *newvalue,
                             struct itimerval *out_opt_oldvalue) {
  int32_t period;
  int64_t ith, duetime;
  if (which != ITIMER_REAL) return einval();
  if (newvalue) {
    if (newvalue->it_value.tv_sec && newvalue->it_value.tv_usec) {
      if (!(ith = CreateWaitableTimer(NULL, false, NULL))) {
        return __winerr();
      }
      duetime = -(newvalue->it_value.tv_sec * HECTONANOSECONDS +
                  newvalue->it_value.tv_usec * 10);
      period = newvalue->it_value.tv_sec * 1000 +
               div1000int64(newvalue->it_value.tv_usec);
      if (!period && newvalue->it_value.tv_usec) period = 1;
      if (!SetWaitableTimer(ith, &duetime, period, NULL, NULL, false)) {
        errno = GetLastError();
        CloseHandle(ith);
        return -1;
      }
    } else {
      ith = 0;
    }
    if (g_itimernt.ith) {
      CloseHandle(g_itimernt.ith);
      g_itimernt.ith = 0;
    }
  } else {
    ith = 0;
  }
  if (out_opt_oldvalue) {
    memcpy(out_opt_oldvalue, &g_itimernt.itv, sizeof(struct itimerval));
  }
  if (ith) {
    g_itimernt.ith = ith;
    memcpy(&g_itimernt.itv, newvalue, sizeof(struct itimerval));
    CloseHandle(
        CreateThread(NULL, STACKSIZE, ItimerWorker, NULL, 0, &g_itimernt.tid));
  }
  return 0;
}
