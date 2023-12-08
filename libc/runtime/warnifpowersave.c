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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"

// RDTSC on Linux has so much jitter when the CPU is in powersave mode.
// Causing things like microbenchmarks to have a 1000% margin of error.

#define FILE "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define WARN                                                                  \
  "warning: this operation isn't reliable in powersave mode. please run:\n\t" \
  "echo performance | sudo tee "                                              \
  "/sys/devices/system/cpu/cpu*/cpufreq/scaling_governor\n"

void __warn_if_powersave(void) {
  int e, fd;
  char buf[16] = {0};
  if (IsLinux()) {
    e = errno;
    BLOCK_CANCELATION;
    if ((fd = __sys_openat(AT_FDCWD, FILE, O_RDONLY, 0)) != -1) {
      sys_read(fd, buf, 15);
      sys_close(fd);
      if (!startswith(buf, "powersave")) return;
      sys_write(2, WARN, sizeof(WARN) - 1);
    }
    ALLOW_CANCELATION;
    errno = e;
  }
}
