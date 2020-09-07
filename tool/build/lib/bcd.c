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
#include "tool/build/lib/alu.h"
#include "tool/build/lib/bcd.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/throw.h"

void OpDas(struct Machine *m, uint32_t rde) {
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

void OpAaa(struct Machine *m, uint32_t rde) {
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

void OpAas(struct Machine *m, uint32_t rde) {
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

void OpAam(struct Machine *m, uint32_t rde) {
  uint8_t i = m->xedd->op.uimm0;
  if (!i) ThrowDivideError(m);
  m->ax[1] = m->ax[0] / i;
  m->ax[0] = m->ax[0] % i;
  AluFlags8(m->ax[0], 0, &m->flags, 0, 0);
}

void OpAad(struct Machine *m, uint32_t rde) {
  uint8_t i = m->xedd->op.uimm0;
  Write16(m->ax, (m->ax[1] * i + m->ax[0]) & 0xff);
  AluFlags8(m->ax[0], 0, &m->flags, 0, 0);
}
