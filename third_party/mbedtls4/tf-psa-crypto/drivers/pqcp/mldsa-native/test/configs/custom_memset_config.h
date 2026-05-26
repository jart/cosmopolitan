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
 *
 * - [FIPS204]
 *   FIPS 204 Module-Lattice-Based Digital Signature Standard
 *   National Institute of Standards and Technology
 *   https://csrc.nist.gov/pubs/fips/204/final
 */

/*
 * WARNING: This file is auto-generated from scripts/autogen
 *          in the mldsa-native repository.
 *          Do not modify it directly.
 */

/*
 * Test configuration: Test configuration with custom memset
 *
 * This configuration differs from the default mldsa/mldsa_native_config.h in
 * the following places:
 *   - MLD_CONFIG_CUSTOM_MEMSET
 */


#ifndef MLD_CONFIG_H
#define MLD_CONFIG_H

/******************************************************************************
 * Name:        MLD_CONFIG_PARAMETER_SET
 *
 * Description: Specifies the parameter set for ML-DSA
 *              - MLD_CONFIG_PARAMETER_SET=44 corresponds to ML-DSA-44
 *              - MLD_CONFIG_PARAMETER_SET=65 corresponds to ML-DSA-65
 *              - MLD_CONFIG_PARAMETER_SET=87 corresponds to ML-DSA-87
 *
 *              If you want to support multiple parameter sets, build the
 *              library multiple times and set MLD_CONFIG_MULTILEVEL_BUILD.
 *              See MLD_CONFIG_MULTILEVEL_BUILD for how to do this while
 *              minimizing code duplication.
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
#ifndef MLD_CONFIG_PARAMETER_SET
#define MLD_CONFIG_PARAMETER_SET \
  44 /* Change this for different security strengths */
#endif

/******************************************************************************
 * Name:        MLD_CONFIG_FILE
 *
 * Description: If defined, this is a header that will be included instead
 *              of the default configuration file mldsa/mldsa_native_config.h.
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
/* #define MLD_CONFIG_FILE "mldsa_native_config.h" */

/******************************************************************************
 * Name:        MLD_CONFIG_NAMESPACE_PREFIX
 *
 * Description: The prefix to use to namespace global symbols from mldsa/.
 *
 *              In a multi-level build, level-dependent symbols will
 *              additionally be prefixed with the parameter set (44/65/87).
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
#if !defined(MLD_CONFIG_NAMESPACE_PREFIX)
#define MLD_CONFIG_NAMESPACE_PREFIX MLD_DEFAULT_NAMESPACE_PREFIX
#endif

/******************************************************************************
 * Name:        MLD_CONFIG_MULTILEVEL_BUILD
 *
 * Description: Set this if the build is part of a multi-level build supporting
 *              multiple parameter sets.
 *
 *              If you need only a single parameter set, keep this unset.
 *
 *              To build mldsa-native with support for all parameter sets,
 *              build it three times -- once per parameter set -- and set the
 *              option MLD_CONFIG_MULTILEVEL_WITH_SHARED for exactly one of
 *              them, and MLD_CONFIG_MULTILEVEL_NO_SHARED for the others.
 *              MLD_CONFIG_MULTILEVEL_BUILD should be set for all of them.
 *
 *              See examples/multilevel_build for an example.
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_MULTILEVEL_BUILD */

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
/* #define MLD_CONFIG_EXTERNAL_API_QUALIFIER */

/******************************************************************************
 * Name:        MLD_CONFIG_NO_RANDOMIZED_API
 *
 * Description: If this option is set, mldsa-native will be built without the
 *              randomized API functions (crypto_sign_keypair,
 *              crypto_sign, crypto_sign_signature, and
 *              crypto_sign_signature_extmu).
 *              This allows users to build mldsa-native without providing a
 *              randombytes() implementation if they only need the
 *              internal deterministic API
 *              (crypto_sign_keypair_internal, crypto_sign_signature_internal).
 *
 *              NOTE: This option is incompatible with MLD_CONFIG_KEYGEN_PCT
 *              as the current PCT implementation requires
 *              crypto_sign_signature().
 *
 *****************************************************************************/
