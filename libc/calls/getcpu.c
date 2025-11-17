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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/struct/processornumber.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/errfuns.h"

static int getcpu_ia32_tsc_aux(unsigned *cpu, unsigned *node, long aux) {
  if (cpu)
    *cpu = aux & 4095;
  if (node)
    *node = (aux >> 12) & 4095;
  return 0;
}

/**
 * Determines cpu and/or node on which thread is currently scheduled.
 *
 * If you don't need the node, consider just calling sched_getcpu(). If
 * you use this function as a generalized solution, then it's important
 * to set `out_opt_node` to NULL when it's not desired, since there's a
 * nontrivial cost for fetching the node id on some OSes, e.g. Windows.
 *
 * @return 0 on success, or -1 w/ errno
 */
int getcpu(unsigned *out_opt_cpu, unsigned *out_opt_node) {

  // Linux 4.18+ (c. 2018)
  if (IsLinux()) {
    int cpu_id;
    struct rseq *rseq;
    if ((cpu_id = (rseq = __get_rseq())->cpu_id) >= 0) {
      if (out_opt_cpu)
        *out_opt_cpu = cpu_id;
      if (out_opt_node)
        *out_opt_node = rseq->node_id;
      return 0;
    }
  }

  if (IsWindows()) {
    struct NtProcessorNumber pn;
    if (out_opt_cpu) {
      GetCurrentProcessorNumberEx(&pn);
      *out_opt_cpu = 64 * pn.Group + pn.Number;
    }
    if (out_opt_node) {
      // using this api costs ~70ns
      unsigned short node16;
      if (GetNumaProcessorNodeEx(&pn, &node16)) {
        *out_opt_node = node16;
      } else {
        return __winerr();
      }
    }
    return 0;
  }

  if (IsXnuSilicon()) {
    if (__syslib && __syslib->__version >= 9) {
      errno_t err;
      size_t out = 0;
      if ((err = __syslib->__pthread_cpu_number_np(&out))) {
        errno = err;
        return -1;
      }
      if (out_opt_cpu)
        *out_opt_cpu = out;
      if (out_opt_node)
        *out_opt_node = 0;
      return 0;
    }
  }

#ifdef __x86_64__

  if (IsLinux() || IsFreebsd()) {
    // Linux and FreeBSD both stuff TSC_AUX in the GDT so we can use this
    // ancient i8086 instruction to quickly retrieve the cpu id and node!
    bool ok;
    long aux;
    asm("lsl\t%2,%1" : "=@ccz"(ok), "=r"(aux) : "r"(0x7bl) : "memory");
    if (ok)
      return getcpu_ia32_tsc_aux(out_opt_cpu, out_opt_node, aux);
  }

  if (IsLinux() || IsFreebsd() || IsWindows()) {
    // Only the Linux, FreeBSD, and Windows kernels can be counted upon
    // to populate the TSC_AUX register with cpu id / node information.
    if (X86_HAVE(RDPID)) {
      long aux;
      asm("rdpid\t%0" : "=r"(aux));
      return getcpu_ia32_tsc_aux(out_opt_cpu, out_opt_node, aux);
    }
    if (X86_HAVE(RDTSCP)) {
      unsigned hi, lo, aux;
      asm("rdtscp" : "=d"(hi), "=a"(lo), "=c"(aux));
      return getcpu_ia32_tsc_aux(out_opt_cpu, out_opt_node, aux);
    }
  }

#elifdef __aarch64__

  if (IsLinux()) {
    // old linux or qemu-aarch64
    // issuing a system call takes like, forever
    register unsigned *x0 asm("x0") = out_opt_cpu;
    register unsigned *x1 asm("x1") = out_opt_node;
    register int x8 asm("x8") = 168;  // getcpu
    asm volatile("svc\t0" : "+r"(x0) : "r"(x1), "r"(x8) : "memory");
    if ((long)x0 < 0) {
      errno = -(long)x0;
      return -1;
    }
    return 0;
  }

  if (IsFreebsd()) {
    // issuing a system call takes like, forever
    register int x0 asm("x0");
    register int x8 asm("x8") = 581;  // sched_getcpu
    asm volatile("svc\t0" : "=r"(x0) : "r"(x8) : "memory");
    if (out_opt_cpu)
      *out_opt_cpu = x0;
    if (out_opt_node)
      *out_opt_node = 0;
    return 0;
  }

#endif

  return enosys();
}
