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
#include "libc/log/check.h"
#include "libc/macros.h"
#include "tool/build/lib/address.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/stack.h"
#include "tool/build/lib/throw.h"

static const uint8_t kStackOsz[2][3] = {
    [0][XED_MODE_REAL] = 2, [0][XED_MODE_LEGACY] = 4, [0][XED_MODE_LONG] = 8,
    [1][XED_MODE_REAL] = 4, [1][XED_MODE_LEGACY] = 2, [1][XED_MODE_LONG] = 2,
};

static const uint8_t kCallOsz[2][3] = {
    [0][XED_MODE_REAL] = 2, [0][XED_MODE_LEGACY] = 4, [0][XED_MODE_LONG] = 8,
    [1][XED_MODE_REAL] = 4, [1][XED_MODE_LEGACY] = 2, [1][XED_MODE_LONG] = 8,
};

static void WriteStackWord(uint8_t *p, uint32_t rde, uint32_t osz, uint64_t x) {
  if (osz == 8) {
    Write64(p, x);
  } else if (osz == 2) {
    Write16(p, x);
  } else {
    Write32(p, x);
  }
}

static uint64_t ReadStackWord(uint8_t *p, uint32_t osz) {
  if (osz == 8) {
    return Read64(p);
  } else if (osz == 2) {
    return Read16(p);
  } else {
    return Read32(p);
  }
}

static void PushN(struct Machine *m, uint32_t rde, uint64_t x, unsigned osz) {
  uint64_t v;
  void *p[2];
  uint8_t b[8];
  switch (Eamode(rde)) {
    case XED_MODE_REAL:
      v = (Read32(m->sp) - osz) & 0xffff;
      Write16(m->sp, v);
      v += Read64(m->ss);
      break;
    case XED_MODE_LEGACY:
      v = (Read32(m->sp) - osz) & 0xffffffff;
      Write64(m->sp, v);
      v += Read64(m->ss);
      break;
    case XED_MODE_LONG:
      v = (Read64(m->sp) - osz) & 0xffffffffffffffff;
      Write64(m->sp, v);
      break;
    default:
      unreachable;
  }
  WriteStackWord(AccessRam(m, v, osz, p, b, false), rde, osz, x);
  EndStore(m, v, osz, p, b);
}

void Push(struct Machine *m, uint32_t rde, uint64_t x) {
  PushN(m, rde, x, kStackOsz[m->xedd->op.osz][Mode(rde)]);
}

void OpPushZvq(struct Machine *m, uint32_t rde) {
  unsigned osz;
  osz = kStackOsz[m->xedd->op.osz][Mode(rde)];
  PushN(m, rde, ReadStackWord(RegRexbSrm(m, rde), osz), osz);
}

static uint64_t PopN(struct Machine *m, uint32_t rde, uint16_t extra,
                     unsigned osz) {
  uint64_t v;
  void *p[2];
  uint8_t b[8];
  switch (Eamode(rde)) {
    case XED_MODE_LONG:
      v = Read64(m->sp);
      Write64(m->sp, v + osz + extra);
      break;
    case XED_MODE_LEGACY:
      v = Read32(m->sp);
      Write64(m->sp, (v + osz + extra) & 0xffffffff);
      v += Read64(m->ss);
      break;
    case XED_MODE_REAL:
      v = Read32(m->sp);
      Write16(m->sp, v + osz + extra);
      v += Read64(m->ss);
      break;
    default:
      unreachable;
  }
  return ReadStackWord(AccessRam(m, v, osz, p, b, true), osz);
}

uint64_t Pop(struct Machine *m, uint32_t rde, uint16_t extra) {
  return PopN(m, rde, extra, kStackOsz[m->xedd->op.osz][Mode(rde)]);
}

void OpPopZvq(struct Machine *m, uint32_t rde) {
  uint64_t x;
  unsigned osz;
  osz = kStackOsz[m->xedd->op.osz][Mode(rde)];
  x = PopN(m, rde, 0, osz);
  switch (osz) {
    case 8:
    case 4:
      Write64(RegRexbSrm(m, rde), x);
      break;
    case 2:
      Write16(RegRexbSrm(m, rde), x);
      break;
    default:
      unreachable;
  }
}

static void OpCall(struct Machine *m, uint32_t rde, uint64_t func) {
  Push(m, rde, m->ip);
  m->ip = func;
}

void OpCallJvds(struct Machine *m, uint32_t rde) {
  OpCall(m, rde, m->ip + m->xedd->op.disp);
}

static uint64_t LoadAddressFromMemory(struct Machine *m, uint32_t rde) {
  unsigned osz;
  osz = kCallOsz[m->xedd->op.osz][Mode(rde)];
  return ReadStackWord(GetModrmRegisterWordPointerRead(m, rde, osz), osz);
}

void OpCallEq(struct Machine *m, uint32_t rde) {
  OpCall(m, rde, LoadAddressFromMemory(m, rde));
}

void OpJmpEq(struct Machine *m, uint32_t rde) {
  m->ip = LoadAddressFromMemory(m, rde);
}