/* #define MLD_CONFIG_NO_RANDOMIZED_API */

/******************************************************************************
 * Name:        MLD_CONFIG_NO_SUPERCOP
 *
 * Description: By default, mldsa_native.h exposes the mldsa-native API in the
 *              SUPERCOP naming convention (crypto_sign_xxx). If you don't need
 *              this, set MLD_CONFIG_NO_SUPERCOP.
 *
 *              NOTE: You must set this for a multi-level build as the SUPERCOP
 *              naming does not disambiguate between the parameter sets.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_NO_SUPERCOP */

/******************************************************************************
 * Name:        MLD_CONFIG_CONSTANTS_ONLY
 *
 * Description: If you only need the size constants (MLDSA_PUBLICKEYBYTES, etc.)
 *              but no function declarations, set MLD_CONFIG_CONSTANTS_ONLY.
 *
 *              This only affects the public header mldsa_native.h, not
 *              the implementation.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CONSTANTS_ONLY */

/******************************************************************************
 *
 * Build-only configuration options
 *
 * The remaining configurations are build-options only.
 * They do not affect the API described in mldsa_native.h.
 *
 *****************************************************************************/

#if defined(MLD_BUILD_INTERNAL)
/******************************************************************************
 * Name:        MLD_CONFIG_MULTILEVEL_WITH_SHARED
 *
 * Description: This is for multi-level builds of mldsa-native only. If you
 *              need only a single parameter set, keep this unset.
 *
 *              If this is set, all MLD_CONFIG_PARAMETER_SET-independent
 *              code will be included in the build, including code needed only
 *              for other parameter sets.
 *
 *              Example: TODO: add example
 *
 *              To build mldsa-native with support for all parameter sets,
 *              build it three times -- once per parameter set -- and set the
 *              option MLD_CONFIG_MULTILEVEL_WITH_SHARED for exactly one of
 *              them, and MLD_CONFIG_MULTILEVEL_NO_SHARED for the others.
 *
 *              See examples/multilevel_build_mldsa for an example.
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_MULTILEVEL_WITH_SHARED */

/******************************************************************************
 * Name:        MLD_CONFIG_MULTILEVEL_NO_SHARED
 *
 * Description: This is for multi-level builds of mldsa-native only. If you
 *              need only a single parameter set, keep this unset.
 *
 *              If this is set, no MLD_CONFIG_PARAMETER_SET-independent code
 *              will be included in the build.
 *
 *              To build mldsa-native with support for all parameter sets,
 *              build it three times -- once per parameter set -- and set the
 *              option MLD_CONFIG_MULTILEVEL_WITH_SHARED for exactly one of
 *              them, and MLD_CONFIG_MULTILEVEL_NO_SHARED for the others.
 *
 *              See examples/multilevel_build_mldsa for an example.
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_MULTILEVEL_NO_SHARED */

/******************************************************************************
 * Name:        MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS
 *
 * Description: This is only relevant for single compilation unit (SCU)
 *              builds of mldsa-native. In this case, it determines whether
 *              directives defined in parameter-set-independent headers should
 *              be #undef'ined or not at the of the SCU file. This is needed
 *              in multilevel builds.
 *
 *              See examples/multilevel_build_native for an example.
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS */

/******************************************************************************
 * Name:        MLD_CONFIG_USE_NATIVE_BACKEND_ARITH
 *
 * Description: Determines whether an native arithmetic backend should be used.
 *
 *              The arithmetic backend covers performance critical functions
 *              such as the number-theoretic transform (NTT).
 *
 *              If this option is unset, the C backend will be used.
 *
 *              If this option is set, the arithmetic backend to be use is
 *              determined by MLD_CONFIG_ARITH_BACKEND_FILE: If the latter is
 *              unset, the default backend for your the target architecture
 *              will be used. If set, it must be the name of a backend metadata
 *              file.
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
#if !defined(MLD_CONFIG_USE_NATIVE_BACKEND_ARITH)
/* #define MLD_CONFIG_USE_NATIVE_BACKEND_ARITH */
#endif

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
#if defined(MLD_CONFIG_USE_NATIVE_BACKEND_ARITH) && \
    !defined(MLD_CONFIG_ARITH_BACKEND_FILE)
