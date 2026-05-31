// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "fips202/fips202.h"

static unsigned int keccak_squeeze(uint8_t *out, size_t outlen,
                                   uint64_t s[MLD_KECCAK_LANES],
                                   unsigned int pos, unsigned int r);

void harness(void)
{
  uint8_t *out;
  size_t outlen;
  uint64_t *s;
  unsigned int pos, r;

  keccak_squeeze(out, outlen, s, pos, r);
}
