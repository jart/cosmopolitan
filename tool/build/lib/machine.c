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
#define REG           ModrmReg(rde)
#define UIMM0         m->xedd->op.uimm0
#define BITS          (8 << RegLog2(rde))
#define SIGN          (1ull << (BITS - 1))
#define MASK          (SIGN | (SIGN - 1))
#define SHIFTMASK     (BITS - 1)
#define TEST          ALU_TEST | ALU_AND
#define ZF            GetFlag(m->flags, FLAGS_ZF)

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

static uint64_t ReadMemory(uint32_t rde, uint8_t p[8]) {
  if (Rexw(rde)) {
    return Read64(p);
  } else if (!Osz(rde)) {
    return Read32(p);
  } else {
    return Read16(p);
  }
}

static int64_t ReadMemorySigned(uint32_t rde, uint8_t p[8]) {
  if (Rexw(rde)) {
    return (int64_t)Read64(p);
  } else if (!Osz(rde)) {
    return (int32_t)Read32(p);
  } else {
    return (int16_t)Read16(p);
  }
}

static void WriteRegister(uint32_t rde, uint8_t p[8], uint64_t x) {
  if (Rexw(rde)) {
    Write64(p, x);
  } else if (!Osz(rde)) {
    Write64(p, x & 0xffffffff);
  } else {
    Write16(p, x);
  }
}

static void WriteMemory(uint32_t rde, uint8_t p[8], uint64_t x) {
  if (Rexw(rde)) {
    Write64(p, x);
  } else if (!Osz(rde)) {
    Write32(p, x);
  } else {
    Write16(p, x);
  }
}

