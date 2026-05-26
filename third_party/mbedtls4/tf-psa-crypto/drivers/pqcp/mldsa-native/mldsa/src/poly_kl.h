/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_POLY_KL_H
#define MLD_POLY_KL_H

#include "cbmc.h"
#include "common.h"
#include "poly.h"

#define mld_poly_decompose MLD_NAMESPACE_KL(poly_decompose)
/*************************************************
 * Name:        mld_poly_decompose
 *
 * Description: For all coefficients c of the input polynomial,
 *              compute high and low bits c0, c1 such c mod MLDSA_Q = c1*ALPHA +
 *              c0 with -ALPHA/2 < c0 <= ALPHA/2 except
 *              c1 = (MLDSA_Q-1)/ALPHA where we set
 *              c1 = 0 and -ALPHA/2 <= c0 = c mod MLDSA_Q - MLDSA_Q < 0.
 *              Assumes coefficients to be standard representatives.
 *
 * Arguments:   - mld_poly *a1: pointer to output polynomial with coefficients
 *                              c1
 *              - mld_poly *a0: pointer to input/output polynomial. Output
 *                              polynomial has coefficients c0
 *
 * Reference: The reference implementation has the input polynomial as a
 *            separate argument that may be aliased with either of the outputs.
 *            Removing the aliasing eases CBMC proofs.
 *
 **************************************************/
