/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_POLYVEC_H
#define MLD_POLYVEC_H

#include <stdint.h>
#include "cbmc.h"
#include "common.h"
#include "poly.h"
#include "poly_kl.h"

/* Parameter set namespacing
 * This is to facilitate building multiple instances
 * of mldsa-native (e.g. with varying parameter sets)
 * within a single compilation unit. */
#define mld_polyvecl MLD_ADD_PARAM_SET(mld_polyvecl)
#define mld_polyveck MLD_ADD_PARAM_SET(mld_polyveck)
#define mld_polymat MLD_ADD_PARAM_SET(mld_polymat)
/* End of parameter set namespacing */

/* Vectors of polynomials of length MLDSA_L */
typedef struct
{
  mld_poly vec[MLDSA_L];
} mld_polyvecl;


#define mld_polyvecl_uniform_gamma1 MLD_NAMESPACE_KL(polyvecl_uniform_gamma1)
/*************************************************
 * Name:        mld_polyvecl_uniform_gamma1
 *
 * Description: Sample vector of polynomials with uniformly random coefficients
 *              in [-(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1] by unpacking output
 *              stream of SHAKE256(seed|nonce)
 *
 * Arguments:   - mld_polyvecl *v: pointer to output vector
 *              - const uint8_t seed[]: byte array with seed of length
 *                MLDSA_CRHBYTES
 *              - uint16_t nonce: 16-bit nonce
 *************************************************/
MLD_INTERNAL_API
void mld_polyvecl_uniform_gamma1(mld_polyvecl *v,
                                 const uint8_t seed[MLDSA_CRHBYTES],
                                 uint16_t nonce)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyvecl)))
  requires(memory_no_alias(seed, MLDSA_CRHBYTES))
  requires(nonce <= (UINT16_MAX - MLDSA_L) / MLDSA_L)
  assigns(memory_slice(v, sizeof(mld_polyvecl)))
  ensures(forall(k0, 0, MLDSA_L,
    array_bound(v->vec[k0].coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1)))
);

#define mld_polyvecl_ntt MLD_NAMESPACE_KL(polyvecl_ntt)
/*************************************************
 * Name:        mld_polyvecl_ntt
 *
 * Description: Forward NTT of all polynomials in vector of length MLDSA_L.
 *              Coefficients can grow by 8*MLDSA_Q in absolute value.
 *
 * Arguments:   - mld_polyvecl *v: pointer to input/output vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyvecl_ntt(mld_polyvecl *v)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyvecl)))
  requires(forall(k0, 0, MLDSA_L, array_abs_bound(v->vec[k0].coeffs, 0, MLDSA_N, MLDSA_Q)))
  assigns(memory_slice(v, sizeof(mld_polyvecl)))
  ensures(forall(k1, 0, MLDSA_L, array_abs_bound(v->vec[k1].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
);

#define mld_polyvecl_pointwise_acc_montgomery \
  MLD_NAMESPACE_KL(polyvecl_pointwise_acc_montgomery)
/*************************************************
 * Name:        mld_polyvecl_pointwise_acc_montgomery
 *
 * Description: Pointwise multiply vectors of polynomials of length MLDSA_L,
 *              multiply resulting vector by 2^{-32} and add (accumulate)
 *              polynomials in it.
 *              Input/output vectors are in NTT domain representation.
 *
 *              The first input "u" must be the output of
 *              polyvec_matrix_expand() and so have coefficients in [0, Q-1]
 *              inclusive.
 *
 *              The second input "v" is assumed to be output of an NTT, and
 *              hence must have coefficients bounded by [-9q+1, +9q-1]
 *              inclusive.
 *
 *
 * Arguments:   - mld_poly *w: output polynomial
 *              - const mld_polyvecl *u: pointer to first input vector
 *              - const mld_polyvecl *v: pointer to second input vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyvecl_pointwise_acc_montgomery(mld_poly *w, const mld_polyvecl *u,
                                           const mld_polyvecl *v)
__contract__(
  requires(memory_no_alias(w, sizeof(mld_poly)))
  requires(memory_no_alias(u, sizeof(mld_polyvecl)))
  requires(memory_no_alias(v, sizeof(mld_polyvecl)))
  requires(forall(l0, 0, MLDSA_L,
                  array_bound(u->vec[l0].coeffs, 0, MLDSA_N, 0, MLDSA_Q)))
  requires(forall(l1, 0, MLDSA_L,
    array_abs_bound(v->vec[l1].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
  assigns(memory_slice(w, sizeof(mld_poly)))
  ensures(array_abs_bound(w->coeffs, 0, MLDSA_N, MLDSA_Q))
);


#define mld_polyvecl_chknorm MLD_NAMESPACE_KL(polyvecl_chknorm)
/*************************************************
 * Name:        mld_polyvecl_chknorm
 *
 * Description: Check infinity norm of polynomials in vector of length MLDSA_L.
 *              Assumes input mld_polyvecl to be reduced by polyvecl_reduce().
 *
 * Arguments:   - const mld_polyvecl *v: pointer to vector
 *              - int32_t B: norm bound
 *
 * Returns 0 if norm of all polynomials is strictly smaller than B <=
 * (MLDSA_Q-1)/8 and 0xFFFFFFFF otherwise.
 **************************************************/
