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
#include "libc/dce.h"
#include "libc/intrin/pshufd.h"
#include "libc/intrin/pshufhw.h"
#include "libc/intrin/pshuflw.h"
#include "libc/intrin/pshufw.h"
#include "libc/intrin/shufpd.h"
#include "libc/intrin/shufps.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/str/str.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/ssefloat.h"
#include "tool/build/lib/throw.h"

void OpUnpcklpsd(struct Machine *m, uint32_t rde) {
  uint8_t *a, *b;
  a = XmmRexrReg(m, rde);
  b = GetModrmRegisterXmmPointerRead8(m, rde);
  if (Osz(rde)) {
    memcpy(a + 8, b, 8);
  } else {
    memcpy(a + 4 * 3, b + 4, 4);
    memcpy(a + 4 * 2, a + 4, 4);
    memcpy(a + 4 * 1, b + 0, 4);
  }
}

void OpUnpckhpsd(struct Machine *m, uint32_t rde) {
  uint8_t *a, *b;
  a = XmmRexrReg(m, rde);
  b = GetModrmRegisterXmmPointerRead16(m, rde);
  if (Osz(rde)) {
    memcpy(a + 0, b + 8, 8);
    memcpy(a + 8, b + 8, 8);
  } else {
    memcpy(a + 4 * 0, a + 4 * 2, 4);
    memcpy(a + 4 * 1, b + 4 * 2, 4);
    memcpy(a + 4 * 2, a + 4 * 3, 4);
    memcpy(a + 4 * 3, b + 4 * 3, 4);
  }
}

void OpPextrwGdqpUdqIb(struct Machine *m, uint32_t rde) {
  uint8_t i;
  i = m->xedd->op.uimm0;
  i &= Osz(rde) ? 7 : 3;
  Write16(RegRexrReg(m, rde), Read16(XmmRexbRm(m, rde) + i * 2));
}

void OpPinsrwVdqEwIb(struct Machine *m, uint32_t rde) {
  uint8_t i;
  i = m->xedd->op.uimm0;
  i &= Osz(rde) ? 7 : 3;
  Write16(XmmRexrReg(m, rde) + i * 2,
          Read16(GetModrmRegisterWordPointerRead2(m, rde)));
}

void OpShuffle(struct Machine *m, uint32_t rde) {
  int16_t q16[4];
  int16_t x16[8];
  int32_t x32[4];
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      memcpy(q16, GetModrmRegisterXmmPointerRead8(m, rde), 8);
      (pshufw)(q16, q16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m, rde), q16, 8);
      break;
    case 1:
      memcpy(x32, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      (pshufd)(x32, x32, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m, rde), x32, 16);
      break;
    case 2:
      memcpy(x16, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      (pshuflw)(x16, x16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m, rde), x16, 16);
      break;
    case 3:
      memcpy(x16, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      (pshufhw)(x16, x16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m, rde), x16, 16);
      break;
    default:
      unreachable;
  }
}

static void Shufps(struct Machine *m, uint32_t rde) {
  shufps((void *)XmmRexrReg(m, rde), (void *)XmmRexrReg(m, rde),
         (void *)GetModrmRegisterXmmPointerRead16(m, rde), m->xedd->op.uimm0);
}

static void Shufpd(struct Machine *m, uint32_t rde) {
  shufpd((void *)XmmRexrReg(m, rde), (void *)XmmRexrReg(m, rde),
         (void *)GetModrmRegisterXmmPointerRead16(m, rde), m->xedd->op.uimm0);
}

void OpShufpsd(struct Machine *m, uint32_t rde) {
  if (Osz(rde)) {
    Shufpd(m, rde);
  } else {
    Shufps(m, rde);
  }
}

