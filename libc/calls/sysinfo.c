/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sysinfo.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define CTL_KERN      1
#define CTL_VM        2
#define CTL_HW        6
#define VM_LOADAVG    2
#define KERN_BOOTTIME 21
#define HW_PHYSMEM    (IsXnu() ? 24 : 5)

struct loadavg {
  uint32_t ldavg[3];
  int64_t fscale;
};

static int64_t GetUptime(void) {
  if (IsNetbsd()) return 0;  // TODO(jart): Why?
  struct timeval x;
  size_t n = sizeof(x);
  int mib[] = {CTL_KERN, KERN_BOOTTIME};
  if (sys_sysctl(mib, ARRAYLEN(mib), &x, &n, 0, 0) == -1) return 0;
  return timespec_real().tv_sec - x.tv_sec;
}

static int64_t GetPhysmem(void) {
  uint64_t x = 0;
  size_t n = sizeof(x);
  int mib[] = {CTL_HW, HW_PHYSMEM};
  if (sys_sysctl(mib, ARRAYLEN(mib), &x, &n, 0, 0) == -1) return 0;
  return x;
}

static void GetLoads(uint64_t loads[3]) {
  size_t size;
  struct loadavg loadinfo;
  int mib[2] = {CTL_VM, VM_LOADAVG};
  size = sizeof(loadinfo);
  if (sys_sysctl(mib, 2, &loadinfo, &size, 0, 0) != -1) {
    for (int i = 0; i < 3; i++) {
      loads[i] = (double)loadinfo.ldavg[i] / loadinfo.fscale * 65536;
    }
  }
}

static int sys_sysinfo_bsd(struct sysinfo *info) {
  GetLoads(info->loads);
  info->uptime = GetUptime();
  info->totalram = GetPhysmem();
  info->bufferram = GetPhysmem();
  info->mem_unit = 1;
  return 0;
}

/**
 * Returns amount of system ram, cores, etc.
 *
 * Only the `totalram` field is supported on all platforms right now.
 * Support is best on Linux. Fields will be set to zero when they're not
 * known.
 *
 * @return 0 on success or -1 w/ errno
 * @error EFAULT
 */
int sysinfo(struct sysinfo *info) {
  int rc;
  struct sysinfo x = {0};
  if (IsAsan() && info && !__asan_is_valid(info, sizeof(*info))) {
    rc = efault();
  } else if (!IsWindows()) {
    if (IsLinux()) {
      rc = sys_sysinfo(&x);
    } else {
      rc = sys_sysinfo_bsd(&x);
    }
  } else {
    rc = sys_sysinfo_nt(&x);
  }
  if (rc != -1) {
    memcpy(info, &x, sizeof(x));
  }
  STRACE("sysinfo(%p) → %d% m", info, rc);
  return rc;
}
