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
#include "libc/intrin/bsr.h"
#include "libc/nexgen32e/x86feature.h"
#include "tool/build/lib/clmul.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/modrm.h"

/**
 * @fileoverview Carryless Multiplication ISA
 */

struct clmul {
  uint64_t x, y;
};

static struct clmul clmul(uint64_t a, uint64_t b) {
  uint64_t t, x = 0, y = 0;
  if (a && b) {
    if (_bsrl(a) < _bsrl(b)) t = a, a = b, b = t;
    for (t = 0; b; a <<= 1, b >>= 1) {
      if (b & 1) x ^= a, y ^= t;
      t = t << 1 | a >> 63;
    }
  }
  return (struct clmul){x, y};
}

void OpSsePclmulqdq(struct Machine *m, uint32_t rde) {
  struct clmul res;
  res = clmul(Read64(XmmRexrReg(m, rde) + ((m->xedd->op.uimm0 & 0x01) << 3)),
              Read64(GetModrmRegisterXmmPointerRead16(m, rde) +
                     ((m->xedd->op.uimm0 & 0x10) >> 1)));
  Write64(XmmRexrReg(m, rde) + 0, res.x);
  Write64(XmmRexrReg(m, rde) + 8, res.y);
}
