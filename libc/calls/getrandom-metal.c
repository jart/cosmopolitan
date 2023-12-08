/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/asmflag.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

static bool GetRandomRdseed(uint64_t *out) {
  int i;
  char cf;
  uint64_t x;
  for (i = 0; i < 10; ++i) {
    asm volatile(CFLAG_ASM("rdseed\t%1")
                 : CFLAG_CONSTRAINT(cf), "=r"(x)
                 : /* no inputs */
                 : "cc");
    if (cf) {
      *out = x;
      return true;
    }
    asm volatile("pause");
  }
  return false;
}

static bool GetRandomRdrand(uint64_t *out) {
  int i;
  char cf;
  uint64_t x;
  for (i = 0; i < 10; ++i) {
    asm volatile(CFLAG_ASM("rdrand\t%1")
                 : CFLAG_CONSTRAINT(cf), "=r"(x)
                 : /* no inputs */
                 : "cc");
    if (cf) {
      *out = x;
      return true;
    }
    asm volatile("pause");
  }
  return false;
}

static ssize_t GetRandomCpu(char *p, size_t n, int f, bool impl(uint64_t *)) {
  uint64_t x;
  size_t i, j;
  for (i = 0; i < n; i += j) {
  TryAgain:
    if (!impl(&x)) {
      if (f || i >= 256) break;
      goto TryAgain;
    }
    for (j = 0; j < 8 && i + j < n; ++j) {
      p[i + j] = x;
      x >>= 8;
    }
  }
  return n;
}

ssize_t sys_getrandom_metal(char *p, size_t n, int f) {
  if (f & GRND_RANDOM) {
    if (X86_HAVE(RDSEED)) {
      return GetRandomCpu(p, n, f, GetRandomRdseed);
    } else {
      return enosys();
    }
  } else {
    if (X86_HAVE(RDRND)) {
      return GetRandomCpu(p, n, f, GetRandomRdrand);
    } else {
      return enosys();
    }
  }
}

#endif /* __x86_64__ */
