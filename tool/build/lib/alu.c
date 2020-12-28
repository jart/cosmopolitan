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
#include "tool/build/lib/alu.h"
#include "tool/build/lib/flags.h"

const aluop_f kAlu[12][4] = {
    {Add8, Add16, Add32, Add64}, {Or8, Or16, Or32, Or64},
    {Adc8, Adc16, Adc32, Adc64}, {Sbb8, Sbb16, Sbb32, Sbb64},
    {And8, And16, And32, And64}, {Sub8, Sub16, Sub32, Sub64},
    {Xor8, Xor16, Xor32, Xor64}, {Sub8, Sub16, Sub32, Sub64},
    {Not8, Not16, Not32, Not64}, {Neg8, Neg16, Neg32, Neg64},
    {Inc8, Inc16, Inc32, Inc64}, {Dec8, Dec16, Dec32, Dec64},
};

const aluop_f kBsu[8][4] = {
    {Rol8, Rol16, Rol32, Rol64}, {Ror8, Ror16, Ror32, Ror64},
    {Rcl8, Rcl16, Rcl32, Rcl64}, {Rcr8, Rcr16, Rcr32, Rcr64},
    {Shl8, Shl16, Shl32, Shl64}, {Shr8, Shr16, Shr32, Shr64},
    {Shl8, Shl16, Shl32, Shl64}, {Sar8, Sar16, Sar32, Sar64},
};

int64_t AluFlags8(uint8_t z, uint32_t af, uint32_t *f, uint32_t of,
                  uint32_t cf) {
  return AluFlags(z, af, f, of, cf, z >> 7);
}

int64_t AluFlags32(uint32_t z, uint32_t af, uint32_t *f, uint32_t of,
                   uint32_t cf) {
  return AluFlags(z, af, f, of, cf, z >> 31);
}

int64_t Xor32(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags32(x ^ y, 0, f, 0, 0);
}

int64_t Sub32(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint32_t x, y, z;
  x = x64;
  y = y64;
  z = x - y;
  cf = x < z;
  af = (x & 15) < (z & 15);
  of = ((z ^ x) & (x ^ y)) >> 31;
  return AluFlags32(z, af, f, of, cf);
}

int64_t AluFlags64(uint64_t z, uint32_t af, uint32_t *f, uint32_t of,
                   uint32_t cf) {
  return AluFlags(z, af, f, of, cf, z >> 63);
}

int64_t Sub64(uint64_t x, uint64_t y, uint32_t *f) {
  uint64_t z;
  bool cf, of, af;
  z = x - y;
  cf = x < z;
  af = (x & 15) < (z & 15);
  of = ((z ^ x) & (x ^ y)) >> 63;
  return AluFlags64(z, af, f, of, cf);
}

int64_t Xor8(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags8(x ^ y, 0, f, 0, 0);
}

int64_t Xor64(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags64(x ^ y, 0, f, 0, 0);
}

int64_t Or8(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags8(x | y, 0, f, 0, 0);
}

int64_t Or32(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags32(x | y, 0, f, 0, 0);
}

int64_t Or64(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags64(x | y, 0, f, 0, 0);
}

int64_t And8(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags8(x & y, 0, f, 0, 0);
}

int64_t And32(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags32(x & y, 0, f, 0, 0);
}

int64_t And64(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags64(x & y, 0, f, 0, 0);
}

int64_t Sub8(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint8_t x, y, z;
  x = x64;
  y = y64;
  z = x - y;
  cf = x < z;
  af = (x & 15) < (z & 15);
  of = ((z ^ x) & (x ^ y)) >> 7;
  return AluFlags8(z, af, f, of, cf);
}

int64_t Add8(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint8_t x, y, z;
  x = x64;
  y = y64;
  z = x + y;
  cf = z < y;
  af = (z & 15) < (y & 15);
  of = ((z ^ x) & (z ^ y)) >> 7;
  return AluFlags8(z, af, f, of, cf);
}

