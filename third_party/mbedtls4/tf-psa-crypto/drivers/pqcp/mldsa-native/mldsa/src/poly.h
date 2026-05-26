/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_POLY_H
#define MLD_POLY_H

#include <stdint.h>
#include "cbmc.h"
#include "common.h"
#include "reduce.h"
#include "rounding.h"

/* Absolute exclusive upper bound for the output of the forward NTT */
#define MLD_NTT_BOUND (9 * MLDSA_Q)
/* Absolute exclusive upper bound for the output of the inverse NTT*/
#define MLD_INTT_BOUND MLDSA_Q

typedef struct
{
  int32_t coeffs[MLDSA_N];
} MLD_ALIGN mld_poly;

#define mld_poly_reduce MLD_NAMESPACE(poly_reduce)
/*************************************************
 * Name:        mld_poly_reduce
 *
 * Description: Inplace reduction of all coefficients of polynomial to
 *              representative in
 *[-MLD_REDUCE32_RANGE_MAX,MLD_REDUCE32_RANGE_MAX].
 *
 * Arguments:   - mld_poly *a: pointer to input/output polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_poly_reduce(mld_poly *a)
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_bound(a->coeffs, 0, MLDSA_N, INT32_MIN, MLD_REDUCE32_DOMAIN_MAX))
  assigns(memory_slice(a, sizeof(mld_poly)))
  ensures(array_bound(a->coeffs, 0, MLDSA_N, -MLD_REDUCE32_RANGE_MAX, MLD_REDUCE32_RANGE_MAX))
);

#define mld_poly_caddq MLD_NAMESPACE(poly_caddq)
/*************************************************
 * Name:        mld_poly_caddq
 *
 * Description: For all coefficients of in/out polynomial add MLDSA_Q if
 *              coefficient is negative.
 *
 * Arguments:   - mld_poly *a: pointer to input/output polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_poly_caddq(mld_poly *a)
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_abs_bound(a->coeffs, 0, MLDSA_N, MLDSA_Q))
  assigns(memory_slice(a, sizeof(mld_poly)))
  ensures(array_bound(a->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
);

#define mld_poly_add MLD_NAMESPACE(poly_add)
/*************************************************
 * Name:        mld_poly_add
 *
 * Description: Add polynomials. No modular reduction is performed.
 *
 * Arguments: - r: Pointer to input-output polynomial to be added to.
 *            - b: Pointer to input polynomial that should be added
 *                 to r. Must be disjoint from r.
 **************************************************/

/*
 * NOTE: The reference implementation uses a 3-argument poly_add.
 * We specialize to the accumulator form to avoid reasoning about aliasing.
 */
MLD_INTERNAL_API
void mld_poly_add(mld_poly *r, const mld_poly *b)
__contract__(
  requires(memory_no_alias(b, sizeof(mld_poly)))
  requires(memory_no_alias(r, sizeof(mld_poly)))
  requires(forall(k0, 0, MLDSA_N, (int64_t) r->coeffs[k0] + b->coeffs[k0] < MLD_REDUCE32_DOMAIN_MAX))
  requires(forall(k1, 0, MLDSA_N, (int64_t) r->coeffs[k1] + b->coeffs[k1] >= INT32_MIN))
  assigns(memory_slice(r, sizeof(mld_poly)))
  ensures(forall(k2, 0, MLDSA_N, r->coeffs[k2] == old(*r).coeffs[k2] + b->coeffs[k2]))
  ensures(forall(k3, 0, MLDSA_N, r->coeffs[k3] < MLD_REDUCE32_DOMAIN_MAX))
  ensures(forall(k4, 0, MLDSA_N, r->coeffs[k4] >= INT32_MIN))
);

#define mld_poly_sub MLD_NAMESPACE(poly_sub)
/*************************************************
 * Name:        mld_poly_sub
 *
 * Description: Subtract polynomials. No modular reduction is
 *              performed.
 *
 * Arguments:   - mld_poly *r: Pointer to input-output polynomial.
 *              - const mld_poly *b: Pointer to input polynomial that should be
 *                               subtracted from r. Must be disjoint from r.
 **************************************************/
/*
 * NOTE: The reference implementation uses a 3-argument poly_sub.
 * We specialize to the accumulator form to avoid reasoning about aliasing.
 */
