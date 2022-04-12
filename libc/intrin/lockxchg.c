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
#include "libc/intrin/lockxchg.h"

/**
 * Compares and exchanges w/ lock prefix.
 *
 * @param memory is uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @param size is automatically supplied by macro wrapper
 * @return true if value was exchanged, otherwise false
 * @see xchg()
 */
intptr_t(lockxchg)(void *memory, void *localvar, size_t size) {
  switch (size) {
    case 1:
      return lockxchg((int8_t *)memory, (int8_t *)localvar);
    case 2:
      return lockxchg((int16_t *)memory, (int16_t *)localvar);
    case 4:
      return lockxchg((int32_t *)memory, (int32_t *)localvar);
    case 8:
      return lockxchg((int64_t *)memory, (int64_t *)localvar);
    default:
      return false;
  }
}