int64_t Add32(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint32_t x, y, z;
  x = x64;
  y = y64;
  z = x + y;
  cf = z < y;
  af = (z & 15) < (y & 15);
  of = ((z ^ x) & (z ^ y)) >> 31;
  return AluFlags32(z, af, f, of, cf);
}

int64_t Add64(uint64_t x, uint64_t y, uint32_t *f) {
  uint64_t z;
  bool cf, of, af;
  z = x + y;
  cf = z < y;
  af = (z & 15) < (y & 15);
  of = ((z ^ x) & (z ^ y)) >> 63;
  return AluFlags64(z, af, f, of, cf);
}

int64_t Adc8(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint8_t x, y, z, t;
  x = x64;
  y = y64;
  t = x + GetFlag(*f, FLAGS_CF);
  z = t + y;
  cf = (t < x) | (z < y);
  of = ((z ^ x) & (z ^ y)) >> 7;
  af = ((t & 15) < (x & 15)) | ((z & 15) < (y & 15));
  return AluFlags8(z, af, f, of, cf);
}

int64_t Adc32(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint32_t x, y, z, t;
  x = x64;
  y = y64;
  t = x + GetFlag(*f, FLAGS_CF);
  z = t + y;
  cf = (t < x) | (z < y);
  of = ((z ^ x) & (z ^ y)) >> 31;
  af = ((t & 15) < (x & 15)) | ((z & 15) < (y & 15));
  return AluFlags32(z, af, f, of, cf);
}

int64_t Adc64(uint64_t x, uint64_t y, uint32_t *f) {
  uint64_t z, t;
  bool cf, of, af;
  t = x + GetFlag(*f, FLAGS_CF);
  z = t + y;
  cf = (t < x) | (z < y);
  of = ((z ^ x) & (z ^ y)) >> 63;
  af = ((t & 15) < (x & 15)) | ((z & 15) < (y & 15));
  return AluFlags64(z, af, f, of, cf);
}

int64_t Sbb8(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint8_t x, y, z, t;
  x = x64;
  y = y64;
  t = x - GetFlag(*f, FLAGS_CF);
  z = t - y;
  cf = (x < t) | (t < z);
  of = ((z ^ x) & (x ^ y)) >> 7;
  af = ((x & 15) < (t & 15)) | ((t & 15) < (z & 15));
  return AluFlags8(z, af, f, of, cf);
}

int64_t Sbb32(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint32_t x, y, z, t;
  x = x64;
  y = y64;
  t = x - GetFlag(*f, FLAGS_CF);
  z = t - y;
  cf = (x < t) | (t < z);
  of = ((z ^ x) & (x ^ y)) >> 31;
  af = ((x & 15) < (t & 15)) | ((t & 15) < (z & 15));
  return AluFlags32(z, af, f, of, cf);
}

int64_t Sbb64(uint64_t x, uint64_t y, uint32_t *f) {
  uint64_t z, t;
  bool cf, of, af;
  t = x - GetFlag(*f, FLAGS_CF);
  z = t - y;
  cf = (x < t) | (t < z);
  of = ((z ^ x) & (x ^ y)) >> 63;
  af = ((x & 15) < (t & 15)) | ((t & 15) < (z & 15));
  return AluFlags64(z, af, f, of, cf);
}

int64_t Not8(uint64_t x, uint64_t y, uint32_t *f) {
  return ~x & 0xFF;
}

int64_t Not32(uint64_t x, uint64_t y, uint32_t *f) {
  return ~x & 0xFFFFFFFF;
}

int64_t Not64(uint64_t x, uint64_t y, uint32_t *f) {
  return ~x & 0xFFFFFFFFFFFFFFFF;
}

int64_t Neg8(uint64_t x64, uint64_t y, uint32_t *f) {
  uint8_t x;
  bool cf, of, af;
  x = x64;
  af = cf = !!x;
  of = x == 0x80;
  x = ~x + 1;
  return AluFlags8(x, af, f, of, cf);
}

int64_t Neg32(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x;
  bool cf, of, af;
  x = x64;
  af = cf = !!x;
  of = x == 0x80000000;
  x = ~x + 1;
  return AluFlags32(x, af, f, of, cf);
}