MLD_INTERNAL_API
void mld_poly_sub(mld_poly *r, const mld_poly *b)
__contract__(
  requires(memory_no_alias(b, sizeof(mld_poly)))
  requires(memory_no_alias(r, sizeof(mld_poly)))
  requires(array_abs_bound(r->coeffs, 0, MLDSA_N, MLDSA_Q))
  requires(array_abs_bound(b->coeffs, 0, MLDSA_N, MLDSA_Q))
  assigns(memory_slice(r, sizeof(mld_poly)))
  ensures(array_bound(r->coeffs, 0, MLDSA_N, INT32_MIN, MLD_REDUCE32_DOMAIN_MAX))
);

#define mld_poly_shiftl MLD_NAMESPACE(poly_shiftl)
/*************************************************
 * Name:        mld_poly_shiftl
 *
 * Description: Multiply polynomial by 2^MLDSA_D without modular reduction.
 *Assumes input coefficients to be less than 2^{31-MLDSA_D} in absolute value.
 *
 * Arguments:   - mld_poly *a: pointer to input/output polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_poly_shiftl(mld_poly *a)
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_bound(a->coeffs, 0, MLDSA_N, 0, 1 << 10))
  assigns(memory_slice(a, sizeof(mld_poly)))
  ensures(array_bound(a->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
);

#define mld_poly_ntt MLD_NAMESPACE(poly_ntt)
/*************************************************
 * Name:        mld_poly_ntt
 *
 * Description: Inplace forward NTT. Coefficients can grow by
 *              8*MLDSA_Q in absolute value.
 *
 * Arguments:   - mld_poly *a: pointer to input/output polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_poly_ntt(mld_poly *a)
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_abs_bound(a->coeffs, 0, MLDSA_N, MLDSA_Q))
  assigns(memory_slice(a, sizeof(mld_poly)))
  ensures(array_abs_bound(a->coeffs, 0, MLDSA_N, MLD_NTT_BOUND))
);


#define mld_poly_invntt_tomont MLD_NAMESPACE(poly_invntt_tomont)
/*************************************************
 * Name:        mld_poly_invntt_tomont
 *
 * Description: Inplace inverse NTT and multiplication by 2^{32}.
 *              Input coefficients need to be less than MLDSA_Q in absolute
 *              value and output coefficients are bounded by
 *              MLD_INTT_BOUND.
 *
 * Arguments:   - mld_poly *a: pointer to input/output polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_poly_invntt_tomont(mld_poly *a)
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_abs_bound(a->coeffs, 0, MLDSA_N, MLDSA_Q))
  assigns(memory_slice(a, sizeof(mld_poly)))
  ensures(array_abs_bound(a->coeffs, 0, MLDSA_N, MLD_INTT_BOUND))
);

#define mld_poly_pointwise_montgomery MLD_NAMESPACE(poly_pointwise_montgomery)
/*************************************************
 * Name:        mld_poly_pointwise_montgomery
 *
 * Description: Pointwise multiplication of polynomials in NTT domain
 *              representation and multiplication of resulting polynomial
 *              by 2^{-32}.
 *
 * Arguments:   - mld_poly *c: pointer to output polynomial
 *              - const mld_poly *a: pointer to first input polynomial
 *              - const mld_poly *b: pointer to second input polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_poly_pointwise_montgomery(mld_poly *c, const mld_poly *a,
                                   const mld_poly *b)
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(memory_no_alias(b, sizeof(mld_poly)))
  requires(memory_no_alias(c, sizeof(mld_poly)))
  requires(array_abs_bound(a->coeffs, 0, MLDSA_N, MLD_NTT_BOUND))
  requires(array_abs_bound(b->coeffs, 0, MLDSA_N, MLD_NTT_BOUND))
  assigns(memory_slice(c, sizeof(mld_poly)))
  ensures(array_abs_bound(c->coeffs, 0, MLDSA_N, MLDSA_Q))
);

#define mld_poly_power2round MLD_NAMESPACE(poly_power2round)
/*************************************************
 * Name:        mld_poly_power2round
 *
 * Description: For all coefficients c of the input polynomial,
 *              compute c0, c1 such that c mod MLDSA_Q = c1*2^MLDSA_D + c0
 *              with -2^{MLDSA_D-1} < c0 <= 2^{MLDSA_D-1}. Assumes coefficients
 *to be standard representatives.
 *
 * Arguments:   - mld_poly *a1: pointer to output polynomial with coefficients
 *c1
 *              - mld_poly *a0: pointer to output polynomial with coefficients
 *c0
 *              - const mld_poly *a: pointer to input polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_poly_power2round(mld_poly *a1, mld_poly *a0, const mld_poly *a)
__contract__(
  requires(memory_no_alias(a0, sizeof(mld_poly)))
  requires(memory_no_alias(a1, sizeof(mld_poly)))
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_bound(a->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
  assigns(memory_slice(a1, sizeof(mld_poly)))
  assigns(memory_slice(a0, sizeof(mld_poly)))
  ensures(array_bound(a0->coeffs, 0, MLDSA_N, -(MLD_2_POW_D/2)+1, (MLD_2_POW_D/2)+1))
  ensures(array_bound(a1->coeffs, 0, MLDSA_N, 0, ((MLDSA_Q - 1) / MLD_2_POW_D) + 1))
);

#define mld_poly_uniform MLD_NAMESPACE(poly_uniform)
/*************************************************
 * Name:        mld_poly_uniform
 *
 * Description: Sample polynomial with uniformly random coefficients
 *              in [0,MLDSA_Q-1] by performing rejection sampling on the
 *              output stream of SHAKE128(seed|nonce)
 *
 * Arguments:   - mld_poly *a: pointer to output polynomial
 *              - const uint8_t seed[]: byte array with seed of length
 *                MLDSA_SEEDBYTES and the packed 2-byte nonce
 **************************************************/