void OpSqrtpsd(struct Machine *m, uint32_t rde) {
  long i;
  float xf[4];
  double xd[2];
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      memcpy(xf, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      for (i = 0; i < 4; ++i) xf[i] = sqrtf(xf[i]);
      memcpy(XmmRexrReg(m, rde), xf, 16);
      break;
    case 1:
      memcpy(xd, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      for (i = 0; i < 2; ++i) xd[i] = sqrt(xd[i]);
      memcpy(XmmRexrReg(m, rde), xd, 16);
      break;
    case 2:
      memcpy(xd, GetModrmRegisterXmmPointerRead8(m, rde), 8);
      xd[0] = sqrt(xd[0]);
      memcpy(XmmRexrReg(m, rde), xd, 8);
      break;
    case 3:
      memcpy(xf, GetModrmRegisterXmmPointerRead4(m, rde), 4);
      xf[0] = sqrtf(xf[0]);
      memcpy(XmmRexrReg(m, rde), xf, 4);
      break;
    default:
      unreachable;
  }
}

void OpRsqrtps(struct Machine *m, uint32_t rde) {
  float x[4];
  unsigned i;
  if (Rep(rde) != 3) {
    memcpy(x, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    for (i = 0; i < 4; ++i) x[i] = 1.f / sqrtf(x[i]);
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    memcpy(x, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    x[0] = 1.f / sqrtf(x[0]);
    memcpy(XmmRexrReg(m, rde), x, 4);
  }
}

void OpRcpps(struct Machine *m, uint32_t rde) {
  float x[4];
  unsigned i;
  if (Rep(rde) != 3) {
    memcpy(x, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    for (i = 0; i < 4; ++i) x[i] = 1.f / x[i];
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    memcpy(x, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    x[0] = 1.f / x[0];
    memcpy(XmmRexrReg(m, rde), x, 4);
  }
}

void OpComissVsWs(struct Machine *m, uint32_t rde) {
  float xf, yf;
  double xd, yd;
  uint8_t zf, cf, pf, ie;
  if (!Osz(rde)) {
    memcpy(&xf, XmmRexrReg(m, rde), 4);
    memcpy(&yf, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    if (!isnan(xf) && !isnan(yf)) {
      zf = xf == yf;
      cf = xf < yf;
      pf = false;
      ie = false;
    } else {
      zf = cf = pf = ie = true;
    }
  } else {
    memcpy(&xd, XmmRexrReg(m, rde), 8);
    memcpy(&yd, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    if (!isnan(xd) && !isnan(yd)) {
      zf = xd == yd;
      cf = xd < yd;
      pf = false;
      ie = false;
    } else {
      zf = cf = pf = ie = true;
    }
  }
  m->flags = SetFlag(m->flags, FLAGS_ZF, zf);
  m->flags = SetFlag(m->flags, FLAGS_PF, pf);
  m->flags = SetFlag(m->flags, FLAGS_CF, cf);
  m->flags = SetFlag(m->flags, FLAGS_SF, false);
  m->flags = SetFlag(m->flags, FLAGS_OF, false);
  if ((m->xedd->op.opcode & 1) && (m->sse.ie = ie) && !m->sse.im) {
    HaltMachine(m, kMachineSimdException);
  }
}

static int Cmps(int imm, float x, float y) {
  switch (imm) {
    case 0:
      return x == y ? -1 : 0;
    case 1:
      return x < y ? -1 : 0;
    case 2:
      return x <= y ? -1 : 0;
    case 3:
      return isnan(x) || isnan(y) ? -1 : 0;
    case 4:
      return x != y ? -1 : 0;
    case 5:
      return x >= y ? -1 : 0;
    case 6:
      return x > y ? -1 : 0;
    case 7:
      return !(isnan(x) || isnan(y)) ? -1 : 0;
    default:
      return 0;
  }
}

static int32_t Cmpd(int imm, double x, double y) {
  long i;
  switch (imm) {
    case 0:
      return x == y ? -1 : 0;
    case 1:
      return x < y ? -1 : 0;
    case 2:
      return x <= y ? -1 : 0;
    case 3:
      return isnan(x) || isnan(y) ? -1 : 0;
    case 4:
      return x != y ? -1 : 0;
    case 5:
      return x >= y ? -1 : 0;
    case 6:
      return x > y ? -1 : 0;
    case 7:
      return !(isnan(x) || isnan(y)) ? -1 : 0;
    default:
      return 0;
  }
}

void OpAddpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    double x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    memcpy(&x, XmmRexrReg(m, rde), 8);
    x += y;
    memcpy(XmmRexrReg(m, rde), &x, 8);
  } else if (Rep(rde) == 3) {
    float x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    memcpy(&x, XmmRexrReg(m, rde), 4);
    x += y;
    memcpy(XmmRexrReg(m, rde), &x, 4);
  } else if (Osz(rde)) {
    double x[2], y[2];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] += y[0];
    x[1] += y[1];
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    float x[4], y[4];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] += y[0];
    x[1] += y[1];
    x[2] += y[2];
    x[3] += y[3];
    memcpy(XmmRexrReg(m, rde), x, 16);
  }
}

void OpMulpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    double x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    memcpy(&x, XmmRexrReg(m, rde), 8);
    x *= y;
    memcpy(XmmRexrReg(m, rde), &x, 8);
  } else if (Rep(rde) == 3) {
    float x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    memcpy(&x, XmmRexrReg(m, rde), 4);
    x *= y;
    memcpy(XmmRexrReg(m, rde), &x, 4);
  } else if (Osz(rde)) {
    double x[2], y[2];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] *= y[0];
    x[1] *= y[1];
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    float x[4], y[4];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] *= y[0];
    x[1] *= y[1];
    x[2] *= y[2];
    x[3] *= y[3];
    memcpy(XmmRexrReg(m, rde), x, 16);
  }
}

void OpSubpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    double x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    memcpy(&x, XmmRexrReg(m, rde), 8);
    x -= y;
    memcpy(XmmRexrReg(m, rde), &x, 8);
  } else if (Rep(rde) == 3) {
    float x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    memcpy(&x, XmmRexrReg(m, rde), 4);
    x -= y;
    memcpy(XmmRexrReg(m, rde), &x, 4);
  } else if (Osz(rde)) {
    double x[2], y[2];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] -= y[0];
    x[1] -= y[1];
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    float x[4], y[4];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] -= y[0];
    x[1] -= y[1];
    x[2] -= y[2];
    x[3] -= y[3];
    memcpy(XmmRexrReg(m, rde), x, 16);
  }
}

void OpDivpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    double x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    memcpy(&x, XmmRexrReg(m, rde), 8);
    x /= y;
    memcpy(XmmRexrReg(m, rde), &x, 8);
  } else if (Rep(rde) == 3) {
    float x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    memcpy(&x, XmmRexrReg(m, rde), 4);
    x /= y;
    memcpy(XmmRexrReg(m, rde), &x, 4);
  } else if (Osz(rde)) {
    double x[2], y[2];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] /= y[0];
    x[1] /= y[1];
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    float x[4], y[4];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] /= y[0];
    x[1] /= y[1];
    x[2] /= y[2];
    x[3] /= y[3];
    memcpy(XmmRexrReg(m, rde), x, 16);
  }
}

void OpMinpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    double x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    memcpy(&x, XmmRexrReg(m, rde), 8);
    x = MIN(x, y);
    memcpy(XmmRexrReg(m, rde), &x, 8);
  } else if (Rep(rde) == 3) {
    float x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    memcpy(&x, XmmRexrReg(m, rde), 4);
    x = MIN(x, y);
    memcpy(XmmRexrReg(m, rde), &x, 4);
  } else if (Osz(rde)) {
    double x[2], y[2];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] = MIN(x[0], y[0]);
    x[1] = MIN(x[1], y[1]);
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    float x[4], y[4];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] = MIN(x[0], y[0]);
    x[1] = MIN(x[1], y[1]);
    x[2] = MIN(x[2], y[2]);
    x[3] = MIN(x[3], y[3]);
    memcpy(XmmRexrReg(m, rde), x, 16);
  }
}

void OpMaxpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    double x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    memcpy(&x, XmmRexrReg(m, rde), 8);
    x = MAX(x, y);
    memcpy(XmmRexrReg(m, rde), &x, 8);
  } else if (Rep(rde) == 3) {
    float x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    memcpy(&x, XmmRexrReg(m, rde), 4);
    x = MAX(x, y);
    memcpy(XmmRexrReg(m, rde), &x, 4);
  } else if (Osz(rde)) {
    double x[2], y[2];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] = MAX(x[0], y[0]);
    x[1] = MAX(x[1], y[1]);
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    float x[4], y[4];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] = MAX(x[0], y[0]);
    x[1] = MAX(x[1], y[1]);
    x[2] = MAX(x[2], y[2]);
    x[3] = MAX(x[3], y[3]);
    memcpy(XmmRexrReg(m, rde), x, 16);
  }
}

