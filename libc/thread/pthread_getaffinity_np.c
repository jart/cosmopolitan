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
#include "libc/calls/struct/cpuset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"

/**
 * Gets CPU affinity for thread.
 *
 * @param size is bytes in bitset, which should be `sizeof(cpu_set_t)`
 * @return 0 on success, or errno on error
 * @raise EINVAL if `size` or `bitset` is invalid
 * @raise ENOSYS if not Linux, FreeBSD, or NetBSD
 * @raise ESRCH if thread isn't alive
 */
errno_t pthread_getaffinity_np(pthread_t thread, size_t size,
                               cpu_set_t *bitset) {
  int rc, tid;
  tid = _pthread_tid((struct PosixThread *)thread);

  if (size != sizeof(cpu_set_t)) {
    rc = einval();
  } else if (IsWindows() || IsMetal() || IsOpenbsd()) {
    rc = enosys();
  } else if (IsFreebsd()) {
    if (!sys_sched_getaffinity_freebsd(CPU_LEVEL_WHICH, CPU_WHICH_TID, tid, 32,
                                       bitset)) {
      rc = 32;
    } else {
      rc = -1;
    }
  } else if (IsNetbsd()) {
    if (!sys_sched_getaffinity_netbsd(tid, 0, 32, bitset)) {
      rc = 32;
    } else {
      rc = -1;
    }
  } else {
    rc = sys_sched_getaffinity(tid, size, bitset);
  }
  if (rc > 0) {
    if (rc < size) {
      bzero((char *)bitset + rc, size - rc);
    }
    rc = 0;
  }

  STRACE("pthread_getaffinity_np(%d, %'zu, %p) → %s", tid, size, bitset,
         DescribeErrno(rc));
  return rc;
}
