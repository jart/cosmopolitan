#!/bin/sh

# compat.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# Purpose
#
# Test interoperbility with OpenSSL, GnuTLS as well as itself.
#
# Check each common ciphersuite, with each version, both ways (client/server),
# with and without client authentication.

set -u

# Limit the size of each log to 10 GiB, in case of failures with this script
# where it may output seemingly unlimited length error logs.
ulimit -f 20971520

ORIGINAL_PWD=$PWD
if ! cd "$(dirname "$0")"; then
    exit 125
fi

# initialise counters
TESTS=0
FAILED=0
SKIPPED=0
SRVMEM=0

# default commands, can be overridden by the environment
: ${M_SRV:=../programs/ssl/ssl_server2}
: ${M_CLI:=../programs/ssl/ssl_client2}
: ${OPENSSL:=openssl}
: ${GNUTLS_CLI:=gnutls-cli}
: ${GNUTLS_SERV:=gnutls-serv}

# The OPENSSL variable used to be OPENSSL_CMD for historical reasons.
# To help the migration, error out if the old variable is set,
# but only if it has a different value than the new one.
if [ "${OPENSSL_CMD+set}" = set ]; then
    # the variable is set, we can now check its value
    if [ "$OPENSSL_CMD" != "$OPENSSL" ]; then
        echo "Please use OPENSSL instead of OPENSSL_CMD." >&2
        exit 125
    fi
fi

# do we have a recent enough GnuTLS?
if ( which $GNUTLS_CLI && which $GNUTLS_SERV ) >/dev/null 2>&1; then
    G_VER="$( $GNUTLS_CLI --version | head -n1 )"
    if echo "$G_VER" | grep '@VERSION@' > /dev/null; then # git version
        PEER_GNUTLS=" GnuTLS"
    else
        eval $( echo $G_VER | sed 's/.* \([0-9]*\)\.\([0-9]\)*\.\([0-9]*\)$/MAJOR="\1" MINOR="\2" PATCH="\3"/' )
        if [ $MAJOR -lt 3 -o \
            \( $MAJOR -eq 3 -a $MINOR -lt 2 \) -o \
            \( $MAJOR -eq 3 -a $MINOR -eq 2 -a $PATCH -lt 15 \) ]
        then
            PEER_GNUTLS=""
        else
            PEER_GNUTLS=" GnuTLS"
            if [ $MINOR -lt 4 ]; then
                GNUTLS_MINOR_LT_FOUR='x'
            fi
        fi
    fi
else
    PEER_GNUTLS=""
fi

guess_config_name() {
    if git diff --quiet ../include/mbedtls/mbedtls_config.h 2>/dev/null; then
        echo "default"
    else
        echo "unknown"
    fi
}
: ${MBEDTLS_TEST_OUTCOME_FILE=}
: ${MBEDTLS_TEST_CONFIGURATION:="$(guess_config_name)"}
: ${MBEDTLS_TEST_PLATFORM:="$(uname -s | tr -c \\n0-9A-Za-z _)-$(uname -m | tr -c \\n0-9A-Za-z _)"}

# default values for options
# /!\ keep this synchronised with:
# - basic-build-test.sh
# - all.sh (multiple components)
MODES="tls12 dtls12"
VERIFIES="NO YES"
TYPES="ECDSA RSA PSK"
FILTER=""
# By default, exclude:
# - NULL: excluded from our default config + requires OpenSSL legacy
# - ARIA: requires OpenSSL >= 1.1.1
# - ChachaPoly: requires OpenSSL >= 1.1.0
EXCLUDE='NULL\|ARIA\|CHACHA20_POLY1305'
VERBOSE=""
MEMCHECK=0
MIN_TESTS=1
PRESERVE_LOGS=0
PEERS="OpenSSL$PEER_GNUTLS mbedTLS"

# hidden option: skip DTLS with OpenSSL
# (travis CI has a version that doesn't work for us)
: ${OSSL_NO_DTLS:=0}

print_usage() {
    echo "Usage: $0"
    printf "  -h|--help\tPrint this help.\n"
    printf "  -f|--filter\tOnly matching ciphersuites are tested (Default: '%s')\n" "$FILTER"
    printf "  -e|--exclude\tMatching ciphersuites are excluded (Default: '%s')\n" "$EXCLUDE"
    printf "  -m|--modes\tWhich modes to perform (Default: '%s')\n" "$MODES"
    printf "  -t|--types\tWhich key exchange type to perform (Default: '%s')\n" "$TYPES"
    printf "  -V|--verify\tWhich verification modes to perform (Default: '%s')\n" "$VERIFIES"
    printf "  -p|--peers\tWhich peers to use (Default: '%s')\n" "$PEERS"
    printf "            \tAlso available: GnuTLS (needs v3.2.15 or higher)\n"
    printf "  -M|--memcheck\tCheck memory leaks and errors.\n"
    printf "  -v|--verbose\tSet verbose output.\n"
    printf "     --list-test-cases\tList all potential test cases (No Execution)\n"
    printf "     --min      \tMinimum number of non-skipped tests (default 1)\n"
    printf "     --outcome-file\tFile where test outcomes are written\n"
    printf "                   \t(default: \$MBEDTLS_TEST_OUTCOME_FILE, none if empty)\n"
    printf "     --preserve-logs\tPreserve logs of successful tests as well\n"
}