void OpLeave(struct Machine *m, uint32_t rde) {
  switch (Eamode(rde)) {
    case XED_MODE_LONG:
      Write64(m->sp, Read64(m->bp));
      Write64(m->bp, Pop(m, rde, 0));
      break;
    case XED_MODE_LEGACY:
      Write64(m->sp, Read32(m->bp));
      Write64(m->bp, Pop(m, rde, 0));
      break;
    case XED_MODE_REAL:
      Write16(m->sp, Read16(m->bp));
      Write16(m->bp, Pop(m, rde, 0));
      break;
    default:
      unreachable;
  }
}

void OpRet(struct Machine *m, uint32_t rde) {
  m->ip = Pop(m, rde, m->xedd->op.uimm0);
}

void OpPushEvq(struct Machine *m, uint32_t rde) {
  unsigned osz;
  osz = kStackOsz[m->xedd->op.osz][Mode(rde)];
  Push(m, rde,
       ReadStackWord(GetModrmRegisterWordPointerRead(m, rde, osz), osz));
}

void OpPopEvq(struct Machine *m, uint32_t rde) {
  unsigned osz;
  osz = kStackOsz[m->xedd->op.osz][Mode(rde)];
  WriteStackWord(GetModrmRegisterWordPointerWrite(m, rde, osz), rde, osz,
                 Pop(m, rde, 0));
}

static relegated void Pushaw(struct Machine *m, uint32_t rde) {
  uint16_t v;
  uint8_t b[8][2];
  memcpy(b[0], m->di, 2);
  memcpy(b[1], m->si, 2);
  memcpy(b[2], m->bp, 2);
  memcpy(b[3], m->sp, 2);
  memcpy(b[4], m->bx, 2);
  memcpy(b[5], m->dx, 2);
  memcpy(b[6], m->cx, 2);
  memcpy(b[7], m->ax, 2);
  Write16(m->sp, (v = (Read16(m->sp) - sizeof(b)) & 0xffff));
  VirtualRecv(m, Read64(m->ss) + v, b, sizeof(b));
}

static relegated void Pushad(struct Machine *m, uint32_t rde) {
  uint32_t v;
  uint8_t b[8][4];
  memcpy(b[0], m->di, 4);
  memcpy(b[1], m->si, 4);
  memcpy(b[2], m->bp, 4);
  memcpy(b[3], m->sp, 4);
  memcpy(b[4], m->bx, 4);
  memcpy(b[5], m->dx, 4);
  memcpy(b[6], m->cx, 4);
  memcpy(b[7], m->ax, 4);
  Write64(m->sp, (v = (Read32(m->sp) - sizeof(b)) & 0xffffffff));
  VirtualRecv(m, Read64(m->ss) + v, b, sizeof(b));
}

static relegated void Popaw(struct Machine *m, uint32_t rde) {
  uint8_t b[8][2];
  VirtualSend(m, b, Read64(m->ss) + Read16(m->sp), sizeof(b));
  Write16(m->sp, (Read32(m->sp) + sizeof(b)) & 0xffff);
  memcpy(m->di, b[0], 2);
  memcpy(m->si, b[1], 2);
  memcpy(m->bp, b[2], 2);
  memcpy(m->sp, b[3], 2);
  memcpy(m->bx, b[4], 2);
  memcpy(m->dx, b[5], 2);
  memcpy(m->cx, b[6], 2);
  memcpy(m->ax, b[7], 2);
}

static relegated void Popad(struct Machine *m, uint32_t rde) {
  uint8_t b[8][4];
  VirtualSend(m, b, Read64(m->ss) + Read32(m->sp), sizeof(b));
  Write64(m->sp, (Read32(m->sp) + sizeof(b)) & 0xffffffff);
  memcpy(m->di, b[0], 4);
  memcpy(m->si, b[1], 4);
  memcpy(m->bp, b[2], 4);
  memcpy(m->sp, b[3], 4);
  memcpy(m->bx, b[4], 4);
  memcpy(m->dx, b[5], 4);
  memcpy(m->cx, b[6], 4);
  memcpy(m->ax, b[7], 4);
}

relegated void OpPusha(struct Machine *m, uint32_t rde) {
  switch (Eamode(rde)) {
    case XED_MODE_REAL:
      Pushaw(m, rde);
      break;
    case XED_MODE_LEGACY:
      Pushad(m, rde);
      break;
    case XED_MODE_LONG:
      OpUd(m, rde);
    default:
      unreachable;
  }
}

relegated void OpPopa(struct Machine *m, uint32_t rde) {
  switch (Eamode(rde)) {
    case XED_MODE_REAL:
      Popaw(m, rde);
      break;
    case XED_MODE_LEGACY:
      Popad(m, rde);
      break;
    case XED_MODE_LONG:
      OpUd(m, rde);
    default:
      unreachable;
  }
}

relegated void OpCallf(struct Machine *m, uint32_t rde) {
  Push(m, rde, Read64(m->cs) >> 4);
  Push(m, rde, m->ip);
  Write64(m->cs, m->xedd->op.uimm0 << 4);
  m->ip = m->xedd->op.disp & (Osz(rde) ? 0xffff : 0xffffffff);
  if (m->onlongbranch) {
    m->onlongbranch(m);
  }
}

relegated void OpRetf(struct Machine *m, uint32_t rde) {
  m->ip = Pop(m, rde, 0);
  Write64(m->cs, Pop(m, rde, m->xedd->op.uimm0) << 4);
  if (m->onlongbranch) {
    m->onlongbranch(m);
  }
}
