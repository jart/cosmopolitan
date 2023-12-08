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
#include "libc/calls/sched-sysv.internal.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"

errno_t _pthread_reschedule(struct PosixThread *pt) {
  int policy = pt->pt_attr.__schedpolicy;
  int e, rc, tid = _pthread_tid(pt);
  struct sched_param param = {pt->pt_attr.__schedparam};
  e = errno;
  if (IsNetbsd()) {
    rc = sys_sched_setparam_netbsd(0, tid, policy, &param);
  } else if (IsLinux()) {
    rc = sys_sched_setscheduler(tid, policy, &param);
  } else if (IsFreebsd()) {
    rc = _pthread_setschedparam_freebsd(tid, policy, &param);
  } else {
    rc = enosys();
  }
  if (rc == -1) {
    rc = errno;
    errno = e;
  }
  return rc;
}
