#!/bin/sh

# Make sure the doxygen documentation builds without warnings
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# Abort on errors (and uninitialised variables)
set -eu

. $(dirname "$0")/project_detection.sh

if in_mbedtls_repo || in_tf_psa_crypto_repo; then :; else
    echo "Must be run from Mbed TLS root or TF-PSA-Crypto root" >&2
    exit 1
fi

if $(dirname "$0")/apidoc_full.sh > doc.out 2>doc.err; then :; else
    cat doc.err
    echo "FAIL" >&2
    exit 1;
fi

cat doc.out doc.err | \
    grep -v "warning: ignoring unsupported tag" \
    > doc.filtered

if grep -E "(warning|error):" doc.filtered; then
    echo "FAIL" >&2
    exit 1;
fi

if in_mbedtls_repo && in_3_6_branch; then
    make apidoc_clean
fi

rm -f doc.out doc.err doc.filtered
