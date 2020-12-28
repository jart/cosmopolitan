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
#include "libc/assert.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "tool/build/lib/address.h"
#include "tool/build/lib/alu.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/ioports.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/string.h"
#include "tool/build/lib/throw.h"

static uint64_t ReadInt(uint8_t p[8], unsigned long w) {
  switch (w) {
    case 0:
      return Read8(p);
    case 1:
      return Read16(p);
    case 2:
      return Read32(p);
    case 3:
      return Read64(p);
    default:
      unreachable;
  }
}

static void WriteInt(uint8_t p[8], uint64_t x, unsigned long w) {
  switch (w) {
    case 0:
      Write8(p, x);
      break;
    case 1:
      Write16(p, x);
      break;
    case 2:
      Write64(p, x & 0xffffffff);
      break;
    case 3:
      Write64(p, x);
      break;
    default:
      unreachable;
  }
}

static void AddDi(struct Machine *m, uint32_t rde, uint64_t x) {
  switch (Eamode(rde)) {
    case XED_MODE_LONG:
      Write64(m->di, Read64(m->di) + x);
      return;
    case XED_MODE_LEGACY:
      Write64(m->di, (Read32(m->di) + x) & 0xffffffff);
      return;
    case XED_MODE_REAL:
      Write16(m->di, Read16(m->di) + x);
      return;
    default:
      unreachable;
  }
}

static void AddSi(struct Machine *m, uint32_t rde, uint64_t x) {
  switch (Eamode(rde)) {
    case XED_MODE_LONG:
      Write64(m->si, Read64(m->si) + x);
      return;
    case XED_MODE_LEGACY:
      Write64(m->si, (Read32(m->si) + x) & 0xffffffff);
      return;
    case XED_MODE_REAL:
      Write16(m->si, Read16(m->si) + x);
      return;
    default:
      unreachable;
  }
}

static uint64_t ReadCx(struct Machine *m, uint32_t rde) {
  switch (Eamode(rde)) {
    case XED_MODE_LONG:
      return Read64(m->cx);
    case XED_MODE_LEGACY:
      return Read32(m->cx);
    case XED_MODE_REAL:
      return Read16(m->cx);
    default:
      unreachable;
  }
}

static uint64_t SubtractCx(struct Machine *m, uint32_t rde, uint64_t x) {
  uint64_t cx;
  cx = Read64(m->cx) - x;
  if (Eamode(rde) != XED_MODE_REAL) {
    if (Eamode(rde) == XED_MODE_LEGACY) {
      cx &= 0xffffffff;
    }
    Write64(m->cx, cx);
  } else {
    cx &= 0xffff;
    Write16(m->cx, cx);
  }
  return cx;
}

static void StringOp(struct Machine *m, uint32_t rde, int op) {
  bool stop;
  void *p[2];
  unsigned n;
  int64_t sgn, v;
  uint8_t s[3][8];
  stop = false;
  n = 1 << RegLog2(rde);
  sgn = GetFlag(m->flags, FLAGS_DF) ? -1 : 1;
  do {
    if (Rep(rde) && !ReadCx(m, rde)) break;
    switch (op) {
      case STRING_CMPS:
        kAlu[ALU_SUB][RegLog2(rde)](
            ReadInt(Load(m, AddressSi(m, rde), n, s[2]), RegLog2(rde)),
            ReadInt(Load(m, AddressDi(m, rde), n, s[1]), RegLog2(rde)),
            &m->flags);
        AddDi(m, rde, sgn * n);
        AddSi(m, rde, sgn * n);
        stop = (Rep(rde) == 2 && GetFlag(m->flags, FLAGS_ZF)) ||
               (Rep(rde) == 3 && !GetFlag(m->flags, FLAGS_ZF));
        break;
      case STRING_MOVS:
        memcpy(BeginStore(m, (v = AddressDi(m, rde)), n, p, s[0]),
               Load(m, AddressSi(m, rde), n, s[1]), n);
        AddDi(m, rde, sgn * n);
        AddSi(m, rde, sgn * n);
        EndStore(m, v, n, p, s[0]);
        break;
      case STRING_STOS:
        memcpy(BeginStore(m, (v = AddressDi(m, rde)), n, p, s[0]), m->ax, n);
        AddDi(m, rde, sgn * n);
        EndStore(m, v, n, p, s[0]);
        break;
      case STRING_LODS:
        memcpy(m->ax, Load(m, AddressSi(m, rde), n, s[1]), n);
        AddSi(m, rde, sgn * n);
        break;
      case STRING_SCAS:
        kAlu[ALU_SUB][RegLog2(rde)](
            ReadInt(Load(m, AddressDi(m, rde), n, s[1]), RegLog2(rde)),
            ReadInt(m->ax, RegLog2(rde)), &m->flags);
        AddDi(m, rde, sgn * n);
        stop = (Rep(rde) == 2 && GetFlag(m->flags, FLAGS_ZF)) ||
               (Rep(rde) == 3 && !GetFlag(m->flags, FLAGS_ZF));
        break;
      case STRING_OUTS:
        OpOut(m, Read16(m->dx),
              ReadInt(Load(m, AddressSi(m, rde), n, s[1]), RegLog2(rde)));
        AddSi(m, rde, sgn * n);
        break;
      case STRING_INS:
        WriteInt(BeginStore(m, (v = AddressDi(m, rde)), n, p, s[0]),
                 OpIn(m, Read16(m->dx)), RegLog2(rde));
        AddDi(m, rde, sgn * n);
        EndStore(m, v, n, p, s[0]);
        break;
      default:
        abort();
    }
    if (Rep(rde)) {
      SubtractCx(m, rde, 1);
    } else {
      break;
    }
  } while (!stop);
}

