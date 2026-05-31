/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#ifndef MLD_NATIVE_X86_64_META_H
#define MLD_NATIVE_X86_64_META_H

/* Identifier for this backend so that source and assembly files
 * in the build can be appropriately guarded. */
#define MLD_ARITH_BACKEND_X86_64_DEFAULT

#define MLD_USE_NATIVE_NTT_CUSTOM_ORDER
#define MLD_USE_NATIVE_NTT
#define MLD_USE_NATIVE_INTT
#define MLD_USE_NATIVE_REJ_UNIFORM
#define MLD_USE_NATIVE_REJ_UNIFORM_ETA2
#define MLD_USE_NATIVE_REJ_UNIFORM_ETA4
#define MLD_USE_NATIVE_POLY_DECOMPOSE_32
#define MLD_USE_NATIVE_POLY_DECOMPOSE_88
#define MLD_USE_NATIVE_POLY_CADDQ
#define MLD_USE_NATIVE_POLY_USE_HINT_32
#define MLD_USE_NATIVE_POLY_USE_HINT_88
#define MLD_USE_NATIVE_POLY_CHKNORM
#define MLD_USE_NATIVE_POLYZ_UNPACK_17
#define MLD_USE_NATIVE_POLYZ_UNPACK_19
#define MLD_USE_NATIVE_POINTWISE_MONTGOMERY
#define MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4
#define MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5
#define MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7

#if !defined(__ASSEMBLER__)
#include <string.h>
#include "../../common.h"
#include "../api.h"
#include "src/arith_native_x86_64.h"

static MLD_INLINE void mld_poly_permute_bitrev_to_custom(int32_t data[MLDSA_N])
{
  if (mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    mld_nttunpack_avx2(data);
  }
}

static MLD_INLINE int mld_ntt_native(int32_t data[MLDSA_N])
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }

  mld_ntt_avx2(data, mld_qdata);
  return MLD_NATIVE_FUNC_SUCCESS;
}
static MLD_INLINE int mld_intt_native(int32_t data[MLDSA_N])
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_invntt_avx2(data, mld_qdata);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_rej_uniform_native(int32_t *r, unsigned len,
                                             const uint8_t *buf,
                                             unsigned buflen)
{
  /* AVX2 implementation assumes specific buffer lengths */
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2) || len != MLDSA_N ||
      buflen != MLD_AVX2_REJ_UNIFORM_BUFLEN)
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }

  /* Safety: outlen is at most MLDSA_N and, hence, this cast is safe. */
  return (int)mld_rej_uniform_avx2(r, buf);
}

static MLD_INLINE int mld_rej_uniform_eta2_native(int32_t *r, unsigned len,
                                                  const uint8_t *buf,
                                                  unsigned buflen)
{
  unsigned int outlen;
  /* AVX2 implementation assumes specific buffer lengths */
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2) || len != MLDSA_N ||
      buflen != MLD_AVX2_REJ_UNIFORM_ETA2_BUFLEN)
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }

  /* Constant time: Inputs and outputs to this function are secret.
   * It is safe to leak which coefficients are accepted/rejected.
   * The assembly implementation must not leak any other information about the
   * accepted coefficients. Constant-time testing cannot cover this, and we
   * hence have to manually verify the assembly.
   * We declassify prior the input data and mark the outputs as secret.
   */
  MLD_CT_TESTING_DECLASSIFY(buf, buflen);
  outlen = mld_rej_uniform_eta2_avx2(r, buf);
  MLD_CT_TESTING_SECRET(r, sizeof(int32_t) * outlen);
  /* Safety: outlen is at most MLDSA_N and, hence, this cast is safe. */
  return (int)outlen;
}

static MLD_INLINE int mld_rej_uniform_eta4_native(int32_t *r, unsigned len,
                                                  const uint8_t *buf,
                                                  unsigned buflen)
{
  unsigned int outlen;
  /* AVX2 implementation assumes specific buffer lengths */
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2) || len != MLDSA_N ||
      buflen != MLD_AVX2_REJ_UNIFORM_ETA4_BUFLEN)
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }

  /* Constant time: Inputs and outputs to this function are secret.
   * It is safe to leak which coefficients are accepted/rejected.
   * The assembly implementation must not leak any other information about the
   * accepted coefficients. Constant-time testing cannot cover this, and we
   * hence have to manually verify the assembly.
   * We declassify prior the input data and mark the outputs as secret.
   */
  MLD_CT_TESTING_DECLASSIFY(buf, buflen);
  outlen = mld_rej_uniform_eta4_avx2(r, buf);
  MLD_CT_TESTING_SECRET(r, sizeof(int32_t) * outlen);
  /* Safety: outlen is at most MLDSA_N and, hence, this cast is safe. */
  return (int)outlen;
}

static MLD_INLINE int mld_poly_decompose_32_native(int32_t *a1, int32_t *a0)
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_poly_decompose_32_avx2(a1, a0);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_poly_decompose_88_native(int32_t *a1, int32_t *a0)
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_poly_decompose_88_avx2(a1, a0);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_poly_caddq_native(int32_t a[MLDSA_N])
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_poly_caddq_avx2(a);
  return MLD_NATIVE_FUNC_SUCCESS;
}
static MLD_INLINE int mld_poly_use_hint_32_native(int32_t *b, const int32_t *a,
                                                  const int32_t *h)
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_poly_use_hint_32_avx2(b, a, h);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_poly_use_hint_88_native(int32_t *b, const int32_t *a,
                                                  const int32_t *h)
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_poly_use_hint_88_avx2(b, a, h);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_poly_chknorm_native(const int32_t *a, int32_t B)
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  return mld_poly_chknorm_avx2(a, B);
}

static MLD_INLINE int mld_polyz_unpack_17_native(int32_t *r, const uint8_t *a)
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_polyz_unpack_17_avx2(r, a);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_polyz_unpack_19_native(int32_t *r, const uint8_t *a)
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_polyz_unpack_19_avx2(r, a);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_poly_pointwise_montgomery_native(
    int32_t c[MLDSA_N], const int32_t a[MLDSA_N], const int32_t b[MLDSA_N])
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_pointwise_avx2(c, a, b, mld_qdata);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_polyvecl_pointwise_acc_montgomery_l4_native(
    int32_t w[MLDSA_N], const int32_t u[4][MLDSA_N],
    const int32_t v[4][MLDSA_N])
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_pointwise_acc_l4_avx2(w, u, v, mld_qdata);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_polyvecl_pointwise_acc_montgomery_l5_native(
    int32_t w[MLDSA_N], const int32_t u[5][MLDSA_N],
    const int32_t v[5][MLDSA_N])
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_pointwise_acc_l5_avx2(w, u, v, mld_qdata);
  return MLD_NATIVE_FUNC_SUCCESS;
}

static MLD_INLINE int mld_polyvecl_pointwise_acc_montgomery_l7_native(
    int32_t w[MLDSA_N], const int32_t u[7][MLDSA_N],
    const int32_t v[7][MLDSA_N])
{
  if (!mld_sys_check_capability(MLD_SYS_CAP_AVX2))
  {
    return MLD_NATIVE_FUNC_FALLBACK;
  }
  mld_pointwise_acc_l7_avx2(w, u, v, mld_qdata);
  return MLD_NATIVE_FUNC_SUCCESS;
}

#endif /* !__ASSEMBLER__ */

#endif /* !MLD_NATIVE_X86_64_META_H */
