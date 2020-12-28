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
#include "libc/limits.h"
#include "libc/log/log.h"
#include "tool/build/lib/divmul.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/throw.h"

void OpDivAlAhAxEbSigned(struct Machine *m, uint32_t rde) {
  int8_t y, rem;
  int16_t x, quo;
  x = Read16(m->ax);
  y = Read8(GetModrmRegisterBytePointerRead(m, rde));
  if (!y || (x == INT16_MIN && y == -1)) ThrowDivideError(m);
  quo = x / y;
  rem = x % y;
  if (!(INT8_MIN <= quo && quo <= INT8_MAX)) ThrowDivideError(m);
  m->ax[0] = quo & 0xff;
  m->ax[1] = rem & 0xff;
}

void OpDivAlAhAxEbUnsigned(struct Machine *m, uint32_t rde) {
  uint8_t y, rem;
  uint16_t x, quo;
  x = Read16(m->ax);
  y = Read8(GetModrmRegisterBytePointerRead(m, rde));
  if (!y) ThrowDivideError(m);
  quo = x / y;
  rem = x % y;
  if (!(UINT8_MIN <= quo && quo <= UINT8_MAX)) ThrowDivideError(m);
  m->ax[0] = quo & 0xff;
  m->ax[1] = rem & 0xff;
}

static void OpDivRdxRaxEvqpSigned64(struct Machine *m, uint32_t rde,
                                    uint8_t *p) {
  int64_t y, rem;
  int128_t x, quo;
  x = (uint128_t)Read64(m->dx) << 64 | Read64(m->ax);
  y = Read64(p);
  if (!y || (x == INT128_MIN && y == -1)) ThrowDivideError(m);
  quo = x / y;
  rem = x % y;
  if (!(INT64_MIN <= quo && quo <= INT64_MAX)) ThrowDivideError(m);
  Write64(m->ax, quo);
  Write64(m->dx, rem);
}

static void OpDivRdxRaxEvqpSigned32(struct Machine *m, uint32_t rde,
                                    uint8_t *p) {
  int32_t y, rem;
  int64_t x, quo;
  x = (uint64_t)Read32(m->dx) << 32 | Read32(m->ax);
  y = Read32(p);
  if (!y || (x == INT64_MIN && y == -1)) ThrowDivideError(m);
  quo = x / y;
  rem = x % y;
  if (!(INT32_MIN <= quo && quo <= INT32_MAX)) ThrowDivideError(m);
  Write64(m->ax, quo & 0xffffffff);
  Write64(m->dx, rem & 0xffffffff);
}

static void OpDivRdxRaxEvqpSigned16(struct Machine *m, uint32_t rde,
                                    uint8_t *p) {
  int16_t y, rem;
  int32_t x, quo;
  x = (uint32_t)Read16(m->dx) << 16 | Read16(m->ax);
  y = Read16(p);
  if (!y || (x == INT32_MIN && y == -1)) ThrowDivideError(m);
  quo = x / y;
  rem = x % y;
  if (!(INT16_MIN <= quo && quo <= INT16_MAX)) ThrowDivideError(m);
  Write16(m->ax, quo);
  Write16(m->dx, rem);
}

static void OpDivRdxRaxEvqpUnsigned16(struct Machine *m, uint32_t rde,
                                      uint8_t *p) {
  uint16_t y, rem;
  uint32_t x, quo;
  x = (uint32_t)Read16(m->dx) << 16 | Read16(m->ax);
  y = Read16(p);
  if (!y) ThrowDivideError(m);
  quo = x / y;
  rem = x % y;
  if (!(UINT16_MIN <= quo && quo <= UINT16_MAX)) ThrowDivideError(m);
  Write16(m->ax, quo);
  Write16(m->dx, rem);
}

static void OpDivRdxRaxEvqpUnsigned32(struct Machine *m, uint32_t rde,
                                      uint8_t *p) {
  uint32_t y, rem;
  uint64_t x, quo;
  x = (uint64_t)Read32(m->dx) << 32 | Read32(m->ax);
  y = Read32(p);
  if (!y) ThrowDivideError(m);
  quo = x / y;
  rem = x % y;
  if (!(UINT32_MIN <= quo && quo <= UINT32_MAX)) ThrowDivideError(m);
  Write64(m->ax, quo & 0xffffffff);
  Write64(m->dx, rem & 0xffffffff);
}

static void OpDivRdxRaxEvqpUnsigned64(struct Machine *m, uint32_t rde,
                                      uint8_t *p) {
  uint64_t y, rem;
  uint128_t x, quo;
  x = (uint128_t)Read64(m->dx) << 64 | Read64(m->ax);
  y = Read64(p);
  if (!y) ThrowDivideError(m);
  quo = x / y;
  rem = x % y;
  if (!(UINT64_MIN <= quo && quo <= UINT64_MAX)) ThrowDivideError(m);
  Write64(m->ax, quo);
  Write64(m->dx, rem);
}

void OpDivRdxRaxEvqpSigned(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  p = GetModrmRegisterWordPointerReadOszRexw(m, rde);
  if (Rexw(rde)) {
    OpDivRdxRaxEvqpSigned64(m, rde, p);
  } else if (!Osz(rde)) {
    OpDivRdxRaxEvqpSigned32(m, rde, p);
  } else {
    OpDivRdxRaxEvqpSigned16(m, rde, p);
  }
}