# print_test_case <CLIENT> <SERVER> <STANDARD_CIPHER_SUITE>
print_test_case() {
    for i in $3; do
        uniform_title $1 $2 $i
        echo "compat;$TITLE"
    done
}

# list_test_cases lists all potential test cases in compat.sh without execution
list_test_cases() {
    for TYPE in $TYPES; do
        reset_ciphersuites
        add_common_ciphersuites
        add_openssl_ciphersuites
        add_gnutls_ciphersuites
        add_mbedtls_ciphersuites

        # PSK cipher suites do not allow client certificate verification.
        SUB_VERIFIES=$VERIFIES
        if [ "$TYPE" = "PSK" ]; then
            SUB_VERIFIES="NO"
        fi

        for VERIFY in $SUB_VERIFIES; do
            VERIF=$(echo $VERIFY | tr '[:upper:]' '[:lower:]')
            for MODE in $MODES; do
                print_test_case m O "$O_CIPHERS"
                print_test_case O m "$O_CIPHERS"
                print_test_case m G "$G_CIPHERS"
                print_test_case G m "$G_CIPHERS"
                print_test_case m m "$M_CIPHERS"
            done
        done
    done
}

get_options() {
    while [ $# -gt 0 ]; do
        case "$1" in
            -f|--filter)
                shift; FILTER=$1
                ;;
            -e|--exclude)
                shift; EXCLUDE=$1
                ;;
            -m|--modes)
                shift; MODES=$1
                ;;
            -t|--types)
                shift; TYPES=$1
                ;;
            -V|--verify)
                shift; VERIFIES=$1
                ;;
            -p|--peers)
                shift; PEERS=$1
                ;;
            -v|--verbose)
                VERBOSE=1
                ;;
            -M|--memcheck)
                MEMCHECK=1
                ;;
            # Please check scripts/check_test_cases.py correspondingly
            # if you have to modify option, --list-test-cases
            --list-test-cases)
                list_test_cases
                exit $?
                ;;
            --min)
                shift; MIN_TESTS=$1
                ;;
            --outcome-file)
                shift; MBEDTLS_TEST_OUTCOME_FILE=$1
                ;;
            --preserve-logs)
                PRESERVE_LOGS=1
                ;;
            -h|--help)
                print_usage
                exit 0
                ;;
            *)
                echo "Unknown argument: '$1'"
                print_usage
                exit 1
                ;;
        esac
        shift
    done

    # sanitize some options (modes checked later)
    VERIFIES="$( echo $VERIFIES | tr [a-z] [A-Z] )"
    TYPES="$( echo $TYPES | tr [a-z] [A-Z] )"
}

log() {
  if [ "X" != "X$VERBOSE" ]; then
    echo ""
    echo "$@"
  fi
}

# is_dtls <mode>
is_dtls()
{
    test "$1" = "dtls12"
}

# minor_ver <mode>
minor_ver()
{
    case "$1" in
        tls12|dtls12)
            echo 3
            ;;
        *)
            echo "error: invalid mode: $MODE" >&2
            # exiting is no good here, typically called in a subshell
            echo -1
    esac
}

filter()
{
  LIST="$1"
  NEW_LIST=""

  EXCLMODE="$EXCLUDE"

  for i in $LIST;
  do
    NEW_LIST="$NEW_LIST $( echo "$i" | grep "$FILTER" | grep -v "$EXCLMODE" )"
  done

  # normalize whitespace
  echo "$NEW_LIST" | sed -e 's/[[:space:]][[:space:]]*/ /g' -e 's/^ //' -e 's/ $//'
}

filter_ciphersuites()
{
    if [ "X" != "X$FILTER" -o "X" != "X$EXCLUDE" ];
    then
        # Ciphersuite for Mbed TLS
        M_CIPHERS=$( filter "$M_CIPHERS" )

        # Ciphersuite for OpenSSL
        O_CIPHERS=$( filter "$O_CIPHERS" )

        # Ciphersuite for GnuTLS
        G_CIPHERS=$( filter "$G_CIPHERS" )
    fi
}

reset_ciphersuites()
{
    M_CIPHERS=""
    O_CIPHERS=""
    G_CIPHERS=""
}

# translate_ciphers {g|m|o} {STANDARD_CIPHER_SUITE_NAME...}
# Set $ciphers to the cipher suite name translations for the specified
# program (gnutls, mbedtls or openssl). $ciphers is a space-separated
# list of entries of the form "STANDARD_NAME=PROGRAM_NAME".
translate_ciphers()
{
    ciphers=$(../framework/scripts/translate_ciphers.py "$@")
    if [ $? -ne 0 ]; then
        echo "translate_ciphers.py failed with exit code $1" >&2
        echo "$2" >&2
        exit 1
    fi
}

