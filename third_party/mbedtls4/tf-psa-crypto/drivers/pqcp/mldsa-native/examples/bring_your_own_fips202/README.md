[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Bring Your Own FIPS-202

This directory contains a minimal example for using mldsa-native with a custom FIPS-202 (SHA-3/SHAKE)
implementation. We use tiny_sha3[^tiny_sha3] as an example.

## Use Case

Use this approach when:
- You need only one ML-DSA parameter set (44, 65, or 87)
- Your application already has a FIPS-202 software/hardware implementation you want to reuse

## Components

1. Arithmetic part of mldsa-native: [`mldsa/src/`](../../mldsa/src) (excluding `fips202/`)
2. A secure random number generator implementing [`randombytes.h`](../../mldsa/src/randombytes.h)
3. Custom FIPS-202 implementation with headers compatible with:
   - [`fips202.h`](../../mldsa/src/fips202/fips202.h)
   - [`fips202x4.h`](../../mldsa/src/fips202/fips202x4.h)
4. Your application source code

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_FIPS202_CUSTOM_HEADER`: Path to your custom `fips202.h`
- `MLD_CONFIG_FIPS202X4_CUSTOM_HEADER`: Path to your custom `fips202x4.h`

Your custom FIPS-202 implementation must impelement the API specified in [FIPS202.md](../../FIPS202.md).

## Notes

- The 4x batched functions (`x4`) can fall back to 4 sequential calls if batching isn't available
- Structure definitions may differ from mldsa-native's defaults (e.g., for incremental hashing)

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
