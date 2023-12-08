/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mremap.h"
#include "libc/sysv/errfuns.h"

/**
 * Relocates memory.
 *
 * This function lets you move to to different addresses witohut copying
 * it. This system call is currently supported on Linux and NetBSD. Your
 * C library runtime won't have any awareness of this memory, so certain
 * features like ASAN memory safety and kprintf() won't work as well.
 */
void *sys_mremap(void *p, size_t n, size_t m, int f, void *q) {
#ifdef __x86_64__
  bool cf;
  uintptr_t res, rdx;
  register uintptr_t r8 asm("r8");
  register uintptr_t r10 asm("r10");
  if (IsLinux()) {
    r10 = f;
    r8 = (uintptr_t)q;
    asm("syscall"
        : "=a"(res)
        : "0"(0x019), "D"(p), "S"(n), "d"(m), "r"(r10), "r"(r8)
        : "rcx", "r11", "memory", "cc");
    if (res > -4096ul) errno = -res, res = -1;
  } else if (IsNetbsd()) {
    if (f & MREMAP_MAYMOVE) {
      res = 0x19B;
      r10 = m;
      r8 = (f & MREMAP_FIXED) ? MAP_FIXED : 0;
      asm(CFLAG_ASM("syscall")
          : CFLAG_CONSTRAINT(cf), "+a"(res), "=d"(rdx)
          : "D"(p), "S"(n), "2"(q), "r"(r10), "r"(r8)
          : "rcx", "r9", "r11", "memory", "cc");
      if (cf) errno = res, res = -1;
    } else {
      res = einval();
    }
  } else {
    res = enosys();
  }
#elif defined(__aarch64__)
  void *res;
  res = __sys_mremap(p, n, m, f, q);
#else
#error "arch unsupported"
#endif
  KERNTRACE("sys_mremap(%p, %'zu, %'zu, %#b, %p) → %p% m", p, n, m, f, q, res);
  return (void *)res;
}
