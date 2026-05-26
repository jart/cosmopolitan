[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Monolithic Multi-Level Build (C Backend)

This directory contains a minimal example for building all three ML-DSA security levels in a single
compilation unit, with shared code deduplicated.

## Use Case

Use this approach when:
- You need all ML-DSA security levels in one application
- You want the simplest possible multi-level integration (one `.c` file)
- You're using only C (no native backend)

## Components

An application using mldsa-native as a monolithic multi-level build needs:

1. Source tree [mldsa_native/*](mldsa_native), including top-level compilation unit
   [mldsa_native.c](mldsa_native/mldsa_native.c) (gathering all C sources)
   and the mldsa-native API [mldsa_native.h](mldsa_native/mldsa_native.h).
2. Manually provided wrapper file [mldsa_native_all.c](mldsa_native_all.c),
   including `mldsa_native.c` three times.
3. Manually provided header file [mldsa_native_all.h](mldsa_native_all.h),
   including `mldsa_native.h` three times)
4. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
5. Your application source code

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_MULTILEVEL_BUILD`: Enables multi-level mode
- `MLD_CONFIG_NAMESPACE_PREFIX=mldsa`: Base prefix
- `MLD_CONFIG_INTERNAL_API_QUALIFIER=static`: Makes internal functions static
- `MLD_CONFIG_NO_SUPERCOP`: Disables the SUPERCOP API (crypto_sign*)

The wrapper [mldsa_native_all.c](mldsa_native_all.c) includes `mldsa_native.c` three times:
```c
#define MLD_CONFIG_FILE "multilevel_config.h"

/* Include level-independent code with first level */
#define MLD_CONFIG_MULTILEVEL_WITH_SHARED
#define MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS
#define MLD_CONFIG_PARAMETER_SET 44
#include "mldsa_native.c"
#undef MLD_CONFIG_PARAMETER_SET
#undef MLD_CONFIG_MULTILEVEL_WITH_SHARED

/* Exclude level-independent code for subsequent levels */
#define MLD_CONFIG_MULTILEVEL_NO_SHARED
#define MLD_CONFIG_PARAMETER_SET 65
#include "mldsa_native.c"
#undef MLD_CONFIG_PARAMETER_SET
#undef MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS

#define MLD_CONFIG_PARAMETER_SET 87
#include "mldsa_native.c"
#undef MLD_CONFIG_PARAMETER_SET
```

The header [mldsa_native_all.h](mldsa_native_all.h) exposes all APIs:
```c
#define MLD_CONFIG_PARAMETER_SET 44
#include <mldsa_native.h>
#undef MLD_CONFIG_PARAMETER_SET
#undef MLD_H

#define MLD_CONFIG_PARAMETER_SET 65
#include <mldsa_native.h>
#undef MLD_CONFIG_PARAMETER_SET
#undef MLD_H

#define MLD_CONFIG_PARAMETER_SET 87
#include <mldsa_native.h>
#undef MLD_CONFIG_PARAMETER_SET
#undef MLD_H
```

## Notes

- `MLD_CONFIG_MULTILEVEL_WITH_SHARED` must be set for exactly ONE level
- `MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS` prevents cleanup of shared headers between inclusions
- `MLD_CONFIG_NO_SUPERCOP` is required to avoid conflicting `CRYPTO_*` macro definitions

## Usage

```bash
make build   # Build the example
make run     # Run the example
```

## Warning

The `randombytes()` implementation in `test_only_rng/` is for TESTING ONLY.
You MUST provide a cryptographically secure RNG for production use.