#define MLD_CONFIG_ARITH_BACKEND_FILE "native/meta.h"
#endif

/******************************************************************************
 * Name:        MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202
 *
 * Description: Determines whether an native FIPS202 backend should be used.
 *
 *              The FIPS202 backend covers 1x/2x/4x-fold Keccak-f1600, which is
 *              the performance bottleneck of SHA3 and SHAKE.
 *
 *              If this option is unset, the C backend will be used.
 *
 *              If this option is set, the FIPS202 backend to be use is
 *              determined by MLD_CONFIG_FIPS202_BACKEND_FILE: If the latter is
 *              unset, the default backend for your the target architecture
 *              will be used. If set, it must be the name of a backend metadata
 *              file.
 *
 *              This can also be set using CFLAGS.
 *
 *****************************************************************************/
#if !defined(MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202)
/* #define MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202 */
#endif

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
#if defined(MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202) && \
    !defined(MLD_CONFIG_FIPS202_BACKEND_FILE)
#define MLD_CONFIG_FIPS202_BACKEND_FILE "fips202/native/auto.h"
#endif

/******************************************************************************
 * Name:        MLD_CONFIG_FIPS202_CUSTOM_HEADER
 *
 * Description: Custom header to use for FIPS-202
 *
 *              This should only be set if you intend to use a custom
 *              FIPS-202 implementation, different from the one shipped
 *              with mldsa-native.
 *
 *              If set, it must be the name of a file serving as the
 *              replacement for mldsa/src/fips202/fips202.h, and exposing
 *              the same API (see FIPS202.md).
 *
 *****************************************************************************/
/* #define MLD_CONFIG_FIPS202_CUSTOM_HEADER "SOME_FILE.h" */

/******************************************************************************
 * Name:        MLD_CONFIG_FIPS202X4_CUSTOM_HEADER
 *
 * Description: Custom header to use for FIPS-202-X4
 *
 *              This should only be set if you intend to use a custom
 *              FIPS-202 implementation, different from the one shipped
 *              with mldsa-native.
 *
 *              If set, it must be the name of a file serving as the
 *              replacement for mldsa/src/fips202/fips202x4.h, and exposing
 *              the same API (see FIPS202.md).
 *
 *****************************************************************************/
/* #define MLD_CONFIG_FIPS202X4_CUSTOM_HEADER "SOME_FILE.h" */

/******************************************************************************
 * Name:        MLD_CONFIG_CUSTOM_ZEROIZE
 *
 * Description: In compliance with @[FIPS204, Section 3.6.3], mldsa-native,
 *              zeroizes intermediate stack buffers before returning from
 *              function calls.
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
   #include "src/src.h"
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
 *              If this option is not set, mldsa-native expects a function
 *              int randombytes(uint8_t *out, size_t outlen).
 *
 *              Set this option and define `mld_randombytes` if you want to
 *              use a custom method to sample randombytes with a different name
 *              or signature.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CUSTOM_RANDOMBYTES
   #if !defined(__ASSEMBLER__)
   #include <stdint.h>
   #include "src/src.h"
   static MLD_INLINE int mld_randombytes(uint8_t *ptr, size_t len)
   {
       ... your implementation ...
       return 0;
   }
   #endif
*/

