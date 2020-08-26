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
#include "libc/bits/popcnt.h"
#include "libc/calls/calls.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/intrin/pshufd.h"
#include "libc/intrin/pshufhw.h"
#include "libc/intrin/pshuflw.h"
#include "libc/intrin/pshufw.h"
#include "libc/intrin/shufpd.h"
#include "libc/intrin/shufps.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/time.h"
#include "tool/build/lib/abp.h"
#include "tool/build/lib/alu.h"
#include "tool/build/lib/bitscan.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/cond.h"
#include "tool/build/lib/cpuid.h"
#include "tool/build/lib/cvt.h"
#include "tool/build/lib/divmul.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/fpu.h"
#include "tool/build/lib/ioports.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/sse.h"
#include "tool/build/lib/stack.h"
#include "tool/build/lib/stats.h"
#include "tool/build/lib/string.h"
#include "tool/build/lib/syscall.h"
#include "tool/build/lib/throw.h"

#define MUTATING      true
#define READONLY      false
#define UNCONDITIONAL true
#define REG           ModrmReg(m->xedd)
#define UIMM0         m->xedd->op.uimm0
#define BITS          (8 << RegLog2(m->xedd))
#define SIGN          (1ull << (BITS - 1))
#define MASK          (SIGN | (SIGN - 1))
#define SHIFTMASK     (BITS - 1)

typedef int int_v _Vector_size(16) aligned(16);
typedef long long_v _Vector_size(16) aligned(16);
typedef float float_v _Vector_size(16) aligned(16);
typedef double double_v _Vector_size(16) aligned(16);
typedef void (*machine_f)(struct Machine *);
typedef void (*machine_u8p_u8p_f)(struct Machine *, uint8_t *, uint8_t *);
typedef void (*alu_f)(struct Machine *, aluop2_f, bool);

static uint64_t AluNot(struct Machine *m, uint64_t x) {
  return ~x & MASK;
}

static uint64_t AluNeg(struct Machine *m, uint64_t x) {
  m->flags = SetFlag(m->flags, FLAGS_CF, !!(x & MASK));
  m->flags = SetFlag(m->flags, FLAGS_OF, (x & MASK) == SIGN);
  x = ~x + 1;
  m->flags = SetFlag(m->flags, FLAGS_ZF, !(x & MASK));
  m->flags = SetFlag(m->flags, FLAGS_SF, !!(x & SIGN));
  m->flags = SetLazyParityByte(m->flags, x);
  return x & MASK;
}

static uint64_t AluInc(struct Machine *m, uint64_t a) {
  uint64_t c;
  uint128_t x, z;
  a &= MASK;
  x = a;
  z = x + 1;
  c = z;
  m->flags = SetFlag(m->flags, FLAGS_ZF, !(c & MASK));
  m->flags = SetFlag(m->flags, FLAGS_SF, !!(c & SIGN));
  m->flags = SetFlag(m->flags, FLAGS_OF, !!((c ^ a) & (c ^ 1) & SIGN));
  m->flags = SetLazyParityByte(m->flags, x);
  return c & MASK;
}

static uint64_t AluDec(struct Machine *m, uint64_t a) {
  uint64_t c;
  uint128_t x, z;
  a &= MASK;
  x = a;
  z = x - 1;
  c = z;
  m->flags = SetFlag(m->flags, FLAGS_ZF, !(c & MASK));
  m->flags = SetFlag(m->flags, FLAGS_SF, !!(c & SIGN));
  m->flags = SetFlag(m->flags, FLAGS_OF, !!((c ^ a) & (a ^ 1) & SIGN));
  m->flags = SetLazyParityByte(m->flags, x);
  return c & MASK;
}

static void ImportFlags(struct Machine *m, uint64_t flags) {
  uint64_t old, mask = 0;
  mask |= 1u << FLAGS_CF;
  mask |= 1u << FLAGS_PF;
  mask |= 1u << FLAGS_AF;
  mask |= 1u << FLAGS_ZF;
  mask |= 1u << FLAGS_SF;
  mask |= 1u << FLAGS_TF;
  mask |= 1u << FLAGS_IF;
  mask |= 1u << FLAGS_DF;
  mask |= 1u << FLAGS_OF;
  mask |= 1u << FLAGS_NT;
  mask |= 1u << FLAGS_AC;
  mask |= 1u << FLAGS_ID;
  m->flags = (flags & mask) | (m->flags & ~mask);
  m->flags = SetFlag(m->flags, FLAGS_RF, false);
  m->flags = SetLazyParityByte(m->flags, !((m->flags >> FLAGS_PF) & 1));
}

static uint64_t ReadMemory(struct Machine *m, uint8_t p[8]) {
  if (Rexw(m->xedd)) {
    return Read64(p);
  } else if (!Osz(m->xedd)) {
    return Read32(p);
  } else {
    return Read16(p);
  }
}

static int64_t ReadMemorySigned(struct Machine *m, uint8_t p[8]) {
  if (Rexw(m->xedd)) {
    return (int64_t)Read64(p);
  } else if (!Osz(m->xedd)) {
    return (int32_t)Read32(p);
  } else {
    return (int16_t)Read16(p);
  }
}

static void WriteRegister(struct Machine *m, uint8_t p[8], uint64_t x) {
  if (Rexw(m->xedd)) {
    Write64(p, x);
  } else if (!Osz(m->xedd)) {
    Write64(p, x & 0xffffffff);
  } else {
    Write16(p, x);
  }
}

static void WriteMemory(struct Machine *m, uint8_t p[8], uint64_t x) {
  if (Rexw(m->xedd)) {
    Write64(p, x);
  } else if (!Osz(m->xedd)) {
    Write32(p, x);
  } else {
    Write16(p, x);
  }
}

static void WriteRegisterOrMemory(struct Machine *m, uint8_t p[8], uint64_t x) {
  if (IsModrmRegister(m->xedd)) {
    WriteRegister(m, p, x);
  } else {
    WriteMemory(m, p, x);
  }
}

static void OpLfence(struct Machine *m) {
}

static void OpMfence(struct Machine *m) {
}

static void OpSfence(struct Machine *m) {
}

static void OpClflush(struct Machine *m) {
}

static void OpWutNopEv(struct Machine *m) {
}

static void OpCmc(struct Machine *m) {
  m->flags = SetFlag(m->flags, FLAGS_CF, !GetFlag(m->flags, FLAGS_CF));
}

static void OpClc(struct Machine *m) {
  m->flags = SetFlag(m->flags, FLAGS_CF, false);
}

static void OpStc(struct Machine *m) {
  m->flags = SetFlag(m->flags, FLAGS_CF, true);
}

static void OpCli(struct Machine *m) {
  m->flags = SetFlag(m->flags, FLAGS_IF, false);
}

static void OpSti(struct Machine *m) {
  m->flags = SetFlag(m->flags, FLAGS_IF, true);
}

static void OpCld(struct Machine *m) {
  m->flags = SetFlag(m->flags, FLAGS_DF, false);
}

static void OpStd(struct Machine *m) {
  m->flags = SetFlag(m->flags, FLAGS_DF, true);
}

static void OpPushf(struct Machine *m) {
  PushOsz(m, ExportFlags(m->flags) & 0xFCFFFF);
}

static void OpPopf(struct Machine *m) {
  if (!Osz(m->xedd)) {
    ImportFlags(m, Pop64(m, 0));
  } else {
    ImportFlags(m, (m->flags & ~0xFFFFull) | Pop16(m, 0));
  }
}

static void OpLahf(struct Machine *m) {
  Write8(m->ax + 1, ExportFlags(m->flags));
}

static void OpPushFs(struct Machine *m) {
  ThrowSegmentationFault(m, m->ip);
}

static void OpPopFs(struct Machine *m) {
  ThrowSegmentationFault(m, m->ip);
}

static void OpPushGs(struct Machine *m) {
  ThrowSegmentationFault(m, m->ip);
}

static void OpPopGs(struct Machine *m) {
  ThrowSegmentationFault(m, m->ip);
}

static void OpSahf(struct Machine *m) {
  ImportFlags(m, (m->flags & ~0xFFull) | m->ax[1]);
}

static void OpEbSetCc(struct Machine *m, bool x) {
  Write8(GetModrmRegisterBytePointerWrite(m), x);
}

static void OpLeaGvqpM(struct Machine *m) {
  Write64(RegRexrReg(m), ComputeAddress(m));
}

static void PushVq(struct Machine *m, uint8_t *p) {
  if (!Osz(m->xedd)) {
    Push64(m, Read64(p));
  } else {
    Push16(m, Read16(p));
  }
}

static void PopVq(struct Machine *m, uint8_t *p) {
  if (!Osz(m->xedd)) {
    Write64(p, Pop64(m, 0));
  } else {
    Write16(p, Pop16(m, 0));
  }
}

static void OpPushEvq(struct Machine *m) {
  PushVq(m, GetModrmRegisterWordPointerReadOsz(m));
}

static void OpPushZvq(struct Machine *m) {
  PushVq(m, RegRexbSrm(m));
}

static void OpPopZvq(struct Machine *m) {
  PopVq(m, RegRexbSrm(m));
}

static void OpPopEvq(struct Machine *m) {
  PopVq(m, GetModrmRegisterWordPointerWriteOsz(m));
}

static void OpJmp(struct Machine *m) {
  m->ip += m->xedd->op.disp;
}

static void OpJmpEq(struct Machine *m) {
  m->ip = Read64(GetModrmRegisterWordPointerRead8(m));
}

static void OpJcxz(struct Machine *m) {
  uint64_t count;
  count = Read64(m->cx);
  if (Asz(m->xedd)) count &= 0xffffffff;
  if (!count) OpJmp(m);
}

static void OpLoop(struct Machine *m, bool cond) {
  uint64_t count;
  count = Read64(m->cx) - 1;
  if (Asz(m->xedd)) count &= 0xffffffff;
  Write64(m->cx, count);
  if (count && cond) m->ip += m->xedd->op.disp;
}

static void OpXlat(struct Machine *m) {
  int64_t v;
  uint8_t al;
  v = Read64(m->bx) + Read8(m->ax);
  if (Asz(m->xedd)) v &= 0xffffffff;
  SetReadAddr(m, v, 1);
  Write8(m->ax, Read8(ResolveAddress(m, v)));
}

static void OpEb(struct Machine *m, aluop1_f op) {
  uint8_t *p;
  p = GetModrmRegisterBytePointerWrite(m);
  Write8(p, op(m, Read8(p)));
}

static void OpEvqp(struct Machine *m, aluop1_f op) {
  uint8_t *p;
  p = GetModrmRegisterWordPointerWriteOszRexw(m);
  WriteRegisterOrMemory(m, p, op(m, ReadMemory(m, p)));
}

static void OpGvqpEvqp(struct Machine *m, aluop2_f op, bool write) {
  uint64_t x;
  x = op(m, ReadMemory(m, RegRexrReg(m)),
         ReadMemory(m, GetModrmRegisterWordPointerReadOszRexw(m)));
  if (write) WriteRegister(m, RegRexrReg(m), x);
}

static void OpEvqpGvqp(struct Machine *m, aluop2_f op, bool write) {
  uint8_t *p;
  uint64_t x, y;
  p = GetModrmRegisterWordPointerWriteOszRexw(m);
  x = ReadMemory(m, p);
  y = ReadMemory(m, RegRexrReg(m));
  x = op(m, x, y);
  if (write) WriteRegisterOrMemory(m, p, x);
}

static void OpXchgZvqp(struct Machine *m) {
  uint64_t x, y;
  x = Read64(m->ax);
  y = Read64(RegRexbSrm(m));
  WriteRegister(m, m->ax, y);
  WriteRegister(m, RegRexbSrm(m), x);
}

static void OpBofram(struct Machine *m) {
  if (m->xedd->op.disp) {
    m->bofram[0] = m->ip;
    m->bofram[1] = m->ip + (m->xedd->op.disp & 0xff);
  } else {
    m->bofram[0] = 0;
    m->bofram[1] = 0;
  }
}

static void OpNopEv(struct Machine *m) {
  if (m->xedd->op.modrm == 0x45) {
    OpBofram(m);
  }
}

static void OpPause(struct Machine *m) {
  sched_yield();
}

static void OpNop(struct Machine *m) {
  if (m->xedd->op.rexb) {
    OpXchgZvqp(m);
  } else if (m->xedd->op.ild_f3) {
    OpPause(m);
  }
}

