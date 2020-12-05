/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2013 Intel Corporation                                             │
│ Use of this source code is governed by the BSD-style licenses that can       │
│ be found in the third_party/zlib/LICENSE file.                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/emmintrin.internal.h"
#include "libc/bits/smmintrin.internal.h"
#include "libc/bits/tmmintrin.internal.h"
#include "libc/bits/wmmintrin.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "third_party/zlib/deflate.h"
#include "third_party/zlib/internal.h"

asm(".ident\t\"\\n\\n\
zlib » crc32 parallelized folding (zlib License)\\n\
Copyright 2013 Intel Corporation\\n\
Authors: Wajdi Feghali,Jim Guilford,Vinodh Gopal,Erdinc Ozturk,Jim Kukunas\"");
asm(".include \"libc/disclaimer.inc\"");

/**
 * CRC32 parallelized folding w/ PCLMULQDQ instruction.
 *
 * Authored by:
 * 	Wajdi Feghali   <wajdi.k.feghali@intel.com>
 * 	Jim Guilford    <james.guilford@intel.com>
 * 	Vinodh Gopal    <vinodh.gopal@intel.com>
 * 	Erdinc Ozturk   <erdinc.ozturk@intel.com>
 * 	Jim Kukunas     <james.t.kukunas@linux.intel.com>
 *
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#define CRC_LOAD(s)                                           \
  __m128i xmm_crc0 = _mm_loadu_si128((__m128i *)s->crc0 + 0); \
  __m128i xmm_crc1 = _mm_loadu_si128((__m128i *)s->crc0 + 1); \
  __m128i xmm_crc2 = _mm_loadu_si128((__m128i *)s->crc0 + 2); \
  __m128i xmm_crc3 = _mm_loadu_si128((__m128i *)s->crc0 + 3); \
  __m128i xmm_crc_part = _mm_loadu_si128((__m128i *)s->crc0 + 4)

#define CRC_SAVE(s)                                   \
  _mm_storeu_si128((__m128i *)s->crc0 + 0, xmm_crc0); \
  _mm_storeu_si128((__m128i *)s->crc0 + 1, xmm_crc1); \
  _mm_storeu_si128((__m128i *)s->crc0 + 2, xmm_crc2); \
  _mm_storeu_si128((__m128i *)s->crc0 + 3, xmm_crc3); \
  _mm_storeu_si128((__m128i *)s->crc0 + 4, xmm_crc_part)

void crc_fold_init(struct DeflateState *const s) {
  CRC_LOAD(s);
  xmm_crc0 = _mm_cvtsi32_si128(0x9db42487);
  xmm_crc1 = _mm_setzero_si128();
  xmm_crc2 = _mm_setzero_si128();
  xmm_crc3 = _mm_setzero_si128();
  CRC_SAVE(s);
  s->strm->adler = 0;
}

static inline void fold_1(struct DeflateState *const s, __m128i *xmm_crc0,
                          __m128i *xmm_crc1, __m128i *xmm_crc2,
                          __m128i *xmm_crc3) {
  const __m128i xmm_fold4 =
      _mm_set_epi32(0x00000001, 0x54442bd4, 0x00000001, 0xc6e41596);

  __m128i x_tmp3;
  __m128 ps_crc0, ps_crc3, ps_res;

  x_tmp3 = *xmm_crc3;

  *xmm_crc3 = *xmm_crc0;
  *xmm_crc0 = _mm_clmulepi64_si128(*xmm_crc0, xmm_fold4, 0x01);
  *xmm_crc3 = _mm_clmulepi64_si128(*xmm_crc3, xmm_fold4, 0x10);
  ps_crc0 = _mm_castsi128_ps(*xmm_crc0);
  ps_crc3 = _mm_castsi128_ps(*xmm_crc3);
  ps_res = _mm_xor_ps(ps_crc0, ps_crc3);

  *xmm_crc0 = *xmm_crc1;
  *xmm_crc1 = *xmm_crc2;
  *xmm_crc2 = x_tmp3;
  *xmm_crc3 = _mm_castps_si128(ps_res);
}

static inline void fold_2(struct DeflateState *const s, __m128i *xmm_crc0,
                          __m128i *xmm_crc1, __m128i *xmm_crc2,
                          __m128i *xmm_crc3) {
  const __m128i xmm_fold4 =
      _mm_set_epi32(0x00000001, 0x54442bd4, 0x00000001, 0xc6e41596);

  __m128i x_tmp3, x_tmp2;
  __m128 ps_crc0, ps_crc1, ps_crc2, ps_crc3, ps_res31, ps_res20;

  x_tmp3 = *xmm_crc3;
  x_tmp2 = *xmm_crc2;

  *xmm_crc3 = *xmm_crc1;
  *xmm_crc1 = _mm_clmulepi64_si128(*xmm_crc1, xmm_fold4, 0x01);
  *xmm_crc3 = _mm_clmulepi64_si128(*xmm_crc3, xmm_fold4, 0x10);
  ps_crc3 = _mm_castsi128_ps(*xmm_crc3);
  ps_crc1 = _mm_castsi128_ps(*xmm_crc1);
  ps_res31 = _mm_xor_ps(ps_crc3, ps_crc1);

  *xmm_crc2 = *xmm_crc0;
  *xmm_crc0 = _mm_clmulepi64_si128(*xmm_crc0, xmm_fold4, 0x01);
  *xmm_crc2 = _mm_clmulepi64_si128(*xmm_crc2, xmm_fold4, 0x10);
  ps_crc0 = _mm_castsi128_ps(*xmm_crc0);
  ps_crc2 = _mm_castsi128_ps(*xmm_crc2);
  ps_res20 = _mm_xor_ps(ps_crc0, ps_crc2);

  *xmm_crc0 = x_tmp2;
  *xmm_crc1 = x_tmp3;
  *xmm_crc2 = _mm_castps_si128(ps_res20);
  *xmm_crc3 = _mm_castps_si128(ps_res31);
}

static inline void fold_3(struct DeflateState *const s, __m128i *xmm_crc0,
                          __m128i *xmm_crc1, __m128i *xmm_crc2,
                          __m128i *xmm_crc3) {
  const __m128i xmm_fold4 =
      _mm_set_epi32(0x00000001, 0x54442bd4, 0x00000001, 0xc6e41596);

  __m128i x_tmp3;
  __m128 ps_crc0, ps_crc1, ps_crc2, ps_crc3, ps_res32, ps_res21, ps_res10;

  x_tmp3 = *xmm_crc3;

  *xmm_crc3 = *xmm_crc2;
  *xmm_crc2 = _mm_clmulepi64_si128(*xmm_crc2, xmm_fold4, 0x01);
  *xmm_crc3 = _mm_clmulepi64_si128(*xmm_crc3, xmm_fold4, 0x10);
  ps_crc2 = _mm_castsi128_ps(*xmm_crc2);
  ps_crc3 = _mm_castsi128_ps(*xmm_crc3);
  ps_res32 = _mm_xor_ps(ps_crc2, ps_crc3);

  *xmm_crc2 = *xmm_crc1;
  *xmm_crc1 = _mm_clmulepi64_si128(*xmm_crc1, xmm_fold4, 0x01);
  *xmm_crc2 = _mm_clmulepi64_si128(*xmm_crc2, xmm_fold4, 0x10);
  ps_crc1 = _mm_castsi128_ps(*xmm_crc1);
  ps_crc2 = _mm_castsi128_ps(*xmm_crc2);
  ps_res21 = _mm_xor_ps(ps_crc1, ps_crc2);

  *xmm_crc1 = *xmm_crc0;
  *xmm_crc0 = _mm_clmulepi64_si128(*xmm_crc0, xmm_fold4, 0x01);
  *xmm_crc1 = _mm_clmulepi64_si128(*xmm_crc1, xmm_fold4, 0x10);
  ps_crc0 = _mm_castsi128_ps(*xmm_crc0);
  ps_crc1 = _mm_castsi128_ps(*xmm_crc1);
  ps_res10 = _mm_xor_ps(ps_crc0, ps_crc1);

  *xmm_crc0 = x_tmp3;
  *xmm_crc1 = _mm_castps_si128(ps_res10);
  *xmm_crc2 = _mm_castps_si128(ps_res21);
  *xmm_crc3 = _mm_castps_si128(ps_res32);
}

static inline void fold_4(struct DeflateState *const s, __m128i *xmm_crc0,
                          __m128i *xmm_crc1, __m128i *xmm_crc2,
                          __m128i *xmm_crc3) {
  const __m128i xmm_fold4 =
      _mm_set_epi32(0x00000001, 0x54442bd4, 0x00000001, 0xc6e41596);

  __m128i x_tmp0, x_tmp1, x_tmp2, x_tmp3;
  __m128 ps_crc0, ps_crc1, ps_crc2, ps_crc3;
  __m128 ps_t0, ps_t1, ps_t2, ps_t3;
  __m128 ps_res0, ps_res1, ps_res2, ps_res3;

  x_tmp0 = *xmm_crc0;
  x_tmp1 = *xmm_crc1;
  x_tmp2 = *xmm_crc2;
  x_tmp3 = *xmm_crc3;

  *xmm_crc0 = _mm_clmulepi64_si128(*xmm_crc0, xmm_fold4, 0x01);
  x_tmp0 = _mm_clmulepi64_si128(x_tmp0, xmm_fold4, 0x10);
  ps_crc0 = _mm_castsi128_ps(*xmm_crc0);
  ps_t0 = _mm_castsi128_ps(x_tmp0);
  ps_res0 = _mm_xor_ps(ps_crc0, ps_t0);

  *xmm_crc1 = _mm_clmulepi64_si128(*xmm_crc1, xmm_fold4, 0x01);
  x_tmp1 = _mm_clmulepi64_si128(x_tmp1, xmm_fold4, 0x10);
  ps_crc1 = _mm_castsi128_ps(*xmm_crc1);
  ps_t1 = _mm_castsi128_ps(x_tmp1);
  ps_res1 = _mm_xor_ps(ps_crc1, ps_t1);

  *xmm_crc2 = _mm_clmulepi64_si128(*xmm_crc2, xmm_fold4, 0x01);
  x_tmp2 = _mm_clmulepi64_si128(x_tmp2, xmm_fold4, 0x10);
  ps_crc2 = _mm_castsi128_ps(*xmm_crc2);
  ps_t2 = _mm_castsi128_ps(x_tmp2);
  ps_res2 = _mm_xor_ps(ps_crc2, ps_t2);

  *xmm_crc3 = _mm_clmulepi64_si128(*xmm_crc3, xmm_fold4, 0x01);
  x_tmp3 = _mm_clmulepi64_si128(x_tmp3, xmm_fold4, 0x10);
  ps_crc3 = _mm_castsi128_ps(*xmm_crc3);
  ps_t3 = _mm_castsi128_ps(x_tmp3);
  ps_res3 = _mm_xor_ps(ps_crc3, ps_t3);

  *xmm_crc0 = _mm_castps_si128(ps_res0);
  *xmm_crc1 = _mm_castps_si128(ps_res1);
  *xmm_crc2 = _mm_castps_si128(ps_res2);
  *xmm_crc3 = _mm_castps_si128(ps_res3);
}

static const unsigned forcealign(32) pshufb_shf_table[60] = {
    0x84838281, 0x88878685, 0x8c8b8a89, 0x008f8e8d, /* shl 15 (16 - 1)/shr1 */
    0x85848382, 0x89888786, 0x8d8c8b8a, 0x01008f8e, /* shl 14 (16 - 3)/shr2 */
    0x86858483, 0x8a898887, 0x8e8d8c8b, 0x0201008f, /* shl 13 (16 - 4)/shr3 */
    0x87868584, 0x8b8a8988, 0x8f8e8d8c, 0x03020100, /* shl 12 (16 - 4)/shr4 */
    0x88878685, 0x8c8b8a89, 0x008f8e8d, 0x04030201, /* shl 11 (16 - 5)/shr5 */
    0x89888786, 0x8d8c8b8a, 0x01008f8e, 0x05040302, /* shl 10 (16 - 6)/shr6 */
    0x8a898887, 0x8e8d8c8b, 0x0201008f, 0x06050403, /* shl  9 (16 - 7)/shr7 */
    0x8b8a8988, 0x8f8e8d8c, 0x03020100, 0x07060504, /* shl  8 (16 - 8)/shr8 */
    0x8c8b8a89, 0x008f8e8d, 0x04030201, 0x08070605, /* shl  7 (16 - 9)/shr9 */
    0x8d8c8b8a, 0x01008f8e, 0x05040302, 0x09080706, /* shl  6 (16 -10)/shr10*/
    0x8e8d8c8b, 0x0201008f, 0x06050403, 0x0a090807, /* shl  5 (16 -11)/shr11*/
    0x8f8e8d8c, 0x03020100, 0x07060504, 0x0b0a0908, /* shl  4 (16 -12)/shr12*/
    0x008f8e8d, 0x04030201, 0x08070605, 0x0c0b0a09, /* shl  3 (16 -13)/shr13*/
    0x01008f8e, 0x05040302, 0x09080706, 0x0d0c0b0a, /* shl  2 (16 -14)/shr14*/
    0x0201008f, 0x06050403, 0x0a090807, 0x0e0d0c0b  /* shl  1 (16 -15)/shr15*/
};

