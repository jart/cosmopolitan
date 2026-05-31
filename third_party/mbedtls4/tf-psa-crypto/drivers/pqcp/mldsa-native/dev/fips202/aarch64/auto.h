/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/* References
 * ==========
 *
 * - [HYBRID]
 *   Hybrid scalar/vector implementations of Keccak and SPHINCS+ on AArch64
 *   Becker, Kannwischer
 *   https://eprint.iacr.org/2022/1243
 */

#ifndef MLD_FIPS202_NATIVE_AARCH64_AUTO_H
#define MLD_FIPS202_NATIVE_AARCH64_AUTO_H
/* Default FIPS202 assembly profile for AArch64 systems */

/*
 * Default logic to decide which implementation to use.
 *
 */

/*
 * Keccak-f1600
 *
 * - On Arm-based Apple CPUs, we pick a pure Neon implementation.
 * - Otherwise, unless MLD_SYS_AARCH64_SLOW_BARREL_SHIFTER is set,
 *   we use lazy-rotation scalar assembly from @[HYBRID].
 * - Otherwise, if MLD_SYS_AARCH64_SLOW_BARREL_SHIFTER is set, we
 *   fall back to the standard C implementation.
 */
#if defined(__ARM_FEATURE_SHA3) && defined(__APPLE__)
#include "x1_v84a.h"
#elif !defined(MLD_SYS_AARCH64_SLOW_BARREL_SHIFTER)
#include "x1_scalar.h"
#endif

/*
 * Keccak-f1600x2/x4
 *
 * The optimal implementation is highly CPU-specific; see @[HYBRID].
 *
 * For now, if v8.4-A is not implemented, we fall back to Keccak-f1600.
 * If v8.4-A is implemented and we are on an Apple CPU, we use a plain
 * Neon-based implementation.
 * If v8.4-A is implemented and we are not on an Apple CPU, we use a
 * scalar/Neon/Neon hybrid.
 * The reason for this distinction is that Apple CPUs appear to implement
 * the SHA3 instructions on all SIMD units, while Arm CPUs prior to Cortex-X4
 * don't, and ordinary Neon instructions are still needed.
 */
#if defined(__ARM_FEATURE_SHA3)
/*
 * For Apple-M cores, we use a plain implementation leveraging SHA3
 * instructions only.
 */
#if defined(__APPLE__)
#include "x2_v84a.h"
#else
#include "x4_v8a_v84a_scalar.h"
#endif

#else /* __ARM_FEATURE_SHA3 */

#include "x4_v8a_scalar.h"

#endif /* !__ARM_FEATURE_SHA3 */

#endif /* !MLD_FIPS202_NATIVE_AARCH64_AUTO_H */
