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
#include "tool/build/lib/case.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/sse.h"
#include "tool/build/lib/throw.h"

static void SsePsubb(void *b, const void *a) {
  psubb(b, b, a);
}

static void SsePaddb(void *b, const void *a) {
  paddb(b, b, a);
}

static void SsePsubw(void *b, const void *a) {
  psubw(b, b, a);
}

static void SsePaddw(void *b, const void *a) {
  paddw(b, b, a);
}

static void SsePsubd(void *b, const void *a) {
  psubd(b, b, a);
}

static void SsePaddd(void *b, const void *a) {
  paddd(b, b, a);
}

static void SsePaddq(void *b, const void *a) {
  paddq(b, b, a);
}

static void SsePsubq(void *b, const void *a) {
  psubq(b, b, a);
}

static void SsePsubsb(void *b, const void *a) {
  psubsb(b, b, a);
}

static void SsePsubsw(void *b, const void *a) {
  psubsw(b, b, a);
}

static void SsePaddsb(void *b, const void *a) {
  paddsb(b, b, a);
}

static void SsePaddsw(void *b, const void *a) {
  paddsw(b, b, a);
}

static void SsePaddusb(void *b, const void *a) {
  paddusb(b, b, a);
}

static void SsePaddusw(void *b, const void *a) {
  paddusw(b, b, a);
}

static void SsePor(void *b, const void *a) {
  por(b, b, a);
}

static void SsePxor(void *b, const void *a) {
  pxor(b, b, a);
}

static void SsePand(void *b, const void *a) {
  pand(b, b, a);
}

static void SsePandn(void *b, const void *a) {
  pandn(b, b, a);
}

static void SsePsubusb(void *b, const void *a) {
  psubusb(b, b, a);
}

static void SsePsubusw(void *b, const void *a) {
  psubusw(b, b, a);
}

static void SsePminub(void *b, const void *a) {
  pminub(b, b, a);
}

static void SsePmaxub(void *b, const void *a) {
  pmaxub(b, b, a);
}

static void SsePminsw(void *b, const void *a) {
  pminsw(b, b, a);
}

static void SsePmaxsw(void *b, const void *a) {
  pmaxsw(b, b, a);
}

static void SsePunpcklbw(void *b, const void *a) {
  punpcklbw(b, b, a);
}

static void SsePunpckhbw(void *b, const void *a) {
  punpckhbw(b, b, a);
}

static void SsePunpcklwd(void *b, const void *a) {
  punpcklwd(b, b, a);
}

static void SsePunpckldq(void *b, const void *a) {
  punpckldq(b, b, a);
}

static void SsePunpckhwd(void *b, const void *a) {
  punpckhwd(b, b, a);
}

static void SsePunpckhdq(void *b, const void *a) {
  punpckhdq(b, b, a);
}

static void SsePunpcklqdq(void *b, const void *a) {
  punpcklqdq(b, b, a);
}

static void SsePunpckhqdq(void *b, const void *a) {
  punpckhqdq(b, b, a);
}

static void SsePacksswb(void *b, const void *a) {
  packsswb(b, b, a);
}

static void SsePackuswb(void *b, const void *a) {
  packuswb(b, b, a);
}

static void SsePackssdw(void *b, const void *a) {
  packssdw(b, b, a);
}

static void SsePcmpgtb(void *b, const void *a) {
  pcmpgtb(b, b, a);
}

static void SsePcmpgtw(void *b, const void *a) {
  pcmpgtw(b, b, a);
}

static void SsePcmpgtd(void *b, const void *a) {
  pcmpgtd(b, b, a);
}

static void SsePcmpeqb(void *b, const void *a) {
  pcmpeqb(b, b, a);
}

static void SsePcmpeqw(void *b, const void *a) {
  pcmpeqw(b, b, a);
}

static void SsePcmpeqd(void *b, const void *a) {
  pcmpeqd(b, b, a);
}

static void SsePsrawv(void *b, const void *a) {
  psrawv(b, b, a);
}

static void SsePsrlwv(void *b, const void *a) {
  psrlwv(b, b, a);
}

static void SsePsllwv(void *b, const void *a) {
  psllwv(b, b, a);
}

static void SsePsradv(void *b, const void *a) {
  psradv(b, b, a);
}

static void SsePsrldv(void *b, const void *a) {
  psrldv(b, b, a);
}

static void SsePslldv(void *b, const void *a) {
  pslldv(b, b, a);
}

static void SsePsrlqv(void *b, const void *a) {
  psrlqv(b, b, a);
}

static void SsePsllqv(void *b, const void *a) {
  psllqv(b, b, a);
}

static void SsePavgb(void *b, const void *a) {
  pavgb(b, b, a);
}

