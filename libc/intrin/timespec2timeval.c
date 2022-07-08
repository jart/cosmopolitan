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
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"

// we don't want instrumentation because:
// - nanosleep() depends on this and ftrace can take microsecs

/**
 * Converts `struct timespec` to `struct timeval`.
 *
 * This divides ts.tv_nsec by 1000 with upward rounding and overflow
 * handling. Your ts.tv_nsec must be on the interval `[0,1000000000)`
 * otherwise `{-1, -1}` is returned.
 *
 * @return converted timeval whose tv_usec will be -1 on error
 */
noinstrument struct timeval _timespec2timeval(struct timespec ts) {
  if (0 <= ts.tv_nsec && ts.tv_nsec < 1000000000) {
    if (0 <= ts.tv_nsec && ts.tv_nsec < 1000000000 - 999) {
      return (struct timeval){ts.tv_sec, (ts.tv_nsec + 999) / 1000};
    } else {
      return (struct timeval){ts.tv_sec + 1, 0};
    }
  } else {
    return (struct timeval){-1, -1};
  }
}
