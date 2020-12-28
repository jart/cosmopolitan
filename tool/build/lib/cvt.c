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
#include "libc/macros.h"
#include "libc/math.h"
#include "tool/build/lib/cvt.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/throw.h"

#define kOpCvt0f2a  0
#define kOpCvtt0f2c 4
#define kOpCvt0f2d  8
#define kOpCvt0f5a  12
#define kOpCvt0f5b  16
#define kOpCvt0fE6  20

static double SseRoundDouble(struct Machine *m, double x) {
  switch (m->sse.rc) {
    case 0:
      return rint(x);
    case 1:
      return floor(x);
    case 2:
      return ceil(x);
    case 3:
      return trunc(x);
    default:
      unreachable;
  }
}

static void OpGdqpWssCvttss2si(struct Machine *m, uint32_t rde) {
  float f;
  int64_t n;
  memcpy(&f, GetModrmRegisterXmmPointerRead4(m, rde), 4);
  n = f;
  if (!Rexw(rde)) n &= 0xffffffff;
  Write64(RegRexrReg(m, rde), n);
}

static void OpGdqpWsdCvttsd2si(struct Machine *m, uint32_t rde) {
  double d;
  int64_t n;
  memcpy(&d, GetModrmRegisterXmmPointerRead8(m, rde), 8);
  n = d;
  if (!Rexw(rde)) n &= 0xffffffff;
  Write64(RegRexrReg(m, rde), n);
}

static void OpGdqpWssCvtss2si(struct Machine *m, uint32_t rde) {
  float f;
  int64_t n;
  memcpy(&f, GetModrmRegisterXmmPointerRead4(m, rde), 4);
  n = rintf(f);
  if (!Rexw(rde)) n &= 0xffffffff;
  Write64(RegRexrReg(m, rde), n);
}

static void OpGdqpWsdCvtsd2si(struct Machine *m, uint32_t rde) {
  double d;
  int64_t n;
  memcpy(&d, GetModrmRegisterXmmPointerRead8(m, rde), 8);
  n = SseRoundDouble(m, d);
  if (!Rexw(rde)) n &= 0xffffffff;
  Write64(RegRexrReg(m, rde), n);
}

static void OpVssEdqpCvtsi2ss(struct Machine *m, uint32_t rde) {
  float f;
  int64_t n;
  uint8_t *p;
  if (Rexw(rde)) {
    n = (int64_t)Read64(GetModrmRegisterWordPointerRead8(m, rde));
  } else {
    n = (int32_t)Read32(GetModrmRegisterWordPointerRead4(m, rde));
  }
  f = n;
  memcpy(XmmRexrReg(m, rde), &f, 4);
}

static void OpVsdEdqpCvtsi2sd(struct Machine *m, uint32_t rde) {
  double d;
  int64_t n;
  uint8_t *p;
  if (Rexw(rde)) {
    n = (int64_t)Read64(GetModrmRegisterWordPointerRead8(m, rde));
  } else {
    n = (int32_t)Read32(GetModrmRegisterWordPointerRead4(m, rde));
  }
  d = n;
  memcpy(XmmRexrReg(m, rde), &d, 8);
}

static void OpVpsQpiCvtpi2ps(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  float f[2];
  int32_t i[2];
  p = GetModrmRegisterMmPointerRead8(m, rde);
  i[0] = Read32(p + 0);
  i[1] = Read32(p + 4);
  f[0] = i[0];
  f[1] = i[1];
  memcpy(XmmRexrReg(m, rde), f, 8);
}

static void OpVpdQpiCvtpi2pd(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  double f[2];
  int32_t n[2];
  p = GetModrmRegisterMmPointerRead8(m, rde);
  n[0] = Read32(p + 0);
  n[1] = Read32(p + 4);
  f[0] = n[0];
  f[1] = n[1];
  memcpy(XmmRexrReg(m, rde), f, 16);
}

static void OpPpiWpsqCvtps2pi(struct Machine *m, uint32_t rde) {
  unsigned i;
  float f[2];
  int32_t n[2];
  memcpy(f, GetModrmRegisterXmmPointerRead8(m, rde), 8);
  switch (m->sse.rc) {
    case 0:
      for (i = 0; i < 2; ++i) n[i] = rintf(f[i]);
      break;
    case 1:
      for (i = 0; i < 2; ++i) n[i] = floorf(f[i]);
      break;
    case 2:
      for (i = 0; i < 2; ++i) n[i] = ceilf(f[i]);
      break;
    case 3:
      for (i = 0; i < 2; ++i) n[i] = truncf(f[i]);
      break;
    default:
      unreachable;
  }
  Write32(MmReg(m, rde) + 0, n[0]);
  Write32(MmReg(m, rde) + 4, n[1]);
}

static void OpPpiWpsqCvttps2pi(struct Machine *m, uint32_t rde) {
  float f[2];
  int32_t n[2];
  memcpy(&f, GetModrmRegisterXmmPointerRead8(m, rde), 8);
  n[0] = f[0];
  n[1] = f[1];
  Write32(MmReg(m, rde) + 0, n[0]);
  Write32(MmReg(m, rde) + 4, n[1]);
}

static void OpPpiWpdCvtpd2pi(struct Machine *m, uint32_t rde) {
  unsigned i;
  double d[2];
  int32_t n[2];
  memcpy(d, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  for (i = 0; i < 2; ++i) n[i] = SseRoundDouble(m, d[i]);
  Write32(MmReg(m, rde) + 0, n[0]);
  Write32(MmReg(m, rde) + 4, n[1]);
}

static void OpPpiWpdCvttpd2pi(struct Machine *m, uint32_t rde) {
  double d[2];
  int32_t n[2];
  memcpy(&d, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  n[0] = d[0];
  n[1] = d[1];
  Write32(MmReg(m, rde) + 0, n[0]);
  Write32(MmReg(m, rde) + 4, n[1]);
}

static void OpVpdWpsCvtps2pd(struct Machine *m, uint32_t rde) {
  float f[2];
  double d[2];
  memcpy(f, GetModrmRegisterXmmPointerRead8(m, rde), 8);
  d[0] = f[0];
  d[1] = f[1];
  memcpy(XmmRexrReg(m, rde), d, 16);
}

static void OpVpsWpdCvtpd2ps(struct Machine *m, uint32_t rde) {
  float f[2];
  double d[2];
  memcpy(d, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  f[0] = d[0];
  f[1] = d[1];
  memcpy(XmmRexrReg(m, rde), f, 8);
}

static void OpVssWsdCvtsd2ss(struct Machine *m, uint32_t rde) {
  float f;
  double d;
  memcpy(&d, GetModrmRegisterXmmPointerRead8(m, rde), 8);
  f = d;
  memcpy(XmmRexrReg(m, rde), &f, 4);
}

static void OpVsdWssCvtss2sd(struct Machine *m, uint32_t rde) {
  float f;
  double d;
  memcpy(&f, GetModrmRegisterXmmPointerRead4(m, rde), 4);
  d = f;
  memcpy(XmmRexrReg(m, rde), &d, 8);
}

static void OpVpsWdqCvtdq2ps(struct Machine *m, uint32_t rde) {
  unsigned i;
  float f[4];
  int32_t n[4];
  memcpy(n, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  for (i = 0; i < 4; ++i) f[i] = n[i];
  memcpy(XmmRexrReg(m, rde), f, 16);
}

static void OpVpdWdqCvtdq2pd(struct Machine *m, uint32_t rde) {
  unsigned i;
  double d[2];
  int32_t n[2];
  memcpy(n, GetModrmRegisterXmmPointerRead8(m, rde), 8);
  for (i = 0; i < 2; ++i) d[i] = n[i];
  memcpy(XmmRexrReg(m, rde), d, 16);
}

static void OpVdqWpsCvttps2dq(struct Machine *m, uint32_t rde) {
  unsigned i;
  float f[4];
  int32_t n[4];
  memcpy(f, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  for (i = 0; i < 4; ++i) n[i] = f[i];
  memcpy(XmmRexrReg(m, rde), n, 16);
}

static void OpVdqWpsCvtps2dq(struct Machine *m, uint32_t rde) {
  unsigned i;
  float f[4];
  int32_t n[4];
  memcpy(f, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  switch (m->sse.rc) {
    case 0:
      for (i = 0; i < 4; ++i) n[i] = rintf(f[i]);
      break;
    case 1:
      for (i = 0; i < 4; ++i) n[i] = floorf(f[i]);
      break;
    case 2:
      for (i = 0; i < 4; ++i) n[i] = ceilf(f[i]);
      break;
    case 3:
      for (i = 0; i < 4; ++i) n[i] = truncf(f[i]);
      break;
    default:
      unreachable;
  }
  memcpy(XmmRexrReg(m, rde), n, 16);
}

static void OpVdqWpdCvttpd2dq(struct Machine *m, uint32_t rde) {
  unsigned i;
  double d[2];
  int32_t n[2];
  memcpy(d, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  for (i = 0; i < 2; ++i) n[i] = d[i];
  memcpy(XmmRexrReg(m, rde), n, 8);
}

static void OpVdqWpdCvtpd2dq(struct Machine *m, uint32_t rde) {
  unsigned i;
  double d[2];
  int32_t n[2];
  memcpy(d, GetModrmRegisterXmmPointerRead16(m, rde), 16);
  for (i = 0; i < 2; ++i) n[i] = SseRoundDouble(m, d[i]);
  memcpy(XmmRexrReg(m, rde), n, 8);
}

static void OpCvt(struct Machine *m, uint32_t rde, unsigned long op) {
  switch (op | Rep(rde) | Osz(rde)) {
    case kOpCvt0f2a + 0:
      OpVpsQpiCvtpi2ps(m, rde);
      break;
    case kOpCvt0f2a + 1:
      OpVpdQpiCvtpi2pd(m, rde);
      break;
    case kOpCvt0f2a + 2:
      OpVsdEdqpCvtsi2sd(m, rde);
      break;
    case kOpCvt0f2a + 3:
      OpVssEdqpCvtsi2ss(m, rde);
      break;
    case kOpCvtt0f2c + 0:
      OpPpiWpsqCvttps2pi(m, rde);
      break;
    case kOpCvtt0f2c + 1:
      OpPpiWpdCvttpd2pi(m, rde);
      break;
    case kOpCvtt0f2c + 2:
      OpGdqpWsdCvttsd2si(m, rde);
      break;
    case kOpCvtt0f2c + 3:
      OpGdqpWssCvttss2si(m, rde);
      break;
    case kOpCvt0f2d + 0:
      OpPpiWpsqCvtps2pi(m, rde);
      break;
    case kOpCvt0f2d + 1:
      OpPpiWpdCvtpd2pi(m, rde);
      break;
    case kOpCvt0f2d + 2:
      OpGdqpWsdCvtsd2si(m, rde);
      break;
    case kOpCvt0f2d + 3:
      OpGdqpWssCvtss2si(m, rde);
      break;
    case kOpCvt0f5a + 0:
      OpVpdWpsCvtps2pd(m, rde);
      break;
    case kOpCvt0f5a + 1:
      OpVpsWpdCvtpd2ps(m, rde);
      break;
    case kOpCvt0f5a + 2:
      OpVssWsdCvtsd2ss(m, rde);
      break;
    case kOpCvt0f5a + 3:
      OpVsdWssCvtss2sd(m, rde);
      break;
    case kOpCvt0f5b + 0:
      OpVpsWdqCvtdq2ps(m, rde);
      break;
    case kOpCvt0f5b + 1:
      OpVdqWpsCvtps2dq(m, rde);
      break;
    case kOpCvt0f5b + 3:
      OpVdqWpsCvttps2dq(m, rde);
      break;
    case kOpCvt0fE6 + 1:
      OpVdqWpdCvtpd2dq(m, rde);
      break;
    case kOpCvt0fE6 + 2:
      OpVdqWpdCvttpd2dq(m, rde);
      break;
    case kOpCvt0fE6 + 3:
      OpVpdWdqCvtdq2pd(m, rde);
      break;
    default:
      OpUd(m, rde);
  }
}

void OpCvt0f2a(struct Machine *m, uint32_t rde) {
  OpCvt(m, rde, kOpCvt0f2a);
}

void OpCvtt0f2c(struct Machine *m, uint32_t rde) {
  OpCvt(m, rde, kOpCvtt0f2c);
}

void OpCvt0f2d(struct Machine *m, uint32_t rde) {
  OpCvt(m, rde, kOpCvt0f2d);
}

void OpCvt0f5a(struct Machine *m, uint32_t rde) {
  OpCvt(m, rde, kOpCvt0f5a);
}

void OpCvt0f5b(struct Machine *m, uint32_t rde) {
  OpCvt(m, rde, kOpCvt0f5b);
}

void OpCvt0fE6(struct Machine *m, uint32_t rde) {
  OpCvt(m, rde, kOpCvt0fE6);
}