static void SsePavgw(void *b, const void *a) {
  pavgw(b, b, a);
}

static void SsePsadbw(void *b, const void *a) {
  psadbw(b, b, a);
}

static void SsePmaddwd(void *b, const void *a) {
  pmaddwd(b, b, a);
}

static void SsePmulhuw(void *b, const void *a) {
  pmulhuw(b, b, a);
}

static void SsePmulhw(void *b, const void *a) {
  pmulhw(b, b, a);
}

static void SsePmuludq(void *b, const void *a) {
  pmuludq(b, b, a);
}

static void SsePmullw(void *b, const void *a) {
  pmullw(b, b, a);
}

static void SsePmulld(void *b, const void *a) {
  pmulld(b, b, a);
}

static void SsePshufb(void *b, const void *a) {
  pshufb(b, b, a);
}

static void SsePhaddw(void *b, const void *a) {
  phaddw(b, b, a);
}

static void SsePhaddd(void *b, const void *a) {
  phaddd(b, b, a);
}

static void SsePhaddsw(void *b, const void *a) {
  phaddsw(b, b, a);
}

static void SsePmaddubsw(void *b, const void *a) {
  pmaddubsw(b, b, a);
}

static void SsePhsubw(void *b, const void *a) {
  phsubw(b, b, a);
}

static void SsePhsubd(void *b, const void *a) {
  phsubd(b, b, a);
}

static void SsePhsubsw(void *b, const void *a) {
  phsubsw(b, b, a);
}

static void SsePsignb(void *b, const void *a) {
  psignb(b, b, a);
}

static void SsePsignw(void *b, const void *a) {
  psignw(b, b, a);
}

static void SsePsignd(void *b, const void *a) {
  psignd(b, b, a);
}

static void SsePmulhrsw(void *b, const void *a) {
  pmulhrsw(b, b, a);
}

static void SsePabsb(void *b, const void *a) {
  pabsb(b, a);
}

static void SsePabsw(void *b, const void *a) {
  pabsw(b, a);
}

static void SsePabsd(void *b, const void *a) {
  pabsd(b, a);
}

static void OpSse(struct Machine *m, uint32_t rde,
                  void kernel(void *, const void *)) {
  char x[16], y[16];
  if (Osz(rde)) {
    kernel(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead16(m, rde));
  } else {
    memcpy(x, XmmRexrReg(m, rde), 8);
    memcpy(y, GetModrmRegisterXmmPointerRead16(m, rde), 8);
    kernel(x, y);
    memcpy(XmmRexrReg(m, rde), x, 8);
  }
}

void OpSsePunpcklbw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePunpcklbw);
}

void OpSsePunpcklwd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePunpcklwd);
}

void OpSsePunpckldq(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePunpckldq);
}

void OpSsePacksswb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePacksswb);
}

void OpSsePcmpgtb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePcmpgtb);
}

void OpSsePcmpgtw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePcmpgtw);
}

void OpSsePcmpgtd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePcmpgtd);
}

void OpSsePackuswb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePackuswb);
}

void OpSsePunpckhbw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePunpckhbw);
}

void OpSsePunpckhwd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePunpckhwd);
}

void OpSsePunpckhdq(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePunpckhdq);
}

void OpSsePackssdw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePackssdw);
}

void OpSsePunpcklqdq(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePunpcklqdq);
}

void OpSsePunpckhqdq(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePunpckhqdq);
}

void OpSsePcmpeqb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePcmpeqb);
}

void OpSsePcmpeqw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePcmpeqw);
}

void OpSsePcmpeqd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePcmpeqd);
}

void OpSsePsrlwv(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsrlwv);
}

void OpSsePsrldv(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsrldv);
}

void OpSsePsrlqv(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsrlqv);
}

void OpSsePaddq(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePaddq);
}

void OpSsePmullw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmullw);
}

void OpSsePsubusb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsubusb);
}

void OpSsePsubusw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsubusw);
}

void OpSsePminub(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePminub);
}

void OpSsePand(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePand);
}

void OpSsePaddusb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePaddusb);
}

void OpSsePaddusw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePaddusw);
}

void OpSsePmaxub(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmaxub);
}

void OpSsePandn(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePandn);
}

void OpSsePavgb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePavgb);
}

void OpSsePsrawv(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsrawv);
}

void OpSsePsradv(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsradv);
}

void OpSsePavgw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePavgw);
}

void OpSsePmulhuw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmulhuw);
}

void OpSsePmulhw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmulhw);
}

void OpSsePsubsb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsubsb);
}

void OpSsePsubsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsubsw);
}

void OpSsePminsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePminsw);
}

void OpSsePor(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePor);
}

