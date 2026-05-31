/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#ifndef MLD_NATIVE_API_H
#define MLD_NATIVE_API_H
/*
 * Native arithmetic interface
 *
 * This header is primarily for documentation purposes.
 * It should not be included by backend implementations.
 *
 * To ensure consistency with backends, the header will be
 * included automatically after inclusion of the active
 * backend, to ensure consistency of function signatures,
 * and run sanity checks.
 */

#include <stdint.h>
#include "../cbmc.h"
#include "../common.h"

/* Backends must return MLD_NATIVE_FUNC_SUCCESS upon success. */
#define MLD_NATIVE_FUNC_SUCCESS (0)
/* Backends may return MLD_NATIVE_FUNC_FALLBACK to signal to the frontend that
 * the target/parameters are unsupported; typically, this would be because of
 * dependencies on CPU features not detected on the host CPU. In this case,
 * the frontend falls back to the default C implementation.
 *
 * IMPORTANT: Backend implementations must ensure that the decision of whether
 * to fallback (return MLD_NATIVE_FUNC_FALLBACK) or not must never depend on
 * the input data itself. Fallback decisions may only depend on system
 * capabilities (e.g., CPU features) and, where present, length information.
 * This requirement applies to all backend functions to maintain constant-time
 * properties.
 */
#define MLD_NATIVE_FUNC_FALLBACK (-1)

/* Bound on absolute value of coefficients after NTT.
 *
 * NOTE: This is the same bound as in ntt.h and has to be kept
 * in sync. */
#define MLD_NTT_BOUND (9 * MLDSA_Q)

/* Absolute exclusive upper bound for the output of the inverse NTT
 *
 * NOTE: This is the same bound as in ntt.h and has to be kept
 * in sync. */
#define MLD_INTT_BOUND MLDSA_Q

/* Absolute bound for range of mld_reduce32()
 *
 * NOTE: This is the same bound as in reduce.h and has to be kept
 * in sync. */
/* check-magic: 6283009 == (MLD_REDUCE32_DOMAIN_MAX - 255 * MLDSA_Q + 1) */
#define REDUCE32_RANGE_MAX 6283009
/*
 * This is the C<->native interface allowing for the drop-in of
 * native code for performance critical arithmetic components of ML-DSA.
 *
 * A _backend_ is a specific implementation of (part of) this interface.
 *
 * To add a function to a backend, define MLD_USE_NATIVE_XXX and
 * implement `static inline xxx(...)` in the profile header.
 */

/*
 * Those functions are meant to be trivial wrappers around the chosen native
 * implementation. The are static inline to avoid unnecessary calls.
 * The macro before each declaration controls whether a native
 * implementation is present.
 */

#if defined(MLD_USE_NATIVE_NTT)
/*************************************************
 * Name:        mld_ntt_native
 *
 * Description: Computes negacyclic number-theoretic transform (NTT) of
 *              a polynomial in place.
 *
 *              The input polynomial is assumed to be in normal order.
 *              The output polynomial is in bitreversed order.
 *
 * Arguments:   - int32_t p[MLDSA_N]: pointer to in/output polynomial
 **************************************************/
