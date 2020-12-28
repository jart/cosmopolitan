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
#include "libc/macros.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "tool/build/lib/abp.h"
#include "tool/build/lib/address.h"
#include "tool/build/lib/alu.h"
#include "tool/build/lib/bcd.h"
#include "tool/build/lib/bitscan.h"
#include "tool/build/lib/case.h"
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
#include "tool/build/lib/op101.h"
#include "tool/build/lib/sse.h"
#include "tool/build/lib/ssefloat.h"
#include "tool/build/lib/ssemov.h"
#include "tool/build/lib/stack.h"
#include "tool/build/lib/stats.h"
#include "tool/build/lib/string.h"
#include "tool/build/lib/syscall.h"
#include "tool/build/lib/throw.h"
#include "tool/build/lib/time.h"

#define OpLfence    OpNoop
#define OpMfence    OpNoop
#define OpSfence    OpNoop
#define OpClflush   OpNoop
#define OpHintNopEv OpNoop

typedef void (*nexgen32e_f)(struct Machine *, uint32_t);

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

static bool IsParity(struct Machine *m) {
  return GetFlag(m->flags, FLAGS_PF);
}

static bool IsBelowOrEqual(struct Machine *m) {
  return GetFlag(m->flags, FLAGS_CF) | GetFlag(m->flags, FLAGS_ZF);
}

static bool IsAbove(struct Machine *m) {
  return !GetFlag(m->flags, FLAGS_CF) && !GetFlag(m->flags, FLAGS_ZF);
}

static bool IsLess(struct Machine *m) {
  return GetFlag(m->flags, FLAGS_SF) != GetFlag(m->flags, FLAGS_OF);
}

static bool IsGreaterOrEqual(struct Machine *m) {
  return GetFlag(m->flags, FLAGS_SF) == GetFlag(m->flags, FLAGS_OF);
}

static bool IsLessOrEqual(struct Machine *m) {
  return GetFlag(m->flags, FLAGS_ZF) |
         (GetFlag(m->flags, FLAGS_SF) != GetFlag(m->flags, FLAGS_OF));
}

static bool IsGreater(struct Machine *m) {
  return !GetFlag(m->flags, FLAGS_ZF) &
         (GetFlag(m->flags, FLAGS_SF) == GetFlag(m->flags, FLAGS_OF));
}

static void OpNoop(struct Machine *m, uint32_t rde) {
}

static void OpCmc(struct Machine *m, uint32_t rde) {
  m->flags = SetFlag(m->flags, FLAGS_CF, !GetFlag(m->flags, FLAGS_CF));
}

static void OpClc(struct Machine *m, uint32_t rde) {
  m->flags = SetFlag(m->flags, FLAGS_CF, false);
}

static void OpStc(struct Machine *m, uint32_t rde) {
  m->flags = SetFlag(m->flags, FLAGS_CF, true);
}

static void OpCli(struct Machine *m, uint32_t rde) {
  m->flags = SetFlag(m->flags, FLAGS_IF, false);
}

static void OpSti(struct Machine *m, uint32_t rde) {
  m->flags = SetFlag(m->flags, FLAGS_IF, true);
}

static void OpCld(struct Machine *m, uint32_t rde) {
  m->flags = SetFlag(m->flags, FLAGS_DF, false);
}

static void OpStd(struct Machine *m, uint32_t rde) {
  m->flags = SetFlag(m->flags, FLAGS_DF, true);
}

static void OpPushf(struct Machine *m, uint32_t rde) {
  Push(m, rde, ExportFlags(m->flags) & 0xFCFFFF);
}

static void OpPopf(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    ImportFlags(m, Pop(m, rde, 0));
  } else {
    ImportFlags(m, (m->flags & ~0xffff) | Pop(m, rde, 0));
  }
}

static void OpLahf(struct Machine *m, uint32_t rde) {
  Write8(m->ax + 1, ExportFlags(m->flags));
}

static void OpSahf(struct Machine *m, uint32_t rde) {
  ImportFlags(m, (m->flags & ~0xff) | m->ax[1]);
}

static void OpLeaGvqpM(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexrReg(m, rde), LoadEffectiveAddress(m, rde).addr);
}

static relegated void OpPushSeg(struct Machine *m, uint32_t rde) {
  uint8_t seg = (m->xedd->op.opcode & 070) >> 3;
  Push(m, rde, Read64(GetSegment(m, rde, seg)) >> 4);
}

static relegated void OpPopSeg(struct Machine *m, uint32_t rde) {
  uint8_t seg = (m->xedd->op.opcode & 070) >> 3;
  Write64(GetSegment(m, rde, seg), Pop(m, rde, 0) << 4);
}

static relegated void OpMovEvqpSw(struct Machine *m, uint32_t rde) {
  WriteRegisterOrMemory(rde, GetModrmRegisterWordPointerWriteOszRexw(m, rde),
                        Read64(GetSegment(m, rde, ModrmReg(rde))) >> 4);
}

static relegated int GetDescriptor(struct Machine *m, int selector,
                                   uint64_t *out_descriptor) {
  uint8_t buf[8];
  DCHECK(m->gdt_base + m->gdt_limit <= m->real.n);
  selector &= -8;
  if (8 <= selector && selector + 8 <= m->gdt_limit) {
    SetReadAddr(m, m->gdt_base + selector, 8);
    *out_descriptor = Read64(m->real.p + m->gdt_base + selector);
    return 0;
  } else {
    return -1;
  }
}

static uint64_t GetDescriptorBase(uint64_t d) {
  return (d & 0xff00000000000000) >> 32 | (d & 0x000000ffffff0000) >> 16;
}

static uint64_t GetDescriptorLimit(uint64_t d) {
  return (d & 0x000f000000000000) >> 32 | d & 0xffff;
}

static int GetDescriptorMode(uint64_t d) {
  uint8_t kMode[] = {
      XED_MACHINE_MODE_REAL,
      XED_MACHINE_MODE_LONG_64,
      XED_MACHINE_MODE_LEGACY_32,
      XED_MACHINE_MODE_LONG_64,
  };
  return kMode[(d & 0x0060000000000000) >> 53];
}

static bool IsProtectedMode(struct Machine *m) {
  return m->cr0 & 1;
}

static relegated void OpMovSwEvqp(struct Machine *m, uint32_t rde) {
  uint64_t x, d;
  x = ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde));
  if (!IsProtectedMode(m)) {
    x <<= 4;
  } else if (GetDescriptor(m, x, &d) != -1) {
    x = GetDescriptorBase(d);
  } else {
    ThrowProtectionFault(m);
  }
  Write64(GetSegment(m, rde, ModrmReg(rde)), x);
}

static void OpLsl(struct Machine *m, uint32_t rde) {
  uint64_t descriptor;
  if (GetDescriptor(m, Read16(GetModrmRegisterWordPointerRead2(m, rde)),
                    &descriptor) != -1) {
    WriteRegister(rde, RegRexrReg(m, rde), GetDescriptorLimit(descriptor));
    SetFlag(m->flags, FLAGS_ZF, true);
  } else {
    SetFlag(m->flags, FLAGS_ZF, false);
  }
}

static void ChangeMachineMode(struct Machine *m, int mode) {
  if (mode == m->mode) return;
  ResetInstructionCache(m);
  m->mode = mode;
}

static void OpJmpf(struct Machine *m, uint32_t rde) {
  uint64_t descriptor;
  if (!IsProtectedMode(m)) {
    Write64(m->cs, m->xedd->op.uimm0 << 4);
    m->ip = m->xedd->op.disp;
  } else if (GetDescriptor(m, m->xedd->op.uimm0, &descriptor) != -1) {
    Write64(m->cs, GetDescriptorBase(descriptor));
    m->ip = m->xedd->op.disp;
    ChangeMachineMode(m, GetDescriptorMode(descriptor));
  } else {
    ThrowProtectionFault(m);
  }
  if (m->onlongbranch) {
    m->onlongbranch(m);
  }
}

static relegated void OpXlatAlBbb(struct Machine *m, uint32_t rde) {
  uint64_t v;
  v = MaskAddress(Eamode(rde), Read64(m->bx) + Read8(m->ax));
  v = DataSegment(m, rde, v);
  SetReadAddr(m, v, 1);
  Write8(m->ax, Read8(ResolveAddress(m, v)));
}

static void WriteEaxAx(struct Machine *m, uint32_t rde, uint32_t x) {
  if (!Osz(rde)) {
    Write64(m->ax, x);
  } else {
    Write16(m->ax, x);
  }
}

