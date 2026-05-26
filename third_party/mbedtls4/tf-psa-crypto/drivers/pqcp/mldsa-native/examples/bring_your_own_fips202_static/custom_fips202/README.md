<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Custom FIPS-202 Implementation (Static Global State)

This directory contains a custom FIPS-202 implementation that wraps `tiny_sha3`[^tiny_sha3]
using a **single global static state** for all operations.

## Purpose

This example demonstrates how mldsa-native can integrate with FIPS-202 implementations
that maintain a single global state, such as:
- Hardware accelerators with a single Keccak engine
- Embedded systems with limited memory
- Libraries that don't support multiple independent contexts

## How It Works

Instead of storing the Keccak state in each context structure, this implementation:

1. Uses a **static global state** for SHA-3/SHAKE operations
2. Provides **dummy context structures** that are ignored by the API functions
3. Includes **state machine assertions** to verify correct API usage
## Configuration

Enable this mode with:
```c
#define MLD_CONFIG_FIPS202_CUSTOM_HEADER "\"fips202.h\""
#define MLD_CONFIG_SERIAL_FIPS202_ONLY
```

The `MLD_CONFIG_SERIAL_FIPS202_ONLY` flag tells mldsa-native to avoid
using the parallel x4 API.

## Files

- `fips202.h` - Custom FIPS-202 wrapper with static global state
- `tiny_sha3/` - Symlink to the tiny_sha3 implementation
- `README.md` - This file

[^tiny_sha3]: https://github.com/mjosaarinen/tiny_sha3

<!--- bibliography --->
[^tiny_sha3]: Markku-Juhani O. Saarinen: tiny_sha3, [https://github.com/mjosaarinen/tiny_sha3](https://github.com/mjosaarinen/tiny_sha3)
