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
#include "tool/build/lib/divmul.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/throw.h"

struct Dubble {
  uint64_t lo;
  uint64_t hi;
};

static inline struct Dubble DubbleNeg(struct Dubble x) {
  struct Dubble d;
  d.lo = -x.lo;
  d.hi = ~(x.hi - (x.lo - 1 > x.lo));
  return d;
}

static inline struct Dubble DubbleShl(struct Dubble x) {
  struct Dubble d;
  d.lo = x.lo << 1;
  d.hi = x.hi << 1 | x.lo >> 63;
  return d;
}

static inline struct Dubble DubbleShr(struct Dubble x) {
  struct Dubble d;
  d.lo = x.lo >> 1 | x.hi << 63;
  d.hi = x.hi >> 1;
  return d;
}

static inline unsigned DubbleLte(struct Dubble a, struct Dubble b) {
  return a.hi == b.hi ? a.lo <= b.lo : a.hi <= b.hi;
}

static struct Dubble DubbleMul(uint64_t a, uint64_t b) {
  struct Dubble d;
  uint64_t x, y, t;
  x = (a & 0xffffffff) * (b & 0xffffffff);
  t = x >> 32;
  x &= 0xffffffff;
  t += (a >> 32) * (b & 0xffffffff);
  x += (t & 0xffffffff) << 32;
  y = t >> 32;
  t = x >> 32;
  x &= 0xffffffff;
  t += (b >> 32) * (a & 0xffffffff);
  x += (t & 0xffffffff) << 32;
  y += t >> 32;
  y += (a >> 32) * (b >> 32);
  d.lo = x;
  d.hi = y;
  return d;
}

static struct Dubble DubbleImul(uint64_t a, uint64_t b) {
  unsigned s, t;
  struct Dubble p;
  if ((s = a >> 63)) a = -a;
  if ((t = b >> 63)) b = -b;
  p = DubbleMul(a, b);
  return s ^ t ? DubbleNeg(p) : p;
}

static struct Dubble DubbleDiv(struct Dubble a, uint64_t b, uint64_t *r) {
  int n, c;
  uint64_t s;
  struct Dubble d, q, t;
  d.lo = b, d.hi = 0;
  q.lo = 0, q.hi = 0;
  for (n = 0; DubbleLte(d, a) && n < 128; ++n) {
    d = DubbleShl(d);
  }
  for (; n > 0; --n) {
    t = a;
    d = DubbleShr(d);
    q = DubbleShl(q);
    s = a.lo, a.lo -= d.lo + 0, c = a.lo > s;
    s = a.hi, a.hi -= d.hi + c, c = a.hi > s;
    if (c) {
      a = t;
    } else {
      q.lo++;
    }
  }
  *r = a.lo;
  return q;
}

static struct Dubble DubbleIdiv(struct Dubble a, uint64_t b, uint64_t *r) {
  unsigned s, t;
  struct Dubble q;
  if ((s = a.hi >> 63)) a = DubbleNeg(a);
  if ((t = b >> 63)) b = -b;
  q = DubbleDiv(a, b, r);
  if (s ^ t) q = DubbleNeg(q);
  if (s) *r = -*r;
  return q;
}

void OpDivAlAhAxEbSigned(struct Machine *m, uint32_t rde) {
  int8_t y, r;
  int16_t x, q;
  x = Read16(m->ax);
  y = Read8(GetModrmRegisterBytePointerRead(m, rde));
  if (!y) ThrowDivideError(m);
  if (x == INT16_MIN) ThrowDivideError(m);
  q = x / y;
  r = x % y;
  if (q != (int8_t)q) ThrowDivideError(m);
  m->ax[0] = q & 0xff;
  m->ax[1] = r & 0xff;
}

void OpDivAlAhAxEbUnsigned(struct Machine *m, uint32_t rde) {
  uint8_t y, r;
  uint16_t x, q;
  x = Read16(m->ax);
  y = Read8(GetModrmRegisterBytePointerRead(m, rde));
  if (!y) ThrowDivideError(m);
  q = x / y;
  r = x % y;
  if (q > 255) ThrowDivideError(m);
  m->ax[0] = q & 0xff;
  m->ax[1] = r & 0xff;
}

static void OpDivRdxRaxEvqpSigned64(struct Machine *m, uint32_t rde,
                                    uint8_t *p) {
  uint64_t d, r;
  struct Dubble q;
  q.lo = Read64(m->ax);
  q.hi = Read64(m->dx);
  d = Read64(p);
  if (!d) ThrowDivideError(m);
  if (!q.lo && q.hi == 0x8000000000000000) ThrowDivideError(m);
  q = DubbleIdiv(q, d, &r);
  if ((int64_t)q.lo < 0 && (int64_t)q.hi != -1) ThrowDivideError(m);
  if ((int64_t)q.lo >= 0 && q.hi) ThrowDivideError(m);
  Write64(m->ax, q.lo);
  Write64(m->dx, r);
}

static void OpDivRdxRaxEvqpSigned32(struct Machine *m, uint32_t rde,
                                    uint8_t *p) {
  int32_t y, r;
  int64_t x, q;
  x = (uint64_t)Read32(m->dx) << 32 | Read32(m->ax);
  y = Read32(p);
  if (!y) ThrowDivideError(m);
  if (x == INT64_MIN) ThrowDivideError(m);
  q = x / y;
  r = x % y;
  if (q != (int32_t)q) ThrowDivideError(m);
  Write64(m->ax, q & 0xffffffff);
  Write64(m->dx, r & 0xffffffff);
}

static void OpDivRdxRaxEvqpSigned16(struct Machine *m, uint32_t rde,
                                    uint8_t *p) {
  int16_t y, r;
  int32_t x, q;
  x = (uint32_t)Read16(m->dx) << 16 | Read16(m->ax);
  y = Read16(p);
  if (!y) ThrowDivideError(m);
  if (x == INT32_MIN) ThrowDivideError(m);
  q = x / y;
  r = x % y;
  if (q != (int16_t)q) ThrowDivideError(m);
  Write16(m->ax, q);
  Write16(m->dx, r);
}

static void OpDivRdxRaxEvqpUnsigned16(struct Machine *m, uint32_t rde,
                                      uint8_t *p) {
  uint16_t y, r;
  uint32_t x, q;
  x = (uint32_t)Read16(m->dx) << 16 | Read16(m->ax);
  y = Read16(p);
  if (!y) ThrowDivideError(m);
  q = x / y;
  r = x % y;
  if (q > 65535) ThrowDivideError(m);
  Write16(m->ax, q);
  Write16(m->dx, r);
}

static void OpDivRdxRaxEvqpUnsigned32(struct Machine *m, uint32_t rde,
                                      uint8_t *p) {
  uint32_t y, r;
  uint64_t x, q;
  x = (uint64_t)Read32(m->dx) << 32 | Read32(m->ax);
  y = Read32(p);
  if (!y) ThrowDivideError(m);
  q = x / y;
  r = x % y;
  if (q > 4294967295) ThrowDivideError(m);
  Write64(m->ax, q & 0xffffffff);
  Write64(m->dx, r & 0xffffffff);
}

static void OpDivRdxRaxEvqpUnsigned64(struct Machine *m, uint32_t rde,
                                      uint8_t *p) {
  uint64_t d, r;
  struct Dubble q;
  q.lo = Read64(m->ax);
  q.hi = Read64(m->dx);
  d = Read64(p);
  if (!d) ThrowDivideError(m);
  q = DubbleDiv(q, d, &r);
  if (q.hi) ThrowDivideError(m);
  Write64(m->ax, q.lo);
  Write64(m->dx, r);
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
  int16_t ax;
  uint8_t *p;
  unsigned of;
  p = GetModrmRegisterBytePointerRead(m, rde);
  ax = (int8_t)Read8(m->ax) * (int8_t)Read8(p);
  of = ax != (int8_t)ax;
  m->flags = SetFlag(m->flags, FLAGS_CF, of);
  m->flags = SetFlag(m->flags, FLAGS_OF, of);
  Write16(m->ax, ax);
}

void OpMulAxAlEbUnsigned(struct Machine *m, uint32_t rde) {
  int ax;
  uint8_t *p;
  unsigned of;
  p = GetModrmRegisterBytePointerRead(m, rde);
  ax = Read8(m->ax) * Read8(p);
  of = ax != (uint8_t)ax;
  m->flags = SetFlag(m->flags, FLAGS_CF, of);
  m->flags = SetFlag(m->flags, FLAGS_OF, of);
  Write16(m->ax, ax);
}

void OpMulRdxRaxEvqpSigned(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  unsigned of;
  int32_t dxax;
  int64_t edxeax;
  struct Dubble rdxrax;
  p = GetModrmRegisterWordPointerReadOszRexw(m, rde);
  if (Rexw(rde)) {
    rdxrax = DubbleImul(Read64(m->ax), Read64(p));
    of = !!(rdxrax.hi + (rdxrax.lo >> 63));
    Write64(m->ax, rdxrax.lo);
    Write64(m->dx, rdxrax.hi);
  } else if (!Osz(rde)) {
    edxeax = (int64_t)(int32_t)Read32(m->ax) * (int32_t)Read32(p);
    of = edxeax != (int32_t)edxeax;
    Write64(m->ax, edxeax);
    Write64(m->dx, edxeax >> 32);
  } else {
    dxax = (int32_t)(int16_t)Read16(m->ax) * (int16_t)Read16(p);
    of = dxax != (int16_t)dxax;
    Write16(m->ax, dxax);
    Write16(m->dx, dxax >> 16);
  }
  m->flags = SetFlag(m->flags, FLAGS_CF, of);
  m->flags = SetFlag(m->flags, FLAGS_OF, of);
}

void OpMulRdxRaxEvqpUnsigned(struct Machine *m, uint32_t rde) {
  uint8_t *p;
  unsigned of;
  uint32_t dxax;
  uint64_t edxeax;
  struct Dubble rdxrax;
  p = GetModrmRegisterWordPointerReadOszRexw(m, rde);
  if (Rexw(rde)) {
    rdxrax = DubbleMul(Read64(m->ax), Read64(p));
    of = !!rdxrax.hi;
    Write64(m->ax, rdxrax.lo);
    Write64(m->dx, rdxrax.hi);
  } else if (!Osz(rde)) {
    edxeax = (uint64_t)Read32(m->ax) * Read32(p);
    of = (uint32_t)edxeax != edxeax;
    Write64(m->ax, edxeax);
    Write64(m->dx, edxeax >> 32);
  } else {
    dxax = (uint32_t)(uint16_t)Read16(m->ax) * (uint16_t)Read16(p);
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
    struct Dubble p;
    p = DubbleImul(Read64(a), Read64(b));
    of = !!(p.hi + (p.lo >> 63));
    Write64(RegRexrReg(m, rde), p.lo);
  } else if (!Osz(rde)) {
    int64_t z;
    z = (int64_t)(int32_t)Read32(a) * (int32_t)Read32(b);
    of = z != (int32_t)z;
    Write64(RegRexrReg(m, rde), z & 0xffffffff);
  } else {
    int32_t z;
    z = (int32_t)(int16_t)Read16(a) * (int16_t)Read16(b);
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
