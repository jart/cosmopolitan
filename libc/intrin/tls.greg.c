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
#include "libc/intrin/threaded.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/sysv/consts/nrlinux.h"

#define __NR_sysarch                      0x000000a5
#define __NR___set_tcb                    0x00000149
#define __NR__lwp_setprivate              0x0000013d
#define __NR_thread_fast_set_cthread_self 0x03000003

/**
 * Initializes thread information block.
 */
privileged void *__initialize_tls(char tib[hasatleast 64]) {
  *(intptr_t *)tib = (intptr_t)tib;
  *(intptr_t *)(tib + 0x30) = (intptr_t)tib;
  *(int *)(tib + 0x3c) = __errno;
  return tib;
}

/**
 * Installs thread information block on main process.
 */
privileged void __install_tls(char tib[hasatleast 64]) {
  int ax, dx;
  uint64_t magic;
  unsigned char *p;
  if (IsWindows()) {
    if (!__tls_index) {
      __tls_index = TlsAlloc();
    }
    asm("mov\t%1,%%gs:%0" : "=m"(*((long *)0x1480 + __tls_index)) : "r"(tib));
  } else if (IsFreebsd()) {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_sysarch), "D"(129), "S"(tib)
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
  } else if (IsNetbsd()) {
    asm volatile("syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(__NR__lwp_setprivate), "D"(tib)
                 : "rcx", "r11", "memory", "cc");
  } else {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_linux_arch_prctl), "D"(ARCH_SET_FS), "S"(tib)
                 : "rcx", "r11", "memory");
  }
  __tls_enabled = true;
}
