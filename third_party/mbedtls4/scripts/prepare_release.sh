#!/bin/bash
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# prepare_release.sh — Prepare the source tree for a release.
#
# This script switches the repo into “release” mode:
#   - Updates all tracked `.gitignore` files to stop
#     ignoring the automatically-generated files.
#   - Sets the CMake option `GEN_FILES` to OFF to explicitely disable
#     recreating the automatically-generated files.
#.  - The script will recursively update the tf-psa-crypto files too.


set -eu

# Portable inline sed. Helper function that will automatically pre-pend
# an empty string as the backup suffix (required by macOS sed).
psed() {
    # macOS sed does not offer a version
    if sed --version >/dev/null 2>&1; then
        sed -i "$@"
    # macOS/BSD sed
    else
        sed -i '' "$@"
    fi
}

#### .gitignore processing ####
for GITIGNORE in $(git ls-files --recurse-submodules -- '*.gitignore'); do
        psed '/###START_GENERATED_FILES###/,/###END_GENERATED_FILES###/s/^/#/' "$GITIGNORE"
        psed 's/###START_GENERATED_FILES###/###START_COMMENTED_GENERATED_FILES###/' "$GITIGNORE"
        psed 's/###END_GENERATED_FILES###/###END_COMMENTED_GENERATED_FILES###/' "$GITIGNORE"
done

#### Build system ####
psed '/[Oo][Ff][Ff] in development/! s/^\( *option *( *GEN_FILES  *"[^"]*"  *\)\([A-Za-z0-9][A-Za-z0-9]*\)/\1OFF/' CMakeLists.txt tf-psa-crypto/CMakeLists.txt
