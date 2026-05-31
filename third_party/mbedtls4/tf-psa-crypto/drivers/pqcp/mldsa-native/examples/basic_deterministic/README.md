[//]: # (SPDX-License-Identifier: CC-BY-4.0)


This directory contains a minimal example for building mldsa-native using only the deterministic API,
without requiring a `randombytes()` implementation.

## Use Case

Use this approach when:
- Your application manages its own entropy/randomness externally
- You only need `crypto_sign_keypair_internal` and `crypto_sign_signature_internal` (deterministic variants)

## Components

1. mldsa-native source tree: [`mldsa/src/`](../../mldsa/src) and [`mldsa/src/fips202/`](../../mldsa/src/fips202)
2. Your application source code

No `randombytes()` implementation is required.

## Configuration

The configuration file [mldsa_native_config.h](mldsa_native/mldsa_native_config.h) sets:
- `MLD_CONFIG_NO_RANDOMIZED_API`: Disables `crypto_sign_keypair`, `crypto_sign_signature`, etc. 
- `MLD_CONFIG_PARAMETER_SET`: Security level (default 65)
- `MLD_CONFIG_NAMESPACE_PREFIX`: Symbol prefix (set to `mldsa`)

## Notes

- This is incompatible with `MLD_CONFIG_KEYGEN_PCT` (pairwise consistency test)

## Usage

```bash
make build   # Build the example
make run     # Run the example
```
