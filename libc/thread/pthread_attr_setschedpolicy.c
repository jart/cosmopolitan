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
#include "libc/thread/thread.h"

/**
 * Sets thread scheduler policy attribute, e.g.
 *
 *     pthread_t id;
 *     pthread_attr_t attr;
 *     pthread_attr_init(&attr);
 *     struct sched_param pri = {sched_get_priority_min(SCHED_OTHER)};
 *     pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
 *     pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
 *     pthread_attr_setschedparam(&attr, &pri);
 *     pthread_create(&id, &attr, func, 0);
 *     pthread_attr_destroy(&attr);
 *     pthread_join(id, 0);
 *
 * @param policy may be one of:
 *     - `SCHED_OTHER` the default policy
 *     - `SCHED_FIFO` for real-time scheduling (usually needs root)
 *     - `SCHED_RR` for round-robin scheduling (usually needs root)
 *     - `SCHED_IDLE` for lowest effort (Linux and FreeBSD only)
 *     - `SCHED_BATCH` for "batch" style execution of processes if
 *       supported (Linux), otherwise it's treated as `SCHED_OTHER`
 * @see sched_setscheduler()
 */
errno_t pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
  attr->__schedpolicy = policy;
  return 0;
}
