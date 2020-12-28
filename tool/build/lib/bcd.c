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
#include "tool/build/lib/alu.h"
#include "tool/build/lib/bcd.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/throw.h"

relegated void OpDas(struct Machine *m, uint32_t rde) {
  uint8_t al, af, cf;
  af = cf = 0;
  al = m->ax[0];
  if ((al & 0x0f) > 9 || GetFlag(m->flags, FLAGS_AF)) {
    cf = m->ax[0] < 6 || GetFlag(m->flags, FLAGS_CF);
    m->ax[0] -= 0x06;
    af = 1;
  }
  if (al > 0x99 || GetFlag(m->flags, FLAGS_CF)) {
    m->ax[0] -= 0x60;
    cf = 1;
  }
  AluFlags8(m->ax[0], af, &m->flags, 0, cf);
}

relegated void OpAaa(struct Machine *m, uint32_t rde) {
  uint8_t af, cf;
  af = cf = 0;
  if ((m->ax[0] & 0x0f) > 9 || GetFlag(m->flags, FLAGS_AF)) {
    cf = m->ax[0] < 6 || GetFlag(m->flags, FLAGS_CF);
    Write16(m->ax, Read16(m->ax) + 0x106);
    af = cf = 1;
  }
  m->ax[0] &= 0x0f;
  AluFlags8(m->ax[0], af, &m->flags, 0, cf);
}

relegated void OpAas(struct Machine *m, uint32_t rde) {
  uint8_t af, cf;
  af = cf = 0;
  if ((m->ax[0] & 0x0f) > 9 || GetFlag(m->flags, FLAGS_AF)) {
    cf = m->ax[0] < 6 || GetFlag(m->flags, FLAGS_CF);
    Write16(m->ax, Read16(m->ax) - 0x106);
    af = cf = 1;
  }
  m->ax[0] &= 0x0f;
  AluFlags8(m->ax[0], af, &m->flags, 0, cf);
}

relegated void OpAam(struct Machine *m, uint32_t rde) {
  uint8_t i = m->xedd->op.uimm0;
  if (!i) ThrowDivideError(m);
  m->ax[1] = m->ax[0] / i;
  m->ax[0] = m->ax[0] % i;
  AluFlags8(m->ax[0], 0, &m->flags, 0, 0);
}

relegated void OpAad(struct Machine *m, uint32_t rde) {
  uint8_t i = m->xedd->op.uimm0;
  Write16(m->ax, (m->ax[1] * i + m->ax[0]) & 0xff);
  AluFlags8(m->ax[0], 0, &m->flags, 0, 0);
}
