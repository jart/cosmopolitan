#! /usr/bin/env bash

# all.sh (mbedtls part)
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file is executable; it is the entry point for users and the CI.
# See "Files structure" in all-core.sh for other files used.

# This script must be invoked from the project's root.

FRAMEWORK="$PWD/framework"
source $FRAMEWORK/scripts/all-core.sh

main "$@"
