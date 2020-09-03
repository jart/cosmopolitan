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
#include "libc/runtime/runtime.h"
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
#include "tool/build/lib/time.h"

#define MUTATING      true
#define READONLY      false
#define UNCONDITIONAL true
#define BITS          (8u << RegLog2(rde))
#define SIGN          (1ull << (BITS - 1))
#define MASK          (SIGN | (SIGN - 1))
#define SHIFTMASK     (BITS - 1)
#define TEST          ALU_TEST | ALU_AND

typedef int int_v _Vector_size(16) aligned(16);
typedef long long_v _Vector_size(16) aligned(16);
typedef float float_v _Vector_size(16) aligned(16);
typedef double double_v _Vector_size(16) aligned(16);

typedef void (*machine_f)(struct Machine *);
typedef void (*map_f)(struct Machine *, uint32_t);
typedef void (*alu_f)(struct Machine *, aluop2_f, bool);
typedef void (*machine_u8p_u8p_f)(struct Machine *, uint8_t *, uint8_t *);

static uint64_t AluNot(struct Machine *m, uint32_t rde, uint64_t x) {
  return ~x & MASK;
}

static uint64_t AluNeg(struct Machine *m, uint32_t rde, uint64_t x) {
  m->flags = SetFlag(m->flags, FLAGS_CF, !!(x & MASK));
  m->flags = SetFlag(m->flags, FLAGS_OF, (x & MASK) == SIGN);
  x = ~x + 1;
  m->flags = SetFlag(m->flags, FLAGS_ZF, !(x & MASK));
  m->flags = SetFlag(m->flags, FLAGS_SF, !!(x & SIGN));
  m->flags = SetLazyParityByte(m->flags, x);
  return x & MASK;
}

static uint64_t AluInc(struct Machine *m, uint32_t rde, uint64_t a) {
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

static uint64_t AluDec(struct Machine *m, uint32_t rde, uint64_t a) {
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

forceinline uint64_t ReadMemory(uint32_t rde, uint8_t p[8]) {
  if (Rexw(rde)) {
    return Read64(p);
  } else if (!Osz(rde)) {
    return Read32(p);
  } else {
    return Read16(p);
  }
}

forceinline int64_t ReadMemorySigned(uint32_t rde, uint8_t p[8]) {
  if (Rexw(rde)) {
    return (int64_t)Read64(p);
  } else if (!Osz(rde)) {
    return (int32_t)Read32(p);
  } else {
    return (int16_t)Read16(p);
  }
}

forceinline void WriteRegister(uint32_t rde, uint8_t p[8], uint64_t x) {
  if (Rexw(rde)) {
    Write64(p, x);
  } else if (!Osz(rde)) {
    Write64(p, x & 0xffffffff);
  } else {
    Write16(p, x);
  }
}

forceinline void WriteMemory(uint32_t rde, uint8_t p[8], uint64_t x) {
  if (Rexw(rde)) {
    Write64(p, x);
  } else if (!Osz(rde)) {
    Write32(p, x);
  } else {
    Write16(p, x);
  }
}

forceinline void WriteRegisterOrMemory(uint32_t rde, uint8_t p[8], uint64_t x) {
  if (IsModrmRegister(rde)) {
    WriteRegister(rde, p, x);
  } else {
    WriteMemory(rde, p, x);
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

static void OpPushf(struct Machine *m, uint32_t rde) {
  PushOsz(m, rde, ExportFlags(m->flags) & 0xFCFFFF);
}

static void OpPopf(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    ImportFlags(m, Pop64(m, 0));
  } else {
    ImportFlags(m, (m->flags & ~0xffff) | Pop16(m, 0));
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
  ImportFlags(m, (m->flags & ~0xff) | m->ax[1]);
}

static void OpEbSetCc(struct Machine *m, uint32_t rde, bool x) {
  Write8(GetModrmRegisterBytePointerWrite(m, rde), x);
}

static void OpLeaGvqpM(struct Machine *m, uint32_t rde) {
  Write64(RegRexrReg(m, rde), ComputeAddress(m, rde));
}

static void PushVq(struct Machine *m, uint32_t rde, uint8_t *p) {
  if (!Osz(rde)) {
    Push64(m, Read64(p));
  } else {
    Push16(m, Read16(p));
  }
}

static void PopVq(struct Machine *m, uint32_t rde, uint8_t *p) {
  if (!Osz(rde)) {
    Write64(p, Pop64(m, 0));
  } else {
    Write16(p, Pop16(m, 0));
  }
}

static void OpPushEvq(struct Machine *m, uint32_t rde) {
  PushVq(m, rde, GetModrmRegisterWordPointerReadOsz(m, rde));
}

static void OpPushZvq(struct Machine *m, uint32_t rde) {
  PushVq(m, rde, RegRexbSrm(m, rde));
}

static void OpPopZvq(struct Machine *m, uint32_t rde) {
  PopVq(m, rde, RegRexbSrm(m, rde));
}

static void OpPopEvq(struct Machine *m, uint32_t rde) {
  PopVq(m, rde, GetModrmRegisterWordPointerWriteOsz(m, rde));
}

static void OpJmp(struct Machine *m) {
  m->ip += m->xedd->op.disp;
}

static void OpJmpEq(struct Machine *m, uint32_t rde) {
  m->ip = Read64(GetModrmRegisterWordPointerRead8(m, rde));
}

static void OpJcxz(struct Machine *m, uint32_t rde) {
  uint64_t count;
  count = Read64(m->cx);
  if (Asz(rde)) count &= 0xffffffff;
  if (!count) OpJmp(m);
}

static void OpLoop(struct Machine *m, uint32_t rde, bool cond) {
  uint64_t count;
  count = Read64(m->cx) - 1;
  if (Asz(rde)) count &= 0xffffffff;
  Write64(m->cx, count);
  if (count && cond) m->ip += m->xedd->op.disp;
}

static void OpXlat(struct Machine *m, uint32_t rde) {
  int64_t v;
  uint8_t al;
  v = Read64(m->bx) + Read8(m->ax);
  if (Asz(rde)) v &= 0xffffffff;
  SetReadAddr(m, v, 1);
  Write8(m->ax, Read8(ResolveAddress(m, v)));
}

static void OpEb(struct Machine *m, uint32_t rde, aluop1_f op) {
  uint8_t *p;
  p = GetModrmRegisterBytePointerWrite(m, rde);
  Write8(p, op(m, rde, Read8(p)));
}

static void OpEvqp(struct Machine *m, uint32_t rde, aluop1_f op) {
  uint8_t *p;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  WriteRegisterOrMemory(rde, p, op(m, rde, ReadMemory(rde, p)));
}

static void OpGvqpEvqp(struct Machine *m, uint32_t rde, aluop2_f op,
                       bool write) {
  uint64_t x;
  x = op(m, rde, ReadMemory(rde, RegRexrReg(m, rde)),
         ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)));
  if (write) WriteRegister(rde, RegRexrReg(m, rde), x);
}

static void OpEvqpGvqp(struct Machine *m, uint32_t rde, aluop2_f op,
                       bool write) {
  uint8_t *p;
  uint64_t x;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  x = op(m, rde, ReadMemory(rde, p), ReadMemory(rde, RegRexrReg(m, rde)));
  if (write) WriteRegisterOrMemory(rde, p, x);
}

static void OpXchgZvqp(struct Machine *m, uint32_t rde) {
  uint64_t x, y;
  x = Read64(m->ax);
  y = Read64(RegRexbSrm(m, rde));
  WriteRegister(rde, m->ax, y);
  WriteRegister(rde, RegRexbSrm(m, rde), x);
}

static void OpNopEv(struct Machine *m) {
  if (m->xedd->op.modrm == 0x45) {
    OpBofram(m);
  }
}

static void OpPause(struct Machine *m) {
  sched_yield();
}

static void OpNop(struct Machine *m, uint32_t rde) {
  if (m->xedd->op.rexb) {
    OpXchgZvqp(m, rde);
  } else if (Rep(rde) == 3) {
    OpPause(m);
  }
}

static void OpXchgGbEb(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint8_t x, y;
  p = GetModrmRegisterBytePointerWrite(m, rde);
  x = Read8(ByteRexrReg(m, rde));
  y = Read8(p);
  Write8(ByteRexrReg(m, rde), y);
  Write8(p, x);
}

static void OpXchgGvqpEvqp(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint64_t x, y;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  x = ReadMemory(rde, RegRexrReg(m, rde));
  y = ReadMemory(rde, p);
  WriteRegister(rde, RegRexrReg(m, rde), y);
  WriteRegisterOrMemory(rde, p, x);
}

static void OpCmpxchgEbAlGb(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint8_t x, y, z;
  p = GetModrmRegisterBytePointerWrite(m, rde);
  x = Read8(m->ax);
  y = Read8(p);
  z = Read8(ByteRexrReg(m, rde));
  Alu(0, ALU_SUB, x, y, &m->flags);
  if (GetFlag(m->flags, FLAGS_ZF)) {
    Write8(p, z);
  } else {
    Write8(m->ax, y);
  }
}

static void OpCmpxchgEvqpRaxGvqp(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint64_t x, y, z;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  x = ReadMemory(rde, m->ax);
  y = ReadMemory(rde, p);
  z = ReadMemory(rde, RegRexrReg(m, rde));
  Alu(RegLog2(rde), ALU_SUB, x, y, &m->flags);
  if (GetFlag(m->flags, FLAGS_ZF)) {
    WriteRegisterOrMemory(rde, p, z);
  } else {
    WriteRegister(rde, m->ax, y);
  }
}

static void OpCmpxchg8b(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint32_t d, a;
  p = GetModrmRegisterXmmPointerRead8(m, rde);
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

static void OpCmpxchg16b(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint64_t d, a;
  p = GetModrmRegisterXmmPointerRead16(m, rde);
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

static void OpCmpxchgDxAx(struct Machine *m, uint32_t rde) {
  if (Rexw(rde)) {
    OpCmpxchg16b(m, rde);
  } else {
    OpCmpxchg8b(m, rde);
  }
}

static void OpXaddEbGb(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint8_t x, y, z;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  x = Read8(p);
  y = Read8(RegRexrReg(m, rde));
  z = Alu(0, ALU_ADD, x, y, &m->flags);
  Write8(p, z);
  Write8(RegRexrReg(m, rde), x);
}

static void OpXaddEvqpGvqp(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint64_t x, y, z;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  x = ReadMemory(rde, p);
  y = ReadMemory(rde, RegRexrReg(m, rde));
  z = Alu(RegLog2(rde), ALU_ADD, x, y, &m->flags);
  WriteRegisterOrMemory(rde, p, z);
  WriteRegister(rde, RegRexrReg(m, rde), x);
}

static uint64_t OpDoubleShift(struct Machine *m, uint32_t rde, uint64_t x,
                              uint64_t y) {
  uint8_t b, w, W[2][2] = {{2, 3}, {1, 3}};
  return BsuDoubleShift(
      W[Osz(rde)][Rexw(rde)], x, y,
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

static void OpBit(struct Machine *m, uint32_t rde) {
  int op;
  uint8_t *p;
  unsigned bit;
  int64_t disp;
  uint64_t v, x, y, z;
  uint8_t w, W[2][2] = {{2, 3}, {1, 3}};
  w = W[Osz(rde)][Rexw(rde)];
  if (m->xedd->op.opcode == 0xBA) {
    op = ModrmReg(rde);
    bit = m->xedd->op.uimm0 & ((8 << w) - 1);
    disp = 0;
  } else {
    op = (m->xedd->op.opcode & 070) >> 3;
    disp = ReadMemorySigned(rde, RegRexrReg(m, rde));
    bit = disp & ((8 << w) - 1);
    disp &= -(8 << w);
    disp >>= 3;
  }
  if (IsModrmRegister(rde)) {
    p = RegRexbRm(m, rde);
  } else {
    v = ComputeAddress(m, rde) + disp;
    if (Asz(rde)) v &= 0xffffffff;
    p = ReserveAddress(m, v, 1 << w);
    if (op == 4) {
      SetReadAddr(m, v, 1 << w);
    } else {
      SetWriteAddr(m, v, 1 << w);
    }
  }
  y = 1ull << bit;
  x = ReadMemory(rde, p);
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
  WriteRegisterOrMemory(rde, p, z);
}

static void OpConvert1(struct Machine *m, uint32_t rde) {
  if (Rexw(rde)) {
    Write64(m->ax, Read32(m->ax) |
                       (Read32(m->ax) & 0x80000000 ? 0xffffffff00000000 : 0));
  } else if (!Osz(rde)) {
    Write64(m->ax, Read16(m->ax) | (Read16(m->ax) & 0x8000 ? 0xffff0000 : 0));
  } else {
    Write16(m->ax, Read8(m->ax) | (Read8(m->ax) & 0x0080 ? 0xff00 : 0));
  }
}

static void OpConvert2(struct Machine *m, uint32_t rde) {
  if (Rexw(rde)) {
    Write64(m->dx, Read64(m->ax) & 0x8000000000000000 ? 0xffffffffffffffff : 0);
  } else if (!Osz(rde)) {
    Write64(m->dx, Read32(m->ax) & 0x80000000 ? 0xffffffff : 0);
  } else {
    Write16(m->dx, Read16(m->ax) & 0x8000 ? 0xffff : 0);
  }
}

static void OpBswapZvqp(struct Machine *m, uint32_t rde) {
  uint64_t x;
  x = Read64(RegRexbSrm(m, rde));
  if (Rexw(rde)) {
    Write64(
        RegRexbSrm(m, rde),
        ((x & 0xff00000000000000) >> 070 | (x & 0x00000000000000ff) << 070 |
         (x & 0x00ff000000000000) >> 050 | (x & 0x000000000000ff00) << 050 |
         (x & 0x0000ff0000000000) >> 030 | (x & 0x0000000000ff0000) << 030 |
         (x & 0x000000ff00000000) >> 010 | (x & 0x00000000ff000000) << 010));
  } else if (!Osz(rde)) {
    Write64(RegRexbSrm(m, rde),
            ((x & 0xff000000) >> 030 | (x & 0x000000ff) << 030 |
             (x & 0x00ff0000) >> 010 | (x & 0x0000ff00) << 010));
  } else {
    Write16(RegRexbSrm(m, rde), ((x & 0x00ff) << 010 | (x & 0xff00) << 010));
  }
}

static uint8_t pmovmskb(uint64_t x) {
  return (x & 0x0000000000000080) >> 007 | (x & 0x0000000000008000) >> 016 |
         (x & 0x0000000000800000) >> 025 | (x & 0x0000000080000000) >> 034 |
         (x & 0x0000008000000000) >> 043 | (x & 0x0000800000000000) >> 052 |
         (x & 0x0080000000000000) >> 061 | (x & 0x8000000000000000) >> 070;
}

static void OpPmovmskbGdqpNqUdq(struct Machine *m, uint32_t rde) {
  uint64_t bitmask;
  if (Osz(rde)) {
    bitmask = pmovmskb(Read64(XmmRexbRm(m, rde) + 8)) << 8 |
              pmovmskb(Read64(XmmRexbRm(m, rde)));
  } else {
    bitmask = pmovmskb(Read64(MmRm(m, rde) + 8)) << 8 |
              pmovmskb(Read64(MmRm(m, rde)));
  }
  Write64(RegRexrReg(m, rde), bitmask);
}

static void OpMovEvqpSw(struct Machine *m) {
  ThrowSegmentationFault(m, m->ip);
}

static void OpMovSwEvqp(struct Machine *m) {
  ThrowSegmentationFault(m, m->ip);
}

static void OpMovEbIb(struct Machine *m, uint32_t rde) {
  Write8(GetModrmRegisterBytePointerWrite(m, rde), m->xedd->op.uimm0);
}

static void OpMovAlOb(struct Machine *m) {
  memcpy(ResolveAddress(m, m->xedd->op.uimm0), m->ax, 1);
}

static void OpMovObAl(struct Machine *m) {
  memcpy(m->ax, ResolveAddress(m, m->xedd->op.uimm0), 1);
}

static void OpMovRaxOvqp(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, m->ax,
                ReadMemory(rde, ResolveAddress(m, m->xedd->op.uimm0)));
}

static void OpMovOvqpRax(struct Machine *m, uint32_t rde) {
  WriteMemory(rde, ResolveAddress(m, m->xedd->op.uimm0), Read64(m->ax));
}

static void OpMovEbGb(struct Machine *m, uint32_t rde) {
  memcpy(GetModrmRegisterBytePointerWrite(m, rde), ByteRexrReg(m, rde), 1);
}

static void OpMovGbEb(struct Machine *m, uint32_t rde) {
  memcpy(ByteRexrReg(m, rde), GetModrmRegisterBytePointerRead(m, rde), 1);
}

static void OpMovZbIb(struct Machine *m, uint32_t rde) {
  Write8(ByteRexbSrm(m, rde), m->xedd->op.uimm0);
}

static void OpMovZvqpIvqp(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexbSrm(m, rde), m->xedd->op.uimm0);
}

static void OpMovEvqpIvds(struct Machine *m, uint32_t rde) {
  WriteRegisterOrMemory(rde, GetModrmRegisterWordPointerWriteOszRexw(m, rde),
                        m->xedd->op.uimm0);
}

noinline void OpMovEvqpGvqp(struct Machine *m, uint32_t rde) {
  WriteRegisterOrMemory(rde, GetModrmRegisterWordPointerWriteOszRexw(m, rde),
                        ReadMemory(rde, RegRexrReg(m, rde)));
}

static void OpMovGvqpEvqp(struct Machine *m, uint32_t rde) {
  WriteRegister(
      rde, RegRexrReg(m, rde),
      ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)));
}

static void OpMovzbGvqpEb(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexrReg(m, rde),
                Read8(GetModrmRegisterBytePointerRead(m, rde)));
}

static void OpMovzwGvqpEw(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexrReg(m, rde),
                Read16(GetModrmRegisterWordPointerRead2(m, rde)));
}

