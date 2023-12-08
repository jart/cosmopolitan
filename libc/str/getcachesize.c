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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/nexgen32e/cachesize.h"
#include "libc/nexgen32e/cpuid4.internal.h"
#ifdef __x86_64__

static unsigned _getcachesize_cpuid4(int type, int level) {
  unsigned i, k;
  static int once;
  static unsigned char kCacheKey[8];
  static unsigned kCacheSize[8];
  if (!once) {
    CPUID4_ITERATE(i, {
      kCacheKey[i] = CPUID4_KEY;
      kCacheSize[i] = CPUID4_CACHE_SIZE_IN_BYTES;
    });
    once = 1;
  }
  k = ((level & 7) << 5) | (type & 31);
  for (i = 0; i < 8; ++i) {
    if (kCacheKey[i] == k) {
      return kCacheSize[i];
    }
  }
  return 0;
}

/**
 * Returns CPU cache size.
 *
 * @param type 1=data, 2=instruction, 3=unified
 * @param level starts at 1
 * @return size in bytes, or 0 if unknown
 */
unsigned _getcachesize(int type, int level) {
  unassert(1 <= type && type <= 3);
  unassert(level >= 1);
  return _getcachesize_cpuid4(type, level);
}

#endif
