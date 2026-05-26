#!/bin/sh

# context-info.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# This program is intended for testing the ssl_context_info program
#

set -eu

if ! cd "$(dirname "$0")"; then
    exit 125
fi

# Variables

THIS_SCRIPT_NAME=$(basename "$0")
PROG_PATH="../programs/ssl/ssl_context_info"
OUT_FILE="ssl_context_info.log"
IN_DIR="../framework/data_files/base64"

USE_VALGRIND=0

T_COUNT=0
T_PASSED=0
T_FAILED=0


# Functions

print_usage() {
    echo "Usage: $0 [options]"
    printf "  -h|--help\tPrint this help.\n"
    printf "  -m|--memcheck\tUse valgrind to check the memory.\n"
}

# Print test name <name>
print_name() {
    printf "%s %.*s " "$1" $(( 71 - ${#1} )) \
    "........................................................................"
}

# Print header to the test output file <test name> <file path> <test command>
print_header()
{
    date="$(date)"
    echo "******************************************************************" >  $2
    echo "* File created by: $THIS_SCRIPT_NAME"                               >> $2
    echo "* Test name:  $1"                                                   >> $2
    echo "* Date:       $date"                                                >> $2
    echo "* Command:    $3"                                                   >> $2
    echo "******************************************************************" >> $2
    echo ""                                                                   >> $2
}

# Print footer at the end of file <file path>
print_footer()
{
    echo ""                                                                   >> $1
    echo "******************************************************************" >> $1
    echo "* End command"                                                      >> $1
    echo "******************************************************************" >> $1
    echo ""                                                                   >> $1
}

# Use the arguments of this script
get_options() {
    while [ $# -gt 0 ]; do
        case "$1" in
            -h|--help)
                print_usage
                exit 0
                ;;
            -m|--memcheck)
                USE_VALGRIND=1
                ;;
            *)
                echo "Unknown argument: '$1'"
                print_usage
                exit 1
                ;;
        esac
        shift
    done
}

# Current test failed
fail()
{
    T_FAILED=$(( $T_FAILED + 1))
    FAIL_OUT="Fail.$T_FAILED""_$OUT_FILE"

    echo "FAIL"
    echo "    Error: $1"

    cp -f "$OUT_FILE" "$FAIL_OUT"
    echo "Error: $1" >> "$FAIL_OUT"
}

# Current test passed
pass()
{
    T_PASSED=$(( $T_PASSED + 1))
    echo "PASS"
}

# Usage: run_test <name> <input file with b64 code> [ -arg <extra arguments for tested program> ] [option [...]]
# Options:  -m <pattern that MUST be present in the output of tested program>
#           -n <pattern that must NOT be present in the output of tested program>
#           -u <pattern that must be UNIQUE in the output of tested program>
run_test()
{
    TEST_NAME="$1"
    RUN_CMD="$PROG_PATH -f $IN_DIR/$2"

    if [ "-arg" = "$3" ]; then
        RUN_CMD="$RUN_CMD $4"
        shift 4
    else
        shift 2
    fi

    # prepend valgrind to our commands if active
    if [ "$USE_VALGRIND" -gt 0 ]; then
        RUN_CMD="valgrind --leak-check=full $RUN_CMD"
    fi

    T_COUNT=$(( $T_COUNT + 1))
    print_name "$TEST_NAME"

    # run tested program
    print_header "$TEST_NAME" "$OUT_FILE" "$RUN_CMD"
    eval "$RUN_CMD" >> "$OUT_FILE" 2>&1
    print_footer "$OUT_FILE"

    # check valgrind's results
    if [ "$USE_VALGRIND" -gt 0 ]; then
        if ! ( grep -F 'All heap blocks were freed -- no leaks are possible' "$OUT_FILE" &&
             grep -F 'ERROR SUMMARY: 0 errors from 0 contexts' "$OUT_FILE" ) > /dev/null
        then
            fail "Memory error detected"
            return
        fi
    fi

    # check other assertions
    # lines beginning with == are added by valgrind, ignore them, because we already checked them before
    # lines with 'Serious error when reading debug info', are valgrind issues as well
    # lines beginning with * are added by this script, ignore too
    while [ $# -gt 0 ]
    do
        case $1 in
            "-m")
                if grep -v '^==' "$OUT_FILE" | grep -v 'Serious error when reading debug info' | grep -v "^*" | grep "$2" >/dev/null; then :; else
                    fail "pattern '$2' MUST be present in the output"
                    return
                fi
                ;;

            "-n")
                if grep -v '^==' "$OUT_FILE" | grep -v 'Serious error when reading debug info' | grep -v "^*" | grep "$2" >/dev/null; then
                    fail "pattern '$2' MUST NOT be present in the output"
                    return
                fi
                ;;

            "-u")
                if [ $(grep -v '^==' "$OUT_FILE"| grep -v 'Serious error when reading debug info' | grep -v "^*" | grep "$2" | wc -l) -ne 1 ]; then
                    fail "lines following pattern '$2' must be once in the output"
                    return
                fi
                ;;

            *)
                echo "Unknown test: $1" >&2
                exit 1
        esac
        shift 2
    done

    rm -f "$OUT_FILE"

    pass
}

