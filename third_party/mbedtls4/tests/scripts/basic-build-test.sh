#!/bin/sh

# basic-build-test.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# Purpose
#
# Executes the basic test suites, captures the results, and generates a simple
# test report and code coverage report.
#
# The tests include:
#   * Unit tests                - executed using tests/scripts/run-test-suite.pl
#   * Self-tests                - executed using the test suites above
#   * System tests              - executed using tests/ssl-opt.sh
#   * Interoperability tests    - executed using tests/compat.sh
#
# The tests focus on functionality and do not consider performance.
#
# Note the tests self-adapt due to configurations in include/mbedtls/mbedtls_config.h
# which can lead to some tests being skipped, and can cause the number of
# available tests to fluctuate.
#
# This script has been written to be generic and should work on any shell.
#
# Usage: basic-build-test.sh
#

# Abort on errors (and uninitiliased variables)
set -eu

if [ -d library -a -d include -a -d tests ]; then :; else
    echo "Must be run from Mbed TLS root" >&2
    exit 1
fi

: ${OPENSSL:="openssl"}
: ${GNUTLS_CLI:="gnutls-cli"}
: ${GNUTLS_SERV:="gnutls-serv"}

# Used to make ssl-opt.sh deterministic.
#
# See also RELEASE_SEED in all.sh. Debugging is easier if both values are kept
# in sync. If you change the value here because it breaks some tests, you'll
# definitely want to change it in all.sh as well.
: ${SEED:=1}
export SEED

# if MAKEFLAGS is not set add the -j option to speed up invocations of make
if [ -z "${MAKEFLAGS+set}" ]; then
    export MAKEFLAGS="-j"
fi

# To avoid setting OpenSSL and GnuTLS for each call to compat.sh and ssl-opt.sh
# we just export the variables they require
export OPENSSL="$OPENSSL"
export GNUTLS_CLI="$GNUTLS_CLI"
export GNUTLS_SERV="$GNUTLS_SERV"

CONFIG_H='include/mbedtls/mbedtls_config.h'
CONFIG_BAK="$CONFIG_H.bak"

# Step 0 - print build environment info
OPENSSL="$OPENSSL"                           \
    GNUTLS_CLI="$GNUTLS_CLI"                 \
    GNUTLS_SERV="$GNUTLS_SERV"               \
    framework/scripts/output_env.sh
echo

# Step 1 - Make and instrumented build for code coverage
export CFLAGS=' --coverage -g3 -O0 '
export LDFLAGS=' --coverage'
make -f scripts/legacy.make clean
cp "$CONFIG_H" "$CONFIG_BAK"
scripts/config.py full
make -f scripts/legacy.make

# Step 2 - Execute the tests
TEST_OUTPUT=out_${PPID}
cd tests
if [ ! -f "seedfile" ]; then
    dd if=/dev/urandom of="seedfile" bs=64 count=1
fi
if [ ! -f "../tf-psa-crypto/tests/seedfile" ]; then
    cp "seedfile" "../tf-psa-crypto/tests/seedfile"
fi
echo

# Step 2a - Unit Tests (keep going even if some tests fail)
echo '################ Unit tests ################'
perl scripts/run-test-suites.pl -v 2 |tee unit-test-$TEST_OUTPUT
echo '^^^^^^^^^^^^^^^^ Unit tests ^^^^^^^^^^^^^^^^'
echo

# Step 2b - System Tests (keep going even if some tests fail)
echo
echo '################ ssl-opt.sh ################'
echo "ssl-opt.sh will use SEED=$SEED for udp_proxy"
sh ssl-opt.sh |tee sys-test-$TEST_OUTPUT
echo '^^^^^^^^^^^^^^^^ ssl-opt.sh ^^^^^^^^^^^^^^^^'
echo