void OpDivRdxRaxEvqpUnsigned(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  p = GetModrmRegisterWordPointerReadOszRexw(m, rde);
  if (Rexw(rde)) {
    OpDivRdxRaxEvqpUnsigned64(m, rde, p);
  } else if (!Osz(rde)) {
    OpDivRdxRaxEvqpUnsigned32(m, rde, p);
  } else {
    OpDivRdxRaxEvqpUnsigned16(m, rde, p);
  }
}

void OpMulAxAlEbSigned(struct Machine *m, uint32_t rde) {
  bool of;
  int16_t ax;
  uint8_t *p;
  p = GetModrmRegisterBytePointerRead(m, rde);
  ax = (int8_t)Read8(m->ax) * (int8_t)Read8(p);
  of = ax != (int8_t)ax;
  m->flags = SetFlag(m->flags, FLAGS_CF, of);
  m->flags = SetFlag(m->flags, FLAGS_OF, of);
  Write16(m->ax, ax);
}

void OpMulAxAlEbUnsigned(struct Machine *m, uint32_t rde) {
  int ax;
  bool of;
  uint8_t *p;
  p = GetModrmRegisterBytePointerRead(m, rde);
  ax = Read8(m->ax) * Read8(p);
  of = ax != (uint8_t)ax;
  m->flags = SetFlag(m->flags, FLAGS_CF, of);
  m->flags = SetFlag(m->flags, FLAGS_OF, of);
  Write16(m->ax, ax);
}

void OpMulRdxRaxEvqpSigned(struct Machine *m, uint32_t rde) {
  bool of;
  uint8_t *p;
  int32_t dxax;
  int64_t edxeax;
  int128_t rdxrax;
  p = GetModrmRegisterWordPointerReadOszRexw(m, rde);
  if (Rexw(rde)) {
    __builtin_mul_overflow((int128_t)(int64_t)Read64(m->ax), (int64_t)Read64(p),
                           &rdxrax);
    of = (int128_t)rdxrax != (int64_t)rdxrax;
    Write64(m->ax, rdxrax);
    Write64(m->dx, rdxrax >> 64);
  } else if (!Osz(rde)) {
    __builtin_mul_overflow((int64_t)(int32_t)Read32(m->ax), (int32_t)Read32(p),
                           &edxeax);
    of = (int64_t)edxeax != (int32_t)edxeax;
    Write64(m->ax, edxeax);
    Write64(m->dx, edxeax >> 32);
  } else {
    __builtin_mul_overflow((int32_t)(int16_t)Read16(m->ax), (int16_t)Read16(p),
                           &dxax);
    of = (int32_t)dxax != (int16_t)dxax;
    Write16(m->ax, dxax);
    Write16(m->dx, dxax >> 16);
  }
  m->flags = SetFlag(m->flags, FLAGS_CF, of);
  m->flags = SetFlag(m->flags, FLAGS_OF, of);
}

void OpMulRdxRaxEvqpUnsigned(struct Machine *m, uint32_t rde) {
  bool of;
  uint8_t *p;
  uint32_t dxax;
  uint64_t edxeax;
  uint128_t rdxrax;
  p = GetModrmRegisterWordPointerReadOszRexw(m, rde);
  if (Rexw(rde)) {
    __builtin_mul_overflow((uint128_t)Read64(m->ax), Read64(p), &rdxrax);
    of = (uint64_t)rdxrax != rdxrax;
    Write64(m->ax, rdxrax);
    Write64(m->dx, rdxrax >> 64);
  } else if (!Osz(rde)) {
    __builtin_mul_overflow((uint64_t)Read32(m->ax), Read32(p), &edxeax);
    of = (uint32_t)edxeax != edxeax;
    Write64(m->ax, edxeax);
    Write64(m->dx, edxeax >> 32);
  } else {
    __builtin_mul_overflow((uint32_t)(uint16_t)Read16(m->ax),
                           (uint16_t)Read16(p), &dxax);
    of = (uint16_t)dxax != dxax;
    Write16(m->ax, dxax);
    Write16(m->dx, dxax >> 16);
  }
  m->flags = SetFlag(m->flags, FLAGS_CF, of);
  m->flags = SetFlag(m->flags, FLAGS_OF, of);
}

static void AluImul(struct Machine *m, uint32_t rde, uint8_t *a, uint8_t *b) {
  unsigned of;
  if (Rexw(rde)) {
    int64_t x, y, z;
    x = Read64(a);
    y = Read64(b);
    of = __builtin_mul_overflow(x, y, &z);
    Write64(RegRexrReg(m, rde), z);
  } else if (!Osz(rde)) {
    int32_t x, y, z;
    x = Read32(a);
    y = Read32(b);
    of = __builtin_mul_overflow(x, y, &z);
    Write64(RegRexrReg(m, rde), z & 0xffffffff);
  } else {
    int z;
    int16_t x, y;
    x = Read16(a);
    y = Read16(b);
    z = x * y;
    of = z != (int16_t)z;
    Write16(RegRexrReg(m, rde), z);
  }
  m->flags = SetFlag(m->flags, FLAGS_CF, of);
  m->flags = SetFlag(m->flags, FLAGS_OF, of);
}

void OpImulGvqpEvqp(struct Machine *m, uint32_t rde) {
  AluImul(m, rde, RegRexrReg(m, rde),
          GetModrmRegisterWordPointerReadOszRexw(m, rde));
}

void OpImulGvqpEvqpImm(struct Machine *m, uint32_t rde) {
  uint8_t b[8];
  Write64(b, m->xedd->op.uimm0);
  AluImul(m, rde, GetModrmRegisterWordPointerReadOszRexw(m, rde), b);
}
