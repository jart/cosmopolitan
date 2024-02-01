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
#include "libc/errno.h"
#include "libc/intrin/asmflag.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/stdio/rand.h"
#include "libc/sysv/consts/grnd.h"

__static_yoink("rdrand_init");

static dontinline uint64_t rdrand_failover(void) {
  int f;
  size_t i;
  ssize_t r;
  volatile uint64_t b;
  register uint64_t x;
  for (f = GRND_RANDOM | GRND_NONBLOCK, i = 0; i < 8; i += r) {
    if ((r = getrandom((char *)&b + i, 8 - i, f)) <= 0) {
      if (r == -1 && errno == EINTR) {
        r = 0;
      } else if (r == -1 && errno == EAGAIN) {
        r = 0;
        f = 0;
      } else {
        return _rand64();
      }
    }
  }
  x = b;
  b = 0;
  return x;
}

/**
 * Retrieves 64-bits of hardware random data from RDRAND instruction.
 *
 * If RDRAND isn't available (we check CPUID and we also disable it
 * automatically for microarchitectures where it's slow or buggy) then
 * we try getrandom(), ProcessPrng(), or sysctl(KERN_ARND). If those
 * aren't available then we try /dev/urandom and if that fails, we try
 * getauxval(AT_RANDOM), and if not we finally use RDTSC and getpid().
 *
 * @note this function could block a nontrivial time on old computers
 * @note this function is indeed intended for cryptography
 * @note this function takes around 300 cycles
 * @see rngset(), rdseed(), _rand64()
 * @asyncsignalsafe
 * @vforksafe
 */
uint64_t rdrand(void) {
  int i;
  char cf;
  uint64_t x;
  if (X86_HAVE(RDRND)) {
    for (i = 0; i < 10; ++i) {
      asm volatile(CFLAG_ASM("rdrand\t%1")
                   : CFLAG_CONSTRAINT(cf), "=r"(x)
                   : /* no inputs */
                   : "cc");
      if (cf) return x;
      asm volatile("pause");
    }
  }
  return rdrand_failover();
}