# Step 2c - Compatibility tests (keep going even if some tests fail)
echo '################ compat.sh ################'
{
    echo '#### compat.sh: Default versions'
    sh compat.sh -e 'ARIA\|CHACHA'
    echo

    echo '#### compat.sh: next (ARIA, ChaCha)'
    OPENSSL="$OPENSSL_NEXT" sh compat.sh -e '^$' -f 'ARIA\|CHACHA'
    echo
} | tee compat-test-$TEST_OUTPUT
echo '^^^^^^^^^^^^^^^^ compat.sh ^^^^^^^^^^^^^^^^'
echo

# Step 3 - Process the coverage report
cd ..
{
    make -f scripts/legacy.make lcov
    echo SUCCESS
} | tee tests/cov-$TEST_OUTPUT

if [ "$(tail -n1 tests/cov-$TEST_OUTPUT)" != "SUCCESS" ]; then
    echo >&2 "Fatal: 'make lcov' failed"
    exit 2
fi


# Step 4 - Summarise the test report
echo
echo "========================================================================="
echo "Test Report Summary"
echo

# A failure of the left-hand side of a pipe is ignored (this is a limitation
# of sh). We'll use the presence of this file as a marker that the generation
# of the report succeeded.
rm -f "tests/basic-build-test-$$.ok"

