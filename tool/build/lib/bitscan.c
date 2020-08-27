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
#include "tool/build/lib/bitscan.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/modrm.h"

uint64_t AluBsr(struct Machine *m, uint32_t rde, uint64_t _, uint64_t x) {
  unsigned i;
  if (Rexw(rde)) {
    x &= 0xffffffffffffffff;
    m->flags = SetFlag(m->flags, FLAGS_ZF, !x);
    if (!x) return 0;
    return 63 ^ __builtin_clzll(x);
  } else if (!Osz(rde)) {
    x &= 0xffffffff;
    m->flags = SetFlag(m->flags, FLAGS_ZF, !x);
    if (!x) return 0;
    return 31 ^ __builtin_clz(x);
  } else {
    x &= 0xffff;
    m->flags = SetFlag(m->flags, FLAGS_ZF, !x);
    if (!x) return 0;
    for (i = 15; !(x & 0x8000); --i) x <<= 1;
    return i;
  }
}

uint64_t AluBsf(struct Machine *m, uint32_t rde, uint64_t _, uint64_t x) {
  unsigned i;
  if (Rexw(rde)) {
    x &= 0xffffffffffffffff;
    m->flags = SetFlag(m->flags, FLAGS_ZF, !x);
    if (!x) return 0;
    return __builtin_ctzll(x);
  } else if (!Osz(rde)) {
    x &= 0xffffffff;
    m->flags = SetFlag(m->flags, FLAGS_ZF, !x);
    if (!x) return 0;
    return __builtin_ctz(x);
  } else {
    x &= 0xffff;
    m->flags = SetFlag(m->flags, FLAGS_ZF, !x);
    if (!x) return 0;
    for (i = 0; !(x & 1); ++i) x >>= 1;
    return i;
  }
}

uint64_t AluPopcnt(struct Machine *m, uint32_t rde, uint64_t _, uint64_t x) {
  m->flags = SetFlag(m->flags, FLAGS_ZF, !x);
  m->flags = SetFlag(m->flags, FLAGS_CF, false);
  m->flags = SetFlag(m->flags, FLAGS_SF, false);
  m->flags = SetFlag(m->flags, FLAGS_OF, false);
  m->flags = SetFlag(m->flags, FLAGS_PF, false);
  x = x - ((x >> 1) & 0x5555555555555555);
  x = ((x >> 2) & 0x3333333333333333) + (x & 0x3333333333333333);
  x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
  x = (x + (x >> 32)) & 0xffffffff;
  x = x + (x >> 16);
  x = (x + (x >> 8)) & 0x7f;
  return x;
}