static void partial_fold(struct DeflateState *const s, const size_t len,
                         __m128i *xmm_crc0, __m128i *xmm_crc1,
                         __m128i *xmm_crc2, __m128i *xmm_crc3,
                         __m128i *xmm_crc_part) {
  const __m128i xmm_fold4 =
      _mm_set_epi32(0x00000001, 0x54442bd4, 0x00000001, 0xc6e41596);
  const __m128i xmm_mask3 = _mm_set1_epi32(0x80808080);

  __m128i xmm_shl, xmm_shr, xmm_tmp1, xmm_tmp2, xmm_tmp3;
  __m128i xmm_a0_0, xmm_a0_1;
  __m128 ps_crc3, psa0_0, psa0_1, ps_res;

  {
    __m128i *wut = (__m128i *)pshufb_shf_table + (len - 1);
    intptr_t huh = (intptr_t)wut;
    intptr_t tab = (intptr_t)pshufb_shf_table;
    if (huh < tab || huh + sizeof(*wut) >= tab + sizeof(pshufb_shf_table)) {
      abort();
    }
    xmm_shl = _mm_load_si128(wut);
  }

  xmm_shr = xmm_shl;
  xmm_shr = _mm_xor_si128(xmm_shr, xmm_mask3);

  xmm_a0_0 = _mm_shuffle_epi8(*xmm_crc0, xmm_shl);

  *xmm_crc0 = _mm_shuffle_epi8(*xmm_crc0, xmm_shr);
  xmm_tmp1 = _mm_shuffle_epi8(*xmm_crc1, xmm_shl);
  *xmm_crc0 = _mm_or_si128(*xmm_crc0, xmm_tmp1);

  *xmm_crc1 = _mm_shuffle_epi8(*xmm_crc1, xmm_shr);
  xmm_tmp2 = _mm_shuffle_epi8(*xmm_crc2, xmm_shl);
  *xmm_crc1 = _mm_or_si128(*xmm_crc1, xmm_tmp2);

  *xmm_crc2 = _mm_shuffle_epi8(*xmm_crc2, xmm_shr);
  xmm_tmp3 = _mm_shuffle_epi8(*xmm_crc3, xmm_shl);
  *xmm_crc2 = _mm_or_si128(*xmm_crc2, xmm_tmp3);

  *xmm_crc3 = _mm_shuffle_epi8(*xmm_crc3, xmm_shr);
  *xmm_crc_part = _mm_shuffle_epi8(*xmm_crc_part, xmm_shl);
  *xmm_crc3 = _mm_or_si128(*xmm_crc3, *xmm_crc_part);

  xmm_a0_1 = _mm_clmulepi64_si128(xmm_a0_0, xmm_fold4, 0x10);
  xmm_a0_0 = _mm_clmulepi64_si128(xmm_a0_0, xmm_fold4, 0x01);

  ps_crc3 = _mm_castsi128_ps(*xmm_crc3);
  psa0_0 = _mm_castsi128_ps(xmm_a0_0);
  psa0_1 = _mm_castsi128_ps(xmm_a0_1);

  ps_res = _mm_xor_ps(ps_crc3, psa0_0);
  ps_res = _mm_xor_ps(ps_res, psa0_1);

  *xmm_crc3 = _mm_castps_si128(ps_res);
}

