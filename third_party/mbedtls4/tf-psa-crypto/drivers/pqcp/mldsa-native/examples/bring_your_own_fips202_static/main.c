/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Import public mldsa-native API
 *
 * This requires specifying the parameter set and namespace prefix
 * used for the build.
 */
#include <mldsa_native.h>
#include "expected_signatures.h"
#include "test_only_rng/notrandombytes.h"

/*
 * This example demonstrates a static global state FIPS-202 implementation
 * that works correctly with ML-DSA when FIPS-202 operations are used serially.
 *
 * This implementation uses a single global state for SHAKE128 and SHAKE256,
 * requiring that no interleaved FIPS-202 operations occur.
 */

#define CHECK(x)                                              \
  do                                                          \
  {                                                           \
    int rc;                                                   \
    rc = (x);                                                 \
    if (!rc)                                                  \
    {                                                         \
      fprintf(stderr, "ERROR (%s,%d)\n", __FILE__, __LINE__); \
      return 1;                                               \
    }                                                         \
  } while (0)

int main(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  size_t siglen;

  const char *test_msg =
      "This is a test message for ML-DSA digital signature algorithm!";
  const char *test_ctx = "test_context_123";

  /* WARNING: Test-only
   * Normally, you would want to seed a PRNG with trustworthy entropy here. */
  randombytes_reset();

  printf("Generating keypair ... ");

  CHECK(crypto_sign_keypair(pk, sk) == 0);

  printf("DONE\n");
  printf("Signing... ");

  CHECK(crypto_sign_signature(sig, &siglen, (const uint8_t *)test_msg,
                              strlen(test_msg), (const uint8_t *)test_ctx,
                              strlen(test_ctx), sk) == 0);

  printf("DONE\n");
  printf("Verifying... ");

  CHECK(crypto_sign_verify(sig, siglen, (const uint8_t *)test_msg,
                           strlen(test_msg), (const uint8_t *)test_ctx,
                           strlen(test_ctx), pk) == 0);
  printf("DONE\n");

#if !defined(MLD_CONFIG_KEYGEN_PCT)
  /* Check against expected signature to make sure that
   * we integrated the library correctly */
  printf("Checking deterministic signature... ");
  CHECK(siglen == sizeof(expected_signature));
  CHECK(memcmp(sig, expected_signature, siglen) == 0);
  printf("DONE\n");
#else  /* !MLD_CONFIG_KEYGEN_PCT */
  printf(
      "[WARNING] Skipping KAT test since PCT is enabled and modifies PRNG\n");
#endif /* MLD_CONFIG_KEYGEN_PCT */

  printf("OK\n");

  return 0;
}