static uint32_t ReadEaxAx(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    return Read32(m->ax);
  } else {
    return Read16(m->ax);
  }
}

static void OpInAlImm(struct Machine *m, uint32_t rde) {
  Write8(m->ax, OpIn(m, m->xedd->op.uimm0));
}

static void OpInAxImm(struct Machine *m, uint32_t rde) {
  WriteEaxAx(m, rde, OpIn(m, m->xedd->op.uimm0));
}

static void OpInAlDx(struct Machine *m, uint32_t rde) {
  Write8(m->ax, OpIn(m, Read16(m->dx)));
}

static void OpInAxDx(struct Machine *m, uint32_t rde) {
  WriteEaxAx(m, rde, OpIn(m, Read16(m->dx)));
}

static void OpOutImmAl(struct Machine *m, uint32_t rde) {
  OpOut(m, m->xedd->op.uimm0, Read8(m->ax));
}

static void OpOutImmAx(struct Machine *m, uint32_t rde) {
  OpOut(m, m->xedd->op.uimm0, ReadEaxAx(m, rde));
}

static void OpOutDxAl(struct Machine *m, uint32_t rde) {
  OpOut(m, Read16(m->dx), Read8(m->ax));
}

static void OpOutDxAx(struct Machine *m, uint32_t rde) {
  OpOut(m, Read16(m->dx), ReadEaxAx(m, rde));
}

static void AluEb(struct Machine *m, uint32_t rde, aluop_f op) {
  uint8_t *p;
  p = GetModrmRegisterBytePointerWrite(m, rde);
  Write8(p, op(Read8(p), 0, &m->flags));
}

static void AluEvqp(struct Machine *m, uint32_t rde, aluop_f ops[4]) {
  uint8_t *p;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  WriteRegisterOrMemory(rde, p,
                        ops[RegLog2(rde)](ReadMemory(rde, p), 0, &m->flags));
}

static void OpXchgZvqp(struct Machine *m, uint32_t rde) {
  uint64_t x, y;
  x = Read64(m->ax);
  y = Read64(RegRexbSrm(m, rde));
  WriteRegister(rde, m->ax, y);
  WriteRegister(rde, RegRexbSrm(m, rde), x);
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
  Sub8(x, y, &m->flags);
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
  kAlu[ALU_SUB][RegLog2(rde)](x, y, &m->flags);
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

static void OpRdrand(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexbRm(m, rde), rand64());
}

static void OpRdseed(struct Machine *m, uint32_t rde) {
  OpRdrand(m, rde);
}

static void Op1c7(struct Machine *m, uint32_t rde) {
  bool ismem;
  ismem = !IsModrmRegister(rde);
  switch (ModrmReg(rde)) {
    case 1:
      if (ismem) {
        if (Rexw(rde)) {
          OpCmpxchg16b(m, rde);
        } else {
          OpCmpxchg8b(m, rde);
        }
      } else {
        OpUd(m, rde);
      }
      break;
    case 6:
      if (!ismem) {
        OpRdrand(m, rde);
      } else {
        OpUd(m, rde);
      }
      break;
    case 7:
      if (!ismem) {
        if (Rep(rde) == 3) {
          OpRdpid(m, rde);
        } else {
          OpRdseed(m, rde);
        }
      } else {
        OpUd(m, rde);
      }
      break;
    default:
      OpUd(m, rde);
  }
}

static void OpXaddEbGb(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint8_t x, y, z;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  x = Read8(p);
  y = Read8(RegRexrReg(m, rde));
  z = Add8(x, y, &m->flags);
  Write8(p, z);
  Write8(RegRexrReg(m, rde), x);
}

static void OpXaddEvqpGvqp(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint64_t x, y, z;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  x = ReadMemory(rde, p);
  y = ReadMemory(rde, RegRexrReg(m, rde));
  z = kAlu[ALU_ADD][RegLog2(rde)](x, y, &m->flags);
  WriteRegisterOrMemory(rde, p, z);
  WriteRegister(rde, RegRexrReg(m, rde), x);
}

static uint64_t Bts(uint64_t x, uint64_t y) {
  return x | y;
}

static uint64_t Btr(uint64_t x, uint64_t y) {
  return x & ~y;
}

static uint64_t Btc(uint64_t x, uint64_t y) {
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
    v = MaskAddress(Eamode(rde), ComputeAddress(m, rde) + disp);
    p = ReserveAddress(m, v, 1 << w);
    if (op == 4) {
      SetReadAddr(m, v, 1 << w);
    } else {
      SetWriteAddr(m, v, 1 << w);
    }
  }
  y = 1;
  y <<= bit;
  x = ReadMemory(rde, p);
  m->flags = SetFlag(m->flags, FLAGS_CF, !!(y & x));
  switch (op) {
    case 4:
      return;
    case 5:
      z = Bts(x, y);
      break;
    case 6:
      z = Btr(x, y);
      break;
    case 7:
      z = Btc(x, y);
      break;
    default:
      OpUd(m, rde);
  }
  WriteRegisterOrMemory(rde, p, z);
}

static void OpSax(struct Machine *m, uint32_t rde) {
  if (Rexw(rde)) {
    Write64(m->ax, (int32_t)Read32(m->ax));
  } else if (!Osz(rde)) {
    Write64(m->ax, (uint32_t)(int16_t)Read16(m->ax));
  } else {
    Write16(m->ax, (int8_t)Read8(m->ax));
  }
}

static void OpConvert(struct Machine *m, uint32_t rde) {
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
    Write16(RegRexbSrm(m, rde), (x & 0x00ff) << 010 | (x & 0xff00) << 010);
  }
}

static void OpMovEbIb(struct Machine *m, uint32_t rde) {
  Write8(GetModrmRegisterBytePointerWrite(m, rde), m->xedd->op.uimm0);
}

static void OpMovAlOb(struct Machine *m, uint32_t rde) {
  int64_t addr;
  addr = AddressOb(m, rde);
  SetWriteAddr(m, addr, 1);
  Write8(m->ax, Read8(ResolveAddress(m, addr)));
}

static void OpMovObAl(struct Machine *m, uint32_t rde) {
  int64_t addr;
  addr = AddressOb(m, rde);
  SetReadAddr(m, addr, 1);
  Write8(ResolveAddress(m, addr), Read8(m->ax));
}

static void OpMovRaxOvqp(struct Machine *m, uint32_t rde) {
  uint64_t v;
  v = DataSegment(m, rde, m->xedd->op.disp);
  SetReadAddr(m, v, 1 << RegLog2(rde));
  WriteRegister(rde, m->ax, ReadMemory(rde, ResolveAddress(m, v)));
}

static void OpMovOvqpRax(struct Machine *m, uint32_t rde) {
  uint64_t v;
  v = DataSegment(m, rde, m->xedd->op.disp);
  SetWriteAddr(m, v, 1 << RegLog2(rde));
  WriteMemory(rde, ResolveAddress(m, v), Read64(m->ax));
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

static relegated void OpIncZv(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    Write32(RegSrm(m, rde), Inc32(Read32(RegSrm(m, rde)), 0, &m->flags));
  } else {
    Write16(RegSrm(m, rde), Inc16(Read16(RegSrm(m, rde)), 0, &m->flags));
  }
}

static relegated void OpDecZv(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    Write32(RegSrm(m, rde), Dec32(Read32(RegSrm(m, rde)), 0, &m->flags));
  } else {
    Write16(RegSrm(m, rde), Dec16(Read16(RegSrm(m, rde)), 0, &m->flags));
  }
}

static void OpMovEvqpIvds(struct Machine *m, uint32_t rde) {
  WriteRegisterOrMemory(rde, GetModrmRegisterWordPointerWriteOszRexw(m, rde),
                        m->xedd->op.uimm0);
}

static void OpMovEvqpGvqp(struct Machine *m, uint32_t rde) {
  WriteRegisterOrMemory(rde, GetModrmRegisterWordPointerWriteOszRexw(m, rde),
                        ReadMemory(rde, RegRexrReg(m, rde)));
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
  Write64(RegRexrReg(m, rde),
          (int32_t)Read32(GetModrmRegisterWordPointerRead4(m, rde)));
}

static void Alub(struct Machine *m, uint32_t rde, aluop_f op) {
  uint8_t *a = GetModrmRegisterBytePointerWrite(m, rde);
  Write8(a, op(Read8(a), Read8(ByteRexrReg(m, rde)), &m->flags));
}

static void OpAlubAdd(struct Machine *m, uint32_t rde) {
  Alub(m, rde, Add8);
}

static void OpAlubOr(struct Machine *m, uint32_t rde) {
  Alub(m, rde, Or8);
}

