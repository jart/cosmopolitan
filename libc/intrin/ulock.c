/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/ulock.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"

// XNU futexes
// https://opensource.apple.com/source/xnu/xnu-7195.50.7.100.1/bsd/sys/ulock.h.auto.html
// https://opensource.apple.com/source/xnu/xnu-3789.41.3/bsd/kern/sys_ulock.c.auto.html

int sys_ulock_wait(uint32_t operation, void *addr, uint64_t value,
                   uint32_t timeout_micros) asm("sys_futex_cp");

// returns number of other waiters, or -1 w/ errno
int ulock_wait(uint32_t operation, void *addr, uint64_t value,
               uint32_t timeout_micros) {
  int rc;
  operation |= ULF_WAIT_CANCEL_POINT;
  LOCKTRACE("ulock_wait(%#x, %p, %lx, %u) → ...", operation, addr, value,
            timeout_micros);
  rc = sys_ulock_wait(operation, addr, value, timeout_micros);
  LOCKTRACE("ulock_wait(%#x, %p, %lx, %u) → %d% m", operation, addr, value,
            timeout_micros, rc);
  return rc;
}

// returns -errno
int ulock_wake(uint32_t operation, void *addr, uint64_t wake_value) {
  int rc;
  rc = __syscall3i(operation, (long)addr, wake_value, 0x2000000 | 516);
  LOCKTRACE("ulock_wake(%#x, %p, %lx) → %s", operation, addr, wake_value,
            DescribeErrno(rc));
  return rc;
}
