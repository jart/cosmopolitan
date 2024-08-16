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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/struct/processornumber.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/errfuns.h"

int sys_getcpu(unsigned *opt_cpu, unsigned *opt_node, void *tcache);

/**
 * Returns ID of CPU on which thread is currently scheduled.
 *
 * This function is supported on the following platforms:
 *
 * - x86-64
 *
 *   - Linux: rdtsc
 *   - FreeBSD: rdtsc
 *   - Windows: win32
 *   - OpenBSD: unsupported
 *   - NetBSD: unsupported
 *   - MacOS: unsupported
 *
 * - aarch64
 *
 *   - Linux: syscall
 *   - FreeBSD: syscall
 *   - MacOS: supported
 *
 * @return cpu number on success, or -1 w/ errno
 */
int sched_getcpu(void) {

  if (IsWindows()) {
    struct NtProcessorNumber pn;
    GetCurrentProcessorNumberEx(&pn);
    return 64 * pn.Group + pn.Number;
  }

#ifdef __x86_64__
  if (X86_HAVE(RDTSCP) && (IsLinux() || IsFreebsd())) {
    // Only the Linux, FreeBSD, and Windows kernels can be counted upon
    // to populate the TSC_AUX register with the current thread number.
    unsigned tsc_aux;
    rdtscp(&tsc_aux);
    return TSC_AUX_CORE(tsc_aux);
  }
#endif

#ifdef __aarch64__
  if (IsXnu()) {
    // pthread_cpu_number_np() is defined by MacOS 11.0+ (Big Sur) in
    // the SDK pthread.h header file, even though there's no man page
    if (__syslib && __syslib->__version >= 9) {
      errno_t err;
      size_t out = 0;
      if ((err = __syslib->__pthread_cpu_number_np(&out))) {
        errno = err;
        return -1;
      }
      return out;
    } else {
      errno = ENOSYS;  // upgrade your ape loader
      return -1;       // cc -o /usr/local/bin/ape ape/ape-m1.c
    }
  }
#endif

#ifdef __aarch64__
  if (IsFreebsd()) {
    register int x0 asm("x0");
    register int x8 asm("x8") = 581;  // sched_getcpu
    asm volatile("svc\t0" : "=r"(x0) : "r"(x8) : "memory");
    return x0;
  }
#endif

  unsigned cpu = 0;
  int rc = sys_getcpu(&cpu, 0, 0);
  if (rc == -1)
    return -1;
  return cpu;
}
