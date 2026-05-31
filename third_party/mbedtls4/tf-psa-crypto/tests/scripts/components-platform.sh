# components-platform.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Platform Testing
################################################################

support_build_tf_psa_crypto_armcc () {
    armc6_cc="$ARMC6_BIN_DIR/armclang"
    (check_tools "$armc6_cc" > /dev/null 2>&1)
}
