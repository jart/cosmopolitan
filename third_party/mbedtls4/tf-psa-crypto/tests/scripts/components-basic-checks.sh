# components-basic-checks.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Basic checks
################################################################

component_tf_psa_crypto_check_code_style () {
    msg "Check C code style"
    $FRAMEWORK/scripts/code_style.py
}

support_tf_psa_crypto_check_code_style () {
    case $(uncrustify --version) in
        *0.75.1*) true;;
        *) false;;
    esac
}

component_tf_psa_crypto_check_files () {
    msg "Check: file sanity checks (permissions, encodings)" # < 1s
    $FRAMEWORK/scripts/check_files.py
}

component_tf_psa_crypto_check_python_files () {
    msg "Lint: Python scripts"
    $FRAMEWORK/scripts/check-python-files.sh
}

component_tf_psa_crypto_check_generated_files () {
    msg "Check generated files"
    cd $OUT_OF_SOURCE_DIR
    cmake -D GEN_FILES=ON "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    cd $TF_PSA_CRYPTO_ROOT_DIR
    $FRAMEWORK/scripts/make_generated_files.py --root $OUT_OF_SOURCE_DIR --check
}

support_tf_psa_crypto_check_committed_generated_files () {
    # check_committed_generated_files.py runs generate_mldsa_tests.py,
    # which requires dilithium-py, which requires Python >=3.9.
    # Check the Python version, not the presence of the package,
    # because the CI runs `all.sh --list-components` outside of the
    # venv that has our desired packages.
    python3 -c 'import sys; exit(1 if sys.version_info < (3, 9) else 0)'
}

component_tf_psa_crypto_check_committed_generated_files () {
    msg "Check committed generated files"
    tests/scripts/check_committed_generated_files.py
}

component_tf_psa_crypto_check_recursion () {
    msg "Check: recursion.pl" # < 1s
    $FRAMEWORK/scripts/recursion.pl ${PSA_CORE_PATH}/*.c
    $FRAMEWORK/scripts/recursion.pl ${BUILTIN_SRC_PATH}/*.c
}

component_tf_psa_crypto_check_changelog () {
    msg "Check: changelog entries" # < 1s
    rm -f ChangeLog.new
    $FRAMEWORK/scripts/assemble_changelog.py -o ChangeLog.new
    if [ -e ChangeLog.new ]; then
        # Show the diff for information. It isn't an error if the diff is
        # non-empty.
        diff -u ChangeLog ChangeLog.new || true
        rm ChangeLog.new
    fi
}

component_tf_psa_crypto_check_test_cases () {
    msg "Check: test case descriptions" # < 1s
    if [ $QUIET -eq 1 ]; then
        opt='--quiet'
    else
        opt=''
    fi
    $FRAMEWORK/scripts/check_test_cases.py $opt
    unset opt
}

component_tf_psa_crypto_check_test_dependencies () {
    msg "Check: test case dependencies: legacy vs PSA" # < 1s
    # The purpose of this component is to catch unjustified dependencies on
    # legacy feature macros (MBEDTLS_xxx) in PSA tests. Generally speaking,
    # PSA test should use PSA feature macros (PSA_WANT_xxx, more rarely
    # MBEDTLS_PSA_xxx).
    #
    # Most of the time, use of legacy MBEDTLS_xxx macros are mistakes, which
    # this component is meant to catch. However a few of them are justified,
    # mostly by the absence of a PSA equivalent, so this component includes a
    # list of expected exceptions.

    found="check-test-deps-found-$$"
    expected="check-test-deps-expected-$$"

    # Find legacy dependencies in PSA tests
    grep 'depends_on' \
        tests/suites/test_suite_psa*.data \
        tests/suites/test_suite_psa*.function |
        grep -Eo '!?MBEDTLS_[^: ]*' |
        grep -v -e MBEDTLS_ENTROPY_HAVE_ -e MBEDTLS_PLATFORM -e MBEDTLS_PSA_ -e MBEDTLS_TEST_ |
        sort -u > $found

    # Expected ones with justification - keep in sorted order by ASCII table!
    rm -f $expected
    # No PSA equivalent - WANT_KEY_TYPE_AES means all sizes
    echo "!MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH" >> $expected
    # No PSA equivalent - used to skip decryption tests in PSA-ECB, CBC/XTS/NIST_KW/DES
    echo "!MBEDTLS_BLOCK_CIPHER_NO_DECRYPT" >> $expected
    # No PSA equivalent - we should probably have one in the future.
    echo "!MBEDTLS_ECP_RESTARTABLE" >> $expected
    # MBEDTLS_ASN1_WRITE_C is used by import_rsa_made_up() in test_suite_psa_crypto
    # in order to build a fake RSA key of the wanted size based on
    # PSA_VENDOR_RSA_MAX_KEY_BITS. The legacy module is only used by
    # the test code and that's probably the most convenient way of achieving
    # the test's goal.
    echo "MBEDTLS_ASN1_WRITE_C" >> $expected
    # No PSA equivalent - used in test_suite_psa_crypto to get some "known" size
    # for raw key generation.
    echo "MBEDTLS_CTR_DRBG_MAX_REQUEST" >> $expected
    # No PSA equivalent - we should probably have one in the future.
    echo "MBEDTLS_ECP_RESTARTABLE" >> $expected
    # No PSA equivalent - needed by some init tests
    echo "MBEDTLS_ENTROPY_NV_SEED" >> $expected
    # No PSA equivalent - required to run threaded tests.
    echo "MBEDTLS_THREADING_PTHREAD" >> $expected

    # Compare reality with expectation.
    # We want an exact match, to ensure the above list remains up-to-date.
    #
    # The output should be empty. When it's not:
    # - Each '+' line is a macro that was found but not expected. You want to
    # find where that macro occurs, and either replace it with PSA macros, or
    # add it to the exceptions list above with a justification.
    # - Each '-' line is a macro that was expected but not found; it means the
    # exceptions list above should be updated by removing that macro.
    diff -U0 $expected $found

    rm $found $expected
}

component_tf_psa_crypto_check_test_helpers () {
    # test_config_checks needs the generated headers in place.
    $FRAMEWORK/scripts/make_generated_files.py

    msg "unit test: generate_test_code.py"
    # unittest writes out mundane stuff like number or tests run on stderr.
    # Our convention is to reserve stderr for actual errors, and write
    # harmless info on stdout so it can be suppress with --quiet.
    ./framework/scripts/test_generate_test_code.py 2>&1

    msg "unit test: translate_ciphers.py"
    python3 -m unittest framework/scripts/translate_ciphers.py 2>&1

    msg "unit test: generate_config_checks.py"
    tests/scripts/test_config_checks.py 2>&1
}

component_tf_psa_crypto_check_names () {
    msg "Check: declared and exported names (builds the library)" # < 3s
    # Make generated files required for check
    ./scripts/generate_driver_wrappers.py
    $FRAMEWORK/scripts/check_names.py -v
}

component_tf_psa_crypto_check_doxy_blocks () {
    msg "Check: doxygen markup outside doxygen blocks" # < 1s
    $FRAMEWORK/scripts/check-doxy-blocks.pl
}

component_tf_psa_crypto_check_doxygen_warnings () {
    msg "Check: doxygen warnings (builds the documentation)" # ~ 3s
    $FRAMEWORK/scripts/doxygen.sh
}

component_tf_psa_crypto_check_headers_guards() {
    msg "Check C header files guards"
    scripts/c_header_guards.py
}
