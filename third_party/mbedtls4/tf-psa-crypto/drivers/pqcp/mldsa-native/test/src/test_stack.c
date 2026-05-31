/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mldsa_native.h"

static void test_keygen_only(void)
{
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];

  /* Only call keypair - this is what we're measuring */
  /* Uses the notrandombytes implementation for deterministic randomness */
  int ret = crypto_sign_keypair(pk, sk);
  (void)ret; /* Ignore return value - we only care about stack measurement */
}

static void test_sign_only(void)
{
  unsigned char sk[CRYPTO_SECRETKEYBYTES] = {0};
  unsigned char sig[CRYPTO_BYTES];
  size_t siglen;
  const unsigned char msg[] = "test message for stack measurement";
  const unsigned char ctx[] = "test context";

  /* Only call signature - this is what we're measuring */
  /* sk is zero-initialized (invalid key, but OK for stack measurement) */
  int ret = crypto_sign_signature(sig, &siglen, msg, sizeof(msg) - 1, ctx,
                                  sizeof(ctx) - 1, sk);
  (void)ret; /* Ignore return value - we only care about stack measurement */
}

static void test_verify_only(void)
{
  unsigned char pk[CRYPTO_PUBLICKEYBYTES] = {0};
  unsigned char sig[CRYPTO_BYTES] = {0};
  const unsigned char msg[] = "test message for stack measurement";
  const unsigned char ctx[] = "test context";

  /* Only call verify - this is what we're measuring */
  /* pk and sig are zero-initialized (invalid, but OK for stack measurement) */
  int ret = crypto_sign_verify(sig, CRYPTO_BYTES, msg, sizeof(msg) - 1, ctx,
                               sizeof(ctx) - 1, pk);
  (void)ret; /* Ignore return value - we only care about stack measurement */
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <keygen|sign|verify>\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "keygen") == 0)
  {
    test_keygen_only();
  }
  else if (strcmp(argv[1], "sign") == 0)
  {
    test_sign_only();
  }
  else if (strcmp(argv[1], "verify") == 0)
  {
    test_verify_only();
  }
  else
  {
    fprintf(stderr, "Unknown test: %s\n", argv[1]);
    return 1;
  }

  return 0;
}