# Ciphersuites that can be used with all peers.
# Since we currently have three possible peers, each ciphersuite should appear
# three times: in each peer's list (with the name that this peer uses).
add_common_ciphersuites()
{
    CIPHERS=""
    case $TYPE in

        "ECDSA")
            CIPHERS="$CIPHERS                           \
                TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA    \
                TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 \
                TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 \
                TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA    \
                TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 \
                TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 \
                TLS_ECDHE_ECDSA_WITH_NULL_SHA           \
                "
            ;;

        "RSA")
            CIPHERS="$CIPHERS                           \
                TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA      \
                TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256   \
                TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256   \
                TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA      \
                TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384   \
                TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384   \
                TLS_ECDHE_RSA_WITH_NULL_SHA             \
                "
            ;;

        "PSK")
            CIPHERS="$CIPHERS                           \
                TLS_PSK_WITH_AES_128_CBC_SHA            \
                TLS_PSK_WITH_AES_256_CBC_SHA            \
                "
            ;;
    esac

    O_CIPHERS="$O_CIPHERS $CIPHERS"
    G_CIPHERS="$G_CIPHERS $CIPHERS"
    M_CIPHERS="$M_CIPHERS $CIPHERS"
}

# Ciphersuites usable only with Mbed TLS and OpenSSL
# A list of ciphersuites in the standard naming convention is appended
# to the list of Mbed TLS ciphersuites $M_CIPHERS and
# to the list of OpenSSL ciphersuites $O_CIPHERS respectively.
# Based on client's naming convention, all ciphersuite names will be
# translated into another naming format before sent to the client.
#
# ChachaPoly suites are here rather than in "common", as they were added in
# GnuTLS in 3.5.0 and the CI only has 3.4.x so far.
add_openssl_ciphersuites()
{
    CIPHERS=""
    case $TYPE in

        "ECDSA")
            CIPHERS="$CIPHERS                                   \
                TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256        \
                TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384        \
                TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256   \
                "
            ;;

        "RSA")
            CIPHERS="$CIPHERS                                   \
                TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256          \
                TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384          \
                TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256     \
                "
            ;;

        "PSK")
            CIPHERS="$CIPHERS                                   \
                TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256     \
                TLS_PSK_WITH_ARIA_128_GCM_SHA256                \
                TLS_PSK_WITH_ARIA_256_GCM_SHA384                \
                TLS_PSK_WITH_CHACHA20_POLY1305_SHA256           \
                "
            ;;
    esac

    O_CIPHERS="$O_CIPHERS $CIPHERS"
    M_CIPHERS="$M_CIPHERS $CIPHERS"
}

# Ciphersuites usable only with Mbed TLS and GnuTLS
# A list of ciphersuites in the standard naming convention is appended
# to the list of Mbed TLS ciphersuites $M_CIPHERS and
# to the list of GnuTLS ciphersuites $G_CIPHERS respectively.
# Based on client's naming convention, all ciphersuite names will be
# translated into another naming format before sent to the client.
add_gnutls_ciphersuites()
{
    CIPHERS=""
    case $TYPE in

        "ECDSA")
            CIPHERS="$CIPHERS                                       \
                TLS_ECDHE_ECDSA_WITH_AES_128_CCM                    \
                TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8                  \
                TLS_ECDHE_ECDSA_WITH_AES_256_CCM                    \
                TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8                  \
                TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256        \
                TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256        \
                TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384        \
                TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384        \
                "
            ;;

        "RSA")
            CIPHERS="$CIPHERS                               \
                TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256  \
                TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256  \
                TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384  \
                TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384  \
                "
            ;;

        "PSK")
            CIPHERS="$CIPHERS                               \
                TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA          \
                TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256       \
                TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA          \
                TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384       \
                TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256  \
                TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384  \
                TLS_ECDHE_PSK_WITH_NULL_SHA256              \
                TLS_ECDHE_PSK_WITH_NULL_SHA384              \
                TLS_PSK_WITH_AES_128_CBC_SHA256             \
                TLS_PSK_WITH_AES_128_CCM                    \
                TLS_PSK_WITH_AES_128_CCM_8                  \
                TLS_PSK_WITH_AES_128_GCM_SHA256             \
                TLS_PSK_WITH_AES_256_CBC_SHA384             \
                TLS_PSK_WITH_AES_256_CCM                    \
                TLS_PSK_WITH_AES_256_CCM_8                  \
                TLS_PSK_WITH_AES_256_GCM_SHA384             \
                TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256        \
                TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256        \
                TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384        \
                TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384        \
                TLS_PSK_WITH_NULL_SHA256                    \
                TLS_PSK_WITH_NULL_SHA384                    \
                "
            ;;
    esac

    G_CIPHERS="$G_CIPHERS $CIPHERS"
    M_CIPHERS="$M_CIPHERS $CIPHERS"
}

# Ciphersuites usable only with Mbed TLS (not currently supported by another
# peer usable in this script). This provides only very rudimentaty testing, as
# this is not interop testing, but it's better than nothing.
add_mbedtls_ciphersuites()
{
    case $TYPE in

        "ECDSA")
            M_CIPHERS="$M_CIPHERS                               \
                TLS_ECDHE_ECDSA_WITH_ARIA_128_CBC_SHA256        \
                TLS_ECDHE_ECDSA_WITH_ARIA_256_CBC_SHA384        \
                "
            ;;

        "RSA")
            M_CIPHERS="$M_CIPHERS                               \
                TLS_ECDHE_RSA_WITH_ARIA_128_CBC_SHA256          \
                TLS_ECDHE_RSA_WITH_ARIA_256_CBC_SHA384          \
                "
            ;;

        "PSK")
            # *PSK_NULL_SHA suites supported by GnuTLS 3.3.5 but not 3.2.15
            M_CIPHERS="$M_CIPHERS                               \
                TLS_ECDHE_PSK_WITH_ARIA_128_CBC_SHA256          \
                TLS_ECDHE_PSK_WITH_ARIA_256_CBC_SHA384          \
                TLS_ECDHE_PSK_WITH_NULL_SHA                     \
                TLS_PSK_WITH_ARIA_128_CBC_SHA256                \
                TLS_PSK_WITH_ARIA_256_CBC_SHA384                \
                TLS_PSK_WITH_NULL_SHA                           \
                "
            ;;
    esac
}