void crc_fold_copy(struct DeflateState *const s, unsigned char *dst,
                   const unsigned char *src, long len) {
  unsigned long algn_diff;
  __m128i xmm_t0, xmm_t1, xmm_t2, xmm_t3;

  CRC_LOAD(s);

  if (len < 16) {
    if (len == 0) return;
    goto partial;
  }

  algn_diff = 0 - ((uintptr_t)src & 0xF);
  if (algn_diff) {
    xmm_crc_part = _mm_loadu_si128((__m128i *)src);
    _mm_storeu_si128((__m128i *)dst, xmm_crc_part);

    dst += algn_diff;
    src += algn_diff;
    len -= algn_diff;

    partial_fold(s, algn_diff, &xmm_crc0, &xmm_crc1, &xmm_crc2, &xmm_crc3,
                 &xmm_crc_part);
  }

  while ((len -= 64) >= 0) {
    xmm_t0 = _mm_load_si128((__m128i *)src);
    xmm_t1 = _mm_load_si128((__m128i *)src + 1);
    xmm_t2 = _mm_load_si128((__m128i *)src + 2);
    xmm_t3 = _mm_load_si128((__m128i *)src + 3);

    fold_4(s, &xmm_crc0, &xmm_crc1, &xmm_crc2, &xmm_crc3);

    _mm_storeu_si128((__m128i *)dst, xmm_t0);
    _mm_storeu_si128((__m128i *)dst + 1, xmm_t1);
    _mm_storeu_si128((__m128i *)dst + 2, xmm_t2);
    _mm_storeu_si128((__m128i *)dst + 3, xmm_t3);

    xmm_crc0 = _mm_xor_si128(xmm_crc0, xmm_t0);
    xmm_crc1 = _mm_xor_si128(xmm_crc1, xmm_t1);
    xmm_crc2 = _mm_xor_si128(xmm_crc2, xmm_t2);
    xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_t3);

    src += 64;
    dst += 64;
  }

  /*
   * len = num bytes left - 64
   */
  if (len + 16 >= 0) {
    len += 16;

    xmm_t0 = _mm_load_si128((__m128i *)src);
    xmm_t1 = _mm_load_si128((__m128i *)src + 1);
    xmm_t2 = _mm_load_si128((__m128i *)src + 2);

    fold_3(s, &xmm_crc0, &xmm_crc1, &xmm_crc2, &xmm_crc3);

    _mm_storeu_si128((__m128i *)dst, xmm_t0);
    _mm_storeu_si128((__m128i *)dst + 1, xmm_t1);
    _mm_storeu_si128((__m128i *)dst + 2, xmm_t2);

    xmm_crc1 = _mm_xor_si128(xmm_crc1, xmm_t0);
    xmm_crc2 = _mm_xor_si128(xmm_crc2, xmm_t1);
    xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_t2);

    if (len == 0) goto done;

    dst += 48;
    src += 48;
  } else if (len + 32 >= 0) {
    len += 32;

    xmm_t0 = _mm_load_si128((__m128i *)src);
    xmm_t1 = _mm_load_si128((__m128i *)src + 1);

    fold_2(s, &xmm_crc0, &xmm_crc1, &xmm_crc2, &xmm_crc3);

    _mm_storeu_si128((__m128i *)dst, xmm_t0);
    _mm_storeu_si128((__m128i *)dst + 1, xmm_t1);

    xmm_crc2 = _mm_xor_si128(xmm_crc2, xmm_t0);
    xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_t1);

    if (len == 0) goto done;

    dst += 32;
    src += 32;
  } else if (len + 48 >= 0) {
    len += 48;

    xmm_t0 = _mm_load_si128((__m128i *)src);

    fold_1(s, &xmm_crc0, &xmm_crc1, &xmm_crc2, &xmm_crc3);

    _mm_storeu_si128((__m128i *)dst, xmm_t0);

    xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_t0);

    if (len == 0) goto done;

    dst += 16;
    src += 16;
  } else {
    len += 64;
    if (len == 0) goto done;
  }