static void OpXchgGbEb(struct Machine *m) {
  uint8_t *b;
  uint8_t x, y;
  b = GetModrmRegisterBytePointerWrite(m);
  x = Read8(ByteRexrReg(m));
  y = Read8(b);
  Write8(ByteRexrReg(m), y);
  Write8(b, x);
}

static void OpXchgGvqpEvqp(struct Machine *m) {
  uint8_t *p;
  uint64_t x, y;
  p = GetModrmRegisterWordPointerWriteOszRexw(m);
  x = ReadMemory(m, RegRexrReg(m));
  y = ReadMemory(m, p);
  WriteRegister(m, RegRexrReg(m), y);
  WriteRegisterOrMemory(m, p, x);
}

static void OpCmpxchgEbAlGb(struct Machine *m) {
  uint8_t *p;
  uint8_t x, y, z;
  p = GetModrmRegisterBytePointerWrite(m);
  x = Read8(m->ax);
  y = Read8(p);
  z = Read8(ByteRexrReg(m));
  Alu(0, ALU_SUB, x, y, &m->flags);
  if (GetFlag(m->flags, FLAGS_ZF)) {
    Write8(p, z);
  } else {
    Write8(m->ax, y);
  }
}

static void OpCmpxchgEvqpRaxGvqp(struct Machine *m) {
  uint8_t *p;
  uint64_t x, y, z;
  p = GetModrmRegisterWordPointerWriteOszRexw(m);
  x = ReadMemory(m, m->ax);
  y = ReadMemory(m, p);
  z = ReadMemory(m, RegRexrReg(m));
  Alu(RegLog2(m->xedd), ALU_SUB, x, y, &m->flags);
  if (GetFlag(m->flags, FLAGS_ZF)) {
    WriteRegisterOrMemory(m, p, z);
  } else {
    WriteRegister(m, m->ax, y);
  }
}

static void OpCmpxchg8b(struct Machine *m) {
  uint8_t *p;
  uint32_t d, a;
  p = GetModrmRegisterXmmPointerRead8(m);
  a = Read32(p + 0);
  d = Read32(p + 4);
  if (a == Read32(m->ax) && d == Read32(m->dx)) {
    SetFlag(m->flags, FLAGS_ZF, true);
    memcpy(p + 0, m->bx, 4);
    memcpy(p + 4, m->cx, 4);
  } else {
    SetFlag(m->flags, FLAGS_ZF, false);
    Write32(m->ax, a);
    Write32(m->dx, d);
  }
}

static void OpCmpxchg16b(struct Machine *m) {
  uint8_t *p;
  uint64_t d, a;
  p = GetModrmRegisterXmmPointerRead16(m);
  a = Read64(p + 0);
  d = Read64(p + 8);
  if (a == Read64(m->ax) && d == Read64(m->dx)) {
    SetFlag(m->flags, FLAGS_ZF, true);
    memcpy(p + 0, m->bx, 8);
    memcpy(p + 8, m->cx, 8);
  } else {
    SetFlag(m->flags, FLAGS_ZF, false);
    Write64(m->ax, a);
    Write64(m->dx, d);
  }
}

static void OpCmpxchgDxAx(struct Machine *m) {
  if (Rexw(m->xedd)) {
    OpCmpxchg16b(m);
  } else {
    OpCmpxchg8b(m);
  }
}

static uint64_t OpDoubleShift(struct Machine *m, uint64_t x, uint64_t y) {
  uint8_t b, w, W[2][2] = {{2, 3}, {1, 3}};
  return BsuDoubleShift(
      W[Osz(m->xedd)][Rexw(m->xedd)], x, y,
      m->xedd->op.opcode & 1 ? Read8(m->cx) : m->xedd->op.uimm0,
      m->xedd->op.opcode & 8, &m->flags);
}

static uint64_t OpBts(uint64_t x, uint64_t y) {
  return x | y;
}

static uint64_t OpBtr(uint64_t x, uint64_t y) {
  return x & ~y;
}

static uint64_t OpBtc(uint64_t x, uint64_t y) {
  return (x & ~y) | (~x & y);
}

static void OpBit(struct Machine *m) {
  int op;
  uint8_t *p;
  unsigned bit;
  int64_t disp;
  uint64_t v, x, y, z;
  uint8_t w, W[2][2] = {{2, 3}, {1, 3}};
  w = W[Osz(m->xedd)][Rexw(m->xedd)];
  if (m->xedd->op.opcode == 0xBA) {
    op = ModrmReg(m->xedd);
    bit = m->xedd->op.uimm0 & ((8 << w) - 1);
    disp = 0;
  } else {
    op = (m->xedd->op.opcode & 070) >> 3;
    disp = ReadMemorySigned(m, RegRexrReg(m));
    bit = disp & ((8 << w) - 1);
    disp &= -(8 << w);
    disp >>= 3;
  }
  if (IsModrmRegister(m->xedd)) {
    p = RegRexbRm(m);
  } else {
    v = ComputeAddress(m) + disp;
    if (Asz(m->xedd)) v &= 0xffffffff;
    p = ReserveAddress(m, v, 1 << w);
    if (op == 4) {
      SetReadAddr(m, v, 1 << w);
    } else {
      SetWriteAddr(m, v, 1 << w);
    }
  }
  y = 1ull << bit;
  x = ReadMemory(m, p);
  m->flags = SetFlag(m->flags, FLAGS_CF, !!(y & x));
  switch (op) {
    case 4:
      return;
    case 5:
      z = OpBts(x, y);
      break;
    case 6:
      z = OpBtr(x, y);
      break;
    case 7:
      z = OpBtc(x, y);
      break;
    default:
      OpUd(m);
  }
  WriteRegisterOrMemory(m, p, z);
}

static void OpConvert1(struct Machine *m) {
  if (Rexw(m->xedd)) {
    Write64(m->ax, Read32(m->ax) |
                       (Read32(m->ax) & 0x80000000 ? 0xffffffff00000000 : 0));
  } else if (!Osz(m->xedd)) {
    Write64(m->ax, Read16(m->ax) | (Read16(m->ax) & 0x8000 ? 0xffff0000 : 0));
  } else {
    Write16(m->ax, Read8(m->ax) | (Read8(m->ax) & 0x0080 ? 0xff00 : 0));
  }
}

static void OpConvert2(struct Machine *m) {
  if (Rexw(m->xedd)) {
    Write64(m->dx, Read64(m->ax) & 0x8000000000000000 ? 0xffffffffffffffff : 0);
  } else if (!Osz(m->xedd)) {
    Write64(m->dx, Read32(m->ax) & 0x80000000 ? 0xffffffff : 0);
  } else {
    Write16(m->dx, Read16(m->ax) & 0x8000 ? 0xffff : 0);
  }
}

static void OpBswapZvqp(struct Machine *m) {
  uint64_t x;
  x = Read64(RegRexbSrm(m));
  if (Rexw(m->xedd)) {
    Write64(
        RegRexbSrm(m),
        ((x & 0xff00000000000000) >> 070 | (x & 0x00000000000000ff) << 070 |
         (x & 0x00ff000000000000) >> 050 | (x & 0x000000000000ff00) << 050 |
         (x & 0x0000ff0000000000) >> 030 | (x & 0x0000000000ff0000) << 030 |
         (x & 0x000000ff00000000) >> 010 | (x & 0x00000000ff000000) << 010));
  } else if (!Osz(m->xedd)) {
    Write64(RegRexbSrm(m), ((x & 0xff000000) >> 030 | (x & 0x000000ff) << 030 |
                            (x & 0x00ff0000) >> 010 | (x & 0x0000ff00) << 010));
  } else {
    Write16(RegRexbSrm(m), (x & 0x00ff) << 010 | (x & 0xff00) << 010);
  }
}

static uint8_t pmovmskb(uint64_t x) {
  return (x & 0x0000000000000080) >> 007 | (x & 0x0000000000008000) >> 016 |
         (x & 0x0000000000800000) >> 025 | (x & 0x0000000080000000) >> 034 |
         (x & 0x0000008000000000) >> 043 | (x & 0x0000800000000000) >> 052 |
         (x & 0x0080000000000000) >> 061 | (x & 0x8000000000000000) >> 070;
}

static void OpPmovmskbGdqpNqUdq(struct Machine *m) {
  uint64_t bitmask;
  if (Osz(m->xedd)) {
    bitmask = pmovmskb(Read64(XmmRexbRm(m) + 8)) << 8 |
              pmovmskb(Read64(XmmRexbRm(m)));
  } else {
    bitmask = pmovmskb(Read64(MmRm(m) + 8)) << 8 | pmovmskb(Read64(MmRm(m)));
  }
  Write64(RegRexrReg(m), bitmask);
}

static void OpRdtsc(struct Machine *m) {
  uint64_t c;
#ifdef __x86_64__
  c = rdtsc();
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  c = ts.tv_sec * 1000000000 + ts.tv_nsec;
#endif
  Write64(m->ax, c & 0xffffffff);
  c >>= 32;
  Write64(m->dx, c & 0xffffffff);
}

static void OpMovEvqpSw(struct Machine *m) {
  ThrowSegmentationFault(m, m->ip);
}

static void OpMovSwEvqp(struct Machine *m) {
  ThrowSegmentationFault(m, m->ip);
}

static void OpMovEbIb(struct Machine *m) {
  Write8(GetModrmRegisterBytePointerWrite(m), m->xedd->op.uimm0);
}

static void OpMovAlOb(struct Machine *m) {
  memcpy(ResolveAddress(m, m->xedd->op.uimm0), m->ax, 1);
}

static void OpMovObAl(struct Machine *m) {
  memcpy(m->ax, ResolveAddress(m, m->xedd->op.uimm0), 1);
}

static void OpMovRaxOvqp(struct Machine *m) {
  WriteRegister(m, m->ax, ReadMemory(m, ResolveAddress(m, m->xedd->op.uimm0)));
}

static void OpMovOvqpRax(struct Machine *m) {
  WriteMemory(m, ResolveAddress(m, m->xedd->op.uimm0), Read64(m->ax));
}

static void OpMovEbGb(struct Machine *m) {
  memcpy(GetModrmRegisterBytePointerWrite(m), ByteRexrReg(m), 1);
}

static void OpMovGbEb(struct Machine *m) {
  memcpy(ByteRexrReg(m), GetModrmRegisterBytePointerRead(m), 1);
}

static void OpMovZbIb(struct Machine *m) {
  Write8(ByteRexbSrm(m), m->xedd->op.uimm0);
}

static void OpMovZvqpIvqp(struct Machine *m) {
  WriteRegister(m, RegRexbSrm(m), m->xedd->op.uimm0);
}

static void OpMovEvqpIvds(struct Machine *m) {
  WriteRegisterOrMemory(m, GetModrmRegisterWordPointerWriteOszRexw(m),
                        m->xedd->op.uimm0);
}

static void OpMovEvqpGvqp(struct Machine *m) {
  WriteRegisterOrMemory(m, GetModrmRegisterWordPointerWriteOszRexw(m),
                        ReadMemory(m, RegRexrReg(m)));
}

static void OpMovGvqpEvqp(struct Machine *m) {
  WriteRegister(m, RegRexrReg(m),
                ReadMemory(m, GetModrmRegisterWordPointerReadOszRexw(m)));
}

static void OpMovzbGvqpEb(struct Machine *m) {
  WriteRegister(m, RegRexrReg(m), Read8(GetModrmRegisterBytePointerRead(m)));
}

static void OpMovzwGvqpEw(struct Machine *m) {
  WriteRegister(m, RegRexrReg(m), Read16(GetModrmRegisterWordPointerRead2(m)));
}

static void OpMovsbGvqpEb(struct Machine *m) {
  WriteRegister(m, RegRexrReg(m),
                (int8_t)Read8(GetModrmRegisterBytePointerRead(m)));
}

static void OpMovswGvqpEw(struct Machine *m) {
  WriteRegister(m, RegRexrReg(m),
                (int16_t)Read16(GetModrmRegisterWordPointerRead2(m)));
}

static void OpMovsxdGdqpEd(struct Machine *m) {
  uint64_t x;
  uint8_t *p;
  x = (int32_t)Read32(GetModrmRegisterWordPointerRead4(m));
  if (!Rexw(m->xedd)) x &= 0xffffffff; /* wut */
  Write64(RegRexrReg(m), x);
}

static void OpMovdquVdqWdq(struct Machine *m) {
  memcpy(XmmRexrReg(m), GetModrmRegisterXmmPointerRead16(m), 16);
}

static void OpMovdquWdqVdq(struct Machine *m) {
  memcpy(GetModrmRegisterXmmPointerWrite16(m), XmmRexrReg(m), 16);
}

