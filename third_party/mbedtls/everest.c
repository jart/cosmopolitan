#include "libc/bits/bits.h"
#include "libc/limits.h"
#include "third_party/mbedtls/asn1.h"
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/common.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/everest.h"
#include "third_party/mbedtls/platform.h"

asm(".ident\t\"\\n\\n\
Everest (Apache 2.0)\\n\
Copyright 2016-2018 INRIA and Microsoft Corporation\"");
asm(".include \"libc/disclaimer.inc\"");

/* clang-format off */
/*
 *  ECDH with curve-optimized implementation multiplexing
 *
 *  Copyright 2016-2018 INRIA and Microsoft Corporation
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if defined(MBEDTLS_ECDH_C) && defined(MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED)

#define load64_le(b) READ64LE(b)
#define store64_le(b, i) WRITE64LE(b, i)

#define KRML_HOST_EXIT exit
#define KRML_HOST_PRINTF printf

#define KRML_EXIT                                                              \
  do {                                                                         \
    KRML_HOST_PRINTF("Unimplemented function at %s:%d\n", __FILE__, __LINE__); \
    KRML_HOST_EXIT(254);                                                       \
  } while (0)

#define _KRML_CHECK_SIZE_PRAGMA \
    _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")

#define KRML_CHECK_SIZE(size_elt, sz)                                          \
  do {                                                                         \
    _KRML_CHECK_SIZE_PRAGMA                                                    \
    if (((size_t)(sz)) > ((size_t)(SIZE_MAX / (size_elt)))) {                  \
      KRML_HOST_PRINTF(                                                        \
          "Maximum allocatable size exceeded, aborting before overflow at "    \
          "%s:%d\n",                                                           \
          __FILE__, __LINE__);                                                 \
      KRML_HOST_EXIT(253);                                                     \
    }                                                                          \
  } while (0)

typedef const char *Prims_string;

typedef struct {
  uint32_t length;
  const char *data;
} FStar_Bytes_bytes;

typedef int32_t Prims_pos, Prims_nat, Prims_nonzero, Prims_int,
    krml_checked_int_t;

/* Prims_nat not yet in scope */
inline static int32_t krml_time() {
  return (int32_t)time(NULL);
}

static uint64_t FStar_UInt64_eq_mask(uint64_t a, uint64_t b)
{
  uint64_t x = a ^ b;
  uint64_t minus_x = ~x + (uint64_t)1U;
  uint64_t x_or_minus_x = x | minus_x;
  uint64_t xnx = x_or_minus_x >> (uint32_t)63U;
  return xnx - (uint64_t)1U;
}

static uint64_t FStar_UInt64_gte_mask(uint64_t a, uint64_t b)
{
  uint64_t x = a;
  uint64_t y = b;
  uint64_t x_xor_y = x ^ y;
  uint64_t x_sub_y = x - y;
  uint64_t x_sub_y_xor_y = x_sub_y ^ y;
  uint64_t q = x_xor_y | x_sub_y_xor_y;
  uint64_t x_xor_q = x ^ q;
  uint64_t x_xor_q_ = x_xor_q >> (uint32_t)63U;
  return x_xor_q_ - (uint64_t)1U;
}

static uint32_t FStar_UInt32_eq_mask(uint32_t a, uint32_t b)
{
  uint32_t x = a ^ b;
  uint32_t minus_x = ~x + (uint32_t)1U;
  uint32_t x_or_minus_x = x | minus_x;
  uint32_t xnx = x_or_minus_x >> (uint32_t)31U;
  return xnx - (uint32_t)1U;
}

static uint32_t FStar_UInt32_gte_mask(uint32_t a, uint32_t b)
{
  uint32_t x = a;
  uint32_t y = b;
  uint32_t x_xor_y = x ^ y;
  uint32_t x_sub_y = x - y;
  uint32_t x_sub_y_xor_y = x_sub_y ^ y;
  uint32_t q = x_xor_y | x_sub_y_xor_y;
  uint32_t x_xor_q = x ^ q;
  uint32_t x_xor_q_ = x_xor_q >> (uint32_t)31U;
  return x_xor_q_ - (uint32_t)1U;
}

static uint16_t FStar_UInt16_eq_mask(uint16_t a, uint16_t b)
{
  uint16_t x = a ^ b;
  uint16_t minus_x = ~x + (uint16_t)1U;
  uint16_t x_or_minus_x = x | minus_x;
  uint16_t xnx = x_or_minus_x >> (uint32_t)15U;
  return xnx - (uint16_t)1U;
}

static uint16_t FStar_UInt16_gte_mask(uint16_t a, uint16_t b)
{
  uint16_t x = a;
  uint16_t y = b;
  uint16_t x_xor_y = x ^ y;
  uint16_t x_sub_y = x - y;
  uint16_t x_sub_y_xor_y = x_sub_y ^ y;
  uint16_t q = x_xor_y | x_sub_y_xor_y;
  uint16_t x_xor_q = x ^ q;
  uint16_t x_xor_q_ = x_xor_q >> (uint32_t)15U;
  return x_xor_q_ - (uint16_t)1U;
}

static uint8_t FStar_UInt8_eq_mask(uint8_t a, uint8_t b)
{
  uint8_t x = a ^ b;
  uint8_t minus_x = ~x + (uint8_t)1U;
  uint8_t x_or_minus_x = x | minus_x;
  uint8_t xnx = x_or_minus_x >> (uint32_t)7U;
  return xnx - (uint8_t)1U;
}

static uint8_t FStar_UInt8_gte_mask(uint8_t a, uint8_t b)
{
  uint8_t x = a;
  uint8_t y = b;
  uint8_t x_xor_y = x ^ y;
  uint8_t x_sub_y = x - y;
  uint8_t x_sub_y_xor_y = x_sub_y ^ y;
  uint8_t q = x_xor_y | x_sub_y_xor_y;
  uint8_t x_xor_q = x ^ q;
  uint8_t x_xor_q_ = x_xor_q >> (uint32_t)7U;
  return x_xor_q_ - (uint8_t)1U;
}

static void Hacl_Bignum_Modulo_carry_top(uint64_t *b)
{
  uint64_t b4 = b[4U];
  uint64_t b0 = b[0U];
  uint64_t b4_ = b4 & (uint64_t)0x7ffffffffffffU;
  uint64_t b0_ = b0 + (uint64_t)19U * (b4 >> (uint32_t)51U);
  b[4U] = b4_;
  b[0U] = b0_;
}

