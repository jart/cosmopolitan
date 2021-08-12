/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/popcnt.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/dll.h"
#include "libc/nt/struct/systeminfo.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"

#define CTL_HW                6
#define HW_NCPU               3
#define HW_NCPUONLINE_OPENBSD 25
#define HW_NCPUONLINE_NETBSD  16
#define ALL_PROCESSOR_GROUPS  0xffff

/**
 * Returns number of CPUs in system.
 *
 * On Intel systems with HyperThreading this will return the number of
 * cores multiplied by two.
 *
 * @return cpu count or 0 if it couldn't be determined
 */
unsigned GetCpuCount(void) {
  size_t len;
  int i, c, n, cmd[2];
  uint64_t cpuset[16];
  uint32_t (*f)(uint16_t);
  struct NtSystemInfo sysinfo;
  if (IsWindows()) {
    if ((f = GetProcAddress(GetModuleHandle("KERNEL32"),
                            "GetMaximumProcessorCount"))) {
      return f(ALL_PROCESSOR_GROUPS);
    } else {
      GetSystemInfo(&sysinfo);
      return sysinfo.dwNumberOfProcessors;
    }
  } else if (IsBsd()) {
    len = sizeof(c);
    cmd[0] = CTL_HW;
    if (IsOpenbsd()) {
      cmd[1] = HW_NCPUONLINE_OPENBSD;
    } else if (IsNetbsd()) {
      cmd[1] = HW_NCPUONLINE_NETBSD;
    } else {
      cmd[1] = HW_NCPU;
    }
    if (!sysctl(cmd, 2, &c, &len, 0, 0)) {
      return c;
    } else {
      return 0;
    }
  } else {
    if ((n = sched_getaffinity(0, sizeof(cpuset), cpuset)) > 0) {
      assert(!(n & 7));
      for (n >>= 3, c = i = 0; i < ARRAYLEN(cpuset); ++i) {
        c += popcnt(cpuset[i]);
      }
      return c;
    } else {
      return 0;
    }
  }
}
