[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Multi-Level Build (C Backend)

This directory contains a minimal example for building mldsa-native with support for all three security levels
(ML-DSA-44, ML-DSA-65, ML-DSA-87), with level-independent code shared to reduce binary size.

## Use Case

Use this approach when:
- You need multiple ML-DSA security levels in the same application
- You want to minimize code duplication across levels
- You want to build the mldsa-native C files separately, not as a single compilation unit.
- You're only using C (no native backends)

## Components

1. mldsa-native source tree: [`mldsa/src/`](../../mldsa/src) and [`mldsa/src/fips202/`](../../mldsa/src/fips202)
2. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
3. Your application source code

## Configuration

The library is built 3 times into separate directories (`build/mldsa44`, `build/mldsa65`, `build/mldsa87`).

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_MULTILEVEL_BUILD`: Enables multi-level build mode
- `MLD_CONFIG_NAMESPACE_PREFIX=mldsa`: Base prefix; level suffix added automatically
- `MLD_CONFIG_NO_SUPERCOP`: Disables the SUPERCOP API (crypto_sign*)

Build-time flags passed via CFLAGS:
- `MLD_CONFIG_PARAMETER_SET=44/65/87`: Selects the security level
- `MLD_CONFIG_MULTILEVEL_WITH_SHARED`: Set for ONE build (e.g., 44) to include shared code
- `MLD_CONFIG_MULTILEVEL_NO_SHARED`: Set for OTHER builds to exclude shared code

The resulting API functions are namespaced as:
- `mldsa44_keypair()`, `mldsa44_signature()`, `mldsa44_verify()`, ...
- `mldsa65_keypair()`, `mldsa65_signature()`, `mldsa65_verify()`, ...
- `mldsa87_keypair()`, `mldsa87_signature()`, `mldsa87_verify()`, ...

## Usage

```bash
make build   # Build all three security levels
make run     # Run the example
```

## Warning

The `randombytes()` implementation in `test_only_rng/` is for TESTING ONLY.
You MUST provide a cryptographically secure RNG for production use.
