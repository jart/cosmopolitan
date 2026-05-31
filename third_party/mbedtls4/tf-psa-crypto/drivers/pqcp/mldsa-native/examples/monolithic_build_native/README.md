[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Monolithic Build (Native Backend)

This directory contains a minimal example for building mldsa-native as a single compilation unit
with native assembly backends, using the auto-generated `mldsa_native.c` and `mldsa_native.S` files.

## Use Case

Use this approach when:
- You need only one ML-DSA parameter set (44, 65, or 87)
- You want simple build integration with optimal performance

## Components

1. Source tree [mldsa_native/*](mldsa_native), including top-level compilation unit
   [mldsa_native.c](mldsa_native/mldsa_native.c) (gathering all C sources),
   [mldsa_native.S](mldsa_native/mldsa_native.S) (gathering all assembly sources),
   and the mldsa-native API [mldsa_native.h](mldsa_native/mldsa_native.h).
2. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
3. Your application source code

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_PARAMETER_SET`: Security level (default 65)
- `MLD_CONFIG_NAMESPACE_PREFIX`: Symbol prefix (set to `mldsa`)
- `MLD_CONFIG_USE_NATIVE_BACKEND_ARITH`: Enables native arithmetic backend
- `MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202`: Enables native FIPS-202 backend

## Notes

- Both `mldsa_native.c` and `mldsa_native.S` must be compiled and linked
- Native backends are auto-selected based on target architecture
- On unsupported platforms, the C backend is used automatically

## Usage

```bash
make build   # Build the example
make run     # Run the example
```

## Warning

The `randombytes()` implementation in `test_only_rng/` is for TESTING ONLY.
You MUST provide a cryptographically secure RNG for production use.
