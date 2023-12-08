/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/limits.h"
#include "libc/stdckdint.h"

/**
 * Converts timespec to scalar.
 *
 * This returns the absolute number of nanoseconds in a timespec. If
 * overflow happens, then `INT64_MAX` or `INT64_MIN` is returned. The
 * `errno` variable isn't changed.
 *
 * @return 64-bit integer holding nanoseconds since epoch
 */
int64_t timespec_tonanos(struct timespec x) {
  int64_t ns;
  if (!ckd_mul(&ns, x.tv_sec, 1000000000ul) && !ckd_add(&ns, ns, x.tv_nsec)) {
    return ns;
  } else if (x.tv_sec < 0) {
    return INT64_MIN;
  } else {
    return INT64_MAX;
  }
}
