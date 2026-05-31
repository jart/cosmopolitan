// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "fips202/fips202.h"

void harness(void)
{
  uint8_t *out;
  size_t outlen;
  mld_shake256ctx *state;

  mld_shake256_squeeze(out, outlen, state);
}
