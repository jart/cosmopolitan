[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# mldsa-native AArch64 backend SLOTHY-optimized code

This directory contains the AArch64 backend after it has been optimized by [SLOTHY](https://github.com/slothy-optimizer/slothy/).

## Re-running SLOTHY

If the "clean" sources [`../../aarch64_clean/src/*.S`](../../aarch64_clean/src/) change, take the following steps to re-optimize and install them into the main source tree:

1. Run `make` to re-generate the optimized sources using SLOTHY. This assumes a working SLOTHY setup, as established e.g. by the default nix shell for mldsa-native. See also the [SLOTHY README](https://github.com/slothy-optimizer/slothy/).

2. Run `autogen` to transfer the newly optimized files into the main source tree [mldsa/src/native](../../../mldsa/src/native).

3. Run `./scripts/tests all --opt=OPT` to check that the new assembly is still functional.