inline static void Hacl_Bignum_Fproduct_copy_from_wide_(uint64_t *output, uint128_t *input)
{
  uint32_t i;
  for (i = (uint32_t)0U; i < (uint32_t)5U; i = i + (uint32_t)1U)
  {
    uint128_t xi = input[i];
    output[i] = (uint64_t)xi;
  }
}

inline static void
Hacl_Bignum_Fproduct_sum_scalar_multiplication_(uint128_t *output, uint64_t *input, uint64_t s)
{
  uint32_t i;
  for (i = (uint32_t)0U; i < (uint32_t)5U; i = i + (uint32_t)1U)
  {
    uint128_t xi = output[i];
    uint64_t yi = input[i];
    output[i] = xi + (uint128_t)yi * s;
  }
}

inline static void Hacl_Bignum_Fproduct_carry_wide_(uint128_t *tmp)
{
  uint32_t i;
  for (i = (uint32_t)0U; i < (uint32_t)4U; i = i + (uint32_t)1U)
  {
    uint32_t ctr = i;
    uint128_t tctr = tmp[ctr];
    uint128_t tctrp1 = tmp[ctr + (uint32_t)1U];
    uint64_t r0 = (uint64_t)tctr & (uint64_t)0x7ffffffffffffU;
    uint128_t c = tctr >> (uint32_t)51U;
    tmp[ctr] = (uint128_t)r0;
    tmp[ctr + (uint32_t)1U] = tctrp1 + c;
  }
}

inline static void Hacl_Bignum_Fmul_shift_reduce(uint64_t *output)
{
  uint64_t tmp = output[4U];
  uint64_t b0;
  {
    uint32_t i;
    for (i = (uint32_t)0U; i < (uint32_t)4U; i = i + (uint32_t)1U)
    {
      uint32_t ctr = (uint32_t)5U - i - (uint32_t)1U;
      uint64_t z = output[ctr - (uint32_t)1U];
      output[ctr] = z;
    }
  }
  output[0U] = tmp;
  b0 = output[0U];
  output[0U] = (uint64_t)19U * b0;
}

static void
Hacl_Bignum_Fmul_mul_shift_reduce_(uint128_t *output, uint64_t *input, uint64_t *input2)
{
  uint32_t i;
  uint64_t input2i;
  {
    uint32_t i0;
    for (i0 = (uint32_t)0U; i0 < (uint32_t)4U; i0 = i0 + (uint32_t)1U)
    {
      uint64_t input2i0 = input2[i0];
      Hacl_Bignum_Fproduct_sum_scalar_multiplication_(output, input, input2i0);
      Hacl_Bignum_Fmul_shift_reduce(input);
    }
  }
  i = (uint32_t)4U;
  input2i = input2[i];
  Hacl_Bignum_Fproduct_sum_scalar_multiplication_(output, input, input2i);
}

inline static void Hacl_Bignum_Fmul_fmul(uint64_t *output, uint64_t *input, uint64_t *input2)
{
  uint64_t tmp[5U] = { 0U };
  memcpy(tmp, input, (uint32_t)5U * sizeof input[0U]);
  KRML_CHECK_SIZE(sizeof (uint128_t), (uint32_t)5U);
  {
    uint128_t t[5U];
    {
      uint32_t _i;
      for (_i = 0U; _i < (uint32_t)5U; ++_i)
        t[_i] = (uint128_t)(uint64_t)0U;
    }
    {
      uint128_t b4;
      uint128_t b0;
      uint128_t b4_;
      uint128_t b0_;
      uint64_t i0;
      uint64_t i1;
      uint64_t i0_;
      uint64_t i1_;
      Hacl_Bignum_Fmul_mul_shift_reduce_(t, tmp, input2);
      Hacl_Bignum_Fproduct_carry_wide_(t);
      b4 = t[4U];
      b0 = t[0U];
      b4_ = b4 & (uint128_t)(uint64_t)0x7ffffffffffffU;
      b0_ = b0 + (uint128_t)(uint64_t)19U * (uint64_t)(b4 >> (uint32_t)51U);
      t[4U] = b4_;
      t[0U] = b0_;
      Hacl_Bignum_Fproduct_copy_from_wide_(output, t);
      i0 = output[0U];
      i1 = output[1U];
      i0_ = i0 & (uint64_t)0x7ffffffffffffU;
      i1_ = i1 + (i0 >> (uint32_t)51U);
      output[0U] = i0_;
      output[1U] = i1_;
    }
  }
}

inline static void Hacl_Bignum_Fsquare_fsquare__(uint128_t *tmp, uint64_t *output)
{
  uint64_t r0 = output[0U];
  uint64_t r1 = output[1U];
  uint64_t r2 = output[2U];
  uint64_t r3 = output[3U];
  uint64_t r4 = output[4U];
  uint64_t d0 = r0 * (uint64_t)2U;
  uint64_t d1 = r1 * (uint64_t)2U;
  uint64_t d2 = r2 * (uint64_t)2U * (uint64_t)19U;
  uint64_t d419 = r4 * (uint64_t)19U;
  uint64_t d4 = d419 * (uint64_t)2U;
  uint128_t s0 = (uint128_t)r0 * r0 + (uint128_t)d4 * r1 + (uint128_t)d2 * r3;
  uint128_t s1 = (uint128_t)d0 * r1 + (uint128_t)d4 * r2 + (uint128_t)(r3 * (uint64_t)19U) * r3;
  uint128_t s2 = (uint128_t)d0 * r2 + (uint128_t)r1 * r1 + (uint128_t)d4 * r3;
  uint128_t s3 = (uint128_t)d0 * r3 + (uint128_t)d1 * r2 + (uint128_t)r4 * d419;
  uint128_t s4 = (uint128_t)d0 * r4 + (uint128_t)d1 * r3 + (uint128_t)r2 * r2;
  tmp[0U] = s0;
  tmp[1U] = s1;
  tmp[2U] = s2;
  tmp[3U] = s3;
  tmp[4U] = s4;
}

