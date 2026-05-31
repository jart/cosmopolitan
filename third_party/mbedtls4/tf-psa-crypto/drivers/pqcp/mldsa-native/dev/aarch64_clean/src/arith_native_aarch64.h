/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#ifndef MLD_NATIVE_AARCH64_SRC_ARITH_NATIVE_AARCH64_H
#define MLD_NATIVE_AARCH64_SRC_ARITH_NATIVE_AARCH64_H

#include <stdint.h>
#include "../../../common.h"

#define mld_aarch64_ntt_zetas_layer123456 \
  MLD_NAMESPACE(aarch64_ntt_zetas_layer123456)
#define mld_aarch64_ntt_zetas_layer78 MLD_NAMESPACE(aarch64_ntt_zetas_layer78)

#define mld_aarch64_intt_zetas_layer78 MLD_NAMESPACE(aarch64_intt_zetas_layer78)
#define mld_aarch64_intt_zetas_layer123456 \
  MLD_NAMESPACE(aarch64_intt_zetas_layer123456)

extern const int32_t mld_aarch64_ntt_zetas_layer123456[];
extern const int32_t mld_aarch64_ntt_zetas_layer78[];

extern const int32_t mld_aarch64_intt_zetas_layer78[];
extern const int32_t mld_aarch64_intt_zetas_layer123456[];

#define mld_rej_uniform_table MLD_NAMESPACE(rej_uniform_table)
extern const uint8_t mld_rej_uniform_table[];
#define mld_rej_uniform_eta_table MLD_NAMESPACE(rej_uniform_eta_table)
extern const uint8_t mld_rej_uniform_eta_table[];

#define mld_polyz_unpack_17_indices MLD_NAMESPACE(polyz_unpack_17_indices)
extern const uint8_t mld_polyz_unpack_17_indices[];
#define mld_polyz_unpack_19_indices MLD_NAMESPACE(polyz_unpack_19_indices)
extern const uint8_t mld_polyz_unpack_19_indices[];


/*
 * Sampling 256 coefficients mod 15 using rejection sampling from 4 bits.
 * Expected number of required bytes: (256 * (16/15))/2 = 136.5 bytes.
 * We sample 1 block (=136 bytes) of SHAKE256_RATE output initially.
 * Sampling 2 blocks initially results in slightly worse performance.
 */
#define MLD_AARCH64_REJ_UNIFORM_ETA2_BUFLEN (1 * 136)
/*
 * Sampling 256 coefficients mod 9 using rejection sampling from 4 bits.
 * Expected number of required bytes: (256 * (16/9))/2 = 227.5 bytes.
 * We sample 2 blocks (=272 bytes) of SHAKE256_RATE output initially.
 */
#define MLD_AARCH64_REJ_UNIFORM_ETA4_BUFLEN (2 * 136)

#define mld_ntt_asm MLD_NAMESPACE(ntt_asm)
void mld_ntt_asm(int32_t *, const int32_t *, const int32_t *);

#define mld_intt_asm MLD_NAMESPACE(intt_asm)
void mld_intt_asm(int32_t *, const int32_t *, const int32_t *);

#define mld_rej_uniform_asm MLD_NAMESPACE(rej_uniform_asm)
uint64_t mld_rej_uniform_asm(int32_t *r, const uint8_t *buf, unsigned buflen,
                             const uint8_t *table);

#define mld_rej_uniform_eta2_asm MLD_NAMESPACE(rej_uniform_eta2_asm)
uint64_t mld_rej_uniform_eta2_asm(int32_t *r, const uint8_t *buf,
                                  unsigned buflen, const uint8_t *table);

#define mld_rej_uniform_eta4_asm MLD_NAMESPACE(rej_uniform_eta4_asm)
uint64_t mld_rej_uniform_eta4_asm(int32_t *r, const uint8_t *buf,
                                  unsigned buflen, const uint8_t *table);

#define mld_poly_decompose_32_asm MLD_NAMESPACE(poly_decompose_32_asm)
void mld_poly_decompose_32_asm(int32_t *a1, int32_t *a0);

#define mld_poly_decompose_88_asm MLD_NAMESPACE(poly_decompose_88_asm)
void mld_poly_decompose_88_asm(int32_t *a1, int32_t *a0);

#define mld_poly_caddq_asm MLD_NAMESPACE(poly_caddq_asm)
void mld_poly_caddq_asm(int32_t *a);

#define mld_poly_use_hint_32_asm MLD_NAMESPACE(poly_use_hint_32_asm)
void mld_poly_use_hint_32_asm(int32_t *b, const int32_t *a, const int32_t *h);

#define mld_poly_use_hint_88_asm MLD_NAMESPACE(poly_use_hint_88_asm)
void mld_poly_use_hint_88_asm(int32_t *b, const int32_t *a, const int32_t *h);

#define mld_poly_chknorm_asm MLD_NAMESPACE(poly_chknorm_asm)
int mld_poly_chknorm_asm(const int32_t *a, int32_t B);

#define mld_polyz_unpack_17_asm MLD_NAMESPACE(polyz_unpack_17_asm)
void mld_polyz_unpack_17_asm(int32_t *r, const uint8_t *buf,
                             const uint8_t *indices);

#define mld_polyz_unpack_19_asm MLD_NAMESPACE(polyz_unpack_19_asm)
void mld_polyz_unpack_19_asm(int32_t *r, const uint8_t *buf,
                             const uint8_t *indices);

#define mld_poly_pointwise_montgomery_asm \
  MLD_NAMESPACE(poly_pointwise_montgomery_asm)
void mld_poly_pointwise_montgomery_asm(int32_t *, const int32_t *,
                                       const int32_t *);

#define mld_polyvecl_pointwise_acc_montgomery_l4_asm \
  MLD_NAMESPACE(polyvecl_pointwise_acc_montgomery_l4_asm)
void mld_polyvecl_pointwise_acc_montgomery_l4_asm(int32_t *, const int32_t *,
                                                  const int32_t *);

#define mld_polyvecl_pointwise_acc_montgomery_l5_asm \
  MLD_NAMESPACE(polyvecl_pointwise_acc_montgomery_l5_asm)
void mld_polyvecl_pointwise_acc_montgomery_l5_asm(int32_t *, const int32_t *,
                                                  const int32_t *);

#define mld_polyvecl_pointwise_acc_montgomery_l7_asm \
  MLD_NAMESPACE(polyvecl_pointwise_acc_montgomery_l7_asm)
void mld_polyvecl_pointwise_acc_montgomery_l7_asm(int32_t *, const int32_t *,
                                                  const int32_t *);

#endif /* !MLD_NATIVE_AARCH64_SRC_ARITH_NATIVE_AARCH64_H */
