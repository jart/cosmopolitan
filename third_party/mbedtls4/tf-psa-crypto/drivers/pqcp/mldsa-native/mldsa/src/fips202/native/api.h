/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#ifndef MLD_FIPS202_NATIVE_API_H
#define MLD_FIPS202_NATIVE_API_H
/*
 * FIPS-202 native interface
 *
 * This header is primarily for documentation purposes.
 * It should not be included by backend implementations.
 */

#include <stdint.h>
#include "../../cbmc.h"

/* Backends must return MLD_NATIVE_FUNC_SUCCESS upon success. */
#define MLD_NATIVE_FUNC_SUCCESS (0)
/* Backends may return MLD_NATIVE_FUNC_FALLBACK to signal to the frontend that
 * the target/parameters are unsupported; typically, this would be because of
 * dependencies on CPU features not detected on the host CPU. In this case,
 * the frontend falls back to the default C implementation.
 *
 * IMPORTANT: Backend implementations must ensure that the decision of whether
 * to fallback (return MLD_NATIVE_FUNC_FALLBACK) or not must never depend on
 * the input data itself. Fallback decisions may only depend on system
 * capabilities (e.g., CPU features) and, where present, length information.
 * This requirement applies to all backend functions to maintain constant-time
 * properties.
 */
#define MLD_NATIVE_FUNC_FALLBACK (-1)

/*
 * This is the C<->native interface allowing for the drop-in
 * of custom Keccak-F1600 implementations.
 *
 * A _backend_ is a specific implementation of parts of this interface.
 *
 * You can replace 1-fold or 4-fold batched Keccak-F1600.
 * To enable, set MLD_USE_FIPS202_X1_NATIVE or MLD_USE_FIPS202_X4_NATIVE
 * in your backend, and define the inline wrappers mld_keccak_f1600_x1_native()
 * and/or mld_keccak_f1600_x4_native(), respectively, to forward to your
 * implementation.
 */

#if defined(MLD_USE_FIPS202_X1_NATIVE)
static MLD_INLINE int mld_keccak_f1600_x1_native(uint64_t *state)
__contract__(
    requires(memory_no_alias(state, sizeof(uint64_t) * 25 * 1))
    assigns(memory_slice(state, sizeof(uint64_t) * 25 * 1))
    ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
    ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged_u64(state, 25 * 1))
);
#endif /* MLD_USE_FIPS202_X1_NATIVE */
#if defined(MLD_USE_FIPS202_X4_NATIVE)
static MLD_INLINE int mld_keccak_f1600_x4_native(uint64_t *state)
__contract__(
    requires(memory_no_alias(state, sizeof(uint64_t) * 25 * 4))
    assigns(memory_slice(state, sizeof(uint64_t) * 25 * 4))
    ensures(return_value == MLD_NATIVE_FUNC_FALLBACK || return_value == MLD_NATIVE_FUNC_SUCCESS)
    ensures((return_value == MLD_NATIVE_FUNC_FALLBACK) ==> array_unchanged_u64(state, 25 * 4))
);
#endif /* MLD_USE_FIPS202_X4_NATIVE */

#endif /* !MLD_FIPS202_NATIVE_API_H */
