# components-compliance.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Compliance Testing
################################################################

component_tf_psa_crypto_test_psa_compliance () {
    msg "unit test: test_psa_compliance.py"

    CC=gcc tests/scripts/test_psa_compliance.py
}

support_tf_psa_crypto_test_psa_compliance () {
    # psa-compliance-tests only supports CMake >= 3.10.0
    ver="$(cmake --version)"
    ver="${ver#cmake version }"
    ver_major="${ver%%.*}"

    ver="${ver#*.}"
    ver_minor="${ver%%.*}"

    [ "$ver_major" -eq 3 ] && [ "$ver_minor" -ge 10 ]
}
