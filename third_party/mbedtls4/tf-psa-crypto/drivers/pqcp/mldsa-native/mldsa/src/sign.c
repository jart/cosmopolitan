/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/* References
 * ==========
 *
 * - [FIPS140_3_IG]
 *   Implementation Guidance for FIPS 140-3 and the Cryptographic Module
 *   Validation Program
 *   National Institute of Standards and Technology
 *   https://csrc.nist.gov/projects/cryptographic-module-validation-program/fips-140-3-ig-announcements
 *
 * - [FIPS204]
 *   FIPS 204 Module-Lattice-Based Digital Signature Standard
 *   National Institute of Standards and Technology
 *   https://csrc.nist.gov/pubs/fips/204/final
 *
 * - [Round3_Spec]
 *   CRYSTALS-Dilithium Algorithm Specifications and Supporting Documentation
 *   (Version 3.1)
 *   Bai, Ducas, Kiltz, Lepoint, Lyubashevsky, Schwabe, Seiler, Stehlé
 *   https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
 */

#include <stdint.h>
#include <string.h>

#include "cbmc.h"
#include "ct.h"
#include "debug.h"
#include "packing.h"
#include "poly.h"
#include "poly_kl.h"
#include "polyvec.h"
#include "randombytes.h"
#include "sign.h"
#include "symmetric.h"

/* Parameter set namespacing
 * This is to facilitate building multiple instances
 * of mldsa-native (e.g. with varying parameter sets)
 * within a single compilation unit. */
#define mld_check_pct MLD_ADD_PARAM_SET(mld_check_pct) MLD_CONTEXT_PARAMETERS_2
#define mld_sample_s1_s2 MLD_ADD_PARAM_SET(mld_sample_s1_s2)
#define mld_validate_hash_length MLD_ADD_PARAM_SET(mld_validate_hash_length)
#define mld_get_hash_oid MLD_ADD_PARAM_SET(mld_get_hash_oid)
#define mld_H MLD_ADD_PARAM_SET(mld_H)
#define mld_compute_pack_z MLD_ADD_PARAM_SET(mld_compute_pack_z)
#define mld_attempt_signature_generation \
  MLD_ADD_PARAM_SET(mld_attempt_signature_generation) MLD_CONTEXT_PARAMETERS_8
#define mld_compute_t0_t1_tr_from_sk_components              \
  MLD_ADD_PARAM_SET(mld_compute_t0_t1_tr_from_sk_components) \
  MLD_CONTEXT_PARAMETERS_7
/* End of parameter set namespacing */


static int mld_check_pct(uint8_t const pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                         uint8_t const sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                         MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  ensures(return_value == 0
	  || return_value == MLD_ERR_FAIL
	  || return_value == MLD_ERR_OUT_OF_MEMORY
    || return_value == MLD_ERR_RNG_FAIL)
);

#if defined(MLD_CONFIG_KEYGEN_PCT)
/*************************************************
 * @[FIPS140_3_IG]
 * (https://csrc.nist.gov/csrc/media/Projects/cryptographic-module-validation-program/documents/fips%20140-3/FIPS%20140-3%20IG.pdf)
 *
 * TE10.35.02: Pair-wise Consistency Test (PCT) for DSA keypairs
 *
 * Purpose: Validates that a generated public/private key pair can correctly
 * sign and verify data. Test performs signature generation using the private
 * key (sk), followed by signature verification using the public key (pk).
 * Returns 0 if the signature was successfully verified, non-zero if it cannot.
 *
 * Note: @[FIPS204] requires that public/private key pairs are to be used only
 * for the calculation and/of verification of digital signatures.
 **************************************************/
static int mld_check_pct(uint8_t const pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                         uint8_t const sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                         MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  MLD_ALIGN uint8_t message[1] = {0};
  size_t siglen;
  int ret;
  MLD_ALLOC(signature, uint8_t, MLDSA_CRYPTO_BYTES, context);
  MLD_ALLOC(pk_test, uint8_t, MLDSA_CRYPTO_PUBLICKEYBYTES, context);

  if (signature == NULL || pk_test == NULL)
  {
    ret = MLD_ERR_OUT_OF_MEMORY;
    goto cleanup;
  }

  /* Copy public key for testing */
  mld_memcpy(pk_test, pk, MLDSA_CRYPTO_PUBLICKEYBYTES);

  /* Sign a test message using the original secret key */
  ret = mld_sign_signature(signature, &siglen, message, sizeof(message), NULL,
                           0, sk, context);
  if (ret != 0)
  {
    goto cleanup;
  }

#if defined(MLD_CONFIG_KEYGEN_PCT_BREAKAGE_TEST)
  /* Deliberately break public key for testing purposes */
  if (mld_break_pct())
  {
    pk_test[0] = ~pk_test[0];
  }
#endif /* MLD_CONFIG_KEYGEN_PCT_BREAKAGE_TEST */

  /* Verify the signature using the (potentially corrupted) public key */
  ret = mld_sign_verify(signature, siglen, message, sizeof(message), NULL, 0,
                        pk_test, context);

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  MLD_FREE(pk_test, uint8_t, MLDSA_CRYPTO_PUBLICKEYBYTES, context);
  MLD_FREE(signature, uint8_t, MLDSA_CRYPTO_BYTES, context);

  return ret;
}
#else /* MLD_CONFIG_KEYGEN_PCT */
static int mld_check_pct(uint8_t const pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                         uint8_t const sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                         MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  /* Skip PCT */
  ((void)pk);
  ((void)sk);
#if defined(MLD_CONFIG_CONTEXT_PARAMETER)
  ((void)context);
#endif
  return 0;
}
#endif /* !MLD_CONFIG_KEYGEN_PCT */

static void mld_sample_s1_s2(mld_polyvecl *s1, mld_polyveck *s2,
                             const uint8_t seed[MLDSA_CRHBYTES])
__contract__(
  requires(memory_no_alias(s1, sizeof(mld_polyvecl)))
  requires(memory_no_alias(s2, sizeof(mld_polyveck)))
  requires(memory_no_alias(seed, MLDSA_CRHBYTES))
  assigns(object_whole(s1), object_whole(s2))
  ensures(forall(l0, 0, MLDSA_L, array_abs_bound(s1->vec[l0].coeffs, 0, MLDSA_N, MLDSA_ETA + 1)))
  ensures(forall(k0, 0, MLDSA_K, array_abs_bound(s2->vec[k0].coeffs, 0, MLDSA_N, MLDSA_ETA + 1)))
)
{
/* Sample short vectors s1 and s2 */
#if defined(MLD_CONFIG_SERIAL_FIPS202_ONLY)
  int i;
  uint16_t nonce = 0;
  /* Safety: The nonces are at most 14 (MLDSA_L + MLDSA_K - 1), and, hence, the
   * casts are safe. */
  for (i = 0; i < MLDSA_L; i++)
  {
    mld_poly_uniform_eta(&s1->vec[i], seed, (uint8_t)(nonce + i));
  }
  for (i = 0; i < MLDSA_K; i++)
  {
    mld_poly_uniform_eta(&s2->vec[i], seed, (uint8_t)(nonce + MLDSA_L + i));
  }
#else /* MLD_CONFIG_SERIAL_FIPS202_ONLY */
#if MLD_CONFIG_PARAMETER_SET == 44
  mld_poly_uniform_eta_4x(&s1->vec[0], &s1->vec[1], &s1->vec[2], &s1->vec[3],
                          seed, 0, 1, 2, 3);
  mld_poly_uniform_eta_4x(&s2->vec[0], &s2->vec[1], &s2->vec[2], &s2->vec[3],
                          seed, 4, 5, 6, 7);
#elif MLD_CONFIG_PARAMETER_SET == 65
  mld_poly_uniform_eta_4x(&s1->vec[0], &s1->vec[1], &s1->vec[2], &s1->vec[3],
                          seed, 0, 1, 2, 3);
  mld_poly_uniform_eta_4x(&s1->vec[4], &s2->vec[0], &s2->vec[1],
                          &s2->vec[2] /* irrelevant */, seed, 4, 5, 6,
                          0xFF /* irrelevant */);
  mld_poly_uniform_eta_4x(&s2->vec[2], &s2->vec[3], &s2->vec[4], &s2->vec[5],
                          seed, 7, 8, 9, 10);
#elif MLD_CONFIG_PARAMETER_SET == 87
  mld_poly_uniform_eta_4x(&s1->vec[0], &s1->vec[1], &s1->vec[2], &s1->vec[3],
                          seed, 0, 1, 2, 3);
  mld_poly_uniform_eta_4x(&s1->vec[4], &s1->vec[5], &s1->vec[6],
                          &s2->vec[0] /* irrelevant */, seed, 4, 5, 6,
                          0xFF /* irrelevant */);
  mld_poly_uniform_eta_4x(&s2->vec[0], &s2->vec[1], &s2->vec[2], &s2->vec[3],
                          seed, 7, 8, 9, 10);
  mld_poly_uniform_eta_4x(&s2->vec[4], &s2->vec[5], &s2->vec[6], &s2->vec[7],
                          seed, 11, 12, 13, 14);
#endif /* MLD_CONFIG_PARAMETER_SET == 87 */
#endif /* !MLD_CONFIG_SERIAL_FIPS202_ONLY */
}

