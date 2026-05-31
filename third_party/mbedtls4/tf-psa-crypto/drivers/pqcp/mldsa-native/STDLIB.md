[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Standard Library Dependencies

mldsa-native has minimal dependencies on the C standard library. This document lists all stdlib functions used and configuration options for custom replacements.

## Dependencies

### Memory Functions
- **memcpy**: Used extensively for copying data structures, keys, and intermediate values (40+ occurrences)
- **memset**: Used for zeroing state structures and buffers (3 occurrences). **Note**: This is NOT used for security-critical zeroing - that is handled by `mld_zeroize` which has its own custom replacement mechanism

### Debug Functions (MLDSA_DEBUG builds only)
- **fprintf**: Used in debug.c for error reporting to stderr
- **exit**: Used in debug.c to terminate on assertion failures

## Custom Replacements

Custom replacements can be provided for memory functions using the configuration options in `mldsa/src/config.h`:

### MLD_CONFIG_CUSTOM_MEMCPY
Replaces all `memcpy` calls with a custom implementation. When enabled, you must define a `mld_memcpy` function with the same signature as the standard `memcpy`.

### MLD_CONFIG_CUSTOM_MEMSET
Replaces all `memset` calls with a custom implementation. When enabled, you must define a `mld_memset` function with the same signature as the standard `memset`.

See the configuration examples in `mldsa/src/config.h` and test configurations in `test/custom_*_config.h` for usage examples and implementation requirements.