/******************************************************************************
 * Name:        MLD_CONFIG_CUSTOM_CAPABILITY_FUNC
 *
 * Description: mldsa-native backends may rely on specific hardware features.
 *              Those backends will only be included in an mldsa-native build
 *              if support for the respective features is enabled at
 *              compile-time. However, when building for a heteroneous set
 *              of CPUs to run the resulting binary/library on, feature
 *              detection at _runtime_ is needed to decided whether a backend
 *              can be used or not.
 *
 *              Set this option and define `mld_sys_check_capability` if you
 *              want to use a custom method to dispatch between implementations.
 *
 *              Return value 1 indicates that a capability is supported.
 *              Return value 0 indicates that a capability is not supported.
 *
 *              If this option is not set, mldsa-native uses compile-time
 *              feature detection only to decide which backend to use.
 *
 *              If you compile mldsa-native on a system with different
 *              capabilities than the system that the resulting binary/library
 *              will be run on, you must use this option.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CUSTOM_CAPABILITY_FUNC
   static MLD_INLINE int mld_sys_check_capability(mld_sys_cap cap)
   {
       ... your implementation ...
   }
*/

/******************************************************************************
 * Name:        MLD_CONFIG_CUSTOM_ALLOC_FREE [EXPERIMENTAL]
 *
 * Description: Set this option and define `MLD_CUSTOM_ALLOC` and
 *              `MLD_CUSTOM_FREE` if you want to use custom allocation for
 *              large local structures or buffers.
 *
 *              By default, all buffers/structures are allocated on the stack.
 *              If this option is set, most of them will be allocated via
 *              MLD_CUSTOM_ALLOC.
 *
 *              Parameters to MLD_CUSTOM_ALLOC:
 *              - T* v: Target pointer to declare.
 *              - T: Type of structure to be allocated
 *              - N: Number of elements to be allocated.
 *
 *              Parameters to MLD_CUSTOM_FREE:
 *              - T* v: Target pointer to free. May be NULL.
 *              - T: Type of structure to be freed.
 *              - N: Number of elements to be freed.
 *
 *              WARNING: This option is experimental!
 *              Its scope, configuration and function/macro signatures may
 *              change at any time. We expect a stable API for v2.
 *
 *              NOTE: Even if this option is set, some allocations further down
 *              the call stack will still be made from the stack. Those will
 *              likely be added to the scope of this option in the future.
 *
 *              NOTE: MLD_CUSTOM_ALLOC need not guarantee a successful
 *              allocation nor include error handling. Upon failure, the
 *              target pointer should simply be set to NULL. The calling
 *              code will handle this case and invoke MLD_CUSTOM_FREE.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CUSTOM_ALLOC_FREE
   #if !defined(__ASSEMBLER__)
   #include <stdlib.h>
   #define MLD_CUSTOM_ALLOC(v, T, N)                              \
     T* (v) = (T *)aligned_alloc(MLD_DEFAULT_ALIGN,               \
                                 MLD_ALIGN_UP(sizeof(T) * (N)))
   #define MLD_CUSTOM_FREE(v, T, N) free(v)
   #endif
*/

/******************************************************************************
 * Name:        MLD_CONFIG_CUSTOM_MEMCPY
 *
 * Description: Set this option and define `mld_memcpy` if you want to
 *              use a custom method to copy memory instead of the standard
 *              library memcpy function.
 *
 *              The custom implementation must have the same signature and
 *              behavior as the standard memcpy function:
 *              void *mld_memcpy(void *dest, const void *src, size_t n)
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CUSTOM_MEMCPY
   #if !defined(__ASSEMBLER__)
   #include <stdint.h>
   #include "src/src.h"
   static MLD_INLINE void *mld_memcpy(void *dest, const void *src, size_t n)
   {
       ... your implementation ...
   }
   #endif
*/

/******************************************************************************
 * Name:        MLD_CONFIG_CUSTOM_MEMSET
 *
 * Description: Set this option and define `mld_memset` if you want to
 *              use a custom method to set memory instead of the standard
 *              library memset function.
 *
 *              The custom implementation must have the same signature and
 *              behavior as the standard memset function:
 *              void *mld_memset(void *s, int c, size_t n)
 *
 *****************************************************************************/
