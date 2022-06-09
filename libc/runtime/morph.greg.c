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
#define ShouldUseMsabiAttribute() 1
#include "libc/bits/asmflag.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"

__msabi extern typeof(VirtualProtect) *const __imp_VirtualProtect;

static int64_t vector;
static sigset_t oldss;

static privileged void __morph_mprotect(void *addr, size_t size, int prot,
                                        int ntprot) {
  int ax, dx;
  uint32_t op;
  if (!IsWindows()) {
    asm volatile("syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(__NR_mprotect), "D"(addr), "S"(size), "1"(prot)
                 : "rcx", "r11", "memory");
  } else {
    __imp_VirtualProtect(addr, size, ntprot, &op);
  }
}

/**
 * Begins code morphing execuatble.
 *
 * @return 0 on success, or -1 w/ errno
 */
privileged void __morph_begin(void) {
  sigset_t ss;
  if (!IsWindows()) {
    sigfillset(&ss);
    sys_sigprocmask(SIG_BLOCK, &ss, &oldss);
  }
  __morph_mprotect(_base, __privileged_addr - _base, PROT_READ | PROT_WRITE,
                   kNtPageWritecopy);
}

/**
 * Begins code morphing execuatble.
 */
privileged void __morph_end(void) {
  __morph_mprotect(_base, __privileged_addr - _base, PROT_READ | PROT_EXEC,
                   kNtPageExecuteRead);
  if (!IsWindows()) {
    sys_sigprocmask(SIG_SETMASK, &oldss, 0);
  }
}
