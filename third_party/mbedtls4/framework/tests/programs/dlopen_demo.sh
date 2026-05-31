#!/bin/sh

# Run the shared library dynamic loading demo program.
# This is only expected to work when Mbed TLS is built as a shared library.

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

SCRIPT_DIR=$(dirname "$0")
. "${SCRIPT_DIR}/../../scripts/demo_common.sh"

msg "Test the dynamic loading of libmbed*"

# Once demo_common.sh is sourced we'll have $root_dir pointing to the root
# path of Mbed TLS or TF-PSA-Crypto.
if is_mbedtls_root $root_dir; then
    msg "Running in Mbed TLS repo"
    program="$root_dir/programs/test/dlopen"
    library_dir="$root_dir/library"
else
    msg "Running in TF-PSA-Crypto repo"
    program="$root_dir/programs/test/tfpsacrypto_dlopen"
    library_dir="$root_dir/core"
fi

# Skip this test if we don't have a shared library build. Detect this
# through the absence of the demo program.
if [ ! -e "$program" ]; then
    msg "Error: demo program $program not found."
    # Exit with a success status so that this counts as a pass for run_demos.py.
    exit
fi

# ELF-based Unix-like (Linux, *BSD, Solaris, ...)
if [ -n "${LD_LIBRARY_PATH-}" ]; then
    LD_LIBRARY_PATH="$library_dir:$LD_LIBRARY_PATH"
else
    LD_LIBRARY_PATH="$library_dir"
fi
export LD_LIBRARY_PATH

# OSX/macOS
if [ -n "${DYLD_LIBRARY_PATH-}" ]; then
    DYLD_LIBRARY_PATH="$library_dir:$DYLD_LIBRARY_PATH"
else
    DYLD_LIBRARY_PATH="$library_dir"
fi
export DYLD_LIBRARY_PATH

msg "Running dynamic loading test program: $program"
msg "Loading libraries from: $library_dir"
"$program"