/*************************************************
 * Name:        mld_compute_t0_t1_tr_from_sk_components
 *
 * Description: Computes t0, t1, tr, and pk from secret key components
 *              rho, s1, s2. This is the shared computation used by
 *              both keygen and generating the public key from the
 *              secret key.
 *
 * Arguments:   - mld_polyveck *t0: output t0
 *              - mld_polyveck *t1: output t1
 *              - uint8_t tr[MLDSA_TRBYTES]: output tr
 *              - uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]: output public key
 *              - const uint8_t rho[MLDSA_SEEDBYTES]: input rho
 *              - const mld_polyvecl *s1: input s1
 *              - const mld_polyveck *s2: input s2
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
static int mld_compute_t0_t1_tr_from_sk_components(
    mld_polyveck *t0, mld_polyveck *t1, uint8_t tr[MLDSA_TRBYTES],
    uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES], const uint8_t rho[MLDSA_SEEDBYTES],
    const mld_polyvecl *s1, const mld_polyveck *s2,
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(memory_no_alias(t0, sizeof(mld_polyveck)))
  requires(memory_no_alias(t1, sizeof(mld_polyveck)))
  requires(memory_no_alias(tr, MLDSA_TRBYTES))
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  requires(memory_no_alias(rho, MLDSA_SEEDBYTES))
  requires(memory_no_alias(s1, sizeof(mld_polyvecl)))
  requires(memory_no_alias(s2, sizeof(mld_polyveck)))
  requires(forall(l0, 0, MLDSA_L, array_bound(s1->vec[l0].coeffs, 0, MLDSA_N, MLD_POLYETA_UNPACK_LOWER_BOUND, MLDSA_ETA + 1)))
  requires(forall(k0, 0, MLDSA_K, array_bound(s2->vec[k0].coeffs, 0, MLDSA_N, MLD_POLYETA_UNPACK_LOWER_BOUND, MLDSA_ETA + 1)))
  assigns(memory_slice(t0, sizeof(mld_polyveck)))
  assigns(memory_slice(t1, sizeof(mld_polyveck)))
  assigns(memory_slice(tr, MLDSA_TRBYTES))
  assigns(memory_slice(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  ensures(forall(k1, 0, MLDSA_K, array_bound(t0->vec[k1].coeffs, 0, MLDSA_N, -(1<<(MLDSA_D-1)) + 1, (1<<(MLDSA_D-1)) + 1)))
  ensures(forall(k2, 0, MLDSA_K, array_bound(t1->vec[k2].coeffs, 0, MLDSA_N, 0, 1 << 10)))
  ensures(return_value == 0 || return_value == MLD_ERR_OUT_OF_MEMORY))
{
  int ret;
  MLD_ALLOC(mat, mld_polymat, 1, context);
  MLD_ALLOC(s1hat, mld_polyvecl, 1, context);
  MLD_ALLOC(t, mld_polyveck, 1, context);

  if (mat == NULL || s1hat == NULL || t == NULL)
  {
    ret = MLD_ERR_OUT_OF_MEMORY;
    goto cleanup;
  }

  /* Expand matrix */
  mld_polyvec_matrix_expand(mat, rho);

  /* Matrix-vector multiplication */
  *s1hat = *s1;
  mld_polyvecl_ntt(s1hat);
  mld_polyvec_matrix_pointwise_montgomery(t, mat, s1hat);
  mld_polyveck_invntt_tomont(t);

  /* Add error vector s2 */
  mld_polyveck_add(t, s2);

  /* Reference: The following reduction is not present in the reference
   *            implementation. Omitting this reduction requires the output of
   *            the invntt to be small enough such that the addition of s2 does
   *            not result in absolute values >= MLDSA_Q. While our C, x86_64,
   *            and AArch64 invntt implementations produce small enough
   *            values for this to work out, it complicates the bounds
   *            reasoning. We instead add an additional reduction, and can
   *            consequently, relax the bounds requirements for the invntt.
   */
  mld_polyveck_reduce(t);

  /* Decompose to get t1, t0 */
  mld_polyveck_caddq(t);
  mld_polyveck_power2round(t1, t0, t);

  /* Pack public key and compute tr */
  mld_pack_pk(pk, rho, t1);
  mld_shake256(tr, MLDSA_TRBYTES, pk, MLDSA_CRYPTO_PUBLICKEYBYTES);

  ret = 0;

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  MLD_FREE(t, mld_polyveck, 1, context);
  MLD_FREE(s1hat, mld_polyvecl, 1, context);
  MLD_FREE(mat, mld_polymat, 1, context);
  return ret;
}

MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_keypair_internal(uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                              uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                              const uint8_t seed[MLDSA_SEEDBYTES],
                              MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  int ret;
  const uint8_t *rho, *rhoprime, *key;
  MLD_ALLOC(seedbuf, uint8_t, 2 * MLDSA_SEEDBYTES + MLDSA_CRHBYTES, context);
  MLD_ALLOC(inbuf, uint8_t, MLDSA_SEEDBYTES + 2, context);
  MLD_ALLOC(tr, uint8_t, MLDSA_TRBYTES, context);
  MLD_ALLOC(s1, mld_polyvecl, 1, context);
  MLD_ALLOC(s2, mld_polyveck, 1, context);
  MLD_ALLOC(t1, mld_polyveck, 1, context);
  MLD_ALLOC(t0, mld_polyveck, 1, context);

  if (seedbuf == NULL || inbuf == NULL || tr == NULL || s1 == NULL ||
      s2 == NULL || t1 == NULL || t0 == NULL)
  {
    ret = MLD_ERR_OUT_OF_MEMORY;
    goto cleanup;
  }

  /* Get randomness for rho, rhoprime and key */
  mld_memcpy(inbuf, seed, MLDSA_SEEDBYTES);
  inbuf[MLDSA_SEEDBYTES + 0] = MLDSA_K;
  inbuf[MLDSA_SEEDBYTES + 1] = MLDSA_L;
  mld_shake256(seedbuf, 2 * MLDSA_SEEDBYTES + MLDSA_CRHBYTES, inbuf,
               MLDSA_SEEDBYTES + 2);
  rho = seedbuf;
  rhoprime = rho + MLDSA_SEEDBYTES;
  key = rhoprime + MLDSA_CRHBYTES;

  /* Constant time: rho is part of the public key and, hence, public. */
  MLD_CT_TESTING_DECLASSIFY(rho, MLDSA_SEEDBYTES);

  /* Sample s1 and s2 */
  mld_sample_s1_s2(s1, s2, rhoprime);

  /* Compute t0, t1, tr, and pk from rho, s1, s2 */
  ret = mld_compute_t0_t1_tr_from_sk_components(t0, t1, tr, pk, rho, s1, s2,
                                                context);
  if (ret != 0)
  {
    goto cleanup;
  }

  /* Pack secret key */
  mld_pack_sk(sk, rho, tr, key, t0, s1, s2);

  /* Constant time: pk is the public key, inherently public data */
  MLD_CT_TESTING_DECLASSIFY(pk, MLDSA_CRYPTO_PUBLICKEYBYTES);

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  MLD_FREE(t0, mld_polyveck, 1, context);
  MLD_FREE(t1, mld_polyveck, 1, context);
  MLD_FREE(s2, mld_polyveck, 1, context);
  MLD_FREE(s1, mld_polyvecl, 1, context);
  MLD_FREE(tr, uint8_t, MLDSA_TRBYTES, context);
  MLD_FREE(inbuf, uint8_t, MLDSA_SEEDBYTES + 2, context);
  MLD_FREE(seedbuf, uint8_t, 2 * MLDSA_SEEDBYTES + MLDSA_CRHBYTES, context);

  if (ret != 0)
  {
    return ret;
  }

  /* Pairwise Consistency Test (PCT) @[FIPS140_3_IG, p.87] */
  /* Do this after freeing all temporaries. */
  return mld_check_pct(pk, sk, context);
}

#if !defined(MLD_CONFIG_NO_RANDOMIZED_API)
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_keypair(uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                     uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                     MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  MLD_ALIGN uint8_t seed[MLDSA_SEEDBYTES];
  int ret;
  if (mld_randombytes(seed, MLDSA_SEEDBYTES) != 0)
  {
    ret = MLD_ERR_RNG_FAIL;
    goto cleanup;
  }
  MLD_CT_TESTING_SECRET(seed, sizeof(seed));
  ret = mld_sign_keypair_internal(pk, sk, seed, context);

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(seed, sizeof(seed));
  return ret;
}
#endif /* !MLD_CONFIG_NO_RANDOMIZED_API */

/*************************************************
 * Name:        mld_H
 *
 * Description: Abstracts application of SHAKE256 to
 *              one, two or three blocks of data,
 *              yielding a user-requested size of
 *              output.
 *
 * Arguments:   - uint8_t *out: pointer to output
 *              - size_t outlen: requested output length in bytes
 *              - const uint8_t *in1: pointer to input block 1
 *                                    Must NOT be NULL
 *              - size_t in1len: length of input in1 bytes
 *              - const uint8_t *in2: pointer to input block 2
 *                                    May be NULL if in2len=0, in which case
 *                                    this block is ignored
 *              - size_t in2len: length of input in2 bytes
 *              - const uint8_t *in3: pointer to input block 3
 *                                    May be NULL if in3len=0, in which case
 *                                    this block is ignored
 *              - size_t in3len: length of input in3 bytes
 **************************************************/
static void mld_H(uint8_t *out, size_t outlen, const uint8_t *in1,
                  size_t in1len, const uint8_t *in2, size_t in2len,
                  const uint8_t *in3, size_t in3len)
__contract__(
  requires(in1len <= MLD_MAX_BUFFER_SIZE)
  requires(in2len <= MLD_MAX_BUFFER_SIZE)
  requires(in3len <= MLD_MAX_BUFFER_SIZE)
  requires(outlen <= 8 * SHAKE256_RATE /* somewhat arbitrary bound */)
  requires(memory_no_alias(in1, in1len))
  requires(in2len == 0 || memory_no_alias(in2, in2len))
  requires(in3len == 0 || memory_no_alias(in3, in3len))
  requires(memory_no_alias(out, outlen))
  assigns(memory_slice(out, outlen))
)
{
  mld_shake256ctx state;
  mld_shake256_init(&state);
  mld_shake256_absorb(&state, in1, in1len);
  if (in2len != 0)
  {
    mld_shake256_absorb(&state, in2, in2len);
  }
  if (in3len != 0)
  {
    mld_shake256_absorb(&state, in3, in3len);
  }
  mld_shake256_finalize(&state);
  mld_shake256_squeeze(out, outlen, &state);
  mld_shake256_release(&state);

  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(&state, sizeof(state));
}

/*************************************************
 * Name:        mld_compute_pack_z
 *
 * Description: Computes z = y + s1*c, checks that z has coefficients smaller
 *              than MLDSA_GAMMA1 - MLDSA_BETA, and packs z into the
 *              signature buffer.
 *
 * Arguments:   - uint8_t *sig: output signature
 *              - const mld_poly *cp: challenge polynomial
 *              - const polyvecl *s1: secret vector s1
 *              - const polyvecl *y: masking vector y
 *
 * Returns:     - 0: Success (z has coefficients smaller than
 *                   MLDSA_GAMMA1 - MLDSA_BETA,)
 *              - MLD_ERR_FAIL: z rejected (norm check failed)
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *
 * Reference: This function is inlined into mld_sign_signature in the
 *            reference implementation.
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
static int mld_compute_pack_z(uint8_t sig[MLDSA_CRYPTO_BYTES],
                              const mld_poly *cp, const mld_polyvecl *s1,
                              const mld_polyvecl *y, mld_poly *z)
__contract__(
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(cp, sizeof(mld_poly)))
  requires(memory_no_alias(s1, sizeof(mld_polyvecl)))
  requires(memory_no_alias(y, sizeof(mld_polyvecl)))
  requires(memory_no_alias(z, sizeof(mld_poly)))
  requires(array_abs_bound(cp->coeffs, 0, MLDSA_N, MLD_NTT_BOUND))
  requires(forall(k0, 0, MLDSA_L,
    array_bound(y->vec[k0].coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1)))
  requires(forall(k1, 0, MLDSA_L, array_abs_bound(s1->vec[k1].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
  assigns(memory_slice(z, sizeof(mld_poly)))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL ||
          return_value == MLD_ERR_OUT_OF_MEMORY)
)
{
  unsigned int i;
  uint32_t z_invalid;
  for (i = 0; i < MLDSA_L; i++)
  __loop__(
    assigns(i, memory_slice(z, sizeof(mld_poly)), memory_slice(sig, MLDSA_CRYPTO_BYTES))
    invariant(i <= MLDSA_L)
  )
  {
    mld_poly_pointwise_montgomery(z, cp, &s1->vec[i]);
    mld_poly_invntt_tomont(z);
    mld_poly_add(z, &y->vec[i]);
    mld_poly_reduce(z);

    z_invalid = mld_poly_chknorm(z, MLDSA_GAMMA1 - MLDSA_BETA);
    /* Constant time: It is fine (and prohibitively expensive to avoid)
     * to leak the result of the norm check and which polynomial in z caused a
     * rejection. It would even be okay to leak which coefficient led to
     * rejection as the candidate signature will be discarded anyway.
     * See Section 5.5 of @[Round3_Spec]. */
    MLD_CT_TESTING_DECLASSIFY(&z_invalid, sizeof(uint32_t));
    if (z_invalid)
    {
      return MLD_ERR_FAIL; /* reject */
    }
    /* If z is valid, then its coefficients are bounded by
     * MLDSA_GAMMA1 - MLDSA_BETA. This will be needed below
     * to prove the pre-condition of pack_sig_z() */
    mld_assert_abs_bound(z, MLDSA_N, (MLDSA_GAMMA1 - MLDSA_BETA));

    /* After the norm check, the distribution of each coefficient of z is
     * independent of the secret key and it can, hence, be considered
     * public. It is, hence, okay to immediately pack it into the user-provided
     * signature buffer. */
    mld_pack_sig_z(sig, z, i);
  }
  return 0;
}

