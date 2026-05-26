/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/* References
 * ==========
 *
 * - [FIPS204]
 *   FIPS 204 Module-Lattice-Based Digital Signature Standard
 *   National Institute of Standards and Technology
 *   https://csrc.nist.gov/pubs/fips/204/final
 */

#ifndef MLD_SIGN_H
#define MLD_SIGN_H

#include <stddef.h>
#include <stdint.h>
#include "cbmc.h"
#include "common.h"
#include "poly.h"
#include "polyvec.h"
#include "sys.h"

#if defined(MLD_CHECK_APIS)
/* Include to ensure consistency between internal sign.h
 * and external mldsa_native.h. */
#include "mldsa_native.h"

#if MLDSA_CRYPTO_SECRETKEYBYTES != \
    MLDSA_SECRETKEYBYTES(MLD_CONFIG_PARAMETER_SET)
#error Mismatch for SECRETKEYBYTES between sign.h and mldsa_native.h
#endif

#if MLDSA_CRYPTO_PUBLICKEYBYTES != \
    MLDSA_PUBLICKEYBYTES(MLD_CONFIG_PARAMETER_SET)
#error Mismatch for PUBLICKEYBYTES between sign.h and mldsa_native.h
#endif

#if MLDSA_CRYPTO_BYTES != MLDSA_BYTES(MLD_CONFIG_PARAMETER_SET)
#error Mismatch for CRYPTO_BYTES between sign.h and mldsa_native.h
#endif

#endif /* MLD_CHECK_APIS */

#define mld_sign_keypair_internal \
  MLD_NAMESPACE_KL(keypair_internal) MLD_CONTEXT_PARAMETERS_3
#define mld_sign_keypair MLD_NAMESPACE_KL(keypair) MLD_CONTEXT_PARAMETERS_2
#define mld_sign_signature_internal \
  MLD_NAMESPACE_KL(signature_internal) MLD_CONTEXT_PARAMETERS_9
#define mld_sign_signature MLD_NAMESPACE_KL(signature) MLD_CONTEXT_PARAMETERS_7
#define mld_sign_signature_extmu \
  MLD_NAMESPACE_KL(signature_extmu) MLD_CONTEXT_PARAMETERS_4
#define mld_sign MLD_NAMESPACE_KL(sign) MLD_CONTEXT_PARAMETERS_7
#define mld_sign_verify_internal \
  MLD_NAMESPACE_KL(verify_internal) MLD_CONTEXT_PARAMETERS_8
#define mld_sign_verify MLD_NAMESPACE_KL(verify) MLD_CONTEXT_PARAMETERS_7
#define mld_sign_verify_extmu \
  MLD_NAMESPACE_KL(verify_extmu) MLD_CONTEXT_PARAMETERS_4
#define mld_sign_open MLD_NAMESPACE_KL(open) MLD_CONTEXT_PARAMETERS_7
#define mld_sign_signature_pre_hash_internal \
  MLD_NAMESPACE_KL(signature_pre_hash_internal) MLD_CONTEXT_PARAMETERS_9
#define mld_sign_verify_pre_hash_internal \
  MLD_NAMESPACE_KL(verify_pre_hash_internal) MLD_CONTEXT_PARAMETERS_8
#define mld_sign_signature_pre_hash_shake256 \
  MLD_NAMESPACE_KL(signature_pre_hash_shake256) MLD_CONTEXT_PARAMETERS_8
#define mld_sign_verify_pre_hash_shake256 \
  MLD_NAMESPACE_KL(verify_pre_hash_shake256) MLD_CONTEXT_PARAMETERS_7
#define mld_prepare_domain_separation_prefix \
  MLD_NAMESPACE_KL(prepare_domain_separation_prefix)
#define mld_sign_pk_from_sk \
  MLD_NAMESPACE_KL(pk_from_sk) MLD_CONTEXT_PARAMETERS_2

/*************************************************
 * Hash algorithm constants for domain separation
 **************************************************/
