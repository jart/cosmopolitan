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
#include "tool/build/lib/flags.h"

bool GetParity(uint8_t b) {
  b ^= b >> 4;
  b ^= b >> 2;
  b ^= b >> 1;
  return ~b & 1;
}

void ImportFlags(struct Machine *m, uint64_t flags) {
  uint64_t old, mask = 0;
  mask |= 1u << FLAGS_CF;
  mask |= 1u << FLAGS_PF;
  mask |= 1u << FLAGS_AF;
  mask |= 1u << FLAGS_ZF;
  mask |= 1u << FLAGS_SF;
  mask |= 1u << FLAGS_TF;
  mask |= 1u << FLAGS_IF;
  mask |= 1u << FLAGS_DF;
  mask |= 1u << FLAGS_OF;
  mask |= 1u << FLAGS_NT;
  mask |= 1u << FLAGS_AC;
  mask |= 1u << FLAGS_ID;
  m->flags = (flags & mask) | (m->flags & ~mask);
  m->flags = SetFlag(m->flags, FLAGS_RF, false);
  m->flags = SetLazyParityByte(m->flags, !((m->flags >> FLAGS_PF) & 1));
}

uint64_t ExportFlags(uint64_t flags) {
  flags = SetFlag(flags, FLAGS_IOPL, 3);
  flags = SetFlag(flags, FLAGS_F1, true);
  flags = SetFlag(flags, FLAGS_F0, false);
  flags = flags & ~(1ull << FLAGS_PF);
  flags |= GetLazyParityBool(flags) << FLAGS_PF;
  return flags;
}
