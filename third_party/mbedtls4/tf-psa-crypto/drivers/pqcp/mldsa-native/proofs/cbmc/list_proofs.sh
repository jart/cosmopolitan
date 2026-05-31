#!/usr/bin/env bash
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
#
# This tiny script just lists the proof directories in proof/cbmc,
# which are those containing a *harness.c file.

ROOT=$(git rev-parse --show-toplevel)
cd $ROOT
ls -1 proofs/cbmc/**/*harness.c | cut -d '/' -f 3