void OpCmppsd(struct Machine *m, uint32_t rde) {
  int imm = m->xedd->op.uimm0;
  if (Rep(rde) == 2) {
    double x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    memcpy(&x, XmmRexrReg(m, rde), 8);
    x = Cmpd(imm, x, y);
    memcpy(XmmRexrReg(m, rde), &x, 8);
  } else if (Rep(rde) == 3) {
    float x, y;
    memcpy(&y, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    memcpy(&x, XmmRexrReg(m, rde), 4);
    x = Cmps(imm, x, y);
    memcpy(XmmRexrReg(m, rde), &x, 4);
  } else if (Osz(rde)) {
    double x[2], y[2];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] = Cmpd(imm, x[0], y[0]);
    x[1] = Cmpd(imm, x[1], y[1]);
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else {
    float x[4], y[4];
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(x, XmmRexrReg(m, rde), 16);
    x[0] = Cmps(imm, x[0], y[0]);
    x[1] = Cmps(imm, x[1], y[1]);
    x[2] = Cmps(imm, x[2], y[2]);
    x[3] = Cmps(imm, x[3], y[3]);
    memcpy(XmmRexrReg(m, rde), x, 16);
  }
}

void OpAndpsd(struct Machine *m, uint32_t rde) {
  uint64_t x[2], y[2];
  memcpy(x, XmmRexrReg(m, rde), 16);
  memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  x[0] &= y[0];
  x[1] &= y[1];
  memcpy(XmmRexrReg(m, rde), x, 16);
}

void OpAndnpsd(struct Machine *m, uint32_t rde) {
  uint64_t x[2], y[2];
  memcpy(x, XmmRexrReg(m, rde), 16);
  memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  x[0] = ~x[0] & y[0];
  x[1] = ~x[1] & y[1];
  memcpy(XmmRexrReg(m, rde), x, 16);
}

void OpOrpsd(struct Machine *m, uint32_t rde) {
  uint64_t x[2], y[2];
  memcpy(x, XmmRexrReg(m, rde), 16);
  memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  x[0] |= y[0];
  x[1] |= y[1];
  memcpy(XmmRexrReg(m, rde), x, 16);
}

void OpXorpsd(struct Machine *m, uint32_t rde) {
  uint64_t x[2], y[2];
  memcpy(x, XmmRexrReg(m, rde), 16);
  memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  x[0] ^= y[0];
  x[1] ^= y[1];
  memcpy(XmmRexrReg(m, rde), x, 16);
}

void OpHaddpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    float x[4], y[4], z[4];
    memcpy(x, XmmRexrReg(m, rde), 16);
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    z[0] = x[0] + x[1];
    z[1] = x[2] + x[3];
    z[2] = y[0] + y[1];
    z[3] = y[2] + y[3];
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else if (Osz(rde)) {
    double x[2], y[2], z[2];
    memcpy(x, XmmRexrReg(m, rde), 16);
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    z[0] = x[0] + x[1];
    z[1] = y[0] + y[1];
    memcpy(XmmRexrReg(m, rde), z, 16);
  } else {
    OpUd(m, rde);
  }
}

void OpHsubpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    float x[4], y[4], z[4];
    memcpy(x, XmmRexrReg(m, rde), 16);
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    z[0] = x[0] - x[1];
    z[1] = x[2] - x[3];
    z[2] = y[0] - y[1];
    z[3] = y[2] - y[3];
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else if (Osz(rde)) {
    double x[2], y[2], z[2];
    memcpy(x, XmmRexrReg(m, rde), 16);
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    z[0] = x[0] - x[1];
    z[1] = y[0] - y[1];
    memcpy(XmmRexrReg(m, rde), z, 16);
  } else {
    OpUd(m, rde);
  }
}

void OpAddsubpsd(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 2) {
    float x[4], y[4], z[4];
    memcpy(x, XmmRexrReg(m, rde), 16);
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    z[0] = x[0] - y[0];
    z[1] = x[1] + y[1];
    z[2] = x[2] - y[2];
    z[3] = x[3] + y[3];
    memcpy(XmmRexrReg(m, rde), x, 16);
  } else if (Osz(rde)) {
    double x[2], y[2], z[2];
    memcpy(x, XmmRexrReg(m, rde), 16);
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    z[0] = x[0] - y[0];
    z[1] = x[1] + y[1];
    memcpy(XmmRexrReg(m, rde), z, 16);
  } else {
    OpUd(m, rde);
  }
}
