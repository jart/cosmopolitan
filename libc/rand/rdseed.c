/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