static void RepMovsbEnhanced(struct Machine *m, uint32_t rde) {
  uint8_t *direal, *sireal;
  uint64_t diactual, siactual, cx;
  unsigned diremain, siremain, i, n;
  if ((cx = ReadCx(m, rde))) {
    do {
      diactual = AddressDi(m, rde);
      siactual = AddressSi(m, rde);
      SetWriteAddr(m, diactual, cx);
      SetReadAddr(m, siactual, cx);
      direal = ResolveAddress(m, diactual);
      sireal = ResolveAddress(m, siactual);
      diremain = 0x1000 - (diactual & 0xfff);
      siremain = 0x1000 - (siactual & 0xfff);
      n = MIN(cx, MIN(diremain, siremain));
      if ((uintptr_t)direal <= (uintptr_t)sireal ||
          (uintptr_t)direal >= (uintptr_t)sireal + n) {
        memcpy(direal, sireal, n);
      } else {
        for (i = 0; i < n; ++i) {
          direal[i] = sireal[i];
        }
      }
      AddDi(m, rde, n);
      AddSi(m, rde, n);
    } while ((cx = SubtractCx(m, rde, n)));
  }
}

static void RepStosbEnhanced(struct Machine *m, uint32_t rde) {
  uint8_t *direal;
  unsigned diremain, n;
  uint64_t divirtual, diactual, cx;
  if ((cx = ReadCx(m, rde))) {
    do {
      diactual = AddressDi(m, rde);
      SetWriteAddr(m, diactual, cx);
      direal = ResolveAddress(m, diactual);
      diremain = 0x1000 - (diactual & 0xfff);
      n = MIN(cx, diremain);
      memset(direal, Read8(m->ax), n);
      AddDi(m, rde, n);
    } while ((cx = SubtractCx(m, rde, n)));
  }
}

void OpMovs(struct Machine *m, uint32_t rde) {
  StringOp(m, rde, STRING_MOVS);
}

void OpCmps(struct Machine *m, uint32_t rde) {
  StringOp(m, rde, STRING_CMPS);
}

void OpStos(struct Machine *m, uint32_t rde) {
  StringOp(m, rde, STRING_STOS);
}

void OpLods(struct Machine *m, uint32_t rde) {
  StringOp(m, rde, STRING_LODS);
}

void OpScas(struct Machine *m, uint32_t rde) {
  StringOp(m, rde, STRING_SCAS);
}

void OpIns(struct Machine *m, uint32_t rde) {
  StringOp(m, rde, STRING_INS);
}

void OpOuts(struct Machine *m, uint32_t rde) {
  StringOp(m, rde, STRING_OUTS);
}

void OpMovsb(struct Machine *m, uint32_t rde) {
  if (Rep(rde) && !GetFlag(m->flags, FLAGS_DF)) {
    RepMovsbEnhanced(m, rde);
  } else {
    OpMovs(m, rde);
  }
}

void OpStosb(struct Machine *m, uint32_t rde) {
  if (Rep(rde) && !GetFlag(m->flags, FLAGS_DF)) {
    RepStosbEnhanced(m, rde);
  } else {
    OpStos(m, rde);
  }
}
