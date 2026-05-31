[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Basic build

This directory contains a minimal example for how to build mldsa-native for a single security level.

## Use Case

Use this approach when:
- You need only one ML-DSA parameter set (44, 65, or 87)
- You want to build the mldsa-native C files separately, not as a single compilation unit.
- You're using C only, no native backends.

## Components

1. mldsa-native source tree: [`mldsa/src/`](../../mldsa/src) and [`mldsa/src/fips202/`](../../mldsa/src/fips202)
2. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
3. Your application source code

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_PARAMETER_SET`: Security level (44, 65, or 87). Default is 65.
- `MLD_CONFIG_NAMESPACE_PREFIX`: Symbol prefix for the API. Set to `mldsa` in this example.

To change the security level, modify `MLD_CONFIG_PARAMETER_SET` in the config file or pass it via CFLAGS.

## Usage

```bash
make build   # Build the example
make run     # Run the example
```

## Warning

The `randombytes()` implementation in `test_only_rng/` is for TESTING ONLY.
You MUST provide a cryptographically secure RNG for production use.
