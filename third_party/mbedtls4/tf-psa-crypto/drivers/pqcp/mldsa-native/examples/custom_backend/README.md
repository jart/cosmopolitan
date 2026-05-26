[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Custom FIPS-202 Backend

This directory contains a minimal example for using mldsa-native with a custom FIPS-202 *backend*
(as opposed to a complete custom FIPS-202 implementation). We use tiny_sha3[^tiny_sha3] as an example.

## Use Case

Use this approach when:
- You need only one ML-DSA parameter set (44, 65, 87)
- You want to replace the low-level Keccak-f1600 permutation
- You want to keep mldsa-native's FIPS-202 frontend (absorb/squeeze logic)

This differs from `bring_your_own_fips202` in that you only replace the *backend* (Keccak permutation),
not the entire FIPS-202 implementation.

## Components

1. Arithmetic part of mldsa-native: [`mldsa/src/`](../../mldsa/src)
2. FIPS-202 frontend: [`mldsa/src/fips202/`](../../mldsa/src/fips202) (can remove existing backends)
3. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
4. Custom FIPS-202 backend (see below)
5. Your application source code

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202`: Enables native FIPS-202 backend
- `MLD_CONFIG_FIPS202_BACKEND_FILE`: Path to your custom backend metadata file

A custom backend consists of:
1. A metadata header (e.g., [custom.h](mldsa_native/src/fips202/native/custom/custom.h)) that:
   - Sets `MLD_USE_FIPS202_X1_NATIVE` (and/or `X4`) to indicate which functions are replaced
   - Includes the implementation header
2. An implementation providing `mld_keccakf1600_native()` (and/or batched variants)

Example backend metadata file:
```c
#ifndef CUSTOM_FIPS202_BACKEND_H
#define CUSTOM_FIPS202_BACKEND_H

/* Indicate we're replacing 1-fold Keccak-f1600 */
#define MLD_USE_FIPS202_X1_NATIVE

/* Include the implementation */
#include "custom/src/keccak_impl.h"

#endif
```

## Notes

- The tiny_sha3 code uses byte-reversed Keccak state on big-endian targets; this example removes
  that reversal since mldsa-native's frontend assumes standard byte order

## Usage

```bash
make build   # Build the example
make run     # Run the example
```

## Warning

The `randombytes()` implementation in `test_only_rng/` is for TESTING ONLY.
You MUST provide a cryptographically secure RNG for production use.

<!--- bibliography --->
[^tiny_sha3]: Markku-Juhani O. Saarinen: tiny_sha3, [https://github.com/mjosaarinen/tiny_sha3](https://github.com/mjosaarinen/tiny_sha3)
