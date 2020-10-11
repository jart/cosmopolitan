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

#define SSE_BUILTINS               \
  (!IsModeDbg() && __SSE3__ + 0 && \
   (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 408)

typedef int int_v _Vector_size(16) aligned(16);
typedef long long_v _Vector_size(16) aligned(16);

static float_v Addps(struct Machine *m, float_v x, float_v y) {
  return x + y;
}

static double_v Addpd(struct Machine *m, double_v x, double_v y) {
  return x + y;
}

static float_v Mulps(struct Machine *m, float_v x, float_v y) {
  return x * y;
}

static double_v Mulpd(struct Machine *m, double_v x, double_v y) {
  return x * y;
}

static float_v Subps(struct Machine *m, float_v x, float_v y) {
  return x - y;
}

static double_v Subpd(struct Machine *m, double_v x, double_v y) {
  return x - y;
}

static float_v Divps(struct Machine *m, float_v x, float_v y) {
  return x / y;
}

static double_v Divpd(struct Machine *m, double_v x, double_v y) {
  return x / y;
}

static float_v Andps(struct Machine *m, float_v x, float_v y) {
  return (float_v)((int_v)x & (int_v)y);
}

static double_v Andpd(struct Machine *m, double_v x, double_v y) {
  return (double_v)((long_v)x & (long_v)y);
}

static float_v Andnps(struct Machine *m, float_v x, float_v y) {
  return (float_v)(~(int_v)x & (int_v)y);
}

static double_v Andnpd(struct Machine *m, double_v x, double_v y) {
  return (double_v)(~(long_v)x & (long_v)y);
}

static float_v Orps(struct Machine *m, float_v x, float_v y) {
  return (float_v)((int_v)x | (int_v)y);
}

static double_v Orpd(struct Machine *m, double_v x, double_v y) {
  return (double_v)((long_v)x | (long_v)y);
}

static float_v Xorps(struct Machine *m, float_v x, float_v y) {
  return (float_v)((int_v)x ^ (int_v)y);
}

static double_v Xorpd(struct Machine *m, double_v x, double_v y) {
  return (double_v)((long_v)x ^ (long_v)y);
}

static float_v Minps(struct Machine *m, float_v x, float_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_minps(x, y);
#else
  unsigned i;
  for (i = 0; i < 4; ++i) {
    x[i] = MIN(x[i], y[i]);
  }
  return x;
#endif
}

static double_v Minpd(struct Machine *m, double_v x, double_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_minpd(x, y);
#else
  unsigned i;
  for (i = 0; i < 2; ++i) {
    x[i] = MIN(x[i], y[i]);
  }
  return x;
#endif
}

static float_v Maxps(struct Machine *m, float_v x, float_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_maxps(x, y);
#else
  unsigned i;
  for (i = 0; i < 4; ++i) {
    x[i] = MAX(x[i], y[i]);
  }
  return x;
#endif
}

static double_v Maxpd(struct Machine *m, double_v x, double_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_maxpd(x, y);
#else
  unsigned i;
  for (i = 0; i < 2; ++i) {
    x[i] = MAX(x[i], y[i]);
  }
  return x;
#endif
}

static double_v Haddpd(struct Machine *m, double_v x, double_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_haddpd(x, y);
#else
  return (double_v){x[0] + x[1], y[0] + y[1]};
#endif
}

static float_v Haddps(struct Machine *m, float_v x, float_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_haddps(x, y);
#else
  return (float_v){x[0] + x[1], x[2] + x[3], y[0] + y[1], y[2] + y[3]};
#endif
}

static double_v Hsubpd(struct Machine *m, double_v x, double_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_hsubpd(x, y);
#else
  return (double_v){x[0] - x[1], y[0] - y[1]};
#endif
}

static float_v Hsubps(struct Machine *m, float_v x, float_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_hsubps(x, y);
#else
  return (float_v){x[0] - x[1], x[2] - x[3], y[0] - y[1], y[2] - y[3]};
#endif
}

static double_v Addsubpd(struct Machine *m, double_v x, double_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_addsubpd(x, y);
#else
  return (double_v){x[0] - y[0], x[1] + y[1]};
#endif
}

static float_v Addsubps(struct Machine *m, float_v x, float_v y) {
#if SSE_BUILTINS
  return __builtin_ia32_addsubps(x, y);
#else
  return (float_v){x[0] - y[0], x[1] + y[1], x[2] - y[2], x[3] + y[3]};
#endif
}

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
  float_v xf;
  double_v xd;
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      memcpy(&xf, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      for (i = 0; i < 4; ++i) xf[i] = sqrtf(xf[i]);
      memcpy(XmmRexrReg(m, rde), &xf, 16);
      break;
    case 1:
      memcpy(&xd, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      for (i = 0; i < 2; ++i) xd[i] = sqrt(xd[i]);
      memcpy(XmmRexrReg(m, rde), &xd, 16);
      break;
    case 2:
      memcpy(&xd, GetModrmRegisterXmmPointerRead8(m, rde), 8);
      xd[0] = sqrt(xd[0]);
      memcpy(XmmRexrReg(m, rde), &xd, 8);
      break;
    case 3:
      memcpy(&xf, GetModrmRegisterXmmPointerRead4(m, rde), 4);
      xf[0] = sqrtf(xf[0]);
      memcpy(XmmRexrReg(m, rde), &xf, 4);
      break;
    default:
      unreachable;
  }
}

void OpRsqrtps(struct Machine *m, uint32_t rde) {
  float_v x;
  unsigned i;
  if (Rep(rde) != 3) {
    memcpy(&x, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    for (i = 0; i < 4; ++i) x[i] = 1.f / sqrtf(x[i]);
    memcpy(XmmRexrReg(m, rde), &x, 16);
  } else {
    memcpy(&x, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    x[0] = 1.f / sqrtf(x[0]);
    memcpy(XmmRexrReg(m, rde), &x, 4);
  }
}

void OpRcpps(struct Machine *m, uint32_t rde) {
  float_v x;
  unsigned i;
  if (Rep(rde) != 3) {
    memcpy(&x, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    for (i = 0; i < 4; ++i) x[i] = 1.f / x[i];
    memcpy(XmmRexrReg(m, rde), &x, 16);
  } else {
    memcpy(&x, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    x[0] = 1.f / x[0];
    memcpy(XmmRexrReg(m, rde), &x, 4);
  }
}

static void VpsdWpsd(struct Machine *m, uint32_t rde,
                     float_v opf(struct Machine *, float_v, float_v),
                     double_v opd(struct Machine *, double_v, double_v),
                     bool isfloat, bool isdouble) {
  float_v xf, yf;
  double_v xd, yd;
  if (isfloat) {
    memcpy(&xf, XmmRexrReg(m, rde), 16);
    memcpy(&yf, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    xf = opf(m, xf, yf);
    memcpy(XmmRexrReg(m, rde), &xf, 16);
  } else if (isdouble) {
    memcpy(&xd, XmmRexrReg(m, rde), 16);
    memcpy(&yd, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    xd = opd(m, xd, yd);
    memcpy(XmmRexrReg(m, rde), &xd, 16);
  } else {
    OpUd(m, rde);
  }
}

static void VpsdWpsd66(struct Machine *m, uint32_t rde,
                       float_v opf(struct Machine *, float_v, float_v),
                       double_v opd(struct Machine *, double_v, double_v)) {
  VpsdWpsd(m, rde, opf, opd, !Osz(rde), Osz(rde));
}

static void VpsdWpsd66f2(struct Machine *m, uint32_t rde,
                         float_v opf(struct Machine *, float_v, float_v),
                         double_v opd(struct Machine *, double_v, double_v)) {
  VpsdWpsd(m, rde, opf, opd, Rep(rde) == 2, Osz(rde));
}

static void VspsdWspsd(struct Machine *m, uint32_t rde,
                       float_v opf(struct Machine *, float_v, float_v),
                       double_v opd(struct Machine *, double_v, double_v)) {
  float_v xf, yf;
  double_v xd, yd;
  if (Rep(rde) == 2) {
    memcpy(&yd, GetModrmRegisterXmmPointerRead8(m, rde), 8);
    memcpy(&xd, XmmRexrReg(m, rde), 8);
    xd = opd(m, xd, yd);
    memcpy(XmmRexrReg(m, rde), &xd, 8);
  } else if (Rep(rde) == 3) {
    memcpy(&yf, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    memcpy(&xf, XmmRexrReg(m, rde), 4);
    xf = opf(m, xf, yf);
    memcpy(XmmRexrReg(m, rde), &xf, 4);
  } else if (Osz(rde)) {
    memcpy(&yd, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(&xd, XmmRexrReg(m, rde), 16);
    xd = opd(m, xd, yd);
    memcpy(XmmRexrReg(m, rde), &xd, 16);
  } else {
    memcpy(&yf, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    memcpy(&xf, XmmRexrReg(m, rde), 16);
    xf = opf(m, xf, yf);
    memcpy(XmmRexrReg(m, rde), &xf, 16);
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

static float_v Cmpps(struct Machine *m, float_v x, float_v y) {
  long i;
  switch (m->xedd->op.uimm0) {
    case 0:
      return x == y;
    case 1:
      return x < y;
    case 2:
      return x <= y;
    case 3:
      for (i = 0; i < 4; ++i) {
        x[i] = isnan(x[i]) || isnan(y[i]);
      }
      return x;
    case 4:
      return x != y;
    case 5:
      return x >= y;
    case 6:
      return x > y;
    case 7:
      for (i = 0; i < 4; ++i) {
        x[i] = !(isnan(x[i]) || isnan(y[i]));
      }
      return x;
    default:
      OpUd(m, 0);
  }
}

static double_v Cmppd(struct Machine *m, double_v x, double_v y) {
  long i;
  switch (m->xedd->op.uimm0) {
    case 0:
      return x == y;
    case 1:
      return x < y;
    case 2:
      return x <= y;
    case 3:
      for (i = 0; i < 2; ++i) {
        x[i] = isnan(x[i]) || isnan(y[i]);
      }
      return x;
    case 4:
      return x != y;
    case 5:
      return x >= y;
    case 6:
      return x > y;
    case 7:
      for (i = 0; i < 2; ++i) {
        x[i] = !(isnan(x[i]) || isnan(y[i]));
      }
      return x;
    default:
      OpUd(m, 0);
  }
}

void OpAddpsd(struct Machine *m, uint32_t rde) {
  VspsdWspsd(m, rde, Addps, Addpd);
}

void OpMulpsd(struct Machine *m, uint32_t rde) {
  VspsdWspsd(m, rde, Mulps, Mulpd);
}

void OpSubpsd(struct Machine *m, uint32_t rde) {
  VspsdWspsd(m, rde, Subps, Subpd);
}

void OpDivpsd(struct Machine *m, uint32_t rde) {
  VspsdWspsd(m, rde, Divps, Divpd);
}

void OpMinpsd(struct Machine *m, uint32_t rde) {
  VspsdWspsd(m, rde, Minps, Minpd);
}

void OpMaxpsd(struct Machine *m, uint32_t rde) {
  VspsdWspsd(m, rde, Maxps, Maxpd);
}

void OpCmppsd(struct Machine *m, uint32_t rde) {
  VspsdWspsd(m, rde, Cmpps, Cmppd);
}

void OpAndpsd(struct Machine *m, uint32_t rde) {
  VpsdWpsd66(m, rde, Andps, Andpd);
}

void OpAndnpsd(struct Machine *m, uint32_t rde) {
  VpsdWpsd66(m, rde, Andnps, Andnpd);
}

void OpOrpsd(struct Machine *m, uint32_t rde) {
  VpsdWpsd66(m, rde, Orps, Orpd);
}

void OpXorpsd(struct Machine *m, uint32_t rde) {
  VpsdWpsd66(m, rde, Xorps, Xorpd);
}

void OpHaddpsd(struct Machine *m, uint32_t rde) {
  VpsdWpsd66f2(m, rde, Haddps, Haddpd);
}

void OpHsubpsd(struct Machine *m, uint32_t rde) {
  VpsdWpsd66f2(m, rde, Hsubps, Hsubpd);
}

void OpAddsubpsd(struct Machine *m, uint32_t rde) {
  VpsdWpsd66f2(m, rde, Addsubps, Addsubpd);
}
