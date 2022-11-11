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
#include "libc/calls/sched-sysv.internal.h"
#include "libc/calls/struct/cpuset.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/threadaccess.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"

static dontinline textwindows int sys_pthread_setaffinity_nt(
    int tid, uint64_t size, const cpu_set_t *bitset) {
  int rc;
  int64_t h;
  h = OpenThread(kNtThreadSetInformation | kNtThreadQueryInformation, false,
                 tid);
  if (!h) return __winerr();
  rc = SetThreadAffinityMask(h, bitset->__bits[0]) ? 0 : __winerr();
  CloseHandle(h);
  return rc;
}

/**
 * Asks kernel to only schedule thread on particular CPUs.
 *
 * @param size is bytes in bitset, which should be `sizeof(cpu_set_t)`
 * @return 0 on success, or errno on error
 * @raise EINVAL if `size` or `bitset` is invalid
 * @raise ENOSYS if not Linux, FreeBSD, NetBSD, or Windows
 * @see sched_setaffinity() for processes
 */
errno_t pthread_setaffinity_np(pthread_t thread, size_t size,
                               const cpu_set_t *bitset) {
  int e, rc, tid;
  if (!(rc = pthread_getunique_np(thread, &tid))) {
    e = errno;
    if (size != sizeof(cpu_set_t)) {
      rc = einval();
    } else if (IsWindows()) {
      rc = sys_pthread_setaffinity_nt(tid, size, bitset);
    } else if (IsFreebsd()) {
      rc = sys_sched_setaffinity_freebsd(CPU_LEVEL_WHICH, CPU_WHICH_TID, tid,
                                         32, bitset);
    } else if (IsNetbsd()) {
      rc = sys_sched_setaffinity_netbsd(tid, 0, 32, bitset);
    } else {
      rc = sys_sched_setaffinity(tid, size, bitset);
    }
    if (rc == -1) {
      rc = errno;
      errno = e;
    }
  }
  STRACE("pthread_setaffinity_np(%d, %'zu, %p) → %s", tid, size, bitset,
         DescribeErrno(rc));
  return rc;
}
