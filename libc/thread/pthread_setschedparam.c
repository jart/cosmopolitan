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
#include "libc/errno.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread2.h"

/**
 * Changes scheduling of thread, e.g.
 *
 *     struct sched_param p = {sched_get_priority_min(SCHED_OTHER)};
 *     pthread_setschedparam(thread, SCHED_OTHER, &p);
 *
 * @param policy may be one of:
 *     - `SCHED_OTHER` the default policy
 *     - `SCHED_FIFO` for real-time scheduling (usually needs root)
 *     - `SCHED_RR` for round-robin scheduling (usually needs root)
 *     - `SCHED_IDLE` for lowest effort (Linux and FreeBSD only)
 *     - `SCHED_BATCH` for "batch" style execution of processes if
 *       supported (Linux), otherwise it's treated as `SCHED_OTHER`
 * @raise ENOSYS on XNU, Windows, OpenBSD
 * @raise EPERM if not authorized to use scheduler in question (e.g.
 *     trying to use a real-time scheduler as non-root on Linux) or
 *     possibly because pledge() was used and isn't allowing this
 * @see sched_get_priority_min()
 * @see sched_get_priority_max()
 * @see sched_setscheduler()
 */
errno_t pthread_setschedparam(pthread_t thread, int policy,
                              const struct sched_param *param) {
  struct PosixThread *pt = (struct PosixThread *)thread;
  pt->pt_attr.__schedpolicy = policy;
  pt->pt_attr.__schedparam = param->sched_priority;
  return _pthread_reschedule(pt);
}
