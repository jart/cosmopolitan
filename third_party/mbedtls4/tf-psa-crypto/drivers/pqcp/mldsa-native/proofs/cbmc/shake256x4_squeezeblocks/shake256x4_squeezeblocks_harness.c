// Copyright (c) The mldsa-native project authors
// Copyright (c) The mlkem-native project authors
// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <fips202x4.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void harness(void)
{
  uint8_t *output0, output1, output2, output3;
  size_t nblocks;
  mld_shake256x4ctx *state;
  mld_shake256x4_squeezeblocks(output0, output1, output2, output3, nblocks,
                               state);
}