{

    cd tests

    # Step 4a - Unit tests
    echo "Unit tests - tests/scripts/run-test-suites.pl"

    PASSED_TESTS=$(tail -n6 unit-test-$TEST_OUTPUT|sed -n -e 's/test cases passed :[\t]*\([0-9]*\)/\1/p'| tr -d ' ')
    SKIPPED_TESTS=$(tail -n6 unit-test-$TEST_OUTPUT|sed -n -e 's/skipped :[ \t]*\([0-9]*\)/\1/p'| tr -d ' ')
    TOTAL_SUITES=$(tail -n6 unit-test-$TEST_OUTPUT|sed -n -e 's/.* (\([0-9]*\) .*, [0-9]* tests run)/\1/p'| tr -d ' ')
    FAILED_TESTS=$(tail -n6 unit-test-$TEST_OUTPUT|sed -n -e 's/failed :[\t]*\([0-9]*\)/\1/p' |tr -d ' ')

    echo "No test suites     : $TOTAL_SUITES"
    echo "Passed             : $PASSED_TESTS"
    echo "Failed             : $FAILED_TESTS"
    echo "Skipped            : $SKIPPED_TESTS"
    echo "Total exec'd tests : $(($PASSED_TESTS + $FAILED_TESTS))"
    echo "Total avail tests  : $(($PASSED_TESTS + $FAILED_TESTS + $SKIPPED_TESTS))"
    echo

    TOTAL_PASS=$PASSED_TESTS
    TOTAL_FAIL=$FAILED_TESTS
    TOTAL_SKIP=$SKIPPED_TESTS
    TOTAL_AVAIL=$(($PASSED_TESTS + $FAILED_TESTS + $SKIPPED_TESTS))
    TOTAL_EXED=$(($PASSED_TESTS + $FAILED_TESTS))

    # Step 4b - TLS Options tests
    echo "TLS Options tests - tests/ssl-opt.sh"

    PASSED_TESTS=$(tail -n5 sys-test-$TEST_OUTPUT|sed -n -e 's/.* (\([0-9]*\) \/ [0-9]* tests ([0-9]* skipped))$/\1/p')
    SKIPPED_TESTS=$(tail -n5 sys-test-$TEST_OUTPUT|sed -n -e 's/.* ([0-9]* \/ [0-9]* tests (\([0-9]*\) skipped))$/\1/p')
    TOTAL_TESTS=$(tail -n5 sys-test-$TEST_OUTPUT|sed -n -e 's/.* ([0-9]* \/ \([0-9]*\) tests ([0-9]* skipped))$/\1/p')
    FAILED_TESTS=$(($TOTAL_TESTS - $PASSED_TESTS))

    echo "Passed             : $PASSED_TESTS"
    echo "Failed             : $FAILED_TESTS"
    echo "Skipped            : $SKIPPED_TESTS"
    echo "Total exec'd tests : $TOTAL_TESTS"
    echo "Total avail tests  : $(($TOTAL_TESTS + $SKIPPED_TESTS))"
    echo

    TOTAL_PASS=$(($TOTAL_PASS+$PASSED_TESTS))
    TOTAL_FAIL=$(($TOTAL_FAIL+$FAILED_TESTS))
    TOTAL_SKIP=$(($TOTAL_SKIP+$SKIPPED_TESTS))
    TOTAL_AVAIL=$(($TOTAL_AVAIL + $TOTAL_TESTS + $SKIPPED_TESTS))
    TOTAL_EXED=$(($TOTAL_EXED + $TOTAL_TESTS))


    # Step 4c - System Compatibility tests
    echo "System/Compatibility tests - tests/compat.sh"

    PASSED_TESTS=$(cat compat-test-$TEST_OUTPUT | sed -n -e 's/.* (\([0-9]*\) \/ [0-9]* tests ([0-9]* skipped))$/\1/p' | awk 'BEGIN{ s = 0 } { s += $1 } END{ print s }')
    SKIPPED_TESTS=$(cat compat-test-$TEST_OUTPUT | sed -n -e 's/.* ([0-9]* \/ [0-9]* tests (\([0-9]*\) skipped))$/\1/p' | awk 'BEGIN{ s = 0 } { s += $1 } END{ print s }')
    EXED_TESTS=$(cat compat-test-$TEST_OUTPUT | sed -n -e 's/.* ([0-9]* \/ \([0-9]*\) tests ([0-9]* skipped))$/\1/p' | awk 'BEGIN{ s = 0 } { s += $1 } END{ print s }')
    FAILED_TESTS=$(($EXED_TESTS - $PASSED_TESTS))

    echo "Passed             : $PASSED_TESTS"
    echo "Failed             : $FAILED_TESTS"
    echo "Skipped            : $SKIPPED_TESTS"
    echo "Total exec'd tests : $EXED_TESTS"
    echo "Total avail tests  : $(($EXED_TESTS + $SKIPPED_TESTS))"
    echo

    TOTAL_PASS=$(($TOTAL_PASS+$PASSED_TESTS))
    TOTAL_FAIL=$(($TOTAL_FAIL+$FAILED_TESTS))
    TOTAL_SKIP=$(($TOTAL_SKIP+$SKIPPED_TESTS))
    TOTAL_AVAIL=$(($TOTAL_AVAIL + $EXED_TESTS + $SKIPPED_TESTS))
    TOTAL_EXED=$(($TOTAL_EXED + $EXED_TESTS))


    # Step 4d - Grand totals
    echo "-------------------------------------------------------------------------"
    echo "Total tests"

    echo "Total Passed       : $TOTAL_PASS"
    echo "Total Failed       : $TOTAL_FAIL"
    echo "Total Skipped      : $TOTAL_SKIP"
    echo "Total exec'd tests : $TOTAL_EXED"
    echo "Total avail tests  : $TOTAL_AVAIL"
    echo


    # Step 4e - Coverage report
    echo "Coverage statistics:"
    sed -n '1,/^Overall coverage/d; /%/p' cov-$TEST_OUTPUT
    echo

    rm unit-test-$TEST_OUTPUT
    rm sys-test-$TEST_OUTPUT
    rm compat-test-$TEST_OUTPUT
    rm cov-$TEST_OUTPUT

    # Mark the report generation as having succeeded. This must be the
    # last thing in the report generation.
    touch "basic-build-test-$$.ok"
} | tee coverage-summary.txt

make -f scripts/legacy.make clean

if [ -f "$CONFIG_BAK" ]; then
    mv "$CONFIG_BAK" "$CONFIG_H"
fi

# The file must exist, otherwise it means something went wrong while generating
# the coverage report. If something did go wrong, rm will complain so this
# script will exit with a failure status.
rm "tests/basic-build-test-$$.ok"