static void OpMovsbGvqpEb(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexrReg(m, rde),
                (int8_t)Read8(GetModrmRegisterBytePointerRead(m, rde)));
}

static void OpMovswGvqpEw(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexrReg(m, rde),
                (int16_t)Read16(GetModrmRegisterWordPointerRead2(m, rde)));
}

static void OpMovsxdGdqpEd(struct Machine *m, uint32_t rde) {
  uint64_t x;
  uint8_t *p;
  x = (int32_t)Read32(GetModrmRegisterWordPointerRead4(m, rde));
  if (!Rexw(rde)) x &= 0xffffffff; /* wut */
  Write64(RegRexrReg(m, rde), x);
}

static void OpMovdquVdqWdq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead16(m, rde), 16);
}

static void OpMovdquWdqVdq(struct Machine *m, uint32_t rde) {
  memcpy(GetModrmRegisterXmmPointerWrite16(m, rde), XmmRexrReg(m, rde), 16);
}

static void OpMovupsVpsWps(struct Machine *m, uint32_t rde) {
  OpMovdquVdqWdq(m, rde);
}

static void OpMovupsWpsVps(struct Machine *m, uint32_t rde) {
  OpMovdquWdqVdq(m, rde);
}

static void OpMovupdVpsWps(struct Machine *m, uint32_t rde) {
  OpMovdquVdqWdq(m, rde);
}

static void OpMovupdWpsVps(struct Machine *m, uint32_t rde) {
  OpMovdquWdqVdq(m, rde);
}

static void OpLddquVdqMdq(struct Machine *m, uint32_t rde) {
  OpMovdquVdqWdq(m, rde);
}

static void OpMovdqaVdqMdq(struct Machine *m, uint32_t rde) {
  int64_t v;
  uint8_t *p;
  v = ComputeAddress(m, rde);
  SetReadAddr(m, v, 16);
  if ((v & 15) || !(p = FindReal(m, v))) ThrowSegmentationFault(m, v);
  memcpy(XmmRexrReg(m, rde), Abp16(p), 16);
}

static void OpMovdqaMdqVdq(struct Machine *m, uint32_t rde) {
  int64_t v;
  uint8_t *p;
  v = ComputeAddress(m, rde);
  SetWriteAddr(m, v, 16);
  if ((v & 15) || !(p = FindReal(m, v))) ThrowSegmentationFault(m, v);
  memcpy(Abp16(p), XmmRexrReg(m, rde), 16);
}

static void OpMovdqaVdqWdq(struct Machine *m, uint32_t rde) {
  if (IsModrmRegister(rde)) {
    memcpy(XmmRexrReg(m, rde), XmmRexbRm(m, rde), 16);
  } else {
    OpMovdqaVdqMdq(m, rde);
  }
}

