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
#include "libc/bits/bits.h"

/**
 * Compares and exchanges.
 *
 * @param ifthing is uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @param size is automatically supplied by macro wrapper
 * @return true if value was exchanged, otherwise false
 * @see lockcmpxchg()
 */
bool(cmpxchg)(void *ifthing, intptr_t isequaltome, intptr_t replaceitwithme,
              size_t size) {
  switch (size) {
    case 1:
      return cmpxchg((int8_t *)ifthing, (int8_t)isequaltome,
                     (int8_t)replaceitwithme);
    case 2:
      return cmpxchg((int16_t *)ifthing, (int16_t)isequaltome,
                     (int16_t)replaceitwithme);
    case 4:
      return cmpxchg((int32_t *)ifthing, (int32_t)isequaltome,
                     (int32_t)replaceitwithme);
    case 8:
      return cmpxchg((int64_t *)ifthing, (int64_t)isequaltome,
                     (int64_t)replaceitwithme);
    default:
      return false;
  }
}
