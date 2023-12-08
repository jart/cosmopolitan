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
#include "libc/assert.h"
#include "libc/errno.h"
#include "libc/thread/thread.h"

__static_yoink("_pthread_reschedule");

/**
 * Sets thread scheduler inheritance attribute, e.g.
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
 * @param inheritsched may be one of:
 *     - `PTHREAD_INHERIT_SCHED` the default
 *     - `PTHREAD_EXPLICIT_SCHED` to enable rescheduling
 * @return 0 on success, or errno on error
 * @raise EINVAL on bad value
 */
errno_t pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
  switch (inheritsched) {
    case PTHREAD_INHERIT_SCHED:
    case PTHREAD_EXPLICIT_SCHED:
      attr->__inheritsched = inheritsched;
      return 0;
    default:
      return EINVAL;
  }
}
