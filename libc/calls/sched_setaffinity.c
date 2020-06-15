/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"

static textwindows noinline int sched_setaffinity$nt(int pid,
                                                     uint64_t bitsetsize,
                                                     const void *bitset) {
  typeof(SetThreadAffinityMask) *SetAffinityMask = SetThreadAffinityMask;
  uintptr_t mask = 0;
  memcpy(&mask, bitset, min(bitsetsize, sizeof(uintptr_t)));
  int64_t handle = 0;
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
  int rc = SetAffinityMask(handle ? handle : pid, mask) ? 0 : winerr();
  if (handle) CloseHandle(handle);
  return rc;
}

/**
 * Asks kernel to only schedule process on particular CPUs.
 *
 * @param pid is the process or thread id
 * @param bitsetsize is byte length of bitset
 * @param bitset can be manipulated using bt(), bts(), etc.
 * @return 0 on success, or -1 w/ errno
 */
int sched_setaffinity(int pid, uint64_t bitsetsize, const void *bitset) {
  if (!IsWindows()) {
    return sched_setaffinity$sysv(pid, bitsetsize, bitset);
  } else {
    return sched_setaffinity$nt(pid, bitsetsize, bitset);
  }
}