void OpSsePaddsb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePaddsb);
}

void OpSsePaddsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePaddsw);
}

void OpSsePmaxsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmaxsw);
}

void OpSsePxor(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePxor);
}

void OpSsePsllwv(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsllwv);
}

void OpSsePslldv(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePslldv);
}

void OpSsePsllqv(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsllqv);
}

void OpSsePmuludq(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmuludq);
}

void OpSsePmaddwd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmaddwd);
}

void OpSsePsadbw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsadbw);
}

void OpSsePsubb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsubb);
}

void OpSsePsubw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsubw);
}

void OpSsePsubd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsubd);
}

void OpSsePsubq(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsubq);
}

void OpSsePaddb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePaddb);
}

void OpSsePaddw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePaddw);
}

void OpSsePaddd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePaddd);
}

void OpSsePshufb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePshufb);
}

void OpSsePhaddw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePhaddw);
}

void OpSsePhaddd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePhaddd);
}

void OpSsePhaddsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePhaddsw);
}

void OpSsePmaddubsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmaddubsw);
}

void OpSsePhsubw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePhsubw);
}

void OpSsePhsubd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePhsubd);
}

void OpSsePhsubsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePhsubsw);
}

void OpSsePsignb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsignb);
}

void OpSsePsignw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsignw);
}

void OpSsePsignd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePsignd);
}

void OpSsePmulhrsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmulhrsw);
}

void OpSsePabsb(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePabsb);
}

void OpSsePabsw(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePabsw);
}

void OpSsePabsd(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePabsd);
}

void OpSsePmulld(struct Machine *m, uint32_t rde) {
  OpSse(m, rde, SsePmulld);
}

static void SseUdqIb(struct Machine *m, uint32_t rde, int kernel) {
  void *y;
  uint8_t i;
  uint8_t x[16];
  i = m->xedd->op.uimm0;
  y = XmmRexbRm(m, rde);
  switch (kernel) {
    CASE(0, (psrlw)((void *)x, y, i));
    CASE(1, (psraw)((void *)x, y, i));
    CASE(2, (psllw)((void *)x, y, i));
    CASE(3, (psrld)((void *)x, y, i));
    CASE(4, (psrad)((void *)x, y, i));
    CASE(5, (pslld)((void *)x, y, i));
    CASE(6, (psrlq)((void *)x, y, i));
    CASE(7, (psrldq)((void *)x, y, i));
    CASE(8, (psllq)((void *)x, y, i));
    CASE(9, (pslldq)((void *)x, y, i));
    default:
      unreachable;
  }
  if (Osz(rde)) {
    memcpy(XmmRexbRm(m, rde), x, 16);
  } else {
    memcpy(XmmRexbRm(m, rde), x, 8);
  }
}

void Op171(struct Machine *m, uint32_t rde) {
  switch (ModrmReg(rde)) {
    case 2:
      SseUdqIb(m, rde, 0);
      break;
    case 4:
      SseUdqIb(m, rde, 1);
      break;
    case 6:
      SseUdqIb(m, rde, 2);
      break;
    default:
      OpUd(m, rde);
  }
}

void Op172(struct Machine *m, uint32_t rde) {
  switch (ModrmReg(rde)) {
    case 2:
      SseUdqIb(m, rde, 3);
      break;
    case 4:
      SseUdqIb(m, rde, 4);
      break;
    case 6:
      SseUdqIb(m, rde, 5);
      break;
    default:
      OpUd(m, rde);
  }
}

void Op173(struct Machine *m, uint32_t rde) {
  switch (ModrmReg(rde)) {
    case 2:
      SseUdqIb(m, rde, 6);
      break;
    case 3:
      SseUdqIb(m, rde, 7);
      break;
    case 6:
      SseUdqIb(m, rde, 8);
      break;
    case 7:
      SseUdqIb(m, rde, 9);
      break;
    default:
      OpUd(m, rde);
  }
}

static void OpSsePalignrMmx(struct Machine *m, uint32_t rde) {
  char t[24];
  memcpy(t, GetModrmRegisterXmmPointerRead8(m, rde), 8);
  memcpy(t + 8, XmmRexrReg(m, rde), 8);
  memset(t + 16, 0, 8);
  memcpy(XmmRexrReg(m, rde), t + MIN(m->xedd->op.uimm0, 16), 8);
}

void OpSsePalignr(struct Machine *m, uint32_t rde) {
  if (Osz(rde)) {
    palignr(XmmRexrReg(m, rde), XmmRexrReg(m, rde),
            GetModrmRegisterXmmPointerRead8(m, rde), m->xedd->op.uimm0);
  } else {
    OpSsePalignrMmx(m, rde);
  }
}
