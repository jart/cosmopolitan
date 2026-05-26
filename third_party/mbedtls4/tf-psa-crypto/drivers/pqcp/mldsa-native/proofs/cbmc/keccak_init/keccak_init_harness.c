// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "fips202/fips202.h"

extern void keccak_init(uint64_t s[MLD_KECCAK_LANES]);

void harness(void)
{
  uint64_t *s;

  keccak_init(s);
}
