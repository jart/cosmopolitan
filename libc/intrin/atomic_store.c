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
#include "libc/intrin/atomic_store.h"

/**
 * Saves scalar to memory w/ one operation.
 *
 * This is guaranteed to happen in either one or zero operations,
 * depending on whether or not it's possible for *(MEM) to be read
 * afterwards. This macro only forbids compiler from using >1 ops.
 *
 * @param MEM is alignas(𝑘) uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @param VAL is uint𝑘_t w/ better encoding for immediates (constexpr)
 * @return VAL
 * @note alignas(𝑘) on nexgen32e only needed for end of page gotcha
 * @note alignas(𝑘) is implied if compiler knows type
 * @note needed to defeat store tearing optimizations
 * @see Intel Six-Thousand Page Manual Manual V.3A §8.2.3.1
 * @see atomic_load()
 */
intptr_t(atomic_store)(void *p, intptr_t x, size_t n) {
  switch (n) {
    case 1:
      __builtin_memcpy(p, &x, 1);
      return x;
    case 2:
      __builtin_memcpy(p, &x, 2);
      return x;
    case 4:
      __builtin_memcpy(p, &x, 4);
      return x;
    case 8:
      __builtin_memcpy(p, &x, 8);
      return x;
    default:
      return 0;
  }
}
