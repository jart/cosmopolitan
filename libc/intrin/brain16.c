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
 * @fileoverview bf16 compiler runtime
 */

_Float32 __extendbfsf2(__bf16 f) {
  union {
    __bf16 f;
    uint16_t i;
  } ub = {f};

  // convert brain16 to binary32
  uint32_t x = (uint32_t)ub.i << 16;

  // force nan to quiet
  if ((x & 0x7fffffff) > 0x7f800000)
    x |= 0x00400000;

  // pun to _Float32
  union {
    uint32_t i;
    _Float32 f;
  } uf = {x};
  return uf.f;
}

_Float64 __extendbfdf2(__bf16 f) {
  return __extendbfsf2(f);
}

#ifdef __x86_64__
__float80 __extendbfxf2(__bf16 f) {
  return __extendbfsf2(f);
}
#endif

#ifdef __aarch64__
_Float128 __extendbftf2(__bf16 f) {
  return __extendbfsf2(f);
}
#endif

__bf16 __truncsfbf2(_Float32 f) {
  union {
    _Float32 f;
    uint32_t i;
  } uf = {f};
  uint32_t x = uf.i;

  if ((x & 0x7fffffff) > 0x7f800000)
    // force nan to quiet
    x = (x | 0x00400000) >> 16;
  else
    // convert binary32 to brain16 with nearest rounding
    x = (x + (0x7fff + ((x >> 16) & 1))) >> 16;

  // pun to bf16
  union {
    uint16_t i;
    __bf16 f;
  } ub = {x};
  return ub.f;
}

__bf16 __truncdfbf2(_Float64 f) {
  return __truncsfbf2(f);
}

#ifdef __x86_64__
__bf16 __truncxfbf2(__float80 f) {
  return __truncsfbf2(f);
}
#endif

#ifdef __aarch64__
__bf16 __trunctfbf2(_Float128 f) {
  return __truncsfbf2(f);
}
#endif
