[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Replacing FIPS-202

If your library has a FIPS-202[^FIPS202] implementation, you can use it instead of the one shipped with mldsa-native.

1. Replace `mldsa/src/fips202/*` by your own FIPS-202 implementation.
2. Provide replacements for the headers [`mldsa/src/fips202/fips202.h`](mldsa/src/fips202/fips202.h) and [`mldsa/src/fips202/fips202x4.h`](mldsa/src/fips202/fips202x4.h) and the
functionalities specified therein:
  * Structure definitions for `mld_shake128ctx`, `mld_shake256ctx`, `mld_shake128x4ctx`, and `mld_shake256x4ctx`
  * `mld_shake128_init()`: Initialize a SHAKE-128 context
  * `mld_shake128_absorb()`: Absorb data into a SHAKE-128 context (can be called multiple times)
  * `mld_shake128_finalize()`: Finalize the absorb phase of a SHAKE-128 context
  * `mld_shake128_squeeze()`: Squeeze data from a SHAKE-128 context (can be called multiple times)
  * `mld_shake128_release()`: Release and securely zero a SHAKE-128 context after use
  * `mld_shake256_init()`: Initialize a SHAKE-256 context
  * `mld_shake256_absorb()`: Absorb data into a SHAKE-256 context (can be called multiple times)
  * `mld_shake256_finalize()`: Finalize the absorb phase of a SHAKE-256 context
  * `mld_shake256_squeeze()`: Squeeze data from a SHAKE-256 context (can be called multiple times)
  * `mld_shake256_release()`: Release and securely zero a SHAKE-256 context after use
  * `mld_shake256()`: One-shot SHAKE-256 operation
  * `mld_shake128x4_init()`: Initialize a 4x-batched SHAKE-128 context
  * `mld_shake128x4_absorb_once()`: Initialize and absorb into a 4x-batched SHAKE-128 context in one step
  * `mld_shake128x4_squeezeblocks()`: Squeeze blocks from a 4x-batched SHAKE-128 context
  * `mld_shake128x4_release()`: Release a 4x-batched SHAKE-128 context after use
  * `mld_shake256x4_init()`: Initialize a 4x-batched SHAKE-256 context
  * `mld_shake256x4_absorb_once()`: Initialize and absorb into a 4x-batched SHAKE-256 context in one step
  * `mld_shake256x4_squeezeblocks()`: Squeeze blocks from a 4x-batched SHAKE-256 context
  * `mld_shake256x4_release()`: Release a 4x-batched SHAKE-256 context after use

See [`mldsa/src/fips202/fips202.h`](mldsa/src/fips202/fips202.h) and [`mldsa/src/fips202/fips202x4.h`](mldsa/src/fips202/fips202x4.h) for more details. Note that the structure
definitions may differ from those shipped with mldsa-native.

## Example

See [`examples/bring_your_own_fips202/`](examples/bring_your_own_fips202/) for an example how to use a custom FIPS-202
implementation with tiny_sha3[^tiny_sha3].

<!--- bibliography --->
[^FIPS202]: National Institute of Standards and Technology: FIPS202 SHA-3 Standard: Permutation-Based Hash and Extendable-Output Functions, [https://csrc.nist.gov/pubs/fips/202/final](https://csrc.nist.gov/pubs/fips/202/final)
[^tiny_sha3]: Markku-Juhani O. Saarinen: tiny_sha3, [https://github.com/mjosaarinen/tiny_sha3](https://github.com/mjosaarinen/tiny_sha3)
