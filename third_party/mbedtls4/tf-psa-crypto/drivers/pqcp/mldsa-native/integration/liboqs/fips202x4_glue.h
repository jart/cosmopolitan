/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_INTEGRATION_LIBOQS_FIPS202X4_GLUE_H
#define MLD_INTEGRATION_LIBOQS_FIPS202X4_GLUE_H

/* Include OQS's own FIPS202_X4 header */
#include "fips202x4.h"

/* OQS's FIPS202_X4 is as-is compatible with the one expected
 * by mldsa-native, so just remove the mld_xxx prefix. */
#define mld_shake128x4ctx shake128x4incctx
#define mld_shake128x4_absorb_once shake128x4_absorb_once
#define mld_shake128x4_squeezeblocks shake128x4_squeezeblocks
#define mld_shake128x4_init shake128x4_inc_init
#define mld_shake128x4_release shake128x4_inc_ctx_release

#define mld_shake256x4ctx shake256x4incctx
#define mld_shake256x4_absorb_once shake256x4_absorb_once
#define mld_shake256x4_squeezeblocks shake256x4_squeezeblocks
#define mld_shake256x4_init shake256x4_inc_init
#define mld_shake256x4_release shake256x4_inc_ctx_release

#endif /* !MLD_INTEGRATION_LIBOQS_FIPS202X4_GLUE_H */