inline static void Hacl_Bignum_Fsquare_fsquare_(uint128_t *tmp, uint64_t *output)
{
  uint128_t b4;
  uint128_t b0;
  uint128_t b4_;
  uint128_t b0_;
  uint64_t i0;
  uint64_t i1;
  uint64_t i0_;
  uint64_t i1_;
  Hacl_Bignum_Fsquare_fsquare__(tmp, output);
  Hacl_Bignum_Fproduct_carry_wide_(tmp);
  b4 = tmp[4U];
  b0 = tmp[0U];
  b4_ = b4 & (uint128_t)(uint64_t)0x7ffffffffffffU;
  b0_ = b0 + (uint128_t)(uint64_t)19U * (uint64_t)(b4 >> (uint32_t)51U);
  tmp[4U] = b4_;
  tmp[0U] = b0_;
  Hacl_Bignum_Fproduct_copy_from_wide_(output, tmp);
  i0 = output[0U];
  i1 = output[1U];
  i0_ = i0 & (uint64_t)0x7ffffffffffffU;
  i1_ = i1 + (i0 >> (uint32_t)51U);
  output[0U] = i0_;
  output[1U] = i1_;
}

static void
Hacl_Bignum_Fsquare_fsquare_times_(uint64_t *input, uint128_t *tmp, uint32_t count1)
{
  uint32_t i;
  Hacl_Bignum_Fsquare_fsquare_(tmp, input);
  for (i = (uint32_t)1U; i < count1; i = i + (uint32_t)1U)
    Hacl_Bignum_Fsquare_fsquare_(tmp, input);
}

inline static void
Hacl_Bignum_Fsquare_fsquare_times(uint64_t *output, uint64_t *input, uint32_t count1)
{
  KRML_CHECK_SIZE(sizeof (uint128_t), (uint32_t)5U);
  {
    uint128_t t[5U];
    {
      uint32_t _i;
      for (_i = 0U; _i < (uint32_t)5U; ++_i)
        t[_i] = (uint128_t)(uint64_t)0U;
    }
    memcpy(output, input, (uint32_t)5U * sizeof input[0U]);
    Hacl_Bignum_Fsquare_fsquare_times_(output, t, count1);
  }
}

inline static void Hacl_Bignum_Fsquare_fsquare_times_inplace(uint64_t *output, uint32_t count1)
{
  KRML_CHECK_SIZE(sizeof (uint128_t), (uint32_t)5U);
  {
    uint128_t t[5U];
    {
      uint32_t _i;
      for (_i = 0U; _i < (uint32_t)5U; ++_i)
        t[_i] = (uint128_t)(uint64_t)0U;
    }
    Hacl_Bignum_Fsquare_fsquare_times_(output, t, count1);
  }
}

inline static void Hacl_Bignum_Crecip_crecip(uint64_t *out, uint64_t *z)
{
  uint64_t buf[20U] = { 0U };
  uint64_t *a0 = buf;
  uint64_t *t00 = buf + (uint32_t)5U;
  uint64_t *b0 = buf + (uint32_t)10U;
  uint64_t *t01;
  uint64_t *b1;
  uint64_t *c0;
  uint64_t *a;
  uint64_t *t0;
  uint64_t *b;
  uint64_t *c;
  Hacl_Bignum_Fsquare_fsquare_times(a0, z, (uint32_t)1U);
  Hacl_Bignum_Fsquare_fsquare_times(t00, a0, (uint32_t)2U);
  Hacl_Bignum_Fmul_fmul(b0, t00, z);
  Hacl_Bignum_Fmul_fmul(a0, b0, a0);
  Hacl_Bignum_Fsquare_fsquare_times(t00, a0, (uint32_t)1U);
  Hacl_Bignum_Fmul_fmul(b0, t00, b0);
  Hacl_Bignum_Fsquare_fsquare_times(t00, b0, (uint32_t)5U);
  t01 = buf + (uint32_t)5U;
  b1 = buf + (uint32_t)10U;
  c0 = buf + (uint32_t)15U;
  Hacl_Bignum_Fmul_fmul(b1, t01, b1);
  Hacl_Bignum_Fsquare_fsquare_times(t01, b1, (uint32_t)10U);
  Hacl_Bignum_Fmul_fmul(c0, t01, b1);
  Hacl_Bignum_Fsquare_fsquare_times(t01, c0, (uint32_t)20U);
  Hacl_Bignum_Fmul_fmul(t01, t01, c0);
  Hacl_Bignum_Fsquare_fsquare_times_inplace(t01, (uint32_t)10U);
  Hacl_Bignum_Fmul_fmul(b1, t01, b1);
  Hacl_Bignum_Fsquare_fsquare_times(t01, b1, (uint32_t)50U);
  a = buf;
  t0 = buf + (uint32_t)5U;
  b = buf + (uint32_t)10U;
  c = buf + (uint32_t)15U;
  Hacl_Bignum_Fmul_fmul(c, t0, b);
  Hacl_Bignum_Fsquare_fsquare_times(t0, c, (uint32_t)100U);
  Hacl_Bignum_Fmul_fmul(t0, t0, c);
  Hacl_Bignum_Fsquare_fsquare_times_inplace(t0, (uint32_t)50U);
  Hacl_Bignum_Fmul_fmul(t0, t0, b);
  Hacl_Bignum_Fsquare_fsquare_times_inplace(t0, (uint32_t)5U);
  Hacl_Bignum_Fmul_fmul(out, t0, a);
}

inline static void Hacl_Bignum_fsum(uint64_t *a, uint64_t *b)
{
  uint32_t i;
  for (i = (uint32_t)0U; i < (uint32_t)5U; i = i + (uint32_t)1U)
  {
    uint64_t xi = a[i];
    uint64_t yi = b[i];
    a[i] = xi + yi;
  }
}

inline static void Hacl_Bignum_fdifference(uint64_t *a, uint64_t *b)
{
  uint64_t tmp[5U] = { 0U };
  uint64_t b0;
  uint64_t b1;
  uint64_t b2;
  uint64_t b3;
  uint64_t b4;
  memcpy(tmp, b, (uint32_t)5U * sizeof b[0U]);
  b0 = tmp[0U];
  b1 = tmp[1U];
  b2 = tmp[2U];
  b3 = tmp[3U];
  b4 = tmp[4U];
  tmp[0U] = b0 + (uint64_t)0x3fffffffffff68U;
  tmp[1U] = b1 + (uint64_t)0x3ffffffffffff8U;
  tmp[2U] = b2 + (uint64_t)0x3ffffffffffff8U;
  tmp[3U] = b3 + (uint64_t)0x3ffffffffffff8U;
  tmp[4U] = b4 + (uint64_t)0x3ffffffffffff8U;
  {
    uint32_t i;
    for (i = (uint32_t)0U; i < (uint32_t)5U; i = i + (uint32_t)1U)
    {
      uint64_t xi = a[i];
      uint64_t yi = tmp[i];
      a[i] = yi - xi;
    }
  }
}

