[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Bring Your Own FIPS-202 (Static State Variant)

This directory contains a minimal example for using mldsa-native with a custom FIPS-202 implementation
that uses a single global state. This is common for hardware accelerators that can only hold one
Keccak state at a time.

## Use Case

Use this approach when:
- You need only one ML-DSA parameter set (44, 65, or 87)
- Your application already has a FIPS-202 software/hardware implementation you want to reuse
- Your FIPS-202 implementation does not support multiple active SHA3/SHAKE computations.

## Components

1. Arithmetic part of mldsa-native: [`mldsa/src/`](../../mldsa/src) (excluding `fips202/`)
2. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
3. Custom FIPS-202 implementation with headers compatible with [`fips202.h`](../../mldsa/src/fips202/fips202.h)
4. Your application source code

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_SERIAL_FIPS202_ONLY`: Disables batched Keccak; matrix entries generated one at a time
- `MLD_CONFIG_FIPS202_CUSTOM_HEADER`: Path to your custom `fips202.h`

Your custom FIPS-202 implementation must provide:
- `mld_shake128_init()`, `mld_shake128_absorb()`, `mld_shake128_finalize()`, `mld_shake128_squeeze()`, `mld_shake128_release()`
- `mld_shake256_init()`, `mld_shake256_absorb()`, `mld_shake256_finalize()`, `mld_shake256_squeeze()`, `mld_shake256_release()`
- `mld_shake256`
- Structure definitions for `mld_shake128ctx` and `mld_shake256ctx`

## Notes

- `MLD_CONFIG_SERIAL_FIPS202_ONLY` may reduce performance on CPUs with SIMD support
- Matrix and vector generation becomes sequential instead of batched (4 entries at a time)
- Only enable this when your hardware requires it

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