#define MLD_PREHASH_NONE 0
#define MLD_PREHASH_SHA2_224 1
#define MLD_PREHASH_SHA2_256 2
#define MLD_PREHASH_SHA2_384 3
#define MLD_PREHASH_SHA2_512 4
#define MLD_PREHASH_SHA2_512_224 5
#define MLD_PREHASH_SHA2_512_256 6
#define MLD_PREHASH_SHA3_224 7
#define MLD_PREHASH_SHA3_256 8
#define MLD_PREHASH_SHA3_384 9
#define MLD_PREHASH_SHA3_512 10
#define MLD_PREHASH_SHAKE_128 11
#define MLD_PREHASH_SHAKE_256 12

/*************************************************
 * Name:        mld_sign_keypair_internal
 *
 * Description: Generates public and private key. Internal API.
 *              When MLD_CONFIG_KEYGEN_PCT is set, performs a Pairwise
 *              Consistency Test (PCT) as required by FIPS 140-3 IG.
 *
 * Arguments:   - uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]:   output public key
 *              - uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]:   output private key
 *              - const uint8_t seed[MLDSA_SEEDBYTES]: input random seed
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_RNG_FAIL: Random number generation failed.
 *              - MLD_ERR_FAIL: Other kinds of failure, incl. PCT failure
 *                  if MLD_CONFIG_KEYGEN_PCT is enabled.
 *
 * Specification: Implements @[FIPS204 Algorithm 6 (ML-DSA.KeyGen_internal)]
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_keypair_internal(uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                              uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                              const uint8_t seed[MLDSA_SEEDBYTES],
                              MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  requires(memory_no_alias(seed, MLDSA_SEEDBYTES))
  assigns(object_whole(pk))
  assigns(object_whole(sk))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL ||
          return_value == MLD_ERR_OUT_OF_MEMORY || return_value == MLD_ERR_RNG_FAIL)
);

/*************************************************
 * Name:        mld_sign_keypair
 *
 * Description: Generates public and private key.
 *              When MLD_CONFIG_KEYGEN_PCT is set, performs a Pairwise
 *              Consistency Test (PCT) as required by FIPS 140-3 IG.
 *
 * Arguments:   - uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]: output public key
 *              - uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]: output private key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_RNG_FAIL: Random number generation failed.
 *              - MLD_ERR_FAIL: Other kinds of failure, incl. PCT failure
 *                  if MLD_CONFIG_KEYGEN_PCT is enabled.
 *
 * Specification: Implements @[FIPS204 Algorithm 1 (ML-DSA.KeyGen)]
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_keypair(uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                     uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                     MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  assigns(object_whole(pk))
  assigns(object_whole(sk))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL ||
          return_value == MLD_ERR_OUT_OF_MEMORY || return_value == MLD_ERR_RNG_FAIL)
);

/*************************************************
 * Name:        mld_sign_signature_internal
 *
 * Description: Computes signature. Internal API.
 *
 * Arguments:   - uint8_t sig[MLDSA_CRYPTO_BYTES]: output signature
 *              - size_t *siglen:            pointer to output length of
 *                                           signature
 *              - const uint8_t *m:          pointer to message to be signed
 *              - size_t mlen:               length of message
 *              - const uint8_t *pre:        pointer to prefix string
 *              - size_t prelen:             length of prefix string
 *              - const uint8_t rnd[MLDSA_RNDBYTES]:
 *                                           random seed
 *              - const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]:
 *                                           bit-packed secret key
 *              - int externalmu:            indicates input message m is
 *                                           processed as mu
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Other kinds of failure
 *
 * If the returned value is non-zero, then the values of *sig and
 * *siglen should not be referenced.
 *
 * Reference: This code differs from the reference implementation
 *            in that it adds an explicit check for nonce exhaustion
 *            and can return MLD_ERR_FAIL in that case.
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature_internal(uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen,
                                const uint8_t *m, size_t mlen,
                                const uint8_t *pre, size_t prelen,
                                const uint8_t rnd[MLDSA_RNDBYTES],
                                const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                                int externalmu,
                                MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(mlen <= MLD_MAX_BUFFER_SIZE)
  requires(prelen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(siglen, sizeof(size_t)))
  requires(memory_no_alias(m, mlen))
  requires(memory_no_alias(rnd, MLDSA_RNDBYTES))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  requires((externalmu == 0 && (prelen == 0 || memory_no_alias(pre, prelen))) ||
           (externalmu == 1 && mlen == MLDSA_CRHBYTES))
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
  assigns(object_whole(siglen))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL ||
          return_value == MLD_ERR_OUT_OF_MEMORY)
  ensures(return_value == 0 ==> *siglen == MLDSA_CRYPTO_BYTES)
  ensures(return_value != 0 ==> *siglen == 0)
);

/*************************************************
 * Name:        mld_sign_signature
 *
 * Description: Computes signature. This function implements the randomized
 *              variant of ML-DSA. If you require the deterministic variant,
 *              use mld_sign_signature_internal directly.
 *
 * Arguments:   - uint8_t sig[MLDSA_CRYPTO_BYTES]: output signature
 *              - size_t *siglen:            pointer to output length of
 *                                           signature
 *              - const uint8_t *m:          pointer to message to be signed
 *              - size_t mlen:               length of message
 *              - uint8_t *ctx:              pointer to context string.
 *                                           May be NULL if ctxlen == 0.
 *              - size_t ctxlen:             length of context string.
 *                                           Should be <= 255.
 *              - const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]:
 *                                           bit-packed secret key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_RNG_FAIL: Random number generation failed.
 *              - MLD_ERR_FAIL: Other kinds of failure.
 *
 * Specification: Implements @[FIPS204 Algorithm 2 (ML-DSA.Sign)].
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature(uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *ctx,
                       size_t ctxlen,
                       const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                       MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(mlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(siglen, sizeof(size_t)))
  requires(memory_no_alias(m, mlen))
  requires(ctxlen <= MLD_MAX_BUFFER_SIZE)
  requires(ctxlen == 0 || memory_no_alias(ctx, ctxlen))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
  assigns(object_whole(siglen))
  ensures((return_value == 0 && *siglen == MLDSA_CRYPTO_BYTES) ||
          ((return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY || return_value == MLD_ERR_RNG_FAIL) && *siglen == 0))
);

/*************************************************
 * Name:        mld_sign_signature_extmu
 *
 * Description: Computes signature. This function implements the randomized
 *              variant of ML-DSA. If you require the deterministic variant,
 *              use mld_sign_signature_internal directly.
 *
 * Arguments:   - uint8_t sig[MLDSA_CRYPTO_BYTES]: output signature
 *              - size_t *siglen:            pointer to output length of
 *                                           signature
 *              - const uint8_t mu[MLDSA_CRHBYTES]:
 *                                           input mu to be signed
 *              - const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]:
 *                                           bit-packed secret key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_RNG_FAIL: Random number generation failed.
 *              - MLD_ERR_FAIL: Other kinds of failure.
 *
 * Specification: Implements @[FIPS204 Algorithm 2 (ML-DSA.Sign external mu
 *                variant)]
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature_extmu(uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen,
                             const uint8_t mu[MLDSA_CRHBYTES],
                             const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                             MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(siglen, sizeof(size_t)))
  requires(memory_no_alias(mu, MLDSA_CRHBYTES))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
  assigns(object_whole(siglen))
  ensures((return_value == 0 && *siglen == MLDSA_CRYPTO_BYTES) ||
          ((return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY || return_value == MLD_ERR_RNG_FAIL) && *siglen == 0))
);

/*************************************************
 * Name:        mld_sign
 *
 * Description: Compute signed message.
 *
 * Arguments:   - uint8_t *sm:        pointer to output signed message
 *                                    (allocated array with MLDSA_CRYPTO_BYTES +
 *mlen bytes), can be equal to m
 *              - size_t *smlen:      pointer to output length of signed message
 *              - const uint8_t *m:   pointer to message to be signed
 *              - size_t mlen:        length of message
 *              - const uint8_t *ctx: pointer to context string
 *              - size_t ctxlen:      length of context string
 *              - const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]:
 *                                    bit-packed secret key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Other kinds of failure
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign(uint8_t *sm, size_t *smlen, const uint8_t *m, size_t mlen,
             const uint8_t *ctx, size_t ctxlen,
             const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
             MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(mlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sm, MLDSA_CRYPTO_BYTES + mlen))
  requires(memory_no_alias(smlen, sizeof(size_t)))
  requires(m == sm || memory_no_alias(m, mlen))
  requires(ctxlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(ctx, ctxlen))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  assigns(memory_slice(sm, MLDSA_CRYPTO_BYTES + mlen))
  assigns(object_whole(smlen))
  ensures((return_value == 0 && *smlen == MLDSA_CRYPTO_BYTES + mlen) ||
          (return_value == MLD_ERR_FAIL
           || return_value == MLD_ERR_OUT_OF_MEMORY
           || return_value == MLD_ERR_RNG_FAIL))
);

/*************************************************
 * Name:        mld_sign_verify_internal
 *
 * Description: Verifies signature. Internal API.
 *
 * Arguments:   - const uint8_t *sig: pointer to input signature
 *              - size_t siglen:      length of signature
 *              - const uint8_t *m:   pointer to message
 *              - size_t mlen:        length of message
 *              - const uint8_t *pre: pointer to prefix string
 *              - size_t prelen:      length of prefix string
 *              - const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]:
 *                                    bit-packed public key
 *              - int externalmu:     indicates input message m is processed as
 *                                    mu
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Signature verification failed
 *
 * Specification: Implements @[FIPS204 Algorithm 8 (ML-DSA.Verify_internal)]
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify_internal(const uint8_t *sig, size_t siglen,
                             const uint8_t *m, size_t mlen, const uint8_t *pre,
                             size_t prelen,
                             const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                             int externalmu,
                             MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(prelen <= MLD_MAX_BUFFER_SIZE)
  requires(mlen <= MLD_MAX_BUFFER_SIZE)
  requires(siglen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, siglen))
  requires(memory_no_alias(m, mlen))
  requires(externalmu == 0 || (externalmu == 1 && mlen == MLDSA_CRHBYTES))
  requires(externalmu == 1 || prelen == 0 || memory_no_alias(pre, prelen))
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY)
);

/*************************************************
 * Name:        mld_sign_verify
 *
 * Description: Verifies signature.
 *
 * Arguments:   - const uint8_t *sig: pointer to input signature
 *              - size_t siglen:      length of signature
 *              - const uint8_t *m:   pointer to message
 *              - size_t mlen:        length of message
 *              - const uint8_t *ctx: pointer to context string.
 *                                    May be NULL if ctxlen == 0.
 *              - size_t ctxlen:      length of context string
 *              - const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]:
 *                                    bit-packed public key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Signature verification failed
 *
 * Specification: Implements @[FIPS204 Algorithm 3 (ML-DSA.Verify)]
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m,
                    size_t mlen, const uint8_t *ctx, size_t ctxlen,
                    const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(mlen <= MLD_MAX_BUFFER_SIZE)
  requires(siglen <= MLD_MAX_BUFFER_SIZE)
  requires(ctxlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, siglen))
  requires(memory_no_alias(m, mlen))
  requires(ctxlen == 0 || memory_no_alias(ctx, ctxlen))
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY)
);

/*************************************************
 * Name:        mld_sign_verify_extmu
 *
 * Description: Verifies signature.
 *
 * Arguments:   - const uint8_t *sig: pointer to input signature
 *              - size_t siglen:      length of signature
 *              - const uint8_t mu[MLDSA_CRHBYTES]:
 *                                    input mu
 *              - const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]:
 *                                    bit-packed public key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Signature verification failed
 *
 * Specification: Implements @[FIPS204 Algorithm 3 (ML-DSA.Verify external mu
 *                variant)]
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify_extmu(const uint8_t *sig, size_t siglen,
                          const uint8_t mu[MLDSA_CRHBYTES],
                          const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                          MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(siglen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, siglen))
  requires(memory_no_alias(mu, MLDSA_CRHBYTES))
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY)
);

/*************************************************
 * Name:        mld_sign_open
 *
 * Description: Verify signed message.
 *
 * Arguments:   - uint8_t *m:         pointer to output message (allocated array
 *                                    with smlen bytes), can be equal to sm
 *              - size_t *mlen:       pointer to output length of message
 *              - const uint8_t *sm:  pointer to signed message
 *              - size_t smlen:       length of signed message
 *              - const uint8_t *ctx: pointer to context tring
 *              - size_t ctxlen:      length of context string
 *              - const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]:
 *                                    bit-packed public key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Signature verification failed
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_open(uint8_t *m, size_t *mlen, const uint8_t *sm, size_t smlen,
                  const uint8_t *ctx, size_t ctxlen,
                  const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                  MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(smlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(m, smlen))
  requires(memory_no_alias(mlen, sizeof(size_t)))
  requires(m == sm || memory_no_alias(sm, smlen))
  requires(ctxlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(ctx, ctxlen))
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  assigns(memory_slice(m, smlen))
  assigns(memory_slice(mlen, sizeof(size_t)))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY)
);

/*************************************************
 * Name:        mld_sign_signature_pre_hash_internal
 *
 * Description: FIPS 204: Algorithm 4 HashML-DSA.Sign.
 *              Computes signature with pre-hashed message.
 *
 * Arguments:   - uint8_t sig[MLDSA_CRYPTO_BYTES]:
 *                                        output signature
 *              - size_t *siglen:         pointer to output length of signature
 *              - const uint8_t *ph:      pointer to pre-hashed message
 *              - size_t phlen:           length of pre-hashed message
 *              - const uint8_t *ctx:     pointer to context string
 *              - size_t ctxlen:          length of context string
 *              - const uint8_t rnd[MLDSA_RNDBYTES]:
 *                                        random seed
 *              - const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]:
 *                                        bit-packed secret key
 *              - int hashalg:            hash algorithm constant (one of
 *                                        MLD_PREHASH_*)
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Other kinds of failure
 *
 * Supported hash algorithm constants:
 *   MLD_PREHASH_SHA2_224, MLD_PREHASH_SHA2_256, MLD_PREHASH_SHA2_384,
 *   MLD_PREHASH_SHA2_512, MLD_PREHASH_SHA2_512_224, MLD_PREHASH_SHA2_512_256,
 *   MLD_PREHASH_SHA3_224, MLD_PREHASH_SHA3_256, MLD_PREHASH_SHA3_384,
 *   MLD_PREHASH_SHA3_512, MLD_PREHASH_SHAKE_128, MLD_PREHASH_SHAKE_256
 *
 * Warning: This is an unstable API that may change in the future. If you need
 * a stable API use mld_sign_signature_pre_hash_shake256.
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature_pre_hash_internal(
    uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen, const uint8_t *ph,
    size_t phlen, const uint8_t *ctx, size_t ctxlen,
    const uint8_t rnd[MLDSA_RNDBYTES],
    const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES], int hashalg,
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(ctxlen <= MLD_MAX_BUFFER_SIZE)
  requires(phlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(siglen, sizeof(size_t)))
  requires(memory_no_alias(ph, phlen))
  requires(ctxlen == 0 || memory_no_alias(ctx, ctxlen))
  requires(memory_no_alias(rnd, MLDSA_RNDBYTES))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
  assigns(object_whole(siglen))
  ensures((return_value == 0 && *siglen == MLDSA_CRYPTO_BYTES) ||
          ((return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY) && *siglen == 0))
);

/*************************************************
 * Name:        mld_sign_verify_pre_hash_internal
 *
 * Description: FIPS 204: Algorithm 5 HashML-DSA.Verify.
 *              Verifies signature with pre-hashed message.
 *
 * Arguments:   - const uint8_t *sig:     pointer to input signature
 *              - size_t siglen:          length of signature
 *              - const uint8_t *ph:      pointer to pre-hashed message
 *              - size_t phlen:           length of pre-hashed message
 *              - const uint8_t *ctx:     pointer to context string
 *              - size_t ctxlen:          length of context string
 *              - const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]:
 *                                        bit-packed public key
 *              - int hashalg:            hash algorithm constant (one of
 *                                        MLD_PREHASH_*)
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Signature verification failed
 *
 * Supported hash algorithm constants:
 *   MLD_PREHASH_SHA2_224, MLD_PREHASH_SHA2_256, MLD_PREHASH_SHA2_384,
 *   MLD_PREHASH_SHA2_512, MLD_PREHASH_SHA2_512_224, MLD_PREHASH_SHA2_512_256,
 *   MLD_PREHASH_SHA3_224, MLD_PREHASH_SHA3_256, MLD_PREHASH_SHA3_384,
 *   MLD_PREHASH_SHA3_512, MLD_PREHASH_SHAKE_128, MLD_PREHASH_SHAKE_256
 *
 * Warning: This is an unstable API that may change in the future. If you need
 * a stable API use mld_sign_verify_pre_hash_shake256.
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify_pre_hash_internal(
    const uint8_t *sig, size_t siglen, const uint8_t *ph, size_t phlen,
    const uint8_t *ctx, size_t ctxlen,
    const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES], int hashalg,
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(phlen <= MLD_MAX_BUFFER_SIZE)
  requires(ctxlen <= MLD_MAX_BUFFER_SIZE - 77)
  requires(siglen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, siglen))
  requires(memory_no_alias(ph, phlen))
  requires(ctxlen == 0 || memory_no_alias(ctx, ctxlen))
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY)
);

/*************************************************
 * Name:        mld_sign_signature_pre_hash_shake256
 *
 * Description: FIPS 204: Algorithm 4 HashML-DSA.Sign with SHAKE256.
 *              Computes signature with pre-hashed message using SHAKE256.
 *              This function computes the SHAKE256 hash of the message
 *internally.
 *
 * Arguments:   - uint8_t sig[MLDSA_CRYPTO_BYTES]:
 *                                    output signature
 *              - size_t *siglen:     pointer to output length of signature
 *              - const uint8_t *m:   pointer to message to be hashed and signed
 *              - size_t mlen:        length of message
 *              - const uint8_t *ctx: pointer to context string
 *              - size_t ctxlen:      length of context string
 *              - const uint8_t rnd[MLDSA_RNDBYTES]:
 *                                    random seed
 *              - const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]:
 *                                    bit-packed secret key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Other kinds of failure
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_signature_pre_hash_shake256(
    uint8_t sig[MLDSA_CRYPTO_BYTES], size_t *siglen, const uint8_t *m,
    size_t mlen, const uint8_t *ctx, size_t ctxlen,
    const uint8_t rnd[MLDSA_RNDBYTES],
    const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(mlen <= MLD_MAX_BUFFER_SIZE)
  requires(ctxlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(siglen, sizeof(size_t)))
  requires(memory_no_alias(m, mlen))
  requires(ctxlen == 0 || memory_no_alias(ctx, ctxlen))
  requires(memory_no_alias(rnd, MLDSA_RNDBYTES))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
  assigns(object_whole(siglen))
  ensures((return_value == 0 && *siglen == MLDSA_CRYPTO_BYTES) ||
          ((return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY) && *siglen == 0))
);

/*************************************************
 * Name:        mld_sign_verify_pre_hash_shake256
 *
 * Description: FIPS 204: Algorithm 5 HashML-DSA.Verify with SHAKE256.
 *              Verifies signature with pre-hashed message using SHAKE256.
 *              This function computes the SHAKE256 hash of the message
 *              internally.
 *
 * Arguments:   - const uint8_t *sig: pointer to input signature
 *              - size_t siglen:      length of signature
 *              - const uint8_t *m:   pointer to message to be hashed and
 *                                    verified
 *              - size_t mlen:        length of message
 *              - const uint8_t *ctx: pointer to context string
 *              - size_t ctxlen:      length of context string
 *              - const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]:
 *                                    bit-packed public key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Signature verification failed
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_verify_pre_hash_shake256(
    const uint8_t *sig, size_t siglen, const uint8_t *m, size_t mlen,
    const uint8_t *ctx, size_t ctxlen,
    const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
    MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(mlen <= MLD_MAX_BUFFER_SIZE)
  requires(ctxlen <= MLD_MAX_BUFFER_SIZE - 77)
  requires(siglen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(sig, siglen))
  requires(memory_no_alias(m, mlen))
  requires(ctxlen == 0 || memory_no_alias(ctx, ctxlen))
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY)
);

/* Maximum formatted domain separation message length:
 * - Pure ML-DSA: 0x00 || ctxlen || ctx (max 255)
 * - HashML-DSA: 0x01 || ctxlen || ctx (max 255) || oid (11) || ph (max 64) */