static void OpMovdqaWdqVdq(struct Machine *m, uint32_t rde) {
  if (IsModrmRegister(rde)) {
    memcpy(XmmRexbRm(m, rde), XmmRexrReg(m, rde), 16);
  } else {
    OpMovdqaMdqVdq(m, rde);
  }
}

static void OpMovntiMdqpGdqp(struct Machine *m, uint32_t rde) {
  int64_t v;
  uint8_t *a;
  void *p[2];
  uint8_t n, b[8];
  v = ComputeAddress(m, rde);
  n = Rexw(rde) ? 8 : 4;
  a = BeginStore(m, v, n, p, b);
  SetWriteAddr(m, v, n);
  memcpy(p, XmmRexrReg(m, rde), n);
  EndStore(m, v, n, p, b);
}

static void OpMovntdqMdqVdq(struct Machine *m, uint32_t rde) {
  OpMovdqaMdqVdq(m, rde);
}

static void OpMovntpsMpsVps(struct Machine *m, uint32_t rde) {
  OpMovdqaMdqVdq(m, rde);
}

static void OpMovntpdMpdVpd(struct Machine *m, uint32_t rde) {
  OpMovdqaMdqVdq(m, rde);
}

static void OpMovntdqaVdqMdq(struct Machine *m, uint32_t rde) {
  OpMovdqaVdqMdq(m, rde);
}

static void OpMovqPqQq(struct Machine *m, uint32_t rde) {
  memcpy(MmReg(m, rde), GetModrmRegisterMmPointerRead8(m, rde), 8);
}

static void OpMovqQqPq(struct Machine *m, uint32_t rde) {
  memcpy(GetModrmRegisterMmPointerWrite8(m, rde), MmReg(m, rde), 8);
}

static void OpMovqVdqEqp(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), GetModrmRegisterWordPointerRead8(m, rde), 8);
  memset(XmmRexrReg(m, rde) + 8, 0, 8);
}

static void OpMovdVdqEd(struct Machine *m, uint32_t rde) {
  memset(XmmRexrReg(m, rde), 0, 16);
  memcpy(XmmRexrReg(m, rde), GetModrmRegisterWordPointerRead4(m, rde), 4);
}

static void OpMovqPqEqp(struct Machine *m, uint32_t rde) {
  memcpy(MmReg(m, rde), GetModrmRegisterWordPointerRead8(m, rde), 8);
}

static void OpMovdPqEd(struct Machine *m, uint32_t rde) {
  memcpy(MmReg(m, rde), GetModrmRegisterWordPointerRead4(m, rde), 4);
  memset(MmReg(m, rde) + 4, 0, 4);
}

static void OpMovdEdVdq(struct Machine *m, uint32_t rde) {
  if (IsModrmRegister(rde)) {
    Write64(RegRexbRm(m, rde), Read32(XmmRexrReg(m, rde)));
  } else {
    memcpy(ComputeReserveAddressWrite4(m, rde), XmmRexrReg(m, rde), 4);
  }
}

static void OpMovqEqpVdq(struct Machine *m, uint32_t rde) {
  memcpy(GetModrmRegisterWordPointerWrite8(m, rde), XmmRexrReg(m, rde), 8);
}

static void OpMovdEdPq(struct Machine *m, uint32_t rde) {
  if (IsModrmRegister(rde)) {
    Write64(RegRexbRm(m, rde), Read32(MmReg(m, rde)));
  } else {
    memcpy(ComputeReserveAddressWrite4(m, rde), MmReg(m, rde), 4);
  }
}

static void OpMovqEqpPq(struct Machine *m, uint32_t rde) {
  memcpy(GetModrmRegisterWordPointerWrite(m, rde, 8), MmReg(m, rde), 8);
}

static void OpMovntqMqPq(struct Machine *m, uint32_t rde) {
  memcpy(ComputeReserveAddressWrite8(m, rde), MmReg(m, rde), 8);
}

static void OpMovqVqWq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead8(m, rde), 8);
  memset(XmmRexrReg(m, rde) + 8, 0, 8);
}

static void OpMovssVpsWps(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead4(m, rde), 4);
}

static void OpMovssWpsVps(struct Machine *m, uint32_t rde) {
  memcpy(GetModrmRegisterXmmPointerWrite4(m, rde), XmmRexrReg(m, rde), 4);
}

static void OpMovsdVpsWps(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead16(m, rde), 8);
}

static void OpMovsdWpsVps(struct Machine *m, uint32_t rde) {
  memcpy(GetModrmRegisterXmmPointerWrite16(m, rde), XmmRexrReg(m, rde), 8);
}

static void OpMaskMovDiXmmRegXmmRm(struct Machine *m, uint32_t rde) {
  void *p[2];
  uint64_t v;
  unsigned i, n;
  uint8_t *mem, b[16];
  n = Osz(rde) ? 16 : 8;
  v = GetSegment() + Read64(m->di);
  if (Asz(rde)) v &= 0xffffffff;
  mem = BeginStore(m, v, n, p, b);
  for (i = 0; i < n; ++i) {
    if (XmmRexbRm(m, rde)[i] & 0x80) {
      mem[i] = XmmRexrReg(m, rde)[i];
    }
  }
  EndStore(m, v, n, p, b);
}

static void OpMovhlpsVqUq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), XmmRexbRm(m, rde) + 8, 8);
}

static void OpMovlpsVqMq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), ComputeReserveAddressRead8(m, rde), 8);
}

static void OpMovlpdVqMq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), ComputeReserveAddressRead8(m, rde), 8);
}

static void OpMovddupVqWq(struct Machine *m, uint32_t rde) {
  uint8_t *src;
  src = GetModrmRegisterXmmPointerRead8(m, rde);
  memcpy(XmmRexrReg(m, rde) + 0, src, 8);
  memcpy(XmmRexrReg(m, rde) + 8, src, 8);
}

static void OpMovsldupVqWq(struct Machine *m, uint32_t rde) {
  uint8_t *dst, *src;
  dst = XmmRexrReg(m, rde);
  src = GetModrmRegisterXmmPointerRead16(m, rde);
  memcpy(dst + 0 + 0, src + 0, 4);
  memcpy(dst + 0 + 4, src + 0, 4);
  memcpy(dst + 8 + 0, src + 8, 4);
  memcpy(dst + 8 + 4, src + 8, 4);
}

static void OpMovlpsMqVq(struct Machine *m, uint32_t rde) {
  memcpy(ComputeReserveAddressWrite8(m, rde), XmmRexrReg(m, rde), 8);
}

static void OpMovlpdMqVq(struct Machine *m, uint32_t rde) {
  memcpy(ComputeReserveAddressWrite8(m, rde), XmmRexrReg(m, rde), 8);
}

static void OpMovlhpsVqUq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde) + 8, XmmRexbRm(m, rde), 8);
}

static void OpMovhpsVqMq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde) + 8, ComputeReserveAddressRead8(m, rde), 8);
}

static void OpMovhpdVqMq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde) + 8, ComputeReserveAddressRead8(m, rde), 8);
}

static void OpMovshdupVqWq(struct Machine *m, uint32_t rde) {
  uint8_t *dst, *src;
  dst = XmmRexrReg(m, rde);
  src = GetModrmRegisterXmmPointerRead16(m, rde);
  memcpy(dst + 0 + 0, src + 04, 4);
  memcpy(dst + 0 + 4, src + 04, 4);
  memcpy(dst + 8 + 0, src + 12, 4);
  memcpy(dst + 8 + 4, src + 12, 4);
}

static void OpMovhpsMqVq(struct Machine *m, uint32_t rde) {
  memcpy(ComputeReserveAddressRead8(m, rde), XmmRexrReg(m, rde) + 8, 8);
}

static void OpMovhpdMqVq(struct Machine *m, uint32_t rde) {
  memcpy(ComputeReserveAddressRead8(m, rde), XmmRexrReg(m, rde) + 8, 8);
}

static void OpMovqWqVq(struct Machine *m, uint32_t rde) {
  if (IsModrmRegister(rde)) {
    memcpy(XmmRexbRm(m, rde), XmmRexrReg(m, rde), 8);
    memset(XmmRexbRm(m, rde) + 8, 0, 8);
  } else {
    memcpy(ComputeReserveAddressWrite8(m, rde), XmmRexrReg(m, rde), 8);
  }
}

static void OpMovq2dqVdqNq(struct Machine *m, uint32_t rde) {
  memcpy(XmmRexrReg(m, rde), MmRm(m, rde), 8);
  memset(XmmRexrReg(m, rde) + 8, 0, 8);
}

static void OpMovdq2qPqUq(struct Machine *m, uint32_t rde) {
  memcpy(MmReg(m, rde), XmmRexbRm(m, rde), 8);
}

static void OpMovapsVpsWps(struct Machine *m, uint32_t rde) {
  OpMovdqaVdqWdq(m, rde);
}

static void OpMovapdVpdWpd(struct Machine *m, uint32_t rde) {
  OpMovdqaVdqWdq(m, rde);
}

static void OpMovapsWpsVps(struct Machine *m, uint32_t rde) {
  OpMovdqaWdqVdq(m, rde);
}

static void OpMovapdWpdVpd(struct Machine *m, uint32_t rde) {
  OpMovdqaWdqVdq(m, rde);
}

static void OpMovWpsVps(struct Machine *m, uint32_t rde) {
  uint8_t *p, *r;
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      OpMovupsWpsVps(m, rde);
      break;
    case 1:
      OpMovupdWpsVps(m, rde);
      break;
    case 2:
      OpMovsdWpsVps(m, rde);
      break;
    case 3:
      OpMovssWpsVps(m, rde);
      break;
    default:
      unreachable;
  }
}

static void OpMov0f28(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    OpMovapsVpsWps(m, rde);
  } else {
    OpMovapdVpdWpd(m, rde);
  }
}

static void OpMov0f6e(struct Machine *m, uint32_t rde) {
  if (Osz(rde)) {
    if (Rexw(rde)) {
      OpMovqVdqEqp(m, rde);
    } else {
      OpMovdVdqEd(m, rde);
    }
  } else {
    if (Rexw(rde)) {
      OpMovqPqEqp(m, rde);
    } else {
      OpMovdPqEd(m, rde);
    }
  }
}

static void OpMov0f6f(struct Machine *m, uint32_t rde) {
  if (Osz(rde)) {
    OpMovdqaVdqWdq(m, rde);
  } else if (Rep(rde) == 3) {
    OpMovdquVdqWdq(m, rde);
  } else {
    OpMovqPqQq(m, rde);
  }
}

