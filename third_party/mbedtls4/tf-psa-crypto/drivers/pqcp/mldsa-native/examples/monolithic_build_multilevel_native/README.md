[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Monolithic Multi-Level Build (Native Backend)

This directory contains a minimal example for building all three ML-DSA security levels in a single
compilation unit with native assembly backends, with shared code deduplicated.

## Use Case

Use this approach when:
- You need all ML-DSA security levels in one application
- You want optimal performance via native assembly
- You want the simplest possible multi-level native integration

## Components

1. Source tree [mldsa_native/*](mldsa_native), including top-level compilation unit
   [mldsa_native.c](mldsa_native/mldsa_native.c) (gathering all C sources),
   [mldsa_native.S](mldsa_native/mldsa_native.S) (gathering all assembly sources),
   and the mldsa-native API [mldsa_native.h](mldsa_native/mldsa_native.h).
2. Manually provided wrapper file [mldsa_native_all.c](mldsa_native_all.c),
   including `mldsa_native.c` three times (in this example, we don't use a
   wrapper header since we directly include `mldsa_native_all.c` into `main.c`).
3. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
4. Your application source code

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_MULTILEVEL_BUILD`: Enables multi-level mode
- `MLD_CONFIG_NAMESPACE_PREFIX=mldsa`: Base prefix
- `MLD_CONFIG_USE_NATIVE_BACKEND_ARITH`: Enables native arithmetic backend
- `MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202`: Enables native FIPS-202 backend
- `MLD_CONFIG_NO_SUPERCOP`: Disables the SUPERCOP API (crypto_sign*)

The wrapper [mldsa_native_all.c](mldsa_native_all.c) includes `mldsa_native.c` three times:
```c
#define MLD_CONFIG_FILE "multilevel_config.h"

/* Include level-independent code with first level */
#define MLD_CONFIG_MULTILEVEL_WITH_SHARED 1
#define MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS
#define MLD_CONFIG_PARAMETER_SET 44
#include "mldsa_native.c"
#undef MLD_CONFIG_MULTILEVEL_WITH_SHARED
#undef MLD_CONFIG_PARAMETER_SET

/* Exclude level-independent code for subsequent levels */
#define MLD_CONFIG_MULTILEVEL_NO_SHARED
#define MLD_CONFIG_PARAMETER_SET 65
#include "mldsa_native.c"
#undef MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS
#undef MLD_CONFIG_PARAMETER_SET

#define MLD_CONFIG_PARAMETER_SET 87
#include "mldsa_native.c"
#undef MLD_CONFIG_PARAMETER_SET
```

The application [main.c](main.c) embeds the wrapper and imports constants:
```c
#include "mldsa_native_all.c"

#define MLD_CONFIG_CONSTANTS_ONLY
#include <mldsa_native.h>
```

## Notes

- Both `mldsa_native_all.c` and `mldsa_native.S` must be compiled and linked
- `MLD_CONFIG_MULTILEVEL_WITH_SHARED` must be set for exactly ONE level
- `MLD_CONFIG_CONSTANTS_ONLY` imports size constants without function declarations
- Native backends are auto-selected based on target architecture

## Usage

```bash
make build   # Build the example
make run     # Run the example
```

## Warning

The `randombytes()` implementation in `test_only_rng/` is for TESTING ONLY.
You MUST provide a cryptographically secure RNG for production use.