static void OpMovupsVpsWps(struct Machine *m) {
  OpMovdquVdqWdq(m);
}

static void OpMovupsWpsVps(struct Machine *m) {
  OpMovdquWdqVdq(m);
}

static void OpMovupdVpsWps(struct Machine *m) {
  OpMovdquVdqWdq(m);
}

static void OpMovupdWpsVps(struct Machine *m) {
  OpMovdquWdqVdq(m);
}

static void OpLddquVdqMdq(struct Machine *m) {
  OpMovdquVdqWdq(m);
}

static void OpMovdqaVdqMdq(struct Machine *m) {
  int64_t v;
  uint8_t *p;
  v = ComputeAddress(m);
  SetReadAddr(m, v, 16);
  if ((v & 15) || !(p = FindReal(m, v))) ThrowSegmentationFault(m, v);
  memcpy(XmmRexrReg(m), Abp16(p), 16);
}

static void OpMovdqaMdqVdq(struct Machine *m) {
  int64_t v;
  uint8_t *p;
  v = ComputeAddress(m);
  SetWriteAddr(m, v, 16);
  if ((v & 15) || !(p = FindReal(m, v))) ThrowSegmentationFault(m, v);
  memcpy(Abp16(p), XmmRexrReg(m), 16);
}

static void OpMovdqaVdqWdq(struct Machine *m) {
  if (IsModrmRegister(m->xedd)) {
    memcpy(XmmRexrReg(m), XmmRexbRm(m), 16);
  } else {
    OpMovdqaVdqMdq(m);
  }
}

static void OpMovdqaWdqVdq(struct Machine *m) {
  if (IsModrmRegister(m->xedd)) {
    memcpy(XmmRexbRm(m), XmmRexrReg(m), 16);
  } else {
    OpMovdqaMdqVdq(m);
  }
}

static void OpMovntiMdqpGdqp(struct Machine *m) {
  int64_t v;
  uint8_t *a;
  void *p[2];
  uint8_t n, b[8];
  v = ComputeAddress(m);
  n = Rexw(m->xedd) ? 8 : 4;
  a = BeginStore(m, v, n, p, b);
  SetWriteAddr(m, v, n);
  memcpy(p, XmmRexrReg(m), n);
  EndStore(m, v, n, p, b);
}

static void OpMovntdqMdqVdq(struct Machine *m) {
  OpMovdqaMdqVdq(m);
}

static void OpMovntpsMpsVps(struct Machine *m) {
  OpMovdqaMdqVdq(m);
}

static void OpMovntpdMpdVpd(struct Machine *m) {
  OpMovdqaMdqVdq(m);
}

static void OpMovntdqaVdqMdq(struct Machine *m) {
  OpMovdqaVdqMdq(m);
}

static void OpMovqPqQq(struct Machine *m) {
  memcpy(MmReg(m), GetModrmRegisterMmPointerRead8(m), 8);
}

static void OpMovqQqPq(struct Machine *m) {
  memcpy(GetModrmRegisterMmPointerWrite8(m), MmReg(m), 8);
}

static void OpMovqVdqEqp(struct Machine *m) {
  memcpy(XmmRexrReg(m), GetModrmRegisterWordPointerRead8(m), 8);
  memset(XmmRexrReg(m) + 8, 0, 8);
}

static void OpMovdVdqEd(struct Machine *m) {
  memset(XmmRexrReg(m), 0, 16);
  memcpy(XmmRexrReg(m), GetModrmRegisterWordPointerRead4(m), 4);
}

static void OpMovqPqEqp(struct Machine *m) {
  memcpy(MmReg(m), GetModrmRegisterWordPointerRead8(m), 8);
}

static void OpMovdPqEd(struct Machine *m) {
  memcpy(MmReg(m), GetModrmRegisterWordPointerRead4(m), 4);
  memset(MmReg(m) + 4, 0, 4);
}

static void OpMovdEdVdq(struct Machine *m) {
  if (IsModrmRegister(m->xedd)) {
    memset(RegRexbRm(m), 0, 16);
    memcpy(RegRexbRm(m), XmmRexrReg(m), 4);
  } else {
    memcpy(ComputeReserveAddressWrite4(m), XmmRexrReg(m), 4);
  }
}

static void OpMovqEqpVdq(struct Machine *m) {
  memcpy(GetModrmRegisterWordPointerWrite8(m), XmmRexrReg(m), 8);
}

static void OpMovdEdPq(struct Machine *m) {
  if (IsModrmRegister(m->xedd)) {
    memcpy(RegRexbRm(m), MmReg(m), 4);
    memset(RegRexbRm(m) + 4, 0, 4);
  } else {
    memcpy(ComputeReserveAddressWrite4(m), MmReg(m), 4);
  }
}

static void OpMovqEqpPq(struct Machine *m) {
  memcpy(GetModrmRegisterWordPointerWrite(m, 8), MmReg(m), 8);
}

static void OpMovntqMqPq(struct Machine *m) {
  memcpy(ComputeReserveAddressWrite8(m), MmReg(m), 8);
}

static void OpMovqVqWq(struct Machine *m) {
  memcpy(XmmRexrReg(m), GetModrmRegisterXmmPointerRead8(m), 8);
  memset(XmmRexrReg(m) + 8, 0, 8);
}

static void OpMovssVpsWps(struct Machine *m) {
  memcpy(XmmRexrReg(m), GetModrmRegisterXmmPointerRead4(m), 4);
}

static void OpMovssWpsVps(struct Machine *m) {
  memcpy(GetModrmRegisterXmmPointerWrite4(m), XmmRexrReg(m), 4);
}

static void OpMovsdVpsWps(struct Machine *m) {
  memcpy(XmmRexrReg(m), GetModrmRegisterXmmPointerRead16(m), 8);
}

static void OpMovsdWpsVps(struct Machine *m) {
  memcpy(GetModrmRegisterXmmPointerWrite16(m), XmmRexrReg(m), 8);
}

static void OpMaskMovDiXmmRegXmmRm(struct Machine *m) {
  void *p[2];
  uint64_t v;
  unsigned i, n;
  uint8_t *mem, b[16];
  n = Osz(m->xedd) ? 16 : 8;
  v = GetSegment() + Read64(m->di);
  if (Asz(m->xedd)) v &= 0xffffffff;
  mem = BeginStore(m, v, n, p, b);
  for (i = 0; i < n; ++i) {
    if (XmmRexbRm(m)[i] & 0x80) {
      mem[i] = XmmRexrReg(m)[i];
    }
  }
  EndStore(m, v, n, p, b);
}

static void OpMovhlpsVqUq(struct Machine *m) {
  memcpy(XmmRexrReg(m), XmmRexbRm(m) + 8, 8);
}

static void OpMovlpsVqMq(struct Machine *m) {
  memcpy(XmmRexrReg(m), ComputeReserveAddressRead8(m), 8);
}

static void OpMovlpdVqMq(struct Machine *m) {
  memcpy(XmmRexrReg(m), ComputeReserveAddressRead8(m), 8);
}

static void OpMovddupVqWq(struct Machine *m) {
  uint8_t *dst, *src;
  dst = XmmRexrReg(m);
  src = GetModrmRegisterXmmPointerRead8(m);
  memcpy(dst + 0, src, 8);
  memcpy(dst + 8, src, 8);
}

static void OpMovsldupVqWq(struct Machine *m) {
  uint8_t *dst, *src;
  dst = XmmRexrReg(m);
  src = GetModrmRegisterXmmPointerRead16(m);
  memcpy(dst + 0 + 0, src + 0, 4);
  memcpy(dst + 0 + 4, src + 0, 4);
  memcpy(dst + 8 + 0, src + 8, 4);
  memcpy(dst + 8 + 4, src + 8, 4);
}

static void OpMovlpsMqVq(struct Machine *m) {
  memcpy(ComputeReserveAddressWrite8(m), XmmRexrReg(m), 8);
}

static void OpMovlpdMqVq(struct Machine *m) {
  memcpy(ComputeReserveAddressWrite8(m), XmmRexrReg(m), 8);
}

static void OpMovlhpsVqUq(struct Machine *m) {
  memcpy(XmmRexrReg(m) + 8, XmmRexbRm(m), 8);
}

static void OpMovhpsVqMq(struct Machine *m) {
  memcpy(XmmRexrReg(m) + 8, ComputeReserveAddressRead8(m), 8);
}

static void OpMovhpdVqMq(struct Machine *m) {
  memcpy(XmmRexrReg(m) + 8, ComputeReserveAddressRead8(m), 8);
}

static void OpMovshdupVqWq(struct Machine *m) {
  uint8_t *dst, *src;
  dst = XmmRexrReg(m);
  src = GetModrmRegisterXmmPointerRead16(m);
  memcpy(dst + 0 + 0, src + 04, 4);
  memcpy(dst + 0 + 4, src + 04, 4);
  memcpy(dst + 8 + 0, src + 12, 4);
  memcpy(dst + 8 + 4, src + 12, 4);
}

static void OpMovhpsMqVq(struct Machine *m) {
  memcpy(ComputeReserveAddressRead8(m), XmmRexrReg(m) + 8, 8);
}

static void OpMovhpdMqVq(struct Machine *m) {
  memcpy(ComputeReserveAddressRead8(m), XmmRexrReg(m) + 8, 8);
}

static void OpMovqWqVq(struct Machine *m) {
  if (IsModrmRegister(m->xedd)) {
    memcpy(XmmRexbRm(m), XmmRexrReg(m), 8);
    memset(XmmRexbRm(m) + 8, 0, 8);
  } else {
    memcpy(ComputeReserveAddressWrite8(m), XmmRexrReg(m), 8);
  }
}

static void OpMovq2dqVdqNq(struct Machine *m) {
  memcpy(XmmRexrReg(m), MmRm(m), 8);
  memset(XmmRexrReg(m) + 8, 0, 8);
}

static void OpMovdq2qPqUq(struct Machine *m) {
  memcpy(MmReg(m), XmmRexbRm(m), 8);
}

static void OpMovapsVpsWps(struct Machine *m) {
  OpMovdqaVdqWdq(m);
}

static void OpMovapdVpdWpd(struct Machine *m) {
  OpMovdqaVdqWdq(m);
}

static void OpMovapsWpsVps(struct Machine *m) {
  OpMovdqaWdqVdq(m);
}

static void OpMovapdWpdVpd(struct Machine *m) {
  OpMovdqaWdqVdq(m);
}

static void OpMovWpsVps(struct Machine *m) {
  uint8_t *p, *r;
  switch (Rep(m->xedd) | Osz(m->xedd)) {
    case 0:
      OpMovupsWpsVps(m);
      break;
    case 1:
      OpMovupdWpsVps(m);
      break;
    case 2:
      OpMovsdWpsVps(m);
      break;
    case 3:
      OpMovssWpsVps(m);
      break;
    default:
      unreachable;
  }
}

static void OpMov0f28(struct Machine *m) {
  if (!Osz(m->xedd)) {
    OpMovapsVpsWps(m);
  } else {
    OpMovapdVpdWpd(m);
  }
}

static void OpMov0f6e(struct Machine *m) {
  if (Osz(m->xedd)) {
    if (Rexw(m->xedd)) {
      OpMovqVdqEqp(m);
    } else {
      OpMovdVdqEd(m);
    }
  } else {
    if (Rexw(m->xedd)) {
      OpMovqPqEqp(m);
    } else {
      OpMovdPqEd(m);
    }
  }
}

static void OpMov0f6f(struct Machine *m) {
  if (Osz(m->xedd)) {
    OpMovdqaVdqWdq(m);
  } else if (m->xedd->op.ild_f3) {
    OpMovdquVdqWdq(m);
  } else {
    OpMovqPqQq(m);
  }
}

static void OpMov0fE7(struct Machine *m) {
  if (!Osz(m->xedd)) {
    OpMovntqMqPq(m);
  } else {
    OpMovntdqMdqVdq(m);
  }
}

static void OpMov0f7e(struct Machine *m) {
  if (m->xedd->op.ild_f3) {
    OpMovqVqWq(m);
  } else if (Osz(m->xedd)) {
    if (Rexw(m->xedd)) {
      OpMovqEqpVdq(m);
    } else {
      OpMovdEdVdq(m);
    }
  } else {
    if (Rexw(m->xedd)) {
      OpMovqEqpPq(m);
    } else {
      OpMovdEdPq(m);
    }
  }
}

