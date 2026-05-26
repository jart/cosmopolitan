/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/* References
 * ==========
 *
 * - [FIPS140_3_IG]
 *   Implementation Guidance for FIPS 140-3 and the Cryptographic Module
 *   Validation Program
 *   National Institute of Standards and Technology
 *   https://csrc.nist.gov/projects/cryptographic-module-validation-program/fips-140-3-ig-announcements
 */

#ifndef MLD_INTEGRATION_LIBOQS_CONFIG_C_H
#define MLD_INTEGRATION_LIBOQS_CONFIG_C_H

/* Enable valgrind-based assertions in mldsa-native through macro
 * from libOQS. */
#if !defined(__ASSEMBLER__)
#include <oqs/common.h>
#if defined(OQS_ENABLE_TEST_CONSTANT_TIME)
#define MLD_CONFIG_CT_TESTING_ENABLED
#endif
#endif /* !__ASSEMBLER__ */

/* Use OQS's FIPS202 via glue headers */
#define MLD_CONFIG_FIPS202_CUSTOM_HEADER \
  "../../integration/liboqs/fips202_glue.h"
#define MLD_CONFIG_FIPS202X4_CUSTOM_HEADER \
  "../../integration/liboqs/fips202x4_glue.h"

/******************************************************************************
 * Name:        MLD_CONFIG_PARAMETER_SET
 *
 * Description: Specifies the parameter set for ML-DSA
 *              - MLD_CONFIG_PARAMETER_SET=44 corresponds to ML-DSA-44
 *              - MLD_CONFIG_PARAMETER_SET=65 corresponds to ML-DSA-65
 *              - MLD_CONFIG_PARAMETER_SET=87 corresponds to ML-DSA-87
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
#ifndef MLD_CONFIG_PARAMETER_SET
#define MLD_CONFIG_PARAMETER_SET \
  44 /* Change this for different security strengths */
#endif

/******************************************************************************
 * Name:        MLD_CONFIG_NAMESPACE_PREFIX
 *
 * Description: The prefix to use to namespace global symbols from mldsa/.
 *              For integration builds, this adds an architecture-specific
 *              suffix to distinguish different builds.
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
#if !defined(MLD_CONFIG_NAMESPACE_PREFIX)
#define MLD_CONFIG_NAMESPACE_PREFIX MLD_DEFAULT_NAMESPACE_PREFIX
#endif

/******************************************************************************
 * Name:        MLD_CONFIG_FILE
 *
 * Description: If defined, this is a header that will be included instead
 *              of this default configuration file mldsa/config.h.
 *
 *              When you need to build mldsa-native in multiple configurations,
 *              using varying MLD_CONFIG_FILE can be more convenient
 *              then configuring everything through CFLAGS.
 *
 *              To use, MLD_CONFIG_FILE _must_ be defined prior
 *              to the inclusion of any mldsa-native headers. For example,
 *              it can be set by passing `-DMLD_CONFIG_FILE="..."`
 *              on the command line.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_FILE "config.h" */

/******************************************************************************
 * Name:        MLD_CONFIG_ARITH_BACKEND_FILE
 *
 * Description: The arithmetic backend to use.
 *
 *              If MLD_CONFIG_USE_NATIVE_BACKEND_ARITH is unset, this option
 *              is ignored.
 *
 *              If MLD_CONFIG_USE_NATIVE_BACKEND_ARITH is set, this option must
 *              either be undefined or the filename of an arithmetic backend.
 *              If unset, the default backend will be used.
 *
 *              This can be set using CFLAGS.
 *
 *****************************************************************************/
/* #if defined(MLD_CONFIG_USE_NATIVE_BACKEND_ARITH) && \
       !defined(MLD_CONFIG_ARITH_BACKEND_FILE)
   #define MLD_CONFIG_ARITH_BACKEND_FILE "native/meta.h"
   #endif
*/

/******************************************************************************
 * Name:        MLD_CONFIG_FIPS202_BACKEND_FILE
 *
 * Description: The FIPS-202 backend to use.
 *
 *              If MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202 is set, this option
 *              must either be undefined or the filename of a FIPS202 backend.
 *              If unset, the default backend will be used.
 *
 *              This can be set using CFLAGS.
 *
 *****************************************************************************/
