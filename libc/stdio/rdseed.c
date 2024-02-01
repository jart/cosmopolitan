/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/grnd.h"

/**
 * Retrieves 64-bits of true random data from RDSEED instruction.
 *
 * If RDSEED isn't available, we'll try RDRAND (which we automatically
 * disable for microarchitectures where it's known to be slow or buggy).
 * If RDRAND isn't available then we try getrandom(), ProcessPrng(), or
 * sysctl(KERN_ARND). If those aren't available then we try /dev/urandom
 * and if that fails, we use RDTSC and getpid().
 *
 * @note this function could block a nontrivial time on old computers
 * @note this function is indeed intended for cryptography
 * @note this function takes around 800 cycles
 * @see rngset(), rdrand(), _rand64()
 * @asyncsignalsafe
 * @vforksafe
 */
uint64_t rdseed(void) {
  int i;
  char cf;
  uint64_t x;
  if (X86_HAVE(RDSEED)) {
    for (i = 0; i < 10; ++i) {
      asm volatile(CFLAG_ASM("rdseed\t%1")
                   : CFLAG_CONSTRAINT(cf), "=r"(x)
                   : /* no inputs */
                   : "cc");
      if (cf) return x;
      asm volatile("pause");
    }
  }
  return rdrand();
}
