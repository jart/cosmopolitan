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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"
#ifndef __x86_64__

// todo(jart): dismal llvm
register long x0 asm("x0");
register long x1 asm("x1");
register long x2 asm("x2");
register long x3 asm("x3");
register long x4 asm("x4");
register long x5 asm("x5");
register long sysv_ordinal asm("x8");
register long freebsd_ordinal asm("x9");
register long xnu_ordinal asm("x16");
register long cosmo_tls_register asm("x28");

void report_cancelation_point(void);

dontinline long systemfive_cancel(void) {
  return _weaken(_pthread_cancel_ack)();
}

// special region of executable memory where cancelation is safe
dontinline long systemfive_cancellable(void) {

  // check (1) this is a cancelation point
  // plus (2) cancelations aren't disabled
  struct PosixThread *pth = 0;
  if (cosmo_tls_register &&            //
      _weaken(_pthread_cancel_ack) &&  //
      (pth = _pthread_self())) {
    // check if cancelation is already pending
    if (!(pth->pt_flags & PT_NOCANCEL) &&
        atomic_load_explicit(&pth->pt_canceled, memory_order_acquire)) {
      return systemfive_cancel();
    }
#if IsModeDbg()
    if (!(pth->pt_flags & PT_INCANCEL)) {
      if (_weaken(report_cancelation_point)) {
        _weaken(report_cancelation_point)();
      }
      __builtin_trap();
    }
#endif
  }

  // invoke cancellable system call
  // this works for both linux and bsd
  asm volatile("mov\tx9,0\n\t"      // clear carry flag (for linux)
               "adds\tx9,x9,0\n\t"  // clear carry flag
               "svc\t0\n"
               "systemfive_cancellable_end:\n\t"
               ".globl\tsystemfive_cancellable_end\n\t"
               "bcs\t1f\n\t"
               "b\t2f\n1:\t"
               "neg\tx0,x0\n2:"
               : /* global output */
               : /* global inputs */
               : "x9", "memory");

  // if it succeeded then we're done
  if (x0 < -4095ul) {
    return x0;
  }

  // check if i/o call was interrupted by sigthr
  if (pth && x0 == -EINTR && !(pth->pt_flags & PT_NOCANCEL) &&
      atomic_load_explicit(&pth->pt_canceled, memory_order_acquire)) {
    return systemfive_cancel();
  }

  // otherwise go down error path
  return _sysret(x0);
}

/**
 * System Five System Call Support.
 *
 * This supports POSIX thread cancelation only when the caller flips a
 * bit in TLS storage that indicates we're inside a cancelation point.
 *
 * @param x0 is first argument
 * @param x1 is second argument
 * @param x2 is third argument
 * @param x3 is fourth argument
 * @param x4 is fifth argument
 * @param x5 is sixth argument
 * @param sysv_ordinal is linux ordinal
 * @param xnu_ordinal is xnu ordinal
 * @return x0
 */
long systemfive(void) {

  // handle special cases
  if (IsLinux() || IsFreebsd()) {
    if (IsFreebsd()) {
      sysv_ordinal = freebsd_ordinal;
    }
    if (sysv_ordinal == 0xfff) {
      return _sysret(-ENOSYS);
    }
    if (sysv_ordinal & 0x800) {
      sysv_ordinal &= ~0x800;
      return systemfive_cancellable();
    }
  }
  if (IsXnu()) {
    if (xnu_ordinal == 0xfff) {
      return _sysret(-ENOSYS);
    }
    if (xnu_ordinal & 0x800) {
      xnu_ordinal &= ~0x800;
      return systemfive_cancellable();
    }
  }

  // invoke non-blocking system call
  // this works for both linux and bsd
  asm volatile("svc\t0\n\t"
               "bcs\t1f\n\t"
               "b\t2f\n1:\t"
               "neg\tx0,x0\n2:"
               : /* global output */
               : /* global inputs */
               : "x9", "memory");

  // check result
  if (x0 < -4095ul) {
    return x0;
  } else {
    return _sysret(x0);
  }
}

#endif /* __x86_64__ */
