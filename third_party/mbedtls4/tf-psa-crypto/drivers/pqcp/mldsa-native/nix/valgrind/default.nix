# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

{ valgrind, ... }:
valgrind.overrideAttrs (_: {
  patches = [
    ./valgrind-varlat-patch-20240808.txt
  ];
})