static void OpMov0fE7(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    OpMovntqMqPq(m, rde);
  } else {
    OpMovntdqMdqVdq(m, rde);
  }
}

static void OpMov0f7e(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 3) {
    OpMovqVqWq(m, rde);
  } else if (Osz(rde)) {
    if (Rexw(rde)) {
      OpMovqEqpVdq(m, rde);
    } else {
      OpMovdEdVdq(m, rde);
    }
  } else {
    if (Rexw(rde)) {
      OpMovqEqpPq(m, rde);
    } else {
      OpMovdEdPq(m, rde);
    }
  }
}

static void OpMov0f7f(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 3) {
    OpMovdquWdqVdq(m, rde);
  } else if (Osz(rde)) {
    OpMovdqaWdqVdq(m, rde);
  } else {
    OpMovqQqPq(m, rde);
  }
}

static void OpMov0f10(struct Machine *m, uint32_t rde) {
  uint8_t *p, *r;
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      OpMovupsVpsWps(m, rde);
      break;
    case 1:
      OpMovupdVpsWps(m, rde);
      break;
    case 2:
      OpMovsdVpsWps(m, rde);
      break;
    case 3:
      OpMovssVpsWps(m, rde);
      break;
    default:
      unreachable;
  }
}

static void OpMov0f29(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    OpMovapsWpsVps(m, rde);
  } else {
    OpMovapdWpdVpd(m, rde);
  }
}

static void OpMov0f2b(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    OpMovntpsMpsVps(m, rde);
  } else {
    OpMovntpdMpdVpd(m, rde);
  }
}

static void OpMov0f12(struct Machine *m, uint32_t rde) {
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      if (IsModrmRegister(rde)) {
        OpMovhlpsVqUq(m, rde);
      } else {
        OpMovlpsVqMq(m, rde);
      }
      break;
    case 1:
      OpMovlpdVqMq(m, rde);
      break;
    case 2:
      OpMovddupVqWq(m, rde);
      break;
    case 3:
      OpMovsldupVqWq(m, rde);
      break;
    default:
      unreachable;
  }
}

static void OpMov0f13(struct Machine *m, uint32_t rde) {
  if (Osz(rde)) {
    OpMovlpdMqVq(m, rde);
  } else {
    OpMovlpsMqVq(m, rde);
  }
}

static void OpMov0f16(struct Machine *m, uint32_t rde) {
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      if (IsModrmRegister(rde)) {
        OpMovlhpsVqUq(m, rde);
      } else {
        OpMovhpsVqMq(m, rde);
      }
      break;
    case 1:
      OpMovhpdVqMq(m, rde);
      break;
    case 3:
      OpMovshdupVqWq(m, rde);
      break;
    default:
      OpUd(m);
      break;
  }
}

static void OpMov0f17(struct Machine *m, uint32_t rde) {
  if (Osz(rde)) {
    OpMovhpdMqVq(m, rde);
  } else {
    OpMovhpsMqVq(m, rde);
  }
}

static void OpMov0fD6(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 3) {
    OpMovq2dqVdqNq(m, rde);
  } else if (Rep(rde) == 2) {
    OpMovdq2qPqUq(m, rde);
  } else if (Osz(rde)) {
    OpMovqWqVq(m, rde);
  } else {
    OpUd(m);
  }
}

static void OpUnpcklpsd(struct Machine *m, uint32_t rde) {
  uint8_t *a, *b;
  a = XmmRexrReg(m, rde);
  b = GetModrmRegisterXmmPointerRead8(m, rde);
  if (Osz(rde)) {
    memcpy(a + 8, b, 8);
  } else {
    memcpy(a + 4 * 3, b + 4, 4);
    memcpy(a + 4 * 2, a + 4, 4);
    memcpy(a + 4 * 1, b + 0, 4);
  }
}

static void OpUnpckhpsd(struct Machine *m, uint32_t rde) {
  uint8_t *a, *b;
  a = XmmRexrReg(m, rde);
  b = GetModrmRegisterXmmPointerRead16(m, rde);
  if (Osz(rde)) {
    memcpy(a + 0, b + 8, 8);
    memcpy(a + 8, b + 8, 8);
  } else {
    memcpy(a + 4 * 0, a + 4 * 2, 4);
    memcpy(a + 4 * 1, b + 4 * 2, 4);
    memcpy(a + 4 * 2, a + 4 * 3, 4);
    memcpy(a + 4 * 3, b + 4 * 3, 4);
  }
}

static void OpPextrwGdqpUdqIb(struct Machine *m, uint32_t rde) {
  uint8_t i;
  i = m->xedd->op.uimm0;
  i &= Osz(rde) ? 7 : 3;
  Write16(RegRexrReg(m, rde), Read16(XmmRexbRm(m, rde) + i * 2));
}

static void OpPinsrwVdqEwIb(struct Machine *m, uint32_t rde) {
  uint8_t i;
  i = m->xedd->op.uimm0;
  i &= Osz(rde) ? 7 : 3;
  Write16(XmmRexrReg(m, rde) + i * 2,
          Read16(GetModrmRegisterWordPointerRead2(m, rde)));
}

static void OpShuffle(struct Machine *m, uint32_t rde) {
  int16_t q16[4];
  int16_t x16[8];
  int32_t x32[4];
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      memcpy(q16, GetModrmRegisterXmmPointerRead8(m, rde), 8);
      (pshufw)(q16, q16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m, rde), q16, 8);
      break;
    case 1:
      memcpy(x32, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      (pshufd)(x32, x32, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m, rde), x32, 16);
      break;
    case 2:
      memcpy(x16, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      (pshuflw)(x16, x16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m, rde), x16, 16);
      break;
    case 3:
      memcpy(x16, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      (pshufhw)(x16, x16, m->xedd->op.uimm0);
      memcpy(XmmRexrReg(m, rde), x16, 16);
      break;
    default:
      unreachable;
  }
}

static void OpShufps(struct Machine *m, uint32_t rde) {
  shufps((void *)XmmRexrReg(m, rde), (void *)XmmRexrReg(m, rde),
         (void *)GetModrmRegisterXmmPointerRead16(m, rde), m->xedd->op.uimm0);
}

static void OpShufpd(struct Machine *m, uint32_t rde) {
  shufpd((void *)XmmRexrReg(m, rde), (void *)XmmRexrReg(m, rde),
         (void *)GetModrmRegisterXmmPointerRead16(m, rde), m->xedd->op.uimm0);
}

static void OpShufpsd(struct Machine *m, uint32_t rde) {
  if (Osz(rde)) {
    OpShufpd(m, rde);
  } else {
    OpShufps(m, rde);
  }
}