MLD_INTERNAL_API
MLD_MUST_CHECK_RETURN_VALUE
uint32_t mld_polyvecl_chknorm(const mld_polyvecl *v, int32_t B)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyvecl)))
  requires(0 <= B && B <= (MLDSA_Q - 1) / 8)
  requires(forall(k0, 0, MLDSA_L,
    array_bound(v->vec[k0].coeffs, 0, MLDSA_N, -MLD_REDUCE32_RANGE_MAX, MLD_REDUCE32_RANGE_MAX)))
  ensures(return_value == 0 || return_value == 0xFFFFFFFF)
  ensures((return_value == 0) == forall(k1, 0, MLDSA_L, array_abs_bound(v->vec[k1].coeffs, 0, MLDSA_N, B)))
);

/* Vectors of polynomials of length MLDSA_K */
typedef struct
{
  mld_poly vec[MLDSA_K];
} mld_polyveck;

/* Matrix of polynomials (K x L) */
typedef struct
{
#if defined(MLD_CONFIG_REDUCE_RAM)
  mld_polyvecl row_buffer;
  uint8_t rho[MLDSA_SEEDBYTES];
#else
  mld_polyvecl vec[MLDSA_K];
#endif
} mld_polymat;

#define mld_polyveck_reduce MLD_NAMESPACE_KL(polyveck_reduce)
/*************************************************
 * Name:        polyveck_reduce
 *
 * Description: Reduce coefficients of polynomials in vector of length MLDSA_K
 *              to representatives in
 *[-MLD_REDUCE32_RANGE_MAX,MLD_REDUCE32_RANGE_MAX].
 *
 * Arguments:   - mld_polyveck *v: pointer to input/output vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_reduce(mld_polyveck *v)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K,
    array_bound(v->vec[k0].coeffs, 0, MLDSA_N, INT32_MIN, MLD_REDUCE32_DOMAIN_MAX)))
  assigns(memory_slice(v, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K,
    array_bound(v->vec[k1].coeffs, 0, MLDSA_N, -MLD_REDUCE32_RANGE_MAX, MLD_REDUCE32_RANGE_MAX)))
);

#define mld_polyveck_caddq MLD_NAMESPACE_KL(polyveck_caddq)
/*************************************************
 * Name:        mld_polyveck_caddq
 *
 * Description: For all coefficients of polynomials in vector of length MLDSA_K
 *              add MLDSA_Q if coefficient is negative.
 *
 * Arguments:   - mld_polyveck *v: pointer to input/output vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_caddq(mld_polyveck *v)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K,
    array_abs_bound(v->vec[k0].coeffs, 0, MLDSA_N, MLDSA_Q)))
  assigns(memory_slice(v, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K,
    array_bound(v->vec[k1].coeffs, 0, MLDSA_N, 0, MLDSA_Q)))
);

#define mld_polyveck_add MLD_NAMESPACE_KL(polyveck_add)
/*************************************************
 * Name:        mld_polyveck_add
 *
 * Description: Add vectors of polynomials of length MLDSA_K.
 *              No modular reduction is performed.
 *
 * Arguments:   - mld_polyveck *u: pointer to input-output vector of polynomials
 *                                 to be added to
 *              - const mld_polyveck *v: pointer to second input vector of
 *                                       polynomials
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_add(mld_polyveck *u, const mld_polyveck *v)
__contract__(
  requires(memory_no_alias(u, sizeof(mld_polyveck)))
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(forall(p0, 0, MLDSA_K, array_abs_bound(u->vec[p0].coeffs, 0, MLDSA_N, MLD_INTT_BOUND)))
  requires(forall(p1, 0, MLDSA_K,
    array_bound(v->vec[p1].coeffs, 0, MLDSA_N, -MLD_REDUCE32_RANGE_MAX, MLD_REDUCE32_RANGE_MAX)))
  assigns(memory_slice(u, sizeof(mld_polyveck)))
  ensures(forall(q2, 0, MLDSA_K,
                array_bound(u->vec[q2].coeffs, 0, MLDSA_N, INT32_MIN, MLD_REDUCE32_DOMAIN_MAX)))
);

#define mld_polyveck_sub MLD_NAMESPACE_KL(polyveck_sub)
/*************************************************
 * Name:        mld_polyveck_sub
 *
 * Description: Subtract vectors of polynomials of length MLDSA_K.
 *              No modular reduction is performed.
 *
 * Arguments:   - mld_polyveck *u: pointer to first input vector
 *              - const mld_polyveck *v: pointer to second input vector to be
 *                                   subtracted from first input vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_sub(mld_polyveck *u, const mld_polyveck *v)
__contract__(
  requires(memory_no_alias(u, sizeof(mld_polyveck)))
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K, array_abs_bound(u->vec[k0].coeffs, 0, MLDSA_N, MLDSA_Q)))
  requires(forall(k1, 0, MLDSA_K, array_abs_bound(v->vec[k1].coeffs, 0, MLDSA_N, MLDSA_Q)))
  assigns(memory_slice(u, sizeof(mld_polyveck)))
  ensures(forall(k0, 0, MLDSA_K,
                 array_bound(u->vec[k0].coeffs, 0, MLDSA_N, INT32_MIN, MLD_REDUCE32_DOMAIN_MAX)))
);

#define mld_polyveck_shiftl MLD_NAMESPACE_KL(polyveck_shiftl)
/*************************************************
 * Name:        mld_polyveck_shiftl
 *
 * Description: Multiply vector of polynomials of Length MLDSA_K by 2^MLDSA_D
 *without modular reduction. Assumes input coefficients to be less than
 *2^{31-MLDSA_D}.
 *
 * Arguments:   - mld_polyveck *v: pointer to input/output vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_shiftl(mld_polyveck *v)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K, array_bound(v->vec[k0].coeffs, 0, MLDSA_N, 0, 1 << 10)))
  assigns(memory_slice(v, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K, array_bound(v->vec[k1].coeffs, 0, MLDSA_N, 0, MLDSA_Q)))
);

#define mld_polyveck_ntt MLD_NAMESPACE_KL(polyveck_ntt)
/*************************************************
 * Name:        mld_polyveck_ntt
 *
 * Description: Forward NTT of all polynomials in vector of length MLDSA_K.
 *              Coefficients can grow by 8*MLDSA_Q in absolute value.
 *
 * Arguments:   - mld_polyveck *v: pointer to input/output vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_ntt(mld_polyveck *v)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K, array_abs_bound(v->vec[k0].coeffs, 0, MLDSA_N, MLDSA_Q)))
  assigns(memory_slice(v, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K, array_abs_bound(v->vec[k1].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
);

#define mld_polyveck_invntt_tomont MLD_NAMESPACE_KL(polyveck_invntt_tomont)
/*************************************************
 * Name:        mld_polyveck_invntt_tomont
 *
 * Description: Inverse NTT and multiplication by 2^{32} of polynomials
 *              in vector of length MLDSA_K.
 *              Input coefficients need to be less than MLDSA_Q, and
 *              Output coefficients are bounded by MLD_INTT_BOUND.
 * Arguments:   - mld_polyveck *v: pointer to input/output vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_invntt_tomont(mld_polyveck *v)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K, array_abs_bound(v->vec[k0].coeffs, 0, MLDSA_N, MLDSA_Q)))
  assigns(memory_slice(v, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K, array_abs_bound(v->vec[k1].coeffs, 0, MLDSA_N, MLD_INTT_BOUND)))
);

#define mld_polyveck_pointwise_poly_montgomery \
  MLD_NAMESPACE_KL(polyveck_pointwise_poly_montgomery)
/*************************************************
 * Name:        mld_polyveck_pointwise_poly_montgomery
 *
 * Description: Pointwise multiplication of a polynomial vector of length
 *              MLDSA_K by a single polynomial in NTT domain and multiplication
 *              of the resulting polynomial vector by 2^{-32}.
 *
 * Arguments:   - mld_polyveck *r: pointer to output vector
 *              - mld_poly *a: pointer to input polynomial
 *              - mld_polyveck *v: pointer to input vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_pointwise_poly_montgomery(mld_polyveck *r, const mld_poly *a,
                                            const mld_polyveck *v)
__contract__(
  requires(memory_no_alias(r, sizeof(mld_polyveck)))
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(array_abs_bound(a->coeffs, 0, MLDSA_N, MLD_NTT_BOUND))
  requires(forall(k0, 0, MLDSA_K, array_abs_bound(v->vec[k0].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
  assigns(memory_slice(r, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K, array_abs_bound(r->vec[k1].coeffs, 0, MLDSA_N, MLDSA_Q)))
);

#define mld_polyveck_chknorm MLD_NAMESPACE_KL(polyveck_chknorm)
/*************************************************
 * Name:        mld_polyveck_chknorm
 *
 * Description: Check infinity norm of polynomials in vector of length MLDSA_K.
 *              Assumes input mld_polyveck to be reduced by polyveck_reduce().
 *
 * Arguments:   - const mld_polyveck *v: pointer to vector
 *              - int32_t B: norm bound
 *
 * Returns 0 if norm of all polynomials are strictly smaller than B <=
 *(MLDSA_Q-1)/8 and 0xFFFFFFFF otherwise.
 **************************************************/
