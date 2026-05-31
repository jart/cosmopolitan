#!/bin/sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

. "${0%/*}/../../framework/scripts/demo_common.sh"

msg <<'EOF'
This program demonstrates the use of the PSA cryptography interface to
compute a SHA-256 hash of a test string using the one-shot API call
and also using the multi-part operation API.
EOF

depends_on MBEDTLS_PSA_CRYPTO_C PSA_WANT_ALG_SHA_256

program="${0%/*}"/psa_hash

"$program"

cleanup
