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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/stdio/rand.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * Returns handles of windows pids being tracked.
 *
 * We return 64 at most because Windows can't await on a larger number
 * of things at the same time. If we have a lot of subprocesses, then we
 * choose a subgroup to monitor at random.
 *
 * @return number of items returned in pids and handles
 */
textwindows int __sample_pids(int pids[hasatleast 64],
                              int64_t handles[hasatleast 64],
                              bool exploratory) {
  uint32_t i, j, base, count;
  base = _rand64() >> 32;
  for (count = i = 0; i < g_fds.n; ++i) {
    j = (base + i) % g_fds.n;
    if (g_fds.p[j].kind == kFdProcess && (!exploratory || !g_fds.p[j].zombie)) {
      pids[count] = j;
      handles[count] = g_fds.p[j].handle;
      if (++count == 64) {
        break;
      }
    }
  }
  return count;
}
