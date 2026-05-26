[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Usage examples

This directory contains minimal examples demonstrating how you can use mldsa-native.

## Basic

See [basic](basic) for a basic example of how to build a single instance of mldsa-native.

## Basic_deterministic

See [basic_deterministic](basic_deterministic) for a basic example of how to build a single instance of mldsa-native without `randombytes()` implementation. This allows users to build mldsa-native using only the deterministic API when randomized functions are not required.

## Basic_lowram

See [basic_lowram](basic_lowram) for a basic example of how to build a single instance of mldsa-native with reduced RAM usage (`MLD_CONFIG_REDUCE_RAM`). This is useful for embedded systems with tight RAM constraints.

## Multi-level build (C only)

See [multilevel_build](multilevel_build) for an example of how to build one instance of mldsa-native per security level,
in such a way that level-independent code is shared.

## Multi-level build (with native code)

See [multilevel_build_native](multilevel_build_native) for an example of how to build one instance of mldsa-native per
security level, in such a way that level-independent code is shared, and leveraging the native backends.

## Custom FIPS202 implementation

See [bring_your_own_fips202](bring_your_own_fips202) for an example of how to use mldsa-native with your own FIPS-202
implementation.

## Custom FIPS202 implementation (static state variant)

See [bring_your_own_fips202_static](bring_your_own_fips202_static) for an example of how to use mldsa-native with a
custom FIPS-202 implementation using a static state. This variant demonstrates the serial-only FIPS-202 configuration
(`MLD_CONFIG_SERIAL_FIPS202_ONLY`).

## Custom config + custom FIPS-202 backend

See [custom_backend](custom_backend) for an example of how to use mldsa-native with a custom configuration file and a
custom FIPS-202 backend.

## Monobuild (C only)

See [monolithic_build](monolithic_build) for an example of how to build mldsa-native (with C backend) from a single
auto-generated compilation unit.

## Multi-level monobuild (C only)

See [monolithic_build_multilevel](monolithic_build_multilevel) for an example of how to build all security levels of
mldsa-native (with C backend) inside a single compilation unit, sharing the level-independent code.

## Multi-level monobuild (with native code)

See [monolithic_build_multilevel_native](monolithic_build_multilevel_native) for an example of how to build all security
levels of mldsa-native inside a single compilation unit, sharing the level-independent code, while also linking in assembly
from the native backends.
