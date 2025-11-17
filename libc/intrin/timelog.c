/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/cosmotime.h"
#include "libc/dce.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/synchronization.h"

#define TIMELOG_ENABLED 1

#if TIMELOG_ENABLED
static bool once;
static struct timespec started;
static struct timespec adjust;
#endif

static struct timespec get(void) {
  if (IsWindows()) {
    struct NtFileTime ft;
    GetSystemTimePreciseAsFileTime(&ft);
    return FileTimeToTimeSpec(ft);
  } else {
    struct timespec ts;
    clock_gettime(0, &ts);
    return ts;
  }
}

void timelog1(void) {
#if TIMELOG_ENABLED
  started = get();
  adjust = (struct timespec){0};
  once = true;
#endif
}

void timelog(const char *s) {
#if TIMELOG_ENABLED
  if (!once)
    timelog1();
  struct timespec now = get();
  kprintf("%!-30s %8ld us\n", s,
          timespec_tomicros(timespec_sub(timespec_sub(now, started), adjust)));
  adjust = timespec_add(adjust, timespec_sub(get(), now));
#endif
}
