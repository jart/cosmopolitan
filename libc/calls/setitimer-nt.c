/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
