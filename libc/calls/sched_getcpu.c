/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/rseq.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/struct/processornumber.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns ID of CPU on which thread is currently scheduled.
 *
 * This function is supported on the following platforms:
 *
 * - aarch64
 *
 *   - Linux: via rseq shared memory, otherwise SYS_getcpu
 *   - MacOS: via Apple pthread_cpu_number_np() DSO API
 *   - FreeBSD: via SYS_sched_getcpu
 *
 * - x86-64
 *
 *   - Linux: via rseq, otherwise lsl msr, or rdpid, or rdtscp
 *   - FreeBSD: via lsl msr, or rdpid, or rdtscp
 *   - Windows: win32 vdso
 *   - OpenBSD: enosys
 *   - NetBSD: enosys
 *   - MacOS: enosys
 *
 * @return cpu number on success, or -1 w/ errno
 */
int sched_getcpu(void) {

  // Linux 4.18+ (c. 2018)
  if (IsLinux()) {
    int scpu = __get_rseq()->cpu_id;
    if (scpu >= 0)
      return scpu;
  }

  if (IsWindows()) {
    struct NtProcessorNumber pn;
    GetCurrentProcessorNumberEx(&pn);
    return 64 * pn.Group + pn.Number;
  }

  if (IsXnuSilicon()) {
    if (__syslib && __syslib->__version >= 9) {
      errno_t err;
      size_t out = 0;
      if ((err = __syslib->__pthread_cpu_number_np(&out))) {
        errno = err;
        return -1;
      }
      return out;
    }
  }

#ifdef __x86_64__

  if (IsLinux() || IsFreebsd()) {
    // Linux and FreeBSD both stuff TSC_AUX in the GDT so we can use this
    // ancient i8086 instruction to quickly retrieve the cpu id and node.
    bool ok;
    long aux;
    asm("lsl\t%2,%1" : "=@ccz"(ok), "=r"(aux) : "r"(0x7bl) : "memory");
    if (ok)
      return aux & 4095;
  }

  if (IsLinux() || IsFreebsd() || IsWindows()) {
    // Only the Linux, FreeBSD, and Windows kernels can be counted upon
    // to populate the TSC_AUX register with cpu id / node information.
    if (X86_HAVE(RDPID)) {
      long aux;
      asm("rdpid\t%0" : "=r"(aux));
      return aux & 4095;
    }
    if (X86_HAVE(RDTSCP)) {
      unsigned hi, lo, aux;
      asm("rdtscp" : "=d"(hi), "=a"(lo), "=c"(aux));
      return aux & 4095;
    }
  }

#elifdef __aarch64__

  if (IsLinux()) {
    // old linux or qemu-aarch64
    // issuing a system call takes like, forever
    unsigned res;
    register unsigned *x0 asm("x0") = &res;
    register unsigned *x1 asm("x1") = 0;
    register int x8 asm("x8") = 168;  // getcpu
    asm volatile("svc\t0" : "+r"(x0) : "r"(x1), "r"(x8) : "memory");
    return res;
  }

  if (IsFreebsd()) {
    // issuing a system call takes like, forever
    register int x0 asm("x0");
    register int x8 asm("x8") = 581;  // sched_getcpu
    asm volatile("svc\t0" : "=r"(x0) : "r"(x8) : "memory");
    return x0;
  }

#endif

  return enosys();
}