# o_check_ciphersuite STANDARD_CIPHER_SUITE
o_check_ciphersuite()
{
    # skip DTLS when lack of support was declared
    if test "$OSSL_NO_DTLS" -gt 0 && is_dtls "$MODE"; then
        SKIP_NEXT_="YES"
    fi

    # skip DTLS 1.2 is support was not detected
    if [ "$O_SUPPORT_DTLS12" = "NO" -a "$MODE" = "dtls12" ]; then
        SKIP_NEXT="YES"
    fi

    # skip static ECDH when OpenSSL doesn't support it
    if [ "${O_SUPPORT_STATIC_ECDH}" = "NO" ]; then
        case "$1" in
            *ECDH_*) SKIP_NEXT="YES"
        esac
    fi
}

setup_arguments()
{
    DATA_FILES_PATH="../framework/data_files"

    O_MODE=""
    G_MODE=""
    case "$MODE" in
        "tls12")
            O_MODE="tls1_2"
            G_PRIO_MODE="+VERS-TLS1.2"
            ;;
        "dtls12")
            O_MODE="dtls1_2"
            G_PRIO_MODE="+VERS-DTLS1.2"
            G_MODE="-u"
            ;;
        *)
            echo "error: invalid mode: $MODE" >&2
            exit 1;
    esac

    # GnuTLS < 3.4 will choke if we try to allow CCM-8
    if [ -z "${GNUTLS_MINOR_LT_FOUR-}" ]; then
        G_PRIO_CCM="+AES-256-CCM-8:+AES-128-CCM-8:"
    else
        G_PRIO_CCM=""
    fi

    M_SERVER_ARGS="server_port=$PORT server_addr=0.0.0.0 force_version=$MODE"
    O_SERVER_ARGS="-accept $PORT -cipher ALL,COMPLEMENTOFALL -$O_MODE"
    G_SERVER_ARGS="-p $PORT --http $G_MODE"
    G_SERVER_PRIO="NORMAL:${G_PRIO_CCM}+NULL:+MD5:+PSK:+ECDHE-PSK:+SHA256:+SHA384:-VERS-TLS-ALL:$G_PRIO_MODE"

    # The default prime for `openssl s_server` depends on the version:
    # * OpenSSL <= 1.0.2a: 512-bit
    # * OpenSSL 1.0.2b to 1.1.1b: 1024-bit
    # * OpenSSL >= 1.1.1c: 2048-bit
    # Mbed TLS wants >=1024, so force that for older versions. Don't force
    # it for newer versions, which reject a 1024-bit prime. Indifferently
    # force it or not for intermediate versions.
    case $($OPENSSL version) in
        "OpenSSL 1.0"*)
            O_SERVER_ARGS="$O_SERVER_ARGS -dhparam $DATA_FILES_PATH/dhparams.pem"
            ;;
    esac

    # with OpenSSL 1.0.1h, -www, -WWW and -HTTP break DTLS handshakes
    if is_dtls "$MODE"; then
        O_SERVER_ARGS="$O_SERVER_ARGS"
    else
        O_SERVER_ARGS="$O_SERVER_ARGS -www"
    fi

    M_CLIENT_ARGS="server_port=$PORT server_addr=127.0.0.1 force_version=$MODE"
    O_CLIENT_ARGS="-connect localhost:$PORT -$O_MODE"
    G_CLIENT_ARGS="-p $PORT --debug 3 $G_MODE"

    # Newer versions of OpenSSL have a syntax to enable all "ciphers", even
    # low-security ones. This covers not just cipher suites but also protocol
    # versions. It is necessary, for example, to use (D)TLS 1.0/1.1 on
    # OpenSSL 1.1.1f from Ubuntu 20.04. The syntax was only introduced in
    # OpenSSL 1.1.0 (21e0c1d23afff48601eb93135defddae51f7e2e3) and I can't find
    # a way to discover it from -help, so check the openssl version.
    case $($OPENSSL version) in
        "OpenSSL 0"*|"OpenSSL 1.0"*) :;;
        *)
            O_CLIENT_ARGS="$O_CLIENT_ARGS -cipher ALL@SECLEVEL=0"
            O_SERVER_ARGS="$O_SERVER_ARGS -cipher ALL@SECLEVEL=0"
            ;;
    esac

    case $($OPENSSL ciphers ALL) in
        *ECDH-ECDSA*|*ECDH-RSA*) O_SUPPORT_STATIC_ECDH="YES";;
        *) O_SUPPORT_STATIC_ECDH="NO";;
    esac

    # OpenSSL <1.0.2 doesn't support DTLS 1.2. Check if OpenSSL
    # supports -dtls1_2 from the s_server help. (The s_client
    # help isn't accurate as of 1.0.2g: it supports DTLS 1.2
    # but doesn't list it. But the s_server help seems to be
    # accurate.)
    O_SUPPORT_DTLS12="NO"
    if $OPENSSL s_server -help 2>&1 | grep -q "^ *-dtls1_2 "; then
        O_SUPPORT_DTLS12="YES"
    fi

    if [ "X$VERIFY" = "XYES" ];
    then
        M_SERVER_ARGS="$M_SERVER_ARGS ca_file=$DATA_FILES_PATH/test-ca_cat12.crt auth_mode=required"
        O_SERVER_ARGS="$O_SERVER_ARGS -CAfile $DATA_FILES_PATH/test-ca_cat12.crt -Verify 10"
        G_SERVER_ARGS="$G_SERVER_ARGS --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt --require-client-cert"

        M_CLIENT_ARGS="$M_CLIENT_ARGS ca_file=$DATA_FILES_PATH/test-ca_cat12.crt auth_mode=required"
        O_CLIENT_ARGS="$O_CLIENT_ARGS -CAfile $DATA_FILES_PATH/test-ca_cat12.crt -verify 10"
        G_CLIENT_ARGS="$G_CLIENT_ARGS --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt"
    else
        # don't request a client cert at all
        M_SERVER_ARGS="$M_SERVER_ARGS ca_file=none auth_mode=none"
        G_SERVER_ARGS="$G_SERVER_ARGS --disable-client-cert"

        M_CLIENT_ARGS="$M_CLIENT_ARGS ca_file=none auth_mode=none"
        O_CLIENT_ARGS="$O_CLIENT_ARGS"
        G_CLIENT_ARGS="$G_CLIENT_ARGS --insecure"
    fi

    case $TYPE in
        "ECDSA")
            M_SERVER_ARGS="$M_SERVER_ARGS crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key"
            O_SERVER_ARGS="$O_SERVER_ARGS -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key"
            G_SERVER_ARGS="$G_SERVER_ARGS --x509certfile $DATA_FILES_PATH/server5.crt --x509keyfile $DATA_FILES_PATH/server5.key"

            if [ "X$VERIFY" = "XYES" ]; then
                M_CLIENT_ARGS="$M_CLIENT_ARGS crt_file=$DATA_FILES_PATH/server6.crt key_file=$DATA_FILES_PATH/server6.key"
                O_CLIENT_ARGS="$O_CLIENT_ARGS -cert $DATA_FILES_PATH/server6.crt -key $DATA_FILES_PATH/server6.key"
                G_CLIENT_ARGS="$G_CLIENT_ARGS --x509certfile $DATA_FILES_PATH/server6.crt --x509keyfile $DATA_FILES_PATH/server6.key"
            else
                M_CLIENT_ARGS="$M_CLIENT_ARGS crt_file=none key_file=none"
            fi
            ;;

        "RSA")
            M_SERVER_ARGS="$M_SERVER_ARGS crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key"
            O_SERVER_ARGS="$O_SERVER_ARGS -cert $DATA_FILES_PATH/server2-sha256.crt -key $DATA_FILES_PATH/server2.key"
            G_SERVER_ARGS="$G_SERVER_ARGS --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key"

            if [ "X$VERIFY" = "XYES" ]; then
                M_CLIENT_ARGS="$M_CLIENT_ARGS crt_file=$DATA_FILES_PATH/cert_sha256.crt key_file=$DATA_FILES_PATH/server1.key"
                O_CLIENT_ARGS="$O_CLIENT_ARGS -cert $DATA_FILES_PATH/cert_sha256.crt -key $DATA_FILES_PATH/server1.key"
                G_CLIENT_ARGS="$G_CLIENT_ARGS --x509certfile $DATA_FILES_PATH/cert_sha256.crt --x509keyfile $DATA_FILES_PATH/server1.key"
            else
                M_CLIENT_ARGS="$M_CLIENT_ARGS crt_file=none key_file=none"
            fi
            ;;

        "PSK")
            M_SERVER_ARGS="$M_SERVER_ARGS psk=6162636465666768696a6b6c6d6e6f70 ca_file=none"
            O_SERVER_ARGS="$O_SERVER_ARGS -psk 6162636465666768696a6b6c6d6e6f70 -nocert"
            G_SERVER_ARGS="$G_SERVER_ARGS --pskpasswd $DATA_FILES_PATH/passwd.psk"

            M_CLIENT_ARGS="$M_CLIENT_ARGS psk=6162636465666768696a6b6c6d6e6f70 crt_file=none key_file=none"
            O_CLIENT_ARGS="$O_CLIENT_ARGS -psk 6162636465666768696a6b6c6d6e6f70"
            G_CLIENT_ARGS="$G_CLIENT_ARGS --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70"
            ;;
    esac
}

