// Copyright (c) The mldsa-native project authors
// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include "sign.h"

int mld_check_pct(uint8_t const pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                  uint8_t const sk[MLDSA_CRYPTO_SECRETKEYBYTES]);

void harness(void)
{
  uint8_t *a, *b;
  mld_check_pct(a, b);
}
