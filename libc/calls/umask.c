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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/nr.h"

static inline unsigned sys_umask(unsigned newmask) {
#ifdef __x86_64__
  unsigned res;
  asm volatile("syscall"
               : "=a"(res)
               : "0"(__NR_umask), "D"(newmask)
               : "memory", "cc");
#elif defined(__aarch64__)
  // xnu m1 doesn't manage carry flag
  register long r0 asm("x0") = newmask;
  register long r8 asm("x8") = __NR_umask & 0x7ff;
  register long r16 asm("x16") = __NR_umask & 0x7ff;
  register unsigned res asm("x0");
  asm volatile("svc\t0" : "=r"(res) : "r"(r0), "r"(r8), "r"(r16) : "memory");
#endif
  return res;
}

/**
 * Sets file mode creation mask.
 *
 * @return previous mask
 * @note always succeeds
 */
unsigned umask(unsigned newmask) {
  int oldmask;
  if (!IsWindows()) {
    oldmask = sys_umask(newmask);
  } else {
    // TODO(jart): what should we do with this?
    oldmask = newmask;
  }
  STRACE("umask(%#o) → %#o", oldmask);
  return oldmask;
}