inline static void Hacl_Bignum_fscalar(uint64_t *output, uint64_t *b, uint64_t s)
{
  KRML_CHECK_SIZE(sizeof (uint128_t), (uint32_t)5U);
  {
    uint128_t tmp[5U];
    {
      uint32_t _i;
      for (_i = 0U; _i < (uint32_t)5U; ++_i)
        tmp[_i] = (uint128_t)(uint64_t)0U;
    }
    {
      uint128_t b4;
      uint128_t b0;
      uint128_t b4_;
      uint128_t b0_;
      {
        uint32_t i;
        for (i = (uint32_t)0U; i < (uint32_t)5U; i = i + (uint32_t)1U)
        {
          uint64_t xi = b[i];
          tmp[i] = (uint128_t)xi * s;
        }
      }
      Hacl_Bignum_Fproduct_carry_wide_(tmp);
      b4 = tmp[4U];
      b0 = tmp[0U];
      b4_ = b4 & (uint128_t)(uint64_t)0x7ffffffffffffU;
      b0_ = b0 + (uint128_t)(uint64_t)19U * (uint64_t)(b4 >> (uint32_t)51U);
      tmp[4U] = b4_;
      tmp[0U] = b0_;
      Hacl_Bignum_Fproduct_copy_from_wide_(output, tmp);
    }
  }
}

inline static void Hacl_Bignum_fmul(uint64_t *output, uint64_t *a, uint64_t *b)
{
  Hacl_Bignum_Fmul_fmul(output, a, b);
}

inline static void Hacl_Bignum_crecip(uint64_t *output, uint64_t *input)
{
  Hacl_Bignum_Crecip_crecip(output, input);
}

static void
Hacl_EC_Point_swap_conditional_step(uint64_t *a, uint64_t *b, uint64_t swap1, uint32_t ctr)
{
  uint32_t i = ctr - (uint32_t)1U;
  uint64_t ai = a[i];
  uint64_t bi = b[i];
  uint64_t x = swap1 & (ai ^ bi);
  uint64_t ai1 = ai ^ x;
  uint64_t bi1 = bi ^ x;
  a[i] = ai1;
  b[i] = bi1;
}

static void
Hacl_EC_Point_swap_conditional_(uint64_t *a, uint64_t *b, uint64_t swap1, uint32_t ctr)
{
  if (!(ctr == (uint32_t)0U))
  {
    uint32_t i;
    Hacl_EC_Point_swap_conditional_step(a, b, swap1, ctr);
    i = ctr - (uint32_t)1U;
    Hacl_EC_Point_swap_conditional_(a, b, swap1, i);
  }
}

static void Hacl_EC_Point_swap_conditional(uint64_t *a, uint64_t *b, uint64_t iswap)
{
  uint64_t swap1 = (uint64_t)0U - iswap;
  Hacl_EC_Point_swap_conditional_(a, b, swap1, (uint32_t)5U);
  Hacl_EC_Point_swap_conditional_(a + (uint32_t)5U, b + (uint32_t)5U, swap1, (uint32_t)5U);
}

static void Hacl_EC_Point_copy(uint64_t *output, uint64_t *input)
{
  memcpy(output, input, (uint32_t)5U * sizeof input[0U]);
  memcpy(output + (uint32_t)5U,
    input + (uint32_t)5U,
    (uint32_t)5U * sizeof (input + (uint32_t)5U)[0U]);
}

static void Hacl_EC_Format_fexpand(uint64_t *output, uint8_t *input)
{
  uint64_t i0 = load64_le(input);
  uint8_t *x00 = input + (uint32_t)6U;
  uint64_t i1 = load64_le(x00);
  uint8_t *x01 = input + (uint32_t)12U;
  uint64_t i2 = load64_le(x01);
  uint8_t *x02 = input + (uint32_t)19U;
  uint64_t i3 = load64_le(x02);
  uint8_t *x0 = input + (uint32_t)24U;
  uint64_t i4 = load64_le(x0);
  uint64_t output0 = i0 & (uint64_t)0x7ffffffffffffU;
  uint64_t output1 = i1 >> (uint32_t)3U & (uint64_t)0x7ffffffffffffU;
  uint64_t output2 = i2 >> (uint32_t)6U & (uint64_t)0x7ffffffffffffU;
  uint64_t output3 = i3 >> (uint32_t)1U & (uint64_t)0x7ffffffffffffU;
  uint64_t output4 = i4 >> (uint32_t)12U & (uint64_t)0x7ffffffffffffU;
  output[0U] = output0;
  output[1U] = output1;
  output[2U] = output2;
  output[3U] = output3;
  output[4U] = output4;
}

static void Hacl_EC_Format_fcontract_first_carry_pass(uint64_t *input)
{
  uint64_t t0 = input[0U];
  uint64_t t1 = input[1U];
  uint64_t t2 = input[2U];
  uint64_t t3 = input[3U];
  uint64_t t4 = input[4U];
  uint64_t t1_ = t1 + (t0 >> (uint32_t)51U);
  uint64_t t0_ = t0 & (uint64_t)0x7ffffffffffffU;
  uint64_t t2_ = t2 + (t1_ >> (uint32_t)51U);
  uint64_t t1__ = t1_ & (uint64_t)0x7ffffffffffffU;
  uint64_t t3_ = t3 + (t2_ >> (uint32_t)51U);
  uint64_t t2__ = t2_ & (uint64_t)0x7ffffffffffffU;
  uint64_t t4_ = t4 + (t3_ >> (uint32_t)51U);
  uint64_t t3__ = t3_ & (uint64_t)0x7ffffffffffffU;
  input[0U] = t0_;
  input[1U] = t1__;
  input[2U] = t2__;
  input[3U] = t3__;
  input[4U] = t4_;
}

static void Hacl_EC_Format_fcontract_first_carry_full(uint64_t *input)
{
  Hacl_EC_Format_fcontract_first_carry_pass(input);
  Hacl_Bignum_Modulo_carry_top(input);
}

static void Hacl_EC_Format_fcontract_second_carry_pass(uint64_t *input)
{
  uint64_t t0 = input[0U];
  uint64_t t1 = input[1U];
  uint64_t t2 = input[2U];
  uint64_t t3 = input[3U];
  uint64_t t4 = input[4U];
  uint64_t t1_ = t1 + (t0 >> (uint32_t)51U);
  uint64_t t0_ = t0 & (uint64_t)0x7ffffffffffffU;
  uint64_t t2_ = t2 + (t1_ >> (uint32_t)51U);
  uint64_t t1__ = t1_ & (uint64_t)0x7ffffffffffffU;
  uint64_t t3_ = t3 + (t2_ >> (uint32_t)51U);
  uint64_t t2__ = t2_ & (uint64_t)0x7ffffffffffffU;
  uint64_t t4_ = t4 + (t3_ >> (uint32_t)51U);
  uint64_t t3__ = t3_ & (uint64_t)0x7ffffffffffffU;
  input[0U] = t0_;
  input[1U] = t1__;
  input[2U] = t2__;
  input[3U] = t3__;
  input[4U] = t4_;
}

