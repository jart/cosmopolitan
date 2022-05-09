/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/atomic_load.h"

/**
 * Reads scalar from memory w/ one operation.
 *
 * This macro is intended to prevent things like compiler load tearing
 * optimizations.
 *
 * @param MEM is alignas(𝑘) uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @return *(MEM)
 * @note defeats compiler load tearing optimizations
 * @note alignas(𝑘) is implied if compiler knows type
 * @note alignas(𝑘) only avoids multi-core / cross-page edge cases
 * @see Intel's Six-Thousand Page Manual V.3A §8.2.3.1
 * @see atomic_store()
 */
intptr_t(atomic_load)(void *p, size_t n) {
  intptr_t x = 0;
  switch (n) {
    case 1:
      __builtin_memcpy(&x, p, 1);
      return x;
    case 2:
      __builtin_memcpy(&x, p, 2);
      return x;
    case 4:
      __builtin_memcpy(&x, p, 4);
      return x;
    case 8:
      __builtin_memcpy(&x, p, 8);
      return x;
    default:
      return 0;
  }
}
