/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/errno.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/clock.h"

textwindows int sys_clock_gettime_nt(int clock, struct timespec *ts) {
  struct NtFileTime ft;
  if (clock == CLOCK_REALTIME) {
    if (!ts) return 0;
    GetSystemTimeAsFileTime(&ft);
    *ts = FileTimeToTimeSpec(ft);
    return 0;
  } else if (clock == CLOCK_MONOTONIC) {
    if (!ts) return 0;
    return sys_clock_gettime_mono(ts);
  } else if (clock == CLOCK_BOOTTIME) {
    if (ts) *ts = timespec_frommillis(GetTickCount64());
    return 0;
  } else {
    return -EINVAL;
  }
}
