#!/usr/bin/env perl

# Perl code that is executed to transform each original line from a library
# source file into the corresponding line in the test driver copy of the
# library. Add a LIBTESTDRIVER1_/libtestdriver1_ to mbedtls_xxx and psa_xxx
# symbols.

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

use warnings;
use File::Basename;

my @public_files = map { basename($_) } glob("../tf-psa-crypto/include/mbedtls/*.h");

my $public_files_regex = join('|', map { quotemeta($_) } @public_files);

my @private_files = map { basename($_) } glob("../tf-psa-crypto/include/mbedtls/private/*.h");

my $private_files_regex = join('|', map { quotemeta($_) } @private_files);

while (<>) {
    s!^(\s*#\s*include\s*[\"<])mbedtls/build_info.h!${1}libtestdriver1/include/mbedtls/build_info.h!;
    s!^(\s*#\s*include\s*[\"<])mbedtls/mbedtls_config.h!${1}libtestdriver1/include/mbedtls/mbedtls_config.h!;
    s!^(\s*#\s*include\s*[\"<])mbedtls/private/config_adjust_x509.h!${1}libtestdriver1/include/mbedtls/private/config_adjust_x509.h!;
    s!^(\s*#\s*include\s*[\"<])mbedtls/private/config_adjust_ssl.h!${1}libtestdriver1/include/mbedtls/private/config_adjust_ssl.h!;
    s!^(\s*#\s*include\s*[\"<])mbedtls/check_config.h!${1}libtestdriver1/include/mbedtls/check_config.h!;
    # Files in include/mbedtls and drivers/builtin/include/mbedtls are both
    # included in files via #include mbedtls/<file>.h, so when expanding to the
    # full path make sure that files in include/mbedtls are not expanded
    # to driver/builtin/include/mbedtls.
    if ( $public_files_regex ) {
        s!^(\s*#\s*include\s*[\"<])mbedtls/($public_files_regex)!${1}libtestdriver1/tf-psa-crypto/include/mbedtls/${2}!;
    }
    if ( $private_files_regex ) {
        s!^(\s*#\s*include\s*[\"<])mbedtls/private/($private_files_regex)!${1}libtestdriver1/tf-psa-crypto/include/mbedtls/private/${2}!;
    }
    s!^(\s*#\s*include\s*[\"<])mbedtls/!${1}libtestdriver1/tf-psa-crypto/drivers/builtin/include/mbedtls/!;
    s!^(\s*#\s*include\s*[\"<])psa/!${1}libtestdriver1/tf-psa-crypto/include/psa/!;
    s!^(\s*#\s*include\s*[\"<])tf-psa-crypto/!${1}libtestdriver1/tf-psa-crypto/include/tf-psa-crypto/!;
    if (/^\s*#\s*include/) {
        print;
        next;
    }
    s/\b(?=MBEDTLS_|PSA_|TF_PSA_CRYPTO_)/LIBTESTDRIVER1_/g;
    s/\b(?=mbedtls_|psa_|tf_psa_crypto_)/libtestdriver1_/g;
    print;
}