static void OpMov0f7f(struct Machine *m) {
  if (m->xedd->op.ild_f3) {
    OpMovdquWdqVdq(m);
  } else if (Osz(m->xedd)) {
    OpMovdqaWdqVdq(m);
  } else {
    OpMovqQqPq(m);
  }
}

static void OpMov0f10(struct Machine *m) {
  uint8_t *p, *r;
  switch (Rep(m->xedd) | Osz(m->xedd)) {
    case 0:
      OpMovupsVpsWps(m);
      break;
    case 1:
      OpMovupdVpsWps(m);
      break;
    case 2:
      OpMovsdVpsWps(m);
      break;
    case 3:
      OpMovssVpsWps(m);
      break;
    default:
      unreachable;
  }
}

static void OpMov0f29(struct Machine *m) {
  if (!Osz(m->xedd)) {
    OpMovapsWpsVps(m);
  } else {
    OpMovapdWpdVpd(m);
  }
}

static void OpMov0f2b(struct Machine *m) {
  if (!Osz(m->xedd)) {
    OpMovntpsMpsVps(m);
  } else {
    OpMovntpdMpdVpd(m);
  }
}

static void OpMov0f12(struct Machine *m) {
  switch (Rep(m->xedd) | Osz(m->xedd)) {
    case 0:
      if (IsModrmRegister(m->xedd)) {
        OpMovhlpsVqUq(m);
      } else {
        OpMovlpsVqMq(m);
      }
      break;
    case 1:
      OpMovlpdVqMq(m);
      break;
    case 2:
      OpMovddupVqWq(m);
      break;
    case 3:
      OpMovsldupVqWq(m);
      break;
    default:
      unreachable;
  }
}

static void OpMov0f13(struct Machine *m) {
  if (Osz(m->xedd)) {
    OpMovlpdMqVq(m);
  } else {
    OpMovlpsMqVq(m);
  }
}

static void OpMov0f16(struct Machine *m) {
  switch (Rep(m->xedd) | Osz(m->xedd)) {
    case 0:
      if (IsModrmRegister(m->xedd)) {
        OpMovlhpsVqUq(m);
      } else {
        OpMovhpsVqMq(m);
      }
      break;
    case 1:
      OpMovhpdVqMq(m);
      break;
    case 3:
      OpMovshdupVqWq(m);
      break;
    default:
      OpUd(m);
      break;
  }
}

static void OpMov0f17(struct Machine *m) {
  if (Osz(m->xedd)) {
    OpMovhpdMqVq(m);
  } else {
    OpMovhpsMqVq(m);
  }
}

static void OpMov0fD6(struct Machine *m) {
  if (m->xedd->op.ild_f3) {
    OpMovq2dqVdqNq(m);
  } else if (m->xedd->op.ild_f2) {
    OpMovdq2qPqUq(m);
  } else if (Osz(m->xedd)) {
    OpMovqWqVq(m);
  } else {
    OpUd(m);
  }
}

static void OpUnpcklpsd(struct Machine *m) {
  uint8_t *a, *b;
  a = XmmRexrReg(m);
  b = GetModrmRegisterXmmPointerRead8(m);
  if (Osz(m->xedd)) {
    memcpy(a + 8, b, 8);
  } else {
    memcpy(a + 4 * 3, b + 4, 4);
    memcpy(a + 4 * 2, a + 4, 4);
    memcpy(a + 4 * 1, b + 0, 4);
  }
}

static void OpUnpckhpsd(struct Machine *m) {
  uint8_t *a, *b;
  a = XmmRexrReg(m);
  b = GetModrmRegisterXmmPointerRead16(m);
  if (Osz(m->xedd)) {
    memcpy(a + 0, b + 8, 8);
    memcpy(a + 8, b + 8, 8);
  } else {
    memcpy(a + 4 * 0, a + 4 * 2, 4);
    memcpy(a + 4 * 1, b + 4 * 2, 4);
    memcpy(a + 4 * 2, a + 4 * 3, 4);
    memcpy(a + 4 * 3, b + 4 * 3, 4);
  }
}

static void OpPextrwGdqpUdqIb(struct Machine *m) {
  uint8_t i;
  i = m->xedd->op.uimm0;
  i &= Osz(m->xedd) ? 7 : 3;
  Write16(RegRexrReg(m), Read16(XmmRexbRm(m) + i * 2));
}

static void OpPinsrwVdqEwIb(struct Machine *m) {
  uint8_t i;
  i = m->xedd->op.uimm0;
  i &= Osz(m->xedd) ? 7 : 3;
  Write16(XmmRexrReg(m) + i * 2, Read16(GetModrmRegisterWordPointerRead2(m)));
}

static void OpShuffle(struct Machine *m) {
  int16_t q16[4];
  int16_t x16[8];
  int32_t x32[4];
  switch (Rep(m->xedd) | Osz(m->xedd)) {
    case 0:
      memcpy(q16, GetModrmRegisterXmmPointerRead8(m), 8);
      (pshufw)(q16, q16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m), q16, 8);
      break;
    case 1:
      memcpy(x32, GetModrmRegisterXmmPointerRead16(m), 16);
      (pshufd)(x32, x32, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m), x32, 16);
      break;
    case 2:
      memcpy(x16, GetModrmRegisterXmmPointerRead16(m), 16);
      (pshuflw)(x16, x16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m), x16, 16);
      break;
    case 3:
      memcpy(x16, GetModrmRegisterXmmPointerRead16(m), 16);
      (pshufhw)(x16, x16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m), x16, 16);
      break;
    default:
      unreachable;
  }
}

static void OpShufpsd(struct Machine *m) {
  float s[4];
  double d[2];
  if (Osz(m->xedd)) {
    memcpy(d, GetModrmRegisterXmmPointerRead16(m), 16);
    (shufpd)(d, d, m->xedd->op.uimm0);
    memcpy(XmmRexrReg(m), d, 16);
  } else {
    memcpy(s, GetModrmRegisterXmmPointerRead16(m), 16);
    (shufps)(s, s, m->xedd->op.uimm0);
    memcpy(XmmRexrReg(m), s, 16);
  }
}

static void OpSqrtpsd(struct Machine *m) {
  long i;
  float_v xf;
  double_v xd;
  switch (Rep(m->xedd) | Osz(m->xedd)) {
    case 0:
      memcpy(&xf, GetModrmRegisterXmmPointerRead16(m), 16);
      for (i = 0; i < 4; ++i) xf[i] = sqrtf(xf[i]);
      memcpy(XmmRexrReg(m), &xf, 16);
      break;
    case 1:
      memcpy(&xd, GetModrmRegisterXmmPointerRead16(m), 16);
      for (i = 0; i < 2; ++i) xd[i] = sqrt(xd[i]);
      memcpy(XmmRexrReg(m), &xd, 16);
      break;
    case 2:
      memcpy(&xd, GetModrmRegisterXmmPointerRead8(m), 8);
      xd[0] = sqrt(xd[0]);
      memcpy(XmmRexrReg(m), &xd, 8);
      break;
    case 3:
      memcpy(&xf, GetModrmRegisterXmmPointerRead4(m), 4);
      xf[0] = sqrtf(xf[0]);
      memcpy(XmmRexrReg(m), &xf, 4);
      break;
    default:
      unreachable;
  }
}

static void OpRsqrtps(struct Machine *m) {
  unsigned i;
  float_v x;
  if (!m->xedd->op.ild_f3) {
    memcpy(&x, GetModrmRegisterXmmPointerRead16(m), 16);
    for (i = 0; i < 4; ++i) x[i] = 1.f / sqrtf(x[i]);
    memcpy(XmmRexrReg(m), &x, 16);
  } else {
    memcpy(&x, GetModrmRegisterXmmPointerRead4(m), 4);
    x[0] = 1.f / sqrtf(x[0]);
    memcpy(XmmRexrReg(m), &x, 4);
  }
}

static void OpRcpps(struct Machine *m) {
  int i;
  float_v x;
  if (!m->xedd->op.ild_f3) {
    memcpy(&x, GetModrmRegisterXmmPointerRead16(m), 16);
    for (i = 0; i < 4; ++i) x[i] = 1.f / x[i];
    memcpy(XmmRexrReg(m), &x, 16);
  } else {
    memcpy(&x, GetModrmRegisterXmmPointerRead4(m), 4);
    x[0] = 1.f / x[0];
    memcpy(XmmRexrReg(m), &x, 4);
  }
}

static void OpVpsdWpsd(struct Machine *m,
                       float_v opf(struct Machine *, float_v, float_v),
                       double_v opd(struct Machine *, double_v, double_v),
                       bool isfloat, bool isdouble) {
  float_v xf, yf;
  double_v xd, yd;
  if (isfloat) {
    memcpy(&xf, XmmRexrReg(m), 16);
    memcpy(&yf, GetModrmRegisterXmmPointerRead16(m), 16);
    xf = opf(m, xf, yf);
    memcpy(XmmRexrReg(m), &xf, 16);
  } else if (isdouble) {
    memcpy(&xd, XmmRexrReg(m), 16);
    memcpy(&yd, GetModrmRegisterXmmPointerRead16(m), 16);
    xd = opd(m, xd, yd);
    memcpy(XmmRexrReg(m), &xd, 16);
  } else {
    OpUd(m);
  }
}

static void OpVpsdWpsd66(struct Machine *m,
                         float_v opf(struct Machine *, float_v, float_v),
                         double_v opd(struct Machine *, double_v, double_v)) {
  OpVpsdWpsd(m, opf, opd, !Osz(m->xedd), Osz(m->xedd));
}

static void OpVpsdWpsd66f2(struct Machine *m,
                           float_v opf(struct Machine *, float_v, float_v),
                           double_v opd(struct Machine *, double_v, double_v)) {
  OpVpsdWpsd(m, opf, opd, m->xedd->op.ild_f2, Osz(m->xedd));
}

static void OpVspsdWspsd(struct Machine *m,
                         float_v opf(struct Machine *, float_v, float_v),
                         double_v opd(struct Machine *, double_v, double_v)) {
  float_v xf, yf;
  double_v xd, yd;
  switch (Rep(m->xedd) | Osz(m->xedd)) {
    case 0:
      memcpy(&yf, GetModrmRegisterXmmPointerRead16(m), 16);
      memcpy(&xf, XmmRexrReg(m), 16);
      xf = opf(m, xf, yf);
      memcpy(XmmRexrReg(m), &xf, 16);
      break;
    case 1:
      memcpy(&yd, GetModrmRegisterXmmPointerRead16(m), 16);
      memcpy(&xd, XmmRexrReg(m), 16);
      xd = opd(m, xd, yd);
      memcpy(XmmRexrReg(m), &xd, 16);
      break;
    case 2:
      memcpy(&yd, GetModrmRegisterXmmPointerRead8(m), 8);
      memcpy(&xd, XmmRexrReg(m), 8);
      xd = opd(m, xd, yd);
      memcpy(XmmRexrReg(m), &xd, 8);
      break;
    case 3:
      memcpy(&yf, GetModrmRegisterXmmPointerRead4(m), 4);
      memcpy(&xf, XmmRexrReg(m), 4);
      xf = opf(m, xf, yf);
      memcpy(XmmRexrReg(m), &xf, 4);
      break;
    default:
      unreachable;
  }
}

