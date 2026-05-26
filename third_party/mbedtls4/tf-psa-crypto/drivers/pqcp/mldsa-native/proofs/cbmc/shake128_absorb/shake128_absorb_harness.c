// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "fips202/fips202.h"

void harness(void)
{
  mld_shake128ctx *s;
  const uint8_t *in;
  size_t inlen;

  mld_shake128_absorb(s, in, inlen);
}
