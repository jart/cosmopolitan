// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void mld_H(uint8_t *out, size_t outlen, const uint8_t *in1, size_t in1len,
           const uint8_t *in2, size_t in2len, const uint8_t *in3,
           size_t in3len);

void harness(void)
{
  uint8_t *out;
  size_t outlen;
  const uint8_t *in1;
  size_t in1len;
  const uint8_t *in2;
  size_t in2len;
  const uint8_t *in3;
  size_t in3len;

  mld_H(out, outlen, in1, in1len, in2, in2len, in3, in3len);
}