static void OpComissVsWs(struct Machine *m) {
  float xf, yf;
  double xd, yd;
  uint8_t zf, cf, pf, ie;
  if (!Osz(m->xedd)) {
    memcpy(&xf, XmmRexrReg(m), 4);
    memcpy(&yf, GetModrmRegisterXmmPointerRead4(m), 4);
    if (!isnan(xf) && !isnan(yf)) {
      zf = xf == yf;
      cf = xf < yf;
      pf = false;
      ie = false;
    } else {
      zf = cf = pf = ie = true;
    }
  } else {
    memcpy(&xd, XmmRexrReg(m), 8);
    memcpy(&yd, GetModrmRegisterXmmPointerRead8(m), 8);
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

static float_v OpAddps(struct Machine *m, float_v x, float_v y) {
  return x + y;
}

static double_v OpAddpd(struct Machine *m, double_v x, double_v y) {
  return x + y;
}

static float_v OpMulps(struct Machine *m, float_v x, float_v y) {
  return x * y;
}

static double_v OpMulpd(struct Machine *m, double_v x, double_v y) {
  return x * y;
}

static float_v OpSubps(struct Machine *m, float_v x, float_v y) {
  return x - y;
}

static double_v OpSubpd(struct Machine *m, double_v x, double_v y) {
  return x - y;
}

static float_v OpDivps(struct Machine *m, float_v x, float_v y) {
  return x / y;
}

static double_v OpDivpd(struct Machine *m, double_v x, double_v y) {
  return x / y;
}

static float_v OpAndps(struct Machine *m, float_v x, float_v y) {
  return (float_v)((int_v)x & (int_v)y);
}

static double_v OpAndpd(struct Machine *m, double_v x, double_v y) {
  return (double_v)((long_v)x & (long_v)y);
}

static float_v OpAndnps(struct Machine *m, float_v x, float_v y) {
  return (float_v)(~(int_v)x & (int_v)y);
}

static double_v OpAndnpd(struct Machine *m, double_v x, double_v y) {
  return (double_v)(~(long_v)x & (long_v)y);
}

static float_v OpOrps(struct Machine *m, float_v x, float_v y) {
  return (float_v)((int_v)x | (int_v)y);
}

static double_v OpOrpd(struct Machine *m, double_v x, double_v y) {
  return (double_v)((long_v)x | (long_v)y);
}

static float_v OpXorps(struct Machine *m, float_v x, float_v y) {
  return (float_v)((int_v)x ^ (int_v)y);
}

static double_v OpXorpd(struct Machine *m, double_v x, double_v y) {
  return (double_v)((long_v)x ^ (long_v)y);
}

static double_v OpHaddpd(struct Machine *m, double_v x, double_v y) {
  return (double_v){x[0] + x[1], y[0] + y[1]};
}

static float_v OpHaddps(struct Machine *m, float_v x, float_v y) {
  return (float_v){x[0] + x[1], x[2] + x[3], y[0] + y[1], y[2] + y[3]};
}

static double_v OpHsubpd(struct Machine *m, double_v x, double_v y) {
  return (double_v){x[0] - x[1], y[0] - y[1]};
}

static float_v OpHsubps(struct Machine *m, float_v x, float_v y) {
  return (float_v){x[0] - x[1], x[2] - x[3], y[0] - y[1], y[2] - y[3]};
}

static double_v OpAddsubpd(struct Machine *m, double_v x, double_v y) {
  return (double_v){x[0] - y[0], x[1] + y[1]};
}

static float_v OpAddsubps(struct Machine *m, float_v x, float_v y) {
  return (float_v){x[0] - y[0], x[1] + y[1], x[2] - y[2], x[3] + y[3]};
}

static float_v OpMinps(struct Machine *m, float_v x, float_v y) {
  int i;
  for (i = 0; i < 4; ++i) {
    x[i] = MIN(x[i], y[i]);
  }
  return x;
}

static double_v OpMinpd(struct Machine *m, double_v x, double_v y) {
  int i;
  for (i = 0; i < 4; ++i) {
    x[i] = MIN(x[i], y[i]);
  }
  return x;
}

static float_v OpMaxps(struct Machine *m, float_v x, float_v y) {
  int i;
  for (i = 0; i < 4; ++i) {
    x[i] = MAX(x[i], y[i]);
  }
  return x;
}

static double_v OpMaxpd(struct Machine *m, double_v x, double_v y) {
  int i;
  for (i = 0; i < 4; ++i) {
    x[i] = MAX(x[i], y[i]);
  }
  return x;
}

static float_v OpCmpps(struct Machine *m, float_v x, float_v y) {
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
      OpUd(m);
  }
}

static double_v OpCmppd(struct Machine *m, double_v x, double_v y) {
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
      OpUd(m);
  }
}

static void OpAddpsd(struct Machine *m) {
  OpVspsdWspsd(m, OpAddps, OpAddpd);
}

static void OpMulpsd(struct Machine *m) {
  OpVspsdWspsd(m, OpMulps, OpMulpd);
}

static void OpSubpsd(struct Machine *m) {
  OpVspsdWspsd(m, OpSubps, OpSubpd);
}

static void OpDivpsd(struct Machine *m) {
  OpVspsdWspsd(m, OpDivps, OpDivpd);
}

static void OpMinpsd(struct Machine *m) {
  OpVspsdWspsd(m, OpMinps, OpMinpd);
}

static void OpMaxpsd(struct Machine *m) {
  OpVspsdWspsd(m, OpMaxps, OpMaxpd);
}

static void OpCmppsd(struct Machine *m) {
  OpVspsdWspsd(m, OpCmpps, OpCmppd);
}

static void OpAndpsd(struct Machine *m) {
  OpVpsdWpsd66(m, OpAndps, OpAndpd);
}

static void OpAndnpsd(struct Machine *m) {
  OpVpsdWpsd66(m, OpAndnps, OpAndnpd);
}

static void OpOrpsd(struct Machine *m) {
  OpVpsdWpsd66(m, OpOrps, OpOrpd);
}

static void OpXorpsd(struct Machine *m) {
  OpVpsdWpsd66(m, OpXorps, OpXorpd);
}

static void OpHaddpsd(struct Machine *m) {
  OpVpsdWpsd66f2(m, OpHaddps, OpHaddpd);
}

static void OpHsubpsd(struct Machine *m) {
  OpVpsdWpsd66f2(m, OpHsubps, OpHsubpd);
}

static void OpAddsubpsd(struct Machine *m) {
  OpVpsdWpsd66f2(m, OpAddsubps, OpAddsubpd);
}

static void OpAluw(struct Machine *m,
                   int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h) {
  int64_t v;
  void *p[2];
  uint64_t x, y, z;
  uint8_t w, *a, *b, *c, split[8];
  w = RegLog2(m->xedd);
  if (ModrmMod(m->xedd) != 0b11) {
    v = ComputeAddress(m);
    if (h & ALU_FLIP) {
      a = Load(m, v, 1 << w, split);
    } else {
      a = BeginLoadStore(m, v, 1 << w, p, split);
    }
  } else {
    v = 0;
    a = RegRexbRm(m);
  }
  b = RegRexrReg(m);
  if (h & ALU_FLIP) {
    c = b;
    b = a;
    a = c;
  }
  y = Read64(b);
  x = Read64(a);
  z = f(w, h, x, y, &m->flags);
  switch (w) {
    case 0:
      Write8(a, z);
      break;
    case 1:
      Write16(a, z);
      break;
    case 2:
    case 3:
      Write64(a, z);
      break;
    default:
      unreachable;
  }
  EndStore(m, v, 1 << w, p, split);
  if (h & ALU_XCHG) {
    switch (w) {
      case 0:
        Write8(b, x);
        break;
      case 1:
        Write16(b, x);
        break;
      case 2:
      case 3:
        Write64(b, x);
        break;
      default:
        unreachable;
    }
  }
}

static void OpAlub(struct Machine *m,
                   int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h) {
  int64_t v;
  uint64_t x, y, z;
  uint8_t *a, *b, *c;
  if (!m->xedd->op.has_modrm || IsModrmRegister(m->xedd)) {
    a = ByteRexbRm(m);
  } else {
    v = ComputeAddress(m);
    a = ResolveAddress(m, v);
    if (h & ALU_FLIP) {
      SetReadAddr(m, v, 1);
    } else {
      SetWriteAddr(m, v, 1);
    }
  }
  b = ByteRexrReg(m);
  if (h & ALU_FLIP) {
    c = b;
    b = a;
    a = c;
  }
  y = Read8(b);
  x = Read8(a);
  z = f(0, h, x, y, &m->flags);
  Write8(a, z);
  if (h & ALU_XCHG) Write8(b, x);
}

static void OpAluwi(struct Machine *m,
                    int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h,
                    uint64_t yimm) {
  int64_t v;
  void *p[2];
  uint64_t x, y, z;
  uint8_t w, *a, *c, split[8];
  w = RegLog2(m->xedd);
  if (m->xedd->op.has_modrm && ModrmMod(m->xedd) != 0b11) {
    v = ComputeAddress(m);
    a = BeginLoadStore(m, v, 1 << w, p, split);
  } else {
    v = 0;
    a = RegRexbRm(m);
  }
  y = yimm;
  x = Read64(a);
  z = f(w, h, x, y, &m->flags);
  switch (w) {
    case 0:
      Write8(a, z);
      break;
    case 1:
      Write16(a, z);
      break;
    case 2:
    case 3:
      Write64(a, z);
      break;
    default:
      unreachable;
  }
  EndStore(m, v, 1 << w, p, split);
}

static void OpAlubi(struct Machine *m,
                    int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h,
                    uint64_t yimm) {
  int64_t v;
  uint64_t x, y, z;
  uint8_t *a, *b, *c;
  if (!m->xedd->op.has_modrm || IsModrmRegister(m->xedd)) {
    a = ByteRexbRm(m);
  } else {
    v = ComputeAddress(m);
    a = ResolveAddress(m, v);
    SetWriteAddr(m, v, 1);
  }
  y = yimm;
  b = NULL;
  x = Read8(a);
  z = f(0, h, x, y, &m->flags);
  Write8(a, z);
}

