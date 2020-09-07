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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/conv/itoa.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/modrm.h"

static const char kScale[4][4] = {"", ",2", ",4", ",8"};
static const char kSegName[8][3] = {"es", "cs", "ss", "ds", "fs", "gs"};
static const char kPuttingOnTheRiz[2][4] = {"eiz", "riz"};
static const char kPuttingOnTheRip[2][4] = {"eip", "rip"};

static const char kRegisterName8[2][2][8][5] = {
    {{"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"},
     {"al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil"}},
    {{"wut", "wut", "wut", "wut", "wut", "wut", "wut", "wut"},
     {"r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"}},
};

static const char kRegisterName[2][2][2][8][5] = {
    {{{"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"},
      {"r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"}},
     {{"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi"},
      {"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"}}},
    {{{"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"},
      {"r8w", "re9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"}},
     {{"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi"},
      {"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"}}},
};

static const char kControlName[8][4] = {
    "cr0", "wut", "cr2", "cr3", "cr4", "wut", "wut", "wut",
};

static int64_t RipRelative(struct DisBuilder b, int64_t d) {
  return b.addr + b.xedd->length + d;
}

static const char *GetAddrReg(struct DisBuilder b, uint32_t rde, uint8_t x,
                              uint8_t r) {
  return kRegisterName[Eamode(rde) == XED_MODE_REAL]
                      [Eamode(rde) == XED_MODE_LONG][x & 1][r & 7];
}