# is_mbedtls <cmd_line>
is_mbedtls() {
    case $1 in
        *ssl_client2*) true;;
        *ssl_server2*) true;;
        *) false;;
    esac
}

# has_mem_err <log_file_name>
has_mem_err() {
    if ( grep -F 'All heap blocks were freed -- no leaks are possible' "$1" &&
         grep -F 'ERROR SUMMARY: 0 errors from 0 contexts' "$1" ) > /dev/null
    then
        return 1 # false: does not have errors
    else
        return 0 # true: has errors
    fi
}

# Wait for process $2 to be listening on port $1
if type lsof >/dev/null 2>/dev/null; then
    wait_server_start() {
        START_TIME=$(date +%s)
        if is_dtls "$MODE"; then
            proto=UDP
        else
            proto=TCP
        fi
        while ! lsof -a -n -b -i "$proto:$1" -p "$2" >/dev/null 2>/dev/null; do
              if [ $(( $(date +%s) - $START_TIME )) -gt $DOG_DELAY ]; then
                  echo "SERVERSTART TIMEOUT"
                  echo "SERVERSTART TIMEOUT" >> $SRV_OUT
                  break
              fi
              # Linux and *BSD support decimal arguments to sleep. On other
              # OSes this may be a tight loop.
              sleep 0.1 2>/dev/null || true
        done
    }
