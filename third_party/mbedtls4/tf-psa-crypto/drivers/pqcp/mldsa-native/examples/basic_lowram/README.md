[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Low RAM build

This directory contains a minimal example for how to build mldsa-native with reduced RAM usage.

## Use Case

Use this approach when:
- You are building for an embedded system with tight RAM constraints
- You need to minimize stack usage
- Performance is less critical than memory footprint

## Configuration

The `MLD_CONFIG_REDUCE_RAM` option enables optimizations that reduce RAM usage:
- Uses unions for major allocations to reduce stack usage
- Trades some performance for lower memory footprint

## Components

1. mldsa-native source tree: [`mldsa/src/`](../../mldsa/src) and [`mldsa/src/fips202/`](../../mldsa/src/fips202)
2. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
3. Your application source code

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_PARAMETER_SET`: Security level (44, 65, or 87). Default is 65.
- `MLD_CONFIG_NAMESPACE_PREFIX`: Symbol prefix for the API. Set to `mldsa` in this example.
- `MLD_CONFIG_REDUCE_RAM`: Enables reduced RAM usage optimizations.

To change the security level, modify `MLD_CONFIG_PARAMETER_SET` in the config file or pass it via CFLAGS.

## Usage

```bash
make build   # Build the example
make run     # Run the example
```

## Warning

The `randombytes()` implementation in `test_only_rng/` is for TESTING ONLY.
You MUST provide a cryptographically secure RNG for production use.
