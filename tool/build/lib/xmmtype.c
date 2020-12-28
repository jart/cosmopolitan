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
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/xmmtype.h"

static void UpdateXmmTypes(struct Machine *m, struct XmmType *xt, int regtype,
                           int rmtype) {
  xt->type[RexrReg(m->xedd->op.rde)] = regtype;
  if (IsModrmRegister(m->xedd->op.rde)) {
    xt->type[RexbRm(m->xedd->op.rde)] = rmtype;
  }
}

static void UpdateXmmSizes(struct Machine *m, struct XmmType *xt, int regsize,
                           int rmsize) {
  xt->size[RexrReg(m->xedd->op.rde)] = regsize;
  if (IsModrmRegister(m->xedd->op.rde)) {
    xt->size[RexbRm(m->xedd->op.rde)] = rmsize;
  }
}

void UpdateXmmType(struct Machine *m, struct XmmType *xt) {
  switch (m->xedd->op.dispatch) {
    case 0x110:
    case 0x111:  // MOVSS,MOVSD
      if (Rep(m->xedd->op.rde) == 3) {
        UpdateXmmTypes(m, xt, kXmmFloat, kXmmFloat);
      } else if (Rep(m->xedd->op.rde) == 2) {
        UpdateXmmTypes(m, xt, kXmmDouble, kXmmDouble);
      }
      break;
    case 0x12E:  // UCOMIS
    case 0x12F:  // COMIS
    case 0x151:  // SQRT
    case 0x152:  // RSQRT
    case 0x153:  // RCP
    case 0x158:  // ADD
    case 0x159:  // MUL
    case 0x15C:  // SUB
    case 0x15D:  // MIN
    case 0x15E:  // DIV
    case 0x15F:  // MAX
    case 0x1C2:  // CMP
      if (Osz(m->xedd->op.rde) || Rep(m->xedd->op.rde) == 2) {
        UpdateXmmTypes(m, xt, kXmmDouble, kXmmDouble);
      } else {
        UpdateXmmTypes(m, xt, kXmmFloat, kXmmFloat);
      }
      break;
    case 0x12A:  // CVTPI2PS,CVTSI2SS,CVTPI2PD,CVTSI2SD
      if (Osz(m->xedd->op.rde) || Rep(m->xedd->op.rde) == 2) {
        UpdateXmmSizes(m, xt, 8, 4);
        UpdateXmmTypes(m, xt, kXmmDouble, kXmmIntegral);
      } else {
        UpdateXmmSizes(m, xt, 4, 4);
        UpdateXmmTypes(m, xt, kXmmFloat, kXmmIntegral);
      }
      break;
    case 0x15A:  // CVT{P,S}{S,D}2{P,S}{S,D}
      if (Osz(m->xedd->op.rde) || Rep(m->xedd->op.rde) == 2) {
        UpdateXmmTypes(m, xt, kXmmFloat, kXmmDouble);
      } else {
        UpdateXmmTypes(m, xt, kXmmDouble, kXmmFloat);
      }
      break;
    case 0x15B:  // CVT{,T}{DQ,PS}2{PS,DQ}
      UpdateXmmSizes(m, xt, 4, 4);
      if (Osz(m->xedd->op.rde) || Rep(m->xedd->op.rde) == 3) {
        UpdateXmmTypes(m, xt, kXmmIntegral, kXmmFloat);
      } else {
        UpdateXmmTypes(m, xt, kXmmFloat, kXmmIntegral);
      }
      break;
    case 0x17C:  // HADD
    case 0x17D:  // HSUB
    case 0x1D0:  // ADDSUB
      if (Osz(m->xedd->op.rde)) {
        UpdateXmmTypes(m, xt, kXmmDouble, kXmmDouble);
      } else {
        UpdateXmmTypes(m, xt, kXmmFloat, kXmmFloat);
      }
      break;
    case 0x164:  // PCMPGTB
    case 0x174:  // PCMPEQB
    case 0x1D8:  // PSUBUSB
    case 0x1DA:  // PMINUB
    case 0x1DC:  // PADDUSB
    case 0x1DE:  // PMAXUB
    case 0x1E0:  // PAVGB
    case 0x1E8:  // PSUBSB
    case 0x1EC:  // PADDSB
    case 0x1F8:  // PSUBB
    case 0x1FC:  // PADDB
      UpdateXmmSizes(m, xt, 1, 1);
      UpdateXmmTypes(m, xt, kXmmIntegral, kXmmIntegral);
      break;
    case 0x165:  // PCMPGTW
    case 0x175:  // PCMPEQW
    case 0x171:  // PSRLW,PSRAW,PSLLW
    case 0x1D1:  // PSRLW
    case 0x1D5:  // PMULLW
    case 0x1D9:  // PSUBUSW
    case 0x1DD:  // PADDUSW
    case 0x1E1:  // PSRAW
    case 0x1E3:  // PAVGW
    case 0x1E4:  // PMULHUW
    case 0x1E5:  // PMULHW
    case 0x1E9:  // PSUBSW
    case 0x1EA:  // PMINSW
    case 0x1ED:  // PADDSW
    case 0x1EE:  // PMAXSW
    case 0x1F1:  // PSLLW
    case 0x1F6:  // PSADBW
    case 0x1F9:  // PSUBW
    case 0x1FD:  // PADDW
      UpdateXmmSizes(m, xt, 2, 2);
      UpdateXmmTypes(m, xt, kXmmIntegral, kXmmIntegral);
      break;
    case 0x166:  // PCMPGTD
    case 0x176:  // PCMPEQD
    case 0x172:  // PSRLD,PSRAD,PSLLD
    case 0x1D2:  // PSRLD
    case 0x1E2:  // PSRAD
    case 0x1F2:  // PSLLD
    case 0x1FA:  // PSUBD
    case 0x1FE:  // PADDD
      UpdateXmmSizes(m, xt, 4, 4);
      UpdateXmmTypes(m, xt, kXmmIntegral, kXmmIntegral);
      break;
    case 0x173:  // PSRLQ,PSRLQ,PSRLDQ,PSLLQ,PSLLDQ
    case 0x1D3:  // PSRLQ
    case 0x1D4:  // PADDQ
    case 0x1F3:  // PSLLQ
    case 0x1F4:  // PMULUDQ
    case 0x1FB:  // PSUBQ
      UpdateXmmSizes(m, xt, 8, 8);
      UpdateXmmTypes(m, xt, kXmmIntegral, kXmmIntegral);
      break;
    case 0x16B:  // PACKSSDW
    case 0x1F5:  // PMADDWD
      UpdateXmmSizes(m, xt, 4, 2);
      UpdateXmmTypes(m, xt, kXmmIntegral, kXmmIntegral);
      break;
    case 0x163:  // PACKSSWB
    case 0x167:  // PACKUSWB
      UpdateXmmSizes(m, xt, 1, 2);
      UpdateXmmTypes(m, xt, kXmmIntegral, kXmmIntegral);
      break;
    case 0x128:  // MOVAPS Vps Wps
      if (IsModrmRegister(m->xedd->op.rde)) {
        xt->type[RexrReg(m->xedd->op.rde)] = xt->type[RexbRm(m->xedd->op.rde)];
        xt->size[RexrReg(m->xedd->op.rde)] = xt->size[RexbRm(m->xedd->op.rde)];
      }
      break;
    case 0x129:  // MOVAPS Wps Vps
      if (IsModrmRegister(m->xedd->op.rde)) {
        xt->type[RexbRm(m->xedd->op.rde)] = xt->type[RexrReg(m->xedd->op.rde)];
        xt->size[RexbRm(m->xedd->op.rde)] = xt->size[RexrReg(m->xedd->op.rde)];
      }
      break;
    case 0x16F:  // MOVDQA Vdq Wdq
      if (Osz(m->xedd->op.rde) && IsModrmRegister(m->xedd->op.rde)) {
        xt->type[RexrReg(m->xedd->op.rde)] = xt->type[RexbRm(m->xedd->op.rde)];
        xt->size[RexrReg(m->xedd->op.rde)] = xt->size[RexbRm(m->xedd->op.rde)];
      }
      break;
    default:
      return;
  }
}