/* Reference: The reference implementation does not explicitly check the
 * maximum nonce value, but instead loops indefinitely (even when the nonce
 * would overflow). Internally, sampling of y uses
 * (nonceL), (nonceL+1), ... (nonce*L+L-1).
 * Hence, there are no overflows if nonce < (UINT16_MAX - L)/L.
 * Explicitly checking for this explicitly allows us to prove type-safety.
 * Note that FIPS204 explicitly allows an upper-bound this loop of
 * 814 (< (UINT16_MAX - L)/L) - see @[FIPS204, Appendix C]. */
#define MLD_NONCE_UB ((UINT16_MAX - MLDSA_L) / MLDSA_L)

/*************************************************
 * Name:        attempt_signature_generation
 *
 * Description: Attempts to generate a single signature.
 *
 * Arguments:   - uint8_t *sig: pointer to output signature
 *              - const uint8_t *mu: pointer to message or hash
 *                                   of exactly MLDSA_CRHBYTES bytes
 *              - const uint8_t *rhoprime: pointer to randomness seed
 *              - uint16_t nonce: current nonce value
 *              - const mld_polymat *mat: expanded matrix
 *              - const polyvecl *s1: secret vector s1
 *              - const polyveck *s2: secret vector s2
 *              - const polyveck *t0: vector t0
 *
 * Returns:     - 0: Signature generation succeeded
 *              - MLD_ERR_FAIL: Signature rejected (norm check failed)
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *
 * Reference: This code differs from the reference implementation
 *            in that it factors out the core signature generation
 *            step into a distinct function here in order to improve
 *            efficiency of CBMC proof.
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
static int mld_attempt_signature_generation(
    uint8_t sig[MLDSA_CRYPTO_BYTES], const uint8_t *mu,
    const uint8_t rhoprime[MLDSA_CRHBYTES], uint16_t nonce, mld_polymat *mat,
    const mld_polyvecl *s1, const mld_polyveck *s2, const mld_polyveck *t0,
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(mu, MLDSA_CRHBYTES))
  requires(memory_no_alias(rhoprime, MLDSA_CRHBYTES))
  requires(memory_no_alias(mat, sizeof(mld_polymat)))
  requires(memory_no_alias(s1, sizeof(mld_polyvecl)))
  requires(memory_no_alias(s2, sizeof(mld_polyveck)))
  requires(memory_no_alias(t0, sizeof(mld_polyveck)))
  requires(nonce <= MLD_NONCE_UB)
  requires(forall(k1, 0, MLDSA_K, forall(l1, 0, MLDSA_L,
                                         array_bound(mat->vec[k1].vec[l1].coeffs, 0, MLDSA_N, 0, MLDSA_Q))))
  requires(forall(k2, 0, MLDSA_K, array_abs_bound(t0->vec[k2].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
  requires(forall(k3, 0, MLDSA_L, array_abs_bound(s1->vec[k3].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
  requires(forall(k4, 0, MLDSA_K, array_abs_bound(s2->vec[k4].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL ||
          return_value == MLD_ERR_OUT_OF_MEMORY)
)
{
  unsigned int n;
  uint32_t w0_invalid, h_invalid;
  int ret;
  /* TODO: Remove the following workaround for
   * https://github.com/diffblue/cbmc/issues/8813 */
  typedef MLD_UNION_OR_STRUCT
  {
    mld_polyvecl y;
    mld_polyveck h;
  }
  yh_u;
  mld_polyvecl *y;
  mld_polyveck *h;

  /* TODO: Remove the following workaround for
   * https://github.com/diffblue/cbmc/issues/8813 */
  typedef MLD_UNION_OR_STRUCT
  {
    mld_polyveck w1;
    mld_polyvecl tmp;
  }
  w1tmp_u;
  mld_polyveck *w1;
  mld_polyvecl *tmp;

  MLD_ALLOC(challenge_bytes, uint8_t, MLDSA_CTILDEBYTES, context);
  MLD_ALLOC(yh, yh_u, 1, context);
  MLD_ALLOC(z, mld_poly, 1, context);
  MLD_ALLOC(w1tmp, w1tmp_u, 1, context);
  MLD_ALLOC(w0, mld_polyveck, 1, context);
  MLD_ALLOC(cp, mld_poly, 1, context);
  MLD_ALLOC(t, mld_poly, 1, context);

  if (challenge_bytes == NULL || yh == NULL || z == NULL || w1tmp == NULL ||
      w0 == NULL || cp == NULL || t == NULL)
  {
    ret = MLD_ERR_OUT_OF_MEMORY;
    goto cleanup;
  }
  y = &yh->y;
  h = &yh->h;
  w1 = &w1tmp->w1;
  tmp = &w1tmp->tmp;

  /* Sample intermediate vector y */
  mld_polyvecl_uniform_gamma1(y, rhoprime, nonce);

  /* Matrix-vector multiplication */
  *tmp = *y;
  mld_polyvecl_ntt(tmp);
  mld_polyvec_matrix_pointwise_montgomery(w0, mat, tmp);
  mld_polyveck_invntt_tomont(w0);

  /* Decompose w and call the random oracle */
  mld_polyveck_caddq(w0);
  mld_polyveck_decompose(w1, w0);
  mld_polyveck_pack_w1(sig, w1);

  mld_H(challenge_bytes, MLDSA_CTILDEBYTES, mu, MLDSA_CRHBYTES, sig,
        MLDSA_K * MLDSA_POLYW1_PACKEDBYTES, NULL, 0);
  /* Constant time: Leaking challenge_bytes does not reveal any information
   * about the secret key as H() is modelled as random oracle.
   * This also applies to challenges for rejected signatures.
   * See Section 5.5 of @[Round3_Spec]. */
  MLD_CT_TESTING_DECLASSIFY(challenge_bytes, MLDSA_CTILDEBYTES);
  mld_poly_challenge(cp, challenge_bytes);
  mld_poly_ntt(cp);

  /* Compute z, reject if it reveals secret */
  ret = mld_compute_pack_z(sig, cp, s1, y, t);
  if (ret)
  {
    goto cleanup;
  }

  /* Check that subtracting cs2 does not change high bits of w and low bits
   * do not reveal secret information */
  mld_polyveck_pointwise_poly_montgomery(h, cp, s2);
  mld_polyveck_invntt_tomont(h);
  mld_polyveck_sub(w0, h);
  mld_polyveck_reduce(w0);

  w0_invalid = mld_polyveck_chknorm(w0, MLDSA_GAMMA2 - MLDSA_BETA);
  /* Constant time: w0_invalid may be leaked - see comment for z_invalid. */
  MLD_CT_TESTING_DECLASSIFY(&w0_invalid, sizeof(uint32_t));
  if (w0_invalid)
  {
    ret = MLD_ERR_FAIL; /* reject */
    goto cleanup;
  }

  /* Compute hints for w1 */
  mld_polyveck_pointwise_poly_montgomery(h, cp, t0);
  mld_polyveck_invntt_tomont(h);
  mld_polyveck_reduce(h);

  h_invalid = mld_polyveck_chknorm(h, MLDSA_GAMMA2);
  /* Constant time: h_invalid may be leaked - see comment for z_invalid. */
  MLD_CT_TESTING_DECLASSIFY(&h_invalid, sizeof(uint32_t));
  if (h_invalid)
  {
    ret = MLD_ERR_FAIL; /* reject */
    goto cleanup;
  }

  mld_polyveck_add(w0, h);

  /* Constant time: At this point all norm checks have passed and we, hence,
   * know that the signature does not leak any secret information.
   * Consequently, any value that can be computed from the signature and public
   * key is considered public.
   * w0 and w1 are public as they can be computed from Az - ct = \alpha w1 + w0.
   * h=c*t0 is public as both c and t0 are public.
   * For a more detailed discussion, refer to https://eprint.iacr.org/2022/1406.
   */
  MLD_CT_TESTING_DECLASSIFY(w0, sizeof(*w0));
  MLD_CT_TESTING_DECLASSIFY(w1, sizeof(*w1));
  n = mld_polyveck_make_hint(h, w0, w1);
  if (n > MLDSA_OMEGA)
  {
    ret = MLD_ERR_FAIL; /* reject */
    goto cleanup;
  }

  /* All is well - write signature */
  mld_pack_sig_c_h(sig, challenge_bytes, h, n);
  /* Constant time: At this point it is clear that the signature is valid - it
   * can, hence, be considered public. */
  MLD_CT_TESTING_DECLASSIFY(sig, MLDSA_CRYPTO_BYTES);
  ret = 0; /* success */

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  MLD_FREE(t, mld_poly, 1, context);
  MLD_FREE(cp, mld_poly, 1, context);
  MLD_FREE(w0, mld_polyveck, 1, context);
  MLD_FREE(w1tmp, w1tmp_u, 1, context);
  MLD_FREE(z, mld_poly, 1, context);
  MLD_FREE(yh, yh_u, 1, context);
  MLD_FREE(challenge_bytes, uint8_t, MLDSA_CTILDEBYTES, context);

  return ret;
}
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature_internal(uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen,
                                const uint8_t *m, size_t mlen,
                                const uint8_t *pre, size_t prelen,
                                const uint8_t rnd[MLDSA_RNDBYTES],
                                const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                                int externalmu,
                                MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  int ret;
  uint8_t *rho, *tr, *key, *mu, *rhoprime;
  uint16_t nonce = 0;
  MLD_ALLOC(seedbuf, uint8_t,
            2 * MLDSA_SEEDBYTES + MLDSA_TRBYTES + 2 * MLDSA_CRHBYTES, context);
  MLD_ALLOC(mat, mld_polymat, 1, context);
  MLD_ALLOC(s1, mld_polyvecl, 1, context);
  MLD_ALLOC(t0, mld_polyveck, 1, context);
  MLD_ALLOC(s2, mld_polyveck, 1, context);

  if (seedbuf == NULL || mat == NULL || s1 == NULL || t0 == NULL || s2 == NULL)
  {
    ret = MLD_ERR_OUT_OF_MEMORY;
    goto cleanup;
  }

  rho = seedbuf;
  tr = rho + MLDSA_SEEDBYTES;
  key = tr + MLDSA_TRBYTES;
  mu = key + MLDSA_SEEDBYTES;
  rhoprime = mu + MLDSA_CRHBYTES;
  mld_unpack_sk(rho, tr, key, t0, s1, s2, sk);

  if (!externalmu)
  {
    /* Compute mu = CRH(tr, pre, msg) */
    mld_H(mu, MLDSA_CRHBYTES, tr, MLDSA_TRBYTES, pre, prelen, m, mlen);
  }
  else
  {
    /* mu has been provided directly */
    mld_memcpy(mu, m, MLDSA_CRHBYTES);
  }

  /* Compute rhoprime = CRH(key, rnd, mu) */
  mld_H(rhoprime, MLDSA_CRHBYTES, key, MLDSA_SEEDBYTES, rnd, MLDSA_RNDBYTES, mu,
        MLDSA_CRHBYTES);

  /* Constant time: rho is part of the public key and, hence, public. */
  MLD_CT_TESTING_DECLASSIFY(rho, MLDSA_SEEDBYTES);
  /* Expand matrix and transform vectors */
  mld_polyvec_matrix_expand(mat, rho);
  mld_polyvecl_ntt(s1);
  mld_polyveck_ntt(s2);
  mld_polyveck_ntt(t0);

  /* By default, return failure. Flip to success and write output
   * once signature generation succeeds. */
  ret = MLD_ERR_FAIL;

  /* Reference: This code is re-structured using a while(1),  */
  /* with explicit "continue" statements (rather than "goto") */
  /* to implement rejection of invalid signatures.            */
  while (1)
  __loop__(
    assigns(nonce, ret, object_whole(siglen), memory_slice(sig, MLDSA_CRYPTO_BYTES))
    invariant(nonce <= MLD_NONCE_UB)

    /* t0, s1, s2, and mat are initialized above and are NOT changed by this */
    /* loop. We can therefore re-assert their bounds here as part of the     */
    /* loop invariant. This makes proof noticeably faster with CBMC          */
    invariant(forall(k1, 0, MLDSA_K, forall(l1, 0, MLDSA_L,
              array_bound(mat->vec[k1].vec[l1].coeffs, 0, MLDSA_N, 0, MLDSA_Q))))
    invariant(forall(k2, 0, MLDSA_K, array_abs_bound(t0->vec[k2].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
    invariant(forall(k3, 0, MLDSA_L, array_abs_bound(s1->vec[k3].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
    invariant(forall(k4, 0, MLDSA_K, array_abs_bound(s2->vec[k4].coeffs, 0, MLDSA_N, MLD_NTT_BOUND)))
    invariant(ret == MLD_ERR_FAIL)
  )
  {
    /* Reference: this code explicitly checks for exhaustion of nonce     */
    /* values to provide predictable termination and results in that case */
    /* Checking here also means that incrementing nonce below can also    */
    /* be proven to be type-safe.                                         */
    if (nonce == MLD_NONCE_UB)
    {
      /* Note that ret == MLD_ERR_FAIL by default, so we
       * don't need to set it here. */
      break;
    }

    ret = mld_attempt_signature_generation(sig, mu, rhoprime, nonce, mat, s1,
                                           s2, t0, context);
    nonce++;
    if (ret == 0)
    {
      *siglen = MLDSA_CRYPTO_BYTES;
      break;
    }
    else if (ret != MLD_ERR_FAIL)
    {
      /* For failures such as out-of-memory, propagate and exit immediately. */
      break;
    }

    /* Otherwise, try again. */
  }

cleanup:

  if (ret != 0)
  {
    /* To be on the safe-side, we zeroize the signature buffer. */
    *siglen = 0;
    mld_memset(sig, 0, MLDSA_CRYPTO_BYTES);
  }

  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  MLD_FREE(s2, mld_polyveck, 1, context);
  MLD_FREE(t0, mld_polyveck, 1, context);
  MLD_FREE(s1, mld_polyvecl, 1, context);
  MLD_FREE(mat, mld_polymat, 1, context);
  MLD_FREE(seedbuf, uint8_t,
           2 * MLDSA_SEEDBYTES + MLDSA_TRBYTES + 2 * MLDSA_CRHBYTES, context);
  return ret;
}

#if !defined(MLD_CONFIG_NO_RANDOMIZED_API)
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature(uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *ctx,
                       size_t ctxlen,
                       const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                       MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  size_t pre_len;
  int ret;
  MLD_ALLOC(pre, uint8_t, MLD_DOMAIN_SEPARATION_MAX_BYTES, context);
  MLD_ALLOC(rnd, uint8_t, MLDSA_RNDBYTES, context);

  if (pre == NULL || rnd == NULL)
  {
    ret = MLD_ERR_OUT_OF_MEMORY;
    goto cleanup;
  }

  /* Prepare domain separation prefix for pure ML-DSA */
  pre_len = mld_prepare_domain_separation_prefix(pre, NULL, 0, ctx, ctxlen,
                                                 MLD_PREHASH_NONE);
  if (pre_len == 0)
  {
    ret = MLD_ERR_FAIL;
    goto cleanup;
  }

  /* Randomized variant of ML-DSA. If you need the deterministic variant,
   * call mld_sign_signature_internal directly with all-zero rnd. */
  if (mld_randombytes(rnd, MLDSA_RNDBYTES) != 0)
  {
    ret = MLD_ERR_RNG_FAIL;
    goto cleanup;
  }
  MLD_CT_TESTING_SECRET(rnd, sizeof(rnd));

  ret = mld_sign_signature_internal(sig, siglen, m, mlen, pre, pre_len, rnd, sk,
                                    0, context);

cleanup:
  if (ret != 0)
  {
    /* To be on the safe-side, make sure *siglen and sig have a well-defined
     * value, even in the case of error.
     *
     * If we come from mld_sign_signature_internal, both are redundant,
     * but the error case should not be the norm, and the added cost of the
     * memset insignificant. */
    *siglen = 0;
    mld_memset(sig, 0, MLDSA_CRYPTO_BYTES);
  }

  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  MLD_FREE(rnd, uint8_t, MLDSA_RNDBYTES, context);
  MLD_FREE(pre, uint8_t, MLD_DOMAIN_SEPARATION_MAX_BYTES, context);

  return ret;
}
#endif /* !MLD_CONFIG_NO_RANDOMIZED_API */

#if !defined(MLD_CONFIG_NO_RANDOMIZED_API)
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature_extmu(uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen,
                             const uint8_t mu[MLDSA_CRHBYTES],
                             const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                             MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  MLD_ALIGN uint8_t rnd[MLDSA_RNDBYTES];
  int ret;

  /* Randomized variant of ML-DSA. If you need the deterministic variant,
   * call mld_sign_signature_internal directly with all-zero rnd. */
  if (mld_randombytes(rnd, MLDSA_RNDBYTES) != 0)
  {
    *siglen = 0;
    ret = MLD_ERR_RNG_FAIL;
    goto cleanup;
  }
  MLD_CT_TESTING_SECRET(rnd, sizeof(rnd));

  ret = mld_sign_signature_internal(sig, siglen, mu, MLDSA_CRHBYTES, NULL, 0,
                                    rnd, sk, 1, context);

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(rnd, sizeof(rnd));

  return ret;
}
#endif /* !MLD_CONFIG_NO_RANDOMIZED_API */

#if !defined(MLD_CONFIG_NO_RANDOMIZED_API)
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign(uint8_t *sm, size_t *smlen, const uint8_t *m, size_t mlen,
             const uint8_t *ctx, size_t ctxlen,
             const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
             MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  int ret;
  size_t i;

  for (i = 0; i < mlen; ++i)
  __loop__(
    assigns(i, object_whole(sm))
    invariant(i <= mlen)
  )
  {
    sm[MLDSA_CRYPTO_BYTES + mlen - 1 - i] = m[mlen - 1 - i];
  }
  ret = mld_sign_signature(sm, smlen, sm + MLDSA_CRYPTO_BYTES, mlen, ctx,
                           ctxlen, sk, context);
  *smlen += mlen;
  return ret;
}
#endif /* !MLD_CONFIG_NO_RANDOMIZED_API */

MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify_internal(const uint8_t *sig, size_t siglen,
                             const uint8_t *m, size_t mlen, const uint8_t *pre,
                             size_t prelen,
                             const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                             int externalmu,
                             MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  int ret, cmp;

  /* TODO: Remove the following workaround for
   * https://github.com/diffblue/cbmc/issues/8813 */
  typedef MLD_UNION_OR_STRUCT
  {
    mld_polyveck t1;
    mld_polyveck w1;
  }
  t1w1_u;
  mld_polyveck *t1;
  mld_polyveck *w1;

  MLD_ALLOC(buf, uint8_t, (MLDSA_K * MLDSA_POLYW1_PACKEDBYTES), context);
  MLD_ALLOC(rho, uint8_t, MLDSA_SEEDBYTES, context);
  MLD_ALLOC(mu, uint8_t, MLDSA_CRHBYTES, context);
  MLD_ALLOC(c, uint8_t, MLDSA_CTILDEBYTES, context);
  MLD_ALLOC(c2, uint8_t, MLDSA_CTILDEBYTES, context);
  MLD_ALLOC(cp, mld_poly, 1, context);
  MLD_ALLOC(mat, mld_polymat, 1, context);
  MLD_ALLOC(z, mld_polyvecl, 1, context);
  MLD_ALLOC(t1w1, t1w1_u, 1, context);
  MLD_ALLOC(tmp, mld_polyveck, 1, context);
  MLD_ALLOC(h, mld_polyveck, 1, context);

  if (buf == NULL || rho == NULL || mu == NULL || c == NULL || c2 == NULL ||
      cp == NULL || mat == NULL || z == NULL || t1w1 == NULL || tmp == NULL ||
      h == NULL)
  {
    ret = MLD_ERR_OUT_OF_MEMORY;
    goto cleanup;
  }
  t1 = &t1w1->t1;
  w1 = &t1w1->w1;

  if (siglen != MLDSA_CRYPTO_BYTES)
  {
    ret = MLD_ERR_FAIL;
    goto cleanup;
  }

  mld_unpack_pk(rho, t1, pk);

  /* mld_unpack_sig and mld_polyvecl_chknorm signal failure through a
   * single non-zero error code that's not yet aligned with MLD_ERR_XXX.
   * Map it to MLD_ERR_FAIL explicitly. */
  if (mld_unpack_sig(c, z, h, sig))
  {
    ret = MLD_ERR_FAIL;
    goto cleanup;
  }
  if (mld_polyvecl_chknorm(z, MLDSA_GAMMA1 - MLDSA_BETA))
  {
    ret = MLD_ERR_FAIL;
    goto cleanup;
  }

  if (!externalmu)
  {
    /* Compute CRH(H(rho, t1), pre, msg) */
    MLD_ALIGN uint8_t hpk[MLDSA_CRHBYTES];
    mld_H(hpk, MLDSA_TRBYTES, pk, MLDSA_CRYPTO_PUBLICKEYBYTES, NULL, 0, NULL,
          0);
    mld_H(mu, MLDSA_CRHBYTES, hpk, MLDSA_TRBYTES, pre, prelen, m, mlen);

    /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
    mld_zeroize(hpk, sizeof(hpk));
  }
  else
  {
    /* mu has been provided directly */
    mld_memcpy(mu, m, MLDSA_CRHBYTES);
  }

  /* Matrix-vector multiplication; compute Az - c2^dt1 */
  mld_poly_challenge(cp, c);
  mld_poly_ntt(cp);
  mld_polyveck_shiftl(t1);
  mld_polyveck_ntt(t1);
  mld_polyveck_pointwise_poly_montgomery(tmp, cp, t1);

  mld_polyvec_matrix_expand(mat, rho);
  mld_polyvecl_ntt(z);
  mld_polyvec_matrix_pointwise_montgomery(w1, mat, z);
  mld_polyveck_sub(w1, tmp);
  mld_polyveck_reduce(w1);
  mld_polyveck_invntt_tomont(w1);

  /* Reconstruct w1 */
  mld_polyveck_caddq(w1);
  mld_polyveck_use_hint(tmp, w1, h);
  mld_polyveck_pack_w1(buf, tmp);
  /* Call random oracle and verify challenge */
  mld_H(c2, MLDSA_CTILDEBYTES, mu, MLDSA_CRHBYTES, buf,
        MLDSA_K * MLDSA_POLYW1_PACKEDBYTES, NULL, 0);

  cmp = mld_ct_memcmp(c, c2, MLDSA_CTILDEBYTES);

  /* Declassify the result of the verification. */
  MLD_CT_TESTING_DECLASSIFY(&cmp, sizeof(cmp));

  ret = cmp == 0 ? 0 : MLD_ERR_FAIL;

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  MLD_FREE(h, mld_polyveck, 1, context);
  MLD_FREE(tmp, mld_polyveck, 1, context);
  MLD_FREE(t1w1, t1w1_u, 1, context);
  MLD_FREE(z, mld_polyvecl, 1, context);
  MLD_FREE(mat, mld_polymat, 1, context);
  MLD_FREE(cp, mld_poly, 1, context);
  MLD_FREE(c2, uint8_t, MLDSA_CTILDEBYTES, context);
  MLD_FREE(c, uint8_t, MLDSA_CTILDEBYTES, context);
  MLD_FREE(mu, uint8_t, MLDSA_CRHBYTES, context);
  MLD_FREE(rho, uint8_t, MLDSA_SEEDBYTES, context);
  MLD_FREE(buf, uint8_t, (MLDSA_K * MLDSA_POLYW1_PACKEDBYTES), context);
  return ret;
}

MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m,
                    size_t mlen, const uint8_t *ctx, size_t ctxlen,
                    const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  MLD_ALIGN uint8_t pre[MLD_DOMAIN_SEPARATION_MAX_BYTES];
  size_t pre_len;
  int ret;

  pre_len = mld_prepare_domain_separation_prefix(pre, NULL, 0, ctx, ctxlen,
                                                 MLD_PREHASH_NONE);
  if (pre_len == 0)
  {
    ret = MLD_ERR_FAIL;
    goto cleanup;
  }

  ret = mld_sign_verify_internal(sig, siglen, m, mlen, pre, pre_len, pk, 0,
                                 context);

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(pre, sizeof(pre));

  return ret;
}

MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify_extmu(const uint8_t *sig, size_t siglen,
                          const uint8_t mu[MLDSA_CRHBYTES],
                          const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                          MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  return mld_sign_verify_internal(sig, siglen, mu, MLDSA_CRHBYTES, NULL, 0, pk,
                                  1, context);
}

MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_open(uint8_t *m, size_t *mlen, const uint8_t *sm, size_t smlen,
                  const uint8_t *ctx, size_t ctxlen,
                  const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                  MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  int ret;
  size_t i;

  if (smlen < MLDSA_CRYPTO_BYTES)
  {
    ret = MLD_ERR_FAIL;
    goto cleanup;
  }

  *mlen = smlen - MLDSA_CRYPTO_BYTES;
  ret = mld_sign_verify(sm, MLDSA_CRYPTO_BYTES, sm + MLDSA_CRYPTO_BYTES, *mlen,
                        ctx, ctxlen, pk, context);
  if (ret == 0)
  {
    /* All good, copy msg, return 0 */
    for (i = 0; i < *mlen; ++i)
    __loop__(
      assigns(i, memory_slice(m, *mlen))
      invariant(i <= *mlen)
    )
    {
      m[i] = sm[MLDSA_CRYPTO_BYTES + i];
    }
  }

cleanup:

  if (ret != 0)
  {
    /* To be on the safe-side, we zeroize the message buffer. */
    *mlen = 0;
    mld_memset(m, 0, smlen);
  }

  return ret;
}


MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature_pre_hash_internal(
    uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen, const uint8_t *ph,
    size_t phlen, const uint8_t *ctx, size_t ctxlen,
    const uint8_t rnd[MLDSA_RNDBYTES],
    const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES], int hashalg,
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  MLD_ALIGN uint8_t pre[MLD_DOMAIN_SEPARATION_MAX_BYTES];
  size_t pre_len;
  int ret;

  pre_len = mld_prepare_domain_separation_prefix(pre, ph, phlen, ctx, ctxlen,
                                                 hashalg);
  if (pre_len == 0)
  {
    ret = MLD_ERR_FAIL;
    goto cleanup;
  }

  ret = mld_sign_signature_internal(sig, siglen, pre, pre_len, NULL, 0, rnd, sk,
                                    0, context);
cleanup:
  if (ret != 0)
  {
    /* To be on the safe-side, make sure *siglen and sig have a well-defined
     * value, even in the case of error.
     *
     * If we come from mld_sign_signature_internal, both are redundant,
     * but the error case should not be the norm, and the added cost of the
     * memset insignificant. */
    *siglen = 0;
    mld_memset(sig, 0, MLDSA_CRYPTO_BYTES);
  }

  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(pre, sizeof(pre));
  return ret;
}

MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify_pre_hash_internal(
    const uint8_t *sig, size_t siglen, const uint8_t *ph, size_t phlen,
    const uint8_t *ctx, size_t ctxlen,
    const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES], int hashalg,
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  MLD_ALIGN uint8_t pre[MLD_DOMAIN_SEPARATION_MAX_BYTES];
  size_t pre_len;
  int ret;

  pre_len = mld_prepare_domain_separation_prefix(pre, ph, phlen, ctx, ctxlen,
                                                 hashalg);
  if (pre_len == 0)
  {
    ret = MLD_ERR_FAIL;
    goto cleanup;
  }

  ret = mld_sign_verify_internal(sig, siglen, pre, pre_len, NULL, 0, pk, 0,
                                 context);

cleanup:
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(pre, sizeof(pre));
  return ret;
}

MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature_pre_hash_shake256(
    uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen, const uint8_t *m,
    size_t mlen, const uint8_t *ctx, size_t ctxlen,
    const uint8_t rnd[MLDSA_RNDBYTES],
    const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  MLD_ALIGN uint8_t ph[64];
  int ret;
  mld_shake256(ph, sizeof(ph), m, mlen);
  ret = mld_sign_signature_pre_hash_internal(sig, siglen, ph, sizeof(ph), ctx,
                                             ctxlen, rnd, sk,
                                             MLD_PREHASH_SHAKE_256, context);
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(ph, sizeof(ph));
  return ret;
}

MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify_pre_hash_shake256(
    const uint8_t *sig, size_t siglen, const uint8_t *m, size_t mlen,
    const uint8_t *ctx, size_t ctxlen,
    const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  MLD_ALIGN uint8_t ph[64];
  int ret;
  mld_shake256(ph, sizeof(ph), m, mlen);
  ret = mld_sign_verify_pre_hash_internal(sig, siglen, ph, sizeof(ph), ctx,
                                          ctxlen, pk, MLD_PREHASH_SHAKE_256,
                                          context);
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(ph, sizeof(ph));
  return ret;
}


#define MLD_PRE_HASH_OID_LEN 11

/*************************************************
 * Name:        mld_get_hash_oid
 *
 * Description: Returns the OID of a given SHA-2/SHA-3 hash function.
 *
 * Arguments:   - uint8_t oid[11]: pointer to output oid
 *              - int hashalg: hash algorithm constant (MLD_PREHASH_*)
 *
 ***************************************************/
static void mld_get_hash_oid(uint8_t oid[MLD_PRE_HASH_OID_LEN], int hashalg)
{
  unsigned int i;
  static const struct
  {
    int alg;
    uint8_t oid[MLD_PRE_HASH_OID_LEN];
  } oid_map[] = {
      {MLD_PREHASH_SHA2_224,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04}},
      {MLD_PREHASH_SHA2_256,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01}},
      {MLD_PREHASH_SHA2_384,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02}},
      {MLD_PREHASH_SHA2_512,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03}},
      {MLD_PREHASH_SHA2_512_224,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x05}},
      {MLD_PREHASH_SHA2_512_256,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x06}},
      {MLD_PREHASH_SHA3_224,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x07}},
      {MLD_PREHASH_SHA3_256,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x08}},
      {MLD_PREHASH_SHA3_384,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x09}},
      {MLD_PREHASH_SHA3_512,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x0A}},
      {MLD_PREHASH_SHAKE_128,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x0B}},
      {MLD_PREHASH_SHAKE_256,
       {0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x0C}}};

  for (i = 0; i < sizeof(oid_map) / sizeof(oid_map[0]); i++)
  __loop__(
    invariant(i <= sizeof(oid_map) / sizeof(oid_map[0]))
  )
  {
    if (oid_map[i].alg == hashalg)
    {
      mld_memcpy(oid, oid_map[i].oid, MLD_PRE_HASH_OID_LEN);
      return;
    }
  }
}