static MLD_INLINE int mld_ntt_native(int32_t p[MLDSA_N])
__contract__(
    requires(memory_no_alias(p, sizeof(int32_t) * MLDSA_N))
    requires(array_abs_bound(p, 0, MLDSA_N, MLDSA_Q))
    assigns(memory_slice(p, sizeof(int32_t) * MLDSA_N))
    ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
    ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_abs_bound(p, 0, MLDSA_N, MLD_NTT_BOUND))
    ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_abs_bound(p, 0, MLDSA_N, MLDSA_Q))
    ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(p, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_NTT */


#if defined(MLD_USE_NATIVE_NTT_CUSTOM_ORDER)
/*
 * This must only be set if NTT and INTT have native implementations
 * that are adapted to the custom order.
 */
#if !defined(MLD_USE_NATIVE_NTT) || !defined(MLD_USE_NATIVE_INTT)
#error \
    "Invalid native profile: MLD_USE_NATIVE_NTT_CUSTOM_ORDER can only be \
set if there are native implementations for NTT and INTT."
#endif

/*************************************************
 * Name:        mlD_poly_permute_bitrev_to_custom
 *
 * Description: When MLD_USE_NATIVE_NTT_CUSTOM_ORDER is defined,
 *              convert a polynomial in NTT domain from bitreversed
 *              order to the custom order output by the native NTT.
 *
 *              This must only be defined if there is native code for
 *              both the NTT and INTT.
 *
 * Arguments:   - int32_t p[MLDSA_N]: pointer to in/output polynomial
 *
 **************************************************/
static MLD_INLINE void mld_poly_permute_bitrev_to_custom(int32_t p[MLDSA_N])
__contract__(
  /* We don't specify that this should be a permutation, but only
   * that it does not change the bound established at the end of
   * mld_polyvec_matrix_expand.
   */
  requires(memory_no_alias(p, sizeof(int32_t) * MLDSA_N))
  requires(array_bound(p, 0, MLDSA_N, 0, MLDSA_Q))
  assigns(memory_slice(p, sizeof(int32_t) * MLDSA_N))
  ensures(array_bound(p, 0, MLDSA_N, 0, MLDSA_Q)));
#endif /* MLD_USE_NATIVE_NTT_CUSTOM_ORDER */


#if defined(MLD_USE_NATIVE_INTT)
/*************************************************
 * Name:        mld_intt_native
 *
 * Description: Computes inverse of negacyclic number-theoretic transform
 *(NTT) of a polynomial in place.
 *
 *              The input polynomial is in bitreversed order.
 *              The output polynomial is assumed to be in normal order.
 *
 * Arguments:   - uint32_t p[MLDSA_N]: pointer to in/output polynomial
 **************************************************/
static MLD_INLINE int mld_intt_native(int32_t p[MLDSA_N])
__contract__(
  requires(memory_no_alias(p, sizeof(int32_t) * MLDSA_N))
  requires(array_abs_bound(p, 0, MLDSA_N, MLDSA_Q))
  assigns(memory_slice(p, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_abs_bound(p, 0, MLDSA_N, MLD_INTT_BOUND))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_abs_bound(p, 0, MLDSA_N, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(p, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_INTT */

#if defined(MLD_USE_NATIVE_REJ_UNIFORM)
/*************************************************
 * Name:        mld_rej_uniform_native
 *
 * Description: Run rejection sampling on uniform random bytes to generate
 *              uniform random integers in [0, MLDSA_Q-1]
 *
 * Arguments:   - int32_t *r:          pointer to output buffer
 *              - unsigned len:        requested number of 32-bit integers
 *                                     (uniform mod q).
 *              - const uint8_t *buf:  pointer to input buffer
 *                                     (assumed to be uniform random bytes)
 *              - unsigned buflen:     length of input buffer in bytes.
 *
 * Return -1 if the native implementation does not support the input
 * lengths. Otherwise, returns non-negative number of sampled 32-bit integers
 * (at most len).
 **************************************************/
static MLD_INLINE int mld_rej_uniform_native(int32_t *r, unsigned len,
                                             const uint8_t *buf,
                                             unsigned buflen)
__contract__(
  requires(len <= MLDSA_N)
  requires(buflen <= ( 5 * 168) && buflen % 3 == 0)
  requires(memory_no_alias(r, sizeof(int32_t) * len))
  requires(memory_no_alias(buf, buflen))
  assigns(memory_slice(r, sizeof(int32_t) * len))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || (0 <= return_value && return_value <= len))
  ensures((return_value != MLD_NATIVE_FUNC_FALLBACK) ==> array_bound(r, 0, (unsigned) return_value, 0, MLDSA_Q))
);
#endif /* MLD_USE_NATIVE_REJ_UNIFORM */

#if defined(MLD_USE_NATIVE_REJ_UNIFORM_ETA2)
/*************************************************
 * Name:        mld_rej_uniform_eta2_native
 *
 * Description: Run rejection sampling on uniform random bytes to generate
 *              uniform random integers in [-2,+2].
 *
 * Arguments:   - int32_t *r:          pointer to output buffer
 *              - unsigned len:        requested number of 32-bit integers
 *                                     (uniform in [-2, +2]).
 *              - const uint8_t *buf:  pointer to input buffer
 *                                     (assumed to be uniform random bytes)
 *              - unsigned buflen:     length of input buffer in bytes.
 *
 * Return -1 if the native implementation does not support the input
 *lengths. Otherwise, returns non-negative number of sampled 32-bit integers
 *(at most len).
 **************************************************/
static MLD_INLINE int mld_rej_uniform_eta2_native(int32_t *r, unsigned len,
                                                  const uint8_t *buf,
                                                  unsigned buflen)
__contract__(
  requires(len <= MLDSA_N)
  requires(buflen <= (2 * 136))
  requires(memory_no_alias(r, sizeof(int32_t) * len))
  requires(memory_no_alias(buf, buflen))
  assigns(memory_slice(r, sizeof(int32_t) * len))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || (0 <= return_value && return_value <= len))
  ensures((return_value != MLD_NATIVE_FUNC_FALLBACK) ==> (array_abs_bound(r, 0, return_value, MLDSA_ETA + 1)))
);
#endif /* MLD_USE_NATIVE_REJ_UNIFORM_ETA2 */

#if defined(MLD_USE_NATIVE_REJ_UNIFORM_ETA4)
/*************************************************
 * Name:        mld_rej_uniform_eta4_native
 *
 * Description: Run rejection sampling on uniform random bytes to generate
 *              uniform random integers in [-4,+4].
 *
 * Arguments:   - int32_t *r:          pointer to output buffer
 *              - unsigned len:        requested number of 32-bit integers
 *                                     (uniform in [-4, +4]).
 *              - const uint8_t *buf:  pointer to input buffer
 *                                     (assumed to be uniform random bytes)
 *              - unsigned buflen:     length of input buffer in bytes.
 *
 * Return -1 if the native implementation does not support the input
 *lengths. Otherwise, returns non-negative number of sampled 32-bit integers
 *(at most len).
 **************************************************/
static MLD_INLINE int mld_rej_uniform_eta4_native(int32_t *r, unsigned len,
                                                  const uint8_t *buf,
                                                  unsigned buflen)
__contract__(
  requires(len <= MLDSA_N)
  requires(buflen <= (2 * 136))
  requires(memory_no_alias(r, sizeof(int32_t) * len))
  requires(memory_no_alias(buf, buflen))
  assigns(memory_slice(r, sizeof(int32_t) * len))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || (0 <= return_value && return_value <= len))
  ensures((return_value != MLD_NATIVE_FUNC_FALLBACK) ==> (array_abs_bound(r, 0, return_value, MLDSA_ETA + 1)))
);
#endif /* MLD_USE_NATIVE_REJ_UNIFORM_ETA4 */

#if defined(MLD_USE_NATIVE_POLY_DECOMPOSE_32)
/*************************************************
 * Name:        mld_poly_decompose_32_native
 *
 * Description: Native implementation of poly_decompose for GAMMA2 = (Q-1)/32.
 *              For all coefficients c of the input polynomial,
 *              compute high and low bits c0, c1 such
 *              c mod MLDSA_Q = c1*(2*GAMMA2) + c0
 *              with -(2*GAMMA2)/2 < c0 <= (2*GAMMA2)/2 except
 *              c1 = (MLDSA_Q-1)/(2*GAMMA2) where we set
 *              c1 = 0 and -(2*GAMMA2)/2 <= c0 = c mod MLDSA_Q - MLDSA_Q < 0.
 *              Assumes coefficients to be standard representatives.
 *
 * Arguments:   - int32_t *a1: output polynomial with coefficients c1
 *              - int32_t *a0: input/output polynomial.
 *                             Output has coefficients c0
 **************************************************/
static MLD_INLINE int mld_poly_decompose_32_native(int32_t *a1, int32_t *a0)
__contract__(
  requires(memory_no_alias(a1,  sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(a0, sizeof(int32_t) * MLDSA_N))
  requires(array_bound(a0, 0, MLDSA_N, 0, MLDSA_Q))
  assigns(memory_slice(a1, sizeof(int32_t) * MLDSA_N))
  assigns(memory_slice(a0, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_bound(a1, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2)))
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_abs_bound(a0, 0, MLDSA_N, MLDSA_GAMMA2+1))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_bound(a0, 0, MLDSA_N, 0, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(a0, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLY_DECOMPOSE_32 */

#if defined(MLD_USE_NATIVE_POLY_DECOMPOSE_88)
/*************************************************
 * Name:        mld_poly_decompose_88_native
 *
 * Description: Native implementation of poly_decompose for GAMMA2 = (Q-1)/88.
 *              For all coefficients c of the input polynomial,
 *              compute high and low bits c0, c1 such
 *              c mod MLDSA_Q = c1*(2*GAMMA2) + c0
 *              with -(2*GAMMA2)/2 < c0 <= (2*GAMMA2)/2 except
 *              c1 = (MLDSA_Q-1)/(2*GAMMA2) where we set
 *              c1 = 0 and -(2*GAMMA2)/2 <= c0 = c mod MLDSA_Q - MLDSA_Q < 0.
 *              Assumes coefficients to be standard representatives.
 *
 * Arguments:   - int32_t *a1: output polynomial with coefficients c1
 *              - int32_t *a0: output polynomial with coefficients c0.
 *                             Output has coefficients c0
 **************************************************/
static MLD_INLINE int mld_poly_decompose_88_native(int32_t *a1, int32_t *a0)
__contract__(
  requires(memory_no_alias(a1,  sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(a0, sizeof(int32_t) * MLDSA_N))
  requires(array_bound(a0, 0, MLDSA_N, 0, MLDSA_Q))
  assigns(memory_slice(a1, sizeof(int32_t) * MLDSA_N))
  assigns(memory_slice(a0, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_bound(a1, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2)))
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_abs_bound(a0, 0, MLDSA_N, MLDSA_GAMMA2+1))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_bound(a0, 0, MLDSA_N, 0, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(a0, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLY_DECOMPOSE_88 */

#if defined(MLD_USE_NATIVE_POLY_CADDQ)
/*************************************************
 * Name:        mld_poly_caddq_native
 *
 * Description: For all coefficients of in/out polynomial add Q if
 *              coefficient is negative.
 *
 * Arguments:   - int32_t *a: pointer to input/output polynomial
 **************************************************/
static MLD_INLINE int mld_poly_caddq_native(int32_t a[MLDSA_N])
__contract__(
  requires(memory_no_alias(a, sizeof(int32_t) * MLDSA_N))
  requires(array_abs_bound(a, 0, MLDSA_N, MLDSA_Q))
  assigns(memory_slice(a, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_bound(a, 0, MLDSA_N, 0, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_abs_bound(a, 0, MLDSA_N, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(a, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLY_CADDQ */

#if defined(MLD_USE_NATIVE_POLY_USE_HINT_32)
/*************************************************
 * Name:        mld_poly_use_hint_32_native
 *
 * Description: Native implementation of poly_use_hint for GAMMA2 = (Q-1)/32.
 *              Use hint polynomial to correct the high bits of a polynomial.
 *
 * Arguments:   - int32_t *b: pointer to output polynomial with corrected high
 *                            bits
 *              - const int32_t *a: pointer to input polynomial
 *              - const int32_t *h: pointer to input hint polynomial
 **************************************************/
static MLD_INLINE int mld_poly_use_hint_32_native(int32_t *b, const int32_t *a,
                                                  const int32_t *h)
__contract__(
  requires(memory_no_alias(a, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(b, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(h, sizeof(int32_t) * MLDSA_N))
  requires(array_bound(a, 0, MLDSA_N, 0, MLDSA_Q))
  requires(array_bound(h, 0, MLDSA_N, 0, 2))
  assigns(memory_slice(b, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_bound(b, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2)))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(b, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLY_USE_HINT_32 */

#if defined(MLD_USE_NATIVE_POLY_USE_HINT_88)
/*************************************************
 * Name:        mld_poly_use_hint_88_native
 *
 * Description: Native implementation of poly_use_hint for GAMMA2 = (Q-1)/88.
 *              Use hint polynomial to correct the high bits of a polynomial.
 *
 * Arguments:   - int32_t *b: pointer to output polynomial with corrected high
 *                            bits
 *              - const int32_t *a: pointer to input polynomial
 *              - const int32_t *h: pointer to input hint polynomial
 **************************************************/
static MLD_INLINE int mld_poly_use_hint_88_native(int32_t *b, const int32_t *a,
                                                  const int32_t *h)
__contract__(
  requires(memory_no_alias(a, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(b, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(h, sizeof(int32_t) * MLDSA_N))
  requires(array_bound(a, 0, MLDSA_N, 0, MLDSA_Q))
  requires(array_bound(h, 0, MLDSA_N, 0, 2))
  assigns(memory_slice(b, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_bound(b, 0, MLDSA_N, 0, (MLDSA_Q-1)/(2*MLDSA_GAMMA2)))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(b, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLY_USE_HINT_88 */

#if defined(MLD_USE_NATIVE_POLY_CHKNORM)
/*************************************************
 * Name:        mld_poly_chknorm_native
 *
 * Description: Check infinity norm of polynomial against given bound.
 *              Assumes input coefficients were reduced by mld_reduce32().
 *
 * Arguments:   - const int32_t *a: pointer to polynomial
 *              - int32_t B: norm bound
 *
 * Returns 0 if the infinity norm is strictly smaller than B, and 1
 * otherwise. B must not be larger than MLDSA_Q - MLD_REDUCE32_RANGE_MAX.
 **************************************************/
static MLD_INLINE int mld_poly_chknorm_native(const int32_t *a, int32_t B)
__contract__(
  requires(memory_no_alias(a, sizeof(int32_t) * MLDSA_N))
  requires(0 <= B && B <= MLDSA_Q - REDUCE32_RANGE_MAX)
  requires(array_bound(a, 0, MLDSA_N, -REDUCE32_RANGE_MAX, REDUCE32_RANGE_MAX))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == 0) == array_abs_bound(a, 0, MLDSA_N, B))
);
#endif /* MLD_USE_NATIVE_POLY_CHKNORM */

#if defined(MLD_USE_NATIVE_POLYZ_UNPACK_17)
/*************************************************
 * Name:        mld_polyz_unpack_17_native
 *
 * Description: Native implementation of polyz_unpack for GAMMA1 = 2^17.
 *              Unpack polynomial z with coefficients
 *              in [-(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1].
 *
 * Arguments:   - int32_t *r: pointer to output polynomial
 *              - const uint8_t *a: byte array with bit-packed polynomial
 **************************************************/
static MLD_INLINE int mld_polyz_unpack_17_native(int32_t *r, const uint8_t *a)
__contract__(
  requires(memory_no_alias(r, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(a, MLDSA_POLYZ_PACKEDBYTES))
  assigns(memory_slice(r, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_bound(r, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(r, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLYZ_UNPACK_17 */

#if defined(MLD_USE_NATIVE_POLYZ_UNPACK_19)
/*************************************************
 * Name:        mld_polyz_unpack_19_native
 *
 * Description: Native implementation of polyz_unpack for GAMMA1 = 2^19.
 *              Unpack polynomial z with coefficients
 *              in [-(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1].
 *
 * Arguments:   - int32_t *r: pointer to output polynomial
 *              - const uint8_t *a: byte array with bit-packed polynomial
 **************************************************/
static MLD_INLINE int mld_polyz_unpack_19_native(int32_t *r, const uint8_t *a)
__contract__(
  requires(memory_no_alias(r, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(a, MLDSA_POLYZ_PACKEDBYTES))
  assigns(memory_slice(r, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_bound(r, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(r, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLYZ_UNPACK_19 */

#if defined(MLD_USE_NATIVE_POINTWISE_MONTGOMERY)
/*************************************************
 * Name:        mld_poly_pointwise_montgomery_native
 *
 * Description: Pointwise multiplication of polynomials in NTT domain
 *              with Montgomery reduction.
 *
 *              Computes c[i] = a[i] * b[i] * R^(-1) mod q for all i,
 *              where R = 2^32.
 *
 * Arguments:   - int32_t c[MLDSA_N]: output polynomial
 *              - const int32_t a[MLDSA_N]: first input polynomial
 *              - const int32_t b[MLDSA_N]: second input polynomial
 **************************************************/
static MLD_INLINE int mld_poly_pointwise_montgomery_native(
    int32_t c[MLDSA_N], const int32_t a[MLDSA_N], const int32_t b[MLDSA_N])
__contract__(
  requires(memory_no_alias(a, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(b, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(c, sizeof(int32_t) * MLDSA_N))
  requires(array_abs_bound(a, 0, MLDSA_N, MLD_NTT_BOUND))
  requires(array_abs_bound(b, 0, MLDSA_N, MLD_NTT_BOUND))
  assigns(memory_slice(c, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_abs_bound(c, 0, MLDSA_N, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_abs_bound(a, 0, MLDSA_N, MLD_NTT_BOUND))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_abs_bound(b, 0, MLDSA_N, MLD_NTT_BOUND))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(c, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POINTWISE_MONTGOMERY */

#if defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4)
/*************************************************
 * Name:        mld_polyvecl_pointwise_acc_montgomery_l4_native
 *
 * Description: Native implementation of polyvecl_pointwise_acc_montgomery for
 *              MLDSA_L = 4.
 *              Pointwise multiply vectors of polynomials of length MLDSA_L,
 *              multiply resulting vector by 2^{-32} and add (accumulate)
 *              polynomials in it.
 *              Input/output vectors are in NTT domain representation.
 *
 * Arguments:   - int32_t w[MLDSA_N]: output polynomial
 *              - const int32_t u[MLDSA_L][MLDSA_N]: first input vector
 *              - const int32_t v[MLDSA_L][MLDSA_N]: second input vector
 **************************************************/
static MLD_INLINE int mld_polyvecl_pointwise_acc_montgomery_l4_native(
    int32_t w[MLDSA_N], const int32_t u[4][MLDSA_N],
    const int32_t v[4][MLDSA_N])
__contract__(
  requires(memory_no_alias(w, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(u, sizeof(int32_t) * 4 * MLDSA_N))
  requires(memory_no_alias(v, sizeof(int32_t) * 4 * MLDSA_N))
  requires(forall(l0, 0, 4,
                  array_bound(u[l0], 0, MLDSA_N, 0, MLDSA_Q)))
  requires(forall(l1, 0, 4,
    array_abs_bound(v[l1], 0, MLDSA_N, MLD_NTT_BOUND)))
  assigns(memory_slice(w, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_abs_bound(w, 0, MLDSA_N, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(w, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4 */

#if defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5)
/*************************************************
 * Name:        mld_polyvecl_pointwise_acc_montgomery_l5_native
 *
 * Description: Native implementation of polyvecl_pointwise_acc_montgomery for
 *              MLDSA_L = 5.
 *              Pointwise multiply vectors of polynomials of length MLDSA_L,
 *              multiply resulting vector by 2^{-32} and add (accumulate)
 *              polynomials in it.
 *              Input/output vectors are in NTT domain representation.
 *
 * Arguments:   - int32_t w[MLDSA_N]: output polynomial
 *              - const int32_t u[MLDSA_L][MLDSA_N]: first input vector
 *              - const int32_t v[MLDSA_L][MLDSA_N]: second input vector
 **************************************************/
static MLD_INLINE int mld_polyvecl_pointwise_acc_montgomery_l5_native(
    int32_t w[MLDSA_N], const int32_t u[5][MLDSA_N],
    const int32_t v[5][MLDSA_N])
__contract__(
  requires(memory_no_alias(w, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(u, sizeof(int32_t) * 5 * MLDSA_N))
  requires(memory_no_alias(v, sizeof(int32_t) * 5 * MLDSA_N))
  requires(forall(l0, 0, 5,
                  array_bound(u[l0], 0, MLDSA_N, 0, MLDSA_Q)))
  requires(forall(l1, 0, 5,
    array_abs_bound(v[l1], 0, MLDSA_N, MLD_NTT_BOUND)))
  assigns(memory_slice(w, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_abs_bound(w, 0, MLDSA_N, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(w, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5 */

#if defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7)
/*************************************************
 * Name:        mld_polyvecl_pointwise_acc_montgomery_l7_native
 *
 * Description: Native implementation of polyvecl_pointwise_acc_montgomery for
 *              MLDSA_L = 7.
 *              Pointwise multiply vectors of polynomials of length MLDSA_L,
 *              multiply resulting vector by 2^{-32} and add (accumulate)
 *              polynomials in it.
 *              Input/output vectors are in NTT domain representation.
 *
 * Arguments:   - int32_t w[MLDSA_N]: output polynomial
 *              - const int32_t u[MLDSA_L][MLDSA_N]: first input vector
 *              - const int32_t v[MLDSA_L][MLDSA_N]: second input vector
 **************************************************/
static MLD_INLINE int mld_polyvecl_pointwise_acc_montgomery_l7_native(
    int32_t w[MLDSA_N], const int32_t u[7][MLDSA_N],
    const int32_t v[7][MLDSA_N])
__contract__(
  requires(memory_no_alias(w, sizeof(int32_t) * MLDSA_N))
  requires(memory_no_alias(u, sizeof(int32_t) * 7 * MLDSA_N))
  requires(memory_no_alias(v, sizeof(int32_t) * 7 * MLDSA_N))
  requires(forall(l0, 0, 7,
                  array_bound(u[l0], 0, MLDSA_N, 0, MLDSA_Q)))
  requires(forall(l1, 0, 7,
    array_abs_bound(v[l1], 0, MLDSA_N, MLD_NTT_BOUND)))
  assigns(memory_slice(w, sizeof(int32_t) * MLDSA_N))
  ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
  ensures((return_value == MLD_NATIVE_FUNC_SUCCESS) ==> array_abs_bound(w, 0, MLDSA_N, MLDSA_Q))
  ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged(w, MLDSA_N))
);
#endif /* MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7 */

#endif /* !MLD_NATIVE_API_H */