partial:

#if defined(_MSC_VER)
  /* VS does not permit the use of _mm_set_epi64x in 32-bit builds */
  {
    int32_t parts[4] = {0, 0, 0, 0};
    memcpy(&parts, src, len);
    xmm_crc_part = _mm_set_epi32(parts[3], parts[2], parts[1], parts[0]);
  }
#else
{
  int64_t parts[2] = {0, 0};
  memcpy(&parts, src, len);
  xmm_crc_part = _mm_set_epi64x(parts[1], parts[0]);
}
#endif

  _mm_storeu_si128((__m128i *)dst, xmm_crc_part);
  partial_fold(s, len, &xmm_crc0, &xmm_crc1, &xmm_crc2, &xmm_crc3,
               &xmm_crc_part);
done:
  CRC_SAVE(s);
}

static const __m128i kCrcFold512[] = {
    {0xffffffffffffffffull, 0x0000000000000000ull},
    {0xffffffff00000000ull, 0xffffffffffffffffull},
    {0x00000000ccaa009eull, 0x00000001751997d0ull}, /* 2: k1 */
    {0x00000000ccaa009eull, 0x0000000163cd6124ull}, /* 3: k5 */
    {0x00000001f7011640ull, 0x00000001db710640ull}  /* 4: k7 */
};

