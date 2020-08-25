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
#include "tool/build/lib/case.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/modrm.h"

static const char kAluOp[8][4] = {"add", "or",  "adc", "sbb",
                                  "and", "sub", "xor", "cmp"};
static const char kBitOp[8][4] = {"rol", "ror", "rcl", "rcr",
                                  "shl", "shr", "sal", "sar"};

static int IsRepOpcode(struct DisBuilder b) {
  switch (b.xedd->op.opcode & ~1u) {
    CASE(0x6C /*INS */, return 1);
    CASE(0x6E /*OUTS*/, return 1);
    CASE(0xA4 /*MOVS*/, return 1);
    CASE(0xAA /*STOS*/, return 1);
    CASE(0xAC /*LODS*/, return 1);
    CASE(0xA6 /*CMPS*/, return 2);
    CASE(0xAE /*SCAS*/, return 2);
    default:
      return 0;
  }
}

static char *DisRepPrefix(struct DisBuilder b, char *p) {
  const char *s;
  if (b.xedd->op.rep && b.xedd->op.map == XED_ILD_MAP0) {
    switch (IsRepOpcode(b)) {
      case 0:
        break;
      case 1:
        p = stpcpy(p, "rep ");
        break;
      case 2:
        p = stpcpy(p, b.xedd->op.rep == 2 ? "repnz " : "repz ");
        break;
      default:
        break;
    }
  }
  return p;
}

static char *DisBranchTaken(struct DisBuilder b, char *p) {
  switch (b.xedd->op.hint) {
    case XED_HINT_NTAKEN:
      return stpcpy(p, ",pn");
    case XED_HINT_TAKEN:
      return stpcpy(p, ",pt");
    default:
      return p;
  }
}

static char *DisName(struct DisBuilder b, char *bp, const char *name,
                     bool ambiguous) {
  char *p, *np;
  bool notbyte, notlong, wantsuffix, wantsuffixsd;
  p = bp;
  if (b.xedd->op.lock) p = stpcpy(p, "lock ");
  p = DisRepPrefix(b, p);
  if (tinystrcmp(name, "BIT") == 0) {
    p = stpcpy(p, kBitOp[ModrmReg(b.xedd)]);
  } else if (tinystrcmp(name, "CALL") == 0) {
    p = stpcpy(p, "call");
  } else if (tinystrcmp(name, "JMP") == 0) {
    p = stpcpy(p, "jmp");
  } else if (tinystrcmp(name, "jcxz") == 0) {
    p = stpcpy(p, Asz(b.xedd) ? "jecxz" : "jrcxz");
    p = DisBranchTaken(b, p);
  } else if (tinystrcmp(name, "loop") == 0) {
    p = stpcpy(p, Asz(b.xedd) ? "loopl" : "loop");
    p = DisBranchTaken(b, p);
  } else if (tinystrcmp(name, "loope") == 0) {
    p = stpcpy(p, Asz(b.xedd) ? "loopel" : "loope");
    p = DisBranchTaken(b, p);
  } else if (tinystrcmp(name, "loopne") == 0) {
    p = stpcpy(p, Asz(b.xedd) ? "loopnel" : "loopne");
    p = DisBranchTaken(b, p);
  } else if (tinystrcmp(name, "cwtl") == 0) {
    if (Osz(b.xedd)) name = "cbtw";
    if (Rexw(b.xedd)) name = "cltq";
    p = stpcpy(p, name);
  } else if (tinystrcmp(name, "cltd") == 0) {
    if (Osz(b.xedd)) name = "cwtd";
    if (Rexw(b.xedd)) name = "cqto";
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
      p = stpcpy(p, kAluOp[ModrmReg(b.xedd)]);
    } else if (tinystrcmp(np, "WLQ") == 0) {
      notbyte = true;
      wantsuffix = true;
    } else if (tinystrcmp(np, "WQ") == 0) {
      notbyte = true;
      notlong = true;
      wantsuffix = true;
    } else if (tinystrcmp(np, "LQ") == 0 || tinystrcmp(np, "WL") == 0) {
      notbyte = true;
      wantsuffix = true;
    } else if (tinystrcmp(np, "SD") == 0) {
      notbyte = true;
      wantsuffixsd = true;
    } else if (tinystrcmp(np, "ABS") == 0) {
      if (Rexw(b.xedd)) p = stpcpy(p, "abs");
    } else if (tinystrcmp(np, "BT") == 0) {
      p = DisBranchTaken(b, p);
    }
    if (wantsuffixsd) {
      if (b.xedd->op.prefix66) {
        *p++ = 'd';
      } else {
        *p++ = 's';
      }
    } else if (wantsuffix || (ambiguous && !startswith(name, "f") &&
                              !startswith(name, "set"))) {
      if (Osz(b.xedd)) {
        *p++ = 'w';
      } else if (Rexw(b.xedd)) {
        *p++ = 'q';
      } else if (ambiguous && !notbyte && IsProbablyByteOp(b.xedd)) {
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
char *DisInst(struct DisBuilder b, char *p, const char *spec) {
  long i, n;
  char sbuf[128];
  char args[4][64];
  char *s, *name, *state;
  bool hasarg, hasmodrm, hasregister, hasmemory;
  DCHECK_LT(strlen(spec), 128);
  hasarg = false;
  hasmodrm = b.xedd->op.has_modrm;
  hasmemory = hasmodrm && !IsModrmRegister(b.xedd);
  hasregister = hasmodrm && IsModrmRegister(b.xedd);
  name = strtok_r(strcpy(sbuf, spec), " ", &state);
  for (n = 0; (s = strtok_r(NULL, " ", &state)); ++n) {
    hasarg = true;
    hasregister |= *s == '%';
    hasmemory |= *s == 'O';
    DisArg(b, args[n], s);
  }
  if (g_dis_high) p = DisHigh(p, g_dis_high->keyword);
  p = DisName(b, p, name, hasarg && !hasregister && hasmemory);
  if (g_dis_high) p = DisHigh(p, -1);
  for (i = 0; i < n; ++i) {
    if (i && args[n - i][0]) {
      *p++ = ',';
    }
    p = stpcpy(p, args[n - i - 1]);
  }
  return p;
}
