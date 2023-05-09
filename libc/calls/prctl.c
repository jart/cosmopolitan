/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/errfuns.h"

/**
 * Tunes process on Linux.
 *
 * @raise ENOSYS on non-Linux
 */
privileged int prctl(int operation, ...) {
  int rc;
  va_list va;
  intptr_t a, b, c, d;

  va_start(va, operation);
  a = va_arg(va, intptr_t);
  b = va_arg(va, intptr_t);
  c = va_arg(va, intptr_t);
  d = va_arg(va, intptr_t);
  va_end(va);

#ifdef __x86_64__
  if (IsLinux()) {
    asm volatile("mov\t%5,%%r10\n\t"
                 "mov\t%6,%%r8\n\t"
                 "syscall"
                 : "=a"(rc)
                 : "0"(157), "D"(operation), "S"(a), "d"(b), "g"(c), "g"(d)
                 : "rcx", "r8", "r10", "r11", "memory");
    if (rc > -4096u) errno = -rc, rc = -1;
  } else {
    rc = enosys();
  }
#elif defined(__aarch64__)
  register long r0 asm("x0") = (long)operation;
  register long r1 asm("x1") = (long)a;
  register long r2 asm("x2") = (long)b;
  register long r3 asm("x3") = (long)c;
  register long r4 asm("x4") = (long)d;
  register long res_x0 asm("x0");
  asm volatile("mov\tx8,%1\n\t"
               "svc\t0"
               : "=r"(res_x0)
               : "i"(167), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4)
               : "x8", "memory");
  rc = _sysret(res_x0);
#else
#error "arch unsupported"
#endif

#ifdef SYSDEBUG
  if (operation == PR_CAPBSET_READ || operation == PR_CAPBSET_DROP) {
    STRACE("prctl(%s, %s) → %d% m", DescribePrctlOperation(operation),
           DescribeCapability(a), rc);
  } else {
    STRACE("prctl(%s, %p, %p, %p, %p) → %d% m",
           DescribePrctlOperation(operation), a, b, c, d, rc);
  }
#endif

  return rc;
}