get_options "$@"

# Tests

run_test "Default configuration, server" \
         "srv_def.txt" \
         -n "ERROR" \
         -u "major.* 2$" \
         -u "minor.* 21$" \
         -u "path.* 0$" \
         -u "MBEDTLS_HAVE_TIME$" \
         -u "MBEDTLS_X509_CRT_PARSE_C$" \
         -u "MBEDTLS_SSL_MAX_FRAGMENT_LENGTH$" \
         -u "MBEDTLS_SSL_ENCRYPT_THEN_MAC$" \
         -u "MBEDTLS_SSL_SESSION_TICKETS$" \
         -u "MBEDTLS_SSL_SESSION_TICKETS and client$" \
         -u "MBEDTLS_SSL_DTLS_ANTI_REPLAY$" \
         -u "MBEDTLS_SSL_ALPN$" \
         -u "ciphersuite.* TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256$" \
         -u "cipher flags.* 0x00$" \
         -u "Message-Digest.* 9$" \
         -u "compression.* disabled$" \
         -u "DTLS datagram packing.* enabled$" \
         -n "Certificate" \
         -n "bytes left to analyze from context"

run_test "Default configuration, client" \
         "cli_def.txt" \
         -n "ERROR" \
         -u "major.* 2$" \
         -u "minor.* 21$" \
         -u "path.* 0$" \
         -u "MBEDTLS_HAVE_TIME$" \
         -u "MBEDTLS_X509_CRT_PARSE_C$" \
         -u "MBEDTLS_SSL_MAX_FRAGMENT_LENGTH$" \
         -u "MBEDTLS_SSL_ENCRYPT_THEN_MAC$" \
         -u "MBEDTLS_SSL_SESSION_TICKETS$" \
         -u "MBEDTLS_SSL_SESSION_TICKETS and client$" \
         -u "MBEDTLS_SSL_DTLS_ANTI_REPLAY$" \
         -u "MBEDTLS_SSL_ALPN$" \
         -u "ciphersuite.* TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256$" \
         -u "cipher flags.* 0x00$" \
         -u "Message-Digest.* 9$" \
         -u "compression.* disabled$" \
         -u "DTLS datagram packing.* enabled$" \
         -u "cert. version .* 3$" \
         -u "serial number.* 02$" \
         -u "issuer name.* C=NL, O=PolarSSL, CN=PolarSSL Test CA$" \
         -u "subject name.* C=NL, O=PolarSSL, CN=localhost$" \
         -u "issued  on.* 2019-02-10 14:44:06$" \
         -u "expires on.* 2029-02-10 14:44:06$" \
         -u "signed using.* RSA with SHA-256$" \
         -u "RSA key size.* 2048 bits$" \
         -u "basic constraints.* CA=false$" \
         -n "bytes left to analyze from context"

run_test "No packing, server" \
         "srv_no_packing.txt" \
         -n "ERROR" \
         -u "DTLS datagram packing.* disabled"

run_test "No packing, client" \
         "cli_no_packing.txt" \
         -n "ERROR" \
         -u "DTLS datagram packing.* disabled"

run_test "DTLS CID, server" \
         "srv_cid.txt" \
         -n "ERROR" \
         -u "in CID.* DE AD" \
         -u "out CID.* BE EF"

run_test "DTLS CID, client" \
         "cli_cid.txt" \
         -n "ERROR" \
         -u "in CID.* BE EF" \
         -u "out CID.* DE AD"

run_test "No MBEDTLS_SSL_MAX_FRAGMENT_LENGTH, server" \
         "srv_no_mfl.txt" \
         -n "ERROR" \
         -n "MBEDTLS_SSL_MAX_FRAGMENT_LENGTH"

run_test "No MBEDTLS_SSL_MAX_FRAGMENT_LENGTH, client" \
         "cli_no_mfl.txt" \
         -n "ERROR" \
         -n "MBEDTLS_SSL_MAX_FRAGMENT_LENGTH"

run_test "No MBEDTLS_SSL_ALPN, server" \
         "srv_no_alpn.txt" \
         -n "ERROR" \
         -n "MBEDTLS_SSL_ALPN"

run_test "No MBEDTLS_SSL_ALPN, client" \
         "cli_no_alpn.txt" \
         -n "ERROR" \
         -n "MBEDTLS_SSL_ALPN"

run_test "No MBEDTLS_SSL_KEEP_PEER_CERTIFICATE, server" \
         "srv_no_keep_cert.txt" \
         -arg "--keep-peer-cert=0" \
         -u "ciphersuite.* TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256$" \
         -u "cipher flags.* 0x00" \
         -u "compression.* disabled" \
         -u "DTLS datagram packing.* enabled" \
         -n "ERROR"

