/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/nt/accounting.h"
#include "libc/runtime/runtime.h"

#define CTL_HW                6
#define HW_NCPU               3
#define HW_NCPUONLINE_OPENBSD 25
#define HW_NCPUONLINE_NETBSD  16
#define ALL_PROCESSOR_GROUPS  0xffff

static int __get_cpu_count_linux(void) {
  cpu_set_t cs = {0};
  if (sys_sched_getaffinity(0, sizeof(cs), &cs) != -1) {
    return CPU_COUNT(&cs);
  } else {
    return -1;
  }
}

static int __get_cpu_count_bsd(void) {
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
    return -1;
  }
}

static textwindows int __get_cpu_count_nt(void) {
  uint32_t res;
  if ((res = GetMaximumProcessorCount(ALL_PROCESSOR_GROUPS))) {
    return res;
  } else {
    return __winerr();
  }
}

/**
 * Returns number of CPUs in system.
 *
 * This is the same as the standard interface:
 *
 *     sysconf(_SC_NPROCESSORS_ONLN);
 *
 * On Intel systems with HyperThreading this will return the number of
 * cores multiplied by two. On Linux, if you change cpu affinity it'll
 * change the cpu count, which also gets inherited by child processes.
 *
 * @return cpu count, or -1 w/ errno
 */
int __get_cpu_count(void) {
  if (!IsWindows()) {
    if (!IsBsd()) {
      return __get_cpu_count_linux();
    } else {
      return __get_cpu_count_bsd();
    }
  } else {
    return __get_cpu_count_nt();
  }
}