#define MLD_CONFIG_CUSTOM_MEMSET
#if !defined(__ASSEMBLER__)
#include <stddef.h>
#include <stdint.h>
#include "../mldsa/src/sys.h"
static MLD_INLINE void *mld_memset(void *s, int c, size_t n)
{
  /* Simple byte-by-byte set implementation for testing */
  unsigned char *ptr = (unsigned char *)s;
  for (size_t i = 0; i < n; i++)
  {
    ptr[i] = (unsigned char)c;
  }
  return s;
}
#endif /* !__ASSEMBLER__ */


/******************************************************************************
 * Name:        MLD_CONFIG_INTERNAL_API_QUALIFIER
 *
 * Description: If set, this option provides an additional function
 *              qualifier to be added to declarations of internal API.
 *
 *              The primary use case for this option are single-CU builds,
 *              in which case this option can be set to `static`.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_INTERNAL_API_QUALIFIER */

/******************************************************************************
 * Name:        MLD_CONFIG_CT_TESTING_ENABLED
 *
 * Description: If set, mldsa-native annotates data as secret / public using
 *              valgrind's annotations VALGRIND_MAKE_MEM_UNDEFINED and
 *              VALGRIND_MAKE_MEM_DEFINED, enabling various checks for secret-
 *              dependent control flow of variable time execution (depending
 *              on the exact version of valgrind installed).
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CT_TESTING_ENABLED */

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
   #include "src/src.h"
   static MLD_INLINE int mld_break_pct(void)
   {
       ... return 0/1 depending on whether PCT should be broken ...
   }
   #endif
*/

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

/******************************************************************************
 * Name:        MLD_CONFIG_CONTEXT_PARAMETER
 *
 * Description: Set this to add a context parameter that is provided to public
 *              API functions and is then available in custom callbacks.
 *
 *              The type of the context parameter is configured via
 *              MLD_CONFIG_CONTEXT_PARAMETER_TYPE.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CONTEXT_PARAMETER */

/******************************************************************************
 * Name:        MLD_CONFIG_CONTEXT_PARAMETER_TYPE
 *
 * Description: Set this to define the type for the context parameter used by
 *              MLD_CONFIG_CONTEXT_PARAMETER.
 *
 *              This is only relevant if MLD_CONFIG_CONTEXT_PARAMETER is set.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_CONTEXT_PARAMETER_TYPE void* */

/******************************************************************************
 * Name:        MLD_CONFIG_REDUCE_RAM [EXPERIMENTAL]
 *
 * Description: Set this to reduce RAM usage.
 *              This trades memory for performance.
 *
 *              For expected memory usage, see the MLD_TOTAL_ALLOC_* constants
 *              defined in mldsa_native.h.
 *
 *              This option is useful for embedded systems with tight RAM
 *              constraints but relaxed performance requirements.
 *
 *              WARNING: This option is experimental!
 *              CBMC proofs do not currently cover this configuration option.
 *              Its scope and configuration may change at any time.
 *
 *****************************************************************************/
/* #define MLD_CONFIG_REDUCE_RAM */

/*************************  Config internals  ********************************/

#endif /* MLD_BUILD_INTERNAL */

/* Default namespace
 *
 * Don't change this. If you need a different namespace, re-define
 * MLD_CONFIG_NAMESPACE_PREFIX above instead, and remove the following.
 *
 * The default MLDSA namespace is
 *
 *   PQCP_MLDSA_NATIVE_MLDSA<LEVEL>_
 *
 * e.g., PQCP_MLDSA_NATIVE_MLDSA44_
 */

#if MLD_CONFIG_PARAMETER_SET == 44
#define MLD_DEFAULT_NAMESPACE_PREFIX PQCP_MLDSA_NATIVE_MLDSA44
#elif MLD_CONFIG_PARAMETER_SET == 65
#define MLD_DEFAULT_NAMESPACE_PREFIX PQCP_MLDSA_NATIVE_MLDSA65
#elif MLD_CONFIG_PARAMETER_SET == 87
#define MLD_DEFAULT_NAMESPACE_PREFIX PQCP_MLDSA_NATIVE_MLDSA87
#endif

#endif /* !MLD_CONFIG_H */