MLD_INTERNAL_API
MLD_MUST_CHECK_RETURN_VALUE
uint32_t mld_polyveck_chknorm(const mld_polyveck *v, int32_t B)
__contract__(
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(0 <= B && B <= (MLDSA_Q - 1) / 8)
  requires(forall(k0, 0, MLDSA_K,
                  array_bound(v->vec[k0].coeffs, 0, MLDSA_N,
                              -MLD_REDUCE32_RANGE_MAX, MLD_REDUCE32_RANGE_MAX)))
  ensures(return_value == 0 || return_value == 0xFFFFFFFF)
  ensures((return_value == 0) == forall(k1, 0, MLDSA_K, array_abs_bound(v->vec[k1].coeffs, 0, MLDSA_N, B)))
);

#define mld_polyveck_power2round MLD_NAMESPACE_KL(polyveck_power2round)
/*************************************************
 * Name:        mld_polyveck_power2round
 *
 * Description: For all coefficients a of polynomials in vector of length
 *MLDSA_K, compute a0, a1 such that a mod^+ MLDSA_Q = a1*2^MLDSA_D + a0 with
 *-2^{MLDSA_D-1} < a0 <= 2^{MLDSA_D-1}. Assumes coefficients to be standard
 *representatives.
 *
 * Arguments:   - mld_polyveck *v1: pointer to output vector of polynomials with
 *                              coefficients a1
 *              - mld_polyveck *v0: pointer to output vector of polynomials with
 *                              coefficients a0
 *              - const mld_polyveck *v: pointer to input vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_power2round(mld_polyveck *v1, mld_polyveck *v0,
                              const mld_polyveck *v)
__contract__(
  requires(memory_no_alias(v1, sizeof(mld_polyveck)))
  requires(memory_no_alias(v0, sizeof(mld_polyveck)))
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K, array_bound(v->vec[k0].coeffs, 0, MLDSA_N, 0, MLDSA_Q)))
  assigns(memory_slice(v1, sizeof(mld_polyveck)))
  assigns(memory_slice(v0, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K, array_bound(v0->vec[k1].coeffs, 0, MLDSA_N, -(MLD_2_POW_D/2)+1, (MLD_2_POW_D/2)+1)))
  ensures(forall(k2, 0, MLDSA_K, array_bound(v1->vec[k2].coeffs, 0, MLDSA_N, 0, ((MLDSA_Q - 1) / MLD_2_POW_D) + 1)))
);

#define mld_polyveck_decompose MLD_NAMESPACE_KL(polyveck_decompose)
/*************************************************
 * Name:        mld_polyveck_decompose
 *
 * Description: For all coefficients a of polynomials in vector of length
 * MLDSA_K, compute high and low bits a0, a1 such a mod^+ MLDSA_Q = a1*ALPHA
 * + a0 with -ALPHA/2 < a0 <= ALPHA/2 except a1 = (MLDSA_Q-1)/ALPHA where we set
 * a1 = 0 and -ALPHA/2 <= a0 = a mod MLDSA_Q - MLDSA_Q < 0. Assumes coefficients
 * to be standard representatives.
 *
 * Arguments:   - mld_polyveck *v1: pointer to output vector of polynomials with
 *                                  coefficients a1
 *              - mld_polyveck *v0: pointer to input/output vector of
 *                                  polynomials with. Output polynomial has
 *                                  coefficients a0
 *
 * Reference: The reference implementation has the input polynomial as a
 *            separate argument that may be aliased with either of the outputs.
 *            Removing the aliasing eases CBMC proofs.
 *
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_decompose(mld_polyveck *v1, mld_polyveck *v0)
__contract__(
  requires(memory_no_alias(v1,  sizeof(mld_polyveck)))
  requires(memory_no_alias(v0, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K,
    array_bound(v0->vec[k0].coeffs, 0, MLDSA_N, 0, MLDSA_Q)))
  assigns(memory_slice(v1, sizeof(mld_polyveck)))
  assigns(memory_slice(v0, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K,
                 array_bound(v1->vec[k1].coeffs, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2))))
  ensures(forall(k2, 0, MLDSA_K,
                 array_abs_bound(v0->vec[k2].coeffs, 0, MLDSA_N, MLDSA_GAMMA2+1)))
);

#define mld_polyveck_make_hint MLD_NAMESPACE_KL(polyveck_make_hint)
/*************************************************
 * Name:        mld_polyveck_make_hint
 *
 * Description: Compute hint vector.
 *
 * Arguments:   - mld_polyveck *h: pointer to output vector
 *              - const mld_polyveck *v0: pointer to low part of input vector
 *              - const mld_polyveck *v1: pointer to high part of input vector
 *
 * Returns number of 1 bits.
 **************************************************/
