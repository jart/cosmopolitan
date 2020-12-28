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

int64_t AluFlags(uint64_t x, uint32_t af, uint32_t *f, uint32_t of, uint32_t cf,
                 uint32_t sf) {
  *f &= ~(1u << FLAGS_CF | 1u << FLAGS_ZF | 1u << FLAGS_SF | 1u << FLAGS_OF |
          1u << FLAGS_AF | 0xFF000000u);
  *f |= sf << FLAGS_SF | cf << FLAGS_CF | !x << FLAGS_ZF | of << FLAGS_OF |
        af << FLAGS_AF | (x & 0xFF) << 24;
  return x;
}
