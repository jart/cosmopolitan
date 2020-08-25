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
#include "libc/intrin/pabsb.h"
#include "libc/intrin/pabsd.h"
#include "libc/intrin/pabsw.h"
#include "libc/intrin/packssdw.h"
#include "libc/intrin/packsswb.h"
#include "libc/intrin/packuswb.h"
#include "libc/intrin/paddb.h"
#include "libc/intrin/paddd.h"
#include "libc/intrin/paddq.h"
#include "libc/intrin/paddsb.h"
#include "libc/intrin/paddsw.h"
#include "libc/intrin/paddusb.h"
#include "libc/intrin/paddusw.h"
#include "libc/intrin/paddw.h"
#include "libc/intrin/palignr.h"
#include "libc/intrin/pand.h"
#include "libc/intrin/pandn.h"
#include "libc/intrin/pavgb.h"
#include "libc/intrin/pavgw.h"
#include "libc/intrin/pcmpeqb.h"
#include "libc/intrin/pcmpeqd.h"
#include "libc/intrin/pcmpeqw.h"
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pcmpgtd.h"
#include "libc/intrin/pcmpgtw.h"
#include "libc/intrin/phaddd.h"
#include "libc/intrin/phaddsw.h"
#include "libc/intrin/phaddw.h"
#include "libc/intrin/phsubd.h"
#include "libc/intrin/phsubsw.h"
#include "libc/intrin/phsubw.h"
#include "libc/intrin/pmaddubsw.h"
#include "libc/intrin/pmaddwd.h"
#include "libc/intrin/pmaxsw.h"
#include "libc/intrin/pmaxub.h"
#include "libc/intrin/pminsw.h"
#include "libc/intrin/pminub.h"
#include "libc/intrin/pmulhrsw.h"
#include "libc/intrin/pmulhuw.h"
#include "libc/intrin/pmulhw.h"
#include "libc/intrin/pmulld.h"
#include "libc/intrin/pmullw.h"
#include "libc/intrin/pmuludq.h"
#include "libc/intrin/por.h"
#include "libc/intrin/psadbw.h"
#include "libc/intrin/pshufb.h"
#include "libc/intrin/psignb.h"
#include "libc/intrin/psignd.h"
#include "libc/intrin/psignw.h"
#include "libc/intrin/pslld.h"
#include "libc/intrin/pslldq.h"
#include "libc/intrin/psllq.h"
#include "libc/intrin/psllw.h"
#include "libc/intrin/psrad.h"
#include "libc/intrin/psraw.h"
#include "libc/intrin/psrld.h"
#include "libc/intrin/psrldq.h"
#include "libc/intrin/psrlq.h"
#include "libc/intrin/psrlw.h"
#include "libc/intrin/psubb.h"
#include "libc/intrin/psubd.h"
#include "libc/intrin/psubq.h"
#include "libc/intrin/psubsb.h"
#include "libc/intrin/psubsw.h"
#include "libc/intrin/psubusb.h"
#include "libc/intrin/psubusw.h"
#include "libc/intrin/psubw.h"
#include "libc/intrin/punpckhbw.h"
#include "libc/intrin/punpckhdq.h"
#include "libc/intrin/punpckhqdq.h"
#include "libc/intrin/punpckhwd.h"
#include "libc/intrin/punpcklbw.h"
#include "libc/intrin/punpckldq.h"
#include "libc/intrin/punpcklqdq.h"
#include "libc/intrin/punpcklwd.h"
#include "libc/intrin/pxor.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/sse.h"

union MachineVector {
  float f32[4];
  double f64[2];
  int8_t i8[16];
  int16_t i16[8];
  int32_t i32[4];
  int64_t i64[2];
  uint8_t u8[16];
  uint16_t u16[8];
  uint32_t u32[4];
  uint64_t u64[2];
};