static void Hacl_EC_Format_fcontract_second_carry_full(uint64_t *input)
{
  uint64_t i0;
  uint64_t i1;
  uint64_t i0_;
  uint64_t i1_;
  Hacl_EC_Format_fcontract_second_carry_pass(input);
  Hacl_Bignum_Modulo_carry_top(input);
  i0 = input[0U];
  i1 = input[1U];
  i0_ = i0 & (uint64_t)0x7ffffffffffffU;
  i1_ = i1 + (i0 >> (uint32_t)51U);
  input[0U] = i0_;
  input[1U] = i1_;
}

static void Hacl_EC_Format_fcontract_trim(uint64_t *input)
{
  uint64_t a0 = input[0U];
  uint64_t a1 = input[1U];
  uint64_t a2 = input[2U];
  uint64_t a3 = input[3U];
  uint64_t a4 = input[4U];
  uint64_t mask0 = FStar_UInt64_gte_mask(a0, (uint64_t)0x7ffffffffffedU);
  uint64_t mask1 = FStar_UInt64_eq_mask(a1, (uint64_t)0x7ffffffffffffU);
  uint64_t mask2 = FStar_UInt64_eq_mask(a2, (uint64_t)0x7ffffffffffffU);
  uint64_t mask3 = FStar_UInt64_eq_mask(a3, (uint64_t)0x7ffffffffffffU);
  uint64_t mask4 = FStar_UInt64_eq_mask(a4, (uint64_t)0x7ffffffffffffU);
  uint64_t mask = (((mask0 & mask1) & mask2) & mask3) & mask4;
  uint64_t a0_ = a0 - ((uint64_t)0x7ffffffffffedU & mask);
  uint64_t a1_ = a1 - ((uint64_t)0x7ffffffffffffU & mask);
  uint64_t a2_ = a2 - ((uint64_t)0x7ffffffffffffU & mask);
  uint64_t a3_ = a3 - ((uint64_t)0x7ffffffffffffU & mask);
  uint64_t a4_ = a4 - ((uint64_t)0x7ffffffffffffU & mask);
  input[0U] = a0_;
  input[1U] = a1_;
  input[2U] = a2_;
  input[3U] = a3_;
  input[4U] = a4_;
}

static void Hacl_EC_Format_fcontract_store(uint8_t *output, uint64_t *input)
{
  uint64_t t0 = input[0U];
  uint64_t t1 = input[1U];
  uint64_t t2 = input[2U];
  uint64_t t3 = input[3U];
  uint64_t t4 = input[4U];
  uint64_t o0 = t1 << (uint32_t)51U | t0;
  uint64_t o1 = t2 << (uint32_t)38U | t1 >> (uint32_t)13U;
  uint64_t o2 = t3 << (uint32_t)25U | t2 >> (uint32_t)26U;
  uint64_t o3 = t4 << (uint32_t)12U | t3 >> (uint32_t)39U;
  uint8_t *b0 = output;
  uint8_t *b1 = output + (uint32_t)8U;
  uint8_t *b2 = output + (uint32_t)16U;
  uint8_t *b3 = output + (uint32_t)24U;
  store64_le(b0, o0);
  store64_le(b1, o1);
  store64_le(b2, o2);
  store64_le(b3, o3);
}

static void Hacl_EC_Format_fcontract(uint8_t *output, uint64_t *input)
{
  Hacl_EC_Format_fcontract_first_carry_full(input);
  Hacl_EC_Format_fcontract_second_carry_full(input);
  Hacl_EC_Format_fcontract_trim(input);
  Hacl_EC_Format_fcontract_store(output, input);
}

static void Hacl_EC_Format_scalar_of_point(uint8_t *scalar, uint64_t *point)
{
  uint64_t *x = point;
  uint64_t *z = point + (uint32_t)5U;
  uint64_t buf[10U] = { 0U };
  uint64_t *zmone = buf;
  uint64_t *sc = buf + (uint32_t)5U;
  Hacl_Bignum_crecip(zmone, z);
  Hacl_Bignum_fmul(sc, x, zmone);
  Hacl_EC_Format_fcontract(scalar, sc);
}

static void
Hacl_EC_AddAndDouble_fmonty(
  uint64_t *pp,
  uint64_t *ppq,
  uint64_t *p,
  uint64_t *pq,
  uint64_t *qmqp
)
{
  uint64_t *qx = qmqp;
  uint64_t *x2 = pp;
  uint64_t *z2 = pp + (uint32_t)5U;
  uint64_t *x3 = ppq;
  uint64_t *z3 = ppq + (uint32_t)5U;
  uint64_t *x = p;
  uint64_t *z = p + (uint32_t)5U;
  uint64_t *xprime = pq;
  uint64_t *zprime = pq + (uint32_t)5U;
  uint64_t buf[40U] = { 0U };
  uint64_t *origx = buf;
  uint64_t *origxprime0 = buf + (uint32_t)5U;
  uint64_t *xxprime0 = buf + (uint32_t)25U;
  uint64_t *zzprime0 = buf + (uint32_t)30U;
  uint64_t *origxprime;
  uint64_t *xx0;
  uint64_t *zz0;
  uint64_t *xxprime;
  uint64_t *zzprime;
  uint64_t *zzzprime;
  uint64_t *zzz;
  uint64_t *xx;
  uint64_t *zz;
  uint64_t scalar;
  memcpy(origx, x, (uint32_t)5U * sizeof x[0U]);
  Hacl_Bignum_fsum(x, z);
  Hacl_Bignum_fdifference(z, origx);
  memcpy(origxprime0, xprime, (uint32_t)5U * sizeof xprime[0U]);
  Hacl_Bignum_fsum(xprime, zprime);
  Hacl_Bignum_fdifference(zprime, origxprime0);
  Hacl_Bignum_fmul(xxprime0, xprime, z);
  Hacl_Bignum_fmul(zzprime0, x, zprime);
  origxprime = buf + (uint32_t)5U;
  xx0 = buf + (uint32_t)15U;
  zz0 = buf + (uint32_t)20U;
  xxprime = buf + (uint32_t)25U;
  zzprime = buf + (uint32_t)30U;
  zzzprime = buf + (uint32_t)35U;
  memcpy(origxprime, xxprime, (uint32_t)5U * sizeof xxprime[0U]);
  Hacl_Bignum_fsum(xxprime, zzprime);
  Hacl_Bignum_fdifference(zzprime, origxprime);
  Hacl_Bignum_Fsquare_fsquare_times(x3, xxprime, (uint32_t)1U);
  Hacl_Bignum_Fsquare_fsquare_times(zzzprime, zzprime, (uint32_t)1U);
  Hacl_Bignum_fmul(z3, zzzprime, qx);
  Hacl_Bignum_Fsquare_fsquare_times(xx0, x, (uint32_t)1U);
  Hacl_Bignum_Fsquare_fsquare_times(zz0, z, (uint32_t)1U);
  zzz = buf + (uint32_t)10U;
  xx = buf + (uint32_t)15U;
  zz = buf + (uint32_t)20U;
  Hacl_Bignum_fmul(x2, xx, zz);
  Hacl_Bignum_fdifference(zz, xx);
  scalar = (uint64_t)121665U;
  Hacl_Bignum_fscalar(zzz, zz, scalar);
  Hacl_Bignum_fsum(zzz, xx);
  Hacl_Bignum_fmul(z2, zzz, zz);
}