static void OpAlubAdc(struct Machine *m, uint32_t rde) {
  Alub(m, rde, Adc8);
}

static void OpAlubSbb(struct Machine *m, uint32_t rde) {
  Alub(m, rde, Sbb8);
}

static void OpAlubAnd(struct Machine *m, uint32_t rde) {
  Alub(m, rde, And8);
}

static void OpAlubSub(struct Machine *m, uint32_t rde) {
  Alub(m, rde, Sub8);
}

static void OpAlubXor(struct Machine *m, uint32_t rde) {
  Alub(m, rde, Xor8);
}

static void AlubRo(struct Machine *m, uint32_t rde, aluop_f op) {
  op(Read8(GetModrmRegisterBytePointerRead(m, rde)), Read8(ByteRexrReg(m, rde)),
     &m->flags);
}

static void OpAlubCmp(struct Machine *m, uint32_t rde) {
  AlubRo(m, rde, Sub8);
}

static void OpAlubTest(struct Machine *m, uint32_t rde) {
  AlubRo(m, rde, And8);
}

static void AlubFlip(struct Machine *m, uint32_t rde, aluop_f op) {
  Write8(ByteRexrReg(m, rde),
         op(Read8(ByteRexrReg(m, rde)),
            Read8(GetModrmRegisterBytePointerRead(m, rde)), &m->flags));
}

static void OpAlubFlipAdd(struct Machine *m, uint32_t rde) {
  AlubFlip(m, rde, Add8);
}

static void OpAlubFlipOr(struct Machine *m, uint32_t rde) {
  AlubFlip(m, rde, Or8);
}

static void OpAlubFlipAdc(struct Machine *m, uint32_t rde) {
  AlubFlip(m, rde, Adc8);
}

static void OpAlubFlipSbb(struct Machine *m, uint32_t rde) {
  AlubFlip(m, rde, Sbb8);
}

static void OpAlubFlipAnd(struct Machine *m, uint32_t rde) {
  AlubFlip(m, rde, And8);
}

static void OpAlubFlipSub(struct Machine *m, uint32_t rde) {
  AlubFlip(m, rde, Sub8);
}

static void OpAlubFlipXor(struct Machine *m, uint32_t rde) {
  AlubFlip(m, rde, Xor8);
}

static void AlubFlipRo(struct Machine *m, uint32_t rde, aluop_f op) {
  op(Read8(ByteRexrReg(m, rde)), Read8(GetModrmRegisterBytePointerRead(m, rde)),
     &m->flags);
}

static void OpAlubFlipCmp(struct Machine *m, uint32_t rde) {
  AlubFlipRo(m, rde, Sub8);
}

static void OpAlubFlipTest(struct Machine *m, uint32_t rde) {
  AlubFlipRo(m, rde, And8);
}

static void Alubi(struct Machine *m, uint32_t rde, aluop_f op) {
  uint8_t *a = GetModrmRegisterBytePointerWrite(m, rde);
  Write8(a, op(Read8(a), m->xedd->op.uimm0, &m->flags));
}

static void AlubiRo(struct Machine *m, uint32_t rde, aluop_f op) {
  op(Read8(GetModrmRegisterBytePointerRead(m, rde)), m->xedd->op.uimm0,
     &m->flags);
}

static void OpAlubiTest(struct Machine *m, uint32_t rde) {
  AlubiRo(m, rde, And8);
}

static void OpAlubiReg(struct Machine *m, uint32_t rde) {
  if (ModrmReg(rde) == ALU_CMP) {
    AlubiRo(m, rde, kAlu[ModrmReg(rde)][0]);
  } else {
    Alubi(m, rde, kAlu[ModrmReg(rde)][0]);
  }
}

static void OpAluw(struct Machine *m, uint32_t rde) {
  uint8_t *a;
  a = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  WriteRegisterOrMemory(
      rde, a,
      kAlu[(m->xedd->op.opcode & 070) >> 3][RegLog2(rde)](
          ReadMemory(rde, a), Read64(RegRexrReg(m, rde)), &m->flags));
}

static void AluwRo(struct Machine *m, uint32_t rde, aluop_f ops[4]) {
  ops[RegLog2(rde)](
      ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)),
      Read64(RegRexrReg(m, rde)), &m->flags);
}

static void OpAluwCmp(struct Machine *m, uint32_t rde) {
  AluwRo(m, rde, kAlu[ALU_SUB]);
}

static void OpAluwTest(struct Machine *m, uint32_t rde) {
  AluwRo(m, rde, kAlu[ALU_AND]);
}

static void OpAluwFlip(struct Machine *m, uint32_t rde) {
  WriteRegister(
      rde, RegRexrReg(m, rde),
      kAlu[(m->xedd->op.opcode & 070) >> 3][RegLog2(rde)](
          Read64(RegRexrReg(m, rde)),
          ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)),
          &m->flags));
}

static void AluwFlipRo(struct Machine *m, uint32_t rde, aluop_f ops[4]) {
  ops[RegLog2(rde)](
      Read64(RegRexrReg(m, rde)),
      ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)),
      &m->flags);
}

static void OpAluwFlipCmp(struct Machine *m, uint32_t rde) {
  AluwFlipRo(m, rde, kAlu[ALU_SUB]);
}

static void OpAluwFlipTest(struct Machine *m, uint32_t rde) {
  AluwFlipRo(m, rde, kAlu[ALU_AND]);
}

static void Aluwi(struct Machine *m, uint32_t rde, aluop_f ops[4]) {
  uint8_t *a;
  a = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  WriteRegisterOrMemory(
      rde, a,
      ops[RegLog2(rde)](ReadMemory(rde, a), m->xedd->op.uimm0, &m->flags));
}

static void AluwiRo(struct Machine *m, uint32_t rde, aluop_f ops[4]) {
  ops[RegLog2(rde)](
      ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)),
      m->xedd->op.uimm0, &m->flags);
}

static void OpAluwiReg(struct Machine *m, uint32_t rde) {
  if (ModrmReg(rde) == ALU_CMP) {
    AluwiRo(m, rde, kAlu[ModrmReg(rde)]);
  } else {
    Aluwi(m, rde, kAlu[ModrmReg(rde)]);
  }
}

static void AluAlIb(struct Machine *m, aluop_f op) {
  Write8(m->ax, op(Read8(m->ax), m->xedd->op.uimm0, &m->flags));
}

static void OpAluAlIbAdd(struct Machine *m, uint32_t rde) {
  AluAlIb(m, Add8);
}

static void OpAluAlIbOr(struct Machine *m, uint32_t rde) {
  AluAlIb(m, Or8);
}

static void OpAluAlIbAdc(struct Machine *m, uint32_t rde) {
  AluAlIb(m, Adc8);
}

static void OpAluAlIbSbb(struct Machine *m, uint32_t rde) {
  AluAlIb(m, Sbb8);
}

static void OpAluAlIbAnd(struct Machine *m, uint32_t rde) {
  AluAlIb(m, And8);
}

static void OpAluAlIbSub(struct Machine *m, uint32_t rde) {
  AluAlIb(m, Sub8);
}

static void OpAluAlIbXor(struct Machine *m, uint32_t rde) {
  AluAlIb(m, Xor8);
}

static void OpAluRaxIvds(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, m->ax,
                kAlu[(m->xedd->op.opcode & 070) >> 3][RegLog2(rde)](
                    ReadMemory(rde, m->ax), m->xedd->op.uimm0, &m->flags));
}

static void OpCmpAlIb(struct Machine *m, uint32_t rde) {
  Sub8(Read8(m->ax), m->xedd->op.uimm0, &m->flags);
}

static void OpCmpRaxIvds(struct Machine *m, uint32_t rde) {
  kAlu[ALU_SUB][RegLog2(rde)](ReadMemory(rde, m->ax), m->xedd->op.uimm0,
                              &m->flags);
}

static void OpTestAlIb(struct Machine *m, uint32_t rde) {
  And8(Read8(m->ax), m->xedd->op.uimm0, &m->flags);
}

static void OpTestRaxIvds(struct Machine *m, uint32_t rde) {
  kAlu[ALU_AND][RegLog2(rde)](ReadMemory(rde, m->ax), m->xedd->op.uimm0,
                              &m->flags);
}

static void Bsuwi(struct Machine *m, uint32_t rde, uint64_t y) {
  uint8_t *p;
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  WriteRegisterOrMemory(
      rde, p,
      kBsu[ModrmReg(rde)][RegLog2(rde)](ReadMemory(rde, p), y, &m->flags));
}