MLD_INTERNAL_API
MLD_MUST_CHECK_RETURN_VALUE
unsigned int mld_polyveck_make_hint(mld_polyveck *h, const mld_polyveck *v0,
                                    const mld_polyveck *v1)
__contract__(
  requires(memory_no_alias(h,  sizeof(mld_polyveck)))
  requires(memory_no_alias(v0, sizeof(mld_polyveck)))
  requires(memory_no_alias(v1, sizeof(mld_polyveck)))
  assigns(memory_slice(h, sizeof(mld_polyveck)))
  ensures(return_value <= MLDSA_N * MLDSA_K)
  ensures(forall(k1, 0, MLDSA_K, array_bound(h->vec[k1].coeffs, 0, MLDSA_N, 0, 2)))
);

#define mld_polyveck_use_hint MLD_NAMESPACE_KL(polyveck_use_hint)
/*************************************************
 * Name:        mld_polyveck_use_hint
 *
 * Description: Use hint vector to correct the high bits of input vector.
 *
 * Arguments:   - mld_polyveck *w: pointer to output vector of polynomials with
 *                             corrected high bits
 *              - const mld_polyveck *u: pointer to input vector
 *              - const mld_polyveck *h: pointer to input hint vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_use_hint(mld_polyveck *w, const mld_polyveck *v,
                           const mld_polyveck *h)
__contract__(
  requires(memory_no_alias(w,  sizeof(mld_polyveck)))
  requires(memory_no_alias(v, sizeof(mld_polyveck)))
  requires(memory_no_alias(h, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K,
    array_bound(v->vec[k0].coeffs, 0, MLDSA_N, 0, MLDSA_Q)))
  requires(forall(k1, 0, MLDSA_K,
    array_bound(h->vec[k1].coeffs, 0, MLDSA_N, 0, 2)))
  assigns(memory_slice(w, sizeof(mld_polyveck)))
  ensures(forall(k2, 0, MLDSA_K,
    array_bound(w->vec[k2].coeffs, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2))))
);

#define mld_polyveck_pack_w1 MLD_NAMESPACE_KL(polyveck_pack_w1)
/*************************************************
 * Name:        mld_polyveck_pack_w1
 *
 * Description: Bit-pack polynomial vector w1 with coefficients in [0,15] or
 *              [0,43].
 *              Input coefficients are assumed to be standard representatives.
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with at least
 *                            MLDSA_K* MLDSA_POLYW1_PACKEDBYTES bytes
 *              - const mld_polyveck *a: pointer to input polynomial vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_pack_w1(uint8_t r[MLDSA_K * MLDSA_POLYW1_PACKEDBYTES],
                          const mld_polyveck *w1)
__contract__(
  requires(memory_no_alias(r, MLDSA_K * MLDSA_POLYW1_PACKEDBYTES))
  requires(memory_no_alias(w1, sizeof(mld_polyveck)))
  requires(forall(k1, 0, MLDSA_K,
    array_bound(w1->vec[k1].coeffs, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2))))
  assigns(memory_slice(r, MLDSA_K * MLDSA_POLYW1_PACKEDBYTES))
);

#define mld_polyveck_pack_eta MLD_NAMESPACE_KL(polyveck_pack_eta)
/*************************************************
 * Name:        mld_polyveck_pack_eta
 *
 * Description: Bit-pack polynomial vector with coefficients
 *              in [-MLDSA_ETA,MLDSA_ETA].
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with
 *                            MLDSA_K * MLDSA_POLYETA_PACKEDBYTES bytes
 *              - const polyveck *p: pointer to input polynomial vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_pack_eta(uint8_t r[MLDSA_K * MLDSA_POLYETA_PACKEDBYTES],
                           const mld_polyveck *p)
__contract__(
  requires(memory_no_alias(r,  MLDSA_K * MLDSA_POLYETA_PACKEDBYTES))
  requires(memory_no_alias(p, sizeof(mld_polyveck)))
  requires(forall(k1, 0, MLDSA_K,
    array_abs_bound(p->vec[k1].coeffs, 0, MLDSA_N, MLDSA_ETA + 1)))
  assigns(memory_slice(r, MLDSA_K * MLDSA_POLYETA_PACKEDBYTES))
);

#define mld_polyvecl_pack_eta MLD_NAMESPACE_KL(polyvecl_pack_eta)
/*************************************************
 * Name:        mld_polyvecl_pack_eta
 *
 * Description: Bit-pack polynomial vector with coefficients in
 *              [-MLDSA_ETA,MLDSA_ETA].
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with
 *                            MLDSA_L * MLDSA_POLYETA_PACKEDBYTES bytes
 *              - const polyveck *p: pointer to input polynomial vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyvecl_pack_eta(uint8_t r[MLDSA_L * MLDSA_POLYETA_PACKEDBYTES],
                           const mld_polyvecl *p)
__contract__(
  requires(memory_no_alias(r,  MLDSA_L * MLDSA_POLYETA_PACKEDBYTES))
  requires(memory_no_alias(p, sizeof(mld_polyvecl)))
  requires(forall(k1, 0, MLDSA_L,
    array_abs_bound(p->vec[k1].coeffs, 0, MLDSA_N, MLDSA_ETA + 1)))
  assigns(memory_slice(r, MLDSA_L * MLDSA_POLYETA_PACKEDBYTES))
);

#define mld_polyveck_pack_t0 MLD_NAMESPACE_KL(polyveck_pack_t0)
/*************************************************
 * Name:        mld_polyveck_pack_t0
 *
 * Description: Bit-pack polynomial vector to with coefficients in
 *              ]-2^{MLDSA_D-1}, 2^{MLDSA_D-1}].
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with
 *                            MLDSA_K * MLDSA_POLYT0_PACKEDBYTES bytes
 *              - const mld_poly *p: pointer to input polynomial vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_pack_t0(uint8_t r[MLDSA_K * MLDSA_POLYT0_PACKEDBYTES],
                          const mld_polyveck *p)
__contract__(
  requires(memory_no_alias(r,  MLDSA_K * MLDSA_POLYT0_PACKEDBYTES))
  requires(memory_no_alias(p, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K,
    array_bound(p->vec[k0].coeffs, 0, MLDSA_N, -(1<<(MLDSA_D-1)) + 1, (1<<(MLDSA_D-1)) + 1)))
  assigns(memory_slice(r, MLDSA_K * MLDSA_POLYT0_PACKEDBYTES))
);

#define mld_polyvecl_unpack_eta MLD_NAMESPACE_KL(polyvecl_unpack_eta)
/*************************************************
 * Name:        mld_polyvecl_unpack_eta
 *
 * Description: Unpack polynomial vector with coefficients in
 *              [-MLDSA_ETA,MLDSA_ETA].
 *
 * Arguments:   - mld_polyvecl *p: pointer to output polynomial vector
 *              - const uint8_t *r: input byte array with
 *                                  bit-packed polynomial vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyvecl_unpack_eta(
    mld_polyvecl *p, const uint8_t r[MLDSA_L * MLDSA_POLYETA_PACKEDBYTES])
__contract__(
  requires(memory_no_alias(r,  MLDSA_L * MLDSA_POLYETA_PACKEDBYTES))
  requires(memory_no_alias(p, sizeof(mld_polyvecl)))
  assigns(memory_slice(p, sizeof(mld_polyvecl)))
  ensures(forall(k1, 0, MLDSA_L,
    array_bound(p->vec[k1].coeffs, 0, MLDSA_N, MLD_POLYETA_UNPACK_LOWER_BOUND, MLDSA_ETA + 1)))
);

#define mld_polyvecl_unpack_z MLD_NAMESPACE_KL(polyvecl_unpack_z)
/*************************************************
 * Name:        mld_polyvecl_unpack_z
 *
 * Description: Unpack polynomial vector with coefficients in
 *              [-(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1].
 *
 * Arguments:   - mld_polyvecl *z: pointer to output polynomial vector
 *              - const uint8_t *r: input byte array with
 *                                  bit-packed polynomial vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyvecl_unpack_z(mld_polyvecl *z,
                           const uint8_t r[MLDSA_L * MLDSA_POLYZ_PACKEDBYTES])
__contract__(
  requires(memory_no_alias(r,  MLDSA_L * MLDSA_POLYZ_PACKEDBYTES))
  requires(memory_no_alias(z, sizeof(mld_polyvecl)))
  assigns(memory_slice(z, sizeof(mld_polyvecl)))
  ensures(forall(k1, 0, MLDSA_L,
    array_bound(z->vec[k1].coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1)))
);

#define mld_polyveck_unpack_eta MLD_NAMESPACE_KL(polyveck_unpack_eta)
/*************************************************
 * Name:        mld_polyveck_unpack_eta
 *
 * Description: Unpack polynomial vector with coefficients in
 *              [-MLDSA_ETA,MLDSA_ETA].
 *
 * Arguments:   - mld_polyveck *p: pointer to output polynomial vector
 *              - const uint8_t *r: input byte array with
 *                                  bit-packed polynomial vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_unpack_eta(
    mld_polyveck *p, const uint8_t r[MLDSA_K * MLDSA_POLYETA_PACKEDBYTES])
__contract__(
  requires(memory_no_alias(r,  MLDSA_K * MLDSA_POLYETA_PACKEDBYTES))
  requires(memory_no_alias(p, sizeof(mld_polyveck)))
  assigns(memory_slice(p, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K,
    array_bound(p->vec[k1].coeffs, 0, MLDSA_N, MLD_POLYETA_UNPACK_LOWER_BOUND, MLDSA_ETA + 1)))
);

#define mld_polyveck_unpack_t0 MLD_NAMESPACE_KL(polyveck_unpack_t0)
/*************************************************
 * Name:        mld_polyveck_unpack_t0
 *
 * Description: Unpack polynomial vector with coefficients in
 *              ]-2^{MLDSA_D-1}, 2^{MLDSA_D-1}].
 *
 * Arguments:   - mld_polyveck *p: pointer to output polynomial vector
 *              - const uint8_t *r: input byte array with
 *                                  bit-packed polynomial vector
 **************************************************/
