// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "fips202/fips202.h"

extern void keccak_finalize(uint64_t s[MLD_KECCAK_LANES], unsigned int pos,
                            unsigned int r, uint8_t p);

void harness(void)
{
  uint64_t *s;
  unsigned int pos, r;
  uint8_t p;

  keccak_finalize(s, pos, r, p);
}
