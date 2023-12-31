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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

int sys_fork(void) {
#ifdef __x86_64__

  axdx_t ad;
  int ax, dx;
  ad = __sys_fork();
  ax = ad.ax;
  dx = ad.dx;
  if (IsXnu() && ax != -1) {
    // eax always returned with childs pid
    // edx is 0 for parent and 1 for child
    ax &= dx - 1;
  }
  return ax;

#elif defined(__aarch64__)

  if (IsLinux()) {
    int flags = 17;  // SIGCHLD
    void *child_stack = 0;
    void *parent_tidptr = 0;
    void *newtls = 0;
    void *child_tidptr = 0;
    register long r0 asm("x0") = (long)flags;
    register long r1 asm("x1") = (long)child_stack;
    register long r2 asm("x2") = (long)parent_tidptr;
    register long r3 asm("x3") = (long)newtls;
    register long r4 asm("x4") = (long)child_tidptr;
    register int res_x0 asm("x0");
    asm volatile("mov\tx8,%1\n\t"
                 "svc\t0"
                 : "=r"(res_x0)
                 : "i"(220), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4)
                 : "x8", "x16", "memory");
    return _sysret(res_x0);
  } else if (__syslib) {
    return _sysret(__syslib->__fork());
  } else {
    int ax;
    axdx_t ad;
    ad = __sys_fork();
    ax = ad.ax;
    return ax;
  }

#else

  return enosys();

#endif
}