static int mld_validate_hash_length(int hashalg, size_t len)
{
  switch (hashalg)
  {
    case MLD_PREHASH_SHA2_224:
      return (len == 224 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA2_256:
      return (len == 256 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA2_384:
      return (len == 384 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA2_512:
      return (len == 512 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA2_512_224:
      return (len == 224 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA2_512_256:
      return (len == 256 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA3_224:
      return (len == 224 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA3_256:
      return (len == 256 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA3_384:
      return (len == 384 / 8) ? 0 : -1;
    case MLD_PREHASH_SHA3_512:
      return (len == 512 / 8) ? 0 : -1;
    case MLD_PREHASH_SHAKE_128:
      return (len == 256 / 8) ? 0 : -1;
    case MLD_PREHASH_SHAKE_256:
      return (len == 512 / 8) ? 0 : -1;
  }
  return -1;
}

size_t mld_prepare_domain_separation_prefix(
    uint8_t prefix[MLD_DOMAIN_SEPARATION_MAX_BYTES], const uint8_t *ph,
    size_t phlen, const uint8_t *ctx, size_t ctxlen, int hashalg)
{
  if (ctxlen > 255)
  {
    return 0;
  }

  if (hashalg != MLD_PREHASH_NONE)
  {
    if (ph == NULL || mld_validate_hash_length(hashalg, phlen) != 0)
    {
      return 0;
    }
  }

  /* Common prefix: 0x00/0x01 || ctxlen || ctx */
  prefix[0] = (hashalg == MLD_PREHASH_NONE) ? 0 : 1;
  prefix[1] = (uint8_t)ctxlen;
  if (ctxlen > 0)
  {
    mld_memcpy(prefix + 2, ctx, ctxlen);
  }

  if (hashalg == MLD_PREHASH_NONE)
  {
    return 2 + ctxlen;
  }

  /* HashML-DSA: append oid || ph */
  mld_get_hash_oid(prefix + 2 + ctxlen, hashalg);
  mld_memcpy(prefix + 2 + ctxlen + MLD_PRE_HASH_OID_LEN, ph, phlen);
  return 2 + ctxlen + MLD_PRE_HASH_OID_LEN + phlen;
}

MLD_EXTERNAL_API
int mld_sign_pk_from_sk(uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                        const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                        MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
{
  uint8_t check, cmp0, cmp1, chk1, chk2;
  int ret;
  MLD_ALLOC(rho, uint8_t, MLDSA_SEEDBYTES, context);
  MLD_ALLOC(tr, uint8_t, MLDSA_TRBYTES, context);
  MLD_ALLOC(tr_computed, uint8_t, MLDSA_TRBYTES, context);
  MLD_ALLOC(key, uint8_t, MLDSA_SEEDBYTES, context);
  MLD_ALLOC(s1, mld_polyvecl, 1, context);
  MLD_ALLOC(s2, mld_polyveck, 1, context);
  MLD_ALLOC(t0, mld_polyveck, 1, context);
  MLD_ALLOC(t0_computed, mld_polyveck, 1, context);
  MLD_ALLOC(t1, mld_polyveck, 1, context);

  if (rho == NULL || tr == NULL || tr_computed == NULL || key == NULL ||
      s1 == NULL || s2 == NULL || t0 == NULL || t0_computed == NULL ||
      t1 == NULL)
  {
    ret = MLD_ERR_OUT_OF_MEMORY;
    goto cleanup;
  }

  /* Unpack secret key */
  mld_unpack_sk(rho, tr, key, t0, s1, s2, sk);

  /* Validate s1 and s2 coefficients are within [-MLDSA_ETA, MLDSA_ETA] */
  chk1 = mld_polyvecl_chknorm(s1, MLDSA_ETA + 1) & 0xFF;
  chk2 = mld_polyveck_chknorm(s2, MLDSA_ETA + 1) & 0xFF;

  /* Recompute t0, t1, tr, and pk from rho, s1, s2 */
  ret = mld_compute_t0_t1_tr_from_sk_components(t0_computed, t1, tr_computed,
                                                pk, rho, s1, s2, context);
  if (ret != 0)
  {
    goto cleanup;
  }

  /* Validate t0 and tr using constant-time comparisons */
  cmp0 = mld_ct_memcmp((const uint8_t *)t0, (const uint8_t *)t0_computed,
                       sizeof(mld_polyveck));
  cmp1 = mld_ct_memcmp((const uint8_t *)tr, (const uint8_t *)tr_computed,
                       MLDSA_TRBYTES);
  check = mld_value_barrier_u8(cmp0 | cmp1 | chk1 | chk2);

  /* Declassify the final result of the validity check. */
  MLD_CT_TESTING_DECLASSIFY(&check, sizeof(check));
  ret = (check != 0) ? MLD_ERR_FAIL : 0;

cleanup:

  if (ret != 0)
  {
    mld_zeroize(pk, MLDSA_CRYPTO_PUBLICKEYBYTES);
  }

  /* Constant time: pk is either the valid public key or zeroed on error */
  MLD_CT_TESTING_DECLASSIFY(pk, MLDSA_CRYPTO_PUBLICKEYBYTES);

  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  MLD_FREE(t1, mld_polyveck, 1, context);
  MLD_FREE(t0_computed, mld_polyveck, 1, context);
  MLD_FREE(t0, mld_polyveck, 1, context);
  MLD_FREE(s2, mld_polyveck, 1, context);
  MLD_FREE(s1, mld_polyvecl, 1, context);
  MLD_FREE(key, uint8_t, MLDSA_SEEDBYTES, context);
  MLD_FREE(tr_computed, uint8_t, MLDSA_TRBYTES, context);
  MLD_FREE(tr, uint8_t, MLDSA_TRBYTES, context);
  MLD_FREE(rho, uint8_t, MLDSA_SEEDBYTES, context);

  return ret;
}

/* To facilitate single-compilation-unit (SCU) builds, undefine all macros.
 * Don't modify by hand -- this is auto-generated by scripts/autogen. */
#undef mld_check_pct
#undef mld_sample_s1_s2
#undef mld_validate_hash_length
#undef mld_get_hash_oid
#undef mld_H
#undef mld_compute_pack_z
#undef mld_attempt_signature_generation
#undef mld_compute_t0_t1_tr_from_sk_components
#undef MLD_NONCE_UB
#undef MLD_PRE_HASH_OID_LEN
