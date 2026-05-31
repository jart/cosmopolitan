#!/bin/sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

. "${0%/*}/../../framework/scripts/demo_common.sh"

msg <<'EOF'
This script demonstrates the use of the PSA cryptography interface to
create a master key, derive a key from it and use that derived key to
wrap some data using an AEAD algorithm.
EOF

depends_on MBEDTLS_SHA256_C MBEDTLS_MD_C MBEDTLS_AES_C MBEDTLS_CCM_C MBEDTLS_PSA_CRYPTO_C MBEDTLS_FS_IO

program="${0%/*}"/key_ladder_demo

if [ -e master.key ]; then
    echo "# Reusing the existing master.key file."
else
    files_to_clean="$files_to_clean master.key"
    run "Generate a master key." \
        "$program" generate master=master.key
fi

files_to_clean="$files_to_clean input.txt hello_world.wrap"
echo "Here is some input. See it wrapped." >input.txt
run "Derive a key and wrap some data with it." \
    "$program" wrap master=master.key label=hello label=world \
               input=input.txt output=hello_world.wrap

files_to_clean="$files_to_clean hello_world.txt"
run "Derive the same key again and unwrap the data." \
    "$program" unwrap master=master.key label=hello label=world \
               input=hello_world.wrap output=hello_world.txt
run "Compare the unwrapped data with the original input." \
    cmp input.txt hello_world.txt

files_to_clean="$files_to_clean hellow_orld.txt"
run_bad "Derive a different key and attempt to unwrap the data." \
  "$program" unwrap master=master.key input=hello_world.wrap output=hellow_orld.txt label=hellow label=orld

files_to_clean="$files_to_clean hello.key"
run "Save the first step of the key ladder, then load it as a master key and construct the rest of the ladder." \
    "$program" save master=master.key label=hello \
               input=hello_world.wrap output=hello.key
run "Check that we get the same key by unwrapping data made by the other key." \
    "$program" unwrap master=hello.key label=world \
               input=hello_world.wrap output=hello_world.txt

cleanup