else
    echo "Warning: lsof not available, wait_server_start = sleep"
    wait_server_start() {
        sleep 2
    }
fi


# start_server <name>
# also saves name and command
start_server() {
    case $1 in
        [Oo]pen*)
            SERVER_CMD="$OPENSSL s_server $O_SERVER_ARGS"
            ;;
        [Gg]nu*)
            SERVER_CMD="$GNUTLS_SERV $G_SERVER_ARGS --priority $G_SERVER_PRIO"
            ;;
        mbed*)
            SERVER_CMD="$M_SRV $M_SERVER_ARGS"
            if [ "$MEMCHECK" -gt 0 ]; then
                SERVER_CMD="valgrind --leak-check=full $SERVER_CMD"
            fi
            ;;
        *)
            echo "error: invalid server name: $1" >&2
            exit 1
            ;;
    esac
    SERVER_NAME=$1

    log "$SERVER_CMD"
    echo "$SERVER_CMD" > $SRV_OUT
    # for servers without -www or equivalent
    while :; do echo bla; sleep 1; done | $SERVER_CMD >> $SRV_OUT 2>&1 &
    SRV_PID=$!

    wait_server_start "$PORT" "$SRV_PID"
}

# terminate the running server
stop_server() {
    # For Ubuntu 22.04, `Terminated` message is outputed by wait command.
    # To remove it from stdout, redirect stdout/stderr to SRV_OUT
    kill $SRV_PID >/dev/null 2>&1
    wait $SRV_PID >> $SRV_OUT 2>&1

    if [ "$MEMCHECK" -gt 0 ]; then
        if is_mbedtls "$SERVER_CMD" && has_mem_err $SRV_OUT; then
            echo "  ! Server had memory errors"
            SRVMEM=$(( $SRVMEM + 1 ))
            return
        fi
    fi

    rm -f $SRV_OUT
}

# kill the running server (used when killed by signal)
cleanup() {
    rm -f $SRV_OUT $CLI_OUT
    kill $SRV_PID >/dev/null 2>&1
    kill $WATCHDOG_PID >/dev/null 2>&1
    exit 1
}

# wait for client to terminate and set EXIT
# must be called right after starting the client
wait_client_done() {
    CLI_PID=$!

    ( sleep "$DOG_DELAY"; echo "TIMEOUT" >> $CLI_OUT; kill $CLI_PID ) &
    WATCHDOG_PID=$!

    # For Ubuntu 22.04, `Terminated` message is outputed by wait command.
    # To remove it from stdout, redirect stdout/stderr to CLI_OUT
    wait $CLI_PID >> $CLI_OUT 2>&1
    EXIT=$?

    kill $WATCHDOG_PID >/dev/null 2>&1
    wait $WATCHDOG_PID >> $CLI_OUT 2>&1

    echo "EXIT: $EXIT" >> $CLI_OUT
}

# uniform_title <CLIENT> <SERVER> <STANDARD_CIPHER_SUITE>
# $TITLE is considered as test case description for both --list-test-cases and
# MBEDTLS_TEST_OUTCOME_FILE. This function aims to control the format of
# each test case description.
uniform_title() {
    TITLE="$1->$2 $MODE,$VERIF $3"
}

# record_outcome <outcome> [<failure-reason>]
record_outcome() {
    echo "$1"
    if [ -n "$MBEDTLS_TEST_OUTCOME_FILE" ]; then
        # The test outcome file has the format (in single line):
        # platform;configuration;
        # test suite name;test case description;
        # PASS/FAIL/SKIP;[failure cause]
        printf '%s;%s;%s;%s;%s;%s\n'                                    \
            "$MBEDTLS_TEST_PLATFORM" "$MBEDTLS_TEST_CONFIGURATION"      \
            "compat" "$TITLE"                                           \
            "$1" "${2-}"                                                \
            >> "$MBEDTLS_TEST_OUTCOME_FILE"
    fi
}

save_logs() {
    cp $SRV_OUT c-srv-${TESTS}.log
    cp $CLI_OUT c-cli-${TESTS}.log
}