static char *DisRegister(char *p, const char *s) {
  if (g_dis_high) p = DisHigh(p, g_dis_high->reg);
  *p++ = '%';
  p = stpcpy(p, s);
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisComment(char *p, const char *s) {
  if (g_dis_high) p = DisHigh(p, g_dis_high->comment);
  p = stpcpy(p, s);
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisRegisterByte(struct DisBuilder b, uint32_t rde, char *p, bool g,
                             int r) {
  return DisRegister(p, kRegisterName8[g][Rex(rde)][r]);
}

static char *DisRegisterWord(struct DisBuilder b, uint32_t rde, char *p, bool g,
                             int r) {
  return DisRegister(p, kRegisterName[Osz(rde)][Rexw(rde)][g][r]);
}

static char *DisInt(char *p, int64_t x) {
  if (-15 <= x && x <= 15) {
    p += int64toarray_radix10(x, p);
  } else if (x == INT64_MIN) {
    p = stpcpy(p, "-0x");
    p += uint64toarray_radix16(INT64_MIN, p);
  } else if (x < 0 && -x < 0xFFFFFFFF) {
    p = stpcpy(p, "-0x");
    p += uint64toarray_radix16(-x, p);
  } else {
    p = stpcpy(p, "0x");
    p += uint64toarray_radix16(x, p);
  }
  return p;
}

static char *DisSym(struct DisBuilder b, char *p, int64_t addr) {
  long sym;
  int64_t addend;
  const char *name;
  if ((sym = DisFindSym(b.dis, addr)) != -1 && b.dis->syms.p[sym].name) {
    addend = addr - b.dis->syms.p[sym].addr;
    name = b.dis->syms.stab + b.dis->syms.p[sym].name;
    p = stpcpy(p, name);
    if (addend) {
      *p++ = '+';
      p = DisInt(p, addend);
    }
    return p;
  } else {
    return DisInt(p, addr);
  }
}

static char *DisSymLiteral(struct DisBuilder b, char *p, uint64_t x) {
  *p++ = '$';
  if (g_dis_high) p = DisHigh(p, g_dis_high->literal);
  p = DisSym(b, p, x);
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisXmm(struct DisBuilder b, uint32_t rde, char *p, const char *s,
                    int reg) {
  if (g_dis_high) p = DisHigh(p, g_dis_high->reg);
  *p++ = '%';
  p = stpcpy(p, s);
  p += uint64toarray_radix10(Rexr(rde) << 3 | reg, p);
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisGvqp(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegisterWord(b, rde, p, Rexr(rde), ModrmReg(rde));
}

static char *DisGdqp(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegister(p, kRegisterName[0][Rexw(rde)][Rexr(rde)][ModrmReg(rde)]);
}

static char *DisGb(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegisterByte(b, rde, p, Rexr(rde), ModrmReg(rde));
}

static char *DisSego(struct DisBuilder b, uint32_t rde, char *p) {
  int seg;
  seg = Sego(rde) ? Sego(rde) : b.xedd->op.hint;
  if (seg) {
    p = DisRegister(p, kSegName[seg - 1]);
    *p++ = ':';
  }
  return p;
}

static char *DisM(struct DisBuilder b, uint32_t rde, char *p) {
  int64_t disp;
  const char *base, *index, *scale;
  p = DisSego(b, rde, p);
  base = index = scale = NULL;
  if (ModrmMod(rde) == 0b01 || ModrmMod(rde) == 0b10 || IsRipRelative(rde) ||
      (Eamode(rde) == XED_MODE_REAL && ModrmRm(rde) == 6 && !ModrmMod(rde)) ||
      (ModrmMod(rde) == 0b00 && ModrmRm(rde) == 0b100 &&
       SibBase(b.xedd) == 0b101)) {
    disp = b.xedd->op.disp;
    if (IsRipRelative(rde)) disp = RipRelative(b, disp);
    p = DisSym(b, p, disp);
  }
  if (Eamode(rde) != XED_MODE_REAL) {
    if (!SibExists(rde)) {
      DCHECK(!b.xedd->op.has_sib);
      if (IsRipRelative(rde)) {
        if (Mode(rde) == XED_MODE_LONG) {
          base = kPuttingOnTheRip[Eamode(rde) == XED_MODE_LONG];
        }
      } else {
        base = GetAddrReg(b, rde, Rexb(rde), ModrmRm(rde));
      }
    } else if (!SibIsAbsolute(b.xedd, rde)) {
      if (SibHasBase(b.xedd, rde)) {
        base = GetAddrReg(b, rde, Rexb(rde), SibBase(b.xedd));
      }
      if (SibHasIndex(b.xedd)) {
        index = GetAddrReg(b, rde, Rexx(b.xedd), SibIndex(b.xedd));
      } else if (b.xedd->op.scale) {
        index = kPuttingOnTheRiz[Eamode(rde) == XED_MODE_LONG];
      }
      scale = kScale[b.xedd->op.scale];
    }
  } else {
    switch (ModrmRm(rde)) {
      case 0:
        base = "bx";
        index = "si";
        break;
      case 1:
        base = "bx";
        index = "di";
        break;
      case 2:
        base = "bp";
        index = "si";
        break;
      case 3:
        base = "bp";
        index = "di";
        break;
      case 4:
        base = "si";
        break;
      case 5:
        base = "di";
        break;
      case 6:
        if (ModrmMod(rde)) base = "bp";
        break;
      case 7:
        base = "bx";
        break;
      default:
        unreachable;
    }
  }
  if (base || index) {
    *p++ = '(';
    if (base) {
      p = DisRegister(p, base);
    }
    if (index) {
      *p++ = ',';
      p = DisRegister(p, index);
      if (scale) {
        p = stpcpy(p, scale);
      }
    }
    *p++ = ')';
  }
  *p = '\0';
  return p;
}

static char *DisEb(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisRegisterByte(b, rde, p, Rexb(rde), ModrmRm(rde));
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisEvqp(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisRegisterWord(b, rde, p, Rexb(rde), ModrmRm(rde));
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisEdqp(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisRegister(p, kRegisterName[0][Rexw(rde)][Rexb(rde)][ModrmRm(rde)]);
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisEv(struct DisBuilder b, uint32_t rde, char *p) {
  const char *s;
  if (IsModrmRegister(rde)) {
    return DisRegister(p, kRegisterName[Osz(rde)][0][Rexb(rde)][ModrmRm(rde)]);
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisGvq(struct DisBuilder b, uint32_t rde, char *p, int r) {
  const char *s;
  if (Mode(rde) == XED_MODE_LONG) {
    s = kRegisterName[Osz(rde)][!Osz(rde)][Rexb(rde)][r];
  } else {
    s = kRegisterName[Osz(rde)][0][Rexb(rde)][r];
  }
  return DisRegister(p, s);
}

static char *DisZvq(struct DisBuilder b, uint32_t rde, char *p) {
  return DisGvq(b, rde, p, ModrmSrm(rde));
}

static char *DisEvq(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisGvq(b, rde, p, ModrmRm(rde));
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisEd(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisRegister(p, kRegisterName[0][0][Rexb(rde)][ModrmRm(rde)]);
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisEq(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisRegister(p, kRegisterName[0][1][Rexb(rde)][ModrmRm(rde)]);
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisZvqp(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegisterWord(b, rde, p, Rexb(rde), ModrmSrm(rde));
}

static char *DisZb(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegisterByte(b, rde, p, Rexb(rde), ModrmSrm(rde));
}

static char *DisEax(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegister(p, kRegisterName[Osz(rde)][0][0][0]);
}

static char *DisRax(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegister(p, kRegisterName[Osz(rde)][Rexw(rde)][0][0]);
}

static char *DisRdx(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegister(p, kRegisterName[Osz(rde)][Rexw(rde)][0][2]);
}

static char *DisCd(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegister(p, kControlName[ModrmReg(rde)]);
}

static char *DisHd(struct DisBuilder b, uint32_t rde, char *p) {
  return DisRegister(
      p, kRegisterName[0][Mode(rde) == XED_MODE_LONG][0][ModrmRm(rde)]);
}

static char *DisImm(struct DisBuilder b, uint32_t rde, char *p) {
  return DisSymLiteral(b, p, b.xedd->op.uimm0);
}

static char *DisRvds(struct DisBuilder b, uint32_t rde, char *p) {
  return DisSymLiteral(b, p, b.xedd->op.disp);
}

static char *DisKvds(struct DisBuilder b, uint32_t rde, char *p) {
  *p++ = '$';
  if (g_dis_high) p = DisHigh(p, g_dis_high->literal);
  p = DisInt(p, b.xedd->op.uimm0);
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisOne(struct DisBuilder b, uint32_t rde, char *p) {
  *p++ = '$';
  if (g_dis_high) p = DisHigh(p, g_dis_high->literal);
  p = stpcpy(p, "1");
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisJbs(struct DisBuilder b, uint32_t rde, char *p) {
  if (b.xedd->op.disp > 0) *p++ = '+';
  p += int64toarray_radix10(b.xedd->op.disp, p);
  return p;
}

static char *DisJb(struct DisBuilder b, uint32_t rde, char *p) {
  if (b.xedd->op.disp > 0) *p++ = '+';
  p += uint64toarray_radix10(b.xedd->op.disp & 0xff, p);
  return p;
}

static char *DisJvds(struct DisBuilder b, uint32_t rde, char *p) {
  return DisSym(b, p, RipRelative(b, b.xedd->op.disp));
}

static char *DisAbs(struct DisBuilder b, uint32_t rde, char *p) {
  return DisSym(b, p, b.xedd->op.disp);
}

static char *DisSw(struct DisBuilder b, uint32_t rde, char *p) {
  if (kSegName[ModrmReg(rde)][0]) p = DisRegister(p, kSegName[ModrmReg(rde)]);
  return p;
}

static char *DisSpecialAddr(struct DisBuilder b, uint32_t rde, char *p, int r) {
  *p++ = '(';
  p = DisRegister(p, GetAddrReg(b, rde, 0, r));
  *p++ = ')';
  *p = '\0';
  return p;
}

static char *DisY(struct DisBuilder b, uint32_t rde, char *p) {
  return DisSpecialAddr(b, rde, p, 7);  // es:di
}

static char *DisX(struct DisBuilder b, uint32_t rde, char *p) {
  DisSego(b, rde, p);
  return DisSpecialAddr(b, rde, p, 6);  // ds:si
}

static char *DisBBb(struct DisBuilder b, uint32_t rde, char *p) {
  DisSego(b, rde, p);
  return DisSpecialAddr(b, rde, p, 3);  // ds:bx
}

static char *DisNq(struct DisBuilder b, uint32_t rde, char *p) {
  return DisXmm(b, rde, p, "mm", ModrmRm(rde));
}

static char *DisUq(struct DisBuilder b, uint32_t rde, char *p) {
  return DisXmm(b, rde, p, "mm", ModrmRm(rde));
}

static char *DisPq(struct DisBuilder b, uint32_t rde, char *p) {
  return DisXmm(b, rde, p, "mm", ModrmReg(rde));
}

static char *DisUdq(struct DisBuilder b, uint32_t rde, char *p) {
  return DisXmm(b, rde, p, "xmm", ModrmRm(rde));
}

static char *DisVdq(struct DisBuilder b, uint32_t rde, char *p) {
  return DisXmm(b, rde, p, "xmm", ModrmReg(rde));
}

static char *DisQq(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisNq(b, rde, p);
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisEst(struct DisBuilder b, uint32_t rde, char *p) {
  p = DisRegister(p, "st");
  if (ModrmRm(rde) != 0) {
    *p++ = '(';
    *p++ = '0' + ModrmRm(rde);
    *p++ = ')';
    *p = '\0';
  }
  return p;
}

static char *DisEst1(struct DisBuilder b, uint32_t rde, char *p) {
  if (ModrmRm(rde) != 1) {
    p = DisEst(b, rde, p);
  } else {
    *p = '\0';
  }
  return p;
}

static char *DisEssr(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisEst(b, rde, p);
  } else {
    return DisM(b, rde, p);
  }
}

static char *DisWps(struct DisBuilder b, uint32_t rde, char *p) {
  if (IsModrmRegister(rde)) {
    return DisUdq(b, rde, p);
  } else {
    return DisM(b, rde, p);
  }
}

#define DisEdr  DisM
#define DisEqp  DisEq
#define DisEsr  DisM
#define DisGv   DisGvqp
#define DisIb   DisImm
#define DisIbs  DisImm
#define DisIbss DisImm
#define DisIvds DisImm
#define DisIvqp DisImm
#define DisIvs  DisImm
#define DisIw   DisImm
#define DisMdi  DisM
#define DisMdq  DisM
#define DisMdqp DisM
#define DisMdr  DisM
#define DisMe   DisM
#define DisMer  DisM
#define DisMp   DisM
#define DisMps  DisM
#define DisMq   DisM
#define DisMqi  DisM
#define DisMs   DisM
#define DisMsr  DisEssr
#define DisMw   DisM
#define DisMwi  DisM
#define DisOb   DisAbs
#define DisOvqp DisAbs
#define DisPpi  DisPq
#define DisQpi  DisQq
#define DisRdqp DisGdqp
#define DisRvqp DisGvqp
#define DisVpd  DisVdq
#define DisVps  DisVdq
#define DisVq   DisVdq
#define DisVsd  DisVdq
#define DisVss  DisVdq
#define DisWdq  DisWps
#define DisWpd  DisWps
#define DisWpsq DisWps
#define DisWq   DisWps
#define DisWsd  DisWps
#define DisWss  DisWps
#define DisXb   DisX
#define DisXv   DisX
#define DisXvqp DisX
#define DisYb   DisY
#define DisYv   DisY
#define DisYvqp DisY
#define DisZv   DisZvqp

static const struct DisArg {
  char s[8];
  char *(*f)(struct DisBuilder, uint32_t, char *);
} kDisArgs[] = /* <sorted> */ {
    {"$1", DisOne},      //
    {"%Cd", DisCd},      //
    {"%Gb", DisGb},      //
    {"%Gdqp", DisGdqp},  //
    {"%Gv", DisGv},      //
    {"%Gvqp", DisGvqp},  //
    {"%Hd", DisHd},      //
    {"%Nq", DisNq},      //
    {"%Ppi", DisPpi},    //
    {"%Pq", DisPq},      //
    {"%Rdqp", DisRdqp},  //
    {"%Rvqp", DisRvqp},  //
    {"%Sw", DisSw},      //
    {"%Udq", DisUdq},    //
    {"%Uq", DisUq},      //
    {"%Vdq", DisVdq},    //
    {"%Vpd", DisVpd},    //
    {"%Vps", DisVps},    //
    {"%Vq", DisVq},      //
    {"%Vsd", DisVsd},    //
    {"%Vss", DisVss},    //
    {"%Zb", DisZb},      //
    {"%Zv", DisZv},      //
    {"%Zvq", DisZvq},    //
    {"%Zvqp", DisZvqp},  //
    {"%eAX", DisEax},    //
    {"%rAX", DisRax},    //
    {"%rDX", DisRdx},    //
    {"BBb", DisBBb},     //
    {"EST", DisEst},     //
    {"EST1", DisEst1},   //
    {"ESsr", DisEssr},   //
    {"Eb", DisEb},       //
    {"Ed", DisEd},       //
    {"Edqp", DisEdqp},   //
    {"Edr", DisEdr},     //
    {"Eq", DisEq},       //
    {"Eqp", DisEqp},     //
    {"Esr", DisEsr},     //
    {"Ev", DisEv},       //
    {"Evq", DisEvq},     //
    {"Evqp", DisEvqp},   //
    {"Ew", DisEvqp},     //
    {"Ib", DisIb},       //
    {"Ibs", DisIbs},     //
    {"Ibss", DisIbss},   //
    {"Ivds", DisIvds},   //
    {"Ivqp", DisIvqp},   //
    {"Ivs", DisIvs},     //
    {"Iw", DisIw},       //
    {"Jb", DisJb},       //
    {"Jbs", DisJbs},     //
    {"Jvds", DisJvds},   //
    {"Kvds", DisKvds},   //
    {"M", DisM},         //
    {"Mdi", DisMdi},     //
    {"Mdq", DisMdq},     //
    {"Mdqp", DisMdqp},   //
    {"Mdr", DisMdr},     //
    {"Me", DisMe},       //
    {"Mer", DisMer},     //
    {"Mp", DisMp},       //
    {"Mps", DisMps},     //
    {"Mq", DisMq},       //
    {"Mqi", DisMqi},     //
    {"Ms", DisMs},       //
    {"Msr", DisMsr},     //
    {"Mw", DisMw},       //
    {"Mwi", DisMwi},     //
    {"Ob", DisOb},       //
    {"Ovqp", DisOvqp},   //
    {"Qpi", DisQpi},     //
    {"Qq", DisQq},       //
    {"Rvds", DisRvds},   //
    {"Wdq", DisWdq},     //
    {"Wpd", DisWpd},     //
    {"Wps", DisWps},     //
    {"Wpsq", DisWpsq},   //
    {"Wq", DisWq},       //
    {"Wsd", DisWsd},     //
    {"Wss", DisWss},     //
    {"Xb", DisXb},       //
    {"Xv", DisXv},       //
    {"Xvqp", DisXvqp},   //
    {"Yb", DisYb},       //
    {"Yv", DisYv},       //
    {"Yvqp", DisYvqp},   //
};

static int CompareString8(const char a[8], const char b[8]) {
  uint64_t x, y;
  x = READ64BE(a);
  y = READ64BE(b);
  return x > y ? 1 : x < y ? -1 : 0;
}

char *DisArg(struct DisBuilder b, char *p, const char *s) {
  char key[8];
  int c, m, l, r;
  l = 0;
  r = ARRAYLEN(kDisArgs) - 1;
  strncpy(key, s, 8);
  while (l <= r) {
    m = (l + r) >> 1;
    c = CompareString8(kDisArgs[m].s, key);
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else {
      return kDisArgs[m].f(b, b.xedd->op.rde, p);
    }
  }
  if (*s == '%') {
    p = DisRegister(p, s + 1);
  } else {
    p = stpcpy(p, s);
  }
  return p;
}
