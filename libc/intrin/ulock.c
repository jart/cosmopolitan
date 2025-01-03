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
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/ulock.h"

// XNU futexes
// https://opensource.apple.com/source/xnu/xnu-7195.50.7.100.1/bsd/sys/ulock.h.auto.html
// https://opensource.apple.com/source/xnu/xnu-3789.41.3/bsd/kern/sys_ulock.c.auto.html

int sys_ulock_wait(uint32_t operation, void *addr, uint64_t value,
                   uint32_t timeout_micros) asm("sys_futex_cp");

// returns number of other waiters, or -1 w/ errno
//
// - EINTR means a signal handler was called. This is how we support
//   things like POSIX thread cancelation.
//
// - EFAULT if XNU couldn't read `addr`. This is normally considered a
//   programming error, but with ulock it can actually be a transient
//   error due to low memory conditions. Apple recommends retrying.
//
// - ENOMEM means XNU wasn't able to allocate memory for kernel internal
//   data structures. Apple doesn't provide any advice on what to do. We
//   simply turn this into EAGAIN.
//
// - EAGAIN if XNU told us EFAULT but cosmo believes the address exists.
//   This value is also used as a substitute for ENOMEM.
//
// - EINVAL could mean operation is invalid, addr is null or misaligned;
//   it could also mean another thread calling ulock on this address was
//   configured (via operation) in an inconsistent way.
//
// see also os_sync_wait_on_address.h from xcode sdk
int ulock_wait(uint32_t operation, void *addr, uint64_t value,
               uint32_t timeout_micros) {
  int rc;
  operation |= ULF_WAIT_CANCEL_POINT;
  LOCKTRACE("ulock_wait(%#x, %p, %lx, %u) → ...", operation, addr, value,
            timeout_micros);
  rc = sys_ulock_wait(operation, addr, value, timeout_micros);
  if (rc == -1) {
    if (errno == ENOMEM)
      errno = EAGAIN;
    if (errno == EFAULT)
      if (!kisdangerous(addr))
        errno = EAGAIN;
  }
  LOCKTRACE("ulock_wait(%#x, %p, %lx, %u) → %d% m", operation, addr, value,
            timeout_micros, rc);
  return rc;
}

// returns -errno
//
// - ENOENT means there wasn't anyone to wake
//
// - EINVAL could mean operation is invalid, addr is null or misaligned;
//   it could also mean another thread calling ulock on this address was
//   configured (via operation) in an inconsistent way.
//
// should be dontinstrument because SiliconThreadMain() calls this from
// a stack managed by apple libc.
//
dontinstrument int ulock_wake(uint32_t operation, void *addr,
                              uint64_t wake_value) {
  int rc;
  rc = __syscall3i(operation, (long)addr, wake_value, 0x2000000 | 516);
  LOCKTRACE("ulock_wake(%#x, %p, %lx) → %s", operation, addr, wake_value,
            DescribeErrno(rc));
  return rc;
}