# display additional information if test case fails
report_fail() {
    FAIL_PROMPT="outputs saved to c-srv-${TESTS}.log, c-cli-${TESTS}.log"
    record_outcome "FAIL" "$FAIL_PROMPT"
    save_logs
    echo "  ! $FAIL_PROMPT"

    if [ "${LOG_FAILURE_ON_STDOUT:-0}" != 0 ]; then
        echo "  ! server output:"
        cat c-srv-${TESTS}.log
        echo "  ! ==================================================="
        echo "  ! client output:"
        cat c-cli-${TESTS}.log
    fi
}

# run_client PROGRAM_NAME STANDARD_CIPHER_SUITE PROGRAM_CIPHER_SUITE
run_client() {
    # announce what we're going to do
    TESTS=$(( $TESTS + 1 ))
    uniform_title "${1%"${1#?}"}" "${SERVER_NAME%"${SERVER_NAME#?}"}" $2
    DOTS72="........................................................................"
    printf "%s %.*s " "$TITLE" "$((71 - ${#TITLE}))" "$DOTS72"

    # should we skip?
    if [ "X$SKIP_NEXT" = "XYES" ]; then
        SKIP_NEXT="NO"
        record_outcome "SKIP"
        SKIPPED=$(( $SKIPPED + 1 ))
        return
    fi

    # run the command and interpret result
    case $1 in
        [Oo]pen*)
            CLIENT_CMD="$OPENSSL s_client $O_CLIENT_ARGS -cipher $3"
            log "$CLIENT_CMD"
            echo "$CLIENT_CMD" > $CLI_OUT
            printf 'GET HTTP/1.0\r\n\r\n' | $CLIENT_CMD >> $CLI_OUT 2>&1 &
            wait_client_done

            if [ $EXIT -eq 0 ]; then
                RESULT=0
            else
                # If it is NULL cipher ...
                if grep 'Cipher is (NONE)' $CLI_OUT >/dev/null; then
                    RESULT=1
                else
                    RESULT=2
                fi
            fi
            ;;

        [Gg]nu*)
            CLIENT_CMD="$GNUTLS_CLI $G_CLIENT_ARGS --priority $G_PRIO_MODE:$3 localhost"
            log "$CLIENT_CMD"
            echo "$CLIENT_CMD" > $CLI_OUT
            printf 'GET HTTP/1.0\r\n\r\n' | $CLIENT_CMD >> $CLI_OUT 2>&1 &
            wait_client_done

            if [ $EXIT -eq 0 ]; then
                RESULT=0
            else
                RESULT=2
                # interpret early failure, with a handshake_failure alert
                # before the server hello, as "no ciphersuite in common"
                if grep -F 'Received alert [40]: Handshake failed' $CLI_OUT; then
                    if grep -i 'SERVER HELLO .* was received' $CLI_OUT; then :
                    else
                        RESULT=1
                    fi
                fi >/dev/null
            fi
            ;;

        mbed*)
            CLIENT_CMD="$M_CLI $M_CLIENT_ARGS force_ciphersuite=$3"
            if [ "$MEMCHECK" -gt 0 ]; then
                CLIENT_CMD="valgrind --leak-check=full $CLIENT_CMD"
            fi
            log "$CLIENT_CMD"
            echo "$CLIENT_CMD" > $CLI_OUT
            $CLIENT_CMD >> $CLI_OUT 2>&1 &
            wait_client_done

            case $EXIT in
                # Success
                "0")    RESULT=0    ;;

                # Ciphersuite not supported
                "2")    RESULT=1    ;;

                # Error
                *)      RESULT=2    ;;
            esac

            if [ "$MEMCHECK" -gt 0 ]; then
                if is_mbedtls "$CLIENT_CMD" && has_mem_err $CLI_OUT; then
                    RESULT=2
                fi
            fi

            ;;

        *)
            echo "error: invalid client name: $1" >&2
            exit 1
            ;;
    esac

    echo "EXIT: $EXIT" >> $CLI_OUT

    # report and count result
    case $RESULT in
        "0")
            record_outcome "PASS"
            if [ "$PRESERVE_LOGS" -gt 0 ]; then
                save_logs
            fi
            ;;
        "1")
            record_outcome "SKIP"
            SKIPPED=$(( $SKIPPED + 1 ))
            ;;
        "2")
            report_fail
            FAILED=$(( $FAILED + 1 ))
            ;;
    esac

    rm -f $CLI_OUT
}

#
# MAIN
#

get_options "$@"

# Make the outcome file path relative to the original directory, not
# to .../tests
case "$MBEDTLS_TEST_OUTCOME_FILE" in
    [!/]*)
        MBEDTLS_TEST_OUTCOME_FILE="$ORIGINAL_PWD/$MBEDTLS_TEST_OUTCOME_FILE"
        ;;
esac

# sanity checks, avoid an avalanche of errors
if [ ! -x "$M_SRV" ]; then
    echo "Command '$M_SRV' is not an executable file" >&2
    exit 1
fi
if [ ! -x "$M_CLI" ]; then
    echo "Command '$M_CLI' is not an executable file" >&2
    exit 1
fi

if echo "$PEERS" | grep -i openssl > /dev/null; then
    if which "$OPENSSL" >/dev/null 2>&1; then :; else
        echo "Command '$OPENSSL' not found" >&2
        exit 1
    fi