static void
Hacl_EC_Ladder_SmallLoop_cmult_small_loop_step(
  uint64_t *nq,
  uint64_t *nqpq,
  uint64_t *nq2,
  uint64_t *nqpq2,
  uint64_t *q,
  uint8_t byt
)
{
  uint64_t bit0 = (uint64_t)(byt >> (uint32_t)7U);
  uint64_t bit;
  Hacl_EC_Point_swap_conditional(nq, nqpq, bit0);
  Hacl_EC_AddAndDouble_fmonty(nq2, nqpq2, nq, nqpq, q);
  bit = (uint64_t)(byt >> (uint32_t)7U);
  Hacl_EC_Point_swap_conditional(nq2, nqpq2, bit);
}

static void
Hacl_EC_Ladder_SmallLoop_cmult_small_loop_double_step(
  uint64_t *nq,
  uint64_t *nqpq,
  uint64_t *nq2,
  uint64_t *nqpq2,
  uint64_t *q,
  uint8_t byt
)
{
  uint8_t byt1;
  Hacl_EC_Ladder_SmallLoop_cmult_small_loop_step(nq, nqpq, nq2, nqpq2, q, byt);
  byt1 = byt << (uint32_t)1U;
  Hacl_EC_Ladder_SmallLoop_cmult_small_loop_step(nq2, nqpq2, nq, nqpq, q, byt1);
}

static void
Hacl_EC_Ladder_SmallLoop_cmult_small_loop(
  uint64_t *nq,
  uint64_t *nqpq,
  uint64_t *nq2,
  uint64_t *nqpq2,
  uint64_t *q,
  uint8_t byt,
  uint32_t i
)
{
  if (!(i == (uint32_t)0U))
  {
    uint32_t i_ = i - (uint32_t)1U;
    uint8_t byt_;
    Hacl_EC_Ladder_SmallLoop_cmult_small_loop_double_step(nq, nqpq, nq2, nqpq2, q, byt);
    byt_ = byt << (uint32_t)2U;
    Hacl_EC_Ladder_SmallLoop_cmult_small_loop(nq, nqpq, nq2, nqpq2, q, byt_, i_);
  }
}

static void
Hacl_EC_Ladder_BigLoop_cmult_big_loop(
  uint8_t *n1,
  uint64_t *nq,
  uint64_t *nqpq,
  uint64_t *nq2,
  uint64_t *nqpq2,
  uint64_t *q,
  uint32_t i
)
{
  if (!(i == (uint32_t)0U))
  {
    uint32_t i1 = i - (uint32_t)1U;
    uint8_t byte = n1[i1];
    Hacl_EC_Ladder_SmallLoop_cmult_small_loop(nq, nqpq, nq2, nqpq2, q, byte, (uint32_t)4U);
    Hacl_EC_Ladder_BigLoop_cmult_big_loop(n1, nq, nqpq, nq2, nqpq2, q, i1);
  }
}

static void Hacl_EC_Ladder_cmult(uint64_t *result, uint8_t *n1, uint64_t *q)
{
  uint64_t point_buf[40U] = { 0U };
  uint64_t *nq = point_buf;
  uint64_t *nqpq = point_buf + (uint32_t)10U;
  uint64_t *nq2 = point_buf + (uint32_t)20U;
  uint64_t *nqpq2 = point_buf + (uint32_t)30U;
  Hacl_EC_Point_copy(nqpq, q);
  nq[0U] = (uint64_t)1U;
  Hacl_EC_Ladder_BigLoop_cmult_big_loop(n1, nq, nqpq, nq2, nqpq2, q, (uint32_t)32U);
  Hacl_EC_Point_copy(result, nq);
}

static void Hacl_Curve25519_crypto_scalarmult(uint8_t *mypublic, uint8_t *secret, uint8_t *basepoint)
{
  uint64_t buf0[10U] = { 0U };
  uint64_t *x0 = buf0;
  uint64_t *z = buf0 + (uint32_t)5U;
  uint64_t *q;
  Hacl_EC_Format_fexpand(x0, basepoint);
  z[0U] = (uint64_t)1U;
  q = buf0;
  {
    uint8_t e[32U] = { 0U };
    uint8_t e0;
    uint8_t e31;
    uint8_t e01;
    uint8_t e311;
    uint8_t e312;
    uint8_t *scalar;
    memcpy(e, secret, (uint32_t)32U * sizeof secret[0U]);
    e0 = e[0U];
    e31 = e[31U];
    e01 = e0 & (uint8_t)248U;
    e311 = e31 & (uint8_t)127U;
    e312 = e311 | (uint8_t)64U;
    e[0U] = e01;
    e[31U] = e312;
    scalar = e;
    {
      uint64_t buf[15U] = { 0U };
      uint64_t *nq = buf;
      uint64_t *x = nq;
      x[0U] = (uint64_t)1U;
      Hacl_EC_Ladder_cmult(nq, scalar, q);
      Hacl_EC_Format_scalar_of_point(mypublic, nq);
    }
  }
}

static void mbedtls_x25519_init( mbedtls_x25519_context *ctx )
{
    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_x25519_context ) );
}