int64_t Neg64(uint64_t x64, uint64_t y, uint32_t *f) {
  uint64_t x;
  bool cf, of, af;
  x = x64;
  af = cf = !!x;
  of = x == 0x8000000000000000;
  x = ~x + 1;
  return AluFlags64(x, af, f, of, cf);
}

static int64_t BumpFlags(uint64_t x, uint32_t af, uint32_t *f, uint32_t of,
                         uint32_t sf) {
  return AluFlags(x, af, f, of, GetFlag(*f, FLAGS_CF), sf);
}

int64_t Dec32(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x, z, of, sf, af;
  x = x64;
  z = x - 1;
  sf = z >> 31;
  af = (x & 15) < (z & 15);
  of = ((z ^ x) & (x ^ 1)) >> 31;
  return BumpFlags(z, af, f, of, sf);
}

int64_t Inc32(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x, z, of, sf, af;
  x = x64;
  z = x + 1;
  sf = z >> 31;
  af = (z & 15) < (y & 15);
  of = ((z ^ x) & (z ^ 1)) >> 31;
  return BumpFlags(z, af, f, of, sf);
}

int64_t Inc64(uint64_t x, uint64_t y, uint32_t *f) {
  uint64_t z;
  uint32_t of, sf, af;
  z = x + 1;
  sf = z >> 63;
  af = (z & 15) < (y & 15);
  of = ((z ^ x) & (z ^ 1)) >> 63;
  return BumpFlags(z, af, f, of, sf);
}

int64_t Dec64(uint64_t x, uint64_t y, uint32_t *f) {
  uint64_t z;
  uint32_t of, sf, af;
  z = x - 1;
  sf = z >> 63;
  af = (x & 15) < (z & 15);
  of = ((z ^ x) & (x ^ 1)) >> 63;
  return BumpFlags(z, af, f, of, sf);
}

int64_t Inc8(uint64_t x64, uint64_t y, uint32_t *f) {
  uint8_t x, z;
  uint32_t of, sf, af;
  x = x64;
  z = x + 1;
  sf = z >> 7;
  af = (z & 15) < (y & 15);
  of = ((z ^ x) & (z ^ 1)) >> 7;
  return BumpFlags(z, af, f, of, sf);
}

int64_t Dec8(uint64_t x64, uint64_t y, uint32_t *f) {
  uint8_t x, z;
  uint32_t of, sf, af;
  x = x64;
  z = x - 1;
  sf = z >> 7;
  af = (x & 15) < (z & 15);
  of = ((z ^ x) & (x ^ 1)) >> 7;
  return BumpFlags(z, af, f, of, sf);
}

int64_t Shr8(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x, cf;
  x = x64 & 0xff;
  if ((y &= 31)) {
    cf = (x >> (y - 1)) & 1;
    x >>= y;
    return AluFlags8(x, 0, f, ((x << 1) ^ x) >> 7, cf);
  } else {
    return x;
  }
}

int64_t Shr32(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t cf, x = x64;
  if ((y &= 31)) {
    cf = (x >> (y - 1)) & 1;
    x >>= y;
    return AluFlags32(x, 0, f, ((x << 1) ^ x) >> 31, cf);
  } else {
    return x;
  }
}

int64_t Shr64(uint64_t x, uint64_t y, uint32_t *f) {
  uint32_t cf;
  if ((y &= 63)) {
    cf = (x >> (y - 1)) & 1;
    x >>= y;
    return AluFlags64(x, 0, f, ((x << 1) ^ x) >> 63, cf);
  } else {
    return x;
  }
}

int64_t Shl8(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x, cf;
  x = x64 & 0xff;
  if ((y &= 31)) {
    cf = (x >> ((8 - y) & 31)) & 1;
    x = (x << y) & 0xff;
    return AluFlags8(x, 0, f, (x >> 7) ^ cf, cf);
  } else {
    return x;
  }
}

