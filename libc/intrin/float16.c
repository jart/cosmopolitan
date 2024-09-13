/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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

/**
 * @fileoverview fp16 compiler runtime
 */

#define isnan16(x) (((x) & 0x7fff) > 0x7c00)

static inline _Float16 tofloat16(int x) {
  union {
    uint16_t i;
    _Float16 f;
  } u = {x};
  return u.f;
}

static inline int fromfloat16(_Float16 x) {
  union {
    _Float16 f;
    uint16_t i;
  } u = {x};
  return u.i;
}

static inline _Float32 tofloat32(uint32_t w) {
  union {
    uint32_t as_bits;
    _Float32 as_value;
  } fp32;
  fp32.as_bits = w;
  return fp32.as_value;
}

static inline uint32_t fromfloat32(_Float32 f) {
  union {
    _Float32 as_value;
    uint32_t as_bits;
  } fp32;
  fp32.as_value = f;
  return fp32.as_bits;
}

static inline _Float32 fabs32(_Float32 x) {
  return tofloat32(fromfloat32(x) & 0x7fffffffu);
}

int __eqhf2(_Float16 fx, _Float16 fy) {
  int x = fromfloat16(fx);
  int y = fromfloat16(fy);
  return (x == y) & !isnan16(x) & !isnan16(y);
}

int __nehf2(_Float16 fx, _Float16 fy) {
  int x = fromfloat16(fx);
  int y = fromfloat16(fy);
  return (x != y) & !isnan16(x) & !isnan16(y);
}

_Float32 __extendhfsf2(_Float16 f) {
  uint16_t h = fromfloat16(f);
  const uint32_t w = (uint32_t)h << 16;
  const uint32_t sign = w & 0x80000000u;
  const uint32_t two_w = w + w;
  const uint32_t exp_offset = 0xE0u << 23;
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) || \
    defined(__GNUC__) && !defined(__STRICT_ANSI__)
  const _Float32 exp_scale = 0x1.0p-112f;
#else
  const _Float32 exp_scale = tofloat32(0x7800000u);
#endif
  const _Float32 normalized_value =
      tofloat32((two_w >> 4) + exp_offset) * exp_scale;
  const uint32_t magic_mask = 126u << 23;
  const _Float32 magic_bias = 0.5f;
  const _Float32 denormalized_value =
      tofloat32((two_w >> 17) | magic_mask) - magic_bias;
  const uint32_t denormalized_cutoff = 1u << 27;
  const uint32_t result =
      sign | (two_w < denormalized_cutoff ? fromfloat32(denormalized_value)
                                          : fromfloat32(normalized_value));
  return tofloat32(result);
}

_Float64 __extendhfdf2(_Float16 f) {
  return __extendhfsf2(f);
}

#ifdef __x86_64__
__float80 __extendhfxf2(_Float16 f) {
  return __extendhfsf2(f);
}
#endif

#ifdef __aarch64__
_Float128 __extendhftf2(_Float16 f) {
  return __extendhfsf2(f);
}
#endif

_Float16 __truncsfhf2(_Float32 f) {
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) || \
    defined(__GNUC__) && !defined(__STRICT_ANSI__)
  const _Float32 scale_to_inf = 0x1.0p+112f;
  const _Float32 scale_to_zero = 0x1.0p-110f;
#else
  const _Float32 scale_to_inf = tofloat32(0x77800000u);
  const _Float32 scale_to_zero = tofloat32(0x08800000u);
#endif
  _Float32 base = (fabs32(f) * scale_to_inf) * scale_to_zero;
  const uint32_t w = fromfloat32(f);
  const uint32_t shl1_w = w + w;
  const uint32_t sign = w & 0x80000000u;
  uint32_t bias = shl1_w & 0xFF000000u;
  if (bias < 0x71000000u)
    bias = 0x71000000u;
  base = tofloat32((bias >> 1) + 0x07800000u) + base;
  const uint32_t bits = fromfloat32(base);
  const uint32_t exp_bits = (bits >> 13) & 0x00007C00u;
  const uint32_t mantissa_bits = bits & 0x00000FFFu;
  const uint32_t nonsign = exp_bits + mantissa_bits;
  return tofloat16((sign >> 16) | (shl1_w > 0xFF000000u ? 0x7E00u : nonsign));
}

_Float16 __truncdfhf2(_Float64 f) {
  return __truncsfhf2(f);
}

#ifdef __x86_64__
_Float16 __truncxfhf2(__float80 f) {
  return __truncsfhf2(f);
}
#endif

#ifdef __aarch64__
_Float16 __trunctfhf2(_Float128 f) {
  return __truncsfhf2(f);
}
#endif