MLD_INTERNAL_API
void mld_polyveck_unpack_t0(mld_polyveck *p,
                            const uint8_t r[MLDSA_K * MLDSA_POLYT0_PACKEDBYTES])
__contract__(
  requires(memory_no_alias(r,  MLDSA_K * MLDSA_POLYT0_PACKEDBYTES))
  requires(memory_no_alias(p, sizeof(mld_polyveck)))
  assigns(memory_slice(p, sizeof(mld_polyveck)))
  ensures(forall(k1, 0, MLDSA_K,
    array_bound(p->vec[k1].coeffs, 0, MLDSA_N, -(1<<(MLDSA_D-1)) + 1, (1<<(MLDSA_D-1)) + 1)))
);

#define mld_polymat_get_row MLD_NAMESPACE_KL(polymat_get_row)
/*************************************************
 * Name:        mld_polymat_get_row
 *
 * Description: Retrieve a pointer to a specific row of the matrix.
 *              In MLD_CONFIG_REDUCE_RAM mode, generates the row on-demand.
 *
 * Arguments:   - mld_polymat *mat: pointer to matrix
 *              - unsigned int row: row index (must be < MLDSA_K)
 *
 * Returns pointer to the row (mld_polyvecl)
 **************************************************/
MLD_INTERNAL_API
const mld_polyvecl *mld_polymat_get_row(mld_polymat *mat, unsigned int row);