static void mbedtls_x25519_free( mbedtls_x25519_context *ctx )
{
    if( ctx == NULL )
        return;
    mbedtls_platform_zeroize( ctx->our_secret, MBEDTLS_X25519_KEY_SIZE_BYTES );
    mbedtls_platform_zeroize( ctx->peer_point, MBEDTLS_X25519_KEY_SIZE_BYTES );
}

static int mbedtls_x25519_make_params( mbedtls_x25519_context *ctx, size_t *olen,
                                       unsigned char *buf, size_t blen,
                                       int( *f_rng )(void *, unsigned char *, size_t),
                                       void *p_rng )
{
    int ret = 0;
    uint8_t base[MBEDTLS_X25519_KEY_SIZE_BYTES] = {0};
    if( ( ret = f_rng( p_rng, ctx->our_secret, MBEDTLS_X25519_KEY_SIZE_BYTES ) ) != 0 )
        return ret;
    *olen = MBEDTLS_X25519_KEY_SIZE_BYTES + 4;
    if( blen < *olen )
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    *buf++ = MBEDTLS_ECP_TLS_NAMED_CURVE;
    *buf++ = MBEDTLS_ECP_TLS_CURVE25519 >> 8;
    *buf++ = MBEDTLS_ECP_TLS_CURVE25519 & 0xFF;
    *buf++ = MBEDTLS_X25519_KEY_SIZE_BYTES;
    base[0] = 9;
    Hacl_Curve25519_crypto_scalarmult( buf, ctx->our_secret, base );
    base[0] = 0;
    if( memcmp( buf, base, MBEDTLS_X25519_KEY_SIZE_BYTES) == 0 )
        return MBEDTLS_ERR_ECP_RANDOM_FAILED;
    return( 0 );
}

static int mbedtls_x25519_read_params( mbedtls_x25519_context *ctx,
                                       const unsigned char **buf, const unsigned char *end )
{
    if( end - *buf < MBEDTLS_X25519_KEY_SIZE_BYTES + 1 )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    if( ( *(*buf)++ != MBEDTLS_X25519_KEY_SIZE_BYTES ) )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    memcpy( ctx->peer_point, *buf, MBEDTLS_X25519_KEY_SIZE_BYTES );
    *buf += MBEDTLS_X25519_KEY_SIZE_BYTES;
    return( 0 );
}

static int mbedtls_x25519_get_params( mbedtls_x25519_context *ctx, const mbedtls_ecp_keypair *key,
                                      mbedtls_x25519_ecdh_side side )
{
    size_t olen = 0;
    switch( side ) {
    case MBEDTLS_X25519_ECDH_THEIRS:
        return mbedtls_ecp_point_write_binary( &key->grp, &key->Q, MBEDTLS_ECP_PF_COMPRESSED, &olen, ctx->peer_point, MBEDTLS_X25519_KEY_SIZE_BYTES );
    case MBEDTLS_X25519_ECDH_OURS:
        return mbedtls_mpi_write_binary_le( &key->d, ctx->our_secret, MBEDTLS_X25519_KEY_SIZE_BYTES );
    default:
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }
}

static int mbedtls_x25519_calc_secret( mbedtls_x25519_context *ctx, size_t *olen,
                                       unsigned char *buf, size_t blen,
                                       int( *f_rng )(void *, unsigned char *, size_t),
                                       void *p_rng )
{
    /* f_rng and p_rng are not used here because this implementation does not
       need blinding since it has constant trace. */
    (( void )f_rng);
    (( void )p_rng);
    *olen = MBEDTLS_X25519_KEY_SIZE_BYTES;
    if( blen < *olen )
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    Hacl_Curve25519_crypto_scalarmult( buf, ctx->our_secret, ctx->peer_point);
    /* Wipe the DH secret and don't let the peer chose a small subgroup point */
    mbedtls_platform_zeroize( ctx->our_secret, MBEDTLS_X25519_KEY_SIZE_BYTES );
    if( memcmp( buf, ctx->our_secret, MBEDTLS_X25519_KEY_SIZE_BYTES) == 0 )
        return MBEDTLS_ERR_ECP_RANDOM_FAILED;
    return( 0 );
}