void OpSse(struct Machine *m, enum OpSseKernel kernel) {
  int i;
  uint8_t *p;
  union MachineVector x, y, t;
  p = GetModrmRegisterXmmPointerRead16(m);
  if (Prefix66(m->xedd)) {
    memcpy(&y, p, 16);
  } else {
    memset(&t, 0, 16);
    memcpy(&t, p, 8);
    memcpy(&y, &t, 16);
  }
  memcpy(&x, XmmRexrReg(m), 16);
  switch (kernel) {
    CASE(kOpSsePsubb, psubb(x.i8, x.i8, y.i8));
    CASE(kOpSsePaddb, paddb(x.i8, x.i8, y.i8));
    CASE(kOpSsePsubw, psubw(x.i16, x.i16, y.i16));
    CASE(kOpSsePaddw, paddw(x.i16, x.i16, y.i16));
    CASE(kOpSsePsubd, psubd(x.i32, x.i32, y.i32));
    CASE(kOpSsePaddd, paddd(x.i32, x.i32, y.i32));
    CASE(kOpSsePaddq, paddq(x.i64, x.i64, y.i64));
    CASE(kOpSsePsubq, psubq(x.i64, x.i64, y.i64));
    CASE(kOpSsePsubsb, psubsb(x.i8, x.i8, y.i8));
    CASE(kOpSsePsubsw, psubsw(x.i16, x.i16, y.i16));
    CASE(kOpSsePaddsb, paddsb(x.i8, x.i8, y.i8));
    CASE(kOpSsePaddsw, paddsw(x.i16, x.i16, y.i16));
    CASE(kOpSsePaddusb, paddusb(x.u8, x.u8, y.u8));
    CASE(kOpSsePaddusw, paddusw(x.u16, x.u16, y.u16));
    CASE(kOpSsePor, por(x.u64, x.u64, y.u64));
    CASE(kOpSsePxor, pxor(x.u64, x.u64, y.u64));
    CASE(kOpSsePand, pand(x.u64, x.u64, y.u64));
    CASE(kOpSsePandn, pandn(x.u64, x.u64, y.u64));
    CASE(kOpSsePsubusb, psubusb(x.u8, x.u8, y.u8));
    CASE(kOpSsePsubusw, psubusw(x.u16, x.u16, y.u16));
    CASE(kOpSsePminub, pminub(x.u8, x.u8, y.u8));
    CASE(kOpSsePmaxub, pmaxub(x.u8, x.u8, y.u8));
    CASE(kOpSsePminsw, pminsw(x.i16, x.i16, y.i16));
    CASE(kOpSsePmaxsw, pmaxsw(x.i16, x.i16, y.i16));
    CASE(kOpSsePunpcklbw, punpcklbw(x.u8, x.u8, y.u8));
    CASE(kOpSsePunpckhbw, punpckhbw(x.u8, x.u8, y.u8));
    CASE(kOpSsePunpcklwd, punpcklwd(x.u16, x.u16, y.u16));
    CASE(kOpSsePunpckldq, punpckldq(x.u32, x.u32, y.u32));
    CASE(kOpSsePunpckhwd, punpckhwd(x.u16, x.u16, y.u16));
    CASE(kOpSsePunpckhdq, punpckhdq(x.u32, x.u32, y.u32));
    CASE(kOpSsePunpcklqdq, punpcklqdq(x.u64, x.u64, y.u64));
    CASE(kOpSsePunpckhqdq, punpckhqdq(x.u64, x.u64, y.u64));
    CASE(kOpSsePacksswb, packsswb(x.i8, x.i16, y.i16));
    CASE(kOpSsePackuswb, packuswb(x.u8, x.i16, y.i16));
    CASE(kOpSsePackssdw, packssdw(x.i16, x.i32, y.i32));
    CASE(kOpSsePcmpgtb, pcmpgtb(x.i8, x.i8, y.i8));
    CASE(kOpSsePcmpgtw, pcmpgtw(x.i16, x.i16, y.i16));
    CASE(kOpSsePcmpgtd, pcmpgtd(x.i32, x.i32, y.i32));
    CASE(kOpSsePcmpeqb, pcmpeqb(x.u8, x.u8, y.u8));
    CASE(kOpSsePcmpeqw, pcmpeqw(x.i16, x.i16, y.i16));
    CASE(kOpSsePcmpeqd, pcmpeqd(x.i32, x.i32, y.i32));
    CASE(kOpSsePsrawv, psrawv(x.i16, x.i16, y.u64));
    CASE(kOpSsePsrlwv, psrlwv(x.u16, x.u16, y.u64));
    CASE(kOpSsePsllwv, psllwv(x.u16, x.u16, y.u64));
    CASE(kOpSsePsradv, psradv(x.i32, x.i32, y.u64));
    CASE(kOpSsePsrldv, psrldv(x.u32, x.u32, y.u64));
    CASE(kOpSsePslldv, pslldv(x.u32, x.u32, y.u64));
    CASE(kOpSsePsrlqv, psrlqv(x.u64, x.u64, y.u64));
    CASE(kOpSsePsllqv, psllqv(x.u64, x.u64, y.u64));
    CASE(kOpSsePavgb, pavgb(x.u8, x.u8, y.u8));
    CASE(kOpSsePavgw, pavgw(x.u16, x.u16, y.u16));
    CASE(kOpSsePsadbw, psadbw(x.u64, x.u8, y.u8));
    CASE(kOpSsePmaddwd, pmaddwd(x.i32, x.i16, y.i16));
    CASE(kOpSsePmulhuw, pmulhuw(x.u16, x.u16, y.u16));
    CASE(kOpSsePmulhw, pmulhw(x.i16, x.i16, y.i16));
    CASE(kOpSsePmuludq, pmuludq(x.u64, x.u32, y.u32));
    CASE(kOpSsePmullw, pmullw(x.i16, x.i16, y.i16));
    CASE(kOpSsePmulld, pmulld(x.i32, x.i32, y.i32));
    CASE(kOpSsePshufb, pshufb(x.u8, x.u8, y.u8));
    CASE(kOpSsePhaddw, phaddw(x.i16, x.i16, y.i16));
    CASE(kOpSsePhaddd, phaddd(x.i32, x.i32, y.i32));
    CASE(kOpSsePhaddsw, phaddsw(x.i16, x.i16, y.i16));
    CASE(kOpSsePmaddubsw, pmaddubsw(x.i16, x.u8, y.i8));
    CASE(kOpSsePhsubw, phsubw(x.i16, x.i16, y.i16));
    CASE(kOpSsePhsubd, phsubd(x.i32, x.i32, y.i32));
    CASE(kOpSsePhsubsw, phsubsw(x.i16, x.i16, y.i16));
    CASE(kOpSsePsignb, psignb(x.i8, x.i8, y.i8));
    CASE(kOpSsePsignw, psignw(x.i16, x.i16, y.i16));
    CASE(kOpSsePsignd, psignd(x.i32, x.i32, y.i32));
    CASE(kOpSsePmulhrsw, pmulhrsw(x.i16, x.i16, y.i16));
    CASE(kOpSsePabsb, pabsb(x.u8, x.i8));
    CASE(kOpSsePabsw, pabsw(x.u16, x.i16));
    CASE(kOpSsePabsd, pabsd(x.u32, x.i32));
    default:
      unreachable;
  }
  if (Prefix66(m->xedd)) {
    memcpy(XmmRexrReg(m), &x, 16);
  } else {
    memcpy(XmmRexrReg(m), &x, 8);
  }
}