#define mld_polyvec_matrix_expand MLD_NAMESPACE_KL(polyvec_matrix_expand)
/*************************************************
 * Name:        mld_polyvec_matrix_expand
 *
 * Description: Implementation of ExpandA. Generates matrix A with uniformly
 *              random coefficients a_{i,j} by performing rejection
 *              sampling on the output stream of SHAKE128(rho|j|i)
 *
 * Arguments:   - mld_polymat *mat: pointer to output matrix
 *              - const uint8_t rho[]: byte array containing seed rho
 **************************************************/
MLD_INTERNAL_API
void mld_polyvec_matrix_expand(mld_polymat *mat,
                               const uint8_t rho[MLDSA_SEEDBYTES])
__contract__(
  requires(memory_no_alias(mat, sizeof(mld_polymat)))
  requires(memory_no_alias(rho, MLDSA_SEEDBYTES))
  assigns(memory_slice(mat, sizeof(mld_polymat)))
  ensures(forall(k1, 0, MLDSA_K, forall(l1, 0, MLDSA_L,
    array_bound(mat->vec[k1].vec[l1].coeffs, 0, MLDSA_N, 0, MLDSA_Q))))
);



#define mld_polyvec_matrix_pointwise_montgomery \
  MLD_NAMESPACE_KL(polyvec_matrix_pointwise_montgomery)
