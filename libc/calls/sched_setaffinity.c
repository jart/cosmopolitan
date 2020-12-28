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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/enum/threadaccess.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"

static textwindows noinline int sched_setaffinity$nt(int pid,
                                                     uint64_t bitsetsize,
                                                     const void *bitset) {
  int rc;
  uintptr_t mask;
  int64_t handle;
  typeof(SetThreadAffinityMask) *SetAffinityMask = SetThreadAffinityMask;
  mask = 0;
  memcpy(&mask, bitset, min(bitsetsize, sizeof(uintptr_t)));
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
 * Asks kernel to only schedule process on particular CPUs.
 *
 * @param pid is the process or thread id (or 0 for caller)
 * @param bitsetsize is byte length of bitset
 * @return 0 on success, or -1 w/ errno
 */
int sched_setaffinity(int pid, uint64_t bitsetsize, const void *bitset) {
  if (!IsWindows()) {
    return sched_setaffinity$sysv(pid, bitsetsize, bitset);
  } else {
    return sched_setaffinity$nt(pid, bitsetsize, bitset);
  }
}