static void OpBsuwi1(struct Machine *m, uint32_t rde) {
  Bsuwi(m, rde, 1);
}

static void OpBsuwiCl(struct Machine *m, uint32_t rde) {
  Bsuwi(m, rde, Read8(m->cx));
}

static void OpBsuwiImm(struct Machine *m, uint32_t rde) {
  Bsuwi(m, rde, m->xedd->op.uimm0);
}

static void Bsubi(struct Machine *m, uint32_t rde, uint64_t y) {
  uint8_t *a = GetModrmRegisterBytePointerWrite(m, rde);
  Write8(a, kBsu[ModrmReg(rde)][RegLog2(rde)](Read8(a), y, &m->flags));
}

static void OpBsubi1(struct Machine *m, uint32_t rde) {
  Bsubi(m, rde, 1);
}

static void OpBsubiCl(struct Machine *m, uint32_t rde) {
  Bsubi(m, rde, Read8(m->cx));
}

static void OpBsubiImm(struct Machine *m, uint32_t rde) {
  Bsubi(m, rde, m->xedd->op.uimm0);
}

static void OpPushImm(struct Machine *m, uint32_t rde) {
  Push(m, rde, m->xedd->op.uimm0);
}

static void Interrupt(struct Machine *m, uint32_t rde, int i) {
  HaltMachine(m, i);
}

static void OpInterruptImm(struct Machine *m, uint32_t rde) {
  Interrupt(m, rde, m->xedd->op.uimm0);
}

static void OpInterrupt1(struct Machine *m, uint32_t rde) {
  Interrupt(m, rde, 1);
}

static void OpInterrupt3(struct Machine *m, uint32_t rde) {
  Interrupt(m, rde, 3);
}

static void OpJmp(struct Machine *m, uint32_t rde) {
  m->ip += m->xedd->op.disp;
}

static void OpJe(struct Machine *m, uint32_t rde) {
  if (GetFlag(m->flags, FLAGS_ZF)) {
    OpJmp(m, rde);
  }
}

static void OpJne(struct Machine *m, uint32_t rde) {
  if (!GetFlag(m->flags, FLAGS_ZF)) {
    OpJmp(m, rde);
  }
}

static void OpJb(struct Machine *m, uint32_t rde) {
  if (GetFlag(m->flags, FLAGS_CF)) {
    OpJmp(m, rde);
  }
}

static void OpJbe(struct Machine *m, uint32_t rde) {
  if (IsBelowOrEqual(m)) {
    OpJmp(m, rde);
  }
}

static void OpJo(struct Machine *m, uint32_t rde) {
  if (GetFlag(m->flags, FLAGS_OF)) {
    OpJmp(m, rde);
  }
}

static void OpJno(struct Machine *m, uint32_t rde) {
  if (!GetFlag(m->flags, FLAGS_OF)) {
    OpJmp(m, rde);
  }
}

static void OpJae(struct Machine *m, uint32_t rde) {
  if (!GetFlag(m->flags, FLAGS_CF)) {
    OpJmp(m, rde);
  }
}

static void OpJa(struct Machine *m, uint32_t rde) {
  if (IsAbove(m)) {
    OpJmp(m, rde);
  }
}

static void OpJs(struct Machine *m, uint32_t rde) {
  if (GetFlag(m->flags, FLAGS_SF)) {
    OpJmp(m, rde);
  }
}

static void OpJns(struct Machine *m, uint32_t rde) {
  if (!GetFlag(m->flags, FLAGS_SF)) {
    OpJmp(m, rde);
  }
}

static void OpJp(struct Machine *m, uint32_t rde) {
  if (IsParity(m)) {
    OpJmp(m, rde);
  }
}

static void OpJnp(struct Machine *m, uint32_t rde) {
  if (!IsParity(m)) {
    OpJmp(m, rde);
  }
}

static void OpJl(struct Machine *m, uint32_t rde) {
  if (IsLess(m)) {
    OpJmp(m, rde);
  }
}

static void OpJge(struct Machine *m, uint32_t rde) {
  if (IsGreaterOrEqual(m)) {
    OpJmp(m, rde);
  }
}

static void OpJle(struct Machine *m, uint32_t rde) {
  if (IsLessOrEqual(m)) {
    OpJmp(m, rde);
  }
}

static void OpJg(struct Machine *m, uint32_t rde) {
  if (IsGreater(m)) {
    OpJmp(m, rde);
  }
}

static void OpMovGvqpEvqp(struct Machine *m, uint32_t rde) {
  WriteRegister(
      rde, RegRexrReg(m, rde),
      ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde)));
}

