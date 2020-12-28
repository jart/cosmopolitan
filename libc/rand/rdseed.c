/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/rand.h"

/**
 * Intel Securer Key Digital Random Number Generator
 * Introduced w/ Ivy Bridge c. 2013 and Excavator c. 2015
 * @see rdrand(), rand32(), rand64(), and randcpy()
 */
uint64_t rdseed(void) {
  char cf;
  size_t i;
  uint64_t res;
  assert(X86_HAVE(RDSEED));
  for (;;) {
    for (i = 0; i < 10; ++i) {
      /* CF=1: Destination register valid. Quoth Intel DRNG-SIG 4.1.3 */
      asm volatile(CFLAG_ASM("rdseed\t%1")
                   : CFLAG_CONSTRAINT(cf), "=r"(res)
                   : /* no inputs */
                   : "cc");
      if (cf) return res;
    }
    asm volatile("rep nop"); /* unlikely 140 cycle spin */
  }
}
