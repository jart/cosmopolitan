// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void harness(void)
{
  uint8_t *prefix;
  const uint8_t *ph;
  size_t phlen;
  const uint8_t *ctx;
  size_t ctxlen;
  int hashalg;

  mld_prepare_domain_separation_prefix(prefix, ph, phlen, ctx, ctxlen, hashalg);
}
