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
#include "libc/dce.h"
#include "libc/intrin/asmflag.h"
#include "libc/nt/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/nr.h"
#include "libc/thread/tls.h"

__msabi extern typeof(ExitThread) *const __imp_ExitThread;

/**
 * Terminates thread with raw system call.
 *
 * The function you want is pthread_exit(). If you call this function
 * whilst using the pthreads then your joiners might not get woken up
 * on non-Linux platforms where we zero __get_tls()->tib_tid manually
 *
 * If this is the main thread, or an orphaned child thread, then this
 * function is equivalent to exiting the process; however, `rc` shall
 * only be reported to the parent process on Linux, FreeBSD & Windows
 * whereas on other platforms, it'll be silently coerced to zero.
 *
 * @param rc only works on Linux and Windows
 * @see cthread_exit()
 * @noreturn
 */
wontreturn void _Exit1(int rc) {
#ifdef __x86_64__
  char cf;
  int ax, dx, di, si;
  if (!IsWindows() && !IsMetal()) {
    // exit() on Linux
    // thr_exit() on FreeBSD
    // __threxit() on OpenBSD
    // __lwp_exit() on NetBSD
    // __bsdthread_terminate() on XNU
    asm volatile(CFLAG_ASM("xor\t%%r10d,%%r10d\n\t"
                           "syscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(ax), "=d"(dx), "=D"(di), "=S"(si)
                 : "1"(__NR_exit), "3"(IsLinux() ? rc : 0), "4"(0), "2"(0)
                 : "rcx", "r8", "r9", "r10", "r11", "memory");
    if ((IsFreebsd() && !cf && !ax) || (SupportsFreebsd() && IsTiny())) {
      // FreeBSD checks if this is either the main thread by itself, or
      // the last running child thread in which case thr_exit() returns
      // zero with an error. In that case we'll exit the whole process.
      // FreeBSD thr_exit() can even clobber registers, like r8 and r9!
      asm volatile("syscall"
                   : /* no outputs */
                   : "a"(__NR_exit_group), "D"(rc)
                   : "rcx", "r11", "memory");
      __builtin_unreachable();
    }
  } else if (IsWindows()) {
    __imp_ExitThread(rc);
    __builtin_unreachable();
  }
  notpossible;
#elif defined(__aarch64__)
  if (IsLinux() || IsFreebsd()) {
    register int x0 asm("x0") = rc;
    register int x8 asm("x8");
    if (IsLinux()) {
      x8 = 93;  // exit
    } else if (IsFreebsd()) {
      x8 = 431;  // thr_exit
    } else {
      __builtin_unreachable();
    }
    asm volatile("svc\t0" : "+r"(x0) : "r"(x8) : "memory");
    if (SupportsFreebsd()) {
      // On FreeBSD, thr_exit() fails if the current thread is orphaned.
      // In that case we're really better off just calling plain _exit()
      x0 = rc;
      asm volatile("mov\tx8,#1\n\t"
                   "svc\t0"
                   : /* no outputs */
                   : "r"(x0)
                   : "memory");
    }
  } else if (IsXnu()) {
    __syslib->__pthread_exit(0);
  }
  notpossible;
#else
#error "arch unsupported"
#endif
}
