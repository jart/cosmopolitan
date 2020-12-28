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
#include "libc/log/check.h"
#include "libc/str/str.h"
#include "third_party/zlib/zlib.h"
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
  if (strcmp(name, "BIT") == 0) {
    p = stpcpy(p, kBitOp[ModrmReg(rde)]);
  } else if (strcmp(name, "nop") == 0 && d->xedd->op.rep) {
    p = stpcpy(p, "pause");
  } else if (strcmp(name, "CALL") == 0) {
    p = stpcpy(p, "call");
  } else if (strcmp(name, "JMP") == 0) {
    p = stpcpy(p, "jmp");
  } else if (strcmp(name, "jcxz") == 0) {
    p = stpcpy(p, kJcxz[Eamode(rde)]);
    p = DisBranchTaken(d, p);
  } else if (strcmp(name, "loop") == 0 || strcmp(name, "loope") == 0 ||
             strcmp(name, "loopne") == 0) {
    p = stpcpy(p, name);
    if (Eamode(rde) != Mode(rde)) {
      *p++ = "wl"[Eamode(rde)];
      *p = '\0';
    }
    p = DisBranchTaken(d, p);
  } else if (strcmp(name, "cwtl") == 0) {
    if (Osz(rde)) name = "cbtw";
    if (Rexw(rde)) name = "cltq";
    p = stpcpy(p, name);
  } else if (strcmp(name, "cltd") == 0) {
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
    if (strcmp(name, "ALU") == 0) {
      p = stpcpy(p, kAluOp[(d->xedd->op.opcode & 070) >> 3]);
    } else if (strcmp(name, "ALU2") == 0) {
      p = stpcpy(p, kAluOp[ModrmReg(rde)]);
    } else if (strcmp(np, "WLQ") == 0) {
      notbyte = true;
      wantsuffix = true;
    } else if (strcmp(np, "CC") == 0) {
      p = stpcpy(p, kCc[d->xedd->op.opcode & 15]);
      p = DisBranchTaken(d, p);
    } else if (strcmp(np, "WQ") == 0) {
      notbyte = true;
      notlong = Eamode(rde) != XED_MODE_REAL;
      wantsuffix = true;
    } else if (strcmp(np, "LQ") == 0 || strcmp(np, "WL") == 0) {
      notbyte = true;
      wantsuffix = true;
    } else if (strcmp(np, "SD") == 0) {
      notbyte = true;
      wantsuffixsd = true;
    } else if (strcmp(np, "ABS") == 0) {
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
        if (ambiguous || Mode(rde) != XED_MODE_REAL) {
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
  char sbuf[64];
  char args[4][256];
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
