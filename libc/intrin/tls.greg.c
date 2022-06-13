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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/sysv/consts/nrlinux.h"

#define __NR_sysarch     0x000000a5  // freebsd+netbsd
#define AMD64_SET_GSBASE 131         // freebsd
#define AMD64_SET_FSBASE 129         // freebsd
#define X86_SET_GSBASE   16          // netbsd
#define X86_SET_FSBASE   17          // netbsd

#define __NR___set_tcb                    0x00000149
#define __NR__lwp_setprivate              0x0000013d
#define __NR_thread_fast_set_cthread_self 0x03000003

/**
 * Initializes thread information block.
 *
 * Here's the layout your c library assumes:
 *
 *     offset size description
 *     0x0000 0x08 linear address pointer
 *     0x0030 0x08 linear address pointer
 *     0x0038 0x04 tid
 *     0x003c 0x04 errno
 *
 */
privileged void *__initialize_tls(char tib[64]) {
  if (tib) {
    *(intptr_t *)tib = (intptr_t)tib;
    *(intptr_t *)(tib + 0x30) = (intptr_t)tib;
    *(int *)(tib + 0x38) = -1;  // tid
    *(int *)(tib + 0x3c) = 0;
  }
  return tib;
}

/**
 * Installs thread information block on main process.
 *
 * For example, to set up TLS correctly for the main thread, without
 * creating any threads using `clone` (which does this automatically),
 * it is sufficient to say:
 *
 *     __attribute__((__constructor__)) static void InitTls(void) {
 *       static char tls[64];
 *       __initialize_tls(tls);
 *       __threaded = *(int *)(tls + 0x38) = gettid();
 *       *(int *)(tls + 0x3c) = __errno;
 *       __install_tls(tls);
 *     }
 *
 * Since that'll ensure it happens exactly once.
 */
privileged void __install_tls(char tib[64]) {
  int ax, dx;
  assert(tib);
  assert(!__tls_enabled);
  assert(*(int *)(tib + 0x38) != -1);
  if (IsWindows()) {
    if (!__tls_index) {
      __tls_index = TlsAlloc();
    }
    asm("mov\t%1,%%gs:%0" : "=m"(*((long *)0x1480 + __tls_index)) : "r"(tib));
  } else if (IsFreebsd()) {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_sysarch), "D"(AMD64_SET_FSBASE), "S"(tib)
                 : "rcx", "r11", "memory", "cc");
  } else if (IsNetbsd()) {
    asm volatile("syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(__NR_sysarch), "D"(X86_SET_FSBASE), "S"(tib)
                 : "rcx", "r11", "memory", "cc");
  } else if (IsXnu()) {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_thread_fast_set_cthread_self),
                   "D"((intptr_t)tib - 0x30)
                 : "rcx", "r11", "memory", "cc");
  } else if (IsOpenbsd()) {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR___set_tcb), "D"(tib)
                 : "rcx", "r11", "memory", "cc");
  } else {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_linux_arch_prctl), "D"(ARCH_SET_FS), "S"(tib)
                 : "rcx", "r11", "memory");
  }
  __tls_enabled = true;
}
