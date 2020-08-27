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
#include "libc/conv/itoa.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/modrm.h"

#define SSTRCMP8(S1, S2)        \
  ({                            \
    uint64_t x, y;              \
    x = READ64BE(S1);           \
    y = READ64BE(S2);           \
    x > y ? 1 : x < y ? -1 : 0; \
  })

static const char kScale[4][4] = {"", ",2", ",4", ",8"};
static const char kSegName[8][3] = {"es", "cs", "ss", "ds", "fs", "gs"};
static const char kSegOverride[8][3] = {"", "cs", "ds", "es", "fs", "gs", "ss"};

static const char kRegisterName8[2][2][8][5] = {
    {{"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"},
     {"al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil"}},
    {{"???", "???", "???", "???", "???", "???", "???", "???"},
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

static int64_t RipRelative(struct DisBuilder b, int64_t d) {
  return b.addr + b.xedd->length + d;
}

static const char *GetAddrReg(struct DisBuilder b, uint8_t x, uint8_t r) {
  return kRegisterName[0][!Asz(b.xedd->op.rde)][x & 1][r & 7];
}

static char *DisRegister(char *p, const char *s) {
  if (g_dis_high) p = DisHigh(p, g_dis_high->reg);
  *p++ = '%';
  p = stpcpy(p, s);
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisLiteral(char *p, const char *s) {
  if (g_dis_high) p = DisHigh(p, g_dis_high->literal);
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

static char *DisRegisterByte(struct DisBuilder b, char *p, bool g, int r) {
  return DisRegister(p, kRegisterName8[g][Rex(b.xedd->op.rde)][r]);
}

static char *DisRegisterWord(struct DisBuilder b, char *p, bool g, int r) {
  return DisRegister(
      p, kRegisterName[Osz(b.xedd->op.rde)][Rexw(b.xedd->op.rde)][g][r]);
}

static char *DisGvqp(struct DisBuilder b, char *p) {
  return DisRegisterWord(b, p, Rexr(b.xedd->op.rde), ModrmReg(b.xedd->op.rde));
}

static char *DisGdqp(struct DisBuilder b, char *p) {
  return DisRegister(p, kRegisterName[0][Rexw(b.xedd->op.rde)][Rexr(
                            b.xedd->op.rde)][ModrmReg(b.xedd->op.rde)]);
}

static char *DisGb(struct DisBuilder b, char *p) {
  return DisRegisterByte(b, p, Rexr(b.xedd->op.rde), ModrmReg(b.xedd->op.rde));
}

static uint8_t DisSeg(struct DisBuilder b) {
  return b.xedd->op.seg_ovd ? b.xedd->op.seg_ovd : b.xedd->op.hint;
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

static char *DisM(struct DisBuilder b, char *p) {
  int64_t disp;
  const char *seg, *base, *index, *scale;
  base = index = scale = NULL;
  seg = kSegOverride[b.xedd->op.seg_ovd ? b.xedd->op.seg_ovd : b.xedd->op.hint];
  if (*seg) {
    p = DisRegister(p, seg);
    *p++ = ':';
  }
  if (ModrmMod(b.xedd->op.rde) == 0b01 || ModrmMod(b.xedd->op.rde) == 0b10 ||
      IsRipRelative(b.xedd->op.rde) ||
      (ModrmMod(b.xedd->op.rde) == 0b00 && ModrmRm(b.xedd->op.rde) == 0b100 &&
       SibBase(b.xedd->op.rde) == 0b101)) {
    disp = b.xedd->op.disp;
    if (IsRipRelative(b.xedd->op.rde)) disp = RipRelative(b, disp);
    p = DisSym(b, p, disp);
  }
  if (!SibExists(b.xedd->op.rde)) {
    DCHECK(!b.xedd->op.has_sib);
    if (IsRipRelative(b.xedd->op.rde)) {
      base = "rip";
    } else {
      base = GetAddrReg(b, Rexb(b.xedd->op.rde), ModrmRm(b.xedd->op.rde));
    }
  } else if (!SibIsAbsolute(b.xedd->op.rde)) {
    if (SibHasBase(b.xedd->op.rde)) {
      base = GetAddrReg(b, Rexb(b.xedd->op.rde), SibBase(b.xedd->op.rde));
    }
    if (SibHasIndex(b.xedd->op.rde)) {
      index = GetAddrReg(b, Rexx(b.xedd->op.rde), SibIndex(b.xedd->op.rde));
    } else if (b.xedd->op.scale) {
      index = Asz(b.xedd->op.rde) ? "eiz" : "riz";
    }
    scale = kScale[b.xedd->op.scale];
  }
  if (base || index) {
    *p++ = '(';
    if (base) {
      p = DisRegister(p, base);
    }
    if (index) {
      *p++ = ',';
      p = DisRegister(p, index);
      p = stpcpy(p, scale);
    }
    *p++ = ')';
  }
  *p = '\0';
  return p;
}

static char *DisEb(struct DisBuilder b, char *p) {
  if (IsModrmRegister(b.xedd->op.rde)) {
    return DisRegisterByte(b, p, Rexb(b.xedd->op.rde), ModrmRm(b.xedd->op.rde));
  } else {
    return DisM(b, p);
  }
}

static char *DisEvqp(struct DisBuilder b, char *p) {
  if (IsModrmRegister(b.xedd->op.rde)) {
    return DisRegisterWord(b, p, Rexb(b.xedd->op.rde), ModrmRm(b.xedd->op.rde));
  } else {
    return DisM(b, p);
  }
}

static char *DisEdqp(struct DisBuilder b, char *p) {
  if (IsModrmRegister(b.xedd->op.rde)) {
    return DisRegister(p, kRegisterName[0][Rexw(b.xedd->op.rde)][Rexb(
                              b.xedd->op.rde)][ModrmRm(b.xedd->op.rde)]);
  } else {
    return DisM(b, p);
  }
}

static char *DisEvq(struct DisBuilder b, char *p) {
  const char *s;
  if (IsModrmRegister(b.xedd->op.rde)) {
    if (Osz(b.xedd->op.rde)) {
      s = kRegisterName[1][0][Rexb(b.xedd->op.rde)][ModrmRm(b.xedd->op.rde)];
    } else {
      s = kRegisterName[0][1][Rexb(b.xedd->op.rde)][ModrmRm(b.xedd->op.rde)];
    }
    return DisRegister(p, s);
  } else {
    return DisM(b, p);
  }
}

static char *DisEd(struct DisBuilder b, char *p) {
  if (IsModrmRegister(b.xedd->op.rde)) {
    return DisRegister(
        p, kRegisterName[0][0][Rexb(b.xedd->op.rde)][ModrmRm(b.xedd->op.rde)]);
  } else {
    return DisM(b, p);
  }
}

static char *DisEq(struct DisBuilder b, char *p) {
  if (IsModrmRegister(b.xedd->op.rde)) {
    return DisRegister(
        p, kRegisterName[0][1][Rexb(b.xedd->op.rde)][ModrmRm(b.xedd->op.rde)]);
  } else {
    return DisM(b, p);
  }
}

static char *DisZvq(struct DisBuilder b, char *p) {
  if (Osz(b.xedd->op.rde)) {
    return DisRegister(
        p, kRegisterName[1][0][Rexb(b.xedd->op.rde)][ModrmSrm(b.xedd->op.rde)]);
  } else {
    return DisRegister(
        p, kRegisterName[0][1][Rexb(b.xedd->op.rde)][ModrmSrm(b.xedd->op.rde)]);
  }
}

static char *DisZvqp(struct DisBuilder b, char *p) {
  return DisRegisterWord(b, p, Rexb(b.xedd->op.rde), ModrmSrm(b.xedd->op.rde));
}

static char *DisZb(struct DisBuilder b, char *p) {
  return DisRegisterByte(b, p, Rexb(b.xedd->op.rde), ModrmSrm(b.xedd->op.rde));
}

static char *DisEax(struct DisBuilder b, char *p) {
  return DisRegister(p, kRegisterName[Osz(b.xedd->op.rde)][0][0][0]);
}

static char *DisRax(struct DisBuilder b, char *p) {
  return DisRegister(
      p, kRegisterName[Osz(b.xedd->op.rde)][Rexw(b.xedd->op.rde)][0][0]);
}

static char *DisRdx(struct DisBuilder b, char *p) {
  return DisRegister(
      p, kRegisterName[Osz(b.xedd->op.rde)][Rexw(b.xedd->op.rde)][0][2]);
}

static char *DisImm(struct DisBuilder b, char *p) {
  *p++ = '$';
  if (g_dis_high) p = DisHigh(p, g_dis_high->literal);
  p = DisSym(b, p, b.xedd->op.uimm0);
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisJbs(struct DisBuilder b, char *p) {
  if (b.xedd->op.disp > 0) *p++ = '+';
  p += int64toarray_radix10(b.xedd->op.disp, p);
  return p;
}

static char *DisJb(struct DisBuilder b, char *p) {
  if (b.xedd->op.disp > 0) *p++ = '+';
  p += uint64toarray_radix10(b.xedd->op.disp & 0xff, p);
  return p;
}

static char *DisJvds(struct DisBuilder b, char *p) {
  return DisSym(b, p, RipRelative(b, b.xedd->op.disp));
}

static char *DisAbs(struct DisBuilder b, char *p) {
  return DisSym(b, p, b.xedd->op.disp);
}

static char *DisSw(struct DisBuilder b, char *p) {
  if (kSegName[ModrmReg(b.xedd->op.rde)][0]) {
    p = DisRegister(p, kSegName[ModrmReg(b.xedd->op.rde)]);
  }
  *p = '\0';
  return p;
}

static char *DisY(struct DisBuilder b, char *p) {
  *p++ = '(';
  p = DisRegister(p, Asz(b.xedd->op.rde) ? "edi" : "rdi");
  *p++ = ')';
  *p = '\0';
  return p;
}

static char *DisX(struct DisBuilder b, char *p) {
  if (kSegOverride[b.xedd->op.seg_ovd][0]) {
    p = DisRegister(p, kSegOverride[b.xedd->op.seg_ovd]);
  }
  *p++ = '(';
  p = DisRegister(p, Asz(b.xedd->op.rde) ? "esi" : "rsi");
  *p++ = ')';
  *p = '\0';
  return p;
}

static char *DisBBb(struct DisBuilder b, char *p) {
  if (kSegOverride[b.xedd->op.seg_ovd][0]) {
    p = DisRegister(p, kSegOverride[b.xedd->op.seg_ovd]);
  }
  *p++ = '(';
  p = DisRegister(p, Asz(b.xedd->op.rde) ? "ebx" : "rbx");
  *p++ = ')';
  *p = '\0';
  return p;
}

static char *DisXmm(struct DisBuilder b, char *p, const char *s, int reg) {
  if (g_dis_high) p = DisHigh(p, g_dis_high->reg);
  *p++ = '%';
  p = stpcpy(p, s);
  p += uint64toarray_radix10(Rexr(b.xedd->op.rde) << 3 | reg, p);
  if (g_dis_high) p = DisHigh(p, -1);
  return p;
}

static char *DisNq(struct DisBuilder b, char *p) {
  return DisXmm(b, p, "mm", ModrmRm(b.xedd->op.rde));
}

static char *DisUq(struct DisBuilder b, char *p) {
  return DisXmm(b, p, "mm", ModrmRm(b.xedd->op.rde));
}

static char *DisPq(struct DisBuilder b, char *p) {
  return DisXmm(b, p, "mm", ModrmReg(b.xedd->op.rde));
}

static char *DisUdq(struct DisBuilder b, char *p) {
  return DisXmm(b, p, "xmm", ModrmRm(b.xedd->op.rde));
}

static char *DisVdq(struct DisBuilder b, char *p) {
  return DisXmm(b, p, "xmm", ModrmReg(b.xedd->op.rde));
}

static char *DisQq(struct DisBuilder b, char *p) {
  if (IsModrmRegister(b.xedd->op.rde)) {
    return DisNq(b, p);
  } else {
    return DisM(b, p);
  }
}

static char *DisEst(struct DisBuilder b, char *p) {
  p = DisRegister(p, "st");
  if (ModrmRm(b.xedd->op.rde) != 0) {
    *p++ = '(';
    *p++ = '0' + ModrmRm(b.xedd->op.rde);
    *p++ = ')';
    *p = '\0';
  }
  return p;
}

static char *DisEst1(struct DisBuilder b, char *p) {
  if (ModrmRm(b.xedd->op.rde) != 1) {
    p = DisEst(b, p);
  } else {
    *p = '\0';
  }
  return p;
}

static char *DisEssr(struct DisBuilder b, char *p) {
  if (IsModrmRegister(b.xedd->op.rde)) {
    return DisEst(b, p);
  } else {
    return DisM(b, p);
  }
}

static char *DisWps(struct DisBuilder b, char *p) {
  if (IsModrmRegister(b.xedd->op.rde)) {
    return DisUdq(b, p);
  } else {
    return DisM(b, p);
  }
}

#define DisEdr  DisM
#define DisEqp  DisEq
#define DisEsr  DisM
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
#define DisMps  DisM
#define DisMq   DisM
#define DisMqi  DisM
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

static const struct DisArg {
  char s[8];
  char *(*f)(struct DisBuilder, char *);
} kDisArgs[] = /* <sorted> */ {
    {"%Gb", DisGb},      //
    {"%Gdqp", DisGdqp},  //
    {"%Gvqp", DisGvqp},  //
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
    {"Ev", DisEvqp},     //
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
    {"M", DisM},         //
    {"Mdi", DisMdi},     //
    {"Mdq", DisMdq},     //
    {"Mdqp", DisMdqp},   //
    {"Mdr", DisMdr},     //
    {"Me", DisMe},       //
    {"Mer", DisMer},     //
    {"Mps", DisMps},     //
    {"Mq", DisMq},       //
    {"Mqi", DisMqi},     //
    {"Msr", DisMsr},     //
    {"Mw", DisMw},       //
    {"Mwi", DisMwi},     //
    {"Ob", DisOb},       //
    {"Ovqp", DisOvqp},   //
    {"Qpi", DisQpi},     //
    {"Qq", DisQq},       //
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

char *DisArg(struct DisBuilder b, char *p, const char *s) {
  char key[8];
  int c, m, l, r;
  l = 0;
  r = ARRAYLEN(kDisArgs) - 1;
  strncpy(key, s, 8);
  while (l <= r) {
    m = (l + r) >> 1;
    c = SSTRCMP8(kDisArgs[m].s, key);
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else {
      return kDisArgs[m].f(b, p);
    }
  }
  if (*s == '%') {
    p = DisRegister(p, s + 1);
  } else {
    p = stpcpy(p, s);
  }
  return p;
}
