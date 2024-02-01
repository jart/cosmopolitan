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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/nexgen32e/msr.internal.h"
#include "libc/nt/thread.h"
#include "libc/sysv/consts/arch.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.internal.h"

#define AMD64_SET_FSBASE 129
#define AMD64_SET_GSBASE 131

int sys_set_tls(uintptr_t, void *);

// we can't allow --ftrace here because cosmo_dlopen() calls this
// function to fix the tls register, and ftrace needs it unbroken
dontinstrument textstartup void __set_tls(struct CosmoTib *tib) {
  tib = __adj_tls(tib);
#ifdef __x86_64__
  // ask the operating system to change the x86 segment register
  if (IsWindows()) {
    asm("mov\t%1,%%gs:%0" : "=m"(*((long *)0x1480 + __tls_index)) : "r"(tib));
  } else if (IsFreebsd()) {
    sys_set_tls(__tls_morphed ? AMD64_SET_GSBASE : AMD64_SET_FSBASE, tib);
  } else if (IsLinux()) {
    sys_set_tls(__tls_morphed ? ARCH_SET_GS : ARCH_SET_FS, tib);
  } else if (IsNetbsd()) {
    // netbsd has sysarch(X86_SET_FSBASE) but we can't use that because
    // signal handlers will cause it to be reset due to not setting the
    // _mc_tlsbase field in struct mcontext_netbsd.
    sys_set_tls((uintptr_t)tib, 0);
  } else if (IsOpenbsd()) {
    sys_set_tls((uintptr_t)tib, 0);
  } else if (IsXnu()) {
    // thread_fast_set_cthread_self has a weird ABI
    sys_set_tls((intptr_t)tib - 0x30, 0);
  } else {
    uint64_t val = (uint64_t)tib;
    asm volatile("wrmsr"
                 : /* no outputs */
                 : "c"(MSR_IA32_FS_BASE), "a"((uint32_t)val),
                   "d"((uint32_t)(val >> 32)));
  }
#elif defined(__aarch64__)
  register long x28 asm("x28") = (long)tib;
  asm volatile("" : "+r"(x28));
#else
#error "unsupported architecture"
#endif
}