void OpSseUdqIb(struct Machine *m, enum OpSseUdqIbKernel kernel) {
  uint8_t i;
  union MachineVector x;
  i = m->xedd->op.uimm0;
  memcpy(&x, XmmRexbRm(m), 16);
  switch (kernel) {
    CASE(kOpSseUdqIbPsrlw, (psrlw)(x.u16, x.u16, i));
    CASE(kOpSseUdqIbPsraw, (psraw)(x.i16, x.i16, i));
    CASE(kOpSseUdqIbPsllw, (psllw)(x.u16, x.u16, i));
    CASE(kOpSseUdqIbPsrld, (psrld)(x.u32, x.u32, i));
    CASE(kOpSseUdqIbPsrad, (psrad)(x.i32, x.i32, i));
    CASE(kOpSseUdqIbPslld, (pslld)(x.u32, x.u32, i));
    CASE(kOpSseUdqIbPsrlq, (psrlq)(x.u64, x.u64, i));
    CASE(kOpSseUdqIbPsrldq, (psrldq)(x.u8, x.u8, i));
    CASE(kOpSseUdqIbPsllq, (psllq)(x.u64, x.u64, i));
    CASE(kOpSseUdqIbPslldq, (pslldq)(x.u8, x.u8, i));
    default:
      unreachable;
  }
  if (Prefix66(m->xedd)) {
    memcpy(XmmRexbRm(m), &x, 16);
  } else {
    memcpy(XmmRexbRm(m), &x, 8);
  }
}

static void OpSsePalignrMmx(struct Machine *m) {
  char t[24];
  memcpy(t, GetModrmRegisterXmmPointerRead8(m), 8);
  memcpy(t + 8, XmmRexrReg(m), 8);
  memset(t + 16, 0, 8);
  memcpy(XmmRexrReg(m), t + MIN(m->xedd->op.uimm0, 16), 8);
}

void OpSsePalignr(struct Machine *m) {
  if (Prefix66(m->xedd)) {
    palignr(XmmRexrReg(m), XmmRexrReg(m), GetModrmRegisterXmmPointerRead8(m),
            m->xedd->op.uimm0);
  } else {
    OpSsePalignrMmx(m);
  }
}