/*************************************************
 * Name:        mld_polyvec_matrix_pointwise_montgomery
 *
 * Description: Compute matrix-vector multiplication in NTT domain with
 *              pointwise multiplication and multiplication by 2^{-32}.
 *              Input matrix and vector must be in NTT domain representation.
 *
 *              The first input "mat" must be the output of
 *              polyvec_matrix_expand() and so have coefficients in [0, Q-1]
 *              inclusive.
 *
 *              The second input "v" is assumed to be output of an NTT, and
 *              hence must have coefficients bounded by [-9q+1, +9q-1]
 *              inclusive.
 *
 *              Note: In MLD_CONFIG_REDUCE_RAM mode, mat cannot be const
 *              as rows are generated on-demand.
 *
 * Arguments:   - mld_polyveck *t: pointer to output vector t
 *              - mld_polymat *mat: pointer to input matrix
 *              - const mld_polyvecl *v: pointer to input vector v
 **************************************************/
MLD_INTERNAL_API
void mld_polyvec_matrix_pointwise_montgomery(mld_polyveck *t, mld_polymat *mat,
                                             const mld_polyvecl *v)
__contract__(
  requires(memory_no_alias(t, sizeof(mld_polyveck)))
  requires(memory_no_alias(mat, sizeof(mld_polymat)))
  requires(memory_no_alias(v, sizeof(mld_polyvecl)))
  requires(forall(k1, 0, MLDSA_K, forall(l1, 0, MLDSA_L,
                                         array_bound(mat->vec[k1].vec[l1].coeffs, 0, MLDSA_N, 0, MLDSA_Q))))
  requires(forall(l1, 0, MLDSA_L,
                  array_abs_bound(v->vec[l1].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
  assigns(memory_slice(t, sizeof(mld_polyveck)))
  ensures(forall(k0, 0, MLDSA_K,
                 array_abs_bound(t->vec[k0].coeffs, 0, MLDSA_N, MLDSA_Q)))
);

#endif /* !MLD_POLYVEC_H */