static void ExecuteInstructionMap0(struct Machine *m) {
  switch (m->xedd->op.opcode) {
    CASE(/*0120 0x50*/ 0b01010000 ... 0x57, OpPushZvq(m));
    CASE(/*0130 0x58*/ 0b01011000 ... 0x5f, OpPopZvq(m));
    CASE(/*0143 0x63*/ 0b01100011, OpMovsxdGdqpEd(m));
    CASE(/*0150 0x68*/ 0b01101000, PushOsz(m, m->xedd->op.uimm0));
    CASE(/*0151 0x69*/ 0b01101001, OpImulGvqpEvqpImm(m));
    CASE(/*0152 0x6A*/ 0b01101010, PushOsz(m, m->xedd->op.uimm0));
    CASE(/*0153 0x6B*/ 0b01101011, OpImulGvqpEvqpImm(m));
    CASE(/*0154 0x6C*/ 0b01101100, OpString(m, STRING_INS));
    CASE(/*0155 0x6D*/ 0b01101101, OpString(m, STRING_INS));
    CASE(/*0156 0x6E*/ 0b01101110, OpString(m, STRING_OUTS));
    CASE(/*0157 0x6F*/ 0b01101111, OpString(m, STRING_OUTS));
    CASE(/*0160 0x70*/ 0b01110000, if (GetCond(m, 0x0)) OpJmp(m));
    CASE(/*0161 0x71*/ 0b01110001, if (GetCond(m, 0x1)) OpJmp(m));
    CASE(/*0162 0x72*/ 0b01110010, if (GetCond(m, 0x2)) OpJmp(m));
    CASE(/*0163 0x73*/ 0b01110011, if (GetCond(m, 0x3)) OpJmp(m));
    CASE(/*0164 0x74*/ 0b01110100, if (GetCond(m, 0x4)) OpJmp(m));
    CASE(/*0165 0x75*/ 0b01110101, if (GetCond(m, 0x5)) OpJmp(m));
    CASE(/*0166 0x76*/ 0b01110110, if (GetCond(m, 0x6)) OpJmp(m));
    CASE(/*0167 0x77*/ 0b01110111, if (GetCond(m, 0x7)) OpJmp(m));
    CASE(/*0170 0x78*/ 0b01111000, if (GetCond(m, 0x8)) OpJmp(m));
    CASE(/*0171 0x79*/ 0b01111001, if (GetCond(m, 0x9)) OpJmp(m));
    CASE(/*0172 0x7A*/ 0b01111010, if (GetCond(m, 0xa)) OpJmp(m));
    CASE(/*0173 0x7B*/ 0b01111011, if (GetCond(m, 0xb)) OpJmp(m));
    CASE(/*0174 0x7C*/ 0b01111100, if (GetCond(m, 0xc)) OpJmp(m));
    CASE(/*0175 0x7D*/ 0b01111101, if (GetCond(m, 0xd)) OpJmp(m));
    CASE(/*0176 0x7E*/ 0b01111110, if (GetCond(m, 0xe)) OpJmp(m));
    CASE(/*0177 0x7F*/ 0b01111111, if (GetCond(m, 0xf)) OpJmp(m));
    CASE(/*0200 0x80*/ 0b10000000, OpAlubi(m, Alu, ModrmReg(m->xedd), UIMM0));
    CASE(/*0201 0x81*/ 0b10000001, OpAluwi(m, Alu, ModrmReg(m->xedd), UIMM0));
    CASE(/*0202 0x82*/ 0b10000010, OpAlubi(m, Alu, ModrmReg(m->xedd), UIMM0));
    CASE(/*0203 0x83*/ 0b10000011, OpAluwi(m, Alu, ModrmReg(m->xedd), UIMM0));
    CASE(/*0204 0x84*/ 0b10000100, OpAlub(m, Alu, ALU_TEST | ALU_AND));
    CASE(/*0205 0x85*/ 0b10000101, OpAluw(m, Alu, ALU_TEST | ALU_AND));
    CASE(/*0206 0x86*/ 0b10000110, OpXchgGbEb(m));
    CASE(/*0207 0x87*/ 0b10000111, OpXchgGvqpEvqp(m));
    CASE(/*0210 0x88*/ 0b10001000, OpMovEbGb(m));
    CASE(/*0211 0x89*/ 0b10001001, OpMovEvqpGvqp(m));
    CASE(/*0212 0x8A*/ 0b10001010, OpMovGbEb(m));
    CASE(/*0213 0x8B*/ 0b10001011, OpMovGvqpEvqp(m));
    CASE(/*0214 0x8C*/ 0b10001100, OpMovEvqpSw(m));
    CASE(/*0215 0x8D*/ 0b10001101, OpLeaGvqpM(m));
    CASE(/*0216 0x8E*/ 0b10001110, OpMovSwEvqp(m));
    CASE(/*0217 0x8F*/ 0b10001111, OpPopEvq(m));
    CASE(/*0220 0x90*/ 0b10010000, OpNop(m));
    CASE(/*0221 0x91*/ 0b10010001 ... 0x97, OpXchgZvqp(m));
    CASE(/*0230 0x98*/ 0b10011000, OpConvert1(m));
    CASE(/*0231 0x99*/ 0b10011001, OpConvert2(m));
    CASE(/*0234 0x9C*/ 0b10011100, OpPushf(m));
    CASE(/*0235 0x9D*/ 0b10011101, OpPopf(m));
    CASE(/*0236 0x9E*/ 0b10011110, OpSahf(m));
    CASE(/*0237 0x9F*/ 0b10011111, OpLahf(m));
    CASE(/*0233 0x9b*/ 0b10011011, OpFwait(m));
    CASE(/*0240 0xA0*/ 0b10100000, OpMovAlOb(m));
    CASE(/*0241 0xA1*/ 0b10100001, OpMovRaxOvqp(m));
    CASE(/*0242 0xA2*/ 0b10100010, OpMovObAl(m));
    CASE(/*0243 0xA3*/ 0b10100011, OpMovOvqpRax(m));
    CASE(/*0244 0xA4*/ 0b10100100, OpMovsb(m));
    CASE(/*0245 0xA5*/ 0b10100101, OpString(m, STRING_MOVS));
    CASE(/*0246 0xA6*/ 0b10100110, OpString(m, STRING_CMPS));
    CASE(/*0247 0xA7*/ 0b10100111, OpString(m, STRING_CMPS));
    CASE(/*0250 0xA8*/ 0b10101000, OpAlubi(m, Alu, ALU_TEST | ALU_AND, UIMM0));
    CASE(/*0251 0xA9*/ 0b10101001, OpAluwi(m, Alu, ALU_TEST | ALU_AND, UIMM0));
    CASE(/*0252 0xAA*/ 0b10101010, OpStosb(m));
    CASE(/*0253 0xAB*/ 0b10101011, OpString(m, STRING_STOS));
    CASE(/*0254 0xAC*/ 0b10101100, OpString(m, STRING_LODS));
    CASE(/*0255 0xAD*/ 0b10101101, OpString(m, STRING_LODS));
    CASE(/*0256 0xAE*/ 0b10101110, OpString(m, STRING_SCAS));
    CASE(/*0257 0xAF*/ 0b10101111, OpString(m, STRING_SCAS));
    CASE(/*0260 0xB0*/ 0b10110000 ... 0b10110111, OpMovZbIb(m));
    CASE(/*0270 0xB8*/ 0b10111000 ... 0b10111111, OpMovZvqpIvqp(m));
    CASE(/*0300 0xC0*/ 0b11000000, OpAlubi(m, Bsu, ModrmReg(m->xedd), UIMM0));
    CASE(/*0301 0xC1*/ 0b11000001, OpAluwi(m, Bsu, ModrmReg(m->xedd), UIMM0));
    CASE(/*0302 0xC2*/ 0b11000010, OpRet(m, m->xedd->op.uimm0));
    CASE(/*0303 0xC3*/ 0b11000011, OpRet(m, 0));
    CASE(/*0306 0xC6*/ 0b11000110, OpMovEbIb(m));
    CASE(/*0307 0xC7*/ 0b11000111, OpMovEvqpIvds(m));
    CASE(/*0311 0xC9*/ 0b11001001, OpLeave(m));
    CASE(/*0314 0xCC*/ 0b11001100, OpInterrupt(m, 3));
    CASE(/*0315 0xCD*/ 0b11001101, OpInterrupt(m, m->xedd->op.uimm0));
    CASE(/*0320 0xD0*/ 0b11010000, OpAlubi(m, Bsu, ModrmReg(m->xedd), 1));
    CASE(/*0321 0xD1*/ 0b11010001, OpAluwi(m, Bsu, ModrmReg(m->xedd), 1));
    CASE(/*0322 0xD2*/ 0b11010010, OpAlubi(m, Bsu, REG, m->cx[0]));
    CASE(/*0323 0xD3*/ 0b11010011, OpAluwi(m, Bsu, REG, m->cx[0]));
    CASE(/*0327 0xD7*/ 0b11010111, OpXlat(m));
    CASE(/*0340 0xE0*/ 0b11100000, OpLoop(m, !GetFlag(m->flags, FLAGS_ZF)));
    CASE(/*0341 0xE1*/ 0b11100001, OpLoop(m, GetFlag(m->flags, FLAGS_ZF)));
    CASE(/*0342 0xE2*/ 0b11100010, OpLoop(m, 1));
    CASE(/*0343 0xE3*/ 0b11100011, OpJcxz(m));
    CASE(/*0344 0xE4*/ 0b11100100, Write8(m->ax, OpIn(m, UIMM0)));
    CASE(/*0345 0xE5*/ 0b11100101, Write32(m->ax, OpIn(m, UIMM0)));
    CASE(/*0346 0xE6*/ 0b11100110, OpOut(m, UIMM0, Read8(m->ax)));
    CASE(/*0347 0xE7*/ 0b11100111, OpOut(m, UIMM0, Read32(m->ax)));
    CASE(/*0350 0xE8*/ 0b11101000, OpCallJvds(m));
    CASE(/*0351 0xE9*/ 0b11101001, OpJmp(m));
    CASE(/*0353 0xEB*/ 0b11101011, OpJmp(m));
    CASE(/*0354 0xEC*/ 0b11101100, Write8(m->ax, OpIn(m, Read16(m->dx))));
    CASE(/*0355 0xED*/ 0b11101101, Write32(m->ax, OpIn(m, Read16(m->dx))));
    CASE(/*0356 0xEE*/ 0b11101110, OpOut(m, Read16(m->dx), Read8(m->ax)));
    CASE(/*0357 0xEF*/ 0b11101111, OpOut(m, Read16(m->dx), Read32(m->ax)));
    CASE(/*0361 0xF1*/ 0b11110001, OpInterrupt(m, 1));
    CASE(/*0364 0xF4*/ 0b11110100, OpHlt(m));
    CASE(/*0365 0xF5*/ 0b11110101, OpCmc(m));
    CASE(/*0370 0xF8*/ 0b11111000, OpClc(m));
    CASE(/*0371 0xF9*/ 0b11111001, OpStc(m));
    CASE(/*0372 0xFA*/ 0b11111010, OpCli(m));
    CASE(/*0373 0xFB*/ 0b11111011, OpSti(m));
    CASE(/*0374 0xFC*/ 0b11111100, OpCld(m));
    CASE(/*0375 0xFD*/ 0b11111101, OpStd(m));
    case /*0004 0x04*/ 0b00000100:
    case /*0014 0x0C*/ 0b00001100:
    case /*0024 0x14*/ 0b00010100:
    case /*0034 0x1C*/ 0b00011100:
    case /*0044 0x24*/ 0b00100100:
    case /*0054 0x2C*/ 0b00101100:
    case /*0064 0x34*/ 0b00110100:
    case /*0074 0x3C*/ 0b00111100:
      OpAlubi(m, Alu, (m->xedd->op.opcode & 070) >> 3, m->xedd->op.uimm0);
      break;
    case /*0005 0x05*/ 0b00000101:
    case /*0015 0x0D*/ 0b00001101:
    case /*0025 0x15*/ 0b00010101:
    case /*0035 0x1D*/ 0b00011101:
    case /*0045 0x25*/ 0b00100101:
    case /*0055 0x2D*/ 0b00101101:
    case /*0065 0x35*/ 0b00110101:
    case /*0075 0x3D*/ 0b00111101:
      OpAluwi(m, Alu, (m->xedd->op.opcode & 070) >> 3, m->xedd->op.uimm0);
      break;
    case /*0000 0x00*/ 0b00000000:
    case /*0002 0x02*/ 0b00000010:
    case /*0010 0x08*/ 0b00001000:
    case /*0012 0x0A*/ 0b00001010:
    case /*0020 0x10*/ 0b00010000:
    case /*0022 0x12*/ 0b00010010:
    case /*0030 0x18*/ 0b00011000:
    case /*0032 0x1A*/ 0b00011010:
    case /*0040 0x20*/ 0b00100000:
    case /*0042 0x22*/ 0b00100010:
    case /*0050 0x28*/ 0b00101000:
    case /*0052 0x2A*/ 0b00101010:
    case /*0060 0x30*/ 0b00110000:
    case /*0062 0x32*/ 0b00110010:
    case /*0070 0x38*/ 0b00111000:
    case /*0072 0x3A*/ 0b00111010:
      OpAlub(m, Alu,
             ((m->xedd->op.opcode & 070) >> 3 |
              (m->xedd->op.opcode & 0b010 ? ALU_FLIP : 0)));
      break;
    case /*0001 0x01*/ 0b00000001:
    case /*0003 0x03*/ 0b00000011:
    case /*0011 0x09*/ 0b00001001:
    case /*0013 0x0B*/ 0b00001011:
    case /*0021 0x11*/ 0b00010001:
    case /*0023 0x13*/ 0b00010011:
    case /*0031 0x19*/ 0b00011001:
    case /*0033 0x1B*/ 0b00011011:
    case /*0041 0x21*/ 0b00100001:
    case /*0043 0x23*/ 0b00100011:
    case /*0051 0x29*/ 0b00101001:
    case /*0053 0x2B*/ 0b00101011:
    case /*0061 0x31*/ 0b00110001:
    case /*0063 0x33*/ 0b00110011:
    case /*0071 0x39*/ 0b00111001:
    case /*0073 0x3B*/ 0b00111011:
      OpAluw(m, Alu,
             ((m->xedd->op.opcode & 070) >> 3 |
              (m->xedd->op.opcode & 0b010 ? ALU_FLIP : 0)));
      break;
    case /*0330 0xD8*/ 0b11011000:
    case /*0331 0xD9*/ 0b11011001:
    case /*0332 0xDA*/ 0b11011010:
    case /*0333 0xDB*/ 0b11011011:
    case /*0334 0xDC*/ 0b11011100:
    case /*0335 0xDD*/ 0b11011101:
    case /*0336 0xDE*/ 0b11011110:
    case /*0337 0xDF*/ 0b11011111:
      OpFpu(m);
      break;
    case /*0366 0xF6*/ 0b11110110:
      switch (ModrmReg(m->xedd)) {
        CASE(0, OpAlubi(m, Alu, ALU_TEST | ALU_AND, UIMM0));
        CASE(1, OpAlubi(m, Alu, ALU_TEST | ALU_AND, UIMM0));
        CASE(2, OpEb(m, AluNot));
        CASE(3, OpEb(m, AluNeg));
        CASE(4, OpMulAxAlEbUnsigned(m));
        CASE(5, OpMulAxAlEbSigned(m));
        CASE(6, OpDivAlAhAxEbUnsigned(m));
        CASE(7, OpDivAlAhAxEbSigned(m));
        default:
          unreachable;
      }
      break;
    case /*0367 0xF7*/ 0b11110111:
      switch (ModrmReg(m->xedd)) {
        CASE(0, OpAluwi(m, Alu, ALU_TEST | ALU_AND, UIMM0));
        CASE(1, OpAluwi(m, Alu, ALU_TEST | ALU_AND, UIMM0));
        CASE(2, OpEvqp(m, AluNot));
        CASE(3, OpEvqp(m, AluNeg));
        CASE(4, OpMulRdxRaxEvqpUnsigned(m));
        CASE(5, OpMulRdxRaxEvqpSigned(m));
        CASE(6, OpDivRdxRaxEvqpUnsigned(m));
        CASE(7, OpDivRdxRaxEvqpSigned(m));
        default:
          unreachable;
      }
      break;
    case /*0376 0xFE*/ 0b11111110:
      switch (ModrmReg(m->xedd)) {
        CASE(0b000, OpEb(m, AluInc));
        CASE(0b001, OpEb(m, AluDec));
        default:
          OpUd(m);
      }
      break;
    case /*0377 0xFF*/ 0b11111111:
      switch (ModrmReg(m->xedd)) {
        CASE(0, OpEvqp(m, AluInc));
        CASE(1, OpEvqp(m, AluDec));
        CASE(2, OpCallEq(m));
        CASE(4, OpJmpEq(m));
        CASE(6, OpPushEvq(m));
        default:
          OpUd(m);
      }
      break;
    default:
      OpUd(m);
  }
}

