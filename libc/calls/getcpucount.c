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
#include "libc/calls/calls.h"
#include "libc/calls/sched-sysv.internal.h"
#include "libc/calls/struct/cpuset.h"
#include "libc/calls/weirdtypes.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/accounting.h"
#include "libc/nt/dll.h"
#include "libc/nt/struct/systeminfo.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"

#define CTL_HW                6
#define HW_NCPU               3
#define HW_NCPUONLINE_OPENBSD 25
#define HW_NCPUONLINE_NETBSD  16
#define ALL_PROCESSOR_GROUPS  0xffff

static unsigned _getcpucount_linux(void) {
  cpu_set_t s = {0};
  if (sys_sched_getaffinity(0, sizeof(s), &s) != -1) {
    return CPU_COUNT(&s);
  } else {
    return 0;
  }
}

static unsigned _getcpucount_bsd(void) {
  size_t n;
  int c, cmd[2];
  n = sizeof(c);
  cmd[0] = CTL_HW;
  if (IsOpenbsd()) {
    cmd[1] = HW_NCPUONLINE_OPENBSD;
  } else if (IsNetbsd()) {
    cmd[1] = HW_NCPUONLINE_NETBSD;
  } else {
    cmd[1] = HW_NCPU;
  }
  if (!sys_sysctl(cmd, 2, &c, &n, 0, 0)) {
    return c;
  } else {
    return 0;
  }
}

static unsigned _getcpucount_impl(void) {
  if (!IsWindows()) {
    if (!IsBsd()) {
      return _getcpucount_linux();
    } else {
      return _getcpucount_bsd();
    }
  } else {
    return GetMaximumProcessorCount(ALL_PROCESSOR_GROUPS);
  }
}

static int g_cpucount;

// precompute because process affinity on linux may change later
__attribute__((__constructor__)) static void _getcpucount_init(void) {
  g_cpucount = _getcpucount_impl();
}

/**
 * Returns number of CPUs in system.
 *
 * This is the same as the standard interface:
 *
 *     sysconf(_SC_NPROCESSORS_ONLN);
 *
 * Except this function isn't a bloated diamond dependency.
 *
 * On Intel systems with HyperThreading this will return the number of
 * cores multiplied by two.
 *
 * @return cpu count or 0 if it couldn't be determined
 */
unsigned _getcpucount(void) {
  return g_cpucount;
}
