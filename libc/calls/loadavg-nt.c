/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/loadavg.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/pdh.h"
#include "libc/nt/enum/securityimpersonationlevel.h"
#include "libc/nt/enum/wt.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/pdh.h"
#include "libc/nt/privilege.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/luid.h"
#include "libc/nt/struct/pdhfmtcountervalue.h"
#include "libc/nt/struct/tokenprivileges.h"
#include "libc/nt/synchronization.h"
#include "libc/str/str.h"

/**
 * @fileoverview sysinfo() on the new technology
 * @kudos Giampaolo Rodola for teaching how to do load average
 */

#define LOAD_SAMPLING_INTERVAL 1  // in seconds

// https://github.com/torvalds/linux/blob/345671ea0f9258f410eb057b9ced9cefbbe5dc78/include/linux/sched/loadavg.h#L20-L23
#define LOAD1F  .9200444146293232478931553241
#define LOAD5F  .9834714538216174894737477501
#define LOAD15F .9944598480048967508795473394

double __ntloadavg[3];

static void LoadavgNtPoll(int64_t hCounter, bool32 timedOut) {
  struct NtPdhFmtCountervalue c;
  if (!PdhGetFormattedCounterValue(hCounter, kNtPdhFmtDouble, 0, &c)) {
    __ntloadavg[0] = __ntloadavg[0] * LOAD1F + c.doubleValue * (1 - LOAD1F);
    __ntloadavg[1] = __ntloadavg[1] * LOAD5F + c.doubleValue * (1 - LOAD5F);
    __ntloadavg[2] = __ntloadavg[2] * LOAD15F + c.doubleValue * (1 - LOAD15F);
  } else {
    STRACE("PdhGetFormattedCounterValue(%ld) failed", hCounter);
  }
}

static textstartup void LoadavgNtInit(void) {
  int64_t hQuery, hCounter, hEvent, hWaiter;
  if (!IsWindows()) return;
  STRACE("LoadavgNtInit()");
  if (PdhOpenQuery(0, 0, &hQuery)) {
    STRACE("PdhOpenQuery failed");
    return;
  }
  if (PdhAddEnglishCounter(hQuery, u"\\System\\Processor Queue Length", 0,
                           &hCounter)) {
    STRACE("PdhAddEnglishCounter() failed");
    return;
  }
  if (!(hEvent = CreateEvent(0, 0, 0, u"LoadUpdateEvent"))) {
    STRACE("CreateEvent() failed");
    return;
  }
  if (PdhCollectQueryDataEx(hQuery, LOAD_SAMPLING_INTERVAL, hEvent)) {
    STRACE("PdhCollectQueryDataEx() failed");
    return;
  }
  if (!RegisterWaitForSingleObject(
          &hWaiter, hEvent, (void *)NT2SYSV(LoadavgNtPoll),
          (void *)(intptr_t)hCounter, -1, kNtWtExecutedefault)) {
    STRACE("RegisterWaitForSingleObject() failed");
    return;
  }
  LoadavgNtPoll(hCounter, 0);
}

const void *const LoadavgNtCtor[] initarray = {
    LoadavgNtInit,
};