/* #if defined(MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202) && \
       !defined(MLD_CONFIG_FIPS202_BACKEND_FILE)
   #define MLD_CONFIG_FIPS202_BACKEND_FILE "fips202/native/auto.h"
   #endif
*/

/******************************************************************************
 * Name:        MLD_CONFIG_CUSTOM_ZEROIZE
 *
 * Description: In compliance with FIPS 204 Section 3.6.3, mldsa-native zeroizes
 *              intermediate stack buffers before returning from function calls.
 *
 *              Set this option and define `mld_zeroize` if you want to
 *              use a custom method to zeroize intermediate stack buffers.
 *              The default implementation uses SecureZeroMemory on Windows
 *              and a memset + compiler barrier otherwise. If neither of those
 *              is available on the target platform, compilation will fail,
 *              and you will need to use MLD_CONFIG_CUSTOM_ZEROIZE to provide
 *              a custom implementation of `mld_zeroize()`.
 *
 *              WARNING:
 *              The explicit stack zeroization conducted by mldsa-native
 *              reduces the likelihood of data leaking on the stack, but
 *              does not eliminate it! The C standard makes no guarantee about
 *              where a compiler allocates structures and whether/where it makes
 *              copies of them. Also, in addition to entire structures, there
 *              may also be potentially exploitable leakage of individual values
 *              on the stack.
 *
 *              If you need bullet-proof zeroization of the stack, you need to
 *              consider additional measures instead of what this feature
 *              provides. In this case, you can set mld_zeroize to a
 *              no-op.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CUSTOM_ZEROIZE
   #if !defined(__ASSEMBLER__)
   #include <stdint.h>
   #include "sys.h"
   static MLD_INLINE void mld_zeroize(void *ptr, size_t len)
   {
       ... your implementation ...
   }
   #endif
*/

/******************************************************************************
 * Name:        MLD_CONFIG_CUSTOM_RANDOMBYTES
 *
 * Description: mldsa-native does not provide a secure randombytes
 *              implementation. Such an implementation has to provided by the
 *              consumer.
 *
 *              If this option is not set, mlkem-native expects a function
 *              int randombytes(uint8_t *out, size_t outlen).
 *
 *              Set this option and define `mlk_randombytes` if you want to
 *              use a custom method to sample randombytes with a different name
 *              or signature.
 *
 *****************************************************************************/
#define MLD_CONFIG_CUSTOM_RANDOMBYTES
#if !defined(__ASSEMBLER__)
#include <oqs/rand.h>
#include <stdint.h>
#include "../../mldsa/src/sys.h"
static MLD_INLINE int mld_randombytes(uint8_t *ptr, size_t len)
{
  OQS_randombytes(ptr, len);
  return 0;
}
#endif /* !__ASSEMBLER__ */

/******************************************************************************
 * Name:        MLD_CONFIG_KEYGEN_PCT
 *
 * Description: Compliance with @[FIPS140_3_IG, p.87] requires a
 *              Pairwise Consistency Test (PCT) to be carried out on a freshly
 *              generated keypair before it can be exported.
 *
 *              Set this option if such a check should be implemented.
 *              In this case, crypto_sign_keypair_internal and
 *              crypto_sign_keypair will return a non-zero error code if the
 *              PCT failed.
 *
 *              NOTE: This feature will drastically lower the performance of
 *              key generation.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_KEYGEN_PCT */

/******************************************************************************
 * Name:        MLD_CONFIG_KEYGEN_PCT_BREAKAGE_TEST
 *
 * Description: If this option is set, the user must provide a runtime
 *              function `static inline int mld_break_pct() { ... }` to
 *              indicate whether the PCT should be made fail.
 *
 *              This option only has an effect if MLD_CONFIG_KEYGEN_PCT is set.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_KEYGEN_PCT_BREAKAGE_TEST
   #if !defined(__ASSEMBLER__)
   #include "sys.h"
   static MLD_INLINE int mld_break_pct(void)
   {
       ... return 0/1 depending on whether PCT should be broken ...
   }
   #endif
*/