#define MLD_DOMAIN_SEPARATION_MAX_BYTES (2 + 255 + 11 + 64)

/*************************************************
 * Name:        mld_prepare_domain_separation_prefix
 *
 * Description: Prepares domain separation prefix for ML-DSA signing.
 *              For pure ML-DSA (hashalg == MLD_PREHASH_NONE):
 *                Format: 0x00 || ctxlen (1 byte) || ctx
 *              For HashML-DSA (hashalg != MLD_PREHASH_NONE):
 *                Format: 0x01 || ctxlen (1 byte) || ctx || oid (11 bytes) || ph
 *
 * Arguments:   - uint8_t prefix[MLD_DOMAIN_SEPARATION_MAX_BYTES]:
 *                output domain separation prefix buffer
 *              - const uint8_t *ph: pointer to pre-hashed message
 *                (ignored for pure ML-DSA)
 *              - size_t phlen: length of pre-hashed message
 *                (ignored for pure ML-DSA)
 *              - const uint8_t *ctx: pointer to context string (may be NULL)
 *              - size_t ctxlen: length of context string
 *              - int hashalg: hash algorithm constant
 *                (MLD_PREHASH_NONE for pure ML-DSA, or MLD_PREHASH_* for
 *                 HashML-DSA)
 *
 * Returns the total length of the formatted prefix, or 0 on error.
 *
 * This function is useful for building incremental signing APIs.
 *
 * Specification:
 * - For HashML-DSA (hashalg != MLD_PREHASH_NONE), implements
 *   @[FIPS204, Algorithm 4, L23]
 * - For Pure ML-DSA (hashalg == MLD_PREHASH_NONE), implements
 *    ```
 *       M' <- BytesToBits(IntegerToBytes(0, 1)
 *              || IntegerToBytes(|ctx|, 1)
 *              || ctx
 *    ```
 *    which is part of @[FIPS204, Algorithm 2 (ML-DSA.Sign), L10] and
 *    @[FIPS204, Algorithm 3 (ML-DSA.Verify), L5].
 *
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
size_t mld_prepare_domain_separation_prefix(
    uint8_t prefix[MLD_DOMAIN_SEPARATION_MAX_BYTES], const uint8_t *ph,
    size_t phlen, const uint8_t *ctx, size_t ctxlen, int hashalg)
__contract__(
  requires(ctxlen <= 255)
  requires(phlen <= MLD_MAX_BUFFER_SIZE)
  requires(ctxlen == 0 || memory_no_alias(ctx, ctxlen))
  requires(hashalg == MLD_PREHASH_NONE || memory_no_alias(ph, phlen))
  requires(memory_no_alias(prefix, MLD_DOMAIN_SEPARATION_MAX_BYTES))
  assigns(memory_slice(prefix, MLD_DOMAIN_SEPARATION_MAX_BYTES))
  ensures(return_value <= MLD_DOMAIN_SEPARATION_MAX_BYTES)
);

/*************************************************
 * Name:        mld_sign_pk_from_sk
 *
 * Description: Performs basic validity checks on secret key, and derives
 *              public key.
 *
 *              Referring to the decoding of the secret key
 *              `sk=(rho, K, tr, s1, s2, t0)`
 *              (cf. [@FIPS204, Algorithm 25 skDecode]),
 *              the following checks are performed:
 *                - Check that s1 and s2 have coefficients in
 *                  [-MLDSA_ETA, MLDSA_ETA]
 *                - Check that t0 and tr stored in sk match recomputed values.
 *
 * Arguments:   - uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES]: output public key
 *              - const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES]: input secret
 *                key
 *
 * Returns:     - 0: Success
 *              - MLD_ERR_OUT_OF_MEMORY: If MLD_CONFIG_CUSTOM_ALLOC_FREE is
 *                  used and an allocation via MLD_CUSTOM_ALLOC returned NULL.
 *              - MLD_ERR_FAIL: Secret key validation failed
 *
 * Note: This function leaks whether the secret key is valid or invalid
 *       through its return value and timing.
 **************************************************/
MLD_MUST_CHECK_RETURN_VALUE
MLD_EXTERNAL_API
int mld_sign_pk_from_sk(uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                        const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                        MLD_CONFIG_CONTEXT_PARAMETER_TYPE context)
__contract__(
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  assigns(memory_slice(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  ensures(return_value == 0 || return_value == MLD_ERR_FAIL || return_value == MLD_ERR_OUT_OF_MEMORY)
);
#endif /* !MLD_SIGN_H */