run_test "No MBEDTLS_SSL_KEEP_PEER_CERTIFICATE, client" \
         "cli_no_keep_cert.txt" \
         -arg "--keep-peer-cert=0" \
         -u "ciphersuite.* TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256$" \
         -u "cipher flags.* 0x00" \
         -u "compression.* disabled" \
         -u "DTLS datagram packing.* enabled" \
         -n "ERROR"

run_test "No MBEDTLS_SSL_KEEP_PEER_CERTIFICATE, negative, server" \
         "srv_no_keep_cert.txt" \
         -m "Deserializing" \
         -m "ERROR"

run_test "No MBEDTLS_SSL_KEEP_PEER_CERTIFICATE, negative, client" \
         "cli_no_keep_cert.txt" \
         -m "Deserializing" \
         -m "ERROR"

run_test "Minimal configuration, server" \
         "srv_min_cfg.txt" \
         -n "ERROR" \
         -n "MBEDTLS_SSL_MAX_FRAGMENT_LENGTH$" \
         -n "MBEDTLS_SSL_ENCRYPT_THEN_MAC$" \
         -n "MBEDTLS_SSL_SESSION_TICKETS$" \
         -n "MBEDTLS_SSL_SESSION_TICKETS and client$" \
         -n "MBEDTLS_SSL_DTLS_ANTI_REPLAY$" \
         -n "MBEDTLS_SSL_ALPN$" \

run_test "Minimal configuration, client" \
         "cli_min_cfg.txt" \
         -n "ERROR" \
         -n "MBEDTLS_SSL_MAX_FRAGMENT_LENGTH$" \
         -n "MBEDTLS_SSL_ENCRYPT_THEN_MAC$" \
         -n "MBEDTLS_SSL_SESSION_TICKETS$" \
         -n "MBEDTLS_SSL_SESSION_TICKETS and client$" \
         -n "MBEDTLS_SSL_DTLS_ANTI_REPLAY$" \
         -n "MBEDTLS_SSL_ALPN$" \

run_test "MTU=10000" \
         "mtu_10000.txt" \
         -n "ERROR" \
         -u "MTU.* 10000$"

run_test "MFL=1024" \
         "mfl_1024.txt" \
         -n "ERROR" \
         -u "MFL.* 1024$"

run_test "Older version (v2.19.1)" \
         "v2.19.1.txt" \
         -n "ERROR" \
         -u "major.* 2$" \
         -u "minor.* 19$" \
         -u "path.* 1$" \
         -u "ciphersuite.* TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8$" \
         -u "Message-Digest.* 9$" \
         -u "compression.* disabled$" \
         -u "serial number.* 01:70:AF:40:B4:E6$" \
         -u "issuer name.* CN=ca$" \
         -u "subject name.* L=160001, OU=acc1, CN=device01$" \
         -u "issued  on.* 2020-03-06 09:50:18$" \
         -u "expires on.* 2056-02-26 09:50:18$" \
         -u "signed using.* ECDSA with SHA256$" \
         -u "lifetime.* 0 sec.$" \
         -u "MFL.* none$" \
         -u "negotiate truncated HMAC.* disabled$" \
         -u "Encrypt-then-MAC.* enabled$" \
         -u "DTLS datagram packing.* enabled$" \
         -u "verify result.* 0x00000000$" \
         -n "bytes left to analyze from context"

run_test "Wrong base64 format" \
         "def_bad_b64.txt" \
         -m "ERROR" \
         -u "The length of the base64 code found should be a multiple of 4" \
         -n "bytes left to analyze from context"

run_test "Too much data at the beginning of base64 code" \
         "def_b64_too_big_1.txt" \
         -m "ERROR" \
         -n "The length of the base64 code found should be a multiple of 4" \

run_test "Too much data in the middle of base64 code" \
         "def_b64_too_big_2.txt" \
         -m "ERROR" \
         -n "The length of the base64 code found should be a multiple of 4" \

run_test "Too much data at the end of base64 code" \
         "def_b64_too_big_3.txt" \
         -m "ERROR" \
         -n "The length of the base64 code found should be a multiple of 4" \
         -u "bytes left to analyze from context"

run_test "Empty file as input" \
         "empty.txt" \
         -u "Finished. No valid base64 code found"

run_test "Not empty file without base64 code" \
         "../../../tests/context-info.sh" \
         -n "Deserializing"

run_test "Binary file instead of text file" \
         "../../../programs/ssl/ssl_context_info" \
         -m "ERROR" \
         -u "Too many bad symbols detected. File check aborted" \
         -n "Deserializing"

run_test "Decoder continues past 0xff character" \
         "def_b64_ff.bin" \
         -n "No valid base64" \
         -u "ciphersuite.* TLS-"


# End of tests

echo
if [ $T_FAILED -eq 0 ]; then
    echo "PASSED ( $T_COUNT tests )"
else
    echo "FAILED ( $T_FAILED / $T_COUNT tests )"
fi

exit $T_FAILED