MLD_INTERNAL_API
void mld_poly_uniform(mld_poly *a, const uint8_t seed[MLDSA_SEEDBYTES + 2])
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(memory_no_alias(seed, MLDSA_SEEDBYTES + 2))
  assigns(memory_slice(a, sizeof(mld_poly)))
  ensures(array_bound(a->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
);

#if !defined(MLD_CONFIG_SERIAL_FIPS202_ONLY) && !defined(MLD_CONFIG_REDUCE_RAM)
#define mld_poly_uniform_4x MLD_NAMESPACE(poly_uniform_4x)
/*************************************************
 * Name:        mld_poly_uniform_x4
 *
 * Description: Generate four polynomials using rejection sampling
 *              on (pseudo-)uniformly random bytes sampled from a seed.
 *
 * Arguments:   - mld_poly *vec0, *vec1, *vec2, *vec3:
 *                Pointers to 4 polynomials to be sampled.
 *              - uint8_t seed[4][MLD_ALIGN_UP(MLDSA_SEEDBYTES + 2)]:
 *                Pointer consecutive array of seed buffers of size
 *                MLDSA_SEEDBYTES + 2 each, plus padding for alignment.
 *
 **************************************************/
MLD_INTERNAL_API
void mld_poly_uniform_4x(mld_poly *vec0, mld_poly *vec1, mld_poly *vec2,
                         mld_poly *vec3,
                         uint8_t seed[4][MLD_ALIGN_UP(MLDSA_SEEDBYTES + 2)])
__contract__(
  requires(memory_no_alias(vec0, sizeof(mld_poly)))
  requires(memory_no_alias(vec1, sizeof(mld_poly)))
  requires(memory_no_alias(vec2, sizeof(mld_poly)))
  requires(memory_no_alias(vec3, sizeof(mld_poly)))
  requires(memory_no_alias(seed,  4 * MLD_ALIGN_UP(MLDSA_SEEDBYTES + 2)))
  assigns(memory_slice(vec0, sizeof(mld_poly)))
  assigns(memory_slice(vec1, sizeof(mld_poly)))
  assigns(memory_slice(vec2, sizeof(mld_poly)))
  assigns(memory_slice(vec3, sizeof(mld_poly)))
  ensures(array_bound(vec0->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
  ensures(array_bound(vec1->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
  ensures(array_bound(vec2->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
  ensures(array_bound(vec3->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
);
#endif /* !MLD_CONFIG_SERIAL_FIPS202_ONLY && !MLD_CONFIG_REDUCE_RAM */

#define mld_polyt1_pack MLD_NAMESPACE(polyt1_pack)
/*************************************************
 * Name:        mld_polyt1_pack
 *
 * Description: Bit-pack polynomial t1 with coefficients fitting in 10 bits.
 *              Input coefficients are assumed to be standard representatives.
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with at least
 *                            MLDSA_POLYT1_PACKEDBYTES bytes
 *              - const mld_poly *a: pointer to input polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyt1_pack(uint8_t r[MLDSA_POLYT1_PACKEDBYTES], const mld_poly *a)
__contract__(
  requires(memory_no_alias(r, MLDSA_POLYT1_PACKEDBYTES))
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_bound(a->coeffs, 0, MLDSA_N, 0, 1 << 10))
  assigns(memory_slice(r, MLDSA_POLYT1_PACKEDBYTES))
);

#define mld_polyt1_unpack MLD_NAMESPACE(polyt1_unpack)
/*************************************************
 * Name:        mld_polyt1_unpack
 *
 * Description: Unpack polynomial t1 with 10-bit coefficients.
 *              Output coefficients are standard representatives.
 *
 * Arguments:   - mld_poly *r: pointer to output polynomial
 *              - const uint8_t *a: byte array with bit-packed polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyt1_unpack(mld_poly *r, const uint8_t a[MLDSA_POLYT1_PACKEDBYTES])
__contract__(
  requires(memory_no_alias(r, sizeof(mld_poly)))
  requires(memory_no_alias(a, MLDSA_POLYT1_PACKEDBYTES))
  assigns(memory_slice(r, sizeof(mld_poly)))
  ensures(array_bound(r->coeffs, 0, MLDSA_N, 0, 1 << 10))
);

#define mld_polyt0_pack MLD_NAMESPACE(polyt0_pack)
/*************************************************
 * Name:        mld_polyt0_pack
 *
 * Description: Bit-pack polynomial t0 with coefficients in ]-2^{MLDSA_D-1},
 *              2^{MLDSA_D-1}].
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with at least
 *                            MLDSA_POLYT0_PACKEDBYTES bytes
 *              - const mld_poly *a: pointer to input polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyt0_pack(uint8_t r[MLDSA_POLYT0_PACKEDBYTES], const mld_poly *a)
__contract__(
  requires(memory_no_alias(r, MLDSA_POLYT0_PACKEDBYTES))
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_bound(a->coeffs, 0, MLDSA_N, -(1<<(MLDSA_D-1)) + 1, (1<<(MLDSA_D-1)) + 1))
  assigns(memory_slice(r, MLDSA_POLYT0_PACKEDBYTES))
);


#define mld_polyt0_unpack MLD_NAMESPACE(polyt0_unpack)
/*************************************************
 * Name:        mld_polyt0_unpack
 *
 * Description: Unpack polynomial t0 with coefficients in ]-2^{MLDSA_D-1},
 *2^{MLDSA_D-1}].
 *
 * Arguments:   - mld_poly *r: pointer to output polynomial
 *              - const uint8_t *a: byte array with bit-packed polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyt0_unpack(mld_poly *r, const uint8_t a[MLDSA_POLYT0_PACKEDBYTES])
__contract__(
  requires(memory_no_alias(r, sizeof(mld_poly)))
  requires(memory_no_alias(a, MLDSA_POLYT0_PACKEDBYTES))
  assigns(memory_slice(r, sizeof(mld_poly)))
  ensures(array_bound(r->coeffs, 0, MLDSA_N, -(1<<(MLDSA_D-1)) + 1, (1<<(MLDSA_D-1)) + 1))
);

#define mld_poly_chknorm MLD_NAMESPACE(poly_chknorm)
/*************************************************
 * Name:        mld_poly_chknorm
 *
 * Description: Check infinity norm of polynomial against given bound.
 *              Assumes input coefficients were reduced by mld_reduce32().
 *
 * Arguments:   - const mld_poly *a: pointer to polynomial
 *              - int32_t B: norm bound
 *
 * Returns 0 if norm is strictly smaller than
 * B <= (MLDSA_Q - MLD_REDUCE32_RANGE_MAX) and 0xFFFFFFFF otherwise.
 *
 * Specification: The definition of this FIPS-204 requires signed canonical
 *                reduction prior to applying the bounds check.
 *                However, `-B < (a mod± MLDSA_Q) < B` is equivalent to
 *                `-B < a < B` under the assumption that
 *                `B <= MLDSA_Q - MLD_REDUCE32_RANGE_MAX` (cf. the assertion in
 *                the code). Hence, the present spec and implementation are
 *                correct without reduction.
 *
 **************************************************/
MLD_INTERNAL_API
uint32_t mld_poly_chknorm(const mld_poly *a, int32_t B)
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(0 <= B && B <= MLDSA_Q - MLD_REDUCE32_RANGE_MAX)
  requires(array_bound(a->coeffs, 0, MLDSA_N, -MLD_REDUCE32_RANGE_MAX, MLD_REDUCE32_RANGE_MAX))
  ensures(return_value == 0 || return_value == 0xFFFFFFFF)
  ensures((return_value == 0) == array_abs_bound(a->coeffs, 0, MLDSA_N, B))
);

#endif /* !MLD_POLY_H */