static void ExecuteInstructionMap1(struct Machine *m) {
  switch (m->xedd->op.opcode) {
    CASE(/*0005 0x05*/ 0b00000101, OpSyscall(m));
    CASE(/*0020 0x10*/ 0b00010000, OpMov0f10(m));
    CASE(/*0021 0x11*/ 0b00010001, OpMovWpsVps(m));
    CASE(/*0022 0x12*/ 0b00010010, OpMov0f12(m));
    CASE(/*0023 0x13*/ 0b00010011, OpMov0f13(m));
    CASE(/*0024 0x14*/ 0b00010100, OpUnpcklpsd(m));
    CASE(/*0025 0x15*/ 0b00010101, OpUnpckhpsd(m));
    CASE(/*0026 0x16*/ 0b00010110, OpMov0f16(m));
    CASE(/*0027 0x17*/ 0b00010111, OpMov0f17(m));
    CASE(/*0050 0x28*/ 0b00101000, OpMov0f28(m));
    CASE(/*0051 0x29*/ 0b00101001, OpMovWpsVps(m));
    CASE(/*0052 0x2A*/ 0b00101010, OpCvt(m, kOpCvt0f2a));
    CASE(/*0053 0x2B*/ 0b00101011, OpMov0f2b(m));
    CASE(/*0054 0x2C*/ 0b00101100, OpCvt(m, kOpCvtt0f2c));
    CASE(/*0055 0x2D*/ 0b00101101, OpCvt(m, kOpCvt0f2d));
    CASE(/*0056 0x2E*/ 0b00101110, OpComissVsWs(m));
    CASE(/*0057 0x2F*/ 0b00101111, OpComissVsWs(m));
    CASE(/*0061 0x31*/ 0b00110001, OpRdtsc(m));
    CASE(/*0100 0x40*/ 0b01000000, if (GetCond(m, 0x0)) OpMovGvqpEvqp(m));
    CASE(/*0101 0x41*/ 0b01000001, if (GetCond(m, 0x1)) OpMovGvqpEvqp(m));
    CASE(/*0102 0x42*/ 0b01000010, if (GetCond(m, 0x2)) OpMovGvqpEvqp(m));
    CASE(/*0103 0x43*/ 0b01000011, if (GetCond(m, 0x3)) OpMovGvqpEvqp(m));
    CASE(/*0104 0x44*/ 0b01000100, if (GetCond(m, 0x4)) OpMovGvqpEvqp(m));
    CASE(/*0105 0x45*/ 0b01000101, if (GetCond(m, 0x5)) OpMovGvqpEvqp(m));
    CASE(/*0106 0x46*/ 0b01000110, if (GetCond(m, 0x6)) OpMovGvqpEvqp(m));
    CASE(/*0107 0x47*/ 0b01000111, if (GetCond(m, 0x7)) OpMovGvqpEvqp(m));
    CASE(/*0110 0x48*/ 0b01001000, if (GetCond(m, 0x8)) OpMovGvqpEvqp(m));
    CASE(/*0111 0x49*/ 0b01001001, if (GetCond(m, 0x9)) OpMovGvqpEvqp(m));
    CASE(/*0112 0x4a*/ 0b01001010, if (GetCond(m, 0xa)) OpMovGvqpEvqp(m));
    CASE(/*0113 0x4b*/ 0b01001011, if (GetCond(m, 0xb)) OpMovGvqpEvqp(m));
    CASE(/*0114 0x4c*/ 0b01001100, if (GetCond(m, 0xc)) OpMovGvqpEvqp(m));
    CASE(/*0115 0x4d*/ 0b01001101, if (GetCond(m, 0xd)) OpMovGvqpEvqp(m));
    CASE(/*0116 0x4e*/ 0b01001110, if (GetCond(m, 0xe)) OpMovGvqpEvqp(m));
    CASE(/*0117 0x4f*/ 0b01001111, if (GetCond(m, 0xf)) OpMovGvqpEvqp(m));
    CASE(/*0121 0x51*/ 0b01010001, OpSqrtpsd(m));
    CASE(/*0122 0x52*/ 0b01010010, OpRsqrtps(m));
    CASE(/*0123 0x53*/ 0b01010011, OpRcpps(m));
    CASE(/*0124 0x54*/ 0b01010100, OpAndpsd(m));
    CASE(/*0125 0x55*/ 0b01010101, OpAndnpsd(m));
    CASE(/*0126 0x56*/ 0b01010110, OpOrpsd(m));
    CASE(/*0127 0x57*/ 0b01010111, OpXorpsd(m));
    CASE(/*0130 0x58*/ 0b01011000, OpAddpsd(m));
    CASE(/*0131 0x59*/ 0b01011001, OpMulpsd(m));
    CASE(/*0132 0x5A*/ 0b01011010, OpCvt(m, kOpCvt0f5a));
    CASE(/*0133 0x5B*/ 0b01011011, OpCvt(m, kOpCvt0f5b));
    CASE(/*0134 0x5C*/ 0b01011100, OpSubpsd(m));
    CASE(/*0135 0x5D*/ 0b01011101, OpMinpsd(m));
    CASE(/*0136 0x5E*/ 0b01011110, OpDivpsd(m));
    CASE(/*0137 0x5F*/ 0b01011111, OpMaxpsd(m));
    CASE(/*0140 0x60*/ 0b01100000, OpSse(m, kOpSsePunpcklbw));
    CASE(/*0141 0x61*/ 0b01100001, OpSse(m, kOpSsePunpcklwd));
    CASE(/*0142 0x62*/ 0b01100010, OpSse(m, kOpSsePunpckldq));
    CASE(/*0143 0x63*/ 0b01100011, OpSse(m, kOpSsePacksswb));
    CASE(/*0144 0x64*/ 0b01100100, OpSse(m, kOpSsePcmpgtb));
    CASE(/*0145 0x65*/ 0b01100101, OpSse(m, kOpSsePcmpgtw));
    CASE(/*0146 0x66*/ 0b01100110, OpSse(m, kOpSsePcmpgtd));
    CASE(/*0147 0x67*/ 0b01100111, OpSse(m, kOpSsePackuswb));
    CASE(/*0150 0x68*/ 0b01101000, OpSse(m, kOpSsePunpckhbw));
    CASE(/*0151 0x69*/ 0b01101001, OpSse(m, kOpSsePunpckhwd));
    CASE(/*0152 0x6A*/ 0b01101010, OpSse(m, kOpSsePunpckhdq));
    CASE(/*0153 0x6B*/ 0b01101011, OpSse(m, kOpSsePackssdw));
    CASE(/*0154 0x6C*/ 0b01101100, OpSse(m, kOpSsePunpcklqdq));
    CASE(/*0155 0x6D*/ 0b01101101, OpSse(m, kOpSsePunpckhqdq));
    CASE(/*0156 0x6E*/ 0b01101110, OpMov0f6e(m));
    CASE(/*0157 0x6F*/ 0b01101111, OpMov0f6f(m));
    CASE(/*0160 0x70*/ 0b01110000, OpShuffle(m));
    CASE(/*0164 0x74*/ 0b01110100, OpSse(m, kOpSsePcmpeqb));
    CASE(/*0165 0x75*/ 0b01110101, OpSse(m, kOpSsePcmpeqw));
    CASE(/*0166 0x76*/ 0b01110110, OpSse(m, kOpSsePcmpeqd));
    CASE(/*0174 0x7C*/ 0b01111100, OpHaddpsd(m));
    CASE(/*0175 0x7D*/ 0b01111101, OpHsubpsd(m));
    CASE(/*0176 0x7E*/ 0b01111110, OpMov0f7e(m));
    CASE(/*0177 0x7F*/ 0b01111111, OpMov0f7f(m));
    CASE(/*0200 0x80*/ 0b10000000, if (GetCond(m, 0x0)) OpJmp(m));
    CASE(/*0201 0x81*/ 0b10000001, if (GetCond(m, 0x1)) OpJmp(m));
    CASE(/*0202 0x82*/ 0b10000010, if (GetCond(m, 0x2)) OpJmp(m));
    CASE(/*0203 0x83*/ 0b10000011, if (GetCond(m, 0x3)) OpJmp(m));
    CASE(/*0204 0x84*/ 0b10000100, if (GetCond(m, 0x4)) OpJmp(m));
    CASE(/*0205 0x85*/ 0b10000101, if (GetCond(m, 0x5)) OpJmp(m));
    CASE(/*0206 0x86*/ 0b10000110, if (GetCond(m, 0x6)) OpJmp(m));
    CASE(/*0207 0x87*/ 0b10000111, if (GetCond(m, 0x7)) OpJmp(m));
    CASE(/*0210 0x88*/ 0b10001000, if (GetCond(m, 0x8)) OpJmp(m));
    CASE(/*0211 0x89*/ 0b10001001, if (GetCond(m, 0x9)) OpJmp(m));
    CASE(/*0212 0x8a*/ 0b10001010, if (GetCond(m, 0xa)) OpJmp(m));
    CASE(/*0213 0x8b*/ 0b10001011, if (GetCond(m, 0xb)) OpJmp(m));
    CASE(/*0214 0x8c*/ 0b10001100, if (GetCond(m, 0xc)) OpJmp(m));
    CASE(/*0215 0x8d*/ 0b10001101, if (GetCond(m, 0xd)) OpJmp(m));
    CASE(/*0216 0x8e*/ 0b10001110, if (GetCond(m, 0xe)) OpJmp(m));
    CASE(/*0217 0x8f*/ 0b10001111, if (GetCond(m, 0xf)) OpJmp(m));
    CASE(/*0220 0x90*/ 0b10010000, OpEbSetCc(m, GetCond(m, 0x0)));
    CASE(/*0221 0x91*/ 0b10010001, OpEbSetCc(m, GetCond(m, 0x1)));
    CASE(/*0222 0x92*/ 0b10010010, OpEbSetCc(m, GetCond(m, 0x2)));
    CASE(/*0223 0x93*/ 0b10010011, OpEbSetCc(m, GetCond(m, 0x3)));
    CASE(/*0224 0x94*/ 0b10010100, OpEbSetCc(m, GetCond(m, 0x4)));
    CASE(/*0225 0x95*/ 0b10010101, OpEbSetCc(m, GetCond(m, 0x5)));
    CASE(/*0226 0x96*/ 0b10010110, OpEbSetCc(m, GetCond(m, 0x6)));
    CASE(/*0227 0x97*/ 0b10010111, OpEbSetCc(m, GetCond(m, 0x7)));
    CASE(/*0230 0x98*/ 0b10011000, OpEbSetCc(m, GetCond(m, 0x8)));
    CASE(/*0231 0x99*/ 0b10011001, OpEbSetCc(m, GetCond(m, 0x9)));
    CASE(/*0232 0x9A*/ 0b10011010, OpEbSetCc(m, GetCond(m, 0xa)));
    CASE(/*0233 0x9B*/ 0b10011011, OpEbSetCc(m, GetCond(m, 0xb)));
    CASE(/*0234 0x9C*/ 0b10011100, OpEbSetCc(m, GetCond(m, 0xc)));
    CASE(/*0235 0x9D*/ 0b10011101, OpEbSetCc(m, GetCond(m, 0xd)));
    CASE(/*0236 0x9E*/ 0b10011110, OpEbSetCc(m, GetCond(m, 0xe)));
    CASE(/*0237 0x9F*/ 0b10011111, OpEbSetCc(m, GetCond(m, 0xf)));
    CASE(/*0240 0xA0*/ 0b10100000, OpPushFs(m));
    CASE(/*0241 0xA1*/ 0b10100001, OpPopFs(m));
    CASE(/*0242 0xA2*/ 0b10100010, OpCpuid(m));
    CASE(/*0243 0xA3*/ 0b10100011, OpBit(m));
    CASE(/*0250 0xA8*/ 0b10101000, OpPushGs(m));
    CASE(/*0251 0xA9*/ 0b10101001, OpPopGs(m));
    CASE(/*0253 0xAB*/ 0b10101011, OpBit(m));
    CASE(/*0257 0xAF*/ 0b10101111, OpImulGvqpEvqp(m));
    CASE(/*0260 0xB0*/ 0b10110000, OpCmpxchgEbAlGb(m));
    CASE(/*0261 0xB1*/ 0b10110001, OpCmpxchgEvqpRaxGvqp(m));
    CASE(/*0263 0xB3*/ 0b10110011, OpBit(m));
    CASE(/*0266 0xB6*/ 0b10110110, OpMovzbGvqpEb(m));
    CASE(/*0267 0xB7*/ 0b10110111, OpMovzwGvqpEw(m));
    CASE(/*0272 0xBA*/ 0b10111010, OpBit(m));
    CASE(/*0273 0xBB*/ 0b10111011, OpBit(m));
    CASE(/*0274 0xBC*/ 0b10111100, OpGvqpEvqp(m, AluBsf, MUTATING));
    CASE(/*0275 0xBD*/ 0b10111101, OpGvqpEvqp(m, AluBsr, MUTATING));
    CASE(/*0276 0xBE*/ 0b10111110, OpMovsbGvqpEb(m));
    CASE(/*0277 0xBF*/ 0b10111111, OpMovswGvqpEw(m));
    CASE(/*0300 0xC0*/ 0b11000000, OpAlub(m, Alu, ALU_XCHG | ALU_ADD));
    CASE(/*0301 0xC1*/ 0b11000001, OpAluw(m, Alu, ALU_XCHG | ALU_ADD));
    CASE(/*0302 0xC2*/ 0b11000010, OpCmppsd(m));
    CASE(/*0303 0xC3*/ 0b11000011, OpMovntiMdqpGdqp(m));
    CASE(/*0304 0xC4*/ 0b11000100, OpPinsrwVdqEwIb(m));
    CASE(/*0305 0xC5*/ 0b11000101, OpPextrwGdqpUdqIb(m));
    CASE(/*0306 0xC6*/ 0b11000110, OpShufpsd(m));
    CASE(/*0307 0xC7*/ 0b11000111, OpCmpxchgDxAx(m));
    CASE(/*0310 0xC8*/ 0b11001000 ... 0b11001111, OpBswapZvqp(m));
    CASE(/*0320 0xD0*/ 0b11010000, OpAddsubpsd(m));
    CASE(/*0321 0xD1*/ 0b11010001, OpSse(m, kOpSsePsrlwv));
    CASE(/*0322 0xD2*/ 0b11010010, OpSse(m, kOpSsePsrldv));
    CASE(/*0323 0xD3*/ 0b11010011, OpSse(m, kOpSsePsrlqv));
    CASE(/*0324 0xD4*/ 0b11010100, OpSse(m, kOpSsePaddq));
    CASE(/*0325 0xD5*/ 0b11010101, OpSse(m, kOpSsePmullw));
    CASE(/*0326 0xD6*/ 0b11010110, OpMov0fD6(m));
    CASE(/*0327 0xD7*/ 0b11010111, OpPmovmskbGdqpNqUdq(m));
    CASE(/*0330 0xD8*/ 0b11011000, OpSse(m, kOpSsePsubusb));
    CASE(/*0331 0xD9*/ 0b11011001, OpSse(m, kOpSsePsubusw));
    CASE(/*0332 0xDA*/ 0b11011010, OpSse(m, kOpSsePminub));
    CASE(/*0333 0xDB*/ 0b11011011, OpSse(m, kOpSsePand));
    CASE(/*0334 0xDC*/ 0b11011100, OpSse(m, kOpSsePaddusb));
    CASE(/*0335 0xDD*/ 0b11011101, OpSse(m, kOpSsePaddusw));
    CASE(/*0336 0xDE*/ 0b11011110, OpSse(m, kOpSsePmaxub));
    CASE(/*0337 0xDF*/ 0b11011111, OpSse(m, kOpSsePandn));
    CASE(/*0340 0xE0*/ 0b11100000, OpSse(m, kOpSsePavgb));
    CASE(/*0341 0xE1*/ 0b11100001, OpSse(m, kOpSsePsrawv));
    CASE(/*0342 0xE2*/ 0b11100010, OpSse(m, kOpSsePsradv));
    CASE(/*0343 0xE3*/ 0b11100011, OpSse(m, kOpSsePavgw));
    CASE(/*0344 0xE4*/ 0b11100100, OpSse(m, kOpSsePmulhuw));
    CASE(/*0345 0xE5*/ 0b11100101, OpSse(m, kOpSsePmulhw));
    CASE(/*0346 0xE6*/ 0b11100110, OpCvt(m, kOpCvt0fE6));
    CASE(/*0347 0xE7*/ 0b11100111, OpMov0fE7(m));
    CASE(/*0350 0xE8*/ 0b11101000, OpSse(m, kOpSsePsubsb));
    CASE(/*0351 0xE9*/ 0b11101001, OpSse(m, kOpSsePsubsw));
    CASE(/*0352 0xEA*/ 0b11101010, OpSse(m, kOpSsePminsw));
    CASE(/*0353 0xEB*/ 0b11101011, OpSse(m, kOpSsePor));
    CASE(/*0354 0xEC*/ 0b11101100, OpSse(m, kOpSsePaddsb));
    CASE(/*0355 0xED*/ 0b11101101, OpSse(m, kOpSsePaddsw));
    CASE(/*0356 0xEE*/ 0b11101110, OpSse(m, kOpSsePmaxsw));
    CASE(/*0357 0xEF*/ 0b11101111, OpSse(m, kOpSsePxor));
    CASE(/*0360 0xF0*/ 0b11110000, OpLddquVdqMdq(m));
    CASE(/*0361 0xF1*/ 0b11110001, OpSse(m, kOpSsePsllwv));
    CASE(/*0362 0xF2*/ 0b11110010, OpSse(m, kOpSsePslldv));
    CASE(/*0363 0xF3*/ 0b11110011, OpSse(m, kOpSsePsllqv));
    CASE(/*0364 0xF4*/ 0b11110100, OpSse(m, kOpSsePmuludq));
    CASE(/*0365 0xF5*/ 0b11110101, OpSse(m, kOpSsePmaddwd));
    CASE(/*0366 0xF6*/ 0b11110110, OpSse(m, kOpSsePsadbw));
    CASE(/*0367 0xF7*/ 0b11110111, OpMaskMovDiXmmRegXmmRm(m));
    CASE(/*0370 0xF8*/ 0b11111000, OpSse(m, kOpSsePsubb));
    CASE(/*0371 0xF9*/ 0b11111001, OpSse(m, kOpSsePsubw));
    CASE(/*0372 0xFA*/ 0b11111010, OpSse(m, kOpSsePsubd));
    CASE(/*0373 0xFB*/ 0b11111011, OpSse(m, kOpSsePsubq));
    CASE(/*0374 0xFC*/ 0b11111100, OpSse(m, kOpSsePaddb));
    CASE(/*0375 0xFD*/ 0b11111101, OpSse(m, kOpSsePaddw));
    CASE(/*0376 0xFE*/ 0b11111110, OpSse(m, kOpSsePaddd));
    case /*0013 0x0B*/ 0b00001011:
    case /*0271 0xB9*/ 0b10111001:
    case /*0377 0xFF*/ 0b11111111:
      OpUd(m);
      break;
    case /*0015 0x0D*/ 0b00001101:
    case /*0030 0x18*/ 0b00011000:
    case /*0031 0x19*/ 0b00011001:
    case /*0032 0x1A*/ 0b00011010:
    case /*0033 0x1B*/ 0b00011011:
    case /*0034 0x1C*/ 0b00011100:
    case /*0035 0x1D*/ 0b00011101:
      OpWutNopEv(m);
      break;
    case /*0037 0x1F*/ 0b00011111:
      OpNopEv(m);
      break;
    case /*0161 0x71*/ 0b01110001:
      switch (ModrmReg(m->xedd)) {
        CASE(2, OpSseUdqIb(m, kOpSseUdqIbPsrlw));
        CASE(4, OpSseUdqIb(m, kOpSseUdqIbPsraw));
        CASE(6, OpSseUdqIb(m, kOpSseUdqIbPsllw));
        default:
          OpUd(m);
      }
      break;
    case /*0162 0x72*/ 0b01110010:
      switch (ModrmReg(m->xedd)) {
        CASE(2, OpSseUdqIb(m, kOpSseUdqIbPsrld));
        CASE(4, OpSseUdqIb(m, kOpSseUdqIbPsrad));
        CASE(6, OpSseUdqIb(m, kOpSseUdqIbPslld));
        default:
          OpUd(m);
      }
      break;
    case /*0163 0x73*/ 0b01110011:
      switch (ModrmReg(m->xedd)) {
        CASE(2, OpSseUdqIb(m, kOpSseUdqIbPsrlq));
        CASE(3, OpSseUdqIb(m, kOpSseUdqIbPsrldq));
        CASE(6, OpSseUdqIb(m, kOpSseUdqIbPsllq));
        CASE(7, OpSseUdqIb(m, kOpSseUdqIbPslldq));
        default:
          OpUd(m);
      }
      break;
    case /*0244 0xA4*/ 0b10100100:
    case /*0245 0xA5*/ 0b10100101:
    case /*0254 0xAC*/ 0b10101100:
    case /*0255 0xAD*/ 0b10101101:
      OpEvqpGvqp(m, OpDoubleShift, MUTATING);
      break;
    case /*0256 0xAE*/ 0b10101110:
      switch (ModrmReg(m->xedd)) {
        CASE(5, OpLfence(m));
        CASE(6, OpMfence(m));
        case 7:
          if (0xf8 <= m->xedd->op.modrm && m->xedd->op.modrm <= 0xff) {
            OpSfence(m);
          } else {
            OpClflush(m);
          }
          break;
        default:
          OpUd(m);
      }
      break;
    case 0xB8:
      if (m->xedd->op.ild_f3) {
        OpGvqpEvqp(m, AluPopcnt, MUTATING);
      } else {
        OpUd(m);
      }
      break;
    default:
      OpUd(m);
  }
}