unsigned crc_fold_512to32(struct DeflateState *const s) {
  const __m128i xmm_mask = kCrcFold512[0];
  const __m128i xmm_mask2 = kCrcFold512[1];

  unsigned crc;
  __m128i x_tmp0, x_tmp1, x_tmp2, crc_fold;

  CRC_LOAD(s);

  /*
   * k1
   */
  crc_fold = kCrcFold512[2];

  x_tmp0 = _mm_clmulepi64_si128(xmm_crc0, crc_fold, 0x10);
  xmm_crc0 = _mm_clmulepi64_si128(xmm_crc0, crc_fold, 0x01);
  xmm_crc1 = _mm_xor_si128(xmm_crc1, x_tmp0);
  xmm_crc1 = _mm_xor_si128(xmm_crc1, xmm_crc0);

  x_tmp1 = _mm_clmulepi64_si128(xmm_crc1, crc_fold, 0x10);
  xmm_crc1 = _mm_clmulepi64_si128(xmm_crc1, crc_fold, 0x01);
  xmm_crc2 = _mm_xor_si128(xmm_crc2, x_tmp1);
  xmm_crc2 = _mm_xor_si128(xmm_crc2, xmm_crc1);

  x_tmp2 = _mm_clmulepi64_si128(xmm_crc2, crc_fold, 0x10);
  xmm_crc2 = _mm_clmulepi64_si128(xmm_crc2, crc_fold, 0x01);
  xmm_crc3 = _mm_xor_si128(xmm_crc3, x_tmp2);
  xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_crc2);

  /*
   * k5
   */
  crc_fold = kCrcFold512[3];

  xmm_crc0 = xmm_crc3;
  xmm_crc3 = _mm_clmulepi64_si128(xmm_crc3, crc_fold, 0);
  xmm_crc0 = _mm_srli_si128(xmm_crc0, 8);
  xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_crc0);

  xmm_crc0 = xmm_crc3;
  xmm_crc3 = _mm_slli_si128(xmm_crc3, 4);
  xmm_crc3 = _mm_clmulepi64_si128(xmm_crc3, crc_fold, 0x10);
  xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_crc0);
  xmm_crc3 = _mm_and_si128(xmm_crc3, xmm_mask2);

  /*
   * k7
   */
  xmm_crc1 = xmm_crc3;
  xmm_crc2 = xmm_crc3;
  crc_fold = kCrcFold512[4];

  xmm_crc3 = _mm_clmulepi64_si128(xmm_crc3, crc_fold, 0);
  xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_crc2);
  xmm_crc3 = _mm_and_si128(xmm_crc3, xmm_mask);

  xmm_crc2 = xmm_crc3;
  xmm_crc3 = _mm_clmulepi64_si128(xmm_crc3, crc_fold, 0x10);
  xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_crc2);
  xmm_crc3 = _mm_xor_si128(xmm_crc3, xmm_crc1);

  crc = _mm_extract_epi32(xmm_crc3, 2);
  return ~crc;
  CRC_SAVE(s); /* TODO(jart): wut? */
}