fi

if echo "$PEERS" | grep -i gnutls > /dev/null; then
    for CMD in "$GNUTLS_CLI" "$GNUTLS_SERV"; do
        if which "$CMD" >/dev/null 2>&1; then :; else
            echo "Command '$CMD' not found" >&2
            exit 1
        fi
    done
fi

for PEER in $PEERS; do
    case "$PEER" in
        mbed*|[Oo]pen*|[Gg]nu*)
            ;;
        *)
            echo "Unknown peers: $PEER" >&2
            exit 1
    esac
done

# Pick a "unique" port in the range 10000-19999.
PORT="0000$$"
PORT="1$(echo $PORT | tail -c 5)"

# Also pick a unique name for intermediate files
SRV_OUT="srv_out.$$"
CLI_OUT="cli_out.$$"

# client timeout delay: be more patient with valgrind
if [ "$MEMCHECK" -gt 0 ]; then
    DOG_DELAY=30
else
    DOG_DELAY=10
fi

SKIP_NEXT="NO"

trap cleanup INT TERM HUP

for MODE in $MODES; do
    for TYPE in $TYPES; do

        # PSK cipher suites do not allow client certificate verification.
        # This means PSK test cases with VERIFY=YES should be replaced by
        # VERIFY=NO or be ignored. SUB_VERIFIES variable is used to constrain
        # verification option for PSK test cases.
        SUB_VERIFIES=$VERIFIES
        if [ "$TYPE" = "PSK" ]; then
            SUB_VERIFIES="NO"
        fi

        for VERIFY in $SUB_VERIFIES; do
            VERIF=$(echo $VERIFY | tr '[:upper:]' '[:lower:]')
            for PEER in $PEERS; do

            setup_arguments

            case "$PEER" in

                [Oo]pen*)

                    reset_ciphersuites
                    add_common_ciphersuites
                    add_openssl_ciphersuites
                    filter_ciphersuites

                    if [ "X" != "X$M_CIPHERS" ]; then
                        start_server "OpenSSL"
                        translate_ciphers m $M_CIPHERS
                        for i in $ciphers; do
                            o_check_ciphersuite "${i%%=*}"
                            run_client mbedTLS ${i%%=*} ${i#*=}
                        done
                        stop_server
                    fi

                    if [ "X" != "X$O_CIPHERS" ]; then
                        start_server "mbedTLS"
                        translate_ciphers o $O_CIPHERS
                        for i in $ciphers; do
                            o_check_ciphersuite "${i%%=*}"
                            run_client OpenSSL ${i%%=*} ${i#*=}
                        done
                        stop_server
                    fi

                    ;;

                [Gg]nu*)

                    reset_ciphersuites
                    add_common_ciphersuites
                    add_gnutls_ciphersuites
                    filter_ciphersuites

                    if [ "X" != "X$M_CIPHERS" ]; then
                        start_server "GnuTLS"
                        translate_ciphers m $M_CIPHERS
                        for i in $ciphers; do
                            run_client mbedTLS ${i%%=*} ${i#*=}
                        done
                        stop_server
                    fi

                    if [ "X" != "X$G_CIPHERS" ]; then
                        start_server "mbedTLS"
                        translate_ciphers g $G_CIPHERS
                        for i in $ciphers; do
                            run_client GnuTLS ${i%%=*} ${i#*=}
                        done
                        stop_server
                    fi

                    ;;

                mbed*)

                    reset_ciphersuites
                    add_common_ciphersuites
                    add_openssl_ciphersuites
                    add_gnutls_ciphersuites
                    add_mbedtls_ciphersuites
                    filter_ciphersuites

                    if [ "X" != "X$M_CIPHERS" ]; then
                        start_server "mbedTLS"
                        translate_ciphers m $M_CIPHERS
                        for i in $ciphers; do
                            run_client mbedTLS ${i%%=*} ${i#*=}
                        done
                        stop_server
                    fi

                    ;;

                *)
                    echo "Unknown peer: $PEER" >&2
                    exit 1
                    ;;

                esac

            done
        done
    done
done

echo "------------------------------------------------------------------------"

if [ $FAILED -ne 0 -o $SRVMEM -ne 0 ]; then
    printf "FAILED"
else
    printf "PASSED"
fi

if [ "$MEMCHECK" -gt 0 ]; then
    MEMREPORT=", $SRVMEM server memory errors"
else
    MEMREPORT=""
fi

PASSED=$(( $TESTS - $FAILED ))
echo " ($PASSED / $TESTS tests ($SKIPPED skipped$MEMREPORT))"

if [ $((TESTS - SKIPPED)) -lt $MIN_TESTS ]; then
    cat <<EOF
Error: Expected to run at least $MIN_TESTS, but only ran $((TESTS - SKIPPED)).
Maybe a bad filter ('$FILTER' excluding '$EXCLUDE') or a bad configuration?
EOF
    if [ $FAILED -eq 0 ]; then
        FAILED=1
    fi
fi

FAILED=$(( $FAILED + $SRVMEM ))
if [ $FAILED -gt 255 ]; then
    # Clamp at 255 as caller gets exit code & 0xFF
    # (so 256 would be 0, or success, etc)
    FAILED=255
fi
exit $FAILED
