/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_NATIVE_X86_64_SRC_ARITH_NATIVE_X86_64_H
#define MLD_NATIVE_X86_64_SRC_ARITH_NATIVE_X86_64_H
#include "../../../common.h"

#include <stdint.h>
#include "consts.h"

#define MLD_AVX2_REJ_UNIFORM_BUFLEN \
  (5 * 168) /* REJ_UNIFORM_NBLOCKS * SHAKE128_RATE */


/*
 * Sampling 256 coefficients mod 15 using rejection sampling from 4 bits.
 * Expected number of required bytes: (256 * (16/15))/2 = 136.5 bytes.
 * We sample 1 block (=136 bytes) of SHAKE256_RATE output initially.
 * Sampling 2 blocks initially results in slightly worse performance.
 */
#define MLD_AVX2_REJ_UNIFORM_ETA2_BUFLEN (1 * 136)

/*
 * Sampling 256 coefficients mod 9 using rejection sampling from 4 bits.
 * Expected number of required bytes: (256 * (16/9))/2 = 227.5 bytes.
 * We sample 2 blocks (=272 bytes) of SHAKE256_RATE output initially.
 */
#define MLD_AVX2_REJ_UNIFORM_ETA4_BUFLEN (2 * 136)

#define mld_rej_uniform_table MLD_NAMESPACE(mld_rej_uniform_table)
extern const uint8_t mld_rej_uniform_table[256][8];

#define mld_ntt_avx2 MLD_NAMESPACE(ntt_avx2)
void mld_ntt_avx2(int32_t *r, const int32_t *qdata)
/* This must be kept in sync with the HOL-Light specification
 * in proofs/hol_light/x86_64/proofs/mldsa_ntt.ml */
__contract__(
  requires(memory_no_alias(r, sizeof(int32_t) * MLDSA_N))
  requires(array_abs_bound(r, 0, MLDSA_N, 8380417))
  requires(qdata == mld_qdata)
  assigns(memory_slice(r, sizeof(int32_t) * MLDSA_N))
  /* check-magic: off */
  ensures(array_abs_bound(r, 0, MLDSA_N, 42035262))
  /* check-magic: on */
);

#define mld_invntt_avx2 MLD_NAMESPACE(invntt_avx2)
void mld_invntt_avx2(int32_t *r, const int32_t *mld_qdata);

#define mld_nttunpack_avx2 MLD_NAMESPACE(nttunpack_avx2)
void mld_nttunpack_avx2(int32_t *r);

#define mld_rej_uniform_avx2 MLD_NAMESPACE(mld_rej_uniform_avx2)
unsigned mld_rej_uniform_avx2(int32_t *r,
                              const uint8_t buf[MLD_AVX2_REJ_UNIFORM_BUFLEN]);

#define mld_rej_uniform_eta2_avx2 MLD_NAMESPACE(mld_rej_uniform_eta2_avx2)
unsigned mld_rej_uniform_eta2_avx2(
    int32_t *r, const uint8_t buf[MLD_AVX2_REJ_UNIFORM_ETA2_BUFLEN]);

#define mld_rej_uniform_eta4_avx2 MLD_NAMESPACE(mld_rej_uniform_eta4_avx2)
unsigned mld_rej_uniform_eta4_avx2(
    int32_t *r, const uint8_t buf[MLD_AVX2_REJ_UNIFORM_ETA4_BUFLEN]);

#define mld_poly_decompose_32_avx2 MLD_NAMESPACE(mld_poly_decompose_32_avx2)
void mld_poly_decompose_32_avx2(int32_t *a1, int32_t *a0);

#define mld_poly_decompose_88_avx2 MLD_NAMESPACE(mld_poly_decompose_88_avx2)
void mld_poly_decompose_88_avx2(int32_t *a1, int32_t *a0);

#define mld_poly_caddq_avx2 MLD_NAMESPACE(poly_caddq_avx2)
void mld_poly_caddq_avx2(int32_t *r);

#define mld_poly_use_hint_32_avx2 MLD_NAMESPACE(mld_poly_use_hint_32_avx2)
void mld_poly_use_hint_32_avx2(int32_t *b, const int32_t *a, const int32_t *h);

#define mld_poly_use_hint_88_avx2 MLD_NAMESPACE(mld_poly_use_hint_88_avx2)
void mld_poly_use_hint_88_avx2(int32_t *b, const int32_t *a, const int32_t *h);

#define mld_poly_chknorm_avx2 MLD_NAMESPACE(mld_poly_chknorm_avx2)
int mld_poly_chknorm_avx2(const int32_t *a, int32_t B);

#define mld_polyz_unpack_17_avx2 MLD_NAMESPACE(mld_polyz_unpack_17_avx2)
void mld_polyz_unpack_17_avx2(int32_t *r, const uint8_t *a);

#define mld_polyz_unpack_19_avx2 MLD_NAMESPACE(mld_polyz_unpack_19_avx2)
void mld_polyz_unpack_19_avx2(int32_t *r, const uint8_t *a);

#define mld_pointwise_avx2 MLD_NAMESPACE(pointwise_avx2)
void mld_pointwise_avx2(int32_t *c, const int32_t *a, const int32_t *b,
                        const int32_t *qdata);

#define mld_pointwise_acc_l4_avx2 MLD_NAMESPACE(pointwise_acc_l4_avx2)
void mld_pointwise_acc_l4_avx2(int32_t c[MLDSA_N], const int32_t a[4][MLDSA_N],
                               const int32_t b[4][MLDSA_N],
                               const int32_t *qdata);

#define mld_pointwise_acc_l5_avx2 MLD_NAMESPACE(pointwise_acc_l5_avx2)
void mld_pointwise_acc_l5_avx2(int32_t c[MLDSA_N], const int32_t a[5][MLDSA_N],
                               const int32_t b[5][MLDSA_N],
                               const int32_t *qdata);

#define mld_pointwise_acc_l7_avx2 MLD_NAMESPACE(pointwise_acc_l7_avx2)
void mld_pointwise_acc_l7_avx2(int32_t c[MLDSA_N], const int32_t a[7][MLDSA_N],
                               const int32_t b[7][MLDSA_N],
                               const int32_t *qdata);

#endif /* !MLD_NATIVE_X86_64_SRC_ARITH_NATIVE_X86_64_H */