int64_t Shl32(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t cf, x = x64;
  if ((y &= 31)) {
    cf = (x >> (32 - y)) & 1;
    x <<= y;
    return AluFlags32(x, 0, f, (x >> 31) ^ cf, cf);
  } else {
    return x;
  }
}

int64_t Shl64(uint64_t x, uint64_t y, uint32_t *f) {
  uint32_t cf;
  if ((y &= 63)) {
    cf = (x >> (64 - y)) & 1;
    x <<= y;
    return AluFlags64(x, 0, f, (x >> 63) ^ cf, cf);
  } else {
    return x;
  }
}

int64_t Sar8(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x, cf;
  x = x64 & 0xff;
  if ((y &= 31)) {
    cf = ((int32_t)(int8_t)x >> (y - 1)) & 1;
    x = ((int32_t)(int8_t)x >> y) & 0xff;
    return AluFlags8(x, 0, f, 0, cf);
  } else {
    return x;
  }
}

int64_t Sar32(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t cf, x = x64;
  if ((y &= 31)) {
    cf = ((int32_t)x >> (y - 1)) & 1;
    x = (int32_t)x >> y;
    return AluFlags32(x, 0, f, 0, cf);
  } else {
    return x;
  }
}

int64_t Sar64(uint64_t x, uint64_t y, uint32_t *f) {
  uint32_t cf;
  if ((y &= 63)) {
    cf = ((int64_t)x >> (y - 1)) & 1;
    x = (int64_t)x >> y;
    return AluFlags64(x, 0, f, 0, cf);
  } else {
    return x;
  }
}

static int64_t RotateFlags(uint64_t x, uint32_t cf, uint32_t *f, uint32_t of) {
  *f &= ~(1u << FLAGS_CF | 1u << FLAGS_OF);
  *f |= cf << FLAGS_CF | of << FLAGS_OF;
  return x;
}

int64_t Rol32(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x = x64;
  if ((y &= 31)) {
    x = x << y | x >> (32 - y);
    return RotateFlags(x, x & 1, f, ((x >> 31) ^ x) & 1);
  } else {
    return x;
  }
}

int64_t Rol64(uint64_t x, uint64_t y, uint32_t *f) {
  if ((y &= 63)) {
    x = x << y | x >> (64 - y);
    return RotateFlags(x, x & 1, f, ((x >> 63) ^ x) & 1);
  } else {
    return x;
  }
}

int64_t Ror32(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x = x64;
  if ((y &= 31)) {
    x = x >> y | x << (32 - y);
    return RotateFlags(x, x >> 31, f, (x >> 31) ^ (x >> 30) & 1);
  } else {
    return x;
  }
}

int64_t Ror64(uint64_t x, uint64_t y, uint32_t *f) {
  if ((y &= 63)) {
    x = x >> y | x << (64 - y);
    return RotateFlags(x, x >> 63, f, (x >> 63) ^ (x >> 62) & 1);
  } else {
    return x;
  }
}

int64_t Rol8(uint64_t x64, uint64_t y, uint32_t *f) {
  uint8_t x = x64;
  if (y & 31) {
    if ((y &= 7)) x = x << y | x >> (8 - y);
    return RotateFlags(x, x & 1, f, ((x >> 7) ^ x) & 1);
  } else {
    return x;
  }
}

int64_t Ror8(uint64_t x64, uint64_t y, uint32_t *f) {
  uint8_t x = x64;
  if (y & 31) {
    if ((y &= 7)) x = x >> y | x << (8 - y);
    return RotateFlags(x, x >> 7, f, (x >> 7) ^ (x >> 6) & 1);
  } else {
    return x;
  }
}

static int64_t Rcr(uint64_t x, uint64_t y, uint32_t *f, uint64_t xm,
                   uint64_t k) {
  uint64_t cf;
  uint32_t ct;
  x &= xm;
  if (y) {
    cf = GetFlag(*f, FLAGS_CF);
    ct = (x >> (y - 1)) & 1;
    if (y == 1) {
      x = (x >> 1 | cf << (k - 1)) & xm;
    } else {
      x = (x >> y | cf << (k - y) | x << (k + 1 - y)) & xm;
    }
    return RotateFlags(x, ct, f, (((x << 1) ^ x) >> (k - 1)) & 1);
  } else {
    return x;
  }
}