/******************************************************************************
 * Name:        MLD_CONFIG_NO_ASM
 *
 * Description: If this option is set, mldsa-native will be built without
 *              use of native code or inline assembly.
 *
 *              By default, inline assembly is used to implement value barriers.
 *              Without inline assembly, mldsa-native will use a global volatile
 *              'opt blocker' instead; see ct.h.
 *
 *              Inline assembly is also used to implement a secure zeroization
 *              function on non-Windows platforms. If this option is set and
 *              the target platform is not Windows, you MUST set
 *              MLD_CONFIG_CUSTOM_ZEROIZE and provide a custom zeroization
 *              function.
 *
 *              If this option is set, MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202 and
 *              and MLD_CONFIG_USE_NATIVE_BACKEND_ARITH will be ignored, and no
 *              native backends will be used.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_NO_ASM */

/******************************************************************************
 * Name:        MLD_CONFIG_NO_ASM_VALUE_BARRIER
 *
 * Description: If this option is set, mldsa-native will be built without
 *              use of native code or inline assembly for value barriers.
 *
 *              By default, inline assembly (if available) is used to implement
 *              value barriers.
 *              Without inline assembly, mldsa-native will use a global volatile
 *              'opt blocker' instead; see ct.h.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_NO_ASM_VALUE_BARRIER */

/******************************************************************************
 * Name:        MLD_CONFIG_EXTERNAL_API_QUALIFIER
 *
 * Description: If set, this option provides an additional function
 *              qualifier to be added to declarations of mldsa-native's
 *              public API.
 *
 *              The primary use case for this option are single-CU builds
 *              where the public API exposed by mldsa-native is wrapped by
 *              another API in the consuming application. In this case,
 *              even mldsa-native's public API can be marked `static`.
 *
 *****************************************************************************/
#if !defined(__ASSEMBLER__)
#include <oqs/common.h>
#define MLD_CONFIG_EXTERNAL_API_QUALIFIER OQS_API
#endif

/******************************************************************************
 * Name:        MLD_CONFIG_SERIAL_FIPS202_ONLY
 *
 * Description: Set this to use a FIPS202 implementation with global state
 *              that supports only one active Keccak computation at a time
 *              (e.g. some hardware accelerators).
 *
 *              If this option is set, ML-DSA will use FIPS202 operations
 *              serially, ensuring that only one SHAKE context is active
 *              at any given time.
 *
 *              This allows offloading Keccak computations to a hardware
 *              accelerator that holds only a single Keccak state locally,
 *              rather than requiring support for multiple concurrent
 *              Keccak states.
 *
 *              NOTE: Depending on the target CPU, this may reduce
 *              performance when using software FIPS202 implementations.
 *              Only enable this when you have to.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_SERIAL_FIPS202_ONLY */

/*************************  Config internals  ********************************/

/* Default namespace
 *
 * Don't change this. If you need a different namespace, re-define
 * MLD_CONFIG_NAMESPACE_PREFIX above instead, and remove the following.
 *
 * The default MLDSA namespace for C integration is
 *
 *   PQCP_MLDSA_NATIVE_MLDSA<LEVEL>_C_
 *
 * e.g., PQCP_MLDSA_NATIVE_MLDSA44_C_
 */

#if MLD_CONFIG_PARAMETER_SET == 44
#define MLD_DEFAULT_NAMESPACE_PREFIX PQCP_MLDSA_NATIVE_MLDSA44_C
#elif MLD_CONFIG_PARAMETER_SET == 65
#define MLD_DEFAULT_NAMESPACE_PREFIX PQCP_MLDSA_NATIVE_MLDSA65_C
#elif MLD_CONFIG_PARAMETER_SET == 87
#define MLD_DEFAULT_NAMESPACE_PREFIX PQCP_MLDSA_NATIVE_MLDSA87_C
#endif

#endif /* !MLD_INTEGRATION_LIBOQS_CONFIG_C_H */