static void OpSqrtpsd(struct Machine *m, uint32_t rde) {
  long i;
  float_v xf;
  double_v xd;
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      memcpy(&xf, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      for (i = 0; i < 4; ++i) xf[i] = sqrtf(xf[i]);
      memcpy(XmmRexrReg(m, rde), &xf, 16);
      break;
    case 1:
      memcpy(&xd, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      for (i = 0; i < 2; ++i) xd[i] = sqrt(xd[i]);
      memcpy(XmmRexrReg(m, rde), &xd, 16);
      break;
    case 2:
      memcpy(&xd, GetModrmRegisterXmmPointerRead8(m, rde), 8);
      xd[0] = sqrt(xd[0]);
      memcpy(XmmRexrReg(m, rde), &xd, 8);
      break;
    case 3:
      memcpy(&xf, GetModrmRegisterXmmPointerRead4(m, rde), 4);
      xf[0] = sqrtf(xf[0]);
      memcpy(XmmRexrReg(m, rde), &xf, 4);
      break;
    default:
      unreachable;
  }
}

static void OpRsqrtps(struct Machine *m, uint32_t rde) {
  float_v x;
  unsigned i;
  if (Rep(rde) != 3) {
    memcpy(&x, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    for (i = 0; i < 4; ++i) x[i] = 1.f / sqrtf(x[i]);
    memcpy(XmmRexrReg(m, rde), &x, 16);
  } else {
    memcpy(&x, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    x[0] = 1.f / sqrtf(x[0]);
    memcpy(XmmRexrReg(m, rde), &x, 4);
  }
}

static void OpRcpps(struct Machine *m, uint32_t rde) {
  float_v x;
  unsigned i;
  if (Rep(rde) != 3) {
    memcpy(&x, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    for (i = 0; i < 4; ++i) x[i] = 1.f / x[i];
    memcpy(XmmRexrReg(m, rde), &x, 16);
  } else {
    memcpy(&x, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    x[0] = 1.f / x[0];
    memcpy(XmmRexrReg(m, rde), &x, 4);
  }
}

static void OpVpsdWpsd(struct Machine *m, uint32_t rde,
                       float_v opf(struct Machine *, float_v, float_v),
                       double_v opd(struct Machine *, double_v, double_v),
                       bool isfloat, bool isdouble) {
  float_v xf, yf;
  double_v xd, yd;
  if (isfloat) {
    memcpy(&xf, XmmRexrReg(m, rde), 16);
    memcpy(&yf, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    xf = opf(m, xf, yf);
    memcpy(XmmRexrReg(m, rde), &xf, 16);
  } else if (isdouble) {
    memcpy(&xd, XmmRexrReg(m, rde), 16);
    memcpy(&yd, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    xd = opd(m, xd, yd);
    memcpy(XmmRexrReg(m, rde), &xd, 16);
  } else {
    OpUd(m);
  }
}

static void OpVpsdWpsd66(struct Machine *m, uint32_t rde,
                         float_v opf(struct Machine *, float_v, float_v),
                         double_v opd(struct Machine *, double_v, double_v)) {
  OpVpsdWpsd(m, rde, opf, opd, !Osz(rde), Osz(rde));
}

static void OpVpsdWpsd66f2(struct Machine *m, uint32_t rde,
                           float_v opf(struct Machine *, float_v, float_v),
                           double_v opd(struct Machine *, double_v, double_v)) {
  OpVpsdWpsd(m, rde, opf, opd, Rep(rde) == 2, Osz(rde));
}

static void OpVspsdWspsd(struct Machine *m, uint32_t rde,
                         float_v opf(struct Machine *, float_v, float_v),
                         double_v opd(struct Machine *, double_v, double_v)) {
  float_v xf, yf;
  double_v xd, yd;
  switch (Rep(rde) | Osz(rde)) {
    case 0:
      memcpy(&yf, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      memcpy(&xf, XmmRexrReg(m, rde), 16);
      xf = opf(m, xf, yf);
      memcpy(XmmRexrReg(m, rde), &xf, 16);
      break;
    case 1:
      memcpy(&yd, GetModrmRegisterXmmPointerRead16(m, rde), 16);
      memcpy(&xd, XmmRexrReg(m, rde), 16);
      xd = opd(m, xd, yd);
      memcpy(XmmRexrReg(m, rde), &xd, 16);
      break;
    case 2:
      memcpy(&yd, GetModrmRegisterXmmPointerRead8(m, rde), 8);
      memcpy(&xd, XmmRexrReg(m, rde), 8);
      xd = opd(m, xd, yd);
      memcpy(XmmRexrReg(m, rde), &xd, 8);
      break;
    case 3:
      memcpy(&yf, GetModrmRegisterXmmPointerRead4(m, rde), 4);
      memcpy(&xf, XmmRexrReg(m, rde), 4);
      xf = opf(m, xf, yf);
      memcpy(XmmRexrReg(m, rde), &xf, 4);
      break;
    default:
      unreachable;
  }
}

static void OpComissVsWs(struct Machine *m, uint32_t rde) {
  float xf, yf;
  double xd, yd;
  uint8_t zf, cf, pf, ie;
  if (!Osz(rde)) {
    memcpy(&xf, XmmRexrReg(m, rde), 4);
    memcpy(&yf, GetModrmRegisterXmmPointerRead4(m, rde), 4);
    if (!isnan(xf) && !isnan(yf)) {
      zf = xf == yf;
      cf = xf < yf;
      pf = false;
      ie = false;
    } else {
      zf = cf = pf = ie = true;
    }
  } else {
    memcpy(&xd, XmmRexrReg(m, rde), 8);
    memcpy(&yd, GetModrmRegisterXmmPointerRead8(m, rde), 8);
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
  unsigned i;
  for (i = 0; i < 4; ++i) {
    x[i] = MIN(x[i], y[i]);
  }
  return x;
}

static double_v OpMinpd(struct Machine *m, double_v x, double_v y) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    x[i] = MIN(x[i], y[i]);
  }
  return x;
}

static float_v OpMaxps(struct Machine *m, float_v x, float_v y) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    x[i] = MAX(x[i], y[i]);
  }
  return x;
}

static double_v OpMaxpd(struct Machine *m, double_v x, double_v y) {
  unsigned i;
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

static void OpAddpsd(struct Machine *m, uint32_t rde) {
  OpVspsdWspsd(m, rde, OpAddps, OpAddpd);
}

static void OpMulpsd(struct Machine *m, uint32_t rde) {
  OpVspsdWspsd(m, rde, OpMulps, OpMulpd);
}

static void OpSubpsd(struct Machine *m, uint32_t rde) {
  OpVspsdWspsd(m, rde, OpSubps, OpSubpd);
}

static void OpDivpsd(struct Machine *m, uint32_t rde) {
  OpVspsdWspsd(m, rde, OpDivps, OpDivpd);
}

static void OpMinpsd(struct Machine *m, uint32_t rde) {
  OpVspsdWspsd(m, rde, OpMinps, OpMinpd);
}

static void OpMaxpsd(struct Machine *m, uint32_t rde) {
  OpVspsdWspsd(m, rde, OpMaxps, OpMaxpd);
}

static void OpCmppsd(struct Machine *m, uint32_t rde) {
  OpVspsdWspsd(m, rde, OpCmpps, OpCmppd);
}

static void OpAndpsd(struct Machine *m, uint32_t rde) {
  OpVpsdWpsd66(m, rde, OpAndps, OpAndpd);
}

static void OpAndnpsd(struct Machine *m, uint32_t rde) {
  OpVpsdWpsd66(m, rde, OpAndnps, OpAndnpd);
}

static void OpOrpsd(struct Machine *m, uint32_t rde) {
  OpVpsdWpsd66(m, rde, OpOrps, OpOrpd);
}

static void OpXorpsd(struct Machine *m, uint32_t rde) {
  OpVpsdWpsd66(m, rde, OpXorps, OpXorpd);
}

static void OpHaddpsd(struct Machine *m, uint32_t rde) {
  OpVpsdWpsd66f2(m, rde, OpHaddps, OpHaddpd);
}

static void OpHsubpsd(struct Machine *m, uint32_t rde) {
  OpVpsdWpsd66f2(m, rde, OpHsubps, OpHsubpd);
}

static void OpAddsubpsd(struct Machine *m, uint32_t rde) {
  OpVpsdWpsd66f2(m, rde, OpAddsubps, OpAddsubpd);
}

static void OpAlub(struct Machine *m, uint32_t rde, int h) {
  uint8_t *a;
  a = GetModrmRegisterBytePointerWrite(m, rde);
  Write8(a, Alu(0, h, Read8(a), Read8(ByteRexrReg(m, rde)), &m->flags));
}

static void OpAlubRo(struct Machine *m, uint32_t rde, int h) {
  Alu(0, h, Read8(GetModrmRegisterBytePointerWrite(m, rde)),
      Read8(ByteRexrReg(m, rde)), &m->flags);
}

static void OpAlubFlip(struct Machine *m, uint32_t rde, int h) {
  Write8(ByteRexrReg(m, rde),
         Alu(0, h, Read8(ByteRexrReg(m, rde)),
             Read8(GetModrmRegisterBytePointerRead(m, rde)), &m->flags));
}

static void OpAlubFlipRo(struct Machine *m, uint32_t rde, int h) {
  Alu(0, h, Read8(ByteRexrReg(m, rde)),
      Read8(GetModrmRegisterBytePointerRead(m, rde)), &m->flags);
}

static void OpAlubi(struct Machine *m, uint32_t rde, int h) {
  uint8_t *a, x;
  a = GetModrmRegisterBytePointerWrite(m, rde);
  x = Alu(0, h, Read8(a), m->xedd->op.uimm0, &m->flags);
  if (h != ALU_CMP) Write8(a, x);
}

static void OpAlubiRo(struct Machine *m, uint32_t rde, int h) {
  Alu(0, h, Read8(GetModrmRegisterBytePointerWrite(m, rde)), m->xedd->op.uimm0,
      &m->flags);
}

static void OpAluw(struct Machine *m, uint32_t rde, int h) {
  uint8_t *a;
  a = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  WriteRegisterOrMemory(rde, a,
                        Alu(RegLog2(rde), h, ReadMemory(rde, a),
                            Read64(RegRexrReg(m, rde)), &m->flags));
}

static void OpAluwRo(struct Machine *m, uint32_t rde, int h) {
  Alu(RegLog2(rde), h,
      ReadMemory(rde, GetModrmRegisterWordPointerWriteOszRexw(m, rde)),
      Read64(RegRexrReg(m, rde)), &m->flags);
}

static void OpAluwFlip(struct Machine *m, uint32_t rde, int h) {
  WriteRegister(
      rde, RegRexrReg(m, rde),
      Alu(RegLog2(rde), h, Read64(RegRexrReg(m, rde)),
          ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)),
          &m->flags));
}

static void OpAluwFlipRo(struct Machine *m, uint32_t rde, int h) {
  Alu(RegLog2(rde), h, Read64(RegRexrReg(m, rde)),
      ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)),
      &m->flags);
}

static void OpAluwi(struct Machine *m, uint32_t rde, int h) {
  uint8_t *a;
  uint64_t x;
  a = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  x = Alu(RegLog2(rde), h, ReadMemory(rde, a), m->xedd->op.uimm0, &m->flags);
  if (h != ALU_CMP) WriteRegisterOrMemory(rde, a, x);
}

static void OpAluwiRo(struct Machine *m, uint32_t rde, int h) {
  Alu(RegLog2(rde), h,
      ReadMemory(rde, GetModrmRegisterWordPointerWriteOszRexw(m, rde)),
      m->xedd->op.uimm0, &m->flags);
}

static void OpBsuwi(struct Machine *m, uint32_t rde, int h, uint64_t yimm) {
  int64_t v;
  void *p[2];
  uint8_t w, *a, split[8];
  w = RegLog2(rde);
  if (IsModrmRegister(rde)) {
    v = 0;
    a = RegRexbRm(m, rde);
  } else {
    v = ComputeAddress(m, rde);
    a = BeginLoadStore(m, v, 1 << w, p, split);
  }
  WriteRegisterOrMemory(rde, a, Bsu(w, h, Read64(a), yimm, &m->flags));
  EndStore(m, v, 1 << w, p, split);
}

static void OpBsubi(struct Machine *m, uint32_t rde, int h, uint64_t yimm) {
  uint8_t *a;
  a = GetModrmRegisterBytePointerWrite(m, rde);
  Write8(a, Bsu(0, h, Read8(a), yimm, &m->flags));
}

static void OpAluAlIb(struct Machine *m, int h) {
  Write8(m->ax, Alu(0, h, Read8(m->ax), m->xedd->op.uimm0, &m->flags));
}

static void OpAluRaxIvds(struct Machine *m, uint32_t rde, int h) {
  WriteRegister(rde, m->ax,
                Alu(RegLog2(rde), h, ReadMemory(rde, m->ax), m->xedd->op.uimm0,
                    &m->flags));
}

static void OpCmpAlIb(struct Machine *m) {
  Alu(0, ALU_SUB, Read8(m->ax), m->xedd->op.uimm0, &m->flags);
}

static void OpCmpRaxIvds(struct Machine *m, uint32_t rde) {
  Alu(RegLog2(rde), ALU_SUB, ReadMemory(rde, m->ax), m->xedd->op.uimm0,
      &m->flags);
}

static void OpTestAlIb(struct Machine *m) {
  Alu(0, ALU_AND, Read8(m->ax), m->xedd->op.uimm0, &m->flags);
}

static void OpTestRaxIvds(struct Machine *m, uint32_t rde) {
  Alu(RegLog2(rde), ALU_AND, ReadMemory(rde, m->ax), m->xedd->op.uimm0,
      &m->flags);
}

long opcount[256 * 4];

void ExecuteInstruction(struct Machine *m) {
  uint32_t rde;
  m->ip += m->xedd->length;
  rde = m->xedd->op.rde;
  opcount[m->xedd->op.map << 8 | m->xedd->op.opcode]++;
  switch (m->xedd->op.map << 8 | m->xedd->op.opcode) {
    CASE(0x089, OpMovEvqpGvqp(m, rde));
    CASE(0x083, OpAluwi(m, rde, ModrmReg(rde)));
    CASR(0x070, if (GetCond(m, 0x0)) OpJmp(m));
    CASR(0x071, if (GetCond(m, 0x1)) OpJmp(m));
    CASR(0x072, if (GetCond(m, 0x2)) OpJmp(m));
    CASR(0x073, if (GetCond(m, 0x3)) OpJmp(m));
    CASR(0x074, if (GetCond(m, 0x4)) OpJmp(m));
    CASR(0x075, if (GetCond(m, 0x5)) OpJmp(m));
    CASR(0x076, if (GetCond(m, 0x6)) OpJmp(m));
    CASR(0x077, if (GetCond(m, 0x7)) OpJmp(m));
    CASR(0x078, if (GetCond(m, 0x8)) OpJmp(m));
    CASR(0x079, if (GetCond(m, 0x9)) OpJmp(m));
    CASR(0x07A, if (GetCond(m, 0xa)) OpJmp(m));
    CASR(0x07B, if (GetCond(m, 0xb)) OpJmp(m));
    CASR(0x07C, if (GetCond(m, 0xc)) OpJmp(m));
    CASR(0x07D, if (GetCond(m, 0xd)) OpJmp(m));
    CASR(0x07E, if (GetCond(m, 0xe)) OpJmp(m));
    CASR(0x07F, if (GetCond(m, 0xf)) OpJmp(m));
    CASR(0x0B0 ... 0x0B7, OpMovZbIb(m, rde));
    CASR(0x0B8 ... 0x0BF, OpMovZvqpIvqp(m, rde));
    CASR(0x050 ... 0x057, OpPushZvq(m, rde));
    CASR(0x058 ... 0x05F, OpPopZvq(m, rde));
    CASR(0x091 ... 0x097, OpXchgZvqp(m, rde));
    CASR(0x1C8 ... 0x1CF, OpBswapZvqp(m, rde));
    CASR(0x000, OpAlub(m, rde, ALU_ADD));
    CASE(0x001, OpAluw(m, rde, ALU_ADD));
    CASR(0x002, OpAlubFlip(m, rde, ALU_ADD));
    CASE(0x003, OpAluwFlip(m, rde, ALU_ADD));
    CASR(0x004, OpAluAlIb(m, ALU_ADD));
    CASR(0x005, OpAluRaxIvds(m, rde, ALU_ADD));
    CASR(0x008, OpAlub(m, rde, ALU_OR));
    CASE(0x009, OpAluw(m, rde, ALU_OR));
    CASR(0x00A, OpAlubFlip(m, rde, ALU_OR));
    CASE(0x00B, OpAluwFlip(m, rde, ALU_OR));
    CASR(0x00C, OpAluAlIb(m, ALU_OR));
    CASR(0x00D, OpAluRaxIvds(m, rde, ALU_OR));
    CASR(0x010, OpAlub(m, rde, ALU_ADC));
    CASE(0x011, OpAluw(m, rde, ALU_ADC));
    CASR(0x012, OpAlubFlip(m, rde, ALU_ADC));
    CASE(0x013, OpAluwFlip(m, rde, ALU_ADC));
    CASR(0x014, OpAluAlIb(m, ALU_ADC));
    CASR(0x015, OpAluRaxIvds(m, rde, ALU_ADC));
    CASR(0x018, OpAlub(m, rde, ALU_SBB));
    CASE(0x019, OpAluw(m, rde, ALU_SBB));
    CASR(0x01A, OpAlubFlip(m, rde, ALU_SBB));
    CASE(0x01B, OpAluwFlip(m, rde, ALU_SBB));
    CASR(0x01C, OpAluAlIb(m, ALU_SBB));
    CASR(0x01D, OpAluRaxIvds(m, rde, ALU_SBB));
    CASR(0x020, OpAlub(m, rde, ALU_AND));
    CASE(0x021, OpAluw(m, rde, ALU_AND));
    CASR(0x022, OpAlubFlip(m, rde, ALU_AND));
    CASE(0x023, OpAluwFlip(m, rde, ALU_AND));
    CASR(0x024, OpAluAlIb(m, ALU_AND));
    CASR(0x025, OpAluRaxIvds(m, rde, ALU_AND));
    CASR(0x028, OpAlub(m, rde, ALU_SUB));
    CASE(0x029, OpAluw(m, rde, ALU_SUB));
    CASR(0x02A, OpAlubFlip(m, rde, ALU_SUB));
    CASE(0x02B, OpAluwFlip(m, rde, ALU_SUB));
    CASR(0x02C, OpAluAlIb(m, ALU_SUB));
    CASR(0x02D, OpAluRaxIvds(m, rde, ALU_SUB));
    CASR(0x030, OpAlub(m, rde, ALU_XOR));
    CASE(0x031, OpAluw(m, rde, ALU_XOR));
    CASR(0x032, OpAlubFlip(m, rde, ALU_XOR));
    CASE(0x033, OpAluwFlip(m, rde, ALU_XOR));
    CASR(0x034, OpAluAlIb(m, ALU_XOR));
    CASR(0x035, OpAluRaxIvds(m, rde, ALU_XOR));
    CASR(0x038, OpAlubRo(m, rde, ALU_CMP));
    CASE(0x039, OpAluwRo(m, rde, ALU_CMP));
    CASR(0x03A, OpAlubFlipRo(m, rde, ALU_CMP));
    CASE(0x03B, OpAluwFlipRo(m, rde, ALU_CMP));
    CASR(0x03C, OpCmpAlIb(m));
    CASR(0x03D, OpCmpRaxIvds(m, rde));
    CASE(0x063, OpMovsxdGdqpEd(m, rde));
    CASE(0x068, PushOsz(m, rde, m->xedd->op.uimm0));
    CASE(0x069, OpImulGvqpEvqpImm(m, rde));
    CASE(0x06A, PushOsz(m, rde, m->xedd->op.uimm0));
    CASE(0x06B, OpImulGvqpEvqpImm(m, rde));
    CASE(0x06C, OpString(m, rde, STRING_INS));
    CASE(0x06D, OpString(m, rde, STRING_INS));
    CASE(0x06E, OpString(m, rde, STRING_OUTS));
    CASE(0x06F, OpString(m, rde, STRING_OUTS));
    CASR(0x080, OpAlubi(m, rde, ModrmReg(rde)));
    CASE(0x081, OpAluwi(m, rde, ModrmReg(rde)));
    CASR(0x082, OpAlubi(m, rde, ModrmReg(rde)));
    CASR(0x084, OpAlubRo(m, rde, TEST));
    CASE(0x085, OpAluwRo(m, rde, TEST));
    CASE(0x086, OpXchgGbEb(m, rde));
    CASE(0x087, OpXchgGvqpEvqp(m, rde));
    CASE(0x088, OpMovEbGb(m, rde));
    CASE(0x08A, OpMovGbEb(m, rde));
    CASE(0x08B, OpMovGvqpEvqp(m, rde));
    CASE(0x08C, OpMovEvqpSw(m));
    CASE(0x08D, OpLeaGvqpM(m, rde));
    CASE(0x08E, OpMovSwEvqp(m));
    CASE(0x08F, OpPopEvq(m, rde));
    CASE(0x090, OpNop(m, rde));
    CASE(0x098, OpConvert1(m, rde));
    CASE(0x099, OpConvert2(m, rde));
    CASE(0x09C, OpPushf(m, rde));
    CASE(0x09D, OpPopf(m, rde));
    CASE(0x09E, OpSahf(m));
    CASE(0x09F, OpLahf(m));
    CASE(0x09B, OpFwait(m));
    CASE(0x0A0, OpMovAlOb(m));
    CASE(0x0A1, OpMovRaxOvqp(m, rde));
    CASE(0x0A2, OpMovObAl(m));
    CASE(0x0A3, OpMovOvqpRax(m, rde));
    CASE(0x0A4, OpMovsb(m, rde));
    CASE(0x0A5, OpString(m, rde, STRING_MOVS));
    CASE(0x0A6, OpString(m, rde, STRING_CMPS));
    CASE(0x0A7, OpString(m, rde, STRING_CMPS));
    CASE(0x0A8, OpTestAlIb(m));
    CASE(0x0A9, OpTestRaxIvds(m, rde));
    CASE(0x0AA, OpStosb(m, rde));
    CASE(0x0AB, OpString(m, rde, STRING_STOS));
    CASE(0x0AC, OpString(m, rde, STRING_LODS));
    CASE(0x0AD, OpString(m, rde, STRING_LODS));
    CASE(0x0AE, OpString(m, rde, STRING_SCAS));
    CASE(0x0AF, OpString(m, rde, STRING_SCAS));
    CASR(0x0C0, OpBsubi(m, rde, ModrmReg(rde), m->xedd->op.uimm0));
    CASR(0x0C1, OpBsuwi(m, rde, ModrmReg(rde), m->xedd->op.uimm0));
    CASE(0x0C2, OpRet(m, m->xedd->op.uimm0));
    CASE(0x0C3, OpRet(m, 0));
    CASE(0x0C6, OpMovEbIb(m, rde));
    CASE(0x0C7, OpMovEvqpIvds(m, rde));
    CASE(0x0C9, OpLeave(m));
    CASE(0x0CC, OpInterrupt(m, 3));
    CASE(0x0CD, OpInterrupt(m, m->xedd->op.uimm0));
    CASR(0x0D0, OpBsubi(m, rde, ModrmReg(rde), 1));
    CASR(0x0D1, OpBsuwi(m, rde, ModrmReg(rde), 1));
    CASR(0x0D2, OpBsubi(m, rde, ModrmReg(rde), m->cx[0]));
    CASR(0x0D3, OpBsuwi(m, rde, ModrmReg(rde), m->cx[0]));
    CASE(0x0D7, OpXlat(m, rde));
    CASE(0x0E0, OpLoop(m, rde, !GetFlag(m->flags, FLAGS_ZF)));
    CASE(0x0E1, OpLoop(m, rde, GetFlag(m->flags, FLAGS_ZF)));
    CASE(0x0E2, OpLoop(m, rde, 1));
    CASE(0x0E3, OpJcxz(m, rde));
    CASE(0x0E4, Write8(m->ax, OpIn(m, m->xedd->op.uimm0)));
    CASE(0x0E5, Write32(m->ax, OpIn(m, m->xedd->op.uimm0)));
    CASE(0x0E6, OpOut(m, m->xedd->op.uimm0, Read8(m->ax)));
    CASE(0x0E7, OpOut(m, m->xedd->op.uimm0, Read32(m->ax)));
    CASE(0x0E8, OpCallJvds(m));
    CASE(0x0E9, OpJmp(m));
    CASE(0x0EB, OpJmp(m));
    CASE(0x0EC, Write8(m->ax, OpIn(m, Read16(m->dx))));
    CASE(0x0ED, Write32(m->ax, OpIn(m, Read16(m->dx))));
    CASE(0x0EE, OpOut(m, Read16(m->dx), Read8(m->ax)));
    CASE(0x0EF, OpOut(m, Read16(m->dx), Read32(m->ax)));
    CASE(0x0F1, OpInterrupt(m, 1));
    CASE(0x0F4, OpHlt(m));
    CASE(0x0F5, OpCmc(m));
    CASE(0x0F8, OpClc(m));
    CASE(0x0F9, OpStc(m));
    CASE(0x0FA, OpCli(m));
    CASE(0x0FB, OpSti(m));
    CASE(0x0FC, OpCld(m));
    CASE(0x0FD, OpStd(m));
    CASE(0x105, OpSyscall(m));
    CASE(0x110, OpMov0f10(m, rde));
    CASE(0x111, OpMovWpsVps(m, rde));
    CASE(0x112, OpMov0f12(m, rde));
    CASE(0x113, OpMov0f13(m, rde));
    CASE(0x114, OpUnpcklpsd(m, rde));
    CASE(0x115, OpUnpckhpsd(m, rde));
    CASE(0x116, OpMov0f16(m, rde));
    CASE(0x117, OpMov0f17(m, rde));
    CASE(0x128, OpMov0f28(m, rde));
    CASE(0x129, OpMovWpsVps(m, rde));
    CASE(0x12A, OpCvt(m, rde, kOpCvt0f2a));
    CASE(0x12B, OpMov0f2b(m, rde));
    CASE(0x12C, OpCvt(m, rde, kOpCvtt0f2c));
    CASE(0x12D, OpCvt(m, rde, kOpCvt0f2d));
    CASE(0x12E, OpComissVsWs(m, rde));
    CASE(0x12F, OpComissVsWs(m, rde));
    CASE(0x131, OpRdtsc(m));
    CASE(0x140, if (GetCond(m, 0x0)) OpMovGvqpEvqp(m, rde));
    CASE(0x141, if (GetCond(m, 0x1)) OpMovGvqpEvqp(m, rde));
    CASE(0x142, if (GetCond(m, 0x2)) OpMovGvqpEvqp(m, rde));
    CASE(0x143, if (GetCond(m, 0x3)) OpMovGvqpEvqp(m, rde));
    CASE(0x144, if (GetCond(m, 0x4)) OpMovGvqpEvqp(m, rde));
    CASE(0x145, if (GetCond(m, 0x5)) OpMovGvqpEvqp(m, rde));
    CASE(0x146, if (GetCond(m, 0x6)) OpMovGvqpEvqp(m, rde));
    CASE(0x147, if (GetCond(m, 0x7)) OpMovGvqpEvqp(m, rde));
    CASE(0x148, if (GetCond(m, 0x8)) OpMovGvqpEvqp(m, rde));
    CASE(0x149, if (GetCond(m, 0x9)) OpMovGvqpEvqp(m, rde));
    CASE(0x14A, if (GetCond(m, 0xa)) OpMovGvqpEvqp(m, rde));
    CASE(0x14B, if (GetCond(m, 0xb)) OpMovGvqpEvqp(m, rde));
    CASE(0x14C, if (GetCond(m, 0xc)) OpMovGvqpEvqp(m, rde));
    CASE(0x14D, if (GetCond(m, 0xd)) OpMovGvqpEvqp(m, rde));
    CASE(0x14E, if (GetCond(m, 0xe)) OpMovGvqpEvqp(m, rde));
    CASE(0x14F, if (GetCond(m, 0xf)) OpMovGvqpEvqp(m, rde));
    CASE(0x151, OpSqrtpsd(m, rde));
    CASE(0x152, OpRsqrtps(m, rde));
    CASE(0x153, OpRcpps(m, rde));
    CASE(0x154, OpAndpsd(m, rde));
    CASE(0x155, OpAndnpsd(m, rde));
    CASE(0x156, OpOrpsd(m, rde));
    CASE(0x157, OpXorpsd(m, rde));
    CASE(0x158, OpAddpsd(m, rde));
    CASE(0x159, OpMulpsd(m, rde));
    CASE(0x15A, OpCvt(m, rde, kOpCvt0f5a));
    CASE(0x15B, OpCvt(m, rde, kOpCvt0f5b));
    CASE(0x15C, OpSubpsd(m, rde));
    CASE(0x15D, OpMinpsd(m, rde));
    CASE(0x15E, OpDivpsd(m, rde));
    CASE(0x15F, OpMaxpsd(m, rde));
    CASR(0x160, OpSse(m, rde, kOpSsePunpcklbw));
    CASR(0x161, OpSse(m, rde, kOpSsePunpcklwd));
    CASR(0x162, OpSse(m, rde, kOpSsePunpckldq));
    CASR(0x163, OpSse(m, rde, kOpSsePacksswb));
    CASR(0x164, OpSse(m, rde, kOpSsePcmpgtb));
    CASR(0x165, OpSse(m, rde, kOpSsePcmpgtw));
    CASR(0x166, OpSse(m, rde, kOpSsePcmpgtd));
    CASR(0x167, OpSse(m, rde, kOpSsePackuswb));
    CASR(0x168, OpSse(m, rde, kOpSsePunpckhbw));
    CASR(0x169, OpSse(m, rde, kOpSsePunpckhwd));
    CASR(0x16A, OpSse(m, rde, kOpSsePunpckhdq));
    CASR(0x16B, OpSse(m, rde, kOpSsePackssdw));
    CASR(0x16C, OpSse(m, rde, kOpSsePunpcklqdq));
    CASR(0x16D, OpSse(m, rde, kOpSsePunpckhqdq));
    CASE(0x16E, OpMov0f6e(m, rde));
    CASE(0x16F, OpMov0f6f(m, rde));
    CASE(0x170, OpShuffle(m, rde));
    CASR(0x174, OpSse(m, rde, kOpSsePcmpeqb));
    CASR(0x175, OpSse(m, rde, kOpSsePcmpeqw));
    CASR(0x176, OpSse(m, rde, kOpSsePcmpeqd));
    CASE(0x17C, OpHaddpsd(m, rde));
    CASE(0x17D, OpHsubpsd(m, rde));
    CASE(0x17E, OpMov0f7e(m, rde));
    CASE(0x17F, OpMov0f7f(m, rde));
    CASE(0x180, if (GetCond(m, 0x0)) OpJmp(m));
    CASE(0x181, if (GetCond(m, 0x1)) OpJmp(m));
    CASE(0x182, if (GetCond(m, 0x2)) OpJmp(m));
    CASE(0x183, if (GetCond(m, 0x3)) OpJmp(m));
    CASE(0x184, if (GetCond(m, 0x4)) OpJmp(m));
    CASE(0x185, if (GetCond(m, 0x5)) OpJmp(m));
    CASE(0x186, if (GetCond(m, 0x6)) OpJmp(m));
    CASE(0x187, if (GetCond(m, 0x7)) OpJmp(m));
    CASE(0x188, if (GetCond(m, 0x8)) OpJmp(m));
    CASE(0x189, if (GetCond(m, 0x9)) OpJmp(m));
    CASE(0x18A, if (GetCond(m, 0xa)) OpJmp(m));
    CASE(0x18B, if (GetCond(m, 0xb)) OpJmp(m));
    CASE(0x18C, if (GetCond(m, 0xc)) OpJmp(m));
    CASE(0x18D, if (GetCond(m, 0xd)) OpJmp(m));
    CASE(0x18E, if (GetCond(m, 0xe)) OpJmp(m));
    CASE(0x18F, if (GetCond(m, 0xf)) OpJmp(m));
    CASE(0x190, OpEbSetCc(m, rde, GetCond(m, 0x0)));
    CASE(0x191, OpEbSetCc(m, rde, GetCond(m, 0x1)));
    CASE(0x192, OpEbSetCc(m, rde, GetCond(m, 0x2)));
    CASE(0x193, OpEbSetCc(m, rde, GetCond(m, 0x3)));
    CASE(0x194, OpEbSetCc(m, rde, GetCond(m, 0x4)));
    CASE(0x195, OpEbSetCc(m, rde, GetCond(m, 0x5)));
    CASE(0x196, OpEbSetCc(m, rde, GetCond(m, 0x6)));
    CASE(0x197, OpEbSetCc(m, rde, GetCond(m, 0x7)));
    CASE(0x198, OpEbSetCc(m, rde, GetCond(m, 0x8)));
    CASE(0x199, OpEbSetCc(m, rde, GetCond(m, 0x9)));
    CASE(0x19A, OpEbSetCc(m, rde, GetCond(m, 0xa)));
    CASE(0x19B, OpEbSetCc(m, rde, GetCond(m, 0xb)));
    CASE(0x19C, OpEbSetCc(m, rde, GetCond(m, 0xc)));
    CASE(0x19D, OpEbSetCc(m, rde, GetCond(m, 0xd)));
    CASE(0x19E, OpEbSetCc(m, rde, GetCond(m, 0xe)));
    CASE(0x19F, OpEbSetCc(m, rde, GetCond(m, 0xf)));
    CASE(0x1A0, OpPushFs(m));
    CASE(0x1A1, OpPopFs(m));
    CASE(0x1A2, OpCpuid(m));
    CASE(0x1A3, OpBit(m, rde));
    CASE(0x1A8, OpPushGs(m));
    CASE(0x1A9, OpPopGs(m));
    CASE(0x1AB, OpBit(m, rde));
    CASE(0x1AF, OpImulGvqpEvqp(m, rde));
    CASE(0x1B0, OpCmpxchgEbAlGb(m, rde));
    CASE(0x1B1, OpCmpxchgEvqpRaxGvqp(m, rde));
    CASE(0x1B3, OpBit(m, rde));
    CASE(0x1B6, OpMovzbGvqpEb(m, rde));
    CASE(0x1B7, OpMovzwGvqpEw(m, rde));
    CASE(0x1BA, OpBit(m, rde));
    CASE(0x1BB, OpBit(m, rde));
    CASE(0x1BC, OpGvqpEvqp(m, rde, AluBsf, MUTATING));
    CASE(0x1BD, OpGvqpEvqp(m, rde, AluBsr, MUTATING));
    CASE(0x1BE, OpMovsbGvqpEb(m, rde));
    CASE(0x1BF, OpMovswGvqpEw(m, rde));
    CASE(0x1C0, OpXaddEbGb(m, rde));
    CASE(0x1C1, OpXaddEvqpGvqp(m, rde));
    CASE(0x1C2, OpCmppsd(m, rde));
    CASE(0x1C3, OpMovntiMdqpGdqp(m, rde));
    CASE(0x1C4, OpPinsrwVdqEwIb(m, rde));
    CASE(0x1C5, OpPextrwGdqpUdqIb(m, rde));
    CASE(0x1C6, OpShufpsd(m, rde));
    CASE(0x1C7, OpCmpxchgDxAx(m, rde));
    CASE(0x1D0, OpAddsubpsd(m, rde));
    CASR(0x1D1, OpSse(m, rde, kOpSsePsrlwv));
    CASR(0x1D2, OpSse(m, rde, kOpSsePsrldv));
    CASR(0x1D3, OpSse(m, rde, kOpSsePsrlqv));
    CASR(0x1D4, OpSse(m, rde, kOpSsePaddq));
    CASR(0x1D5, OpSse(m, rde, kOpSsePmullw));
    CASE(0x1D6, OpMov0fD6(m, rde));
    CASE(0x1D7, OpPmovmskbGdqpNqUdq(m, rde));
    CASR(0x1D8, OpSse(m, rde, kOpSsePsubusb));
    CASR(0x1D9, OpSse(m, rde, kOpSsePsubusw));
    CASR(0x1DA, OpSse(m, rde, kOpSsePminub));
    CASR(0x1DB, OpSse(m, rde, kOpSsePand));
    CASR(0x1DC, OpSse(m, rde, kOpSsePaddusb));
    CASR(0x1DD, OpSse(m, rde, kOpSsePaddusw));
    CASR(0x1DE, OpSse(m, rde, kOpSsePmaxub));
    CASR(0x1DF, OpSse(m, rde, kOpSsePandn));
    CASR(0x1E0, OpSse(m, rde, kOpSsePavgb));
    CASR(0x1E1, OpSse(m, rde, kOpSsePsrawv));
    CASR(0x1E2, OpSse(m, rde, kOpSsePsradv));
    CASR(0x1E3, OpSse(m, rde, kOpSsePavgw));
    CASR(0x1E4, OpSse(m, rde, kOpSsePmulhuw));
    CASR(0x1E5, OpSse(m, rde, kOpSsePmulhw));
    CASE(0x1E6, OpCvt(m, rde, kOpCvt0fE6));
    CASE(0x1E7, OpMov0fE7(m, rde));
    CASR(0x1E8, OpSse(m, rde, kOpSsePsubsb));
    CASR(0x1E9, OpSse(m, rde, kOpSsePsubsw));
    CASR(0x1EA, OpSse(m, rde, kOpSsePminsw));
    CASR(0x1EB, OpSse(m, rde, kOpSsePor));
    CASR(0x1EC, OpSse(m, rde, kOpSsePaddsb));
    CASR(0x1ED, OpSse(m, rde, kOpSsePaddsw));
    CASR(0x1EE, OpSse(m, rde, kOpSsePmaxsw));
    CASR(0x1EF, OpSse(m, rde, kOpSsePxor));
    CASE(0x1F0, OpLddquVdqMdq(m, rde));
    CASR(0x1F1, OpSse(m, rde, kOpSsePsllwv));
    CASR(0x1F2, OpSse(m, rde, kOpSsePslldv));
    CASR(0x1F3, OpSse(m, rde, kOpSsePsllqv));
    CASR(0x1F4, OpSse(m, rde, kOpSsePmuludq));
    CASR(0x1F5, OpSse(m, rde, kOpSsePmaddwd));
    CASR(0x1F6, OpSse(m, rde, kOpSsePsadbw));
    CASE(0x1F7, OpMaskMovDiXmmRegXmmRm(m, rde));
    CASR(0x1F8, OpSse(m, rde, kOpSsePsubb));
    CASR(0x1F9, OpSse(m, rde, kOpSsePsubw));
    CASR(0x1FA, OpSse(m, rde, kOpSsePsubd));
    CASR(0x1FB, OpSse(m, rde, kOpSsePsubq));
    CASR(0x1FC, OpSse(m, rde, kOpSsePaddb));
    CASR(0x1FD, OpSse(m, rde, kOpSsePaddw));
    CASR(0x1FE, OpSse(m, rde, kOpSsePaddd));
    CASR(0x200, OpSse(m, rde, kOpSsePshufb));
    CASR(0x201, OpSse(m, rde, kOpSsePhaddw));
    CASR(0x202, OpSse(m, rde, kOpSsePhaddd));
    CASR(0x203, OpSse(m, rde, kOpSsePhaddsw));
    CASR(0x204, OpSse(m, rde, kOpSsePmaddubsw));
    CASR(0x205, OpSse(m, rde, kOpSsePhsubw));
    CASR(0x206, OpSse(m, rde, kOpSsePhsubd));
    CASR(0x207, OpSse(m, rde, kOpSsePhsubsw));
    CASR(0x208, OpSse(m, rde, kOpSsePsignb));
    CASR(0x209, OpSse(m, rde, kOpSsePsignw));
    CASR(0x20A, OpSse(m, rde, kOpSsePsignd));
    CASR(0x20B, OpSse(m, rde, kOpSsePmulhrsw));
    CASR(0x21C, OpSse(m, rde, kOpSsePabsb));
    CASR(0x21D, OpSse(m, rde, kOpSsePabsw));
    CASR(0x21E, OpSse(m, rde, kOpSsePabsd));
    CASE(0x22A, OpMovntdqaVdqMdq(m, rde));
    CASR(0x240, OpSse(m, rde, kOpSsePmulld));
    CASE(0x30F, OpSsePalignr(m, rde));
    case 0xD8:
    case 0xD9:
    case 0xDA:
    case 0xDB:
    case 0xDC:
    case 0xDD:
    case 0xDE:
    case 0xDF:
      OpFpu(m);
      break;
    case 0xF6:
      switch (ModrmReg(rde)) {
        CASR(0, OpAlubiRo(m, rde, TEST));
        CASR(1, OpAlubiRo(m, rde, TEST));
        CASR(2, OpEb(m, rde, AluNot));
        CASR(3, OpEb(m, rde, AluNeg));
        CASE(4, OpMulAxAlEbUnsigned(m, rde));
        CASE(5, OpMulAxAlEbSigned(m, rde));
        CASE(6, OpDivAlAhAxEbUnsigned(m, rde));
        CASE(7, OpDivAlAhAxEbSigned(m, rde));
        default:
          unreachable;
      }
      break;
    case 0xF7:
      switch (ModrmReg(rde)) {
        CASE(0, OpAluwiRo(m, rde, TEST));
        CASE(1, OpAluwiRo(m, rde, TEST));
        CASE(2, OpEvqp(m, rde, AluNot));
        CASE(3, OpEvqp(m, rde, AluNeg));
        CASE(4, OpMulRdxRaxEvqpUnsigned(m, rde));
        CASE(5, OpMulRdxRaxEvqpSigned(m, rde));
        CASE(6, OpDivRdxRaxEvqpUnsigned(m, rde));
        CASE(7, OpDivRdxRaxEvqpSigned(m, rde));
        default:
          unreachable;
      }
      break;
    case 0xFE:
      switch (ModrmReg(rde)) {
        CASE(0b000, OpEb(m, rde, AluInc));
        CASE(0b001, OpEb(m, rde, AluDec));
        default:
          OpUd(m);
      }
      break;
    case 0xFF:
      switch (ModrmReg(rde)) {
        CASE(0, OpEvqp(m, rde, AluInc));
        CASE(1, OpEvqp(m, rde, AluDec));
        CASE(2, OpCallEq(m, rde));
        CASE(4, OpJmpEq(m, rde));
        CASE(6, OpPushEvq(m, rde));
        default:
          OpUd(m);
      }
      break;
    case 0x10B:
    case 0x1B9:
    case 0x1FF:
      OpUd(m);
      break;
    case 0x10D:
    case 0x118:
    case 0x119:
    case 0x11A:
    case 0x11B:
    case 0x11C:
    case 0x11D:
      OpWutNopEv(m);
      break;
    case 0x11F:
      OpNopEv(m);
      break;
    case 0x171:
      switch (ModrmReg(rde)) {
        CASE(2, OpSseUdqIb(m, rde, kOpSseUdqIbPsrlw));
        CASE(4, OpSseUdqIb(m, rde, kOpSseUdqIbPsraw));
        CASE(6, OpSseUdqIb(m, rde, kOpSseUdqIbPsllw));
        default:
          OpUd(m);
      }
      break;
    case 0x172:
      switch (ModrmReg(rde)) {
        CASE(2, OpSseUdqIb(m, rde, kOpSseUdqIbPsrld));
        CASE(4, OpSseUdqIb(m, rde, kOpSseUdqIbPsrad));
        CASE(6, OpSseUdqIb(m, rde, kOpSseUdqIbPslld));
        default:
          OpUd(m);
      }
      break;
    case 0x173:
      switch (ModrmReg(rde)) {
        CASE(2, OpSseUdqIb(m, rde, kOpSseUdqIbPsrlq));
        CASE(3, OpSseUdqIb(m, rde, kOpSseUdqIbPsrldq));
        CASE(6, OpSseUdqIb(m, rde, kOpSseUdqIbPsllq));
        CASE(7, OpSseUdqIb(m, rde, kOpSseUdqIbPslldq));
        default:
          OpUd(m);
      }
      break;
    case 0x1A4:
    case 0x1A5:
    case 0x1AC:
    case 0x1AD:
      OpEvqpGvqp(m, rde, OpDoubleShift, MUTATING);
      break;
    case 0x1AE:
      switch (ModrmReg(rde)) {
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
    case 0x1B8:
      if (Rep(rde) == 3) {
        OpGvqpEvqp(m, rde, AluPopcnt, MUTATING);
      } else {
        OpUd(m);
      }
      break;
    default:
      OpUd(m);
  }
  if (m->stashaddr) {
    VirtualRecv(m, m->stashaddr, m->stash, m->stashsize);
    m->stashaddr = 0;
  }
}