static void WriteRegisterOrMemory(uint32_t rde, uint8_t p[8], uint64_t x) {
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
  uint8_t *dst, *src;
  dst = XmmRexrReg(m, rde);
  src = GetModrmRegisterXmmPointerRead8(m, rde);
  memcpy(dst + 0, src, 8);
  memcpy(dst + 8, src, 8);
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

static void OpShufpsd(struct Machine *m, uint32_t rde) {
  float s[4];
  double d[2];
  if (Osz(rde)) {
    memcpy(d, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    (shufpd)(d, d, m->xedd->op.uimm0);
    memcpy(XmmRexrReg(m, rde), d, 16);
  } else {
    memcpy(s, GetModrmRegisterXmmPointerRead16(m, rde), 16);
    (shufps)(s, s, m->xedd->op.uimm0);
    memcpy(XmmRexrReg(m, rde), s, 16);
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
  unsigned i;
  float_v x;
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
  int i;
  float_v x;
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

static void OpAluw(struct Machine *m, uint32_t rde,
                   int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h) {
  int64_t v;
  void *p[2];
  bool readonly;
  uint64_t x, y, z;
  uint8_t w, *a, *b, *c, split[8];
  w = RegLog2(rde);
  readonly = (h & ALU_TEST) || (h & 7) == ALU_CMP;
  if (IsModrmRegister(rde)) {
    v = 0;
    a = RegRexbRm(m, rde);
  } else {
    v = ComputeAddress(m, rde);
    if (h & ALU_FLIP) {
      a = Load(m, v, 1 << w, split);
    } else {
      a = AccessRam(m, v, 1 << w, p, split, true);
      if (readonly) {
        SetReadAddr(m, v, 1);
      } else {
        SetWriteAddr(m, v, 1);
      }
    }
  }
  b = RegRexrReg(m, rde);
  if (h & ALU_FLIP) {
    c = b;
    b = a;
    a = c;
  }
  y = Read64(b);
  x = Read64(a);
  z = f(w, h, x, y, &m->flags);
  if (!readonly) {
    WriteRegisterOrMemory(rde, a, z);
    EndStore(m, v, 1 << w, p, split);
  }
}

static void OpAlub(struct Machine *m, uint32_t rde,
                   int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h) {
  int64_t v;
  bool readonly;
  uint64_t x, y, z;
  uint8_t *a, *b, *c;
  readonly = (h & ALU_TEST) || (h & 7) == ALU_CMP;
  if (!m->xedd->op.has_modrm || IsModrmRegister(rde)) {
    a = ByteRexbRm(m, rde);
  } else {
    v = ComputeAddress(m, rde);
    a = ResolveAddress(m, v);
    if ((h & ALU_FLIP) || readonly) {
      SetReadAddr(m, v, 1);
    } else {
      SetWriteAddr(m, v, 1);
    }
  }
  b = ByteRexrReg(m, rde);
  if (h & ALU_FLIP) {
    c = b;
    b = a;
    a = c;
  }
  y = Read8(b);
  x = Read8(a);
  z = f(0, h, x, y, &m->flags);
  if (!readonly) Write8(a, z);
}

static void OpAluwi(struct Machine *m, uint32_t rde,
                    int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h,
                    uint64_t yimm) {
  int64_t v;
  void *p[2];
  bool readonly;
  uint64_t x, y, z;
  uint8_t w, *a, *c, split[8];
  w = RegLog2(rde);
  readonly = (h & ALU_TEST) || (h & 7) == ALU_CMP;
  if (!m->xedd->op.has_modrm || IsModrmRegister(rde)) {
    v = 0;
    a = RegRexbRm(m, rde);
  } else {
    v = ComputeAddress(m, rde);
    a = AccessRam(m, v, 1 << w, p, split, true);
    if (readonly) {
      SetReadAddr(m, v, 1 << w);
    } else {
      SetWriteAddr(m, v, 1 << w);
    }
  }
  y = yimm;
  x = Read64(a);
  z = f(w, h, x, y, &m->flags);
  if (!readonly) {
    WriteRegisterOrMemory(rde, a, z);
    EndStore(m, v, 1 << w, p, split);
  }
}

static void OpAlubi(struct Machine *m, uint32_t rde,
                    int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h,
                    uint64_t yimm) {
  int64_t v;
  bool readonly;
  uint64_t x, y, z;
  uint8_t *a, *b, *c;
  readonly = (h & ALU_TEST) || (h & 7) == ALU_CMP;
  if (!m->xedd->op.has_modrm || IsModrmRegister(rde)) {
    a = ByteRexbRm(m, rde);
  } else {
    v = ComputeAddress(m, rde);
    a = ResolveAddress(m, v);
    if (readonly) {
      SetReadAddr(m, v, 1);
    } else {
      SetWriteAddr(m, v, 1);
    }
  }
  y = yimm;
  b = NULL;
  x = Read8(a);
  z = f(0, h, x, y, &m->flags);
  if (!readonly) Write8(a, z);
}

static void OpBsuwi(struct Machine *m, uint32_t rde,
                    int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h,
                    uint64_t yimm) {
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
  WriteRegisterOrMemory(rde, a, f(w, h, Read64(a), yimm, &m->flags));
  EndStore(m, v, 1 << w, p, split);
}

static void OpBsubi(struct Machine *m, uint32_t rde,
                    int64_t f(int, int, uint64_t, uint64_t, uint32_t *), int h,
                    uint64_t yimm) {
  uint8_t *a;
  a = GetModrmRegisterBytePointerWrite(m, rde);
  Write8(a, f(0, h, Read8(a), yimm, &m->flags));
}

void ExecuteInstructionMap0(struct Machine *m, uint32_t rde) {
  switch (m->xedd->op.opcode) {
    CASE(/*0120 0x50*/ 0b01010000 ... 0x57, OpPushZvq(m, rde));
    CASE(/*0130 0x58*/ 0b01011000 ... 0x5f, OpPopZvq(m, rde));
    CASE(/*0143 0x63*/ 0b01100011, OpMovsxdGdqpEd(m, rde));
    CASE(/*0150 0x68*/ 0b01101000, PushOsz(m, rde, m->xedd->op.uimm0));
    CASE(/*0151 0x69*/ 0b01101001, OpImulGvqpEvqpImm(m, rde));
    CASE(/*0152 0x6A*/ 0b01101010, PushOsz(m, rde, m->xedd->op.uimm0));
    CASE(/*0153 0x6B*/ 0b01101011, OpImulGvqpEvqpImm(m, rde));
    CASE(/*0154 0x6C*/ 0b01101100, OpString(m, rde, STRING_INS));
    CASE(/*0155 0x6D*/ 0b01101101, OpString(m, rde, STRING_INS));
    CASE(/*0156 0x6E*/ 0b01101110, OpString(m, rde, STRING_OUTS));
    CASE(/*0157 0x6F*/ 0b01101111, OpString(m, rde, STRING_OUTS));
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
    CASE(/*0200 0x80*/ 0b10000000, OpAlubi(m, rde, Alu, ModrmReg(rde), UIMM0));
    CASE(/*0201 0x81*/ 0b10000001, OpAluwi(m, rde, Alu, ModrmReg(rde), UIMM0));
    CASE(/*0202 0x82*/ 0b10000010, OpAlubi(m, rde, Alu, ModrmReg(rde), UIMM0));
    CASE(/*0203 0x83*/ 0b10000011, OpAluwi(m, rde, Alu, ModrmReg(rde), UIMM0));
    CASE(/*0204 0x84*/ 0b10000100, OpAlub(m, rde, Alu, TEST));
    CASE(/*0205 0x85*/ 0b10000101, OpAluw(m, rde, Alu, TEST));
    CASE(/*0206 0x86*/ 0b10000110, OpXchgGbEb(m, rde));
    CASE(/*0207 0x87*/ 0b10000111, OpXchgGvqpEvqp(m, rde));
    CASE(/*0210 0x88*/ 0b10001000, OpMovEbGb(m, rde));
    CASE(/*0211 0x89*/ 0b10001001, OpMovEvqpGvqp(m, rde));
    CASE(/*0212 0x8A*/ 0b10001010, OpMovGbEb(m, rde));
    CASE(/*0213 0x8B*/ 0b10001011, OpMovGvqpEvqp(m, rde));
    CASE(/*0214 0x8C*/ 0b10001100, OpMovEvqpSw(m));
    CASE(/*0215 0x8D*/ 0b10001101, OpLeaGvqpM(m, rde));
    CASE(/*0216 0x8E*/ 0b10001110, OpMovSwEvqp(m));
    CASE(/*0217 0x8F*/ 0b10001111, OpPopEvq(m, rde));
    CASE(/*0220 0x90*/ 0b10010000, OpNop(m, rde));
    CASE(/*0221 0x91*/ 0b10010001 ... 0x97, OpXchgZvqp(m, rde));
    CASE(/*0230 0x98*/ 0b10011000, OpConvert1(m, rde));
    CASE(/*0231 0x99*/ 0b10011001, OpConvert2(m, rde));
    CASE(/*0234 0x9C*/ 0b10011100, OpPushf(m, rde));
    CASE(/*0235 0x9D*/ 0b10011101, OpPopf(m, rde));
    CASE(/*0236 0x9E*/ 0b10011110, OpSahf(m));
    CASE(/*0237 0x9F*/ 0b10011111, OpLahf(m));
    CASE(/*0233 0x9b*/ 0b10011011, OpFwait(m));
    CASE(/*0240 0xA0*/ 0b10100000, OpMovAlOb(m));
    CASE(/*0241 0xA1*/ 0b10100001, OpMovRaxOvqp(m, rde));
    CASE(/*0242 0xA2*/ 0b10100010, OpMovObAl(m));
    CASE(/*0243 0xA3*/ 0b10100011, OpMovOvqpRax(m, rde));
    CASE(/*0244 0xA4*/ 0b10100100, OpMovsb(m, rde));
    CASE(/*0245 0xA5*/ 0b10100101, OpString(m, rde, STRING_MOVS));
    CASE(/*0246 0xA6*/ 0b10100110, OpString(m, rde, STRING_CMPS));
    CASE(/*0247 0xA7*/ 0b10100111, OpString(m, rde, STRING_CMPS));
    CASE(/*0250 0xA8*/ 0b10101000, OpAlubi(m, rde, Alu, TEST, UIMM0));
    CASE(/*0251 0xA9*/ 0b10101001, OpAluwi(m, rde, Alu, TEST, UIMM0));
    CASE(/*0252 0xAA*/ 0b10101010, OpStosb(m, rde));
    CASE(/*0253 0xAB*/ 0b10101011, OpString(m, rde, STRING_STOS));
    CASE(/*0254 0xAC*/ 0b10101100, OpString(m, rde, STRING_LODS));
    CASE(/*0255 0xAD*/ 0b10101101, OpString(m, rde, STRING_LODS));
    CASE(/*0256 0xAE*/ 0b10101110, OpString(m, rde, STRING_SCAS));
    CASE(/*0257 0xAF*/ 0b10101111, OpString(m, rde, STRING_SCAS));
    CASE(/*0260 0xB0*/ 0b10110000 ... 0b10110111, OpMovZbIb(m, rde));
    CASE(/*0270 0xB8*/ 0b10111000 ... 0b10111111, OpMovZvqpIvqp(m, rde));
    CASE(/*0300 0xC0*/ 0b11000000, OpBsubi(m, rde, Bsu, ModrmReg(rde), UIMM0));
    CASE(/*0301 0xC1*/ 0b11000001, OpBsuwi(m, rde, Bsu, ModrmReg(rde), UIMM0));
    CASE(/*0302 0xC2*/ 0b11000010, OpRet(m, m->xedd->op.uimm0));
    CASE(/*0303 0xC3*/ 0b11000011, OpRet(m, 0));
    CASE(/*0306 0xC6*/ 0b11000110, OpMovEbIb(m, rde));
    CASE(/*0307 0xC7*/ 0b11000111, OpMovEvqpIvds(m, rde));
    CASE(/*0311 0xC9*/ 0b11001001, OpLeave(m));
    CASE(/*0314 0xCC*/ 0b11001100, OpInterrupt(m, 3));
    CASE(/*0315 0xCD*/ 0b11001101, OpInterrupt(m, m->xedd->op.uimm0));
    CASE(/*0320 0xD0*/ 0b11010000, OpBsubi(m, rde, Bsu, ModrmReg(rde), 1));
    CASE(/*0321 0xD1*/ 0b11010001, OpBsuwi(m, rde, Bsu, ModrmReg(rde), 1));
    CASE(/*0322 0xD2*/ 0b11010010, OpBsubi(m, rde, Bsu, REG, m->cx[0]));
    CASE(/*0323 0xD3*/ 0b11010011, OpBsuwi(m, rde, Bsu, REG, m->cx[0]));
    CASE(/*0327 0xD7*/ 0b11010111, OpXlat(m, rde));
    CASE(/*0340 0xE0*/ 0b11100000, OpLoop(m, rde, !ZF));
    CASE(/*0341 0xE1*/ 0b11100001, OpLoop(m, rde, ZF));
    CASE(/*0342 0xE2*/ 0b11100010, OpLoop(m, rde, 1));
    CASE(/*0343 0xE3*/ 0b11100011, OpJcxz(m, rde));
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
      OpAlubi(m, rde, Alu, (m->xedd->op.opcode & 070) >> 3, m->xedd->op.uimm0);
      break;
    case /*0005 0x05*/ 0b00000101:
    case /*0015 0x0D*/ 0b00001101:
    case /*0025 0x15*/ 0b00010101:
    case /*0035 0x1D*/ 0b00011101:
    case /*0045 0x25*/ 0b00100101:
    case /*0055 0x2D*/ 0b00101101:
    case /*0065 0x35*/ 0b00110101:
    case /*0075 0x3D*/ 0b00111101:
      OpAluwi(m, rde, Alu, (m->xedd->op.opcode & 070) >> 3, m->xedd->op.uimm0);
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
      OpAlub(m, rde, Alu,
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
      OpAluw(m, rde, Alu,
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
      switch (ModrmReg(rde)) {
        CASE(0, OpAlubi(m, rde, Alu, TEST, UIMM0));
        CASE(1, OpAlubi(m, rde, Alu, TEST, UIMM0));
        CASE(2, OpEb(m, rde, AluNot));
        CASE(3, OpEb(m, rde, AluNeg));
        CASE(4, OpMulAxAlEbUnsigned(m, rde));
        CASE(5, OpMulAxAlEbSigned(m, rde));
        CASE(6, OpDivAlAhAxEbUnsigned(m, rde));
        CASE(7, OpDivAlAhAxEbSigned(m, rde));
        default:
          unreachable;
      }
      break;
    case /*0367 0xF7*/ 0b11110111:
      switch (ModrmReg(rde)) {
        CASE(0, OpAluwi(m, rde, Alu, TEST, UIMM0));
        CASE(1, OpAluwi(m, rde, Alu, TEST, UIMM0));
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
    case /*0376 0xFE*/ 0b11111110:
      switch (ModrmReg(rde)) {
        CASE(0b000, OpEb(m, rde, AluInc));
        CASE(0b001, OpEb(m, rde, AluDec));
        default:
          OpUd(m);
      }
      break;
    case /*0377 0xFF*/ 0b11111111:
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
    default:
      OpUd(m);
  }
}

void ExecuteInstructionMap1(struct Machine *m, uint32_t rde) {
  switch (m->xedd->op.opcode) {
    CASE(/*0005 0x05*/ 0b00000101, OpSyscall(m));
    CASE(/*0020 0x10*/ 0b00010000, OpMov0f10(m, rde));
    CASE(/*0021 0x11*/ 0b00010001, OpMovWpsVps(m, rde));
    CASE(/*0022 0x12*/ 0b00010010, OpMov0f12(m, rde));
    CASE(/*0023 0x13*/ 0b00010011, OpMov0f13(m, rde));
    CASE(/*0024 0x14*/ 0b00010100, OpUnpcklpsd(m, rde));
    CASE(/*0025 0x15*/ 0b00010101, OpUnpckhpsd(m, rde));
    CASE(/*0026 0x16*/ 0b00010110, OpMov0f16(m, rde));
    CASE(/*0027 0x17*/ 0b00010111, OpMov0f17(m, rde));
    CASE(/*0050 0x28*/ 0b00101000, OpMov0f28(m, rde));
    CASE(/*0051 0x29*/ 0b00101001, OpMovWpsVps(m, rde));
    CASE(/*0052 0x2A*/ 0b00101010, OpCvt(m, rde, kOpCvt0f2a));
    CASE(/*0053 0x2B*/ 0b00101011, OpMov0f2b(m, rde));
    CASE(/*0054 0x2C*/ 0b00101100, OpCvt(m, rde, kOpCvtt0f2c));
    CASE(/*0055 0x2D*/ 0b00101101, OpCvt(m, rde, kOpCvt0f2d));
    CASE(/*0056 0x2E*/ 0b00101110, OpComissVsWs(m, rde));
    CASE(/*0057 0x2F*/ 0b00101111, OpComissVsWs(m, rde));
    CASE(/*0061 0x31*/ 0b00110001, OpRdtsc(m));
    CASE(/*0100 0x40*/ 0b01000000, if (GetCond(m, 0x0)) OpMovGvqpEvqp(m, rde));
    CASE(/*0101 0x41*/ 0b01000001, if (GetCond(m, 0x1)) OpMovGvqpEvqp(m, rde));
    CASE(/*0102 0x42*/ 0b01000010, if (GetCond(m, 0x2)) OpMovGvqpEvqp(m, rde));
    CASE(/*0103 0x43*/ 0b01000011, if (GetCond(m, 0x3)) OpMovGvqpEvqp(m, rde));
    CASE(/*0104 0x44*/ 0b01000100, if (GetCond(m, 0x4)) OpMovGvqpEvqp(m, rde));
    CASE(/*0105 0x45*/ 0b01000101, if (GetCond(m, 0x5)) OpMovGvqpEvqp(m, rde));
    CASE(/*0106 0x46*/ 0b01000110, if (GetCond(m, 0x6)) OpMovGvqpEvqp(m, rde));
    CASE(/*0107 0x47*/ 0b01000111, if (GetCond(m, 0x7)) OpMovGvqpEvqp(m, rde));
    CASE(/*0110 0x48*/ 0b01001000, if (GetCond(m, 0x8)) OpMovGvqpEvqp(m, rde));
    CASE(/*0111 0x49*/ 0b01001001, if (GetCond(m, 0x9)) OpMovGvqpEvqp(m, rde));
    CASE(/*0112 0x4a*/ 0b01001010, if (GetCond(m, 0xa)) OpMovGvqpEvqp(m, rde));
    CASE(/*0113 0x4b*/ 0b01001011, if (GetCond(m, 0xb)) OpMovGvqpEvqp(m, rde));
    CASE(/*0114 0x4c*/ 0b01001100, if (GetCond(m, 0xc)) OpMovGvqpEvqp(m, rde));
    CASE(/*0115 0x4d*/ 0b01001101, if (GetCond(m, 0xd)) OpMovGvqpEvqp(m, rde));
    CASE(/*0116 0x4e*/ 0b01001110, if (GetCond(m, 0xe)) OpMovGvqpEvqp(m, rde));
    CASE(/*0117 0x4f*/ 0b01001111, if (GetCond(m, 0xf)) OpMovGvqpEvqp(m, rde));
    CASE(/*0121 0x51*/ 0b01010001, OpSqrtpsd(m, rde));
    CASE(/*0122 0x52*/ 0b01010010, OpRsqrtps(m, rde));
    CASE(/*0123 0x53*/ 0b01010011, OpRcpps(m, rde));
    CASE(/*0124 0x54*/ 0b01010100, OpAndpsd(m, rde));
    CASE(/*0125 0x55*/ 0b01010101, OpAndnpsd(m, rde));
    CASE(/*0126 0x56*/ 0b01010110, OpOrpsd(m, rde));
    CASE(/*0127 0x57*/ 0b01010111, OpXorpsd(m, rde));
    CASE(/*0130 0x58*/ 0b01011000, OpAddpsd(m, rde));
    CASE(/*0131 0x59*/ 0b01011001, OpMulpsd(m, rde));
    CASE(/*0132 0x5A*/ 0b01011010, OpCvt(m, rde, kOpCvt0f5a));
    CASE(/*0133 0x5B*/ 0b01011011, OpCvt(m, rde, kOpCvt0f5b));
    CASE(/*0134 0x5C*/ 0b01011100, OpSubpsd(m, rde));
    CASE(/*0135 0x5D*/ 0b01011101, OpMinpsd(m, rde));
    CASE(/*0136 0x5E*/ 0b01011110, OpDivpsd(m, rde));
    CASE(/*0137 0x5F*/ 0b01011111, OpMaxpsd(m, rde));
    CASE(/*0140 0x60*/ 0b01100000, OpSse(m, rde, kOpSsePunpcklbw));
    CASE(/*0141 0x61*/ 0b01100001, OpSse(m, rde, kOpSsePunpcklwd));
    CASE(/*0142 0x62*/ 0b01100010, OpSse(m, rde, kOpSsePunpckldq));
    CASE(/*0143 0x63*/ 0b01100011, OpSse(m, rde, kOpSsePacksswb));
    CASE(/*0144 0x64*/ 0b01100100, OpSse(m, rde, kOpSsePcmpgtb));
    CASE(/*0145 0x65*/ 0b01100101, OpSse(m, rde, kOpSsePcmpgtw));
    CASE(/*0146 0x66*/ 0b01100110, OpSse(m, rde, kOpSsePcmpgtd));
    CASE(/*0147 0x67*/ 0b01100111, OpSse(m, rde, kOpSsePackuswb));
    CASE(/*0150 0x68*/ 0b01101000, OpSse(m, rde, kOpSsePunpckhbw));
    CASE(/*0151 0x69*/ 0b01101001, OpSse(m, rde, kOpSsePunpckhwd));
    CASE(/*0152 0x6A*/ 0b01101010, OpSse(m, rde, kOpSsePunpckhdq));
    CASE(/*0153 0x6B*/ 0b01101011, OpSse(m, rde, kOpSsePackssdw));
    CASE(/*0154 0x6C*/ 0b01101100, OpSse(m, rde, kOpSsePunpcklqdq));
    CASE(/*0155 0x6D*/ 0b01101101, OpSse(m, rde, kOpSsePunpckhqdq));
    CASE(/*0156 0x6E*/ 0b01101110, OpMov0f6e(m, rde));
    CASE(/*0157 0x6F*/ 0b01101111, OpMov0f6f(m, rde));
    CASE(/*0160 0x70*/ 0b01110000, OpShuffle(m, rde));
    CASE(/*0164 0x74*/ 0b01110100, OpSse(m, rde, kOpSsePcmpeqb));
    CASE(/*0165 0x75*/ 0b01110101, OpSse(m, rde, kOpSsePcmpeqw));
    CASE(/*0166 0x76*/ 0b01110110, OpSse(m, rde, kOpSsePcmpeqd));
    CASE(/*0174 0x7C*/ 0b01111100, OpHaddpsd(m, rde));
    CASE(/*0175 0x7D*/ 0b01111101, OpHsubpsd(m, rde));
    CASE(/*0176 0x7E*/ 0b01111110, OpMov0f7e(m, rde));
    CASE(/*0177 0x7F*/ 0b01111111, OpMov0f7f(m, rde));
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
    CASE(/*0220 0x90*/ 0b10010000, OpEbSetCc(m, rde, GetCond(m, 0x0)));
    CASE(/*0221 0x91*/ 0b10010001, OpEbSetCc(m, rde, GetCond(m, 0x1)));
    CASE(/*0222 0x92*/ 0b10010010, OpEbSetCc(m, rde, GetCond(m, 0x2)));
    CASE(/*0223 0x93*/ 0b10010011, OpEbSetCc(m, rde, GetCond(m, 0x3)));
    CASE(/*0224 0x94*/ 0b10010100, OpEbSetCc(m, rde, GetCond(m, 0x4)));
    CASE(/*0225 0x95*/ 0b10010101, OpEbSetCc(m, rde, GetCond(m, 0x5)));
    CASE(/*0226 0x96*/ 0b10010110, OpEbSetCc(m, rde, GetCond(m, 0x6)));
    CASE(/*0227 0x97*/ 0b10010111, OpEbSetCc(m, rde, GetCond(m, 0x7)));
    CASE(/*0230 0x98*/ 0b10011000, OpEbSetCc(m, rde, GetCond(m, 0x8)));
    CASE(/*0231 0x99*/ 0b10011001, OpEbSetCc(m, rde, GetCond(m, 0x9)));
    CASE(/*0232 0x9A*/ 0b10011010, OpEbSetCc(m, rde, GetCond(m, 0xa)));
    CASE(/*0233 0x9B*/ 0b10011011, OpEbSetCc(m, rde, GetCond(m, 0xb)));
    CASE(/*0234 0x9C*/ 0b10011100, OpEbSetCc(m, rde, GetCond(m, 0xc)));
    CASE(/*0235 0x9D*/ 0b10011101, OpEbSetCc(m, rde, GetCond(m, 0xd)));
    CASE(/*0236 0x9E*/ 0b10011110, OpEbSetCc(m, rde, GetCond(m, 0xe)));
    CASE(/*0237 0x9F*/ 0b10011111, OpEbSetCc(m, rde, GetCond(m, 0xf)));
    CASE(/*0240 0xA0*/ 0b10100000, OpPushFs(m));
    CASE(/*0241 0xA1*/ 0b10100001, OpPopFs(m));
    CASE(/*0242 0xA2*/ 0b10100010, OpCpuid(m));
    CASE(/*0243 0xA3*/ 0b10100011, OpBit(m, rde));
    CASE(/*0250 0xA8*/ 0b10101000, OpPushGs(m));
    CASE(/*0251 0xA9*/ 0b10101001, OpPopGs(m));
    CASE(/*0253 0xAB*/ 0b10101011, OpBit(m, rde));
    CASE(/*0257 0xAF*/ 0b10101111, OpImulGvqpEvqp(m, rde));
    CASE(/*0260 0xB0*/ 0b10110000, OpCmpxchgEbAlGb(m, rde));
    CASE(/*0261 0xB1*/ 0b10110001, OpCmpxchgEvqpRaxGvqp(m, rde));
    CASE(/*0263 0xB3*/ 0b10110011, OpBit(m, rde));
    CASE(/*0266 0xB6*/ 0b10110110, OpMovzbGvqpEb(m, rde));
    CASE(/*0267 0xB7*/ 0b10110111, OpMovzwGvqpEw(m, rde));
    CASE(/*0272 0xBA*/ 0b10111010, OpBit(m, rde));
    CASE(/*0273 0xBB*/ 0b10111011, OpBit(m, rde));
    CASE(/*0274 0xBC*/ 0b10111100, OpGvqpEvqp(m, rde, AluBsf, MUTATING));
    CASE(/*0275 0xBD*/ 0b10111101, OpGvqpEvqp(m, rde, AluBsr, MUTATING));
    CASE(/*0276 0xBE*/ 0b10111110, OpMovsbGvqpEb(m, rde));
    CASE(/*0277 0xBF*/ 0b10111111, OpMovswGvqpEw(m, rde));
    CASE(/*0300 0xC0*/ 0b11000000, OpXaddEbGb(m, rde));
    CASE(/*0301 0xC1*/ 0b11000001, OpXaddEvqpGvqp(m, rde));
    CASE(/*0302 0xC2*/ 0b11000010, OpCmppsd(m, rde));
    CASE(/*0303 0xC3*/ 0b11000011, OpMovntiMdqpGdqp(m, rde));
    CASE(/*0304 0xC4*/ 0b11000100, OpPinsrwVdqEwIb(m, rde));
    CASE(/*0305 0xC5*/ 0b11000101, OpPextrwGdqpUdqIb(m, rde));
    CASE(/*0306 0xC6*/ 0b11000110, OpShufpsd(m, rde));
    CASE(/*0307 0xC7*/ 0b11000111, OpCmpxchgDxAx(m, rde));
    CASE(/*0310 0xC8*/ 0b11001000 ... 0b11001111, OpBswapZvqp(m, rde));
    CASE(/*0320 0xD0*/ 0b11010000, OpAddsubpsd(m, rde));
    CASE(/*0321 0xD1*/ 0b11010001, OpSse(m, rde, kOpSsePsrlwv));
    CASE(/*0322 0xD2*/ 0b11010010, OpSse(m, rde, kOpSsePsrldv));
    CASE(/*0323 0xD3*/ 0b11010011, OpSse(m, rde, kOpSsePsrlqv));
    CASE(/*0324 0xD4*/ 0b11010100, OpSse(m, rde, kOpSsePaddq));
    CASE(/*0325 0xD5*/ 0b11010101, OpSse(m, rde, kOpSsePmullw));
    CASE(/*0326 0xD6*/ 0b11010110, OpMov0fD6(m, rde));
    CASE(/*0327 0xD7*/ 0b11010111, OpPmovmskbGdqpNqUdq(m, rde));
    CASE(/*0330 0xD8*/ 0b11011000, OpSse(m, rde, kOpSsePsubusb));
    CASE(/*0331 0xD9*/ 0b11011001, OpSse(m, rde, kOpSsePsubusw));
    CASE(/*0332 0xDA*/ 0b11011010, OpSse(m, rde, kOpSsePminub));
    CASE(/*0333 0xDB*/ 0b11011011, OpSse(m, rde, kOpSsePand));
    CASE(/*0334 0xDC*/ 0b11011100, OpSse(m, rde, kOpSsePaddusb));
    CASE(/*0335 0xDD*/ 0b11011101, OpSse(m, rde, kOpSsePaddusw));
    CASE(/*0336 0xDE*/ 0b11011110, OpSse(m, rde, kOpSsePmaxub));
    CASE(/*0337 0xDF*/ 0b11011111, OpSse(m, rde, kOpSsePandn));
    CASE(/*0340 0xE0*/ 0b11100000, OpSse(m, rde, kOpSsePavgb));
    CASE(/*0341 0xE1*/ 0b11100001, OpSse(m, rde, kOpSsePsrawv));
    CASE(/*0342 0xE2*/ 0b11100010, OpSse(m, rde, kOpSsePsradv));
    CASE(/*0343 0xE3*/ 0b11100011, OpSse(m, rde, kOpSsePavgw));
    CASE(/*0344 0xE4*/ 0b11100100, OpSse(m, rde, kOpSsePmulhuw));
    CASE(/*0345 0xE5*/ 0b11100101, OpSse(m, rde, kOpSsePmulhw));
    CASE(/*0346 0xE6*/ 0b11100110, OpCvt(m, rde, kOpCvt0fE6));
    CASE(/*0347 0xE7*/ 0b11100111, OpMov0fE7(m, rde));
    CASE(/*0350 0xE8*/ 0b11101000, OpSse(m, rde, kOpSsePsubsb));
    CASE(/*0351 0xE9*/ 0b11101001, OpSse(m, rde, kOpSsePsubsw));
    CASE(/*0352 0xEA*/ 0b11101010, OpSse(m, rde, kOpSsePminsw));
    CASE(/*0353 0xEB*/ 0b11101011, OpSse(m, rde, kOpSsePor));
    CASE(/*0354 0xEC*/ 0b11101100, OpSse(m, rde, kOpSsePaddsb));
    CASE(/*0355 0xED*/ 0b11101101, OpSse(m, rde, kOpSsePaddsw));
    CASE(/*0356 0xEE*/ 0b11101110, OpSse(m, rde, kOpSsePmaxsw));
    CASE(/*0357 0xEF*/ 0b11101111, OpSse(m, rde, kOpSsePxor));
    CASE(/*0360 0xF0*/ 0b11110000, OpLddquVdqMdq(m, rde));
    CASE(/*0361 0xF1*/ 0b11110001, OpSse(m, rde, kOpSsePsllwv));
    CASE(/*0362 0xF2*/ 0b11110010, OpSse(m, rde, kOpSsePslldv));
    CASE(/*0363 0xF3*/ 0b11110011, OpSse(m, rde, kOpSsePsllqv));
    CASE(/*0364 0xF4*/ 0b11110100, OpSse(m, rde, kOpSsePmuludq));
    CASE(/*0365 0xF5*/ 0b11110101, OpSse(m, rde, kOpSsePmaddwd));
    CASE(/*0366 0xF6*/ 0b11110110, OpSse(m, rde, kOpSsePsadbw));
    CASE(/*0367 0xF7*/ 0b11110111, OpMaskMovDiXmmRegXmmRm(m, rde));
    CASE(/*0370 0xF8*/ 0b11111000, OpSse(m, rde, kOpSsePsubb));
    CASE(/*0371 0xF9*/ 0b11111001, OpSse(m, rde, kOpSsePsubw));
    CASE(/*0372 0xFA*/ 0b11111010, OpSse(m, rde, kOpSsePsubd));
    CASE(/*0373 0xFB*/ 0b11111011, OpSse(m, rde, kOpSsePsubq));
    CASE(/*0374 0xFC*/ 0b11111100, OpSse(m, rde, kOpSsePaddb));
    CASE(/*0375 0xFD*/ 0b11111101, OpSse(m, rde, kOpSsePaddw));
    CASE(/*0376 0xFE*/ 0b11111110, OpSse(m, rde, kOpSsePaddd));
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
      switch (ModrmReg(rde)) {
        CASE(2, OpSseUdqIb(m, rde, kOpSseUdqIbPsrlw));
        CASE(4, OpSseUdqIb(m, rde, kOpSseUdqIbPsraw));
        CASE(6, OpSseUdqIb(m, rde, kOpSseUdqIbPsllw));
        default:
          OpUd(m);
      }
      break;
    case /*0162 0x72*/ 0b01110010:
      switch (ModrmReg(rde)) {
        CASE(2, OpSseUdqIb(m, rde, kOpSseUdqIbPsrld));
        CASE(4, OpSseUdqIb(m, rde, kOpSseUdqIbPsrad));
        CASE(6, OpSseUdqIb(m, rde, kOpSseUdqIbPslld));
        default:
          OpUd(m);
      }
      break;
    case /*0163 0x73*/ 0b01110011:
      switch (ModrmReg(rde)) {
        CASE(2, OpSseUdqIb(m, rde, kOpSseUdqIbPsrlq));
        CASE(3, OpSseUdqIb(m, rde, kOpSseUdqIbPsrldq));
        CASE(6, OpSseUdqIb(m, rde, kOpSseUdqIbPsllq));
        CASE(7, OpSseUdqIb(m, rde, kOpSseUdqIbPslldq));
        default:
          OpUd(m);
      }
      break;
    case /*0244 0xA4*/ 0b10100100:
    case /*0245 0xA5*/ 0b10100101:
    case /*0254 0xAC*/ 0b10101100:
    case /*0255 0xAD*/ 0b10101101:
      OpEvqpGvqp(m, rde, OpDoubleShift, MUTATING);
      break;
    case /*0256 0xAE*/ 0b10101110:
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
    case 0xB8:
      if (Rep(rde) == 3) {
        OpGvqpEvqp(m, rde, AluPopcnt, MUTATING);
      } else {
        OpUd(m);
      }
      break;
    default:
      OpUd(m);
  }
}

void ExecuteInstructionMap2(struct Machine *m, uint32_t rde) {
  switch (m->xedd->op.opcode) {
    CASE(0x00, OpSse(m, rde, kOpSsePshufb));
    CASE(0x01, OpSse(m, rde, kOpSsePhaddw));
    CASE(0x02, OpSse(m, rde, kOpSsePhaddd));
    CASE(0x03, OpSse(m, rde, kOpSsePhaddsw));
    CASE(0x04, OpSse(m, rde, kOpSsePmaddubsw));
    CASE(0x05, OpSse(m, rde, kOpSsePhsubw));
    CASE(0x06, OpSse(m, rde, kOpSsePhsubd));
    CASE(0x07, OpSse(m, rde, kOpSsePhsubsw));
    CASE(0x08, OpSse(m, rde, kOpSsePsignb));
    CASE(0x09, OpSse(m, rde, kOpSsePsignw));
    CASE(0x0A, OpSse(m, rde, kOpSsePsignd));
    CASE(0x0B, OpSse(m, rde, kOpSsePmulhrsw));
    CASE(0x1C, OpSse(m, rde, kOpSsePabsb));
    CASE(0x1D, OpSse(m, rde, kOpSsePabsw));
    CASE(0x1E, OpSse(m, rde, kOpSsePabsd));
    CASE(0x2A, OpMovntdqaVdqMdq(m, rde));
    CASE(0x40, OpSse(m, rde, kOpSsePmulld));
    default:
      OpUd(m);
  }
}

void ExecuteInstructionMap3(struct Machine *m, uint32_t rde) {
  switch (m->xedd->op.opcode) {
    CASE(0x0F, OpSsePalignr(m, rde));
    default:
      OpUd(m);
  }
}

void ExecuteInstructionMapUndefined(struct Machine *m, uint32_t rde) {
  OpUd(m);
}

static const map_f kExecuteInstructionMap[16] = {
    ExecuteInstructionMap0,         ExecuteInstructionMap1,
    ExecuteInstructionMap2,         ExecuteInstructionMap3,
    ExecuteInstructionMapUndefined, ExecuteInstructionMapUndefined,
    ExecuteInstructionMapUndefined, ExecuteInstructionMapUndefined,
    ExecuteInstructionMapUndefined, ExecuteInstructionMapUndefined,
    ExecuteInstructionMapUndefined, ExecuteInstructionMapUndefined,
    ExecuteInstructionMapUndefined, ExecuteInstructionMapUndefined,
    ExecuteInstructionMapUndefined, ExecuteInstructionMapUndefined,
};

void ExecuteInstruction(struct Machine *m) {
  uint8_t *p;
  m->ip += m->xedd->length;
  kExecuteInstructionMap[m->xedd->op.map & 15](m, m->xedd->op.rde);
  if (m->stashaddr) {
    VirtualRecv(m, m->stashaddr, m->stash, m->stashsize);
    m->stashaddr = 0;
  }
}
