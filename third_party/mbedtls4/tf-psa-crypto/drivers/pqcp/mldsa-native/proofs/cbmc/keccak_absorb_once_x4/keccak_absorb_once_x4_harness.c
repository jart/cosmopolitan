// Copyright (c) The mldsa-native project authors
// Copyright (c) The mlkem-native project authors
// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <keccakf1600.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


void mld_keccak_absorb_once_x4(uint64_t *s, uint32_t r, const uint8_t *in0,
                               const uint8_t *in1, const uint8_t *in2,
                               const uint8_t *in3, size_t inlen, uint8_t p);

void harness(void)
{
  uint64_t *s;
  uint32_t r;
  const uint8_t *in0, *in1, *in2, *in3;
  size_t inlen;
  uint8_t p;
  mld_keccak_absorb_once_x4(s, r, in0, in1, in2, in3, inlen, p);
}