MLD_INTERNAL_API
void mld_poly_decompose(mld_poly *a1, mld_poly *a0)
__contract__(
  requires(memory_no_alias(a1,  sizeof(mld_poly)))
  requires(memory_no_alias(a0, sizeof(mld_poly)))
  requires(array_bound(a0->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
  assigns(memory_slice(a1, sizeof(mld_poly)))
  assigns(memory_slice(a0, sizeof(mld_poly)))
  ensures(array_bound(a1->coeffs, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2)))
  ensures(array_abs_bound(a0->coeffs, 0, MLDSA_N, MLDSA_GAMMA2+1))
);


#define mld_poly_make_hint MLD_NAMESPACE_KL(poly_make_hint)
/*************************************************
 * Name:        mld_poly_make_hint
 *
 * Description: Compute hint polynomial. The coefficients of which indicate
 *              whether the low bits of the corresponding coefficient of
 *              the input polynomial overflow into the high bits.
 *
 * Arguments:   - mld_poly *h: pointer to output hint polynomial
 *              - const mld_poly *a0: pointer to low part of input polynomial
 *              - const mld_poly *a1: pointer to high part of input polynomial
 *
 * Returns number of 1 bits.
 **************************************************/
MLD_INTERNAL_API
unsigned int mld_poly_make_hint(mld_poly *h, const mld_poly *a0,
                                const mld_poly *a1)
__contract__(
  requires(memory_no_alias(h,  sizeof(mld_poly)))
  requires(memory_no_alias(a0, sizeof(mld_poly)))
  requires(memory_no_alias(a1, sizeof(mld_poly)))
  assigns(memory_slice(h, sizeof(mld_poly)))
  ensures(return_value <= MLDSA_N)
  ensures(array_bound(h->coeffs, 0, MLDSA_N, 0, 2))
);

#define mld_poly_use_hint MLD_NAMESPACE_KL(poly_use_hint)
/*************************************************
 * Name:        mld_poly_use_hint
 *
 * Description: Use hint polynomial to correct the high bits of a polynomial.
 *
 * Arguments:   - mld_poly *b: pointer to output polynomial with corrected high
 *bits
 *              - const mld_poly *a: pointer to input polynomial
 *              - const mld_poly *h: pointer to input hint polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_poly_use_hint(mld_poly *b, const mld_poly *a, const mld_poly *h)
__contract__(
  requires(memory_no_alias(a,  sizeof(mld_poly)))
  requires(memory_no_alias(b, sizeof(mld_poly)))
  requires(memory_no_alias(h, sizeof(mld_poly)))
  requires(array_bound(a->coeffs, 0, MLDSA_N, 0, MLDSA_Q))
  requires(array_bound(h->coeffs, 0, MLDSA_N, 0, 2))
  assigns(memory_slice(b, sizeof(mld_poly)))
  ensures(array_bound(b->coeffs, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2)))
);

#if !defined(MLD_CONFIG_SERIAL_FIPS202_ONLY)
#define mld_poly_uniform_eta_4x MLD_NAMESPACE_KL(poly_uniform_eta_4x)
/*************************************************
 * Name:        mld_poly_uniform_eta
 *
 * Description: Sample four polynomials with uniformly random coefficients
 *              in [-MLDSA_ETA,MLDSA_ETA] by performing rejection sampling on
 *              the output stream from SHAKE256(seed|nonce_i)
 *
 * Arguments:   - mld_poly *r0: pointer to first output polynomial
 *              - mld_poly *r1: pointer to second output polynomial
 *              - mld_poly *r2: pointer to third output polynomial
 *              - mld_poly *r3: pointer to fourth output polynomial
 *              - const uint8_t seed[]: byte array with seed of length
 *                MLDSA_CRHBYTES
 *              - uint8_t nonce0: first nonce
 *              - uint8_t nonce1: second nonce
 *              - uint8_t nonce2: third nonce
 *              - uint8_t nonce3: fourth nonce
 **************************************************/
MLD_INTERNAL_API
void mld_poly_uniform_eta_4x(mld_poly *r0, mld_poly *r1, mld_poly *r2,
                             mld_poly *r3, const uint8_t seed[MLDSA_CRHBYTES],
                             uint8_t nonce0, uint8_t nonce1, uint8_t nonce2,
                             uint8_t nonce3)
__contract__(
  requires(memory_no_alias(r0, sizeof(mld_poly)))
  requires(memory_no_alias(r1, sizeof(mld_poly)))
  requires(memory_no_alias(r2, sizeof(mld_poly)))
  requires(memory_no_alias(r3, sizeof(mld_poly)))
  requires(memory_no_alias(seed, MLDSA_CRHBYTES))
  assigns(memory_slice(r0, sizeof(mld_poly)))
  assigns(memory_slice(r1, sizeof(mld_poly)))
  assigns(memory_slice(r2, sizeof(mld_poly)))
  assigns(memory_slice(r3, sizeof(mld_poly)))
  ensures(array_abs_bound(r0->coeffs, 0, MLDSA_N, MLDSA_ETA + 1))
  ensures(array_abs_bound(r1->coeffs, 0, MLDSA_N, MLDSA_ETA + 1))
  ensures(array_abs_bound(r2->coeffs, 0, MLDSA_N, MLDSA_ETA + 1))
  ensures(array_abs_bound(r3->coeffs, 0, MLDSA_N, MLDSA_ETA + 1))
);
#endif /* !MLD_CONFIG_SERIAL_FIPS202_ONLY */

#if defined(MLD_CONFIG_SERIAL_FIPS202_ONLY)
#define mld_poly_uniform_eta MLD_NAMESPACE_KL(poly_uniform_eta)
/*************************************************
 * Name:        mld_poly_uniform_eta
 *
 * Description: Sample polynomial with uniformly random coefficients
 *              in [-MLDSA_ETA,MLDSA_ETA] by performing rejection sampling on
 *              the output stream from SHAKE256(seed|nonce)
 *
 * Arguments:   - mld_poly *r: pointer to output polynomial
 *              - const uint8_t seed[]: byte array with seed of length
 *                MLDSA_CRHBYTES
 *              - uint8_t nonce: nonce
 **************************************************/
MLD_INTERNAL_API
void mld_poly_uniform_eta(mld_poly *r, const uint8_t seed[MLDSA_CRHBYTES],
                          uint8_t nonce)
__contract__(
  requires(memory_no_alias(r, sizeof(mld_poly)))
  requires(memory_no_alias(seed, MLDSA_CRHBYTES))
  assigns(memory_slice(r, sizeof(mld_poly)))
  ensures(array_abs_bound(r->coeffs, 0, MLDSA_N, MLDSA_ETA + 1))
);
#endif /* MLD_CONFIG_SERIAL_FIPS202_ONLY */

#if MLD_CONFIG_PARAMETER_SET == 65 || defined(MLD_CONFIG_SERIAL_FIPS202_ONLY)
#define mld_poly_uniform_gamma1 MLD_NAMESPACE_KL(poly_uniform_gamma1)
/*************************************************
 * Name:        mld_poly_uniform_gamma1
 *
 * Description: Sample polynomial with uniformly random coefficients
 *              in [-(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1] by unpacking output
 *              stream of SHAKE256(seed|nonce)
 *
 * Arguments:   - mld_poly *a: pointer to output polynomial
 *              - const uint8_t seed[]: byte array with seed of length
 *                MLDSA_CRHBYTES
 *              - uint16_t nonce: 16-bit nonce
 **************************************************/
MLD_INTERNAL_API
void mld_poly_uniform_gamma1(mld_poly *a, const uint8_t seed[MLDSA_CRHBYTES],
                             uint16_t nonce)
__contract__(
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(memory_no_alias(seed, MLDSA_CRHBYTES))
  assigns(memory_slice(a, sizeof(mld_poly)))
  ensures(array_bound(a->coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
);
#endif /* MLD_CONFIG_PARAMETER_SET == 65 || MLD_CONFIG_SERIAL_FIPS202_ONLY */

#if !defined(MLD_CONFIG_SERIAL_FIPS202_ONLY)
#define mld_poly_uniform_gamma1_4x MLD_NAMESPACE_KL(poly_uniform_gamma1_4x)
/*************************************************
 * Name:        mld_poly_uniform_gamma1_4x
 *
 * Description: Sample polynomial with uniformly random coefficients
 *              in [-(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1] by unpacking output
 *              stream of SHAKE256(seed|nonce)
 *
 * Arguments:   - mld_poly *a: pointer to output polynomial
 *              - const uint8_t seed[]: byte array with seed of length
 *                MLDSA_CRHBYTES
 *              - uint16_t nonce: 16-bit nonce
 **************************************************/
MLD_INTERNAL_API
void mld_poly_uniform_gamma1_4x(mld_poly *r0, mld_poly *r1, mld_poly *r2,
                                mld_poly *r3,
                                const uint8_t seed[MLDSA_CRHBYTES],
                                uint16_t nonce0, uint16_t nonce1,
                                uint16_t nonce2, uint16_t nonce3)
__contract__(
  requires(memory_no_alias(r0, sizeof(mld_poly)))
  requires(memory_no_alias(r1, sizeof(mld_poly)))
  requires(memory_no_alias(r2, sizeof(mld_poly)))
  requires(memory_no_alias(r3, sizeof(mld_poly)))
  requires(memory_no_alias(seed, MLDSA_CRHBYTES))
  assigns(memory_slice(r0, sizeof(mld_poly)))
  assigns(memory_slice(r1, sizeof(mld_poly)))
  assigns(memory_slice(r2, sizeof(mld_poly)))
  assigns(memory_slice(r3, sizeof(mld_poly)))
  ensures(array_bound(r0->coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
  ensures(array_bound(r1->coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
  ensures(array_bound(r2->coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
  ensures(array_bound(r3->coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
);
#endif /* !MLD_CONFIG_SERIAL_FIPS202_ONLY */

#define mld_poly_challenge MLD_NAMESPACE_KL(poly_challenge)
/*************************************************
 * Name:        mld_poly_challenge
 *
 * Description: Implementation of H. Samples polynomial with MLDSA_TAU nonzero
 *              coefficients in {-1,1} using the output stream of
 *              SHAKE256(seed).
 *
 * Arguments:   - mld_poly *c: pointer to output polynomial
 *              - const uint8_t mu[]: byte array containing seed of length
 *                MLDSA_CTILDEBYTES
 **************************************************/
MLD_INTERNAL_API
void mld_poly_challenge(mld_poly *c, const uint8_t seed[MLDSA_CTILDEBYTES])
__contract__(
  requires(memory_no_alias(c, sizeof(mld_poly)))
  requires(memory_no_alias(seed, MLDSA_CTILDEBYTES))
  assigns(memory_slice(c, sizeof(mld_poly)))
  /* All coefficients of c are -1, 0 or +1 */
  ensures(array_bound(c->coeffs, 0, MLDSA_N, -1, 2))
);

#define mld_polyeta_pack MLD_NAMESPACE_KL(polyeta_pack)
/*************************************************
 * Name:        mld_polyeta_pack
 *
 * Description: Bit-pack polynomial with coefficients in [-MLDSA_ETA,MLDSA_ETA].
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with at least
 *                            MLDSA_POLYETA_PACKEDBYTES bytes
 *              - const mld_poly *a: pointer to input polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyeta_pack(uint8_t r[MLDSA_POLYETA_PACKEDBYTES], const mld_poly *a)
__contract__(
  requires(memory_no_alias(r, MLDSA_POLYETA_PACKEDBYTES))
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_abs_bound(a->coeffs, 0, MLDSA_N, MLDSA_ETA + 1))
  assigns(memory_slice(r, MLDSA_POLYETA_PACKEDBYTES))
);

/*
 * polyeta_unpack produces coefficients in [-MLDSA_ETA,MLDSA_ETA] for
 * well-formed inputs (i.e., those produced by polyeta_pack).
 * However, when passed an arbitrary byte array, it may produce smaller values,
 * i.e, values in [MLD_POLYETA_UNPACK_LOWER_BOUND,MLDSA_ETA]
 * Even though this should never happen, we use use the bound for arbitrary
 * inputs in the CBMC proofs.
 */
#if MLDSA_ETA == 2
#define MLD_POLYETA_UNPACK_LOWER_BOUND (-5)
#elif MLDSA_ETA == 4
#define MLD_POLYETA_UNPACK_LOWER_BOUND (-11)
#else
#error "Invalid value of MLDSA_ETA"
#endif

#define mld_polyeta_unpack MLD_NAMESPACE_KL(polyeta_unpack)
/*************************************************
 * Name:        mld_polyeta_unpack
 *
 * Description: Unpack polynomial with coefficients in [-MLDSA_ETA,MLDSA_ETA].
 *
 * Arguments:   - mld_poly *r: pointer to output polynomial
 *              - const uint8_t *a: byte array with bit-packed polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyeta_unpack(mld_poly *r, const uint8_t a[MLDSA_POLYETA_PACKEDBYTES])
__contract__(
  requires(memory_no_alias(r, sizeof(mld_poly)))
  requires(memory_no_alias(a, MLDSA_POLYETA_PACKEDBYTES))
  assigns(memory_slice(r, sizeof(mld_poly)))
  ensures(array_bound(r->coeffs, 0, MLDSA_N, MLD_POLYETA_UNPACK_LOWER_BOUND, MLDSA_ETA + 1))
);

#define mld_polyz_pack MLD_NAMESPACE_KL(polyz_pack)
/*************************************************
 * Name:        mld_polyz_pack
 *
 * Description: Bit-pack polynomial with coefficients
 *              in [-(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1].
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with at least
 *                            MLDSA_POLYZ_PACKEDBYTES bytes
 *              - const mld_poly *a: pointer to input polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyz_pack(uint8_t r[MLDSA_POLYZ_PACKEDBYTES], const mld_poly *a)
__contract__(
  requires(memory_no_alias(r, MLDSA_POLYZ_PACKEDBYTES))
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_bound(a->coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
  assigns(memory_slice(r, MLDSA_POLYZ_PACKEDBYTES))
);


#define mld_polyz_unpack MLD_NAMESPACE_KL(polyz_unpack)
/*************************************************
 * Name:        mld_polyz_unpack
 *
 * Description: Unpack polynomial z with coefficients
 *              in [-(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1].
 *
 * Arguments:   - mld_poly *r: pointer to output polynomial
 *              - const uint8_t *a: byte array with bit-packed polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyz_unpack(mld_poly *r, const uint8_t a[MLDSA_POLYZ_PACKEDBYTES])
__contract__(
  requires(memory_no_alias(r, sizeof(mld_poly)))
  requires(memory_no_alias(a, MLDSA_POLYZ_PACKEDBYTES))
  assigns(memory_slice(r, sizeof(mld_poly)))
  ensures(array_bound(r->coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
);

#define mld_polyw1_pack MLD_NAMESPACE_KL(polyw1_pack)
/*************************************************
 * Name:        mld_polyw1_pack
 *
 * Description: Bit-pack polynomial w1 with coefficients in [0,15] or [0,43].
 *              Input coefficients are assumed to be standard representatives.
 *
 * Arguments:   - uint8_t *r: pointer to output byte array with at least
 *                            MLDSA_POLYW1_PACKEDBYTES bytes
 *              - const mld_poly *a: pointer to input polynomial
 **************************************************/
MLD_INTERNAL_API
void mld_polyw1_pack(uint8_t r[MLDSA_POLYW1_PACKEDBYTES], const mld_poly *a)
__contract__(
  requires(memory_no_alias(r, MLDSA_POLYW1_PACKEDBYTES))
  requires(memory_no_alias(a, sizeof(mld_poly)))
  requires(array_bound(a->coeffs, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2)))
  assigns(memory_slice(r, MLDSA_POLYW1_PACKEDBYTES))
);

#endif /* !MLD_POLY_KL_H */
