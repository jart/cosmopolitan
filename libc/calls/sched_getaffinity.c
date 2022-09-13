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
#include "libc/intrin/strace.internal.h"
#include "libc/calls/struct/cpuset.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static textwindows int sys_sched_getaffinity_nt(int tid, size_t size,
                                                cpu_set_t *bitset) {
  uint64_t ProcessAffinityMask, SystemAffinityMask;
  if (GetProcessAffinityMask(GetCurrentProcess(), &ProcessAffinityMask,
                             &SystemAffinityMask)) {
    bzero(bitset, size);
    bitset->__bits[0] = ProcessAffinityMask;
    return 0;
  } else {
    return __winerr();
  }
}

/**
 * Gets CPU affinity for thread.
 *
 * While Windows allows us to change the thread affinity mask, it's only
 * possible to read the process affinity mask. Therefore this function
 * won't reflect the changes made by sched_setaffinity() on Windows.
 *
 * @param pid is the process or thread id (or 0 for caller)
 * @param size is byte length of bitset, which should 128
 * @param bitset receives bitset and should be uint64_t[16] in order to
 *     work on older versions of Linux
 * @return 0 on success, or -1 w/ errno
 * @raise ENOSYS on non-Linux
 */
int sched_getaffinity(int tid, size_t size, cpu_set_t *bitset) {
  int rc;
  if (size != 128) {
    rc = einval();
  } else if (IsWindows()) {
    rc = sys_sched_getaffinity_nt(tid, size, bitset);
  } else {
    rc = sys_sched_getaffinity(tid, size, bitset);
  }
  if (rc > 0) {
    if (rc < size) {
      bzero((char *)bitset + rc, size - rc);
    }
    rc = 0;
  }
  STRACE("sched_getaffinity(%d, %'zu, %p) → %d% m", tid, size, bitset);
  return rc;
}
