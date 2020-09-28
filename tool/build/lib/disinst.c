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
#include "libc/log/check.h"
#include "libc/nexgen32e/tinystrcmp.h"
#include "libc/str/str.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/high.h"
#include "tool/build/lib/modrm.h"

static const char kJcxz[3][6] = {"jcxz", "jecxz", "jrcxz"};
static const char kAluOp[8][4] = {"add", "or",  "adc", "sbb",
                                  "and", "sub", "xor", "cmp"};
static const char kBitOp[8][4] = {"rol", "ror", "rcl", "rcr",
                                  "shl", "shr", "sal", "sar"};
static const char kCc[16][3] = {"o", "no", "b", "ae", "e", "ne", "be", "a",
                                "s", "ns", "p", "np", "l", "ge", "le", "g"};

static bool IsProbablyByteOp(struct XedDecodedInst *x) {
  return !(x->op.opcode & 1);
}

static int IsRepOpcode(struct Dis *d) {
  switch (d->xedd->op.opcode & ~1) {
    case 0x6C:  // INS
      return 1;
    case 0x6E:  // OUTS
      return 1;
    case 0xA4:  // MOVS
      return 1;
    case 0xAA:  // STOS
      return 1;
    case 0xAC:  // LODS
      return 1;
    case 0xA6:  // CMPS
      return 2;
    case 0xAE:  // SCAS
      return 2;
    default:
      return 0;
  }
}

static char *DisRepPrefix(struct Dis *d, char *p) {
  const char *s;
  if (Rep(d->xedd->op.rde) && d->xedd->op.map == XED_ILD_MAP0) {
    switch (IsRepOpcode(d)) {
      case 0:
        break;
      case 1:
        p = stpcpy(p, "rep ");
        break;
      case 2:
        p = stpcpy(p, Rep(d->xedd->op.rde) == 2 ? "repnz " : "repz ");
        break;
      default:
        break;
    }
  }
  return p;
}

static char *DisBranchTaken(struct Dis *d, char *p) {
  switch (d->xedd->op.hint) {
    case XED_HINT_NTAKEN:
      return stpcpy(p, ",pn");
    case XED_HINT_TAKEN:
      return stpcpy(p, ",pt");
    default:
      return p;
  }
}

static char *DisName(struct Dis *d, char *bp, const char *name,
                     bool ambiguous) {
  char *p, *np;
  uint32_t rde;
  bool notbyte, notlong, wantsuffix, wantsuffixsd;
  p = bp;
  rde = d->xedd->op.rde;
  if (d->xedd->op.lock) p = stpcpy(p, "lock ");
  p = DisRepPrefix(d, p);
  if (tinystrcmp(name, "BIT") == 0) {
    p = stpcpy(p, kBitOp[ModrmReg(rde)]);
  } else if (tinystrcmp(name, "nop") == 0 && d->xedd->op.rep) {
    p = stpcpy(p, "pause");
  } else if (tinystrcmp(name, "CALL") == 0) {
    p = stpcpy(p, "call");
  } else if (tinystrcmp(name, "JMP") == 0) {
    p = stpcpy(p, "jmp");
  } else if (tinystrcmp(name, "jcxz") == 0) {
    p = stpcpy(p, kJcxz[Eamode(rde)]);
    p = DisBranchTaken(d, p);
  } else if (tinystrcmp(name, "loop") == 0 || tinystrcmp(name, "loope") == 0 ||
             tinystrcmp(name, "loopne") == 0) {
    p = stpcpy(p, name);
    if (Eamode(rde) != Mode(rde)) {
      *p++ = "wl"[Eamode(rde)];
      *p = '\0';
    }
    p = DisBranchTaken(d, p);
  } else if (tinystrcmp(name, "cwtl") == 0) {
    if (Osz(rde)) name = "cbtw";
    if (Rexw(rde)) name = "cltq";
    p = stpcpy(p, name);
  } else if (tinystrcmp(name, "cltd") == 0) {
    if (Osz(rde)) name = "cwtd";
    if (Rexw(rde)) name = "cqto";
    p = stpcpy(p, name);
  } else {
    notbyte = false;
    notlong = false;
    wantsuffix = false;
    wantsuffixsd = false;
    for (np = name; *np && (islower(*np) || isdigit(*np)); ++np) {
      *p++ = *np;
    }
    if (tinystrcmp(name, "ALU") == 0) {
      p = stpcpy(p, kAluOp[(d->xedd->op.opcode & 070) >> 3]);
    } else if (tinystrcmp(name, "ALU2") == 0) {
      p = stpcpy(p, kAluOp[ModrmReg(rde)]);
    } else if (tinystrcmp(np, "WLQ") == 0) {
      notbyte = true;
      wantsuffix = true;
    } else if (tinystrcmp(np, "CC") == 0) {
      p = stpcpy(p, kCc[d->xedd->op.opcode & 15]);
      p = DisBranchTaken(d, p);
    } else if (tinystrcmp(np, "WQ") == 0) {
      notbyte = true;
      notlong = Eamode(rde) != XED_MODE_REAL;
      wantsuffix = true;
    } else if (tinystrcmp(np, "LQ") == 0 || tinystrcmp(np, "WL") == 0) {
      notbyte = true;
      wantsuffix = true;
    } else if (tinystrcmp(np, "SD") == 0) {
      notbyte = true;
      wantsuffixsd = true;
    } else if (tinystrcmp(np, "ABS") == 0) {
      if (Rexw(rde)) p = stpcpy(p, "abs");
    }
    if (wantsuffixsd) {
      if (Osz(rde)) {
        *p++ = 'd';
      } else {
        *p++ = 's';
      }
    } else if (wantsuffix || (ambiguous && !startswith(name, "f") &&
                              !startswith(name, "set"))) {
      if (Osz(rde)) {
        if (Mode(rde) != XED_MODE_REAL) {
          *p++ = 'w';
        }
      } else if (Rexw(rde)) {
        *p++ = 'q';
      } else if (ambiguous && !notbyte && IsProbablyByteOp(d->xedd)) {
        *p++ = 'b';
      } else if (!notlong) {
        *p++ = 'l';
      }
    }
  }
  *p++ = ' ';
  while (p - bp < 8) *p++ = ' ';
  *p = '\0';
  return p;
}

/**
 * Disassembles instruction based on string spec.
 * @see DisSpec()
 */
char *DisInst(struct Dis *d, char *p, const char *spec) {
  long i, n;
  char sbuf[300];
  char args[4][300];
  char *s, *name, *state;
  bool hasarg, hasmodrm, hasregister, hasmemory;
  CHECK_EQ(0, (int)d->xedd->op.error);
  DCHECK_LT(strlen(spec), 128);
  hasarg = false;
  hasmodrm = d->xedd->op.has_modrm;
  hasmemory = hasmodrm && !IsModrmRegister(d->xedd->op.rde);
  hasregister = hasmodrm && IsModrmRegister(d->xedd->op.rde);
  name = strtok_r(strcpy(sbuf, spec), " ", &state);
  for (n = 0; (s = strtok_r(NULL, " ", &state)); ++n) {
    hasarg = true;
    hasregister |= *s == '%';
    hasmemory |= *s == 'O';
    CHECK_LT(DisArg(d, args[n], s) - args[n], sizeof(args[n]));
  }
  p = HighStart(p, g_high.keyword);
  p = DisName(d, p, name, hasarg && !hasregister && hasmemory);
  p = HighEnd(p);
  for (i = 0; i < n; ++i) {
    if (i && args[n - i][0]) {
      *p++ = ',';
    }
    p = stpcpy(p, args[n - i - 1]);
  }
  return p;
}