int64_t Rcr8(uint64_t x, uint64_t y, uint32_t *f) {
  return Rcr(x, (y & 31) % 9, f, 0xff, 8);
}

int64_t Rcr16(uint64_t x, uint64_t y, uint32_t *f) {
  return Rcr(x, (y & 31) % 17, f, 0xffff, 16);
}

int64_t Rcr32(uint64_t x, uint64_t y, uint32_t *f) {
  return Rcr(x, y & 31, f, 0xffffffff, 32);
}

int64_t Rcr64(uint64_t x, uint64_t y, uint32_t *f) {
  return Rcr(x, y & 63, f, 0xffffffffffffffff, 64);
}

static int64_t Rcl(uint64_t x, uint64_t y, uint32_t *f, uint64_t xm,
                   uint64_t k) {
  uint64_t cf;
  uint32_t ct;
  x &= xm;
  if (y) {
    cf = GetFlag(*f, FLAGS_CF);
    ct = (x >> (k - y)) & 1;
    if (y == 1) {
      x = (x << 1 | cf) & xm;
    } else {
      x = (x << y | cf << (y - 1) | x >> (k + 1 - y)) & xm;
    }
    return RotateFlags(x, ct, f, ct ^ ((x >> (k - 1)) & 1));
  } else {
    return x;
  }
}

int64_t Rcl8(uint64_t x, uint64_t y, uint32_t *f) {
  return Rcl(x, (y & 31) % 9, f, 0xff, 8);
}

int64_t Rcl16(uint64_t x, uint64_t y, uint32_t *f) {
  return Rcl(x, (y & 31) % 17, f, 0xffff, 16);
}

int64_t Rcl32(uint64_t x, uint64_t y, uint32_t *f) {
  return Rcl(x, y & 31, f, 0xffffffff, 32);
}

int64_t Rcl64(uint64_t x, uint64_t y, uint32_t *f) {
  return Rcl(x, y & 63, f, 0xffffffffffffffff, 64);
}

uint64_t BsuDoubleShift(int w, uint64_t x, uint64_t y, uint8_t b, bool isright,
                        uint32_t *f) {
  bool cf, of;
  uint64_t s, k, m, z;
  k = 8;
  k <<= w;
  s = 1;
  s <<= k - 1;
  m = s | s - 1;
  b &= w == 3 ? 63 : 31;
  x &= m;
  if (b) {
    if (isright) {
      z = x >> b | y << (k - b);
      cf = (x >> (b - 1)) & 1;
      of = b == 1 && (z & s) != (x & s);
    } else {
      z = x << b | y >> (k - b);
      cf = (x >> (k - b)) & 1;
      of = b == 1 && (z & s) != (x & s);
    }
    x = z;
    x &= m;
    return AluFlags(x, 0, f, of, cf, !!(x & s));
  } else {
    return x;
  }
}

int64_t AluFlags16(uint16_t z, uint32_t af, uint32_t *f, uint32_t of,
                   uint32_t cf) {
  return AluFlags(z, af, f, of, cf, z >> 15);
}

int64_t Xor16(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags16(x ^ y, 0, f, 0, 0);
}

int64_t Or16(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags16(x | y, 0, f, 0, 0);
}

int64_t And16(uint64_t x, uint64_t y, uint32_t *f) {
  return AluFlags16(x & y, 0, f, 0, 0);
}

int64_t Sub16(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint16_t x, y, z;
  x = x64;
  y = y64;
  z = x - y;
  cf = x < z;
  af = (x & 15) < (z & 15);
  of = ((z ^ x) & (x ^ y)) >> 15;
  return AluFlags16(z, af, f, of, cf);
}

int64_t Add16(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint16_t x, y, z;
  x = x64;
  y = y64;
  z = x + y;
  cf = z < y;
  af = (z & 15) < (y & 15);
  of = ((z ^ x) & (z ^ y)) >> 15;
  return AluFlags16(z, af, f, of, cf);
}

int64_t Adc16(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint16_t x, y, z, t;
  x = x64;
  y = y64;
  t = x + GetFlag(*f, FLAGS_CF);
  z = t + y;
  cf = (t < x) | (z < y);
  of = ((z ^ x) & (z ^ y)) >> 15;
  af = ((t & 15) < (x & 15)) | ((z & 15) < (y & 15));
  return AluFlags16(z, af, f, of, cf);
}

int64_t Sbb16(uint64_t x64, uint64_t y64, uint32_t *f) {
  bool cf, of, af;
  uint16_t x, y, z, t;
  x = x64;
  y = y64;
  t = x - GetFlag(*f, FLAGS_CF);
  z = t - y;
  cf = (x < t) | (t < z);
  of = ((z ^ x) & (x ^ y)) >> 15;
  af = ((x & 15) < (t & 15)) | ((t & 15) < (z & 15));
  return AluFlags16(z, af, f, of, cf);
}

int64_t Not16(uint64_t x, uint64_t y, uint32_t *f) {
  return ~x & 0xFFFF;
}

int64_t Neg16(uint64_t x64, uint64_t y, uint32_t *f) {
  uint16_t x;
  bool cf, of, af;
  x = x64;
  af = cf = !!x;
  of = x == 0x8000;
  x = ~x + 1;
  return AluFlags16(x, af, f, of, cf);
}

int64_t Inc16(uint64_t x64, uint64_t y, uint32_t *f) {
  uint16_t x, z;
  uint32_t of, sf, af;
  x = x64;
  z = x + 1;
  sf = z >> 15;
  af = (z & 15) < (y & 15);
  of = ((z ^ x) & (z ^ 1)) >> 15;
  return BumpFlags(z, af, f, of, sf);
}

int64_t Dec16(uint64_t x64, uint64_t y, uint32_t *f) {
  uint16_t x, z;
  uint32_t of, sf, af;
  x = x64;
  z = x - 1;
  sf = z >> 15;
  af = (x & 15) < (z & 15);
  of = ((z ^ x) & (x ^ 1)) >> 15;
  return BumpFlags(z, af, f, of, sf);
}

int64_t Shr16(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x, cf;
  x = x64 & 0xffff;
  if ((y &= 31)) {
    cf = (x >> (y - 1)) & 1;
    x >>= y;
    return AluFlags16(x, 0, f, ((x << 1) ^ x) >> 15, cf);
  } else {
    return x;
  }
}

int64_t Shl16(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x, cf;
  x = x64 & 0xffff;
  if ((y &= 31)) {
    cf = (x >> ((16 - y) & 31)) & 1;
    x = (x << y) & 0xffff;
    return AluFlags16(x, 0, f, (x >> 15) ^ cf, cf);
  } else {
    return x;
  }
}

int64_t Sar16(uint64_t x64, uint64_t y, uint32_t *f) {
  uint32_t x, cf;
  x = x64 & 0xffff;
  if ((y &= 31)) {
    cf = ((int32_t)(int16_t)x >> (y - 1)) & 1;
    x = ((int32_t)(int16_t)x >> y) & 0xffff;
    return AluFlags16(x, 0, f, 0, cf);
  } else {
    return x;
  }
}

int64_t Rol16(uint64_t x64, uint64_t y, uint32_t *f) {
  uint16_t x = x64;
  if (y & 31) {
    if ((y &= 15)) x = x << y | x >> (16 - y);
    return RotateFlags(x, x & 1, f, ((x >> 15) ^ x) & 1);
  } else {
    return x;
  }
}

int64_t Ror16(uint64_t x64, uint64_t y, uint32_t *f) {
  uint16_t x = x64;
  if (y & 31) {
    if ((y &= 15)) x = x >> y | x << (16 - y);
    return RotateFlags(x, x >> 15, f, (x >> 15) ^ (x >> 14) & 1);
  } else {
    return x;
  }
}
