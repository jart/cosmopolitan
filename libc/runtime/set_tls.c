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
#include "libc/nexgen32e/msr.internal.h"
#include "libc/nt/thread.h"
#include "libc/thread/tls.h"

int sys_set_tls();

void __set_tls(struct CosmoTib *tib) {
  // ask the operating system to change the x86 segment register
  int ax, dx;
  if (IsWindows()) {
    __tls_index = TlsAlloc();
    _npassert(0 <= __tls_index && __tls_index < 64);
    asm("mov\t%1,%%gs:%0" : "=m"(*((long *)0x1480 + __tls_index)) : "r"(tib));
  } else if (IsFreebsd()) {
    sys_set_tls(129 /*AMD64_SET_FSBASE*/, tib);
  } else if (IsLinux()) {
    sys_set_tls(ARCH_SET_FS, tib);
  } else if (IsNetbsd()) {
    // netbsd has sysarch(X86_SET_FSBASE) but we can't use that because
    // signal handlers will cause it to be reset due to not setting the
    // _mc_tlsbase field in struct mcontext_netbsd.
    sys_set_tls(tib);
  } else if (IsOpenbsd()) {
    sys_set_tls(tib);
  } else if (IsXnu()) {
    // thread_fast_set_cthread_self has a weird ABI
    int e = errno;
    sys_set_tls((intptr_t)tib - 0x30);
    errno = e;
  } else {
    uint64_t val = (uint64_t)tib;
    asm volatile("wrmsr"
                 : /* no outputs */
                 : "c"(MSR_IA32_FS_BASE), "a"((uint32_t)val),
                   "d"((uint32_t)(val >> 32)));
  }
}
