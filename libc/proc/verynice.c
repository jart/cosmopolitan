/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/sysv/consts/ioprio.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/sched.h"

/**
 * Makes current process as low-priority as possible.
 *
 * @return 0 on success, or -1 w/ errno
 * @note error reporting currently not implemented
 */
int verynice(void) {
  int e = errno;
  setpriority(PRIO_PROCESS, 0, NZERO);
  sys_ioprio_set(IOPRIO_WHO_PROCESS, 0,
                 IOPRIO_PRIO_VALUE(IOPRIO_CLASS_IDLE, 0));
  struct sched_param param = {sched_get_priority_min(SCHED_IDLE)};
  sched_setscheduler(0, SCHED_IDLE, &param);
  errno = e;
  return 0;
}
