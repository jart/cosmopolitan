/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/sched-sysv.internal.h"
#include "libc/calls/struct/cpuset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/enum/threadaccess.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static textwindows dontinline int sys_sched_setaffinity_nt(int pid,
                                                           uint64_t size,
                                                           const void *bitset) {
  int rc;
  int64_t handle;
  uintptr_t mask;
  typeof(SetThreadAffinityMask) *SetAffinityMask = SetThreadAffinityMask;
  mask = 0;
  memcpy(&mask, bitset, min(size, sizeof(uintptr_t)));
  handle = 0;
  if (!pid) pid = GetCurrentThreadId();
  if (0 < pid && pid <= UINT32_MAX) {
    if (pid == GetCurrentProcessId()) {
      pid = GetCurrentProcess();
      SetAffinityMask = SetProcessAffinityMask;
    } else if (pid == GetCurrentThreadId()) {
      pid = GetCurrentThread();
    } else {
      handle = OpenThread(kNtThreadSetInformation | kNtThreadQueryInformation,
                          false, pid);
      if (!handle) {
        handle = OpenProcess(
            kNtProcessSetInformation | kNtProcessQueryInformation, false, pid);
        SetAffinityMask = SetProcessAffinityMask;
      }
    }
  }
  rc = SetAffinityMask(handle ? handle : pid, mask) ? 0 : __winerr();
  if (handle) CloseHandle(handle);
  return rc;
}

/**
 * Asks kernel to only schedule thread on particular CPUs.
 *
 * @param tid is the process or thread id (or 0 for caller)
 * @param size is byte length of bitset, which should be 128
 * @return 0 on success, or -1 w/ errno
 * @raise ENOSYS if not Linux, NetBSD, or Windows
 */
int sched_setaffinity(int tid, size_t size, const cpu_set_t *bitset) {
  int rc;
  if (size != 128) {
    rc = einval();
  } else if (IsWindows()) {
    rc = sys_sched_setaffinity_nt(tid, size, bitset);
  } else if (IsNetbsd()) {
    rc = sys_sched_setaffinity_netbsd(0, tid, MIN(size, 32), bitset);
  } else {
    rc = sys_sched_setaffinity(tid, size, bitset);
  }
  STRACE("sched_setaffinity(%d, %'zu, %p) → %d% m", tid, size, bitset);
  return rc;
}