static void ExecuteInstructionMap2(struct Machine *m) {
  switch (m->xedd->op.opcode) {
    CASE(0x00, OpSse(m, kOpSsePshufb));
    CASE(0x01, OpSse(m, kOpSsePhaddw));
    CASE(0x02, OpSse(m, kOpSsePhaddd));
    CASE(0x03, OpSse(m, kOpSsePhaddsw));
    CASE(0x04, OpSse(m, kOpSsePmaddubsw));
    CASE(0x05, OpSse(m, kOpSsePhsubw));
    CASE(0x06, OpSse(m, kOpSsePhsubd));
    CASE(0x07, OpSse(m, kOpSsePhsubsw));
    CASE(0x08, OpSse(m, kOpSsePsignb));
    CASE(0x09, OpSse(m, kOpSsePsignw));
    CASE(0x0A, OpSse(m, kOpSsePsignd));
    CASE(0x0B, OpSse(m, kOpSsePmulhrsw));
    CASE(0x1C, OpSse(m, kOpSsePabsb));
    CASE(0x1D, OpSse(m, kOpSsePabsw));
    CASE(0x1E, OpSse(m, kOpSsePabsd));
    CASE(0x2A, OpMovntdqaVdqMdq(m));
    CASE(0x40, OpSse(m, kOpSsePmulld));
    default:
      OpUd(m);
  }
}

static void ExecuteInstructionMap3(struct Machine *m) {
  switch (m->xedd->op.opcode) {
    CASE(0x0F, OpSsePalignr(m));
    default:
      OpUd(m);
  }
}

static void SaveStash(struct Machine *m) {
  VirtualRecv(m, m->stashaddr, m->stash, m->stashsize);
  m->stashaddr = 0;
}

void ExecuteInstruction(struct Machine *m) {
  uint8_t *p;
  m->ip += m->xedd->length;
  switch (m->xedd->op.map) {
    CASE(XED_ILD_MAP0, ExecuteInstructionMap0(m));
    CASE(XED_ILD_MAP1, ExecuteInstructionMap1(m));
    CASE(XED_ILD_MAP2, ExecuteInstructionMap2(m));
    CASE(XED_ILD_MAP3, ExecuteInstructionMap3(m));
    default:
      OpUd(m);
  }
  if (m->stashaddr) {
    SaveStash(m);
  }
}