static int mbedtls_x25519_make_public( mbedtls_x25519_context *ctx, size_t *olen,
                                       unsigned char *buf, size_t blen,
                                       int( *f_rng )(void *, unsigned char *, size_t),
                                       void *p_rng )
{
    int ret = 0;
    unsigned char base[MBEDTLS_X25519_KEY_SIZE_BYTES] = { 0 };
    if( ctx == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    if( ( ret = f_rng( p_rng, ctx->our_secret, MBEDTLS_X25519_KEY_SIZE_BYTES ) ) != 0 )
        return ret;
    *olen = MBEDTLS_X25519_KEY_SIZE_BYTES + 1;
    if( blen < *olen )
        return(MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL);
    *buf++ = MBEDTLS_X25519_KEY_SIZE_BYTES;
    base[0] = 9;
    Hacl_Curve25519_crypto_scalarmult( buf, ctx->our_secret, base );
    base[0] = 0;
    if( memcmp( buf, base, MBEDTLS_X25519_KEY_SIZE_BYTES ) == 0 )
        return MBEDTLS_ERR_ECP_RANDOM_FAILED;
    return( ret );
}

static int mbedtls_x25519_read_public( mbedtls_x25519_context *ctx,
                                       const unsigned char *buf, size_t blen )
{
    if( blen < MBEDTLS_X25519_KEY_SIZE_BYTES + 1 )
        return(MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL);
    if( (*buf++ != MBEDTLS_X25519_KEY_SIZE_BYTES) )
        return(MBEDTLS_ERR_ECP_BAD_INPUT_DATA);
    memcpy( ctx->peer_point, buf, MBEDTLS_X25519_KEY_SIZE_BYTES );
    return( 0 );
}

/**
 * \brief           This function sets up the ECDH context with the information
 *                  given.
 *
 *                  This function should be called after mbedtls_ecdh_init() but
 *                  before mbedtls_ecdh_make_params(). There is no need to call
 *                  this function before mbedtls_ecdh_read_params().
 *
 *                  This is the first function used by a TLS server for ECDHE
 *                  ciphersuites.
 *
 * \param ctx       The ECDH context to set up.
 * \param grp_id    The group id of the group to set up the context for.
 *
 * \return          \c 0 on success.
 */
int mbedtls_everest_setup( mbedtls_ecdh_context_everest *ctx, int grp_id )
{
    if( grp_id != MBEDTLS_ECP_DP_CURVE25519 )
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    mbedtls_x25519_init( &ctx->ctx );
    return 0;
}

/**
 * \brief           This function frees a context.
 *
 * \param ctx       The context to free.
 */
void mbedtls_everest_free( mbedtls_ecdh_context_everest *ctx )
{
    mbedtls_x25519_free( &ctx->ctx );
}

/**
 * \brief           This function generates a public key and a TLS
 *                  ServerKeyExchange payload.
 *
 *                  This is the second function used by a TLS server for ECDHE
 *                  ciphersuites. (It is called after mbedtls_ecdh_setup().)
 *
 * \note            This function assumes that the ECP group (grp) of the
 *                  \p ctx context has already been properly set,
 *                  for example, using mbedtls_ecp_group_load().
 *
 * \see             ecp.h
 *
 * \param ctx       The ECDH context.
 * \param olen      The number of characters written.
 * \param buf       The destination buffer.
 * \param blen      The length of the destination buffer.
 * \param f_rng     The RNG function.
 * \param p_rng     The RNG context.
 *
 * \return          \c 0 on success.
 * \return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */
int mbedtls_everest_make_params( mbedtls_ecdh_context_everest *ctx, size_t *olen,
                                 unsigned char *buf, size_t blen,
                                 int( *f_rng )( void *, unsigned char *, size_t ),
                                 void *p_rng )
{
    mbedtls_x25519_context *x25519_ctx = &ctx->ctx;
    return mbedtls_x25519_make_params( x25519_ctx, olen, buf, blen, f_rng, p_rng );
}

/**
 * \brief           This function parses and processes a TLS ServerKeyExhange
 *                  payload.
 *
 *                  This is the first function used by a TLS client for ECDHE
 *                  ciphersuites.
 *
 * \see             ecp.h
 *
 * \param ctx       The ECDH context.
 * \param buf       The pointer to the start of the input buffer.
 * \param end       The address for one Byte past the end of the buffer.
 *
 * \return          \c 0 on success.
 * \return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 *
 */
int mbedtls_everest_read_params( mbedtls_ecdh_context_everest *ctx,
                                 const unsigned char **buf,
                                 const unsigned char *end )
{
    mbedtls_x25519_context *x25519_ctx = &ctx->ctx;
    return mbedtls_x25519_read_params( x25519_ctx, buf, end );
}

/**
 * \brief           This function sets up an ECDH context from an EC key.
 *
 *                  It is used by clients and servers in place of the
 *                  ServerKeyEchange for static ECDH, and imports ECDH
 *                  parameters from the EC key information of a certificate.
 *
 * \see             ecp.h
 *
 * \param ctx       The ECDH context to set up.
 * \param key       The EC key to use.
 * \param side      Defines the source of the key: 1: Our key, or
 *                  0: The key of the peer.
 *
 * \return          \c 0 on success.
 * \return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 *
 */
int mbedtls_everest_get_params( mbedtls_ecdh_context_everest *ctx,
                                const mbedtls_ecp_keypair *key,
                                mbedtls_everest_ecdh_side side )
{
    mbedtls_x25519_context *x25519_ctx = &ctx->ctx;
    mbedtls_x25519_ecdh_side s = side == MBEDTLS_EVEREST_ECDH_OURS ?
                                            MBEDTLS_X25519_ECDH_OURS :
                                            MBEDTLS_X25519_ECDH_THEIRS;
    return mbedtls_x25519_get_params( x25519_ctx, key, s );
}

/**
 * \brief           This function generates a public key and a TLS
 *                  ClientKeyExchange payload.
 *
 *                  This is the second function used by a TLS client for ECDH(E)
 *                  ciphersuites.
 *
 * \see             ecp.h
 *
 * \param ctx       The ECDH context.
 * \param olen      The number of Bytes written.
 * \param buf       The destination buffer.
 * \param blen      The size of the destination buffer.
 * \param f_rng     The RNG function.
 * \param p_rng     The RNG context.
 *
 * \return          \c 0 on success.
 * \return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */
int mbedtls_everest_make_public( mbedtls_ecdh_context_everest *ctx, size_t *olen,
                                 unsigned char *buf, size_t blen,
                                 int( *f_rng )( void *, unsigned char *, size_t ),
                                 void *p_rng )
{
    mbedtls_x25519_context *x25519_ctx = &ctx->ctx;
    return mbedtls_x25519_make_public( x25519_ctx, olen, buf, blen, f_rng, p_rng );
}

/**
 * \brief       This function parses and processes a TLS ClientKeyExchange
 *              payload.
 *
 *              This is the third function used by a TLS server for ECDH(E)
 *              ciphersuites. (It is called after mbedtls_ecdh_setup() and
 *              mbedtls_ecdh_make_params().)
 *
 * \see         ecp.h
 *
 * \param ctx   The ECDH context.
 * \param buf   The start of the input buffer.
 * \param blen  The length of the input buffer.
 *
 * \return      \c 0 on success.
 * \return      An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */
int mbedtls_everest_read_public( mbedtls_ecdh_context_everest *ctx,
                                 const unsigned char *buf, size_t blen )
{
    mbedtls_x25519_context *x25519_ctx = &ctx->ctx;
    return mbedtls_x25519_read_public ( x25519_ctx, buf, blen );
}

/**
 * \brief           This function derives and exports the shared secret.
 *
 *                  This is the last function used by both TLS client
 *                  and servers.
 *
 * \note            If \p f_rng is not NULL, it is used to implement
 *                  countermeasures against side-channel attacks.
 *                  For more information, see mbedtls_ecp_mul().
 *
 * \see             ecp.h
 *
 * \param ctx       The ECDH context.
 * \param olen      The number of Bytes written.
 * \param buf       The destination buffer.
 * \param blen      The length of the destination buffer.
 * \param f_rng     The RNG function.
 * \param p_rng     The RNG context.
 *
 * \return          \c 0 on success.
 * \return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */
int mbedtls_everest_calc_secret( mbedtls_ecdh_context_everest *ctx, size_t *olen,
                                 unsigned char *buf, size_t blen,
                                 int( *f_rng )( void *, unsigned char *, size_t ),
                                 void *p_rng )
{
    mbedtls_x25519_context *x25519_ctx = &ctx->ctx;
    return mbedtls_x25519_calc_secret( x25519_ctx, olen, buf, blen, f_rng, p_rng );
}

#endif /* MBEDTLS_ECDH_C && MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED */
