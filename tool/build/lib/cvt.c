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
#include "libc/macros.h"
#include "libc/math.h"
#include "tool/build/lib/cvt.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/throw.h"

static void OpGdqpWssCvttss2si(struct Machine *m) {
  float f;
  int64_t n;
  memcpy(&f, GetModrmRegisterXmmPointerRead4(m), 4);
  n = f;
  if (!Rexw(m->xedd)) n &= 0xffffffff;
  Write64(RegRexrReg(m), n);
}

static void OpGdqpWsdCvttsd2si(struct Machine *m) {
  double d;
  int64_t n;
  memcpy(&d, GetModrmRegisterXmmPointerRead8(m), 8);
  n = d;
  if (!Rexw(m->xedd)) n &= 0xffffffff;
  Write64(RegRexrReg(m), n);
}

static void OpGdqpWssCvtss2si(struct Machine *m) {
  float f;
  int64_t n;
  memcpy(&f, GetModrmRegisterXmmPointerRead4(m), 4);
  n = rintf(f);
  if (!Rexw(m->xedd)) n &= 0xffffffff;
  Write64(RegRexrReg(m), n);
}

static void OpGdqpWsdCvtsd2si(struct Machine *m) {
  double d;
  int64_t n;
  memcpy(&d, GetModrmRegisterXmmPointerRead8(m), 8);
  n = nearbyint(d);
  if (!Rexw(m->xedd)) n &= 0xffffffff;
  Write64(RegRexrReg(m), n);
}

static void OpVssEdqpCvtsi2ss(struct Machine *m) {
  float f;
  int64_t n;
  uint8_t *p;
  if (Rexw(m->xedd)) {
    n = (int64_t)Read64(GetModrmRegisterWordPointerRead8(m));
  } else {
    n = (int32_t)Read32(GetModrmRegisterWordPointerRead4(m));
  }
  f = n;
  memcpy(XmmRexrReg(m), &f, 4);
}

static void OpVsdEdqpCvtsi2sd(struct Machine *m) {
  double d;
  int64_t n;
  uint8_t *p;
  if (Rexw(m->xedd)) {
    n = (int64_t)Read64(GetModrmRegisterWordPointerRead8(m));
  } else {
    n = (int32_t)Read32(GetModrmRegisterWordPointerRead4(m));
  }
  d = n;
  memcpy(XmmRexrReg(m), &d, 8);
}

static void OpVpsQpiCvtpi2ps(struct Machine *m) {
  uint8_t *p;
  float f[2];
  int32_t i[2];
  p = GetModrmRegisterMmPointerRead8(m);
  i[0] = Read32(p + 0);
  i[1] = Read32(p + 4);
  f[0] = i[0];
  f[1] = i[1];
  memcpy(XmmRexrReg(m), f, 8);
}

static void OpVpdQpiCvtpi2pd(struct Machine *m) {
  uint8_t *p;
  double f[2];
  int32_t n[2];
  p = GetModrmRegisterMmPointerRead8(m);
  n[0] = Read32(p + 0);
  n[1] = Read32(p + 4);
  f[0] = n[0];
  f[1] = n[1];
  memcpy(XmmRexrReg(m), f, 16);
}

static void OpPpiWpsqCvtps2pi(struct Machine *m) {
  float f[2];
  int32_t n[2];
  memcpy(f, GetModrmRegisterXmmPointerRead8(m), 8);
  n[0] = nearbyintf(f[0]);
  n[1] = nearbyintf(f[1]);
  Write32(MmReg(m) + 0, n[0]);
  Write32(MmReg(m) + 4, n[1]);
}

static void OpPpiWpsqCvttps2pi(struct Machine *m) {
  float f[2];
  int32_t n[2];
  memcpy(&f, GetModrmRegisterXmmPointerRead8(m), 8);
  n[0] = f[0];
  n[1] = f[1];
  Write32(MmReg(m) + 0, n[0]);
  Write32(MmReg(m) + 4, n[1]);
}

static void OpPpiWpdCvtpd2pi(struct Machine *m) {
  double d[2];
  int32_t n[2];
  memcpy(d, GetModrmRegisterXmmPointerRead16(m), 16);
  n[0] = nearbyint(d[0]);
  n[1] = nearbyint(d[1]);
  Write32(MmReg(m) + 0, n[0]);
  Write32(MmReg(m) + 4, n[1]);
}

static void OpPpiWpdCvttpd2pi(struct Machine *m) {
  double d[2];
  int32_t n[2];
  memcpy(&d, GetModrmRegisterXmmPointerRead16(m), 16);
  n[0] = d[0];
  n[1] = d[1];
  Write32(MmReg(m) + 0, n[0]);
  Write32(MmReg(m) + 4, n[1]);
}

static void OpVpdWpsCvtps2pd(struct Machine *m) {
  float f[2];
  double d[2];
  memcpy(f, GetModrmRegisterXmmPointerRead8(m), 8);
  d[0] = f[0];
  d[1] = f[1];
  memcpy(XmmRexrReg(m), d, 16);
}

static void OpVpsWpdCvtpd2ps(struct Machine *m) {
  float f[2];
  double d[2];
  memcpy(d, GetModrmRegisterXmmPointerRead16(m), 16);
  f[0] = d[0];
  f[1] = d[1];
  memcpy(XmmRexrReg(m), f, 8);
}

static void OpVssWsdCvtsd2ss(struct Machine *m) {
  float f;
  double d;
  memcpy(&d, GetModrmRegisterXmmPointerRead8(m), 8);
  f = d;
  memcpy(XmmRexrReg(m), &f, 4);
}

static void OpVsdWssCvtss2sd(struct Machine *m) {
  float f;
  double d;
  memcpy(&f, GetModrmRegisterXmmPointerRead4(m), 4);
  d = f;
  memcpy(XmmRexrReg(m), &d, 8);
}

static void OpVpsWdqCvtdq2ps(struct Machine *m) {
  unsigned i;
  float f[4];
  int32_t n[4];
  memcpy(n, GetModrmRegisterXmmPointerRead16(m), 16);
  for (i = 0; i < 4; ++i) f[i] = n[i];
  memcpy(XmmRexrReg(m), f, 16);
}

static void OpVpdWdqCvtdq2pd(struct Machine *m) {
  unsigned i;
  double d[2];
  int32_t n[2];
  memcpy(n, GetModrmRegisterXmmPointerRead8(m), 8);
  for (i = 0; i < 2; ++i) d[i] = n[i];
  memcpy(XmmRexrReg(m), d, 16);
}

static void OpVdqWpsCvttps2dq(struct Machine *m) {
  unsigned i;
  float f[4];
  int32_t n[4];
  memcpy(f, GetModrmRegisterXmmPointerRead16(m), 16);
  for (i = 0; i < 4; ++i) n[i] = f[i];
  memcpy(XmmRexrReg(m), n, 16);
}

static void OpVdqWpsCvtps2dq(struct Machine *m) {
  unsigned i;
  float f[4];
  int32_t n[4];
  memcpy(f, GetModrmRegisterXmmPointerRead16(m), 16);
  for (i = 0; i < 4; ++i) n[i] = nearbyintf(f[i]);
  memcpy(XmmRexrReg(m), n, 16);
}

static void OpVdqWpdCvttpd2dq(struct Machine *m) {
  unsigned i;
  double d[2];
  int32_t n[2];
  memcpy(d, GetModrmRegisterXmmPointerRead16(m), 16);
  for (i = 0; i < 2; ++i) n[i] = d[i];
  memcpy(XmmRexrReg(m), n, 8);
}

static void OpVdqWpdCvtpd2dq(struct Machine *m) {
  unsigned i;
  double d[2];
  int32_t n[2];
  memcpy(d, GetModrmRegisterXmmPointerRead16(m), 16);
  for (i = 0; i < 2; ++i) n[i] = nearbyintf(d[i]);
  memcpy(XmmRexrReg(m), n, 8);
}

void OpCvt(struct Machine *m, unsigned long op) {
  switch (op | Rep(m->xedd) | Osz(m->xedd)) {
    case kOpCvt0f2a + 0:
      OpVpsQpiCvtpi2ps(m);
      break;
    case kOpCvt0f2a + 1:
      OpVpdQpiCvtpi2pd(m);
      break;
    case kOpCvt0f2a + 2:
      OpVsdEdqpCvtsi2sd(m);
      break;
    case kOpCvt0f2a + 3:
      OpVssEdqpCvtsi2ss(m);
      break;
    case kOpCvtt0f2c + 0:
      OpPpiWpsqCvttps2pi(m);
      break;
    case kOpCvtt0f2c + 1:
      OpPpiWpdCvttpd2pi(m);
      break;
    case kOpCvtt0f2c + 2:
      OpGdqpWsdCvttsd2si(m);
      break;
    case kOpCvtt0f2c + 3:
      OpGdqpWssCvttss2si(m);
      break;
    case kOpCvt0f2d + 0:
      OpPpiWpsqCvtps2pi(m);
      break;
    case kOpCvt0f2d + 1:
      OpPpiWpdCvtpd2pi(m);
      break;
    case kOpCvt0f2d + 2:
      OpGdqpWsdCvtsd2si(m);
      break;
    case kOpCvt0f2d + 3:
      OpGdqpWssCvtss2si(m);
      break;
    case kOpCvt0f5a + 0:
      OpVpdWpsCvtps2pd(m);
      break;
    case kOpCvt0f5a + 1:
      OpVpsWpdCvtpd2ps(m);
      break;
    case kOpCvt0f5a + 2:
      OpVssWsdCvtsd2ss(m);
      break;
    case kOpCvt0f5a + 3:
      OpVsdWssCvtss2sd(m);
      break;
    case kOpCvt0f5b + 0:
      OpVpsWdqCvtdq2ps(m);
      break;
    case kOpCvt0f5b + 1:
      OpVdqWpsCvtps2dq(m);
      break;
    case kOpCvt0f5b + 3:
      OpVdqWpsCvttps2dq(m);
      break;
    case kOpCvt0fE6 + 1:
      OpVdqWpdCvtpd2dq(m);
      break;
    case kOpCvt0fE6 + 2:
      OpVdqWpdCvttpd2dq(m);
      break;
    case kOpCvt0fE6 + 3:
      OpVpdWdqCvtdq2pd(m);
      break;
    default:
      OpUd(m);
  }
}
