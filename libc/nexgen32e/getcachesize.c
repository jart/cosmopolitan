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
#include "libc/dce.h"
#include "libc/nexgen32e/cachesize.h"
#include "libc/nexgen32e/cpuid4.h"

static unsigned getcachesize$cpuid4(int type, int level) {
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
unsigned getcachesize(enum CpuCacheType type, int level) {
  assert(1 <= type && type <= 3);
  assert(level >= 1);
  return getcachesize$cpuid4(type, level);
}