static void OpCmovo(struct Machine *m, uint32_t rde) {
  if (GetFlag(m->flags, FLAGS_OF)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovno(struct Machine *m, uint32_t rde) {
  if (!GetFlag(m->flags, FLAGS_OF)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovb(struct Machine *m, uint32_t rde) {
  if (GetFlag(m->flags, FLAGS_CF)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovae(struct Machine *m, uint32_t rde) {
  if (!GetFlag(m->flags, FLAGS_CF)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmove(struct Machine *m, uint32_t rde) {
  if (GetFlag(m->flags, FLAGS_ZF)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovne(struct Machine *m, uint32_t rde) {
  if (!GetFlag(m->flags, FLAGS_ZF)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovbe(struct Machine *m, uint32_t rde) {
  if (IsBelowOrEqual(m)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmova(struct Machine *m, uint32_t rde) {
  if (IsAbove(m)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovs(struct Machine *m, uint32_t rde) {
  if (GetFlag(m->flags, FLAGS_SF)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovns(struct Machine *m, uint32_t rde) {
  if (!GetFlag(m->flags, FLAGS_SF)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovp(struct Machine *m, uint32_t rde) {
  if (IsParity(m)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovnp(struct Machine *m, uint32_t rde) {
  if (!IsParity(m)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovl(struct Machine *m, uint32_t rde) {
  if (IsLess(m)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovge(struct Machine *m, uint32_t rde) {
  if (IsGreaterOrEqual(m)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovle(struct Machine *m, uint32_t rde) {
  if (IsLessOrEqual(m)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void OpCmovg(struct Machine *m, uint32_t rde) {
  if (IsGreater(m)) {
    OpMovGvqpEvqp(m, rde);
  }
}

static void SetEb(struct Machine *m, uint32_t rde, bool x) {
  Write8(GetModrmRegisterBytePointerWrite(m, rde), x);
}

static void OpSeto(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, GetFlag(m->flags, FLAGS_OF));
}

static void OpSetno(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, !GetFlag(m->flags, FLAGS_OF));
}

static void OpSetb(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, GetFlag(m->flags, FLAGS_CF));
}

static void OpSetae(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, !GetFlag(m->flags, FLAGS_CF));
}

static void OpSete(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, GetFlag(m->flags, FLAGS_ZF));
}

static void OpSetne(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, !GetFlag(m->flags, FLAGS_ZF));
}

static void OpSetbe(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, IsBelowOrEqual(m));
}

static void OpSeta(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, IsAbove(m));
}

static void OpSets(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, GetFlag(m->flags, FLAGS_SF));
}

static void OpSetns(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, !GetFlag(m->flags, FLAGS_SF));
}

static void OpSetp(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, IsParity(m));
}

static void OpSetnp(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, !IsParity(m));
}

static void OpSetl(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, IsLess(m));
}

static void OpSetge(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, IsGreaterOrEqual(m));
}

static void OpSetle(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, IsLessOrEqual(m));
}

static void OpSetg(struct Machine *m, uint32_t rde) {
  SetEb(m, rde, IsGreater(m));
}

static void OpJcxz(struct Machine *m, uint32_t rde) {
  if (!MaskAddress(Eamode(rde), Read64(m->cx))) {
    OpJmp(m, rde);
  }
}

static void Bitscan(struct Machine *m, uint32_t rde, bitscan_f op) {
  WriteRegister(
      rde, RegRexrReg(m, rde),
      op(m, rde,
         ReadMemory(rde, GetModrmRegisterWordPointerReadOszRexw(m, rde))));
}

static void OpBsf(struct Machine *m, uint32_t rde) {
  Bitscan(m, rde, AluBsf);
}

static void OpBsr(struct Machine *m, uint32_t rde) {
  Bitscan(m, rde, AluBsr);
}

static void Op1b8(struct Machine *m, uint32_t rde) {
  if (Rep(rde) == 3) {
    Bitscan(m, rde, AluPopcnt);
  } else {
    OpUd(m, rde);
  }
}

static void OpNotEb(struct Machine *m, uint32_t rde) {
  AluEb(m, rde, Not8);
}

static void OpNegEb(struct Machine *m, uint32_t rde) {
  AluEb(m, rde, Neg8);
}

static relegated void LoadFarPointer(struct Machine *m, uint32_t rde,
                                     uint8_t seg[8]) {
  uint32_t fp;
  fp = Read32(ComputeReserveAddressRead4(m, rde));
  Write64(seg, (fp & 0x0000ffff) << 4);
  Write16(RegRexrReg(m, rde), fp >> 16);
}

static relegated void OpLes(struct Machine *m, uint32_t rde) {
  LoadFarPointer(m, rde, m->es);
}

static relegated void OpLds(struct Machine *m, uint32_t rde) {
  LoadFarPointer(m, rde, m->ds);
}

static relegated void Loop(struct Machine *m, uint32_t rde, bool cond) {
  uint64_t cx;
  cx = Read64(m->cx) - 1;
  if (Eamode(rde) != XED_MODE_REAL) {
    if (Eamode(rde) == XED_MODE_LEGACY) {
      cx &= 0xffffffff;
    }
    Write64(m->cx, cx);
  } else {
    cx &= 0xffff;
    Write16(m->cx, cx);
  }
  if (cx && cond) {
    OpJmp(m, rde);
  }
}

static relegated void OpLoope(struct Machine *m, uint32_t rde) {
  Loop(m, rde, GetFlag(m->flags, FLAGS_ZF));
}

static relegated void OpLoopne(struct Machine *m, uint32_t rde) {
  Loop(m, rde, !GetFlag(m->flags, FLAGS_ZF));
}

static relegated void OpLoop1(struct Machine *m, uint32_t rde) {
  Loop(m, rde, true);
}

static const nexgen32e_f kOp0f6[] = {
    OpAlubiTest,
    OpAlubiTest,
    OpNotEb,
    OpNegEb,
    OpMulAxAlEbUnsigned,
    OpMulAxAlEbSigned,
    OpDivAlAhAxEbUnsigned,
    OpDivAlAhAxEbSigned,
};

static void Op0f6(struct Machine *m, uint32_t rde) {
  kOp0f6[ModrmReg(rde)](m, rde);
}

static void OpTestEvqpIvds(struct Machine *m, uint32_t rde) {
  AluwiRo(m, rde, kAlu[ALU_AND]);
}

static void OpNotEvqp(struct Machine *m, uint32_t rde) {
  AluEvqp(m, rde, kAlu[ALU_NOT]);
}

static void OpNegEvqp(struct Machine *m, uint32_t rde) {
  AluEvqp(m, rde, kAlu[ALU_NEG]);
}

static const nexgen32e_f kOp0f7[] = {
    OpTestEvqpIvds,
    OpTestEvqpIvds,
    OpNotEvqp,
    OpNegEvqp,
    OpMulRdxRaxEvqpUnsigned,
    OpMulRdxRaxEvqpSigned,
    OpDivRdxRaxEvqpUnsigned,
    OpDivRdxRaxEvqpSigned,
};

static void Op0f7(struct Machine *m, uint32_t rde) {
  kOp0f7[ModrmReg(rde)](m, rde);
}

static void Op0fe(struct Machine *m, uint32_t rde) {
  switch (ModrmReg(rde)) {
    case 0:
      AluEb(m, rde, Inc8);
      break;
    case 1:
      AluEb(m, rde, Dec8);
      break;
    default:
      OpUd(m, rde);
  }
}

static void OpIncEvqp(struct Machine *m, uint32_t rde) {
  AluEvqp(m, rde, kAlu[ALU_INC]);
}

static void OpDecEvqp(struct Machine *m, uint32_t rde) {
  AluEvqp(m, rde, kAlu[ALU_DEC]);
}

static const nexgen32e_f kOp0ff[] = {OpIncEvqp, OpDecEvqp, OpCallEq,  OpUd,
                                     OpJmpEq,   OpUd,      OpPushEvq, OpUd};

static void Op0ff(struct Machine *m, uint32_t rde) {
  kOp0ff[ModrmReg(rde)](m, rde);
}

static void OpDoubleShift(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  uint64_t x;
  uint8_t W[2][2] = {{2, 3}, {1, 3}};
  p = GetModrmRegisterWordPointerWriteOszRexw(m, rde);
  WriteRegisterOrMemory(
      rde, p,
      BsuDoubleShift(W[Osz(rde)][Rexw(rde)], ReadMemory(rde, p),
                     ReadMemory(rde, RegRexrReg(m, rde)),
                     m->xedd->op.opcode & 1 ? Read8(m->cx) : m->xedd->op.uimm0,
                     m->xedd->op.opcode & 8, &m->flags));
}

static void OpFxsave(struct Machine *m, uint32_t rde) {
  int64_t v;
  uint8_t buf[32];
  memset(buf, 0, 32);
  Write16(buf + 0, m->fpu.cw);
  Write16(buf + 2, m->fpu.sw);
  Write8(buf + 4, m->fpu.tw);
  Write16(buf + 6, m->fpu.op);
  Write32(buf + 8, m->fpu.ip);
  Write32(buf + 24, m->sse.mxcsr);
  v = ComputeAddress(m, rde);
  VirtualRecv(m, v + 0, buf, 32);
  VirtualRecv(m, v + 32, m->fpu.st, 128);
  VirtualRecv(m, v + 160, m->xmm, 256);
  SetWriteAddr(m, v, 416);
}

static void OpFxrstor(struct Machine *m, uint32_t rde) {
  int64_t v;
  uint8_t buf[32];
  v = ComputeAddress(m, rde);
  SetReadAddr(m, v, 416);
  VirtualSend(m, buf, v + 0, 32);
  VirtualSend(m, m->fpu.st, v + 32, 128);
  VirtualSend(m, m->xmm, v + 160, 256);
  m->fpu.cw = Read16(buf + 0);
  m->fpu.sw = Read16(buf + 2);
  m->fpu.tw = Read8(buf + 4);
  m->fpu.op = Read16(buf + 6);
  m->fpu.ip = Read32(buf + 8);
  m->sse.mxcsr = Read32(buf + 24);
}

static void OpXsave(struct Machine *m, uint32_t rde) {
}

static void OpLdmxcsr(struct Machine *m, uint32_t rde) {
  m->sse.mxcsr = Read32(ComputeReserveAddressRead4(m, rde));
}

static void OpStmxcsr(struct Machine *m, uint32_t rde) {
  Write32(ComputeReserveAddressWrite4(m, rde), m->sse.mxcsr);
}

static void OpRdfsbase(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexbRm(m, rde), Read64(m->fs));
}

static void OpRdgsbase(struct Machine *m, uint32_t rde) {
  WriteRegister(rde, RegRexbRm(m, rde), Read64(m->gs));
}

static void OpWrfsbase(struct Machine *m, uint32_t rde) {
  Write64(m->fs, ReadMemory(rde, RegRexbRm(m, rde)));
}

static void OpWrgsbase(struct Machine *m, uint32_t rde) {
  Write64(m->gs, ReadMemory(rde, RegRexbRm(m, rde)));
}

static void Op1ae(struct Machine *m, uint32_t rde) {
  bool ismem;
  ismem = !IsModrmRegister(rde);
  switch (ModrmReg(rde)) {
    case 0:
      if (ismem) {
        OpFxsave(m, rde);
      } else {
        OpRdfsbase(m, rde);
      }
      break;
    case 1:
      if (ismem) {
        OpFxrstor(m, rde);
      } else {
        OpRdgsbase(m, rde);
      }
      break;
    case 2:
      if (ismem) {
        OpLdmxcsr(m, rde);
      } else {
        OpWrfsbase(m, rde);
      }
      break;
    case 3:
      if (ismem) {
        OpStmxcsr(m, rde);
      } else {
        OpWrgsbase(m, rde);
      }
      break;
    case 4:
      if (ismem) {
        OpXsave(m, rde);
      } else {
        OpUd(m, rde);
      }
      break;
    case 5:
      OpLfence(m, rde);
      break;
    case 6:
      OpMfence(m, rde);
      break;
    case 7:
      if (ismem) {
        OpClflush(m, rde);
      } else {
        OpSfence(m, rde);
      }
      break;
    default:
      OpUd(m, rde);
  }
}

static void OpSalc(struct Machine *m, uint32_t rde) {
  Write8(m->ax, GetFlag(m->flags, FLAGS_CF));
}

static void OpBofram(struct Machine *m, uint32_t rde) {
  if (m->xedd->op.disp) {
    m->bofram[0] = m->ip;
    m->bofram[1] = m->ip + (m->xedd->op.disp & 0xff);
  } else {
    m->bofram[0] = 0;
    m->bofram[1] = 0;
  }
}

static void OpBinbase(struct Machine *m, uint32_t rde) {
  if (m->onbinbase) {
    m->onbinbase(m);
  }
}

static void OpNopEv(struct Machine *m, uint32_t rde) {
  switch (ModrmMod(rde) << 6 | ModrmReg(rde) << 3 | ModrmRm(rde)) {
    case 0105:
      OpBofram(m, rde);
      break;
    case 0007:
    case 0107:
    case 0207:
      OpBinbase(m, rde);
      break;
    default:
      OpNoop(m, rde);
  }
}

static void OpNop(struct Machine *m, uint32_t rde) {
  if (Rexb(rde)) {
    OpXchgZvqp(m, rde);
  } else if (Rep(rde) == 3) {
    OpPause(m, rde);
  } else {
    OpNoop(m, rde);
  }
}

static void OpMovRqCq(struct Machine *m, uint32_t rde) {
  switch (ModrmReg(rde)) {
    case 0:
      Write64(RegRexbRm(m, rde), m->cr0);
      break;
    case 2:
      Write64(RegRexbRm(m, rde), m->cr2);
      break;
    case 3:
      Write64(RegRexbRm(m, rde), m->cr3);
      break;
    case 4:
      Write64(RegRexbRm(m, rde), m->cr4);
      break;
    default:
      OpUd(m, rde);
  }
}

static void OpMovCqRq(struct Machine *m, uint32_t rde) {
  int64_t cr3;
  switch (ModrmReg(rde)) {
    case 0:
      m->cr0 = Read64(RegRexbRm(m, rde));
      break;
    case 2:
      m->cr2 = Read64(RegRexbRm(m, rde));
      break;
    case 3:
      cr3 = Read64(RegRexbRm(m, rde));
      if (0 <= cr3 && cr3 + 512 * 8 <= m->real.n) {
        m->cr3 = cr3;
      } else {
        ThrowProtectionFault(m);
      }
      break;
    case 4:
      m->cr4 = Read64(RegRexbRm(m, rde));
      break;
    default:
      OpUd(m, rde);
  }
}

static void OpWrmsr(struct Machine *m, uint32_t rde) {
}

static void OpRdmsr(struct Machine *m, uint32_t rde) {
  Write32(m->dx, 0);
  Write32(m->ax, 0);
}

static const nexgen32e_f kNexgen32e[] = {
    [0x000] = OpAlubAdd,
    [0x001] = OpAluw,
    [0x002] = OpAlubFlipAdd,
    [0x003] = OpAluwFlip,
    [0x004] = OpAluAlIbAdd,
    [0x005] = OpAluRaxIvds,
    [0x006] = OpPushSeg,
    [0x007] = OpPopSeg,
    [0x008] = OpAlubOr,
    [0x009] = OpAluw,
    [0x00A] = OpAlubFlipOr,
    [0x00B] = OpAluwFlip,
    [0x00C] = OpAluAlIbOr,
    [0x00D] = OpAluRaxIvds,
    [0x00E] = OpPushSeg,
    [0x00F] = OpPopSeg,
    [0x010] = OpAlubAdc,
    [0x011] = OpAluw,
    [0x012] = OpAlubFlipAdc,
    [0x013] = OpAluwFlip,
    [0x014] = OpAluAlIbAdc,
    [0x015] = OpAluRaxIvds,
    [0x016] = OpPushSeg,
    [0x017] = OpPopSeg,
    [0x018] = OpAlubSbb,
    [0x019] = OpAluw,
    [0x01A] = OpAlubFlipSbb,
    [0x01B] = OpAluwFlip,
    [0x01C] = OpAluAlIbSbb,
    [0x01D] = OpAluRaxIvds,
    [0x01E] = OpPushSeg,
    [0x01F] = OpPopSeg,
    [0x020] = OpAlubAnd,
    [0x021] = OpAluw,
    [0x022] = OpAlubFlipAnd,
    [0x023] = OpAluwFlip,
    [0x024] = OpAluAlIbAnd,
    [0x025] = OpAluRaxIvds,
    [0x026] = OpPushSeg,
    [0x027] = OpPopSeg,
    [0x028] = OpAlubSub,
    [0x029] = OpAluw,
    [0x02A] = OpAlubFlipSub,
    [0x02B] = OpAluwFlip,
    [0x02C] = OpAluAlIbSub,
    [0x02D] = OpAluRaxIvds,
    [0x02E] = OpUd,
    [0x02F] = OpDas,
    [0x030] = OpAlubXor,
    [0x031] = OpAluw,
    [0x032] = OpAlubFlipXor,
    [0x033] = OpAluwFlip,
    [0x034] = OpAluAlIbXor,
    [0x035] = OpAluRaxIvds,
    [0x036] = OpUd,
    [0x037] = OpAaa,
    [0x038] = OpAlubCmp,
    [0x039] = OpAluwCmp,
    [0x03A] = OpAlubFlipCmp,
    [0x03B] = OpAluwFlipCmp,
    [0x03C] = OpCmpAlIb,
    [0x03D] = OpCmpRaxIvds,
    [0x03E] = OpUd,
    [0x03F] = OpAas,
    [0x040] = OpIncZv,
    [0x041] = OpIncZv,
    [0x042] = OpIncZv,
    [0x043] = OpIncZv,
    [0x044] = OpIncZv,
    [0x045] = OpIncZv,
    [0x046] = OpIncZv,
    [0x047] = OpIncZv,
    [0x048] = OpDecZv,
    [0x049] = OpDecZv,
    [0x04A] = OpDecZv,
    [0x04B] = OpDecZv,
    [0x04C] = OpDecZv,
    [0x04D] = OpDecZv,
    [0x04E] = OpDecZv,
    [0x04F] = OpDecZv,
    [0x050] = OpPushZvq,
    [0x051] = OpPushZvq,
    [0x052] = OpPushZvq,
    [0x053] = OpPushZvq,
    [0x054] = OpPushZvq,
    [0x055] = OpPushZvq,
    [0x056] = OpPushZvq,
    [0x057] = OpPushZvq,
    [0x058] = OpPopZvq,
    [0x059] = OpPopZvq,
    [0x05A] = OpPopZvq,
    [0x05B] = OpPopZvq,
    [0x05C] = OpPopZvq,
    [0x05D] = OpPopZvq,
    [0x05E] = OpPopZvq,
    [0x05F] = OpPopZvq,
    [0x060] = OpPusha,
    [0x061] = OpPopa,
    [0x062] = OpUd,
    [0x063] = OpMovsxdGdqpEd,
    [0x064] = OpUd,
    [0x065] = OpUd,
    [0x066] = OpUd,
    [0x067] = OpUd,
    [0x068] = OpPushImm,
    [0x069] = OpImulGvqpEvqpImm,
    [0x06A] = OpPushImm,
    [0x06B] = OpImulGvqpEvqpImm,
    [0x06C] = OpIns,
    [0x06D] = OpIns,
    [0x06E] = OpOuts,
    [0x06F] = OpOuts,
    [0x070] = OpJo,
    [0x071] = OpJno,
    [0x072] = OpJb,
    [0x073] = OpJae,
    [0x074] = OpJe,
    [0x075] = OpJne,
    [0x076] = OpJbe,
    [0x077] = OpJa,
    [0x078] = OpJs,
    [0x079] = OpJns,
    [0x07A] = OpJp,
    [0x07B] = OpJnp,
    [0x07C] = OpJl,
    [0x07D] = OpJge,
    [0x07E] = OpJle,
    [0x07F] = OpJg,
    [0x080] = OpAlubiReg,
    [0x081] = OpAluwiReg,
    [0x082] = OpAlubiReg,
    [0x083] = OpAluwiReg,
    [0x084] = OpAlubTest,
    [0x085] = OpAluwTest,
    [0x086] = OpXchgGbEb,
    [0x087] = OpXchgGvqpEvqp,
    [0x088] = OpMovEbGb,
    [0x089] = OpMovEvqpGvqp,
    [0x08A] = OpMovGbEb,
    [0x08B] = OpMovGvqpEvqp,
    [0x08C] = OpMovEvqpSw,
    [0x08D] = OpLeaGvqpM,
    [0x08E] = OpMovSwEvqp,
    [0x08F] = OpPopEvq,
    [0x090] = OpNop,
    [0x091] = OpXchgZvqp,
    [0x092] = OpXchgZvqp,
    [0x093] = OpXchgZvqp,
    [0x094] = OpXchgZvqp,
    [0x095] = OpXchgZvqp,
    [0x096] = OpXchgZvqp,
    [0x097] = OpXchgZvqp,
    [0x098] = OpSax,
    [0x099] = OpConvert,
    [0x09A] = OpCallf,
    [0x09B] = OpFwait,
    [0x09C] = OpPushf,
    [0x09D] = OpPopf,
    [0x09E] = OpSahf,
    [0x09F] = OpLahf,
    [0x0A0] = OpMovAlOb,
    [0x0A1] = OpMovRaxOvqp,
    [0x0A2] = OpMovObAl,
    [0x0A3] = OpMovOvqpRax,
    [0x0A4] = OpMovsb,
    [0x0A5] = OpMovs,
    [0x0A6] = OpCmps,
    [0x0A7] = OpCmps,
    [0x0A8] = OpTestAlIb,
    [0x0A9] = OpTestRaxIvds,
    [0x0AA] = OpStosb,
    [0x0AB] = OpStos,
    [0x0AC] = OpLods,
    [0x0AD] = OpLods,
    [0x0AE] = OpScas,
    [0x0AF] = OpScas,
    [0x0B0] = OpMovZbIb,
    [0x0B1] = OpMovZbIb,
    [0x0B2] = OpMovZbIb,
    [0x0B3] = OpMovZbIb,
    [0x0B4] = OpMovZbIb,
    [0x0B5] = OpMovZbIb,
    [0x0B6] = OpMovZbIb,
    [0x0B7] = OpMovZbIb,
    [0x0B8] = OpMovZvqpIvqp,
    [0x0B9] = OpMovZvqpIvqp,
    [0x0BA] = OpMovZvqpIvqp,
    [0x0BB] = OpMovZvqpIvqp,
    [0x0BC] = OpMovZvqpIvqp,
    [0x0BD] = OpMovZvqpIvqp,
    [0x0BE] = OpMovZvqpIvqp,
    [0x0BF] = OpMovZvqpIvqp,
    [0x0C0] = OpBsubiImm,
    [0x0C1] = OpBsuwiImm,
    [0x0C2] = OpRet,
    [0x0C3] = OpRet,
    [0x0C4] = OpLes,
    [0x0C5] = OpLds,
    [0x0C6] = OpMovEbIb,
    [0x0C7] = OpMovEvqpIvds,
    [0x0C8] = OpUd,
    [0x0C9] = OpLeave,
    [0x0CA] = OpRetf,
    [0x0CB] = OpRetf,
    [0x0CC] = OpInterrupt3,
    [0x0CD] = OpInterruptImm,
    [0x0CE] = OpUd,
    [0x0CF] = OpUd,
    [0x0D0] = OpBsubi1,
    [0x0D1] = OpBsuwi1,
    [0x0D2] = OpBsubiCl,
    [0x0D3] = OpBsuwiCl,
    [0x0D4] = OpAam,
    [0x0D5] = OpAad,
    [0x0D6] = OpSalc,
    [0x0D7] = OpXlatAlBbb,
    [0x0D8] = OpFpu,
    [0x0D9] = OpFpu,
    [0x0DA] = OpFpu,
    [0x0DB] = OpFpu,
    [0x0DC] = OpFpu,
    [0x0DD] = OpFpu,
    [0x0DE] = OpFpu,
    [0x0DF] = OpFpu,
    [0x0E0] = OpLoopne,
    [0x0E1] = OpLoope,
    [0x0E2] = OpLoop1,
    [0x0E3] = OpJcxz,
    [0x0E4] = OpInAlImm,
    [0x0E5] = OpInAxImm,
    [0x0E6] = OpOutImmAl,
    [0x0E7] = OpOutImmAx,
    [0x0E8] = OpCallJvds,
    [0x0E9] = OpJmp,
    [0x0EA] = OpJmpf,
    [0x0EB] = OpJmp,
    [0x0EC] = OpInAlDx,
    [0x0ED] = OpInAxDx,
    [0x0EE] = OpOutDxAl,
    [0x0EF] = OpOutDxAx,
    [0x0F0] = OpUd,
    [0x0F1] = OpInterrupt1,
    [0x0F2] = OpUd,
    [0x0F3] = OpUd,
    [0x0F4] = OpHlt,
    [0x0F5] = OpCmc,
    [0x0F6] = Op0f6,
    [0x0F7] = Op0f7,
    [0x0F8] = OpClc,
    [0x0F9] = OpStc,
    [0x0FA] = OpCli,
    [0x0FB] = OpSti,
    [0x0FC] = OpCld,
    [0x0FD] = OpStd,
    [0x0FE] = Op0fe,
    [0x0FF] = Op0ff,
    [0x100] = OpUd,
    [0x101] = Op101,
    [0x102] = OpUd,
    [0x103] = OpLsl,
    [0x104] = OpUd,
    [0x105] = OpSyscall,
    [0x106] = OpUd,
    [0x107] = OpUd,
    [0x108] = OpUd,
    [0x109] = OpUd,
    [0x10A] = OpUd,
    [0x10B] = OpUd,
    [0x10C] = OpUd,
    [0x10D] = OpHintNopEv,
    [0x10E] = OpUd,
    [0x10F] = OpUd,
    [0x110] = OpMov0f10,
    [0x111] = OpMovWpsVps,
    [0x112] = OpMov0f12,
    [0x113] = OpMov0f13,
    [0x114] = OpUnpcklpsd,
    [0x115] = OpUnpckhpsd,
    [0x116] = OpMov0f16,
    [0x117] = OpMov0f17,
    [0x118] = OpHintNopEv,
    [0x119] = OpHintNopEv,
    [0x11A] = OpHintNopEv,
    [0x11B] = OpHintNopEv,
    [0x11C] = OpHintNopEv,
    [0x11D] = OpHintNopEv,
    [0x11E] = OpHintNopEv,
    [0x11F] = OpNopEv,
    [0x120] = OpMovRqCq,
    [0x121] = OpUd,
    [0x122] = OpMovCqRq,
    [0x123] = OpUd,
    [0x124] = OpUd,
    [0x125] = OpUd,
    [0x126] = OpUd,
    [0x127] = OpUd,
    [0x128] = OpMov0f28,
    [0x129] = OpMovWpsVps,
    [0x12A] = OpCvt0f2a,
    [0x12B] = OpMov0f2b,
    [0x12C] = OpCvtt0f2c,
    [0x12D] = OpCvt0f2d,
    [0x12E] = OpComissVsWs,
    [0x12F] = OpComissVsWs,
    [0x130] = OpWrmsr,
    [0x131] = OpRdtsc,
    [0x132] = OpRdmsr,
    [0x133] = OpUd,
    [0x134] = OpUd,
    [0x135] = OpUd,
    [0x136] = OpUd,
    [0x137] = OpUd,
    [0x138] = OpUd,
    [0x139] = OpUd,
    [0x13A] = OpUd,
    [0x13B] = OpUd,
    [0x13C] = OpUd,
    [0x13D] = OpUd,
    [0x13E] = OpUd,
    [0x13F] = OpUd,
    [0x140] = OpCmovo,
    [0x141] = OpCmovno,
    [0x142] = OpCmovb,
    [0x143] = OpCmovae,
    [0x144] = OpCmove,
    [0x145] = OpCmovne,
    [0x146] = OpCmovbe,
    [0x147] = OpCmova,
    [0x148] = OpCmovs,
    [0x149] = OpCmovns,
    [0x14A] = OpCmovp,
    [0x14B] = OpCmovnp,
    [0x14C] = OpCmovl,
    [0x14D] = OpCmovge,
    [0x14E] = OpCmovle,
    [0x14F] = OpCmovg,
    [0x150] = OpUd,
    [0x151] = OpSqrtpsd,
    [0x152] = OpRsqrtps,
    [0x153] = OpRcpps,
    [0x154] = OpAndpsd,
    [0x155] = OpAndnpsd,
    [0x156] = OpOrpsd,
    [0x157] = OpXorpsd,
    [0x158] = OpAddpsd,
    [0x159] = OpMulpsd,
    [0x15A] = OpCvt0f5a,
    [0x15B] = OpCvt0f5b,
    [0x15C] = OpSubpsd,
    [0x15D] = OpMinpsd,
    [0x15E] = OpDivpsd,
    [0x15F] = OpMaxpsd,
    [0x160] = OpSsePunpcklbw,
    [0x161] = OpSsePunpcklwd,
    [0x162] = OpSsePunpckldq,
    [0x163] = OpSsePacksswb,
    [0x164] = OpSsePcmpgtb,
    [0x165] = OpSsePcmpgtw,
    [0x166] = OpSsePcmpgtd,
    [0x167] = OpSsePackuswb,
    [0x168] = OpSsePunpckhbw,
    [0x169] = OpSsePunpckhwd,
    [0x16A] = OpSsePunpckhdq,
    [0x16B] = OpSsePackssdw,
    [0x16C] = OpSsePunpcklqdq,
    [0x16D] = OpSsePunpckhqdq,
    [0x16E] = OpMov0f6e,
    [0x16F] = OpMov0f6f,
    [0x170] = OpShuffle,
    [0x171] = Op171,
    [0x172] = Op172,
    [0x173] = Op173,
    [0x174] = OpSsePcmpeqb,
    [0x175] = OpSsePcmpeqw,
    [0x176] = OpSsePcmpeqd,
    [0x177] = OpUd,
    [0x178] = OpUd,
    [0x179] = OpUd,
    [0x17A] = OpUd,
    [0x17B] = OpUd,
    [0x17C] = OpHaddpsd,
    [0x17D] = OpHsubpsd,
    [0x17E] = OpMov0f7e,
    [0x17F] = OpMov0f7f,
    [0x180] = OpJo,
    [0x181] = OpJno,
    [0x182] = OpJb,
    [0x183] = OpJae,
    [0x184] = OpJe,
    [0x185] = OpJne,
    [0x186] = OpJbe,
    [0x187] = OpJa,
    [0x188] = OpJs,
    [0x189] = OpJns,
    [0x18A] = OpJp,
    [0x18B] = OpJnp,
    [0x18C] = OpJl,
    [0x18D] = OpJge,
    [0x18E] = OpJle,
    [0x18F] = OpJg,
    [0x190] = OpSeto,
    [0x191] = OpSetno,
    [0x192] = OpSetb,
    [0x193] = OpSetae,
    [0x194] = OpSete,
    [0x195] = OpSetne,
    [0x196] = OpSetbe,
    [0x197] = OpSeta,
    [0x198] = OpSets,
    [0x199] = OpSetns,
    [0x19A] = OpSetp,
    [0x19B] = OpSetnp,
    [0x19C] = OpSetl,
    [0x19D] = OpSetge,
    [0x19E] = OpSetle,
    [0x19F] = OpSetg,
    [0x1A0] = OpPushSeg,
    [0x1A1] = OpPopSeg,
    [0x1A2] = OpCpuid,
    [0x1A3] = OpBit,
    [0x1A4] = OpDoubleShift,
    [0x1A5] = OpDoubleShift,
    [0x1A6] = OpUd,
    [0x1A7] = OpUd,
    [0x1A8] = OpPushSeg,
    [0x1A9] = OpPopSeg,
    [0x1AA] = OpUd,
    [0x1AB] = OpBit,
    [0x1AC] = OpDoubleShift,
    [0x1AD] = OpDoubleShift,
    [0x1AE] = Op1ae,
    [0x1AF] = OpImulGvqpEvqp,
    [0x1B0] = OpCmpxchgEbAlGb,
    [0x1B1] = OpCmpxchgEvqpRaxGvqp,
    [0x1B2] = OpUd,
    [0x1B3] = OpBit,
    [0x1B4] = OpUd,
    [0x1B5] = OpUd,
    [0x1B6] = OpMovzbGvqpEb,
    [0x1B7] = OpMovzwGvqpEw,
    [0x1B8] = Op1b8,
    [0x1B9] = OpUd,
    [0x1BA] = OpBit,
    [0x1BB] = OpBit,
    [0x1BC] = OpBsf,
    [0x1BD] = OpBsr,
    [0x1BE] = OpMovsbGvqpEb,
    [0x1BF] = OpMovswGvqpEw,
    [0x1C0] = OpXaddEbGb,
    [0x1C1] = OpXaddEvqpGvqp,
    [0x1C2] = OpCmppsd,
    [0x1C3] = OpMovntiMdqpGdqp,
    [0x1C4] = OpPinsrwVdqEwIb,
    [0x1C5] = OpPextrwGdqpUdqIb,
    [0x1C6] = OpShufpsd,
    [0x1C7] = Op1c7,
    [0x1C8] = OpBswapZvqp,
    [0x1C9] = OpBswapZvqp,
    [0x1CA] = OpBswapZvqp,
    [0x1CB] = OpBswapZvqp,
    [0x1CC] = OpBswapZvqp,
    [0x1CD] = OpBswapZvqp,
    [0x1CE] = OpBswapZvqp,
    [0x1CF] = OpBswapZvqp,
    [0x1D0] = OpAddsubpsd,
    [0x1D1] = OpSsePsrlwv,
    [0x1D2] = OpSsePsrldv,
    [0x1D3] = OpSsePsrlqv,
    [0x1D4] = OpSsePaddq,
    [0x1D5] = OpSsePmullw,
    [0x1D6] = OpMov0fD6,
    [0x1D7] = OpPmovmskbGdqpNqUdq,
    [0x1D8] = OpSsePsubusb,
    [0x1D9] = OpSsePsubusw,
    [0x1DA] = OpSsePminub,
    [0x1DB] = OpSsePand,
    [0x1DC] = OpSsePaddusb,
    [0x1DD] = OpSsePaddusw,
    [0x1DE] = OpSsePmaxub,
    [0x1DF] = OpSsePandn,
    [0x1E0] = OpSsePavgb,
    [0x1E1] = OpSsePsrawv,
    [0x1E2] = OpSsePsradv,
    [0x1E3] = OpSsePavgw,
    [0x1E4] = OpSsePmulhuw,
    [0x1E5] = OpSsePmulhw,
    [0x1E6] = OpCvt0fE6,
    [0x1E7] = OpMov0fE7,
    [0x1E8] = OpSsePsubsb,
    [0x1E9] = OpSsePsubsw,
    [0x1EA] = OpSsePminsw,
    [0x1EB] = OpSsePor,
    [0x1EC] = OpSsePaddsb,
    [0x1ED] = OpSsePaddsw,
    [0x1EE] = OpSsePmaxsw,
    [0x1EF] = OpSsePxor,
    [0x1F0] = OpLddquVdqMdq,
    [0x1F1] = OpSsePsllwv,
    [0x1F2] = OpSsePslldv,
    [0x1F3] = OpSsePsllqv,
    [0x1F4] = OpSsePmuludq,
    [0x1F5] = OpSsePmaddwd,
    [0x1F6] = OpSsePsadbw,
    [0x1F7] = OpMaskMovDiXmmRegXmmRm,
    [0x1F8] = OpSsePsubb,
    [0x1F9] = OpSsePsubw,
    [0x1FA] = OpSsePsubd,
    [0x1FB] = OpSsePsubq,
    [0x1FC] = OpSsePaddb,
    [0x1FD] = OpSsePaddw,
    [0x1FE] = OpSsePaddd,
    [0x1FF] = OpUd,
    [0x200] = OpSsePshufb,
    [0x201] = OpSsePhaddw,
    [0x202] = OpSsePhaddd,
    [0x203] = OpSsePhaddsw,
    [0x204] = OpSsePmaddubsw,
    [0x205] = OpSsePhsubw,
    [0x206] = OpSsePhsubd,
    [0x207] = OpSsePhsubsw,
    [0x208] = OpSsePsignb,
    [0x209] = OpSsePsignw,
    [0x20A] = OpSsePsignd,
    [0x20B] = OpSsePmulhrsw,
};

void ExecuteSparseInstruction(struct Machine *m, uint32_t rde, uint32_t d) {
  switch (d) {
    CASE(0x21c, OpSsePabsb(m, rde));
    CASE(0x21d, OpSsePabsw(m, rde));
    CASE(0x21e, OpSsePabsd(m, rde));
    CASE(0x22a, OpMovntdqaVdqMdq(m, rde));
    CASE(0x240, OpSsePmulld(m, rde));
    CASE(0x30f, OpSsePalignr(m, rde));
    default:
      OpUd(m, rde);
  }
}

void ExecuteInstruction(struct Machine *m) {
  m->ip += m->xedd->length;
  if (m->xedd->op.dispatch < ARRAYLEN(kNexgen32e)) {
    kNexgen32e[m->xedd->op.dispatch](m, m->xedd->op.rde);
  } else {
    ExecuteSparseInstruction(m, m->xedd->op.rde, m->xedd->op.dispatch);
  }
  if (m->stashaddr) {
    VirtualRecv(m, m->stashaddr, m->stash, m->stashsize);
    m->stashaddr = 0;
  }
}
