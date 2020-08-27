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
#include "libc/log/check.h"
#include "libc/macros.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/stack.h"

void Push64(struct Machine *m, uint64_t x) {
  uint64_t v;
  void *p[2];
  uint8_t b[8];
  v = Read64(m->sp);
  v -= 8;
  Write64(m->sp, v);
  Write64(AccessRam(m, v, 8, p, b, false), x);
  EndStore(m, v, 8, p, b);
}

void Push16(struct Machine *m, uint16_t x) {
  uint64_t v;
  void *p[2];
  uint8_t b[2];
  v = Read64(m->sp);
  v -= 2;
  Write64(m->sp, v);
  Write16(b, x);
  Write64(AccessRam(m, v, 2, p, b, false), x);
  EndStore(m, v, 2, p, b);
}

uint64_t Pop64(struct Machine *m, uint16_t extra) {
  void *p[2];
  uint8_t b[8];
  uint64_t v, x;
  v = Read64(m->sp);
  x = Read64(AccessRam(m, v, 8, p, b, true));
  Write64(m->sp, v + 8 + extra);
  return x;
}

uint16_t Pop16(struct Machine *m, uint16_t extra) {
  void *p[2];
  uint8_t b[2];
  uint16_t v, x;
  v = Read64(m->sp);
  x = Read16(AccessRam(m, v, 2, p, b, true));
  Write64(m->sp, v + 2 + extra);
  return x;
}

static void OpCall(struct Machine *m, uint64_t func) {
  Push64(m, m->ip);
  m->ip = func;
}

void OpCallJvds(struct Machine *m) {
  OpCall(m, m->ip + m->xedd->op.disp);
}

void OpCallEq(struct Machine *m, uint32_t rde) {
  void *p[2];
  uint8_t b[8];
  OpCall(m, Read64(IsModrmRegister(rde)
                       ? RegRexbRm(m, rde)
                       : AccessRam(m, ComputeAddress(m, rde), 8, p, b, true)));
}

void OpLeave(struct Machine *m) {
  memcpy(m->sp, m->bp, sizeof(m->sp));
  Write64(m->bp, Pop64(m, 0));
}

void OpRet(struct Machine *m, uint16_t n) {
  m->ip = Pop64(m, n);
}

void PushOsz(struct Machine *m, uint32_t rde, uint64_t x) {
  if (!Osz(rde)) {
    Push64(m, x);
  } else {
    Push16(m, x);
  }
}

void OpBofram(struct Machine *m) {
  if (m->xedd->op.disp) {
    m->bofram[0] = m->ip;
    m->bofram[1] = m->ip + (m->xedd->op.disp & 0xff);
  } else {
    m->bofram[0] = 0;
    m->bofram[1] = 0;
  }
}
