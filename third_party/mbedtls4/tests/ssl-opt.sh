#!/bin/sh

# ssl-opt.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# Purpose
#
# Executes tests to prove various TLS/SSL options and extensions.
#
# The goal is not to cover every ciphersuite/version, but instead to cover
# specific options (max fragment length, truncated hmac, etc) or procedures
# (session resumption from cache or ticket, renego, etc).
#
# The tests assume a build with default options, with exceptions expressed
# with a dependency.  The tests focus on functionality and do not consider
# performance.
#

set -u

# Limit the size of each log to 10 GiB, in case of failures with this script
# where it may output seemingly unlimited length error logs.
ulimit -f 20971520

ORIGINAL_PWD=$PWD
if ! cd "$(dirname "$0")"; then
    exit 125
fi

DATA_FILES_PATH=../framework/data_files

# default values, can be overridden by the environment
: ${P_SRV:=../programs/ssl/ssl_server2}
: ${P_CLI:=../programs/ssl/ssl_client2}
: ${P_PXY:=../programs/test/udp_proxy}
: ${P_QUERY:=../programs/test/query_compile_time_config}
: ${OPENSSL:=openssl}
: ${GNUTLS_CLI:=gnutls-cli}
: ${GNUTLS_SERV:=gnutls-serv}
: ${PERL:=perl}

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
: ${EARLY_DATA_INPUT:="$DATA_FILES_PATH/tls13_early_data.txt"}

O_SRV="$OPENSSL s_server -www -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key"
O_CLI="echo 'GET / HTTP/1.0' | $OPENSSL s_client"
G_SRV="$GNUTLS_SERV --x509certfile $DATA_FILES_PATH/server5.crt --x509keyfile $DATA_FILES_PATH/server5.key"
G_CLI="echo 'GET / HTTP/1.0' | $GNUTLS_CLI --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt"

# alternative versions of OpenSSL and GnuTLS (no default path)

# If $OPENSSL is at least 1.1.1, use it as OPENSSL_NEXT as well.
if [ -z "${OPENSSL_NEXT:-}" ]; then
    case $($OPENSSL version) in
        OpenSSL\ 1.1.[1-9]*) OPENSSL_NEXT=$OPENSSL;;
        OpenSSL\ [3-9]*) OPENSSL_NEXT=$OPENSSL;;
    esac
fi

# If $GNUTLS_CLI is at least 3.7, use it as GNUTLS_NEXT_CLI as well.
if [ -z "${GNUTLS_NEXT_CLI:-}" ]; then
    case $($GNUTLS_CLI --version) in
        gnutls-cli\ 3.[1-9][0-9]*) GNUTLS_NEXT_CLI=$GNUTLS_CLI;;
        gnutls-cli\ 3.[7-9].*) GNUTLS_NEXT_CLI=$GNUTLS_CLI;;
        gnutls-cli\ [4-9]*) GNUTLS_NEXT_CLI=$GNUTLS_CLI;;
    esac
fi

# If $GNUTLS_SERV is at least 3.7, use it as GNUTLS_NEXT_SERV as well.
if [ -z "${GNUTLS_NEXT_SERV:-}" ]; then
    case $($GNUTLS_SERV --version) in
        gnutls-cli\ 3.[1-9][0-9]*) GNUTLS_NEXT_SERV=$GNUTLS_SERV;;
        gnutls-cli\ 3.[7-9].*) GNUTLS_NEXT_SERV=$GNUTLS_SERV;;
        gnutls-cli\ [4-9]*) GNUTLS_NEXT_SERV=$GNUTLS_SERV;;
    esac
fi

if [ -n "${OPENSSL_NEXT:-}" ]; then
    O_NEXT_SRV="$OPENSSL_NEXT s_server -www -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key"
    O_NEXT_SRV_EARLY_DATA="$OPENSSL_NEXT s_server -early_data -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key"
    O_NEXT_SRV_NO_CERT="$OPENSSL_NEXT s_server -www "
    O_NEXT_CLI="echo 'GET / HTTP/1.0' | $OPENSSL_NEXT s_client -CAfile $DATA_FILES_PATH/test-ca_cat12.crt"
    O_NEXT_CLI_NO_CERT="echo 'GET / HTTP/1.0' | $OPENSSL_NEXT s_client"
    O_NEXT_CLI_RENEGOTIATE="echo 'R' | $OPENSSL_NEXT s_client -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key"
else
    O_NEXT_SRV=false
    O_NEXT_SRV_NO_CERT=false
    O_NEXT_SRV_EARLY_DATA=false
    O_NEXT_CLI_NO_CERT=false
    O_NEXT_CLI=false
    O_NEXT_CLI_RENEGOTIATE=false
fi

if [ -n "${GNUTLS_NEXT_SERV:-}" ]; then
    G_NEXT_SRV="$GNUTLS_NEXT_SERV --x509certfile $DATA_FILES_PATH/server5.crt --x509keyfile $DATA_FILES_PATH/server5.key"
    G_NEXT_SRV_NO_CERT="$GNUTLS_NEXT_SERV"
else
    G_NEXT_SRV=false
    G_NEXT_SRV_NO_CERT=false
fi

if [ -n "${GNUTLS_NEXT_CLI:-}" ]; then
    G_NEXT_CLI="echo 'GET / HTTP/1.0' | $GNUTLS_NEXT_CLI --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt"
    G_NEXT_CLI_NO_CERT="echo 'GET / HTTP/1.0' | $GNUTLS_NEXT_CLI"
else
    G_NEXT_CLI=false
    G_NEXT_CLI_NO_CERT=false
fi

TESTS=0
FAILS=0
SKIPS=0

CONFIG_H='../include/mbedtls/mbedtls_config.h'

MEMCHECK=0
FILTER='.*'
EXCLUDE='^$'

SHOW_TEST_NUMBER=0
LIST_TESTS=0
RUN_TEST_NUMBER=''
RUN_TEST_SUITE=''

MIN_TESTS=1
PRESERVE_LOGS=0

# Pick a "unique" server port in the range 10000-19999, and a proxy
# port which is this plus 10000. Each port number may be independently
# overridden by a command line option.
SRV_PORT=$(($$ % 10000 + 10000))
PXY_PORT=$((SRV_PORT + 10000))

print_usage() {
    echo "Usage: $0 [options]"
    printf "  -h|--help\tPrint this help.\n"
    printf "  -m|--memcheck\tCheck memory leaks and errors.\n"
    printf "  -f|--filter\tOnly matching tests are executed (substring or BRE)\n"
    printf "  -e|--exclude\tMatching tests are excluded (substring or BRE)\n"
    printf "  -n|--number\tExecute only numbered test (comma-separated, e.g. '245,256')\n"
    printf "  -s|--show-numbers\tShow test numbers in front of test names\n"
    printf "  -p|--preserve-logs\tPreserve logs of successful tests as well\n"
    printf "     --list-test-cases\tList all potential test cases (No Execution)\n"
    printf "     --min      \tMinimum number of non-skipped tests (default 1)\n"
    printf "     --outcome-file\tFile where test outcomes are written\n"
    printf "                \t(default: \$MBEDTLS_TEST_OUTCOME_FILE, none if empty)\n"
    printf "     --port     \tTCP/UDP port (default: randomish 1xxxx)\n"
    printf "     --proxy-port\tTCP/UDP proxy port (default: randomish 2xxxx)\n"
    printf "     --seed     \tInteger seed value to use for this test run\n"
    printf "     --test-suite\tOnly matching test suites are executed\n"
    printf "                 \t(comma-separated, e.g. 'ssl-opt,tls13-compat')\n\n"
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
            -m|--memcheck)
                MEMCHECK=1
                ;;
            -n|--number)
                shift; RUN_TEST_NUMBER=$1
                ;;
            -s|--show-numbers)
                SHOW_TEST_NUMBER=1
                ;;
            -l|--list-test-cases)
                LIST_TESTS=1
                ;;
            -p|--preserve-logs)
                PRESERVE_LOGS=1
                ;;
            --min)
                shift; MIN_TESTS=$1
                ;;
            --outcome-file)
                shift; MBEDTLS_TEST_OUTCOME_FILE=$1
                ;;
            --port)
                shift; SRV_PORT=$1
                ;;
            --proxy-port)
                shift; PXY_PORT=$1
                ;;
            --seed)
                shift; SEED="$1"
                ;;
            --test-suite)
                shift; RUN_TEST_SUITE="$1"
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
}

get_options "$@"

# Read boolean configuration options from mbedtls_config.h for easy and quick
# testing. Skip non-boolean options (with something other than spaces
# and a comment after "#define SYMBOL"). The variable contains a
# space-separated list of symbols. The list should always be
# terminated by a single whitespace character, otherwise the last entry
# will not get matched by the parsing regex.
if [ "$LIST_TESTS" -eq 0 ];then
    CONFIGS_ENABLED=" $(echo `$P_QUERY -l` ) "
else
    P_QUERY=":"
    CONFIGS_ENABLED=""
fi
# Skip next test; use this macro to skip tests which are legitimate
# in theory and expected to be re-introduced at some point, but
# aren't expected to succeed at the moment due to problems outside
# our control (such as bugs in other TLS implementations).
skip_next_test() {
    SKIP_NEXT="YES"
}

# Check if the required configuration ($1) is enabled
is_config_enabled()
{
    case $CONFIGS_ENABLED in
        *" $1"[\ =]*) return 0;;
        *) return 1;;
    esac
}

# skip next test if the flag is not enabled in mbedtls_config.h
requires_config_enabled() {
    case $CONFIGS_ENABLED in
        *" $1"[\ =]*) :;;
        *) SKIP_NEXT="YES";;
    esac
}

# skip next test if the flag is enabled in mbedtls_config.h
requires_config_disabled() {
    case $CONFIGS_ENABLED in
        *" $1"[\ =]*) SKIP_NEXT="YES";;
    esac
}

requires_all_configs_enabled() {
    for x in "$@"; do
        if ! is_config_enabled "$x"; then
            SKIP_NEXT="YES"
            return
        fi
    done
}

requires_all_configs_disabled() {
    for x in "$@"; do
        if is_config_enabled "$x"; then
            SKIP_NEXT="YES"
            return
        fi
    done
}

requires_any_configs_enabled() {
    for x in "$@"; do
        if is_config_enabled "$x"; then
            return
        fi
    done
    SKIP_NEXT="YES"
}

requires_any_configs_disabled() {
    for x in "$@"; do
        if ! is_config_enabled "$x"; then
            return
        fi
    done
    SKIP_NEXT="YES"
}

TLS1_2_KEY_EXCHANGES_WITH_CERT="MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED \
                                MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED"

TLS1_2_KEY_EXCHANGES_WITH_CERT_WO_ECDH="MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED \
                                        MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED"

requires_certificate_authentication () {
    if is_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
    then
        # TLS 1.3 is negotiated by default, so check whether it supports
        # certificate-based authentication.
        requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
    else # Only TLS 1.2 is enabled.
        requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
    fi
}

get_config_value_or_default() {
    # This function uses the query_config command line option to query the
    # required Mbed TLS compile time configuration from the ssl_server2
    # program. The command will always return a success value if the
    # configuration is defined and the value will be printed to stdout.
    #
    # Note that if the configuration is not defined or is defined to nothing,
    # the output of this function will be an empty string.
    if [ "$LIST_TESTS" -eq 0 ];then
        ${P_SRV} "query_config=${1}"
    else
        echo "1"
    fi

}

requires_config_value_at_least() {
    VAL="$( get_config_value_or_default "$1" )"
    if [ -z "$VAL" ]; then
        # Should never happen
        echo "Mbed TLS configuration $1 is not defined"
        exit 1
    elif [ "$VAL" -lt "$2" ]; then
       SKIP_NEXT="YES"
    fi
}

requires_config_value_at_most() {
    VAL=$( get_config_value_or_default "$1" )
    if [ -z "$VAL" ]; then
        # Should never happen
        echo "Mbed TLS configuration $1 is not defined"
        exit 1
    elif [ "$VAL" -gt "$2" ]; then
       SKIP_NEXT="YES"
    fi
}

requires_config_value_equals() {
    VAL=$( get_config_value_or_default "$1" )
    if [ -z "$VAL" ]; then
        # Should never happen
        echo "Mbed TLS configuration $1 is not defined"
        exit 1
    elif [ "$VAL" -ne "$2" ]; then
       SKIP_NEXT="YES"
    fi
}

# Require Mbed TLS to support the given protocol version.
#
# Inputs:
# * $1: protocol version in mbedtls syntax (argument to force_version=)
requires_protocol_version() {
    # Support for DTLS is detected separately in detect_dtls().
    case "$1" in
        tls12|dtls12) requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2;;
        tls13|dtls13) requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3;;
        *) echo "Unknown required protocol version: $1"; exit 1;;
    esac
}

# Space-separated list of ciphersuites supported by this build of
# Mbed TLS.
P_CIPHERSUITES=""
if [ "$LIST_TESTS" -eq 0 ]; then
    P_CIPHERSUITES=" $($P_CLI help_ciphersuites 2>/dev/null |
                       grep 'TLS-\|TLS1-3' |
                       tr -s ' \n' ' ')"

    if [ -z "${P_CIPHERSUITES# }" ]; then
        echo >&2 "$0: fatal error: no cipher suites found!"
        exit 125
    fi
fi

requires_ciphersuite_enabled() {
    case $P_CIPHERSUITES in
        *" $1 "*) :;;
        *) SKIP_NEXT="YES";;
    esac
}

requires_cipher_enabled() {
    KEY_TYPE=$1
    MODE=${2:-}
    case "$KEY_TYPE" in
        CHACHA20)
            requires_config_enabled PSA_WANT_ALG_CHACHA20_POLY1305
            requires_config_enabled PSA_WANT_KEY_TYPE_CHACHA20
            ;;
        *)
            requires_config_enabled PSA_WANT_ALG_${MODE}
            requires_config_enabled PSA_WANT_KEY_TYPE_${KEY_TYPE}
            ;;
    esac
}

# Automatically detect required features based on command line parameters.
# Parameters are:
# - $1 = command line (call to a TLS client or server program)
# - $2 = client/server
# - $3 = TLS version (TLS12 or TLS13)
# - $4 = run test options
detect_required_features() {
    CMD_LINE=$1
    ROLE=$2
    TLS_VERSION=$3
    TEST_OPTIONS=${4:-}

    case "$CMD_LINE" in
        *\ force_version=*)
            tmp="${CMD_LINE##*\ force_version=}"
            tmp="${tmp%%[!-0-9A-Z_a-z]*}"
            requires_protocol_version "$tmp";;
    esac

    case "$CMD_LINE" in
        *\ force_ciphersuite=*)
            tmp="${CMD_LINE##*\ force_ciphersuite=}"
            tmp="${tmp%%[!-0-9A-Z_a-z]*}"
            requires_ciphersuite_enabled "$tmp";;
    esac

    case " $CMD_LINE " in
        *[-_\ =]tickets=[^0]*)
            requires_config_enabled MBEDTLS_SSL_TICKET_C;;
    esac
    case " $CMD_LINE " in
        *[-_\ =]alpn=*)
            requires_config_enabled MBEDTLS_SSL_ALPN;;
    esac

    case " $CMD_LINE " in
         *\ auth_mode=*|*[-_\ =]crt[_=]*)
            # The test case involves certificates (crt), or a relevant
            # aspect of it is the (certificate-based) authentication mode.
            requires_certificate_authentication;;
    esac

    case " $CMD_LINE " in
        *\ ca_callback=1\ *)
            requires_config_enabled MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK;;
    esac

    case " $CMD_LINE " in
        *"programs/ssl/dtls_client "*|\
        *"programs/ssl/ssl_client1 "*)
            requires_config_enabled MBEDTLS_CTR_DRBG_C
            requires_config_enabled MBEDTLS_PSA_CRYPTO_C
            requires_config_disabled MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
            requires_config_enabled MBEDTLS_PEM_PARSE_C
            requires_config_enabled MBEDTLS_SSL_CLI_C
            requires_certificate_authentication
            ;;
        *"programs/ssl/dtls_server "*|\
        *"programs/ssl/ssl_fork_server "*|\
        *"programs/ssl/ssl_pthread_server "*|\
        *"programs/ssl/ssl_server "*)
            requires_config_enabled MBEDTLS_CTR_DRBG_C
            requires_config_enabled MBEDTLS_PSA_CRYPTO_C
            requires_config_disabled MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
            requires_config_enabled MBEDTLS_PEM_PARSE_C
            requires_config_enabled MBEDTLS_SSL_SRV_C
            requires_certificate_authentication
            # The actual minimum depends on the configuration since it's
            # mostly about the certificate size.
            # In config-suite-b.h, for the test certificates (server5.crt),
            # 1024 is not enough.
            requires_config_value_at_least MBEDTLS_SSL_OUT_CONTENT_LEN 2000
            ;;
    esac

    case " $CMD_LINE " in
        *"programs/ssl/ssl_pthread_server "*)
            requires_config_enabled MBEDTLS_THREADING_PTHREAD;;
    esac

    case "$CMD_LINE" in
        *[-_\ =]psk*|*[-_\ =]PSK*) :;; # No certificate requirement with PSK
        */server5*|\
        */server7*|\
        */dir-maxpath*)
            requires_certificate_authentication
            if [ "$TLS_VERSION" = "TLS13" ]; then
                # In case of TLS13 the support for ECDSA is enough
                requires_pk_alg "ECDSA"
            else
                # For TLS12 requirements are different between server and client
                if [ "$ROLE" = "server" ]; then
                    requires_any_configs_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
                elif [ "$ROLE" = "client" ]; then
                    requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT_WO_ECDH
                    requires_pk_alg "ECDSA"
                fi
            fi
            ;;
    esac

    case "$CMD_LINE" in
        *[-_\ =]psk*|*[-_\ =]PSK*) :;; # No certificate requirement with PSK
        */server1*|\
        */server2*|\
        */server7*)
            requires_certificate_authentication
            # Certificates with an RSA key. The algorithm requirement is
            # some subset of {PKCS#1v1.5 encryption, PKCS#1v1.5 signature,
            # PSS signature}. We can't easily tell which subset works, and
            # we aren't currently running ssl-opt.sh in configurations
            # where partial RSA support is a problem, so generically, we
            # just require RSA and it works out for our tests so far.
            requires_config_enabled "PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC"
    esac

    unset tmp
}

adapt_cmd_for_psk () {
    case "$2" in
        *openssl*s_server*) s='-psk 73776f726466697368 -nocert';;
        *openssl*) s='-psk 73776f726466697368';;
        *gnutls-cli*) s='--pskusername=Client_identity --pskkey=73776f726466697368';;
        *gnutls-serv*) s='--pskpasswd=../framework/data_files/simplepass.psk';;
        *) s='psk=73776f726466697368';;
    esac
    eval $1='"$2 $s"'
    unset s
}

# maybe_adapt_for_psk [RUN_TEST_OPTION...]
# If running in a PSK-only build, maybe adapt the test to use a pre-shared key.
#
# If not running in a PSK-only build, do nothing.
# If the test looks like it doesn't use a pre-shared key but can run with a
# pre-shared key, pass a pre-shared key. If the test looks like it can't run
# with a pre-shared key, skip it. If the test looks like it's already using
# a pre-shared key, do nothing.
#
# This code does not consider builds with ECDHE-PSK.
#
# Inputs:
# * $CLI_CMD, $SRV_CMD, $PXY_CMD: client/server/proxy commands.
# * $PSK_ONLY: YES if running in a PSK-only build (no asymmetric key exchanges).
# * "$@": options passed to run_test.
#
# Outputs:
# * $CLI_CMD, $SRV_CMD: may be modified to add PSK-relevant arguments.
# * $SKIP_NEXT: set to YES if the test can't run with PSK.
maybe_adapt_for_psk() {
    if [ "$PSK_ONLY" != "YES" ]; then
        return
    fi
    if [ "$SKIP_NEXT" = "YES" ]; then
        return
    fi
    case "$CLI_CMD $SRV_CMD" in
        *[-_\ =]psk*|*[-_\ =]PSK*)
            return;;
        *force_ciphersuite*)
            # The test case forces a non-PSK cipher suite. In some cases, a
            # PSK cipher suite could be substituted, but we're not ready for
            # that yet.
            SKIP_NEXT="YES"
            return;;
        *\ auth_mode=*|*[-_\ =]crt[_=]*)
            # The test case involves certificates. PSK won't do.
            SKIP_NEXT="YES"
            return;;
    esac
    adapt_cmd_for_psk CLI_CMD "$CLI_CMD"
    adapt_cmd_for_psk SRV_CMD "$SRV_CMD"
}

# PSK_PRESENT="YES" if at least one protocol versions supports at least
# one PSK key exchange mode.
PSK_PRESENT="NO"
# PSK_ONLY="YES" if all the available key exchange modes are PSK-based
# (pure-PSK or PSK-ephemeral, possibly both).
PSK_ONLY=""
for c in $CONFIGS_ENABLED; do
    case $c in
        MBEDTLS_KEY_EXCHANGE_PSK_ENABLED) PSK_PRESENT="YES";;
        MBEDTLS_KEY_EXCHANGE_*_PSK_ENABLED) PSK_PRESENT="YES";;
        MBEDTLS_KEY_EXCHANGE_*_ENABLED) PSK_ONLY="NO";;
        MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED) PSK_PRESENT="YES";;
        MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_*_ENABLED) PSK_PRESENT="YES";;
        MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_*_ENABLED) PSK_ONLY="NO";;
    esac
done
# At this stage, $PSK_ONLY is empty if we haven't detected a non-PSK
# key exchange, i.e. if we're in a PSK-only build or a build with no
# key exchanges at all. We avoid triggering PSK-only adaptation code in
# the edge case of no key exchanges.
: ${PSK_ONLY:=$PSK_PRESENT}
unset c

HAS_ALG_MD5="NO"
HAS_ALG_SHA_1="NO"
HAS_ALG_SHA_224="NO"
HAS_ALG_SHA_256="NO"
HAS_ALG_SHA_384="NO"
HAS_ALG_SHA_512="NO"

check_for_hash_alg()
{
    CURR_ALG="INVALID";
    CURR_ALG=PSA_WANT_ALG_${1}

    case $CONFIGS_ENABLED in
        *" $CURR_ALG"[\ =]*)
            return 0
        ;;
        *) :;;
    esac
    return 1
}

populate_enabled_hash_algs()
{
    for hash_alg in SHA_1 SHA_224 SHA_256 SHA_384 SHA_512 MD5; do
        if check_for_hash_alg "$hash_alg"; then
            hash_alg_variable=HAS_ALG_${hash_alg}
            eval ${hash_alg_variable}=YES
        fi
    done
}

# skip next test if the given hash alg is not supported
requires_hash_alg() {
    HASH_DEFINE="Invalid"
    HAS_HASH_ALG="NO"
    case $1 in
        MD5):;;
        SHA_1):;;
        SHA_224):;;
        SHA_256):;;
        SHA_384):;;
        SHA_512):;;
      *)
            echo "Unsupported hash alg - $1"
            exit 1
        ;;
    esac

    HASH_DEFINE=HAS_ALG_${1}
    eval "HAS_HASH_ALG=\${${HASH_DEFINE}}"
    if [ "$HAS_HASH_ALG" = "NO" ]
    then
        SKIP_NEXT="YES"
    fi
}

# Skip next test if the given pk alg is not enabled
requires_pk_alg() {
    case $1 in
        ECDSA)
            requires_config_enabled PSA_WANT_ALG_ECDSA
            ;;
        *)
            echo "Unknown/unimplemented case $1 in requires_pk_alg"
            exit 1
            ;;
    esac
}

# skip next test if OpenSSL doesn't support FALLBACK_SCSV
requires_openssl_with_fallback_scsv() {
    if [ -z "${OPENSSL_HAS_FBSCSV:-}" ]; then
        if $OPENSSL s_client -help 2>&1 | grep fallback_scsv >/dev/null
        then
            OPENSSL_HAS_FBSCSV="YES"
        else
            OPENSSL_HAS_FBSCSV="NO"
        fi
    fi
    if [ "$OPENSSL_HAS_FBSCSV" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# skip next test if either IN_CONTENT_LEN or MAX_CONTENT_LEN are below a value
requires_max_content_len() {
    requires_config_value_at_least "MBEDTLS_SSL_IN_CONTENT_LEN" $1
    requires_config_value_at_least "MBEDTLS_SSL_OUT_CONTENT_LEN" $1
}

# skip next test if GnuTLS isn't available
requires_gnutls() {
    if [ -z "${GNUTLS_AVAILABLE:-}" ]; then
        if ( which "$GNUTLS_CLI" && which "$GNUTLS_SERV" ) >/dev/null 2>&1; then
            GNUTLS_AVAILABLE="YES"
        else
            GNUTLS_AVAILABLE="NO"
        fi
    fi
    if [ "$GNUTLS_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# skip next test if GnuTLS-next isn't available
requires_gnutls_next() {
    if [ -z "${GNUTLS_NEXT_AVAILABLE:-}" ]; then
        if ( which "${GNUTLS_NEXT_CLI:-}" && which "${GNUTLS_NEXT_SERV:-}" ) >/dev/null 2>&1; then
            GNUTLS_NEXT_AVAILABLE="YES"
        else
            GNUTLS_NEXT_AVAILABLE="NO"
        fi
    fi
    if [ "$GNUTLS_NEXT_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

requires_openssl_next() {
    if [ -z "${OPENSSL_NEXT_AVAILABLE:-}" ]; then
        if which "${OPENSSL_NEXT:-}" >/dev/null 2>&1; then
            OPENSSL_NEXT_AVAILABLE="YES"
        else
            OPENSSL_NEXT_AVAILABLE="NO"
        fi
    fi
    if [ "$OPENSSL_NEXT_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# skip next test if openssl version is lower than 3.0
requires_openssl_3_x() {
    requires_openssl_next
    if [ "$OPENSSL_NEXT_AVAILABLE" = "NO" ]; then
        OPENSSL_3_X_AVAILABLE="NO"
    fi
    if [ -z "${OPENSSL_3_X_AVAILABLE:-}" ]; then
        if $OPENSSL_NEXT version 2>&1 | grep "OpenSSL 3." >/dev/null
        then
            OPENSSL_3_X_AVAILABLE="YES"
        else
            OPENSSL_3_X_AVAILABLE="NO"
        fi
    fi
    if [ "$OPENSSL_3_X_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# skip next test if openssl does not support ffdh keys
requires_openssl_tls1_3_with_ffdh() {
    requires_openssl_3_x
}

# skip next test if openssl cannot handle ephemeral key exchange
requires_openssl_tls1_3_with_compatible_ephemeral() {
    requires_openssl_next

    if !(is_config_enabled "PSA_WANT_ALG_ECDH"); then
        requires_openssl_tls1_3_with_ffdh
    fi
}

# skip next test if tls1_3 is not available
requires_openssl_tls1_3() {
    requires_openssl_next
    if [ "$OPENSSL_NEXT_AVAILABLE" = "NO" ]; then
        OPENSSL_TLS1_3_AVAILABLE="NO"
    fi
    if [ -z "${OPENSSL_TLS1_3_AVAILABLE:-}" ]; then
        if $OPENSSL_NEXT s_client -help 2>&1 | grep tls1_3 >/dev/null
        then
            OPENSSL_TLS1_3_AVAILABLE="YES"
        else
            OPENSSL_TLS1_3_AVAILABLE="NO"
        fi
    fi
    if [ "$OPENSSL_TLS1_3_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# OpenSSL servers forbid client renegotiation by default since OpenSSL 3.0.
# Older versions always allow it and have no command-line option.
OPENSSL_S_SERVER_CLIENT_RENEGOTIATION=
case $($OPENSSL s_server -help 2>&1) in
    *-client_renegotiation*)
        OPENSSL_S_SERVER_CLIENT_RENEGOTIATION=-client_renegotiation;;
esac

# skip next test if tls1_3 is not available
requires_gnutls_tls1_3() {
    requires_gnutls_next
    if [ "$GNUTLS_NEXT_AVAILABLE" = "NO" ]; then
        GNUTLS_TLS1_3_AVAILABLE="NO"
    fi
    if [ -z "${GNUTLS_TLS1_3_AVAILABLE:-}" ]; then
        if $GNUTLS_NEXT_CLI -l 2>&1 | grep VERS-TLS1.3 >/dev/null
        then
            GNUTLS_TLS1_3_AVAILABLE="YES"
        else
            GNUTLS_TLS1_3_AVAILABLE="NO"
        fi
    fi
    if [ "$GNUTLS_TLS1_3_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# Check %NO_TICKETS option
requires_gnutls_next_no_ticket() {
    requires_gnutls_next
    if [ "$GNUTLS_NEXT_AVAILABLE" = "NO" ]; then
        GNUTLS_NO_TICKETS_AVAILABLE="NO"
    fi
    if [ -z "${GNUTLS_NO_TICKETS_AVAILABLE:-}" ]; then
        if $GNUTLS_NEXT_CLI --priority-list 2>&1 | grep NO_TICKETS >/dev/null
        then
            GNUTLS_NO_TICKETS_AVAILABLE="YES"
        else
            GNUTLS_NO_TICKETS_AVAILABLE="NO"
        fi
    fi
    if [ "$GNUTLS_NO_TICKETS_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# Check %DISABLE_TLS13_COMPAT_MODE option
requires_gnutls_next_disable_tls13_compat() {
    requires_gnutls_next
    if [ "$GNUTLS_NEXT_AVAILABLE" = "NO" ]; then
        GNUTLS_DISABLE_TLS13_COMPAT_MODE_AVAILABLE="NO"
    fi
    if [ -z "${GNUTLS_DISABLE_TLS13_COMPAT_MODE_AVAILABLE:-}" ]; then
        if $GNUTLS_NEXT_CLI --priority-list 2>&1 | grep DISABLE_TLS13_COMPAT_MODE >/dev/null
        then
            GNUTLS_DISABLE_TLS13_COMPAT_MODE_AVAILABLE="YES"
        else
            GNUTLS_DISABLE_TLS13_COMPAT_MODE_AVAILABLE="NO"
        fi
    fi
    if [ "$GNUTLS_DISABLE_TLS13_COMPAT_MODE_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# skip next test if GnuTLS does not support the record size limit extension
requires_gnutls_record_size_limit() {
    requires_gnutls_next
    if [ "$GNUTLS_NEXT_AVAILABLE" = "NO" ]; then
        GNUTLS_RECORD_SIZE_LIMIT_AVAILABLE="NO"
    else
        GNUTLS_RECORD_SIZE_LIMIT_AVAILABLE="YES"
    fi
    if [ "$GNUTLS_RECORD_SIZE_LIMIT_AVAILABLE" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# skip next test if IPv6 isn't available on this host
requires_ipv6() {
    if [ -z "${HAS_IPV6:-}" ]; then
        $P_SRV server_addr='::1' > $SRV_OUT 2>&1 &
        SRV_PID=$!
        sleep 1
        kill $SRV_PID >/dev/null 2>&1
        if grep "NET - Binding of the socket failed" $SRV_OUT >/dev/null; then
            HAS_IPV6="NO"
        else
            HAS_IPV6="YES"
        fi
        rm -r $SRV_OUT
    fi

    if [ "$HAS_IPV6" = "NO" ]; then
        SKIP_NEXT="YES"
    fi
}

# skip next test if it's i686 or uname is not available
requires_not_i686() {
    if [ -z "${IS_I686:-}" ]; then
        IS_I686="YES"
        if which "uname" >/dev/null 2>&1; then
            if [ -z "$(uname -a | grep i686)" ]; then
                IS_I686="NO"
            fi
        fi
    fi
    if [ "$IS_I686" = "YES" ]; then
        SKIP_NEXT="YES"
    fi
}

MAX_CONTENT_LEN=16384
MAX_IN_LEN=$( get_config_value_or_default "MBEDTLS_SSL_IN_CONTENT_LEN" )
MAX_OUT_LEN=$( get_config_value_or_default "MBEDTLS_SSL_OUT_CONTENT_LEN" )
if [ "$LIST_TESTS" -eq 0 ];then
    # Calculate the input & output maximum content lengths set in the config

    # Calculate the maximum content length that fits both
    if [ "$MAX_IN_LEN" -lt "$MAX_CONTENT_LEN" ]; then
        MAX_CONTENT_LEN="$MAX_IN_LEN"
    fi
    if [ "$MAX_OUT_LEN" -lt "$MAX_CONTENT_LEN" ]; then
        MAX_CONTENT_LEN="$MAX_OUT_LEN"
    fi
fi
# skip the next test if the SSL output buffer is less than 16KB
requires_full_size_output_buffer() {
    if [ "$MAX_OUT_LEN" -ne 16384 ]; then
        SKIP_NEXT="YES"
    fi
}

# Skip the next test if called by all.sh in a component with MSan
# (which we also call MemSan) or Valgrind.
not_with_msan_or_valgrind() {
    case "_${MBEDTLS_TEST_CONFIGURATION:-}_" in
        *_msan_*|*_memsan_*|*_valgrind_*) SKIP_NEXT="YES";;
    esac
}

# skip the next test if valgrind is in use
not_with_valgrind() {
    if [ "$MEMCHECK" -gt 0 ]; then
        SKIP_NEXT="YES"
    fi
}

# skip the next test if valgrind is NOT in use
only_with_valgrind() {
    if [ "$MEMCHECK" -eq 0 ]; then
        SKIP_NEXT="YES"
    fi
}

# multiply the client timeout delay by the given factor for the next test
client_needs_more_time() {
    CLI_DELAY_FACTOR=$1
}

# wait for the given seconds after the client finished in the next test
server_needs_more_time() {
    SRV_DELAY_SECONDS=$1
}

# print_name <name>
print_name() {
    TESTS=$(( $TESTS + 1 ))
    LINE=""

    if [ "$SHOW_TEST_NUMBER" -gt 0 ]; then
        LINE="$TESTS "
    fi

    LINE="$LINE$1"

    printf "%s " "$LINE"
    LEN=$(( 72 - `echo "$LINE" | wc -c` ))
    for i in `seq 1 $LEN`; do printf '.'; done
    printf ' '

}

# record_outcome <outcome> [<failure-reason>]
# The test name must be in $NAME.
# Use $TEST_SUITE_NAME as the test suite name if set.
record_outcome() {
    echo "$1"
    if [ -n "$MBEDTLS_TEST_OUTCOME_FILE" ]; then
        printf '%s;%s;%s;%s;%s;%s\n' \
               "$MBEDTLS_TEST_PLATFORM" "$MBEDTLS_TEST_CONFIGURATION" \
               "${TEST_SUITE_NAME:-ssl-opt}" "$NAME" \
               "$1" "${2-}" \
               >>"$MBEDTLS_TEST_OUTCOME_FILE"
    fi
}
unset TEST_SUITE_NAME

# True if the presence of the given pattern in a log definitely indicates
# that the test has failed. False if the presence is inconclusive.
#
# Inputs:
# * $1: pattern found in the logs
# * $TIMES_LEFT: >0 if retrying is an option
#
# Outputs:
# * $outcome: set to a retry reason if the pattern is inconclusive,
#             unchanged otherwise.
# * Return value: 1 if the pattern is inconclusive,
#                 0 if the failure is definitive.
log_pattern_presence_is_conclusive() {
    # If we've run out of attempts, then don't retry no matter what.
    if [ $TIMES_LEFT -eq 0 ]; then
        return 0
    fi
    case $1 in
        "resend")
            # An undesired resend may have been caused by the OS dropping or
            # delaying a packet at an inopportune time.
            outcome="RETRY(resend)"
            return 1;;
    esac
}

# fail <message>
fail() {
    record_outcome "FAIL" "$1"
    echo "  ! $1"

    mv $SRV_OUT o-srv-${TESTS}.log
    mv $CLI_OUT o-cli-${TESTS}.log
    if [ -n "$PXY_CMD" ]; then
        mv $PXY_OUT o-pxy-${TESTS}.log
    fi
    echo "  ! outputs saved to o-XXX-${TESTS}.log"

    if [ "${LOG_FAILURE_ON_STDOUT:-0}" != 0 ]; then
        echo "  ! server output:"
        cat o-srv-${TESTS}.log
        echo "  ! ========================================================"
        echo "  ! client output:"
        cat o-cli-${TESTS}.log
        if [ -n "$PXY_CMD" ]; then
            echo "  ! ========================================================"
            echo "  ! proxy output:"
            cat o-pxy-${TESTS}.log
        fi
        echo ""
    fi

    FAILS=$(( $FAILS + 1 ))
}

# is_polar <cmd_line>
is_polar() {
    case "$1" in
        *ssl_client2*) true;;
        *ssl_server2*) true;;
        *) false;;
    esac
}

# openssl s_server doesn't have -www with DTLS
check_osrv_dtls() {
    case "$SRV_CMD" in
        *s_server*-dtls*)
            NEEDS_INPUT=1
            SRV_CMD="$( echo $SRV_CMD | sed s/-www// )";;
        *) NEEDS_INPUT=0;;
    esac
}

# provide input to commands that need it
provide_input() {
    if [ $NEEDS_INPUT -eq 0 ]; then
        return
    fi

    while true; do
        echo "HTTP/1.0 200 OK"
        sleep 1
    done
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

# Wait for process $2 named $3 to be listening on port $1. Print error to $4.
if type lsof >/dev/null 2>/dev/null; then
    wait_app_start() {
        newline='
'
        START_TIME=$(date +%s)
        if [ "$DTLS" -eq 1 ]; then
            proto=UDP
        else
            proto=TCP
        fi
        # Make a tight loop, server normally takes less than 1s to start.
        while true; do
              SERVER_PIDS=$(lsof -a -n -b -i "$proto:$1" -t)
              # When we use a proxy, it will be listening on the same port we
              # are checking for as well as the server and lsof will list both.
             case ${newline}${SERVER_PIDS}${newline} in
                  *${newline}${2}${newline}*) break;;
              esac
              if [ $(( $(date +%s) - $START_TIME )) -gt $DOG_DELAY ]; then
                  echo "$3 START TIMEOUT"
                  echo "$3 START TIMEOUT" >> $4
                  break
              fi
              # Linux and *BSD support decimal arguments to sleep. On other
              # OSes this may be a tight loop.
              sleep 0.1 2>/dev/null || true
        done
    }
else
    echo "Warning: lsof not available, wait_app_start = sleep"
    wait_app_start() {
        sleep "$START_DELAY"
    }
fi

# Wait for server process $2 to be listening on port $1.
wait_server_start() {
    wait_app_start $1 $2 "SERVER" $SRV_OUT
}

# Wait for proxy process $2 to be listening on port $1.
wait_proxy_start() {
    wait_app_start $1 $2 "PROXY" $PXY_OUT
}

# Given the client or server debug output, parse the unix timestamp that is
# included in the first 4 bytes of the random bytes and check that it's within
# acceptable bounds
check_server_hello_time() {
    # Extract the time from the debug (lvl 3) output of the client
    SERVER_HELLO_TIME="$(sed -n 's/.*server hello, current time: //p' < "$1")"
    # Get the Unix timestamp for now
    CUR_TIME=$(date +'%s')
    THRESHOLD_IN_SECS=300

    # Check if the ServerHello time was printed
    if [ -z "$SERVER_HELLO_TIME" ]; then
        return 1
    fi

    # Check the time in ServerHello is within acceptable bounds
    if [ $SERVER_HELLO_TIME -lt $(( $CUR_TIME - $THRESHOLD_IN_SECS )) ]; then
        # The time in ServerHello is at least 5 minutes before now
        return 1
    elif [ $SERVER_HELLO_TIME -gt $(( $CUR_TIME + $THRESHOLD_IN_SECS )) ]; then
        # The time in ServerHello is at least 5 minutes later than now
        return 1
    else
        return 0
    fi
}

# Extract the exported key from the output.
get_exported_key() {
    OUTPUT="$1"
    EXPORTED_KEY1=$(sed -n '/Exporting key of length 20 with label ".*": /s/.*: //p' $OUTPUT)
}

# Check that the exported key from the output matches the one obtained in get_exported_key().
check_exported_key() {
    OUTPUT="$1"
    EXPORTED_KEY2=$(sed -n '/Exporting key of length 20 with label ".*": /s/.*: //p' $OUTPUT)
    test "$EXPORTED_KEY1" = "$EXPORTED_KEY2"
}

# Check that the exported key from the output matches the one obtained in get_exported_key().
check_exported_key_openssl() {
    OUTPUT="$1"
    EXPORTED_KEY2=0x$(sed -n '/Keying material: /s/.*: //p' $OUTPUT)
    test "$EXPORTED_KEY1" = "$EXPORTED_KEY2"
}

# Get handshake memory usage from server or client output and put it into the variable specified by the first argument
handshake_memory_get() {
    OUTPUT_VARIABLE="$1"
    OUTPUT_FILE="$2"

    # Get memory usage from a pattern like "Heap memory usage after handshake: 23112 bytes. Peak memory usage was 33112"
    MEM_USAGE=$(sed -n 's/.*Heap memory usage after handshake: //p' < "$OUTPUT_FILE" | grep -o "[0-9]*" | head -1)

    # Check if memory usage was read
    if [ -z "$MEM_USAGE" ]; then
        echo "Error: Can not read the value of handshake memory usage"
        return 1
    else
        eval "$OUTPUT_VARIABLE=$MEM_USAGE"
        return 0
    fi
}

# Get handshake memory usage from server or client output and check if this value
# is not higher than the maximum given by the first argument
handshake_memory_check() {
    MAX_MEMORY="$1"
    OUTPUT_FILE="$2"

    # Get memory usage
    if ! handshake_memory_get "MEMORY_USAGE" "$OUTPUT_FILE"; then
        return 1
    fi

    # Check if memory usage is below max value
    if [ "$MEMORY_USAGE" -gt "$MAX_MEMORY" ]; then
        echo "\nFailed: Handshake memory usage was $MEMORY_USAGE bytes," \
             "but should be below $MAX_MEMORY bytes"
        return 1
    else
        return 0
    fi
}

# wait for client to terminate and set CLI_EXIT
# must be called right after starting the client
wait_client_done() {
    CLI_PID=$!

    CLI_DELAY=$(( $DOG_DELAY * $CLI_DELAY_FACTOR ))
    CLI_DELAY_FACTOR=1

    ( sleep $CLI_DELAY; echo "===CLIENT_TIMEOUT===" >> $CLI_OUT; kill $CLI_PID ) &
    DOG_PID=$!

    # For Ubuntu 22.04, `Terminated` message is outputed by wait command.
    # To remove it from stdout, redirect stdout/stderr to CLI_OUT
    wait $CLI_PID >> $CLI_OUT 2>&1
    CLI_EXIT=$?

    kill $DOG_PID >/dev/null 2>&1
    wait $DOG_PID >> $CLI_OUT 2>&1

    echo "EXIT: $CLI_EXIT" >> $CLI_OUT

    sleep $SRV_DELAY_SECONDS
    SRV_DELAY_SECONDS=0
}

# check if the given command uses dtls and sets global variable DTLS
detect_dtls() {
    case "$1" in
        *dtls=1*|*-dtls*|*-u*|*/dtls_*) DTLS=1;;
        *) DTLS=0;;
    esac
}

# check if the given command uses gnutls and sets global variable CMD_IS_GNUTLS
is_gnutls() {
    case "$1" in
    *gnutls-cli*)
        CMD_IS_GNUTLS=1
        ;;
    *gnutls-serv*)
        CMD_IS_GNUTLS=1
        ;;
    *)
        CMD_IS_GNUTLS=0
        ;;
    esac
}

# Generate random psk_list argument for ssl_server2
get_srv_psk_list ()
{
    case $(( TESTS % 3 )) in
        0) echo "psk_list=abc,dead,def,beef,Client_identity,6162636465666768696a6b6c6d6e6f70";;
        1) echo "psk_list=abc,dead,Client_identity,6162636465666768696a6b6c6d6e6f70,def,beef";;
        2) echo "psk_list=Client_identity,6162636465666768696a6b6c6d6e6f70,abc,dead,def,beef";;
    esac
}

# Determine what calc_verify trace is to be expected, if any.
#
# calc_verify is only called for two things: to calculate the
# extended master secret, and to process client authentication.
#
# Warning: the current implementation assumes that extended_ms is not
#          disabled on the client or on the server.
#
# Inputs:
# * $1: the value of the server auth_mode parameter.
#       'required' if client authentication is expected,
#       'none' or absent if not.
# * $CONFIGS_ENABLED
#
# Outputs:
# * $maybe_calc_verify: set to a trace expected in the debug logs
set_maybe_calc_verify() {
    maybe_calc_verify=
    case $CONFIGS_ENABLED in
        *\ MBEDTLS_SSL_EXTENDED_MASTER_SECRET\ *) :;;
        *)
            case ${1-} in
                ''|none) return;;
                required) :;;
                *) echo "Bad parameter 1 to set_maybe_calc_verify: $1"; exit 1;;
            esac
    esac
    maybe_calc_verify="PSA calc verify"
}

# Compare file content
# Usage: find_in_both pattern file1 file2
# extract from file1 the first line matching the pattern
# check in file2 that the same line can be found
find_in_both() {
        srv_pattern=$(grep -m 1 "$1" "$2");
        if [ -z "$srv_pattern" ]; then
                return 1;
        fi

        if grep "$srv_pattern" $3 >/dev/null; then :
                return 0;
        else
                return 1;
        fi
}

SKIP_HANDSHAKE_CHECK="NO"
skip_handshake_stage_check() {
    SKIP_HANDSHAKE_CHECK="YES"
}

# Analyze the commands that will be used in a test.
#
# Analyze and possibly instrument $PXY_CMD, $CLI_CMD, $SRV_CMD to pass
# extra arguments or go through wrappers.
#
# Inputs:
# * $@: supplemental options to run_test() (after the mandatory arguments).
# * $CLI_CMD, $PXY_CMD, $SRV_CMD: the client, proxy and server commands.
# * $DTLS: 1 if DTLS, otherwise 0.
#
# Outputs:
# * $CLI_CMD, $PXY_CMD, $SRV_CMD: may be tweaked.
analyze_test_commands() {
    # If the test uses DTLS, does not force a specific port, and does not
    # specify a custom proxy, add a simple proxy.
    # It provides timing info that's useful to debug failures.
    if [ "$DTLS" -eq 1 ] &&
       [ "$THIS_SRV_PORT" = "$SRV_PORT" ] &&
       [ -z "$PXY_CMD" ]
    then
        PXY_CMD="$P_PXY"
        case " $SRV_CMD " in
            *' server_addr=::1 '*)
                PXY_CMD="$PXY_CMD server_addr=::1 listen_addr=::1";;
        esac
    fi

    # update CMD_IS_GNUTLS variable
    is_gnutls "$SRV_CMD"

    # if the server uses gnutls but doesn't set priority, explicitly
    # set the default priority
    if [ "$CMD_IS_GNUTLS" -eq 1 ]; then
        case "$SRV_CMD" in
              *--priority*) :;;
              *) SRV_CMD="$SRV_CMD --priority=NORMAL";;
        esac
    fi

    # update CMD_IS_GNUTLS variable
    is_gnutls "$CLI_CMD"

    # if the client uses gnutls but doesn't set priority, explicitly
    # set the default priority
    if [ "$CMD_IS_GNUTLS" -eq 1 ]; then
        case "$CLI_CMD" in
              *--priority*) :;;
              *) CLI_CMD="$CLI_CMD --priority=NORMAL";;
        esac
    fi

    # fix client port
    if [ -n "$PXY_CMD" ]; then
        CLI_CMD=$( echo "$CLI_CMD" | sed s/+SRV_PORT/$PXY_PORT/g )
    else
        CLI_CMD=$( echo "$CLI_CMD" | sed s/+SRV_PORT/$THIS_SRV_PORT/g )
    fi

    # If the test forces a specific port and the server is OpenSSL or
    # GnuTLS, override its port specification.
    if [ "$THIS_SRV_PORT" != "$SRV_PORT" ]; then
        case "$SRV_CMD" in
            "$G_SRV"*|"$G_NEXT_SRV"*)
                SRV_CMD=$(
                    printf %s "$SRV_CMD " |
                    sed -e "s/ -p $SRV_PORT / -p $THIS_SRV_PORT /"
                );;
            "$O_SRV"*|"$O_NEXT_SRV"*) SRV_CMD="$SRV_CMD -accept $THIS_SRV_PORT";;
        esac
    fi

    # prepend valgrind to our commands if active
    if [ "$MEMCHECK" -gt 0 ]; then
        if is_polar "$SRV_CMD"; then
            SRV_CMD="valgrind --leak-check=full $SRV_CMD"
        fi
        if is_polar "$CLI_CMD"; then
            CLI_CMD="valgrind --leak-check=full $CLI_CMD"
        fi
    fi
}

# Check for failure conditions after a test case.
#
# Inputs from run_test:
# * positional parameters: test options (see run_test documentation)
# * $CLI_EXIT: client return code
# * $CLI_EXPECT: expected client return code
# * $SRV_RET: server return code
# * $CLI_OUT, $SRV_OUT, $PXY_OUT: files containing client/server/proxy logs
# * $TIMES_LEFT: if nonzero, a RETRY outcome is allowed
#
# Outputs:
# * $outcome: one of PASS/RETRY*/FAIL
check_test_failure() {
    outcome=FAIL

    if [ $TIMES_LEFT -gt 0 ] &&
       grep '===CLIENT_TIMEOUT===' $CLI_OUT >/dev/null
    then
        outcome="RETRY(client-timeout)"
        return
    fi

    # check if the client and server went at least to the handshake stage
    # (useful to avoid tests with only negative assertions and non-zero
    # expected client exit to incorrectly succeed in case of catastrophic
    # failure)
    if [ "X$SKIP_HANDSHAKE_CHECK" != "XYES" ]
    then
        if is_polar "$SRV_CMD"; then
            if grep "Performing the SSL/TLS handshake" $SRV_OUT >/dev/null; then :;
            else
                fail "server or client failed to reach handshake stage"
                return
            fi
        fi
        if is_polar "$CLI_CMD"; then
            if grep "Performing the SSL/TLS handshake" $CLI_OUT >/dev/null; then :;
            else
                fail "server or client failed to reach handshake stage"
                return
            fi
        fi
    fi

    SKIP_HANDSHAKE_CHECK="NO"
    # Check server exit code (only for Mbed TLS: GnuTLS and OpenSSL don't
    # exit with status 0 when interrupted by a signal, and we don't really
    # care anyway), in case e.g. the server reports a memory leak.
    if [ $SRV_RET != 0 ] && is_polar "$SRV_CMD"; then
        fail "Server exited with status $SRV_RET"
        return
    fi

    # check client exit code
    if [ \( "$CLI_EXPECT" = 0 -a "$CLI_EXIT" != 0 \) -o \
         \( "$CLI_EXPECT" != 0 -a "$CLI_EXIT" = 0 \) ]
    then
        fail "bad client exit code (expected $CLI_EXPECT, got $CLI_EXIT)"
        return
    fi

    # check other assertions
    # lines beginning with == are added by valgrind, ignore them
    # lines with 'Serious error when reading debug info', are valgrind issues as well
    while [ $# -gt 0 ]
    do
        case $1 in
            "-s")
                if grep -v '^==' $SRV_OUT | grep -v 'Serious error when reading debug info' | grep "$2" >/dev/null; then :; else
                    fail "pattern '$2' MUST be present in the Server output"
                    return
                fi
                ;;

            "-c")
                if grep -v '^==' $CLI_OUT | grep -v 'Serious error when reading debug info' | grep "$2" >/dev/null; then :; else
                    fail "pattern '$2' MUST be present in the Client output"
                    return
                fi
                ;;

            "-S")
                if grep -v '^==' $SRV_OUT | grep -v 'Serious error when reading debug info' | grep "$2" >/dev/null; then
                    if log_pattern_presence_is_conclusive "$2"; then
                        fail "pattern '$2' MUST NOT be present in the Server output"
                    fi
                    return
                fi
                ;;

            "-C")
                if grep -v '^==' $CLI_OUT | grep -v 'Serious error when reading debug info' | grep "$2" >/dev/null; then
                    if log_pattern_presence_is_conclusive "$2"; then
                        fail "pattern '$2' MUST NOT be present in the Client output"
                    fi
                    return
                fi
                ;;

                # The filtering in the following two options (-u and -U) do the following
                #   - ignore valgrind output
                #   - filter out everything but lines right after the pattern occurrences
                #   - keep one of each non-unique line
                #   - count how many lines remain
                # A line with '--' will remain in the result from previous outputs, so the number of lines in the result will be 1
                # if there were no duplicates.
            "-U")
                if [ $(grep -v '^==' $SRV_OUT | grep -v 'Serious error when reading debug info' | grep -A1 "$2" | grep -v "$2" | sort | uniq -d | wc -l) -gt 1 ]; then
                    fail "lines following pattern '$2' must be unique in Server output"
                    return
                fi
                ;;

            "-u")
                if [ $(grep -v '^==' $CLI_OUT | grep -v 'Serious error when reading debug info' | grep -A1 "$2" | grep -v "$2" | sort | uniq -d | wc -l) -gt 1 ]; then
                    fail "lines following pattern '$2' must be unique in Client output"
                    return
                fi
                ;;
            "-F")
                if ! $2 "$SRV_OUT"; then
                    fail "function call to '$2' failed on Server output"
                    return
                fi
                ;;
            "-f")
                if ! $2 "$CLI_OUT"; then
                    fail "function call to '$2' failed on Client output"
                    return
                fi
                ;;
            "-g")
                if ! eval "$2 '$SRV_OUT' '$CLI_OUT'"; then
                    fail "function call to '$2' failed on Server and Client output"
                    return
                fi
                ;;

            *)
                echo "Unknown test: $1" >&2
                exit 1
        esac
        shift 2
    done

    # check valgrind's results
    if [ "$MEMCHECK" -gt 0 ]; then
        if is_polar "$SRV_CMD" && has_mem_err $SRV_OUT; then
            fail "Server has memory errors"
            return
        fi
        if is_polar "$CLI_CMD" && has_mem_err $CLI_OUT; then
            fail "Client has memory errors"
            return
        fi
    fi

    # if we're here, everything is ok
    outcome=PASS
}

# Run the current test case: start the server and if applicable the proxy, run
# the client, wait for all processes to finish or time out.
#
# Inputs:
# * $NAME: test case name
# * $CLI_CMD, $SRV_CMD, $PXY_CMD: commands to run
# * $CLI_OUT, $SRV_OUT, $PXY_OUT: files to contain client/server/proxy logs
#
# Outputs:
# * $CLI_EXIT: client return code
# * $SRV_RET: server return code
do_run_test_once() {
    # run the commands
    if [ -n "$PXY_CMD" ]; then
        printf "# %s\n%s\n" "$NAME" "$PXY_CMD" > $PXY_OUT
        $PXY_CMD >> $PXY_OUT 2>&1 &
        PXY_PID=$!
        wait_proxy_start "$PXY_PORT" "$PXY_PID"
    fi

    check_osrv_dtls
    printf '# %s\n%s\n' "$NAME" "$SRV_CMD" > $SRV_OUT
    provide_input | $SRV_CMD >> $SRV_OUT 2>&1 &
    SRV_PID=$!
    wait_server_start "$THIS_SRV_PORT" "$SRV_PID"

    printf '# %s\n%s\n' "$NAME" "$CLI_CMD" > $CLI_OUT
    # The client must be a subprocess of the script in order for killing it to
    # work properly, that's why the ampersand is placed inside the eval command,
    # not at the end of the line: the latter approach will spawn eval as a
    # subprocess, and the $CLI_CMD as a grandchild.
    eval "$CLI_CMD &" >> $CLI_OUT 2>&1
    wait_client_done

    sleep 0.05

    # terminate the server (and the proxy)
    kill $SRV_PID
    # For Ubuntu 22.04, `Terminated` message is outputed by wait command.
    # To remove it from stdout, redirect stdout/stderr to SRV_OUT
    wait $SRV_PID >> $SRV_OUT 2>&1
    SRV_RET=$?

    if [ -n "$PXY_CMD" ]; then
        kill $PXY_PID >/dev/null 2>&1
        wait $PXY_PID >> $PXY_OUT 2>&1
    fi
}

# Detect if the current test is going to use TLS 1.3 or TLS 1.2.
# $1 and $2 contain the server and client command lines, respectively.
#
# Note: this function only provides some guess about TLS version by simply
#       looking at the server/client command lines. Even though this works
#       for the sake of tests' filtering (especially in conjunction with the
#       detect_required_features() function), it does NOT guarantee that the
#       result is accurate. It does not check other conditions, such as:
#       - we can force a ciphersuite which contains "WITH" in its name, meaning
#         that we are going to use TLS 1.2
#       - etc etc
get_tls_version() {
    # First check if the version is forced on an Mbed TLS peer
    case $1 in
        *tls12*)
            echo "TLS12"
            return;;
        *tls13*)
            echo "TLS13"
            return;;
    esac
    case $2 in
        *tls12*)
            echo "TLS12"
            return;;
        *tls13*)
            echo "TLS13"
            return;;
    esac
    # Second check if the version is forced on an OpenSSL or GnuTLS peer
    case $1 in
        tls1_2*)
            echo "TLS12"
            return;;
        *tls1_3)
            echo "TLS13"
            return;;
    esac
    case $2 in
        *tls1_2)
            echo "TLS12"
            return;;
        *tls1_3)
            echo "TLS13"
            return;;
    esac
    # Third if the version is not forced, if TLS 1.3 is enabled then the test
    # is aimed to run a TLS 1.3 handshake.
    if is_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
    then
        echo "TLS13"
    else
        echo "TLS12"
    fi
}

# Usage: run_test name [-p proxy_cmd] srv_cmd cli_cmd cli_exit [option [...]]
# Options:  -s pattern  pattern that must be present in server output
#           -c pattern  pattern that must be present in client output
#           -u pattern  lines after pattern must be unique in client output
#           -f call shell function on client output
#           -S pattern  pattern that must be absent in server output
#           -C pattern  pattern that must be absent in client output
#           -U pattern  lines after pattern must be unique in server output
#           -F call shell function on server output
#           -g call shell function on server and client output
run_test() {
    NAME="$1"
    shift 1

    if is_excluded "$NAME"; then
        SKIP_NEXT="NO"
        # There was no request to run the test, so don't record its outcome.
        return
    fi

    if [ "$LIST_TESTS" -gt 0 ]; then
        printf "%s\n" "${TEST_SUITE_NAME:-ssl-opt};$NAME"
        return
    fi

    # Use ssl-opt as default test suite name. Also see record_outcome function
    if is_excluded_test_suite "${TEST_SUITE_NAME:-ssl-opt}"; then
        # Do not skip next test and skip current test.
        SKIP_NEXT="NO"
        return
    fi

    print_name "$NAME"

    # Do we only run numbered tests?
    if [ -n "$RUN_TEST_NUMBER" ]; then
        case ",$RUN_TEST_NUMBER," in
            *",$TESTS,"*) :;;
            *) SKIP_NEXT="YES";;
        esac
    fi

    # Does this test specify a proxy?
    if [ "X$1" = "X-p" ]; then
        PXY_CMD="$2"
        shift 2
    else
        PXY_CMD=""
    fi

    # Does this test force a specific port?
    if [ "$1" = "-P" ]; then
        THIS_SRV_PORT="$2"
        shift 2
    else
        THIS_SRV_PORT="$SRV_PORT"
    fi

    # get commands and client output
    SRV_CMD="$1"
    CLI_CMD="$2"
    CLI_EXPECT="$3"
    shift 3

    # Check if test uses files
    case "$SRV_CMD $CLI_CMD" in
        *$DATA_FILES_PATH/*)
            requires_config_enabled MBEDTLS_FS_IO;;
    esac

    # Check if the test uses DTLS.
    detect_dtls "$SRV_CMD"
    if [ "$DTLS" -eq 1 ]; then
        requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
    fi


    # Guess the TLS version which is going to be used.
    # Note that this detection is wrong in some cases, which causes unduly
    # skipped test cases in builds with TLS 1.3 but not TLS 1.2.
    # https://github.com/Mbed-TLS/mbedtls/issues/9560
    TLS_VERSION=$(get_tls_version "$SRV_CMD" "$CLI_CMD")

    # If we're in a PSK-only build and the test can be adapted to PSK, do that.
    maybe_adapt_for_psk "$@"

    # If the client or server requires certain features that can be detected
    # from their command-line arguments, check whether they're enabled.
    detect_required_features "$SRV_CMD" "server" "$TLS_VERSION" "$@"
    detect_required_features "$CLI_CMD" "client" "$TLS_VERSION" "$@"

    # should we skip?
    if [ "X$SKIP_NEXT" = "XYES" ]; then
        SKIP_NEXT="NO"
        record_outcome "SKIP"
        SKIPS=$(( $SKIPS + 1 ))
        return
    fi

    analyze_test_commands "$@"

    # One regular run and two retries
    TIMES_LEFT=3
    while [ $TIMES_LEFT -gt 0 ]; do
        TIMES_LEFT=$(( $TIMES_LEFT - 1 ))

        do_run_test_once

        check_test_failure "$@"
        case $outcome in
            PASS) break;;
            RETRY*) printf "$outcome ";;
            FAIL) return;;
        esac
    done

    # If we get this far, the test case passed.
    record_outcome "PASS"
    if [ "$PRESERVE_LOGS" -gt 0 ]; then
        mv $SRV_OUT o-srv-${TESTS}.log
        mv $CLI_OUT o-cli-${TESTS}.log
        if [ -n "$PXY_CMD" ]; then
            mv $PXY_OUT o-pxy-${TESTS}.log
        fi
    fi

    rm -f $SRV_OUT $CLI_OUT $PXY_OUT
}

run_test_psa() {
    set_maybe_calc_verify none
    run_test    "PSA-supported ciphersuite: $1" \
                "$P_SRV debug_level=3 force_version=tls12" \
                "$P_CLI debug_level=3 force_ciphersuite=$1" \
                0 \
                -c "$maybe_calc_verify" \
                -c "calc PSA finished" \
                -s "$maybe_calc_verify" \
                -s "calc PSA finished" \
                -s "Protocol is TLSv1.2" \
                -c "Perform PSA-based ECDH computation."\
                -c "Perform PSA-based computation of digest of ServerKeyExchange" \
                -S "error" \
                -C "error"
    unset maybe_calc_verify
}

run_test_psa_force_curve() {
    set_maybe_calc_verify none
    run_test    "PSA - ECDH with $1" \
                "$P_SRV debug_level=4 force_version=tls12 groups=$1" \
                "$P_CLI debug_level=4 force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256 groups=$1" \
                0 \
                -c "$maybe_calc_verify" \
                -c "calc PSA finished" \
                -s "$maybe_calc_verify" \
                -s "calc PSA finished" \
                -s "Protocol is TLSv1.2" \
                -c "Perform PSA-based ECDH computation."\
                -c "Perform PSA-based computation of digest of ServerKeyExchange" \
                -S "error" \
                -C "error"
    unset maybe_calc_verify
}

# Test that the server's memory usage after a handshake is reduced when a client specifies
# a maximum fragment length.
#  first argument ($1) is MFL for SSL client
#  second argument ($2) is memory usage for SSL client with default MFL (16k)
run_test_memory_after_handshake_with_mfl()
{
    # The test passes if the difference is around 2*(16k-MFL)
    MEMORY_USAGE_LIMIT="$(( $2 - ( 2 * ( 16384 - $1 )) ))"

    # Leave some margin for robustness
    MEMORY_USAGE_LIMIT="$(( ( MEMORY_USAGE_LIMIT * 110 ) / 100 ))"

    run_test    "Handshake memory usage (MFL $1)" \
                "$P_SRV debug_level=3 auth_mode=required force_version=tls12" \
                "$P_CLI debug_level=3 \
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
                    force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM max_frag_len=$1" \
                0 \
                -F "handshake_memory_check $MEMORY_USAGE_LIMIT"
}


# Test that the server's memory usage after a handshake is reduced when a client specifies
# different values of Maximum Fragment Length: default (16k), 4k, 2k, 1k and 512 bytes
run_tests_memory_after_handshake()
{
    # all tests in this sequence requires the same configuration (see requires_config_enabled())
    SKIP_THIS_TESTS="$SKIP_NEXT"

    # first test with default MFU is to get reference memory usage
    MEMORY_USAGE_MFL_16K=0
    run_test    "Handshake memory usage initial (MFL 16384 - default)" \
                "$P_SRV debug_level=3 auth_mode=required force_version=tls12" \
                "$P_CLI debug_level=3 \
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
                    force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM" \
                0 \
                -F "handshake_memory_get MEMORY_USAGE_MFL_16K"

    SKIP_NEXT="$SKIP_THIS_TESTS"
    run_test_memory_after_handshake_with_mfl 4096 "$MEMORY_USAGE_MFL_16K"

    SKIP_NEXT="$SKIP_THIS_TESTS"
    run_test_memory_after_handshake_with_mfl 2048 "$MEMORY_USAGE_MFL_16K"

    SKIP_NEXT="$SKIP_THIS_TESTS"
    run_test_memory_after_handshake_with_mfl 1024 "$MEMORY_USAGE_MFL_16K"

    SKIP_NEXT="$SKIP_THIS_TESTS"
    run_test_memory_after_handshake_with_mfl 512 "$MEMORY_USAGE_MFL_16K"
}

run_test_export_keying_material() {
    unset EXPORTED_KEY1
    unset EXPORTED_KEY2
    TLS_VERSION="$1"

    case $TLS_VERSION in
        tls12) TLS_VERSION_PRINT="TLS 1.2";;
        tls13) TLS_VERSION_PRINT="TLS 1.3";;
    esac

    run_test    "$TLS_VERSION_PRINT: Export keying material" \
                "$P_SRV debug_level=4 force_version=$TLS_VERSION exp_label=test-label" \
                "$P_CLI debug_level=4 force_version=$TLS_VERSION exp_label=test-label" \
                0 \
                -s "Exporting key of length 20 with label \".*\": 0x" \
                -c "Exporting key of length 20 with label \".*\": 0x" \
                -f get_exported_key \
                -F check_exported_key
}

run_test_export_keying_material_openssl_compat() {
    unset EXPORTED_KEY1
    unset EXPORTED_KEY2
    TLS_VERSION="$1"

    case $TLS_VERSION in
        tls12) TLS_VERSION_PRINT="TLS 1.2"; OPENSSL_CLIENT="$O_CLI";;
        tls13) TLS_VERSION_PRINT="TLS 1.3"; OPENSSL_CLIENT="$O_NEXT_CLI";;
    esac

    run_test    "$TLS_VERSION_PRINT: Export keying material (OpenSSL compatibility)" \
                "$P_SRV debug_level=4 force_version=$TLS_VERSION exp_label=test-label" \
                "$OPENSSL_CLIENT -keymatexport test-label" \
                0 \
                -s "Exporting key of length 20 with label \".*\": 0x" \
                -c "Keying material exporter:" \
                -F get_exported_key \
                -f check_exported_key_openssl
}

cleanup() {
    rm -f $CLI_OUT $SRV_OUT $PXY_OUT $SESSION
    rm -f context_srv.txt
    rm -f context_cli.txt
    test -n "${SRV_PID:-}" && kill $SRV_PID >/dev/null 2>&1
    test -n "${PXY_PID:-}" && kill $PXY_PID >/dev/null 2>&1
    test -n "${CLI_PID:-}" && kill $CLI_PID >/dev/null 2>&1
    test -n "${DOG_PID:-}" && kill $DOG_PID >/dev/null 2>&1
    exit 1
}

#
# MAIN
#

# Make the outcome file path relative to the original directory, not
# to .../tests
case "$MBEDTLS_TEST_OUTCOME_FILE" in
    [!/]*)
        MBEDTLS_TEST_OUTCOME_FILE="$ORIGINAL_PWD/$MBEDTLS_TEST_OUTCOME_FILE"
        ;;
esac

populate_enabled_hash_algs

# Optimize filters: if $FILTER and $EXCLUDE can be expressed as shell
# patterns rather than regular expressions, use a case statement instead
# of calling grep. To keep the optimizer simple, it is incomplete and only
# detects simple cases: plain substring, everything, nothing.
#
# As an exception, the character '.' is treated as an ordinary character
# if it is the only special character in the string. This is because it's
# rare to need "any one character", but needing a literal '.' is common
# (e.g. '-f "DTLS 1.2"').
need_grep=
case "$FILTER" in
    '^$') simple_filter=;;
    '.*') simple_filter='*';;
    *[][$+*?\\^{\|}]*) # Regexp special characters (other than .), we need grep
        need_grep=1;;
    *) # No regexp or shell-pattern special character
        simple_filter="*$FILTER*";;
esac
case "$EXCLUDE" in
    '^$') simple_exclude=;;
    '.*') simple_exclude='*';;
    *[][$+*?\\^{\|}]*) # Regexp special characters (other than .), we need grep
        need_grep=1;;
    *) # No regexp or shell-pattern special character
        simple_exclude="*$EXCLUDE*";;
esac
if [ -n "$need_grep" ]; then
    is_excluded () {
        ! echo "$1" | grep "$FILTER" | grep -q -v "$EXCLUDE"
    }
else
    is_excluded () {
        case "$1" in
            $simple_exclude) true;;
            $simple_filter) false;;
            *) true;;
        esac
    }
fi

# Filter tests according to TEST_SUITE_NAME
is_excluded_test_suite () {
    if [ -n "$RUN_TEST_SUITE" ]
    then
        case ",$RUN_TEST_SUITE," in
            *",$1,"*) false;;
            *) true;;
        esac
    else
        false
    fi

}


if [ "$LIST_TESTS" -eq 0 ];then

    # sanity checks, avoid an avalanche of errors
    P_SRV_BIN="${P_SRV%%[  ]*}"
    P_CLI_BIN="${P_CLI%%[  ]*}"
    P_PXY_BIN="${P_PXY%%[  ]*}"
    if [ ! -x "$P_SRV_BIN" ]; then
        echo "Command '$P_SRV_BIN' is not an executable file"
        exit 1
    fi
    if [ ! -x "$P_CLI_BIN" ]; then
        echo "Command '$P_CLI_BIN' is not an executable file"
        exit 1
    fi
    if [ ! -x "$P_PXY_BIN" ]; then
        echo "Command '$P_PXY_BIN' is not an executable file"
        exit 1
    fi
    if [ "$MEMCHECK" -gt 0 ]; then
        if which valgrind >/dev/null 2>&1; then :; else
            echo "Memcheck not possible. Valgrind not found"
            exit 1
        fi
    fi
    if which $OPENSSL >/dev/null 2>&1; then :; else
        echo "Command '$OPENSSL' not found"
        exit 1
    fi

    # used by watchdog
    MAIN_PID="$$"

    # We use somewhat arbitrary delays for tests:
    # - how long do we wait for the server to start (when lsof not available)?
    # - how long do we allow for the client to finish?
    #   (not to check performance, just to avoid waiting indefinitely)
    # Things are slower with valgrind, so give extra time here.
    #
    # Note: without lsof, there is a trade-off between the running time of this
    # script and the risk of spurious errors because we didn't wait long enough.
    # The watchdog delay on the other hand doesn't affect normal running time of
    # the script, only the case where a client or server gets stuck.
    if [ "$MEMCHECK" -gt 0 ]; then
        START_DELAY=6
        DOG_DELAY=60
    else
        START_DELAY=2
        DOG_DELAY=20
    fi

    # some particular tests need more time:
    # - for the client, we multiply the usual watchdog limit by a factor
    # - for the server, we sleep for a number of seconds after the client exits
    # see client_need_more_time() and server_needs_more_time()
    CLI_DELAY_FACTOR=1
    SRV_DELAY_SECONDS=0

    # fix commands to use this port, force IPv4 while at it
    # +SRV_PORT will be replaced by either $SRV_PORT or $PXY_PORT later
    # Note: Using 'localhost' rather than 127.0.0.1 here is unwise, as on many
    # machines that will resolve to ::1, and we don't want ipv6 here.
    P_SRV="$P_SRV server_addr=127.0.0.1 server_port=$SRV_PORT"
    P_CLI="$P_CLI server_addr=127.0.0.1 server_port=+SRV_PORT"
    P_PXY="$P_PXY server_addr=127.0.0.1 server_port=$SRV_PORT listen_addr=127.0.0.1 listen_port=$PXY_PORT ${SEED:+"seed=$SEED"}"
    O_SRV="$O_SRV -accept $SRV_PORT"
    O_CLI="$O_CLI -connect 127.0.0.1:+SRV_PORT"
    G_SRV="$G_SRV -p $SRV_PORT"
    G_CLI="$G_CLI -p +SRV_PORT"

    # Newer versions of OpenSSL have a syntax to enable all "ciphers", even
    # low-security ones. This covers not just cipher suites but also protocol
    # versions. It is necessary, for example, to use (D)TLS 1.0/1.1 on
    # OpenSSL 1.1.1f from Ubuntu 20.04. The syntax was only introduced in
    # OpenSSL 1.1.0 (21e0c1d23afff48601eb93135defddae51f7e2e3) and I can't find
    # a way to discover it from -help, so check the openssl version.
    case $($OPENSSL version) in
        "OpenSSL 0"*|"OpenSSL 1.0"*) :;;
        *)
            O_CLI="$O_CLI -cipher ALL@SECLEVEL=0"
            O_SRV="$O_SRV -cipher ALL@SECLEVEL=0"
            ;;
    esac

    if [ -n "${OPENSSL_NEXT:-}" ]; then
        O_NEXT_SRV="$O_NEXT_SRV -accept $SRV_PORT"
        O_NEXT_SRV_NO_CERT="$O_NEXT_SRV_NO_CERT -accept $SRV_PORT"
        O_NEXT_SRV_EARLY_DATA="$O_NEXT_SRV_EARLY_DATA -accept $SRV_PORT"
        O_NEXT_CLI="$O_NEXT_CLI -connect 127.0.0.1:+SRV_PORT"
        O_NEXT_CLI_NO_CERT="$O_NEXT_CLI_NO_CERT -connect 127.0.0.1:+SRV_PORT"
    fi

    if [ -n "${GNUTLS_NEXT_SERV:-}" ]; then
        G_NEXT_SRV="$G_NEXT_SRV -p $SRV_PORT"
        G_NEXT_SRV_NO_CERT="$G_NEXT_SRV_NO_CERT -p $SRV_PORT"
    fi

    if [ -n "${GNUTLS_NEXT_CLI:-}" ]; then
        G_NEXT_CLI="$G_NEXT_CLI -p +SRV_PORT"
        G_NEXT_CLI_NO_CERT="$G_NEXT_CLI_NO_CERT -p +SRV_PORT localhost"
    fi

    # Allow SHA-1, because many of our test certificates use it
    P_SRV="$P_SRV allow_sha1=1"
    P_CLI="$P_CLI allow_sha1=1"

fi
# Also pick a unique name for intermediate files
SRV_OUT="srv_out.$$"
CLI_OUT="cli_out.$$"
PXY_OUT="pxy_out.$$"
SESSION="session.$$"

SKIP_NEXT="NO"

trap cleanup INT TERM HUP

# Basic test

# Checks that:
# - things work with all ciphersuites active (used with config-full in all.sh)
# - the expected parameters are selected
requires_ciphersuite_enabled TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256
requires_hash_alg SHA_512 # "signature_algorithm ext: 6"
requires_config_enabled PSA_WANT_ECC_MONTGOMERY_255
run_test    "Default, TLS 1.2" \
            "$P_SRV debug_level=3" \
            "$P_CLI force_version=tls12" \
            0 \
            -s "Protocol is TLSv1.2" \
            -s "Ciphersuite is TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256" \
            -s "client hello v3, signature_algorithm ext: 6" \
            -s "ECDHE curve: x25519" \
            -S "error" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_ciphersuite_enabled TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256
run_test    "Default, DTLS" \
            "$P_SRV dtls=1" \
            "$P_CLI dtls=1" \
            0 \
            -s "Protocol is DTLSv1.2" \
            -s "Ciphersuite is TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256"

run_test    "TLS client auth: required" \
            "$P_SRV auth_mode=required" \
            "$P_CLI" \
            0 \
            -s "Verifying peer X.509 certificate... ok"

run_test    "key size: TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            "$P_SRV" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -c "Ciphersuite is TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            -c "Key size is 256"

run_test    "key size: TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            "$P_SRV" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Ciphersuite is TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            -c "Key size is 128"

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
# server5.key.enc is in PEM format and AES-256-CBC crypted. Unfortunately PEM
# module does not support PSA dispatching so we need builtin support. With the
# removal of the legacy cryptography configuration options, there is currently
# no way to express this dependency. This test fails if run in a configuration
# where the built-in implementation of CBC or AES is not present.
requires_hash_alg MD5
requires_hash_alg SHA_256
run_test    "TLS: password protected client key" \
            "$P_SRV force_version=tls12 auth_mode=required" \
            "$P_CLI crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key.enc key_pwd=PolarSSLTest" \
            0

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
# server5.key.enc is in PEM format and AES-256-CBC crypted. Unfortunately PEM
# module does not support PSA dispatching so we need builtin support. With the
# removal of the legacy cryptography configuration options, there is currently
# no way to express this dependency. This test fails if run in a configuration
# where the built-in implementation of CBC or AES is not present.
requires_hash_alg MD5
requires_hash_alg SHA_256
run_test    "TLS: password protected server key" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key.enc key_pwd=PolarSSLTest" \
            "$P_CLI force_version=tls12" \
            0

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
# server5.key.enc is in PEM format and AES-256-CBC crypted. Unfortunately PEM
# module does not support PSA dispatching so we need builtin support. With the
# removal of the legacy cryptography configuration options, there is currently
# no way to express this dependency. This test fails if run in a configuration
# where the built-in implementation of CBC or AES is not present.
requires_hash_alg MD5
requires_hash_alg SHA_256
run_test    "TLS: password protected server key, two certificates" \
            "$P_SRV force_version=tls12\
              key_file=$DATA_FILES_PATH/server5.key.enc key_pwd=PolarSSLTest crt_file=$DATA_FILES_PATH/server5.crt \
              key_file2=$DATA_FILES_PATH/server2.key.enc key_pwd2=PolarSSLTest crt_file2=$DATA_FILES_PATH/server2.crt" \
            "$P_CLI" \
            0

run_test    "CA callback on client" \
            "$P_SRV debug_level=3" \
            "$P_CLI ca_callback=1 debug_level=3 " \
            0 \
            -c "use CA callback for X.509 CRT verification" \
            -S "error" \
            -C "error"

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_hash_alg SHA_256
run_test    "CA callback on server" \
            "$P_SRV auth_mode=required" \
            "$P_CLI ca_callback=1 debug_level=3 crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            0 \
            -c "use CA callback for X.509 CRT verification" \
            -s "Verifying peer X.509 certificate... ok" \
            -S "error" \
            -C "error"

# Test using an EC opaque private key for client authentication
requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
requires_hash_alg SHA_256
run_test    "Opaque key for client authentication: ECDHE-ECDSA" \
            "$P_SRV force_version=tls12 auth_mode=required crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI key_opaque=1 crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key key_opaque_algs=ecdsa-sign,none" \
            0 \
            -c "key type: EC" \
            -c "Ciphersuite is TLS-ECDHE-ECDSA" \
            -s "Verifying peer X.509 certificate... ok" \
            -s "Ciphersuite is TLS-ECDHE-ECDSA" \
            -S "error" \
            -C "error"

# Test using a RSA opaque private key for client authentication
requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
requires_hash_alg SHA_256
run_test    "Opaque key for client authentication: ECDHE-RSA" \
            "$P_SRV force_version=tls12 auth_mode=required crt_file=$DATA_FILES_PATH/server2-sha256.crt \
             key_file=$DATA_FILES_PATH/server2.key" \
            "$P_CLI key_opaque=1 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
             key_file=$DATA_FILES_PATH/server2.key key_opaque_algs=rsa-sign-pkcs1,none" \
            0 \
            -c "key type: RSA" \
            -c "Ciphersuite is TLS-ECDHE-RSA" \
            -s "Verifying peer X.509 certificate... ok" \
            -s "Ciphersuite is TLS-ECDHE-RSA" \
            -S "error" \
            -C "error"

# Test using an EC opaque private key for server authentication
requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
requires_hash_alg SHA_256
run_test    "Opaque key for server authentication: ECDHE-ECDSA" \
            "$P_SRV key_opaque=1 crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key  key_opaque_algs=ecdsa-sign,none" \
            "$P_CLI force_version=tls12" \
            0 \
            -c "Verifying peer X.509 certificate... ok" \
            -c "Ciphersuite is TLS-ECDHE-ECDSA" \
            -s "key types: EC, none" \
            -s "Ciphersuite is TLS-ECDHE-ECDSA" \
            -S "error" \
            -C "error"

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_hash_alg SHA_256
run_test    "Opaque key for server authentication: invalid alg: ECDHE-ECDSA with ecdh" \
            "$P_SRV key_opaque=1 crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key key_opaque_algs=ecdh,none \
             debug_level=1" \
            "$P_CLI force_version=tls12 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM" \
            1 \
            -s "key types: EC, none" \
            -s "got ciphersuites in common, but none of them usable" \
            -s "error" \
            -c "error"

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
requires_hash_alg SHA_256
requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "Opaque keys for server authentication: EC keys with different algs, force ECDHE-ECDSA" \
            "$P_SRV force_version=tls12 key_opaque=1 crt_file=$DATA_FILES_PATH/server7.crt \
             key_file=$DATA_FILES_PATH/server7.key key_opaque_algs=ecdh,none \
             crt_file2=$DATA_FILES_PATH/server5.crt key_file2=$DATA_FILES_PATH/server5.key \
             key_opaque_algs2=ecdsa-sign,none" \
            "$P_CLI force_version=tls12" \
            0 \
            -c "Verifying peer X.509 certificate... ok" \
            -c "Ciphersuite is TLS-ECDHE-ECDSA" \
            -c "CN=Polarssl Test EC CA" \
            -s "key types: EC, EC" \
            -s "Ciphersuite is TLS-ECDHE-ECDSA" \
            -S "error" \
            -C "error"

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_hash_alg SHA_384
requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "Opaque keys for server authentication: EC + RSA, force ECDHE-ECDSA" \
            "$P_SRV key_opaque=1 crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key key_opaque_algs=ecdsa-sign,none \
             crt_file2=$DATA_FILES_PATH/server2-sha256.crt \
             key_file2=$DATA_FILES_PATH/server2.key key_opaque_algs2=rsa-sign-pkcs1,none" \
            "$P_CLI force_version=tls12 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM" \
            0 \
            -c "Verifying peer X.509 certificate... ok" \
            -c "Ciphersuite is TLS-ECDHE-ECDSA" \
            -c "CN=Polarssl Test EC CA" \
            -s "key types: EC, RSA" \
            -s "Ciphersuite is TLS-ECDHE-ECDSA" \
            -S "error" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
run_test    "TLS 1.3 opaque key: no suitable algorithm found" \
            "$P_SRV debug_level=4 auth_mode=required key_opaque=1 key_opaque_algs=rsa-sign-pkcs1,none" \
            "$P_CLI debug_level=4 key_opaque=1 key_opaque_algs=rsa-sign-pkcs1,rsa-sign-pss" \
            1 \
            -c "key type: RSA" \
            -s "key types: RSA, EC" \
            -c "error" \
            -s "no suitable signature algorithm"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
run_test    "TLS 1.3 opaque key: suitable algorithm found" \
            "$P_SRV debug_level=4 auth_mode=required key_opaque=1 key_opaque_algs=rsa-sign-pkcs1,rsa-sign-pss" \
            "$P_CLI debug_level=4 key_opaque=1 key_opaque_algs=rsa-sign-pkcs1,rsa-sign-pss" \
            0 \
            -c "key type: RSA" \
            -s "key types: RSA, EC" \
            -C "error" \
            -S "error"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
run_test    "TLS 1.3 opaque key: first client sig alg not suitable" \
            "$P_SRV debug_level=4 auth_mode=required key_opaque=1 key_opaque_algs=rsa-sign-pss-sha512,none" \
            "$P_CLI debug_level=4 sig_algs=rsa_pss_rsae_sha256,rsa_pss_rsae_sha512" \
            0 \
            -s "key types: RSA, EC" \
            -s "CertificateVerify signature failed with rsa_pss_rsae_sha256" \
            -s "CertificateVerify signature with rsa_pss_rsae_sha512" \
            -C "error" \
            -S "error" \

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
run_test    "TLS 1.3 opaque key: 2 keys on server, suitable algorithm found" \
            "$P_SRV debug_level=4 auth_mode=required key_opaque=1 key_opaque_algs2=ecdsa-sign,none key_opaque_algs=rsa-sign-pkcs1,rsa-sign-pss" \
            "$P_CLI debug_level=4 key_opaque=1 key_opaque_algs=rsa-sign-pkcs1,rsa-sign-pss" \
            0 \
            -c "key type: RSA" \
            -s "key types: RSA, EC" \
            -C "error" \
            -S "error" \

# Test using a RSA opaque private key for server authentication
requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
requires_hash_alg SHA_256
run_test    "Opaque key for server authentication: ECDHE-RSA" \
            "$P_SRV key_opaque=1 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
             key_file=$DATA_FILES_PATH/server2.key key_opaque_algs=rsa-sign-pkcs1,none" \
            "$P_CLI force_version=tls12" \
            0 \
            -c "Verifying peer X.509 certificate... ok" \
            -c "Ciphersuite is TLS-ECDHE-RSA" \
            -s "key types: RSA, none" \
            -s "Ciphersuite is TLS-ECDHE-RSA" \
            -S "error" \
            -C "error"

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
run_test    "Opaque key for server authentication: ECDHE-RSA, PSS instead of PKCS1" \
            "$P_SRV auth_mode=required key_opaque=1 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
             key_file=$DATA_FILES_PATH/server2.key key_opaque_algs=rsa-sign-pss,none debug_level=1" \
            "$P_CLI crt_file=$DATA_FILES_PATH/server2-sha256.crt \
             key_file=$DATA_FILES_PATH/server2.key force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            1 \
            -s "key types: RSA, none" \
            -s "got ciphersuites in common, but none of them usable" \
            -s "error" \
            -c "error"

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_config_disabled MBEDTLS_X509_REMOVE_INFO
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
run_test    "Opaque keys for server authentication: RSA keys with different algs" \
            "$P_SRV force_version=tls12 auth_mode=required key_opaque=1 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
             key_file=$DATA_FILES_PATH/server2.key key_opaque_algs=rsa-sign-pss,none \
             crt_file2=$DATA_FILES_PATH/server4.crt \
             key_file2=$DATA_FILES_PATH/server4.key key_opaque_algs2=rsa-sign-pkcs1,none" \
            "$P_CLI force_version=tls12" \
            0 \
            -c "Verifying peer X.509 certificate... ok" \
            -c "Ciphersuite is TLS-ECDHE-RSA" \
            -c "CN=Polarssl Test EC CA" \
            -s "key types: RSA, RSA" \
            -s "Ciphersuite is TLS-ECDHE-RSA" \
            -S "error" \
            -C "error"

requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
requires_hash_alg SHA_384
requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "Opaque keys for server authentication: EC + RSA, force ECDHE-RSA" \
            "$P_SRV auth_mode=required key_opaque=1 crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key key_opaque_algs=ecdsa-sign,none \
             crt_file2=$DATA_FILES_PATH/server4.crt \
             key_file2=$DATA_FILES_PATH/server4.key key_opaque_algs2=rsa-sign-pkcs1,none" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            0 \
            -c "Verifying peer X.509 certificate... ok" \
            -c "Ciphersuite is TLS-ECDHE-RSA" \
            -c "CN=Polarssl Test EC CA" \
            -s "key types: EC, RSA" \
            -s "Ciphersuite is TLS-ECDHE-RSA" \
            -S "error" \
            -C "error"

# Test using an EC opaque private key for client/server authentication
requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
requires_hash_alg SHA_256
run_test    "Opaque key for client/server authentication: ECDHE-ECDSA" \
            "$P_SRV force_version=tls12 auth_mode=required key_opaque=1 crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key key_opaque_algs=ecdsa-sign,none" \
            "$P_CLI key_opaque=1 crt_file=$DATA_FILES_PATH/server5.crt \
             key_file=$DATA_FILES_PATH/server5.key key_opaque_algs=ecdsa-sign,none" \
            0 \
            -c "key type: EC" \
            -c "Verifying peer X.509 certificate... ok" \
            -c "Ciphersuite is TLS-ECDHE-ECDSA" \
            -s "key types: EC, none" \
            -s "Verifying peer X.509 certificate... ok" \
            -s "Ciphersuite is TLS-ECDHE-ECDSA" \
            -S "error" \
            -C "error"

# Test using a RSA opaque private key for client/server authentication
requires_config_enabled MBEDTLS_X509_CRT_PARSE_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
run_test    "Opaque key for client/server authentication: ECDHE-RSA" \
            "$P_SRV auth_mode=required key_opaque=1 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
             key_file=$DATA_FILES_PATH/server2.key  key_opaque_algs=rsa-sign-pkcs1,none" \
            "$P_CLI force_version=tls12 key_opaque=1 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
             key_file=$DATA_FILES_PATH/server2.key  key_opaque_algs=rsa-sign-pkcs1,none" \
            0 \
            -c "key type: RSA" \
            -c "Verifying peer X.509 certificate... ok" \
            -c "Ciphersuite is TLS-ECDHE-RSA" \
            -s "key types: RSA, none" \
            -s "Verifying peer X.509 certificate... ok" \
            -s "Ciphersuite is TLS-ECDHE-RSA" \
            -S "error" \
            -C "error"

# Test ciphersuites which we expect to be fully supported by PSA Crypto
# and check that we don't fall back to Mbed TLS' internal crypto primitives.
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-128-CCM
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-256-CCM
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-256-CCM-8
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256
run_test_psa TLS-ECDHE-ECDSA-WITH-AES-256-CBC-SHA384

requires_config_enabled PSA_WANT_ECC_SECP_R1_521
run_test_psa_force_curve "secp521r1"
requires_config_enabled PSA_WANT_ECC_BRAINPOOL_P_R1_512
run_test_psa_force_curve "brainpoolP512r1"
requires_config_enabled PSA_WANT_ECC_SECP_R1_384
run_test_psa_force_curve "secp384r1"
requires_config_enabled PSA_WANT_ECC_BRAINPOOL_P_R1_384
run_test_psa_force_curve "brainpoolP384r1"
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test_psa_force_curve "secp256r1"
requires_config_enabled PSA_WANT_ECC_SECP_K1_256
run_test_psa_force_curve "secp256k1"
requires_config_enabled PSA_WANT_ECC_BRAINPOOL_P_R1_256
run_test_psa_force_curve "brainpoolP256r1"

# Test current time in ServerHello
requires_config_enabled MBEDTLS_HAVE_TIME
run_test    "ServerHello contains gmt_unix_time" \
            "$P_SRV debug_level=3" \
            "$P_CLI force_version=tls12 debug_level=3" \
            0 \
            -f "check_server_hello_time" \
            -F "check_server_hello_time"

# Test for uniqueness of IVs in AEAD ciphersuites
run_test    "Unique IV in GCM" \
            "$P_SRV exchanges=20 debug_level=4" \
            "$P_CLI exchanges=20 debug_level=4 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384" \
            0 \
            -u "IV used" \
            -U "IV used"

# Test for correctness of sent single supported algorithm
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
requires_pk_alg "ECDSA"
requires_hash_alg SHA_256
run_test    "Single supported algorithm sending: mbedtls client" \
            "$P_SRV sig_algs=ecdsa_secp256r1_sha256 auth_mode=required" \
            "$P_CLI force_version=tls12 sig_algs=ecdsa_secp256r1_sha256 debug_level=3" \
            0 \
            -c "Supported Signature Algorithm found: 04 03"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
requires_hash_alg SHA_256
run_test    "Single supported algorithm sending: openssl client" \
            "$P_SRV sig_algs=ecdsa_secp256r1_sha256 auth_mode=required" \
            "$O_CLI -cert $DATA_FILES_PATH/server6.crt \
                    -key $DATA_FILES_PATH/server6.key" \
            0

# Tests for certificate verification callback
run_test    "Configuration-specific CRT verification callback" \
            "$P_SRV debug_level=3" \
            "$P_CLI context_crt_cb=0 debug_level=3" \
            0 \
            -S "error" \
            -c "Verify requested for " \
            -c "Use configuration-specific verification callback" \
            -C "Use context-specific verification callback" \
            -C "error"

run_test    "Context-specific CRT verification callback" \
            "$P_SRV debug_level=3" \
            "$P_CLI context_crt_cb=1 debug_level=3" \
            0 \
            -S "error" \
            -c "Verify requested for " \
            -c "Use context-specific verification callback" \
            -C "Use configuration-specific verification callback" \
            -C "error"

# Tests for SHA-1 support
requires_hash_alg SHA_1
run_test    "SHA-1 forbidden by default in server certificate" \
            "$P_SRV key_file=$DATA_FILES_PATH/server2.key crt_file=$DATA_FILES_PATH/server2.crt" \
            "$P_CLI debug_level=2 force_version=tls12 allow_sha1=0" \
            1 \
            -c "The certificate is signed with an unacceptable hash"

requires_hash_alg SHA_1
run_test    "SHA-1 explicitly allowed in server certificate" \
            "$P_SRV key_file=$DATA_FILES_PATH/server2.key crt_file=$DATA_FILES_PATH/server2.crt" \
            "$P_CLI force_version=tls12 allow_sha1=1" \
            0

run_test    "SHA-256 allowed by default in server certificate" \
            "$P_SRV key_file=$DATA_FILES_PATH/server2.key crt_file=$DATA_FILES_PATH/server2-sha256.crt" \
            "$P_CLI force_version=tls12 allow_sha1=0" \
            0

requires_hash_alg SHA_1
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
run_test    "SHA-1 forbidden by default in client certificate" \
            "$P_SRV force_version=tls12 auth_mode=required allow_sha1=0" \
            "$P_CLI key_file=$DATA_FILES_PATH/cli-rsa.key crt_file=$DATA_FILES_PATH/cli-rsa-sha1.crt" \
            1 \
            -s "The certificate is signed with an unacceptable hash"

requires_hash_alg SHA_1
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
run_test    "SHA-1 explicitly allowed in client certificate" \
            "$P_SRV force_version=tls12 auth_mode=required allow_sha1=1" \
            "$P_CLI key_file=$DATA_FILES_PATH/cli-rsa.key crt_file=$DATA_FILES_PATH/cli-rsa-sha1.crt" \
            0

requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
run_test    "SHA-256 allowed by default in client certificate" \
            "$P_SRV force_version=tls12 auth_mode=required allow_sha1=0" \
            "$P_CLI key_file=$DATA_FILES_PATH/cli-rsa.key crt_file=$DATA_FILES_PATH/cli-rsa-sha256.crt" \
            0

# Tests for datagram packing
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS: multiple records in same datagram, client and server" \
            "$P_SRV dtls=1 dgram_packing=1 debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=1 debug_level=2" \
            0 \
            -c "next record in same datagram" \
            -s "next record in same datagram"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS: multiple records in same datagram, client only" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=1 debug_level=2" \
            0 \
            -s "next record in same datagram" \
            -C "next record in same datagram"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS: multiple records in same datagram, server only" \
            "$P_SRV dtls=1 dgram_packing=1 debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=2" \
            0 \
            -S "next record in same datagram" \
            -c "next record in same datagram"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS: multiple records in same datagram, neither client nor server" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=2" \
            0 \
            -S "next record in same datagram" \
            -C "next record in same datagram"

# Tests for Context serialization

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, client serializes, CCM" \
            "$P_SRV dtls=1 serialize=0 exchanges=2" \
            "$P_CLI dtls=1 serialize=1 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Deserializing connection..." \
            -S "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, client serializes, ChaChaPoly" \
            "$P_SRV dtls=1 serialize=0 exchanges=2" \
            "$P_CLI dtls=1 serialize=1 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -c "Deserializing connection..." \
            -S "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, client serializes, GCM" \
            "$P_SRV dtls=1 serialize=0 exchanges=2" \
            "$P_CLI dtls=1 serialize=1 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256" \
            0 \
            -c "Deserializing connection..." \
            -S "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Context serialization, client serializes, with CID" \
            "$P_SRV dtls=1 serialize=0 exchanges=2 cid=1 cid_val=dead" \
            "$P_CLI dtls=1 serialize=1 exchanges=2 cid=1 cid_val=beef" \
            0 \
            -c "Deserializing connection..." \
            -S "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, server serializes, CCM" \
            "$P_SRV dtls=1 serialize=1 exchanges=2" \
            "$P_CLI dtls=1 serialize=0 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -C "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, server serializes, ChaChaPoly" \
            "$P_SRV dtls=1 serialize=1 exchanges=2" \
            "$P_CLI dtls=1 serialize=0 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -C "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, server serializes, GCM" \
            "$P_SRV dtls=1 serialize=1 exchanges=2" \
            "$P_CLI dtls=1 serialize=0 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256" \
            0 \
            -C "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Context serialization, server serializes, with CID" \
            "$P_SRV dtls=1 serialize=1 exchanges=2 cid=1 cid_val=dead" \
            "$P_CLI dtls=1 serialize=0 exchanges=2 cid=1 cid_val=beef" \
            0 \
            -C "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, both serialize, CCM" \
            "$P_SRV dtls=1 serialize=1 exchanges=2" \
            "$P_CLI dtls=1 serialize=1 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, both serialize, ChaChaPoly" \
            "$P_SRV dtls=1 serialize=1 exchanges=2" \
            "$P_CLI dtls=1 serialize=1 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -c "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, both serialize, GCM" \
            "$P_SRV dtls=1 serialize=1 exchanges=2" \
            "$P_CLI dtls=1 serialize=1 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256" \
            0 \
            -c "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Context serialization, both serialize, with CID" \
            "$P_SRV dtls=1 serialize=1 exchanges=2 cid=1 cid_val=dead" \
            "$P_CLI dtls=1 serialize=1 exchanges=2 cid=1 cid_val=beef" \
            0 \
            -c "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, client serializes, CCM" \
            "$P_SRV dtls=1 serialize=0 exchanges=2" \
            "$P_CLI dtls=1 serialize=2 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Deserializing connection..." \
            -S "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, client serializes, ChaChaPoly" \
            "$P_SRV dtls=1 serialize=0 exchanges=2" \
            "$P_CLI dtls=1 serialize=2 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -c "Deserializing connection..." \
            -S "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, client serializes, GCM" \
            "$P_SRV dtls=1 serialize=0 exchanges=2" \
            "$P_CLI dtls=1 serialize=2 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256" \
            0 \
            -c "Deserializing connection..." \
            -S "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Context serialization, re-init, client serializes, with CID" \
            "$P_SRV dtls=1 serialize=0 exchanges=2 cid=1 cid_val=dead" \
            "$P_CLI dtls=1 serialize=2 exchanges=2 cid=1 cid_val=beef" \
            0 \
            -c "Deserializing connection..." \
            -S "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, server serializes, CCM" \
            "$P_SRV dtls=1 serialize=2 exchanges=2" \
            "$P_CLI dtls=1 serialize=0 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -C "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, server serializes, ChaChaPoly" \
            "$P_SRV dtls=1 serialize=2 exchanges=2" \
            "$P_CLI dtls=1 serialize=0 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -C "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, server serializes, GCM" \
            "$P_SRV dtls=1 serialize=2 exchanges=2" \
            "$P_CLI dtls=1 serialize=0 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -C "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Context serialization, re-init, server serializes, with CID" \
            "$P_SRV dtls=1 serialize=2 exchanges=2 cid=1 cid_val=dead" \
            "$P_CLI dtls=1 serialize=0 exchanges=2 cid=1 cid_val=beef" \
            0 \
            -C "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, both serialize, CCM" \
            "$P_SRV dtls=1 serialize=2 exchanges=2" \
            "$P_CLI dtls=1 serialize=2 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, both serialize, ChaChaPoly" \
            "$P_SRV dtls=1 serialize=2 exchanges=2" \
            "$P_CLI dtls=1 serialize=2 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -c "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Context serialization, re-init, both serialize, GCM" \
            "$P_SRV dtls=1 serialize=2 exchanges=2" \
            "$P_CLI dtls=1 serialize=2 exchanges=2 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256" \
            0 \
            -c "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Context serialization, re-init, both serialize, with CID" \
            "$P_SRV dtls=1 serialize=2 exchanges=2 cid=1 cid_val=dead" \
            "$P_CLI dtls=1 serialize=2 exchanges=2 cid=1 cid_val=beef" \
            0 \
            -c "Deserializing connection..." \
            -s "Deserializing connection..."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CONTEXT_SERIALIZATION
run_test    "Saving the serialized context to a file" \
            "$P_SRV dtls=1 serialize=1 context_file=context_srv.txt" \
            "$P_CLI dtls=1 serialize=1 context_file=context_cli.txt" \
            0 \
            -s "Save serialized context to a file... ok" \
            -c "Save serialized context to a file... ok"

requires_config_enabled MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
requires_protocol_version tls12
run_test_export_keying_material tls12

requires_config_enabled MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
requires_protocol_version tls12
run_test_export_keying_material_openssl_compat tls12

requires_config_enabled MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
requires_protocol_version tls13
run_test_export_keying_material tls13

requires_config_enabled MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_openssl_tls1_3_with_compatible_ephemeral
run_test_export_keying_material_openssl_compat tls13

rm -f context_srv.txt
rm -f context_cli.txt

# Tests for DTLS Connection ID extension

# So far, the CID API isn't implemented, so we can't
# grep for output witnessing its use. This needs to be
# changed once the CID extension is implemented.

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli enabled, Srv disabled" \
            "$P_SRV debug_level=3 dtls=1 cid=0" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=deadbeef" \
            0 \
            -s "Disable use of CID extension." \
            -s "found CID extension"           \
            -s "Client sent CID extension, but CID disabled" \
            -c "Enable use of CID extension."  \
            -c "client hello, adding CID extension" \
            -S "server hello, adding CID extension" \
            -C "found CID extension" \
            -S "Copy CIDs into SSL transform" \
            -C "Copy CIDs into SSL transform" \
            -c "Use of Connection ID was rejected by the server"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli disabled, Srv enabled" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=deadbeef" \
            "$P_CLI debug_level=3 dtls=1 cid=0" \
            0 \
            -c "Disable use of CID extension." \
            -C "client hello, adding CID extension"           \
            -S "found CID extension"           \
            -s "Enable use of CID extension." \
            -S "server hello, adding CID extension" \
            -C "found CID extension" \
            -S "Copy CIDs into SSL transform" \
            -C "Copy CIDs into SSL transform"  \
            -s "Use of Connection ID was not offered by client"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli+Srv CID nonempty" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 2 Bytes): de ad" \
            -s "Peer CID (length 2 Bytes): be ef" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID, 3D: Cli+Srv enabled, Cli+Srv CID nonempty" \
            -p "$P_PXY drop=5 delay=5 duplicate=5 bad_cid=1" \
            "$P_SRV debug_level=3 dtls=1 cid=1 dgram_packing=0 cid_val=dead" \
            "$P_CLI debug_level=3 dtls=1 cid=1 dgram_packing=0 cid_val=beef" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 2 Bytes): de ad" \
            -s "Peer CID (length 2 Bytes): be ef" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated" \
            -c "ignoring unexpected CID" \
            -s "ignoring unexpected CID"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID, MTU: Cli+Srv enabled, Cli+Srv CID nonempty" \
            -p "$P_PXY mtu=800" \
            "$P_SRV debug_level=3 mtu=800 dtls=1 cid=1 cid_val=dead" \
            "$P_CLI debug_level=3 mtu=800 dtls=1 cid=1 cid_val=beef" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 2 Bytes): de ad" \
            -s "Peer CID (length 2 Bytes): be ef" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID, 3D+MTU: Cli+Srv enabled, Cli+Srv CID nonempty" \
            -p "$P_PXY mtu=800 drop=5 delay=5 duplicate=5 bad_cid=1" \
            "$P_SRV debug_level=3 mtu=800 dtls=1 cid=1 cid_val=dead" \
            "$P_CLI debug_level=3 mtu=800 dtls=1 cid=1 cid_val=beef" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 2 Bytes): de ad" \
            -s "Peer CID (length 2 Bytes): be ef" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated" \
            -c "ignoring unexpected CID" \
            -s "ignoring unexpected CID"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli CID empty" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=deadbeef" \
            "$P_CLI debug_level=3 dtls=1 cid=1" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 4 Bytes): de ad be ef" \
            -s "Peer CID (length 0 Bytes):" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Srv CID empty" \
            "$P_SRV debug_level=3 dtls=1 cid=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=deadbeef" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -s "Peer CID (length 4 Bytes): de ad be ef" \
            -c "Peer CID (length 0 Bytes):" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli+Srv CID empty" \
            "$P_SRV debug_level=3 dtls=1 cid=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -S "Use of Connection ID has been negotiated" \
            -C "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli+Srv CID nonempty, AES-128-CCM-8" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 2 Bytes): de ad" \
            -s "Peer CID (length 2 Bytes): be ef" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli CID empty, AES-128-CCM-8" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=deadbeef" \
            "$P_CLI debug_level=3 dtls=1 cid=1 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 4 Bytes): de ad be ef" \
            -s "Peer CID (length 0 Bytes):" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Srv CID empty, AES-128-CCM-8" \
            "$P_SRV debug_level=3 dtls=1 cid=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=deadbeef force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -s "Peer CID (length 4 Bytes): de ad be ef" \
            -c "Peer CID (length 0 Bytes):" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli+Srv CID empty, AES-128-CCM-8" \
            "$P_SRV debug_level=3 dtls=1 cid=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -S "Use of Connection ID has been negotiated" \
            -C "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli+Srv CID nonempty, AES-128-CBC" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 2 Bytes): de ad" \
            -s "Peer CID (length 2 Bytes): be ef" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli CID empty, AES-128-CBC" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=deadbeef" \
            "$P_CLI debug_level=3 dtls=1 cid=1 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 4 Bytes): de ad be ef" \
            -s "Peer CID (length 0 Bytes):" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Srv CID empty, AES-128-CBC" \
            "$P_SRV debug_level=3 dtls=1 cid=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=deadbeef force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -s "Peer CID (length 4 Bytes): de ad be ef" \
            -c "Peer CID (length 0 Bytes):" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Connection ID: Cli+Srv enabled, Cli+Srv CID empty, AES-128-CBC" \
            "$P_SRV debug_level=3 dtls=1 cid=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -S "Use of Connection ID has been negotiated" \
            -C "Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID: Cli+Srv enabled, renegotiate without change of CID" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -s "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -s "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID: Cli+Srv enabled, renegotiate with different CID" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead cid_val_renego=beef renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef cid_val_renego=dead renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -s "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -s "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID, no packing: Cli+Srv enabled, renegotiate with different CID" \
            "$P_SRV debug_level=3 dtls=1 cid=1 dgram_packing=0 cid_val=dead cid_val_renego=beef renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 dgram_packing=0 cid_val=beef cid_val_renego=dead renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -s "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -s "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID, 3D+MTU: Cli+Srv enabled, renegotiate with different CID" \
            -p "$P_PXY mtu=800 drop=5 delay=5 duplicate=5 bad_cid=1" \
            "$P_SRV debug_level=3 mtu=800 dtls=1 cid=1 cid_val=dead cid_val_renego=beef renegotiation=1" \
            "$P_CLI debug_level=3 mtu=800 dtls=1 cid=1 cid_val=beef cid_val_renego=dead renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -s "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -s "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "ignoring unexpected CID" \
            -s "ignoring unexpected CID"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID: Cli+Srv enabled, renegotiate without CID" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead cid_renego=0 renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef cid_renego=0 renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -S "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -C "(after renegotiation) Use of Connection ID has been negotiated" \
            -S "(after renegotiation) Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID, no packing: Cli+Srv enabled, renegotiate without CID" \
            "$P_SRV debug_level=3 dtls=1 dgram_packing=0 cid=1 cid_val=dead cid_renego=0 renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 dgram_packing=0 cid=1 cid_val=beef cid_renego=0 renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -S "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -C "(after renegotiation) Use of Connection ID has been negotiated" \
            -S "(after renegotiation) Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID, 3D+MTU: Cli+Srv enabled, renegotiate without CID" \
            -p "$P_PXY drop=5 delay=5 duplicate=5 bad_cid=1" \
            "$P_SRV debug_level=3 mtu=800 dtls=1 cid=1 cid_val=dead cid_renego=0 renegotiation=1" \
            "$P_CLI debug_level=3 mtu=800 dtls=1 cid=1 cid_val=beef cid_renego=0 renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -S "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -C "(after renegotiation) Use of Connection ID has been negotiated" \
            -S "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "ignoring unexpected CID" \
            -s "ignoring unexpected CID"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID: Cli+Srv enabled, CID on renegotiation" \
            "$P_SRV debug_level=3 dtls=1 cid=0 cid_renego=1 cid_val_renego=dead renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=0 cid_renego=1 cid_val_renego=beef renegotiation=1 renegotiate=1" \
            0 \
            -S "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -s "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -c "(after renegotiation) Use of Connection ID has been negotiated" \
            -s "(after renegotiation) Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID, no packing: Cli+Srv enabled, CID on renegotiation" \
            "$P_SRV debug_level=3 dtls=1 dgram_packing=0 cid=0 cid_renego=1 cid_val_renego=dead renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 dgram_packing=0 cid=0 cid_renego=1 cid_val_renego=beef renegotiation=1 renegotiate=1" \
            0 \
            -S "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -s "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -c "(after renegotiation) Use of Connection ID has been negotiated" \
            -s "(after renegotiation) Use of Connection ID has been negotiated"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID, 3D+MTU: Cli+Srv enabled, CID on renegotiation" \
            -p "$P_PXY mtu=800 drop=5 delay=5 duplicate=5 bad_cid=1" \
            "$P_SRV debug_level=3 mtu=800 dtls=1 dgram_packing=1 cid=0 cid_renego=1 cid_val_renego=dead renegotiation=1" \
            "$P_CLI debug_level=3 mtu=800 dtls=1 dgram_packing=1 cid=0 cid_renego=1 cid_val_renego=beef renegotiation=1 renegotiate=1" \
            0 \
            -S "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -s "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -c "(after renegotiation) Use of Connection ID has been negotiated" \
            -s "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "ignoring unexpected CID" \
            -s "ignoring unexpected CID"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID: Cli+Srv enabled, Cli disables on renegotiation" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef cid_renego=0 renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -S "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -C "(after renegotiation) Use of Connection ID has been negotiated" \
            -S "(after renegotiation) Use of Connection ID has been negotiated" \
            -s "(after renegotiation) Use of Connection ID was not offered by client"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID, 3D: Cli+Srv enabled, Cli disables on renegotiation" \
            -p "$P_PXY drop=5 delay=5 duplicate=5 bad_cid=1" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef cid_renego=0 renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -S "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -C "(after renegotiation) Use of Connection ID has been negotiated" \
            -S "(after renegotiation) Use of Connection ID has been negotiated" \
            -s "(after renegotiation) Use of Connection ID was not offered by client" \
            -c "ignoring unexpected CID" \
            -s "ignoring unexpected CID"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID: Cli+Srv enabled, Srv disables on renegotiation" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead cid_renego=0 renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -S "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -C "(after renegotiation) Use of Connection ID has been negotiated" \
            -S "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Use of Connection ID was rejected by the server"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Connection ID, 3D: Cli+Srv enabled, Srv disables on renegotiation" \
            -p "$P_PXY drop=5 delay=5 duplicate=5 bad_cid=1" \
            "$P_SRV debug_level=3 dtls=1 cid=1 cid_val=dead cid_renego=0 renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 cid=1 cid_val=beef renegotiation=1 renegotiate=1" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -C "(after renegotiation) Peer CID (length 2 Bytes): de ad" \
            -S "(after renegotiation) Peer CID (length 2 Bytes): be ef" \
            -C "(after renegotiation) Use of Connection ID has been negotiated" \
            -S "(after renegotiation) Use of Connection ID has been negotiated" \
            -c "(after renegotiation) Use of Connection ID was rejected by the server" \
            -c "ignoring unexpected CID" \
            -s "ignoring unexpected CID"

# This and the test below it require MAX_CONTENT_LEN to be at least MFL+1, because the
# tests check that the buffer contents are reallocated when the message is
# larger than the buffer.
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
requires_max_content_len 513
run_test    "Connection ID: Cli+Srv enabled, variable buffer lengths, MFL=512" \
            "$P_SRV dtls=1 cid=1 cid_val=dead debug_level=2" \
            "$P_CLI force_ciphersuite="TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" max_frag_len=512 dtls=1 cid=1 cid_val=beef" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -s "Reallocating in_buf" \
            -s "Reallocating out_buf"

requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
requires_config_enabled MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
requires_max_content_len 1025
run_test    "Connection ID: Cli+Srv enabled, variable buffer lengths, MFL=1024" \
            "$P_SRV dtls=1 cid=1 cid_val=dead debug_level=2" \
            "$P_CLI force_ciphersuite="TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" max_frag_len=1024 dtls=1 cid=1 cid_val=beef" \
            0 \
            -c "(initial handshake) Peer CID (length 2 Bytes): de ad" \
            -s "(initial handshake) Peer CID (length 2 Bytes): be ef" \
            -s "(initial handshake) Use of Connection ID has been negotiated" \
            -c "(initial handshake) Use of Connection ID has been negotiated" \
            -s "Reallocating in_buf" \
            -s "Reallocating out_buf"

# Tests for Encrypt-then-MAC extension

run_test    "Encrypt then MAC: default" \
            "$P_SRV debug_level=3 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            "$P_CLI debug_level=3" \
            0 \
            -c "client hello, adding encrypt_then_mac extension" \
            -s "found encrypt then mac extension" \
            -s "server hello, adding encrypt then mac extension" \
            -c "found encrypt_then_mac extension" \
            -c "using encrypt then mac" \
            -s "using encrypt then mac"

run_test    "Encrypt then MAC: client enabled, server disabled" \
            "$P_SRV debug_level=3 etm=0 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            "$P_CLI debug_level=3 etm=1" \
            0 \
            -c "client hello, adding encrypt_then_mac extension" \
            -s "found encrypt then mac extension" \
            -S "server hello, adding encrypt then mac extension" \
            -C "found encrypt_then_mac extension" \
            -C "using encrypt then mac" \
            -S "using encrypt then mac"

run_test    "Encrypt then MAC: client enabled, aead cipher" \
            "$P_SRV debug_level=3 etm=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256" \
            "$P_CLI debug_level=3 etm=1" \
            0 \
            -c "client hello, adding encrypt_then_mac extension" \
            -s "found encrypt then mac extension" \
            -S "server hello, adding encrypt then mac extension" \
            -C "found encrypt_then_mac extension" \
            -C "using encrypt then mac" \
            -S "using encrypt then mac"

run_test    "Encrypt then MAC: client disabled, server enabled" \
            "$P_SRV debug_level=3 etm=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            "$P_CLI debug_level=3 etm=0" \
            0 \
            -C "client hello, adding encrypt_then_mac extension" \
            -S "found encrypt then mac extension" \
            -S "server hello, adding encrypt then mac extension" \
            -C "found encrypt_then_mac extension" \
            -C "using encrypt then mac" \
            -S "using encrypt then mac"

# Tests for Extended Master Secret extension

requires_config_enabled MBEDTLS_SSL_EXTENDED_MASTER_SECRET
run_test    "Extended Master Secret: default" \
            "$P_SRV debug_level=3" \
            "$P_CLI force_version=tls12 debug_level=3" \
            0 \
            -c "client hello, adding extended_master_secret extension" \
            -s "found extended master secret extension" \
            -s "server hello, adding extended master secret extension" \
            -c "found extended_master_secret extension" \
            -c "session hash for extended master secret" \
            -s "session hash for extended master secret"

requires_config_enabled MBEDTLS_SSL_EXTENDED_MASTER_SECRET
run_test    "Extended Master Secret: client enabled, server disabled" \
            "$P_SRV debug_level=3 extended_ms=0" \
            "$P_CLI force_version=tls12 debug_level=3 extended_ms=1" \
            0 \
            -c "client hello, adding extended_master_secret extension" \
            -s "found extended master secret extension" \
            -S "server hello, adding extended master secret extension" \
            -C "found extended_master_secret extension" \
            -C "session hash for extended master secret" \
            -S "session hash for extended master secret"

requires_config_enabled MBEDTLS_SSL_EXTENDED_MASTER_SECRET
run_test    "Extended Master Secret: client disabled, server enabled" \
            "$P_SRV force_version=tls12 debug_level=3 extended_ms=1" \
            "$P_CLI debug_level=3 extended_ms=0" \
            0 \
            -C "client hello, adding extended_master_secret extension" \
            -S "found extended master secret extension" \
            -S "server hello, adding extended master secret extension" \
            -C "found extended_master_secret extension" \
            -C "session hash for extended master secret" \
            -S "session hash for extended master secret"

# Test sending and receiving empty application data records

run_test    "Encrypt then MAC: empty application data record" \
            "$P_SRV auth_mode=none debug_level=4 etm=1" \
            "$P_CLI auth_mode=none etm=1 request_size=0 force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -S "0000:  0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f" \
            -s "dumping 'input payload after decrypt' (0 bytes)" \
            -c "0 bytes written in 1 fragments"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Encrypt then MAC: disabled, empty application data record" \
            "$P_SRV auth_mode=none debug_level=4 etm=0" \
            "$P_CLI auth_mode=none etm=0 request_size=0" \
            0 \
            -s "dumping 'input payload after decrypt' (0 bytes)" \
            -c "0 bytes written in 1 fragments"

run_test    "Encrypt then MAC, DTLS: empty application data record" \
            "$P_SRV auth_mode=none debug_level=4 etm=1 dtls=1" \
            "$P_CLI auth_mode=none etm=1 request_size=0 force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA dtls=1" \
            0 \
            -S "0000:  0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f 0f" \
            -s "dumping 'input payload after decrypt' (0 bytes)" \
            -c "0 bytes written in 1 fragments"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Encrypt then MAC, DTLS: disabled, empty application data record" \
            "$P_SRV auth_mode=none debug_level=4 etm=0 dtls=1" \
            "$P_CLI auth_mode=none etm=0 request_size=0 dtls=1" \
            0 \
            -s "dumping 'input payload after decrypt' (0 bytes)" \
            -c "0 bytes written in 1 fragments"

# Tests for CBC 1/n-1 record splitting

run_test    "CBC Record splitting: TLS 1.2, no splitting" \
            "$P_SRV force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA \
             request_size=123" \
            0 \
            -s "Read from client: 123 bytes read" \
            -S "Read from client: 1 bytes read" \
            -S "122 bytes read"

# Tests for Session Tickets

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: basic" \
            "$P_SRV debug_level=3 tickets=1" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: manual rotation" \
            "$P_SRV debug_level=3 tickets=1 ticket_rotate=1" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: cache disabled" \
            "$P_SRV debug_level=3 tickets=1 cache_max=0" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: timeout" \
            "$P_SRV debug_level=3 tickets=1 cache_max=0 ticket_timeout=1" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1 reco_delay=2000" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -S "a session has been resumed" \
            -C "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: session copy" \
            "$P_SRV debug_level=3 tickets=1 cache_max=0" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1 reco_mode=0" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: openssl server" \
            "$O_SRV -tls1_2" \
            "$P_CLI debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: openssl client" \
            "$P_SRV force_version=tls12 debug_level=3 tickets=1" \
            "( $O_CLI -sess_out $SESSION; \
               $O_CLI -sess_in $SESSION; \
               rm -f $SESSION )" \
            0 \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed"

requires_cipher_enabled "AES" "GCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: AES-128-GCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=AES-128-GCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "AES" "GCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: AES-192-GCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=AES-192-GCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "AES" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: AES-128-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=AES-128-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "AES" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: AES-192-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=AES-192-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "AES" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: AES-256-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=AES-256-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "CAMELLIA" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: CAMELLIA-128-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=CAMELLIA-128-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "CAMELLIA" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: CAMELLIA-192-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=CAMELLIA-192-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "CAMELLIA" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: CAMELLIA-256-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=CAMELLIA-256-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "ARIA" "GCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: ARIA-128-GCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=ARIA-128-GCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "ARIA" "GCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: ARIA-192-GCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=ARIA-192-GCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "ARIA" "GCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: ARIA-256-GCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=ARIA-256-GCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "ARIA" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: ARIA-128-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=ARIA-128-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "ARIA" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: ARIA-192-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=ARIA-192-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "ARIA" "CCM"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: ARIA-256-CCM" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=ARIA-256-CCM" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_cipher_enabled "CHACHA20"
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets: CHACHA20-POLY1305" \
            "$P_SRV debug_level=3 tickets=1 ticket_aead=CHACHA20-POLY1305" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

# Tests for Session Tickets with DTLS

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets, DTLS: basic" \
            "$P_SRV debug_level=3 dtls=1 tickets=1" \
            "$P_CLI debug_level=3 dtls=1 tickets=1 reconnect=1 skip_close_notify=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets, DTLS: cache disabled" \
            "$P_SRV debug_level=3 dtls=1 tickets=1 cache_max=0" \
            "$P_CLI debug_level=3 dtls=1 tickets=1 reconnect=1 skip_close_notify=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets, DTLS: timeout" \
            "$P_SRV debug_level=3 dtls=1 tickets=1 cache_max=0 ticket_timeout=1" \
            "$P_CLI debug_level=3 dtls=1 tickets=1 reconnect=1 skip_close_notify=1 reco_delay=2000" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -S "a session has been resumed" \
            -C "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets, DTLS: session copy" \
            "$P_SRV debug_level=3 dtls=1 tickets=1 cache_max=0" \
            "$P_CLI debug_level=3 dtls=1 tickets=1 reconnect=1 skip_close_notify=1 reco_mode=0" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets, DTLS: openssl server" \
            "$O_SRV -dtls" \
            "$P_CLI dtls=1 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -c "found session_ticket extension" \
            -c "parse new session ticket" \
            -c "a session has been resumed"

# For reasons that aren't fully understood, this test randomly fails with high
# probability with OpenSSL 1.0.2g on the CI, see #5012.
requires_openssl_next
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using tickets, DTLS: openssl client" \
            "$P_SRV dtls=1 debug_level=3 tickets=1" \
            "( $O_NEXT_CLI -dtls -sess_out $SESSION; \
               $O_NEXT_CLI -dtls -sess_in $SESSION; \
               rm -f $SESSION )" \
            0 \
            -s "found session ticket extension" \
            -s "server hello, adding session ticket extension" \
            -S "session successfully restored from cache" \
            -s "session successfully restored from ticket" \
            -s "a session has been resumed"

# Tests for Session Resume based on session-ID and cache

requires_config_enabled MBEDTLS_SSL_CACHE_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using cache: tickets enabled on client" \
            "$P_SRV debug_level=3 tickets=0" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=1 reconnect=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -S "server hello, adding session ticket extension" \
            -C "found session_ticket extension" \
            -C "parse new session ticket" \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_CACHE_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using cache: tickets enabled on server" \
            "$P_SRV debug_level=3 tickets=1" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=0 reconnect=1" \
            0 \
            -C "client hello, adding session ticket extension" \
            -S "found session ticket extension" \
            -S "server hello, adding session ticket extension" \
            -C "found session_ticket extension" \
            -C "parse new session ticket" \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache: cache_max=0" \
            "$P_SRV debug_level=3 tickets=0 cache_max=0" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=0 reconnect=1" \
            0 \
            -S "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -S "a session has been resumed" \
            -C "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache: cache_max=1" \
            "$P_SRV debug_level=3 tickets=0 cache_max=1" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=0 reconnect=1" \
            0 \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache: cache removed" \
            "$P_SRV debug_level=3 tickets=0 cache_remove=1" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=0 reconnect=1" \
            0 \
            -C "client hello, adding session ticket extension" \
            -S "found session ticket extension" \
            -S "server hello, adding session ticket extension" \
            -C "found session_ticket extension" \
            -C "parse new session ticket" \
            -S "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -S "a session has been resumed" \
            -C "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache: timeout > delay" \
            "$P_SRV debug_level=3 tickets=0" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=0 reconnect=1 reco_delay=0" \
            0 \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache: timeout < delay" \
            "$P_SRV debug_level=3 tickets=0 cache_timeout=1" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=0 reconnect=1 reco_delay=2000" \
            0 \
            -S "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -S "a session has been resumed" \
            -C "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache: no timeout" \
            "$P_SRV debug_level=3 tickets=0 cache_timeout=0" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=0 reconnect=1 reco_delay=2000" \
            0 \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache: session copy" \
            "$P_SRV debug_level=3 tickets=0" \
            "$P_CLI force_version=tls12 debug_level=3 tickets=0 reconnect=1 reco_mode=0" \
            0 \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_CACHE_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using cache: openssl client" \
            "$P_SRV force_version=tls12 debug_level=3 tickets=0" \
            "( $O_CLI -sess_out $SESSION; \
               $O_CLI -sess_in $SESSION; \
               rm -f $SESSION )" \
            0 \
            -s "found session ticket extension" \
            -S "server hello, adding session ticket extension" \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache: openssl server" \
            "$O_SRV -tls1_2" \
            "$P_CLI debug_level=3 tickets=0 reconnect=1" \
            0 \
            -C "found session_ticket extension" \
            -C "parse new session ticket" \
            -c "a session has been resumed"

# Tests for Session resume and extensions

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_DTLS_CONNECTION_ID
run_test    "Session resume and connection ID" \
            "$P_SRV debug_level=3 cid=1 cid_val=dead dtls=1 tickets=0" \
            "$P_CLI debug_level=3 cid=1 cid_val=beef dtls=1 tickets=0 reconnect=1" \
            0 \
            -c "Enable use of CID extension." \
            -s "Enable use of CID extension." \
            -c "client hello, adding CID extension" \
            -s "found CID extension"           \
            -s "Use of CID extension negotiated" \
            -s "server hello, adding CID extension" \
            -c "found CID extension" \
            -c "Use of CID extension negotiated" \
            -s "Copy CIDs into SSL transform" \
            -c "Copy CIDs into SSL transform" \
            -c "Peer CID (length 2 Bytes): de ad" \
            -s "Peer CID (length 2 Bytes): be ef" \
            -s "Use of Connection ID has been negotiated" \
            -c "Use of Connection ID has been negotiated"

# Tests for Session Resume based on session-ID and cache, DTLS

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using cache, DTLS: tickets enabled on client" \
            "$P_SRV dtls=1 debug_level=3 tickets=0" \
            "$P_CLI dtls=1 debug_level=3 tickets=1 reconnect=1 skip_close_notify=1" \
            0 \
            -c "client hello, adding session ticket extension" \
            -s "found session ticket extension" \
            -S "server hello, adding session ticket extension" \
            -C "found session_ticket extension" \
            -C "parse new session ticket" \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using cache, DTLS: tickets enabled on server" \
            "$P_SRV dtls=1 debug_level=3 tickets=1" \
            "$P_CLI dtls=1 debug_level=3 tickets=0 reconnect=1 skip_close_notify=1" \
            0 \
            -C "client hello, adding session ticket extension" \
            -S "found session ticket extension" \
            -S "server hello, adding session ticket extension" \
            -C "found session_ticket extension" \
            -C "parse new session ticket" \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache, DTLS: cache_max=0" \
            "$P_SRV dtls=1 debug_level=3 tickets=0 cache_max=0" \
            "$P_CLI dtls=1 debug_level=3 tickets=0 reconnect=1 skip_close_notify=1" \
            0 \
            -S "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -S "a session has been resumed" \
            -C "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache, DTLS: cache_max=1" \
            "$P_SRV dtls=1 debug_level=3 tickets=0 cache_max=1" \
            "$P_CLI dtls=1 debug_level=3 tickets=0 reconnect=1 skip_close_notify=1" \
            0 \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache, DTLS: timeout > delay" \
            "$P_SRV dtls=1 debug_level=3 tickets=0" \
            "$P_CLI dtls=1 debug_level=3 tickets=0 reconnect=1 skip_close_notify=1 reco_delay=0" \
            0 \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache, DTLS: timeout < delay" \
            "$P_SRV dtls=1 debug_level=3 tickets=0 cache_timeout=1" \
            "$P_CLI dtls=1 debug_level=3 tickets=0 reconnect=1 skip_close_notify=1 reco_delay=2000" \
            0 \
            -S "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -S "a session has been resumed" \
            -C "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache, DTLS: no timeout" \
            "$P_SRV dtls=1 debug_level=3 tickets=0 cache_timeout=0" \
            "$P_CLI dtls=1 debug_level=3 tickets=0 reconnect=1 skip_close_notify=1 reco_delay=2000" \
            0 \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache, DTLS: session copy" \
            "$P_SRV dtls=1 debug_level=3 tickets=0" \
            "$P_CLI dtls=1 debug_level=3 tickets=0 reconnect=1 skip_close_notify=1 reco_mode=0" \
            0 \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed" \
            -c "a session has been resumed"

# For reasons that aren't fully understood, this test randomly fails with high
# probability with OpenSSL 1.0.2g on the CI, see #5012.
requires_openssl_next
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Session resume using cache, DTLS: openssl client" \
            "$P_SRV dtls=1 debug_level=3 tickets=0" \
            "( $O_NEXT_CLI -dtls -sess_out $SESSION; \
               $O_NEXT_CLI -dtls -sess_in $SESSION; \
               rm -f $SESSION )" \
            0 \
            -s "found session ticket extension" \
            -S "server hello, adding session ticket extension" \
            -s "session successfully restored from cache" \
            -S "session successfully restored from ticket" \
            -s "a session has been resumed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "Session resume using cache, DTLS: openssl server" \
            "$O_SRV -dtls" \
            "$P_CLI dtls=1 debug_level=3 tickets=0 reconnect=1" \
            0 \
            -C "found session_ticket extension" \
            -C "parse new session ticket" \
            -c "a session has been resumed"

# Tests for Max Fragment Length extension

requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Max fragment length: enabled, default" \
            "$P_SRV debug_level=3 force_version=tls12" \
            "$P_CLI debug_level=3" \
            0 \
            -c "Maximum incoming record payload length is $MAX_CONTENT_LEN" \
            -c "Maximum outgoing record payload length is $MAX_CONTENT_LEN" \
            -s "Maximum incoming record payload length is $MAX_CONTENT_LEN" \
            -s "Maximum outgoing record payload length is $MAX_CONTENT_LEN" \
            -C "client hello, adding max_fragment_length extension" \
            -S "found max fragment length extension" \
            -S "server hello, max_fragment_length extension" \
            -C "found max_fragment_length extension"

requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Max fragment length: enabled, default, larger message" \
            "$P_SRV debug_level=3 force_version=tls12" \
            "$P_CLI debug_level=3 request_size=$(( $MAX_CONTENT_LEN + 1))" \
            0 \
            -c "Maximum incoming record payload length is $MAX_CONTENT_LEN" \
            -c "Maximum outgoing record payload length is $MAX_CONTENT_LEN" \
            -s "Maximum incoming record payload length is $MAX_CONTENT_LEN" \
            -s "Maximum outgoing record payload length is $MAX_CONTENT_LEN" \
            -C "client hello, adding max_fragment_length extension" \
            -S "found max fragment length extension" \
            -S "server hello, max_fragment_length extension" \
            -C "found max_fragment_length extension" \
            -c "$(( $MAX_CONTENT_LEN + 1)) bytes written in 2 fragments" \
            -s "$MAX_CONTENT_LEN bytes read" \
            -s "1 bytes read"

requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Max fragment length, DTLS: enabled, default, larger message" \
            "$P_SRV debug_level=3 dtls=1" \
            "$P_CLI debug_level=3 dtls=1 request_size=$(( $MAX_CONTENT_LEN + 1))" \
            1 \
            -c "Maximum incoming record payload length is $MAX_CONTENT_LEN" \
            -c "Maximum outgoing record payload length is $MAX_CONTENT_LEN" \
            -s "Maximum incoming record payload length is $MAX_CONTENT_LEN" \
            -s "Maximum outgoing record payload length is $MAX_CONTENT_LEN" \
            -C "client hello, adding max_fragment_length extension" \
            -S "found max fragment length extension" \
            -S "server hello, max_fragment_length extension" \
            -C "found max_fragment_length extension" \
            -c "fragment larger than.*maximum "

# Run some tests with MBEDTLS_SSL_MAX_FRAGMENT_LENGTH disabled
# (session fragment length will be 16384 regardless of mbedtls
# content length configuration.)

requires_config_disabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Max fragment length: disabled, larger message" \
            "$P_SRV debug_level=3 force_version=tls12" \
            "$P_CLI debug_level=3 request_size=$(( $MAX_CONTENT_LEN + 1))" \
            0 \
            -C "Maximum incoming record payload length is 16384" \
            -C "Maximum outgoing record payload length is 16384" \
            -S "Maximum incoming record payload length is 16384" \
            -S "Maximum outgoing record payload length is 16384" \
            -c "$(( $MAX_CONTENT_LEN + 1)) bytes written in 2 fragments" \
            -s "$MAX_CONTENT_LEN bytes read" \
            -s "1 bytes read"

requires_config_disabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Max fragment length, DTLS: disabled, larger message" \
            "$P_SRV debug_level=3 dtls=1 force_version=tls12" \
            "$P_CLI debug_level=3 dtls=1 request_size=$(( $MAX_CONTENT_LEN + 1))" \
            1 \
            -C "Maximum incoming record payload length is 16384" \
            -C "Maximum outgoing record payload length is 16384" \
            -S "Maximum incoming record payload length is 16384" \
            -S "Maximum outgoing record payload length is 16384" \
            -c "fragment larger than.*maximum "

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: used by client" \
            "$P_SRV debug_level=3" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=4096" \
            0 \
            -c "Maximum incoming record payload length is 4096" \
            -c "Maximum outgoing record payload length is 4096" \
            -s "Maximum incoming record payload length is 4096" \
            -s "Maximum outgoing record payload length is 4096" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 1024
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 512, server 1024" \
            "$P_SRV debug_level=3 max_frag_len=1024" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=512" \
            0 \
            -c "Maximum incoming record payload length is 512" \
            -c "Maximum outgoing record payload length is 512" \
            -s "Maximum incoming record payload length is 512" \
            -s "Maximum outgoing record payload length is 512" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 512, server 2048" \
            "$P_SRV debug_level=3 max_frag_len=2048" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=512" \
            0 \
            -c "Maximum incoming record payload length is 512" \
            -c "Maximum outgoing record payload length is 512" \
            -s "Maximum incoming record payload length is 512" \
            -s "Maximum outgoing record payload length is 512" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 512, server 4096" \
            "$P_SRV debug_level=3 max_frag_len=4096" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=512" \
            0 \
            -c "Maximum incoming record payload length is 512" \
            -c "Maximum outgoing record payload length is 512" \
            -s "Maximum incoming record payload length is 512" \
            -s "Maximum outgoing record payload length is 512" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 1024
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 1024, server 512" \
            "$P_SRV force_version=tls12 debug_level=3 max_frag_len=512" \
            "$P_CLI debug_level=3 max_frag_len=1024" \
            0 \
            -c "Maximum incoming record payload length is 1024" \
            -c "Maximum outgoing record payload length is 1024" \
            -s "Maximum incoming record payload length is 1024" \
            -s "Maximum outgoing record payload length is 512" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 1024, server 2048" \
            "$P_SRV debug_level=3 max_frag_len=2048" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=1024" \
            0 \
            -c "Maximum incoming record payload length is 1024" \
            -c "Maximum outgoing record payload length is 1024" \
            -s "Maximum incoming record payload length is 1024" \
            -s "Maximum outgoing record payload length is 1024" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 1024, server 4096" \
            "$P_SRV debug_level=3 max_frag_len=4096" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=1024" \
            0 \
            -c "Maximum incoming record payload length is 1024" \
            -c "Maximum outgoing record payload length is 1024" \
            -s "Maximum incoming record payload length is 1024" \
            -s "Maximum outgoing record payload length is 1024" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 2048, server 512" \
            "$P_SRV force_version=tls12 debug_level=3 max_frag_len=512" \
            "$P_CLI debug_level=3 max_frag_len=2048" \
            0 \
            -c "Maximum incoming record payload length is 2048" \
            -c "Maximum outgoing record payload length is 2048" \
            -s "Maximum incoming record payload length is 2048" \
            -s "Maximum outgoing record payload length is 512" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 2048, server 1024" \
            "$P_SRV force_version=tls12 debug_level=3 max_frag_len=1024" \
            "$P_CLI debug_level=3 max_frag_len=2048" \
            0 \
            -c "Maximum incoming record payload length is 2048" \
            -c "Maximum outgoing record payload length is 2048" \
            -s "Maximum incoming record payload length is 2048" \
            -s "Maximum outgoing record payload length is 1024" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 2048, server 4096" \
            "$P_SRV debug_level=3 max_frag_len=4096" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=2048" \
            0 \
            -c "Maximum incoming record payload length is 2048" \
            -c "Maximum outgoing record payload length is 2048" \
            -s "Maximum incoming record payload length is 2048" \
            -s "Maximum outgoing record payload length is 2048" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 4096, server 512" \
            "$P_SRV force_version=tls12 debug_level=3 max_frag_len=512" \
            "$P_CLI debug_level=3 max_frag_len=4096" \
            0 \
            -c "Maximum incoming record payload length is 4096" \
            -c "Maximum outgoing record payload length is 4096" \
            -s "Maximum incoming record payload length is 4096" \
            -s "Maximum outgoing record payload length is 512" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 4096, server 1024" \
            "$P_SRV force_version=tls12 debug_level=3 max_frag_len=1024" \
            "$P_CLI debug_level=3 max_frag_len=4096" \
            0 \
            -c "Maximum incoming record payload length is 4096" \
            -c "Maximum outgoing record payload length is 4096" \
            -s "Maximum incoming record payload length is 4096" \
            -s "Maximum outgoing record payload length is 1024" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client 4096, server 2048" \
            "$P_SRV force_version=tls12 debug_level=3 max_frag_len=2048" \
            "$P_CLI debug_level=3 max_frag_len=4096" \
            0 \
            -c "Maximum incoming record payload length is 4096" \
            -c "Maximum outgoing record payload length is 4096" \
            -s "Maximum incoming record payload length is 4096" \
            -s "Maximum outgoing record payload length is 2048" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: used by server" \
            "$P_SRV force_version=tls12 debug_level=3 max_frag_len=4096" \
            "$P_CLI debug_level=3" \
            0 \
            -c "Maximum incoming record payload length is $MAX_CONTENT_LEN" \
            -c "Maximum outgoing record payload length is $MAX_CONTENT_LEN" \
            -s "Maximum incoming record payload length is $MAX_CONTENT_LEN" \
            -s "Maximum outgoing record payload length is 4096" \
            -C "client hello, adding max_fragment_length extension" \
            -S "found max fragment length extension" \
            -S "server hello, max_fragment_length extension" \
            -C "found max_fragment_length extension"

requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Max fragment length: gnutls server" \
            "$G_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2" \
            "$P_CLI debug_level=3 max_frag_len=4096" \
            0 \
            -c "Maximum incoming record payload length is 4096" \
            -c "Maximum outgoing record payload length is 4096" \
            -c "client hello, adding max_fragment_length extension" \
            -c "found max_fragment_length extension"

requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client, message just fits" \
            "$P_SRV debug_level=3" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=2048 request_size=2048" \
            0 \
            -c "Maximum incoming record payload length is 2048" \
            -c "Maximum outgoing record payload length is 2048" \
            -s "Maximum incoming record payload length is 2048" \
            -s "Maximum outgoing record payload length is 2048" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension" \
            -c "2048 bytes written in 1 fragments" \
            -s "2048 bytes read"

requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
run_test    "Max fragment length: client, larger message" \
            "$P_SRV debug_level=3" \
            "$P_CLI force_version=tls12 debug_level=3 max_frag_len=2048 request_size=2345" \
            0 \
            -c "Maximum incoming record payload length is 2048" \
            -c "Maximum outgoing record payload length is 2048" \
            -s "Maximum incoming record payload length is 2048" \
            -s "Maximum outgoing record payload length is 2048" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension" \
            -c "2345 bytes written in 2 fragments" \
            -s "2048 bytes read" \
            -s "297 bytes read"

requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Max fragment length: DTLS client, larger message" \
            "$P_SRV debug_level=3 dtls=1" \
            "$P_CLI debug_level=3 dtls=1 max_frag_len=2048 request_size=2345" \
            1 \
            -c "Maximum incoming record payload length is 2048" \
            -c "Maximum outgoing record payload length is 2048" \
            -s "Maximum incoming record payload length is 2048" \
            -s "Maximum outgoing record payload length is 2048" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension" \
            -c "fragment larger than.*maximum"

# Tests for Record Size Limit extension

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server-side parsing and debug output" \
            "$P_SRV debug_level=3 force_version=tls13" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -d 4" \
            0 \
            -s "RecordSizeLimit: 16385 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 16383" \
            -s "bytes written in 1 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client-side parsing and debug output" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL --disable-client-cert -d 4" \
            "$P_CLI debug_level=4 force_version=tls13" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes"                                      \
            -c "ClientHello: record_size_limit(28) extension exists."                   \
            -c "EncryptedExtensions: record_size_limit(28) extension received."         \
            -c "RecordSizeLimit: 16385 Bytes"                                           \

# In the following tests, --recordsize is the value used by the G_NEXT_CLI (3.7.2) to configure the
# maximum record size using gnutls_record_set_max_size()
# (https://gnutls.org/reference/gnutls-gnutls.html#gnutls-record-set-max-size).
# There is currently a lower limit of 512, caused by gnutls_record_set_max_size()
# not respecting the "%ALLOW_SMALL_RECORDS" priority string and not using the
# more recent function gnutls_record_set_max_recv_size()
# (https://gnutls.org/reference/gnutls-gnutls.html#gnutls-record-set-max-recv-size).
# There is currently an upper limit of 4096, caused by the cli arg parser:
# https://gitlab.com/gnutls/gnutls/-/blob/3.7.2/src/cli-args.def#L395.
# Thus, these tests are currently limited to the value range 512-4096.
# Also, the value sent in the extension will be one larger than the value
# set at the command line:
# https://gitlab.com/gnutls/gnutls/-/blob/3.7.2/lib/ext/record_size_limit.c#L142

# Currently test certificates being used do not fit in 513 record size limit
# so for 513 record size limit tests we use preshared key to avoid sending
# the certificate.

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (513), 1 fragment" \
            "$P_SRV debug_level=3 force_version=tls13 tls13_kex_modes=psk \
                    psk_list=Client_identity,6162636465666768696a6b6c6d6e6f70 \
                    response_size=256" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+PSK --recordsize 512 \
                    --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70" \
            0 \
            -s "RecordSizeLimit: 513 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 511" \
            -s "256 bytes written in 1 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (513), 2 fragments" \
            "$P_SRV debug_level=3 force_version=tls13 tls13_kex_modes=psk \
                    psk_list=Client_identity,6162636465666768696a6b6c6d6e6f70 \
                    response_size=768" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+PSK --recordsize 512 \
                         --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70" \
            0 \
            -s "RecordSizeLimit: 513 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 511" \
            -s "768 bytes written in 2 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (513), 3 fragments" \
            "$P_SRV debug_level=3 force_version=tls13 tls13_kex_modes=psk \
                    psk_list=Client_identity,6162636465666768696a6b6c6d6e6f70 \
                    response_size=1280" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+PSK --recordsize 512 \
                         --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70" \
            0 \
            -s "RecordSizeLimit: 513 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 511" \
            -s "1280 bytes written in 3 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (1024), 1 fragment" \
            "$P_SRV debug_level=3 force_version=tls13 response_size=512" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -d 4 --recordsize 1023" \
            0 \
            -s "RecordSizeLimit: 1024 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 1023" \
            -s "512 bytes written in 1 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (1024), 2 fragments" \
            "$P_SRV debug_level=3 force_version=tls13 response_size=1536" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -d 4 --recordsize 1023" \
            0 \
            -s "RecordSizeLimit: 1024 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 1023" \
            -s "1536 bytes written in 2 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (1024), 3 fragments" \
            "$P_SRV debug_level=3 force_version=tls13 response_size=2560" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -d 4 --recordsize 1023" \
            0 \
            -s "RecordSizeLimit: 1024 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 1023" \
            -s "2560 bytes written in 3 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (4096), 1 fragment" \
            "$P_SRV debug_level=3 force_version=tls13 response_size=2048" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -d 4 --recordsize 4095" \
            0 \
            -s "RecordSizeLimit: 4096 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 4095" \
            -s "2048 bytes written in 1 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (4096), 2 fragments" \
            "$P_SRV debug_level=3 force_version=tls13 response_size=6144" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -d 4 --recordsize 4095" \
            0 \
            -s "RecordSizeLimit: 4096 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 4095" \
            -s "6144 bytes written in 2 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Server complies with record size limit (4096), 3 fragments" \
            "$P_SRV debug_level=3 force_version=tls13 response_size=10240" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -d 4 --recordsize 4095" \
            0 \
            -s "RecordSizeLimit: 4096 Bytes" \
            -s "ClientHello: record_size_limit(28) extension exists." \
            -s "Sent RecordSizeLimit: 16384 Bytes" \
            -s "EncryptedExtensions: record_size_limit(28) extension exists." \
            -s "Maximum outgoing record payload length is 4095" \
            -s "10240 bytes written in 3 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (513), 1 fragment" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --disable-client-cert --recordsize 512" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=256" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 513 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 511" \
            -c "256 bytes written in 1 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (513), 2 fragments" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --disable-client-cert --recordsize 512" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=768" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 513 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 511" \
            -c "768 bytes written in 2 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (513), 3 fragments" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --disable-client-cert --recordsize 512" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=1280" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 513 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 511" \
            -c "1280 bytes written in 3 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (1024), 1 fragment" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --recordsize 1023" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=512" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 1024 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 1023" \
            -c "512 bytes written in 1 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (1024), 2 fragments" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --recordsize 1023" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=1536" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 1024 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 1023" \
            -c "1536 bytes written in 2 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (1024), 3 fragments" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --recordsize 1023" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=2560" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 1024 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 1023" \
            -c "2560 bytes written in 3 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (4096), 1 fragment" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --recordsize 4095" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=2048" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 4096 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 4095" \
            -c "2048 bytes written in 1 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (4096), 2 fragments" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --recordsize 4095" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=6144" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 4096 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 4095" \
            -c "6144 bytes written in 2 fragments"

requires_gnutls_tls1_3
requires_gnutls_record_size_limit
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3: Client complies with record size limit (4096), 3 fragments" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL -d 4 --recordsize 4095" \
            "$P_CLI debug_level=4 force_version=tls13 request_size=10240" \
            0 \
            -c "Sent RecordSizeLimit: 16384 Bytes" \
            -c "ClientHello: record_size_limit(28) extension exists." \
            -c "RecordSizeLimit: 4096 Bytes" \
            -c "EncryptedExtensions: record_size_limit(28) extension exists." \
            -c "Maximum outgoing record payload length is 4095" \
            -c "10240 bytes written in 3 fragments"

# TODO: For time being, we send fixed value of RecordSizeLimit defined by
# MBEDTLS_SSL_IN_CONTENT_LEN. Once we support variable buffer length of
# RecordSizeLimit, we need to modify value of RecordSizeLimit in below test.
requires_config_value_equals "MBEDTLS_SSL_IN_CONTENT_LEN" 16384
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_RECORD_SIZE_LIMIT
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Record Size Limit: TLS 1.3 m->m: both peer comply with record size limit (default)" \
            "$P_SRV debug_level=4 force_version=tls13" \
            "$P_CLI debug_level=4" \
            0 \
            -c "Sent RecordSizeLimit: $MAX_IN_LEN Bytes"         \
            -c "RecordSizeLimit: $MAX_IN_LEN Bytes"              \
            -s "RecordSizeLimit: $MAX_IN_LEN Bytes"              \
            -s "Sent RecordSizeLimit: $MAX_IN_LEN Bytes"         \
            -s "Maximum outgoing record payload length is 16383" \
            -s "Maximum incoming record payload length is 16384"

# End of Record size limit tests

# Tests for renegotiation

# G_NEXT_SRV is used in renegotiation tests becuase of the increased
# extensions limit since we exceed the limit in G_SRV when we send
# TLS 1.3 extensions in the initial handshake.

# Renegotiation SCSV always added, regardless of SSL_RENEGOTIATION
run_test    "Renegotiation: none, for reference" \
            "$P_SRV debug_level=3 exchanges=2 auth_mode=optional" \
            "$P_CLI force_version=tls12 debug_level=3 exchanges=2" \
            0 \
            -C "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -S "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -C "=> renegotiate" \
            -S "=> renegotiate" \
            -S "write hello request"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: client-initiated" \
            "$P_SRV debug_level=3 exchanges=2 renegotiation=1 auth_mode=optional" \
            "$P_CLI force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -S "write hello request"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: server-initiated" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 auth_mode=optional renegotiate=1" \
            "$P_CLI debug_level=3 exchanges=2 renegotiation=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request"

# Checks that no Signature Algorithm with SHA-1 gets negotiated. Negotiating SHA-1 would mean that
# the server did not parse the Signature Algorithm extension. This test is valid only if an MD
# algorithm stronger than SHA-1 is enabled in mbedtls_config.h
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: Signature Algorithms parsing, client-initiated" \
            "$P_SRV debug_level=3 exchanges=2 renegotiation=1 auth_mode=optional" \
            "$P_CLI force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -S "write hello request" \
            -S "client hello v3, signature_algorithm ext: 2" # Is SHA-1 negotiated?

# Checks that no Signature Algorithm with SHA-1 gets negotiated. Negotiating SHA-1 would mean that
# the server did not parse the Signature Algorithm extension. This test is valid only if an MD
# algorithm stronger than SHA-1 is enabled in mbedtls_config.h
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: Signature Algorithms parsing, server-initiated" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 auth_mode=optional renegotiate=1" \
            "$P_CLI debug_level=3 exchanges=2 renegotiation=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request" \
            -S "client hello v3, signature_algorithm ext: 2" # Is SHA-1 negotiated?

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: double" \
            "$P_SRV debug_level=3 exchanges=2 renegotiation=1 auth_mode=optional renegotiate=1" \
            "$P_CLI force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 2048
run_test    "Renegotiation with max fragment length: client 2048, server 512" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 auth_mode=optional renegotiate=1 max_frag_len=512" \
            "$P_CLI debug_level=3 exchanges=2 renegotiation=1 renegotiate=1 max_frag_len=2048 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
            0 \
            -c "Maximum incoming record payload length is 2048" \
            -c "Maximum outgoing record payload length is 2048" \
            -s "Maximum incoming record payload length is 2048" \
            -s "Maximum outgoing record payload length is 512" \
            -c "client hello, adding max_fragment_length extension" \
            -s "found max fragment length extension" \
            -s "server hello, max_fragment_length extension" \
            -c "found max_fragment_length extension" \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: client-initiated, server-rejected" \
            "$P_SRV debug_level=3 exchanges=2 renegotiation=0 auth_mode=optional" \
            "$P_CLI force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1" \
            1 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -S "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -S "=> renegotiate" \
            -S "write hello request" \
            -c "SSL - Unexpected message at ServerHello in renegotiation" \
            -c "failed"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: server-initiated, client-rejected, default" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1 auth_mode=optional" \
            "$P_CLI debug_level=3 exchanges=2 renegotiation=0" \
            0 \
            -C "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -S "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -C "=> renegotiate" \
            -S "=> renegotiate" \
            -s "write hello request" \
            -S "SSL - An unexpected message was received from our peer" \
            -S "failed"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: server-initiated, client-rejected, not enforced" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1 \
             renego_delay=-1 auth_mode=optional" \
            "$P_CLI debug_level=3 exchanges=2 renegotiation=0" \
            0 \
            -C "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -S "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -C "=> renegotiate" \
            -S "=> renegotiate" \
            -s "write hello request" \
            -S "SSL - An unexpected message was received from our peer" \
            -S "failed"

# delay 2 for 1 alert record + 1 application data record
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: server-initiated, client-rejected, delay 2" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1 \
             renego_delay=2 auth_mode=optional" \
            "$P_CLI debug_level=3 exchanges=2 renegotiation=0" \
            0 \
            -C "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -S "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -C "=> renegotiate" \
            -S "=> renegotiate" \
            -s "write hello request" \
            -S "SSL - An unexpected message was received from our peer" \
            -S "failed"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: server-initiated, client-rejected, delay 0" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1 \
             renego_delay=0 auth_mode=optional" \
            "$P_CLI debug_level=3 exchanges=2 renegotiation=0" \
            0 \
            -C "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -S "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -C "=> renegotiate" \
            -S "=> renegotiate" \
            -s "write hello request" \
            -s "SSL - An unexpected message was received from our peer"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: server-initiated, client-accepted, delay 0" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=2 renegotiation=1 renegotiate=1 \
             renego_delay=0 auth_mode=optional" \
            "$P_CLI debug_level=3 exchanges=2 renegotiation=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request" \
            -S "SSL - An unexpected message was received from our peer" \
            -S "failed"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: periodic, just below period" \
            "$P_SRV debug_level=3 exchanges=9 renegotiation=1 renego_period=3 auth_mode=optional" \
            "$P_CLI force_version=tls12 debug_level=3 exchanges=2 renegotiation=1" \
            0 \
            -C "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -S "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -S "record counter limit reached: renegotiate" \
            -C "=> renegotiate" \
            -S "=> renegotiate" \
            -S "write hello request" \
            -S "SSL - An unexpected message was received from our peer" \
            -S "failed"

# one extra exchange to be able to complete renego
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: periodic, just above period" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=9 renegotiation=1 renego_period=3 auth_mode=optional" \
            "$P_CLI debug_level=3 exchanges=4 renegotiation=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -s "record counter limit reached: renegotiate" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request" \
            -S "SSL - An unexpected message was received from our peer" \
            -S "failed"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: periodic, two times period" \
            "$P_SRV debug_level=3 exchanges=9 renegotiation=1 renego_period=3 auth_mode=optional" \
            "$P_CLI force_version=tls12 debug_level=3 exchanges=7 renegotiation=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -s "record counter limit reached: renegotiate" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request" \
            -S "SSL - An unexpected message was received from our peer" \
            -S "failed"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: periodic, above period, disabled" \
            "$P_SRV force_version=tls12 debug_level=3 exchanges=9 renegotiation=0 renego_period=3 auth_mode=optional" \
            "$P_CLI debug_level=3 exchanges=4 renegotiation=1" \
            0 \
            -C "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -S "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -S "record counter limit reached: renegotiate" \
            -C "=> renegotiate" \
            -S "=> renegotiate" \
            -S "write hello request" \
            -S "SSL - An unexpected message was received from our peer" \
            -S "failed"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: nbio, client-initiated" \
            "$P_SRV debug_level=3 nbio=2 exchanges=2 renegotiation=1 auth_mode=optional" \
            "$P_CLI force_version=tls12 debug_level=3 nbio=2 exchanges=2 renegotiation=1 renegotiate=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -S "write hello request"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Renegotiation: nbio, server-initiated" \
            "$P_SRV force_version=tls12 debug_level=3 nbio=2 exchanges=2 renegotiation=1 renegotiate=1 auth_mode=optional" \
            "$P_CLI debug_level=3 nbio=2 exchanges=2 renegotiation=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: openssl server, client-initiated" \
            "$O_SRV -www $OPENSSL_S_SERVER_CLIENT_RENEGOTIATION -tls1_2" \
            "$P_CLI debug_level=3 exchanges=1 renegotiation=1 renegotiate=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -C "ssl_handshake() returned" \
            -C "error" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: gnutls server strict, client-initiated" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%SAFE_RENEGOTIATION" \
            "$P_CLI debug_level=3 exchanges=1 renegotiation=1 renegotiate=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -C "ssl_handshake() returned" \
            -C "error" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: gnutls server unsafe, client-initiated default" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%DISABLE_SAFE_RENEGOTIATION" \
            "$P_CLI debug_level=3 exchanges=1 renegotiation=1 renegotiate=1" \
            1 \
            -c "client hello, adding renegotiation extension" \
            -C "found renegotiation extension" \
            -c "=> renegotiate" \
            -c "mbedtls_ssl_handshake() returned" \
            -c "error" \
            -C "HTTP/1.0 200 [Oo][Kk]"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: gnutls server unsafe, client-inititated no legacy" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%DISABLE_SAFE_RENEGOTIATION" \
            "$P_CLI debug_level=3 exchanges=1 renegotiation=1 renegotiate=1 \
             allow_legacy=0" \
            1 \
            -c "client hello, adding renegotiation extension" \
            -C "found renegotiation extension" \
            -c "=> renegotiate" \
            -c "mbedtls_ssl_handshake() returned" \
            -c "error" \
            -C "HTTP/1.0 200 [Oo][Kk]"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: gnutls server unsafe, client-inititated legacy" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%DISABLE_SAFE_RENEGOTIATION" \
            "$P_CLI debug_level=3 exchanges=1 renegotiation=1 renegotiate=1 \
             allow_legacy=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -C "found renegotiation extension" \
            -c "=> renegotiate" \
            -C "ssl_handshake() returned" \
            -C "error" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: DTLS, client-initiated" \
            "$P_SRV debug_level=3 dtls=1 exchanges=2 renegotiation=1" \
            "$P_CLI debug_level=3 dtls=1 exchanges=2 renegotiation=1 renegotiate=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -S "write hello request"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: DTLS, server-initiated" \
            "$P_SRV debug_level=3 dtls=1 exchanges=2 renegotiation=1 renegotiate=1" \
            "$P_CLI debug_level=3 dtls=1 exchanges=2 renegotiation=1 \
             read_timeout=1000 max_resend=2" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request"

requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: DTLS, renego_period overflow" \
            "$P_SRV debug_level=3 dtls=1 exchanges=4 renegotiation=1 renego_period=18446462598732840962 auth_mode=optional" \
            "$P_CLI debug_level=3 dtls=1 exchanges=4 renegotiation=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -s "record counter limit reached: renegotiate" \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "write hello request"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renegotiation: DTLS, gnutls server, client-initiated" \
            "$G_NEXT_SRV -u --mtu 4096" \
            "$P_CLI debug_level=3 dtls=1 exchanges=1 renegotiation=1 renegotiate=1" \
            0 \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -C "mbedtls_ssl_handshake returned" \
            -C "error" \
            -s "Extra-header:"

# Test for the "secure renegotiation" extension only (no actual renegotiation)

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renego ext: gnutls server strict, client default" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%SAFE_RENEGOTIATION" \
            "$P_CLI debug_level=3" \
            0 \
            -c "found renegotiation extension" \
            -C "error" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renego ext: gnutls server unsafe, client default" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%DISABLE_SAFE_RENEGOTIATION" \
            "$P_CLI debug_level=3" \
            0 \
            -C "found renegotiation extension" \
            -C "error" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renego ext: gnutls server unsafe, client break legacy" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%DISABLE_SAFE_RENEGOTIATION" \
            "$P_CLI debug_level=3 allow_legacy=-1" \
            1 \
            -C "found renegotiation extension" \
            -c "error" \
            -C "HTTP/1.0 200 [Oo][Kk]"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renego ext: gnutls client strict, server default" \
            "$P_SRV debug_level=3" \
            "$G_CLI --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%SAFE_RENEGOTIATION localhost" \
            0 \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO\|found renegotiation extension" \
            -s "server hello, secure renegotiation extension"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renego ext: gnutls client unsafe, server default" \
            "$P_SRV debug_level=3" \
            "$G_CLI --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%DISABLE_SAFE_RENEGOTIATION localhost" \
            0 \
            -S "received TLS_EMPTY_RENEGOTIATION_INFO\|found renegotiation extension" \
            -S "server hello, secure renegotiation extension"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Renego ext: gnutls client unsafe, server break legacy" \
            "$P_SRV debug_level=3 allow_legacy=-1" \
            "$G_CLI --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:%DISABLE_SAFE_RENEGOTIATION localhost" \
            1 \
            -S "received TLS_EMPTY_RENEGOTIATION_INFO\|found renegotiation extension" \
            -S "server hello, secure renegotiation extension"

# Tests for silently dropping trailing extra bytes in .der certificates

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DER format: no trailing bytes" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-der0.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$G_CLI localhost" \
            0 \
            -c "Handshake was completed" \

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DER format: with a trailing zero byte" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-der1a.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$G_CLI localhost" \
            0 \
            -c "Handshake was completed" \

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DER format: with a trailing random byte" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-der1b.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$G_CLI localhost" \
            0 \
            -c "Handshake was completed" \

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DER format: with 2 trailing random bytes" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-der2.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$G_CLI localhost" \
            0 \
            -c "Handshake was completed" \

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DER format: with 4 trailing random bytes" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-der4.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$G_CLI localhost" \
            0 \
            -c "Handshake was completed" \

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DER format: with 8 trailing random bytes" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-der8.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$G_CLI localhost" \
            0 \
            -c "Handshake was completed" \

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DER format: with 9 trailing random bytes" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-der9.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$G_CLI localhost" \
            0 \
            -c "Handshake was completed" \

# Tests for auth_mode, there are duplicated tests using ca callback for authentication
# When updating these tests, modify the matching authentication tests accordingly

# The next 4 cases test the 3 auth modes with a badly signed server cert.
run_test    "Authentication: server badcert, client required" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI debug_level=3 auth_mode=required" \
            1 \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -c "! mbedtls_ssl_handshake returned" \
            -c "send alert level=2 message=48" \
            -c "X509 - Certificate verification failed"
            # MBEDTLS_X509_BADCERT_NOT_TRUSTED -> MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA
# We don't check that the server receives the alert because it might
# detect that its write end of the connection is closed and abort
# before reading the alert message.

run_test    "Authentication: server badcert, client required (1.2)" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_version=tls12 debug_level=3 auth_mode=required" \
            1 \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -c "! mbedtls_ssl_handshake returned" \
            -c "send alert level=2 message=48" \
            -c "X509 - Certificate verification failed"
            # MBEDTLS_X509_BADCERT_NOT_TRUSTED -> MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA

run_test    "Authentication: server badcert, client optional" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_version=tls13 debug_level=3 auth_mode=optional" \
            0 \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "send alert level=2 message=48" \
            -C "X509 - Certificate verification failed"

run_test    "Authentication: server badcert, client optional (1.2)" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_version=tls12 debug_level=3 auth_mode=optional" \
            0 \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "send alert level=2 message=48" \
            -C "X509 - Certificate verification failed"

run_test    "Authentication: server badcert, client none" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI debug_level=3 auth_mode=none" \
            0 \
            -c "! Certificate verification was skipped" \
            -C "x509_verify_cert() returned" \
            -C "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "send alert level=2 message=48" \
            -C "X509 - Certificate verification failed"

run_test    "Authentication: server badcert, client none (1.2)" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_version=tls12 debug_level=3 auth_mode=none" \
            0 \
            -c "! Certificate verification was skipped" \
            -C "x509_verify_cert() returned" \
            -C "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "send alert level=2 message=48" \
            -C "X509 - Certificate verification failed"


run_test    "Authentication: server goodcert, client required, no trusted CA" \
            "$P_SRV" \
            "$P_CLI debug_level=3 auth_mode=required ca_file=none ca_path=none" \
            1 \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -c "! Certificate verification flags"\
            -c "! mbedtls_ssl_handshake returned" \
            -c "SSL - No CA Chain is set, but required to operate"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: server goodcert, client required, no trusted CA (1.2)" \
            "$P_SRV force_version=tls12" \
            "$P_CLI debug_level=3 auth_mode=required ca_file=none ca_path=none" \
            1 \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -c "! Certificate verification flags"\
            -c "! mbedtls_ssl_handshake returned" \
            -c "SSL - No CA Chain is set, but required to operate"

run_test    "Authentication: server goodcert, client optional, no trusted CA" \
            "$P_SRV" \
            "$P_CLI debug_level=3 auth_mode=optional ca_file=none ca_path=none" \
            0 \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -c "! Certificate verification flags"\
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed" \
            -C "SSL - No CA Chain is set, but required to operate"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: server goodcert, client optional, no trusted CA (1.2)" \
            "$P_SRV" \
            "$P_CLI force_version=tls12 debug_level=3 auth_mode=optional ca_file=none ca_path=none" \
            0 \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -c "! Certificate verification flags"\
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed" \
            -C "SSL - No CA Chain is set, but required to operate"

run_test    "Authentication: server goodcert, client none, no trusted CA" \
            "$P_SRV" \
            "$P_CLI debug_level=3 auth_mode=none ca_file=none ca_path=none" \
            0 \
            -c "! Certificate verification was skipped" \
            -C "x509_verify_cert() returned" \
            -C "! The certificate is not correctly signed by the trusted CA" \
            -C "! Certificate verification flags"\
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed" \
            -C "SSL - No CA Chain is set, but required to operate"


requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: server goodcert, client none, no trusted CA (1.2)" \
            "$P_SRV" \
            "$P_CLI force_version=tls12 debug_level=3 auth_mode=none ca_file=none ca_path=none" \
            0 \
            -c "! Certificate verification was skipped" \
            -C "x509_verify_cert() returned" \
            -C "! The certificate is not correctly signed by the trusted CA" \
            -C "! Certificate verification flags"\
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed" \
            -C "SSL - No CA Chain is set, but required to operate"


# The next few tests check what happens if the server has a valid certificate
# that does not match its name (impersonation).

run_test "Authentication: hostname match, client required" \
         "$P_SRV" \
         "$P_CLI auth_mode=required server_name=localhost debug_level=2" \
         0 \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "! mbedtls_ssl_handshake returned" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname match, client required, CA callback" \
         "$P_SRV" \
         "$P_CLI auth_mode=required server_name=localhost debug_level=3 ca_callback=1" \
         0 \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -c "use CA callback for X.509 CRT verification" \
         -C "x509_verify_cert() returned -" \
         -C "! mbedtls_ssl_handshake returned" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname mismatch (wrong), client required" \
         "$P_SRV" \
         "$P_CLI auth_mode=required server_name=wrong-name debug_level=1" \
         1 \
         -c "does not match with the expected CN" \
         -c "x509_verify_cert() returned -" \
         -c "! mbedtls_ssl_handshake returned" \
         -c "X509 - Certificate verification failed"

run_test "Authentication: hostname mismatch (empty), client required" \
         "$P_SRV" \
         "$P_CLI auth_mode=required server_name= debug_level=1" \
         1 \
         -c "does not match with the expected CN" \
         -c "x509_verify_cert() returned -" \
         -c "! mbedtls_ssl_handshake returned" \
         -c "X509 - Certificate verification failed"

run_test "Authentication: hostname mismatch (truncated), client required" \
         "$P_SRV" \
         "$P_CLI auth_mode=required server_name=localhos debug_level=1" \
         1 \
         -c "does not match with the expected CN" \
         -c "x509_verify_cert() returned -" \
         -c "! mbedtls_ssl_handshake returned" \
         -c "X509 - Certificate verification failed"

run_test "Authentication: hostname mismatch (last char), client required" \
         "$P_SRV" \
         "$P_CLI auth_mode=required server_name=localhoss debug_level=1" \
         1 \
         -c "does not match with the expected CN" \
         -c "x509_verify_cert() returned -" \
         -c "! mbedtls_ssl_handshake returned" \
         -c "X509 - Certificate verification failed"

run_test "Authentication: hostname mismatch (trailing), client required" \
         "$P_SRV" \
         "$P_CLI auth_mode=required server_name=localhostt debug_level=1" \
         1 \
         -c "does not match with the expected CN" \
         -c "x509_verify_cert() returned -" \
         -c "! mbedtls_ssl_handshake returned" \
         -c "X509 - Certificate verification failed"

run_test "Authentication: hostname mismatch, client optional" \
         "$P_SRV" \
         "$P_CLI auth_mode=optional server_name=wrong-name debug_level=2" \
         0 \
         -c "does not match with the expected CN" \
         -c "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname mismatch, client none" \
         "$P_SRV" \
         "$P_CLI auth_mode=none server_name=wrong-name debug_level=2" \
         0 \
         -c "! Certificate verification was skipped" \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"


run_test "Authentication: hostname null, client required" \
         "$P_SRV" \
         "$P_CLI auth_mode=required set_hostname=NULL debug_level=2" \
         0 \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -c "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "! mbedtls_ssl_handshake returned" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname null, client optional" \
         "$P_SRV" \
         "$P_CLI auth_mode=optional set_hostname=NULL debug_level=2" \
         0 \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -c "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname null, client none" \
         "$P_SRV" \
         "$P_CLI auth_mode=none set_hostname=NULL debug_level=2" \
         0 \
         -c "! Certificate verification was skipped" \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"


run_test "Authentication: hostname unset, client required" \
         "$P_SRV" \
         "$P_CLI auth_mode=required set_hostname=no debug_level=2" \
         1 \
         -C "does not match with the expected CN" \
         -c "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -c "get_hostname_for_verification() returned -" \
         -C "x509_verify_cert() returned -" \
         -c "! mbedtls_ssl_handshake returned" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname unset, client required, CA callback" \
         "$P_SRV" \
         "$P_CLI auth_mode=required set_hostname=no debug_level=3 ca_callback=1" \
         1 \
         -C "does not match with the expected CN" \
         -c "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -c "get_hostname_for_verification() returned -" \
         -C "use CA callback for X.509 CRT verification" \
         -C "x509_verify_cert() returned -" \
         -c "! mbedtls_ssl_handshake returned" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname unset, client optional" \
         "$P_SRV" \
         "$P_CLI auth_mode=optional set_hostname=no debug_level=2" \
         0 \
         -C "does not match with the expected CN" \
         -c "Certificate verification without having set hostname" \
         -c "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname unset, client none" \
         "$P_SRV" \
         "$P_CLI auth_mode=none set_hostname=no debug_level=2" \
         0 \
         -c "! Certificate verification was skipped" \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname unset, client default, server picks cert, 1.2" \
         "$P_SRV force_version=tls12 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8" \
         "$P_CLI psk=73776f726466697368 psk_identity=foo set_hostname=no debug_level=2" \
         1 \
         -C "does not match with the expected CN" \
         -c "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -c "get_hostname_for_verification() returned -" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test "Authentication: hostname unset, client default, server picks cert, 1.3" \
         "$P_SRV force_version=tls13 tls13_kex_modes=ephemeral" \
         "$P_CLI psk=73776f726466697368 psk_identity=foo set_hostname=no debug_level=2" \
         1 \
         -C "does not match with the expected CN" \
         -c "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -c "get_hostname_for_verification() returned -" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"

run_test "Authentication: hostname unset, client default, server picks PSK, 1.2" \
         "$P_SRV force_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8 psk=73776f726466697368 psk_identity=foo" \
         "$P_CLI psk=73776f726466697368 psk_identity=foo set_hostname=no debug_level=2" \
         0 \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test "Authentication: hostname unset, client default, server picks PSK, 1.3" \
         "$P_SRV force_version=tls13 tls13_kex_modes=psk psk=73776f726466697368 psk_identity=foo" \
         "$P_CLI psk=73776f726466697368 psk_identity=foo set_hostname=no debug_level=2" \
         0 \
         -C "does not match with the expected CN" \
         -C "Certificate verification without having set hostname" \
         -C "Certificate verification without CN verification" \
         -C "x509_verify_cert() returned -" \
         -C "X509 - Certificate verification failed"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: client SHA256, server required" \
            "$P_SRV auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384" \
            0 \
            -c "Supported Signature Algorithm found: 04 " \
            -c "Supported Signature Algorithm found: 05 "

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: client SHA384, server required" \
            "$P_SRV auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256" \
            0 \
            -c "Supported Signature Algorithm found: 04 " \
            -c "Supported Signature Algorithm found: 05 "

run_test    "Authentication: client has no cert, server required (TLS)" \
            "$P_SRV debug_level=3 auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=none \
             key_file=$DATA_FILES_PATH/server5.key" \
            1 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -c "= write certificate$" \
            -C "skip write certificate$" \
            -S "x509_verify_cert() returned" \
            -s "peer has no certificate" \
            -s "! mbedtls_ssl_handshake returned" \
            -s "No client certification received from the client, but required by the authentication mode"

run_test    "Authentication: client badcert, server required" \
            "$P_SRV debug_level=3 auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            1 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -s "! mbedtls_ssl_handshake returned" \
            -s "send alert level=2 message=48" \
            -s "X509 - Certificate verification failed"
# We don't check that the client receives the alert because it might
# detect that its write end of the connection is closed and abort
# before reading the alert message.

run_test    "Authentication: client cert self-signed and trusted, server required" \
            "$P_SRV debug_level=3 auth_mode=required ca_file=$DATA_FILES_PATH/server5-selfsigned.crt" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-selfsigned.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -S "x509_verify_cert() returned" \
            -S "! The certificate is not correctly signed" \
            -S "X509 - Certificate verification failed"

run_test    "Authentication: client cert not trusted, server required" \
            "$P_SRV debug_level=3 auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-selfsigned.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            1 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -s "! mbedtls_ssl_handshake returned" \
            -s "X509 - Certificate verification failed"

run_test    "Authentication: client badcert, server optional" \
            "$P_SRV debug_level=3 auth_mode=optional" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -S "! mbedtls_ssl_handshake returned" \
            -C "! mbedtls_ssl_handshake returned" \
            -S "X509 - Certificate verification failed"

run_test    "Authentication: client badcert, server none" \
            "$P_SRV debug_level=3 auth_mode=none" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            0 \
            -s "! Certificate verification was skipped" \
            -s "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got no certificate request" \
            -c "skip write certificate" \
            -c "skip write certificate verify" \
            -s "skip parse certificate verify" \
            -S "x509_verify_cert() returned" \
            -S "! The certificate is not correctly signed by the trusted CA" \
            -S "! mbedtls_ssl_handshake returned" \
            -C "! mbedtls_ssl_handshake returned" \
            -S "X509 - Certificate verification failed"

run_test    "Authentication: client no cert, server optional" \
            "$P_SRV debug_level=3 auth_mode=optional" \
            "$P_CLI debug_level=3 crt_file=none key_file=none" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate$" \
            -C "got no certificate to send" \
            -c "skip write certificate verify" \
            -s "skip parse certificate verify" \
            -s "! Certificate was missing" \
            -S "! mbedtls_ssl_handshake returned" \
            -C "! mbedtls_ssl_handshake returned" \
            -S "X509 - Certificate verification failed"

requires_openssl_tls1_3_with_compatible_ephemeral
run_test    "Authentication: openssl client no cert, server optional" \
            "$P_SRV debug_level=3 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -no_middlebox" \
            0 \
            -S "skip write certificate request" \
            -s "skip parse certificate verify" \
            -s "! Certificate was missing" \
            -S "! mbedtls_ssl_handshake returned" \
            -S "X509 - Certificate verification failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Authentication: client no cert, openssl server optional" \
            "$O_SRV -verify 10 -tls1_2" \
            "$P_CLI debug_level=3 crt_file=none key_file=none" \
            0 \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate$" \
            -c "skip write certificate verify" \
            -C "! mbedtls_ssl_handshake returned"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Authentication: client no cert, openssl server required" \
            "$O_SRV -Verify 10 -tls1_2" \
            "$P_CLI debug_level=3 crt_file=none key_file=none" \
            1 \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate$" \
            -c "skip write certificate verify" \
            -c "! mbedtls_ssl_handshake returned"

# This script assumes that MBEDTLS_X509_MAX_INTERMEDIATE_CA has its default
# value, defined here as MAX_IM_CA. Some test cases will be skipped if the
# library is configured with a different value.

MAX_IM_CA='8'

# The tests for the max_int tests can pass with any number higher than MAX_IM_CA
# because only a chain of MAX_IM_CA length is tested. Equally, the max_int+1
# tests can pass with any number less than MAX_IM_CA. However, stricter preconditions
# are in place so that the semantics are consistent with the test description.
requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication: server max_int chain, client default" \
            "$P_SRV crt_file=$DATA_FILES_PATH/dir-maxpath/c09.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/09.key" \
            "$P_CLI server_name=CA09 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt" \
            0 \
            -C "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication: server max_int+1 chain, client default" \
            "$P_SRV crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            "$P_CLI server_name=CA10 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt" \
            1 \
            -c "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication: server max_int+1 chain, client optional" \
            "$P_SRV crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            "$P_CLI server_name=CA10 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt \
                    auth_mode=optional" \
            1 \
            -c "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication: server max_int+1 chain, client none" \
            "$P_SRV crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            "$P_CLI force_version=tls12 server_name=CA10 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt \
                    auth_mode=none" \
            0 \
            -c "! Certificate verification was skipped" \
            -C "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication: client max_int+1 chain, server default" \
            "$P_SRV ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt" \
            "$P_CLI crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            0 \
            -S "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication: client max_int+1 chain, server optional" \
            "$P_SRV ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt auth_mode=optional" \
            "$P_CLI crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            1 \
            -s "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication: client max_int+1 chain, server required" \
            "$P_SRV ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt auth_mode=required" \
            "$P_CLI crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            1 \
            -s "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication: client max_int chain, server required" \
            "$P_SRV ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt auth_mode=required" \
            "$P_CLI crt_file=$DATA_FILES_PATH/dir-maxpath/c09.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/09.key" \
            0 \
            -S "X509 - A fatal error occurred"

# Tests for CA list in CertificateRequest messages

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: send CA list in CertificateRequest  (default)" \
            "$P_SRV debug_level=3 auth_mode=required" \
            "$P_CLI force_version=tls12 crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key" \
            0 \
            -s "requested DN"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: do not send CA list in CertificateRequest" \
            "$P_SRV debug_level=3 auth_mode=required cert_req_ca_list=0" \
            "$P_CLI force_version=tls12 crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key" \
            0 \
            -S "requested DN"

run_test    "Authentication: send CA list in CertificateRequest, client self signed" \
            "$P_SRV force_version=tls12 debug_level=3 auth_mode=required cert_req_ca_list=0" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-selfsigned.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            1 \
            -S "requested DN" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -s "! mbedtls_ssl_handshake returned" \
            -c "! mbedtls_ssl_handshake returned" \
            -s "X509 - Certificate verification failed"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: send alt conf DN hints in CertificateRequest" \
            "$P_SRV debug_level=3 auth_mode=optional cert_req_ca_list=2 \
             crt_file2=$DATA_FILES_PATH/server1.crt \
             key_file2=$DATA_FILES_PATH/server1.key" \
            "$P_CLI force_version=tls12 debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key" \
            0 \
            -c "DN hint: C=NL, O=PolarSSL, CN=PolarSSL Server 1"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: send alt conf DN hints in CertificateRequest (2)" \
            "$P_SRV debug_level=3 auth_mode=optional cert_req_ca_list=2 \
             crt_file2=$DATA_FILES_PATH/server2.crt \
             key_file2=$DATA_FILES_PATH/server2.key" \
            "$P_CLI force_version=tls12 debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key" \
            0 \
            -c "DN hint: C=NL, O=PolarSSL, CN=localhost"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication: send alt hs DN hints in CertificateRequest" \
            "$P_SRV debug_level=3 auth_mode=optional cert_req_ca_list=3 \
             crt_file2=$DATA_FILES_PATH/server1.crt \
             key_file2=$DATA_FILES_PATH/server1.key" \
            "$P_CLI force_version=tls12 debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key" \
            0 \
            -c "DN hint: C=NL, O=PolarSSL, CN=PolarSSL Server 1"

# Tests for auth_mode, using CA callback, these are duplicated from the authentication tests
# When updating these tests, modify the matching authentication tests accordingly

run_test    "Authentication, CA callback: server badcert, client required" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI ca_callback=1 debug_level=3 auth_mode=required" \
            1 \
            -c "use CA callback for X.509 CRT verification" \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -c "! mbedtls_ssl_handshake returned" \
            -c "X509 - Certificate verification failed"

run_test    "Authentication, CA callback: server badcert, client optional" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI ca_callback=1 debug_level=3 auth_mode=optional" \
            0 \
            -c "use CA callback for X.509 CRT verification" \
            -c "x509_verify_cert() returned" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed"

run_test    "Authentication, CA callback: server badcert, client none" \
            "$P_SRV crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI ca_callback=1 debug_level=3 auth_mode=none" \
            0 \
            -C "use CA callback for X.509 CRT verification" \
            -C "x509_verify_cert() returned" \
            -C "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication, CA callback: client SHA384, server required" \
            "$P_SRV ca_callback=1 debug_level=3 auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384" \
            0 \
            -s "use CA callback for X.509 CRT verification" \
            -c "Supported Signature Algorithm found: 04 " \
            -c "Supported Signature Algorithm found: 05 "

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Authentication, CA callback: client SHA256, server required" \
            "$P_SRV ca_callback=1 debug_level=3 auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server6.crt \
             key_file=$DATA_FILES_PATH/server6.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256" \
            0 \
            -s "use CA callback for X.509 CRT verification" \
            -c "Supported Signature Algorithm found: 04 " \
            -c "Supported Signature Algorithm found: 05 "

run_test    "Authentication, CA callback: client badcert, server required" \
            "$P_SRV ca_callback=1 debug_level=3 auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            1 \
            -s "use CA callback for X.509 CRT verification" \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -s "! mbedtls_ssl_handshake returned" \
            -s "send alert level=2 message=48" \
            -s "X509 - Certificate verification failed"
# We don't check that the client receives the alert because it might
# detect that its write end of the connection is closed and abort
# before reading the alert message.

run_test    "Authentication, CA callback: client cert not trusted, server required" \
            "$P_SRV ca_callback=1 debug_level=3 auth_mode=required" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-selfsigned.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            1 \
            -s "use CA callback for X.509 CRT verification" \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -s "! mbedtls_ssl_handshake returned" \
            -s "X509 - Certificate verification failed"

run_test    "Authentication, CA callback: client badcert, server optional" \
            "$P_SRV ca_callback=1 debug_level=3 auth_mode=optional" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            0 \
            -s "use CA callback for X.509 CRT verification" \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -S "! mbedtls_ssl_handshake returned" \
            -C "! mbedtls_ssl_handshake returned" \
            -S "X509 - Certificate verification failed"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication, CA callback: server max_int chain, client default" \
            "$P_SRV crt_file=$DATA_FILES_PATH/dir-maxpath/c09.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/09.key" \
            "$P_CLI ca_callback=1 debug_level=3 server_name=CA09 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt" \
            0 \
            -c "use CA callback for X.509 CRT verification" \
            -C "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication, CA callback: server max_int+1 chain, client default" \
            "$P_SRV crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            "$P_CLI debug_level=3 ca_callback=1 server_name=CA10 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt" \
            1 \
            -c "use CA callback for X.509 CRT verification" \
            -c "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication, CA callback: server max_int+1 chain, client optional" \
            "$P_SRV crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            "$P_CLI ca_callback=1 server_name=CA10 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt \
                    debug_level=3 auth_mode=optional" \
            1 \
            -c "use CA callback for X.509 CRT verification" \
            -c "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication, CA callback: client max_int+1 chain, server optional" \
            "$P_SRV ca_callback=1 debug_level=3 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt auth_mode=optional" \
            "$P_CLI crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            1 \
            -s "use CA callback for X.509 CRT verification" \
            -s "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication, CA callback: client max_int+1 chain, server required" \
            "$P_SRV ca_callback=1 debug_level=3 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt auth_mode=required" \
            "$P_CLI crt_file=$DATA_FILES_PATH/dir-maxpath/c10.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/10.key" \
            1 \
            -s "use CA callback for X.509 CRT verification" \
            -s "X509 - A fatal error occurred"

requires_config_value_equals "MBEDTLS_X509_MAX_INTERMEDIATE_CA" $MAX_IM_CA
requires_full_size_output_buffer
run_test    "Authentication, CA callback: client max_int chain, server required" \
            "$P_SRV ca_callback=1 debug_level=3 ca_file=$DATA_FILES_PATH/dir-maxpath/00.crt auth_mode=required" \
            "$P_CLI crt_file=$DATA_FILES_PATH/dir-maxpath/c09.pem \
                    key_file=$DATA_FILES_PATH/dir-maxpath/09.key" \
            0 \
            -s "use CA callback for X.509 CRT verification" \
            -S "X509 - A fatal error occurred"

# Tests for certificate selection based on SHA version

requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "Certificate hash: client TLS 1.2 -> SHA-2" \
            "$P_SRV force_version=tls12 crt_file=$DATA_FILES_PATH/server5.crt \
                    key_file=$DATA_FILES_PATH/server5.key \
                    crt_file2=$DATA_FILES_PATH/server5-sha1.crt \
                    key_file2=$DATA_FILES_PATH/server5.key" \
            "$P_CLI" \
            0 \
            -c "signed using.*ECDSA with SHA256" \
            -C "signed using.*ECDSA with SHA1"

# tests for SNI

requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "SNI: no SNI callback" \
            "$P_SRV debug_level=3 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI server_name=localhost" \
            0 \
            -c "issuer name *: C=NL, O=PolarSSL, CN=Polarssl Test EC CA" \
            -c "subject name *: C=NL, O=PolarSSL, CN=localhost"

requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "SNI: matching cert 1" \
            "$P_SRV debug_level=3 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$P_CLI server_name=localhost" \
            0 \
            -s "parse ServerName extension" \
            -c "issuer name *: C=NL, O=PolarSSL, CN=PolarSSL Test CA" \
            -c "subject name *: C=NL, O=PolarSSL, CN=localhost"

requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "SNI: matching cert 2" \
            "$P_SRV debug_level=3 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$P_CLI server_name=polarssl.example" \
            0 \
            -s "parse ServerName extension" \
            -c "issuer name *: C=NL, O=PolarSSL, CN=PolarSSL Test CA" \
            -c "subject name *: C=NL, O=PolarSSL, CN=polarssl.example"

requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "SNI: no matching cert" \
            "$P_SRV debug_level=3 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$P_CLI server_name=nonesuch.example" \
            1 \
            -s "parse ServerName extension" \
            -s "ssl_sni_wrapper() returned" \
            -s "mbedtls_ssl_handshake returned" \
            -c "mbedtls_ssl_handshake returned" \
            -c "SSL - A fatal alert message was received from our peer"

run_test    "SNI: client auth no override: optional" \
            "$P_SRV debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-" \
            "$P_CLI debug_level=3 server_name=localhost" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify"

run_test    "SNI: client auth override: none -> optional" \
            "$P_SRV debug_level=3 auth_mode=none \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,optional" \
            "$P_CLI debug_level=3 server_name=localhost" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify"

run_test    "SNI: client auth override: optional -> none" \
            "$P_SRV debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,none" \
            "$P_CLI debug_level=3 server_name=localhost" \
            0 \
            -s "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got no certificate request" \
            -c "skip write certificate"

run_test    "SNI: CA no override" \
            "$P_SRV debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             ca_file=$DATA_FILES_PATH/test-ca.crt \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,required" \
            "$P_CLI debug_level=3 server_name=localhost \
             crt_file=$DATA_FILES_PATH/server6.crt key_file=$DATA_FILES_PATH/server6.key" \
            1 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -S "The certificate has been revoked (is on a CRL)"

run_test    "SNI: CA override" \
            "$P_SRV debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             ca_file=$DATA_FILES_PATH/test-ca.crt \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,$DATA_FILES_PATH/test-ca2.crt,-,required" \
            "$P_CLI debug_level=3 server_name=localhost \
             crt_file=$DATA_FILES_PATH/server6.crt key_file=$DATA_FILES_PATH/server6.key" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -S "x509_verify_cert() returned" \
            -S "! The certificate is not correctly signed by the trusted CA" \
            -S "The certificate has been revoked (is on a CRL)"

run_test    "SNI: CA override with CRL" \
            "$P_SRV debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             ca_file=$DATA_FILES_PATH/test-ca.crt \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,$DATA_FILES_PATH/test-ca2.crt,$DATA_FILES_PATH/crl-ec-sha256.pem,required" \
            "$P_CLI debug_level=3 server_name=localhost \
             crt_file=$DATA_FILES_PATH/server6.crt key_file=$DATA_FILES_PATH/server6.key" \
            1 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -S "! The certificate is not correctly signed by the trusted CA" \
            -s "send alert level=2 message=44" \
            -s "The certificate has been revoked (is on a CRL)"
            # MBEDTLS_X509_BADCERT_REVOKED -> MBEDTLS_SSL_ALERT_MSG_CERT_REVOKED

# Tests for SNI and DTLS

requires_config_disabled MBEDTLS_X509_REMOVE_INFO
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, no SNI callback" \
            "$P_SRV debug_level=3 dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI server_name=localhost dtls=1" \
            0 \
            -c "issuer name *: C=NL, O=PolarSSL, CN=Polarssl Test EC CA" \
            -c "subject name *: C=NL, O=PolarSSL, CN=localhost"

requires_config_disabled MBEDTLS_X509_REMOVE_INFO
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, matching cert 1" \
            "$P_SRV debug_level=3 dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$P_CLI server_name=localhost dtls=1" \
            0 \
            -s "parse ServerName extension" \
            -c "issuer name *: C=NL, O=PolarSSL, CN=PolarSSL Test CA" \
            -c "subject name *: C=NL, O=PolarSSL, CN=localhost"

requires_config_disabled MBEDTLS_X509_REMOVE_INFO
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, matching cert 2" \
            "$P_SRV debug_level=3 dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$P_CLI server_name=polarssl.example dtls=1" \
            0 \
            -s "parse ServerName extension" \
            -c "issuer name *: C=NL, O=PolarSSL, CN=PolarSSL Test CA" \
            -c "subject name *: C=NL, O=PolarSSL, CN=polarssl.example"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, no matching cert" \
            "$P_SRV debug_level=3 dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$P_CLI server_name=nonesuch.example dtls=1" \
            1 \
            -s "parse ServerName extension" \
            -s "ssl_sni_wrapper() returned" \
            -s "mbedtls_ssl_handshake returned" \
            -c "mbedtls_ssl_handshake returned" \
            -c "SSL - A fatal alert message was received from our peer"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, client auth no override: optional" \
            "$P_SRV debug_level=3 auth_mode=optional dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-" \
            "$P_CLI debug_level=3 server_name=localhost dtls=1" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, client auth override: none -> optional" \
            "$P_SRV debug_level=3 auth_mode=none dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,optional" \
            "$P_CLI debug_level=3 server_name=localhost dtls=1" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, client auth override: optional -> none" \
            "$P_SRV debug_level=3 auth_mode=optional dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,none" \
            "$P_CLI debug_level=3 server_name=localhost dtls=1" \
            0 \
            -s "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got no certificate request" \
            -c "skip write certificate" \
            -c "skip write certificate verify" \
            -s "skip parse certificate verify"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, CA no override" \
            "$P_SRV debug_level=3 auth_mode=optional dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             ca_file=$DATA_FILES_PATH/test-ca.crt \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,required" \
            "$P_CLI debug_level=3 server_name=localhost dtls=1 \
             crt_file=$DATA_FILES_PATH/server6.crt key_file=$DATA_FILES_PATH/server6.key" \
            1 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -s "! The certificate is not correctly signed by the trusted CA" \
            -S "The certificate has been revoked (is on a CRL)"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, CA override" \
            "$P_SRV debug_level=3 auth_mode=optional dtls=1 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             ca_file=$DATA_FILES_PATH/test-ca.crt \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,$DATA_FILES_PATH/test-ca2.crt,-,required" \
            "$P_CLI debug_level=3 server_name=localhost dtls=1 \
             crt_file=$DATA_FILES_PATH/server6.crt key_file=$DATA_FILES_PATH/server6.key" \
            0 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -S "x509_verify_cert() returned" \
            -S "! The certificate is not correctly signed by the trusted CA" \
            -S "The certificate has been revoked (is on a CRL)"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "SNI: DTLS, CA override with CRL" \
            "$P_SRV debug_level=3 auth_mode=optional \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key dtls=1 \
             ca_file=$DATA_FILES_PATH/test-ca.crt \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,$DATA_FILES_PATH/test-ca2.crt,$DATA_FILES_PATH/crl-ec-sha256.pem,required" \
            "$P_CLI debug_level=3 server_name=localhost dtls=1 \
             crt_file=$DATA_FILES_PATH/server6.crt key_file=$DATA_FILES_PATH/server6.key" \
            1 \
            -S "skip write certificate request" \
            -C "skip parse certificate request" \
            -c "got a certificate request" \
            -C "skip write certificate" \
            -C "skip write certificate verify" \
            -S "skip parse certificate verify" \
            -s "x509_verify_cert() returned" \
            -S "! The certificate is not correctly signed by the trusted CA" \
            -s "send alert level=2 message=44" \
            -s "The certificate has been revoked (is on a CRL)"
            # MBEDTLS_X509_BADCERT_REVOKED -> MBEDTLS_SSL_ALERT_MSG_CERT_REVOKED

# Tests for non-blocking I/O: exercise a variety of handshake flows

run_test    "Non-blocking I/O: basic handshake" \
            "$P_SRV nbio=2 tickets=0 auth_mode=none" \
            "$P_CLI nbio=2 tickets=0" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

run_test    "Non-blocking I/O: client auth" \
            "$P_SRV nbio=2 tickets=0 auth_mode=required" \
            "$P_CLI nbio=2 tickets=0" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Non-blocking I/O: ticket" \
            "$P_SRV nbio=2 tickets=1 auth_mode=none" \
            "$P_CLI nbio=2 tickets=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Non-blocking I/O: ticket + client auth" \
            "$P_SRV nbio=2 tickets=1 auth_mode=required" \
            "$P_CLI nbio=2 tickets=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Non-blocking I/O: TLS 1.2 + ticket + client auth + resume" \
            "$P_SRV nbio=2 tickets=1 auth_mode=required" \
            "$P_CLI force_version=tls12 nbio=2 tickets=1 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Non-blocking I/O: TLS 1.3 + ticket + client auth + resume" \
            "$P_SRV nbio=2 tickets=1 auth_mode=required" \
            "$P_CLI nbio=2 tickets=1 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Non-blocking I/O: TLS 1.2 + ticket + resume" \
            "$P_SRV nbio=2 tickets=1 auth_mode=none" \
            "$P_CLI force_version=tls12 nbio=2 tickets=1 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Non-blocking I/O: TLS 1.3 + ticket + resume" \
            "$P_SRV nbio=2 tickets=1 auth_mode=none" \
            "$P_CLI nbio=2 tickets=1 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Non-blocking I/O: session-id resume" \
            "$P_SRV nbio=2 tickets=0 auth_mode=none" \
            "$P_CLI force_version=tls12 nbio=2 tickets=0 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

# Tests for event-driven I/O: exercise a variety of handshake flows

run_test    "Event-driven I/O: basic handshake" \
            "$P_SRV event=1 tickets=0 auth_mode=none" \
            "$P_CLI event=1 tickets=0" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

run_test    "Event-driven I/O: client auth" \
            "$P_SRV event=1 tickets=0 auth_mode=required" \
            "$P_CLI event=1 tickets=0" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O: ticket" \
            "$P_SRV event=1 tickets=1 auth_mode=none" \
            "$P_CLI event=1 tickets=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O: ticket + client auth" \
            "$P_SRV event=1 tickets=1 auth_mode=required" \
            "$P_CLI event=1 tickets=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O: TLS 1.2 + ticket + client auth + resume" \
            "$P_SRV event=1 tickets=1 auth_mode=required" \
            "$P_CLI force_version=tls12 event=1 tickets=1 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O: TLS 1.3 + ticket + client auth + resume" \
            "$P_SRV event=1 tickets=1 auth_mode=required" \
            "$P_CLI event=1 tickets=1 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O: TLS 1.2 + ticket + resume" \
            "$P_SRV event=1 tickets=1 auth_mode=none" \
            "$P_CLI force_version=tls12 event=1 tickets=1 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O: TLS 1.3 + ticket + resume" \
            "$P_SRV event=1 tickets=1 auth_mode=none" \
            "$P_CLI event=1 tickets=1 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Event-driven I/O: session-id resume" \
            "$P_SRV event=1 tickets=0 auth_mode=none" \
            "$P_CLI force_version=tls12 event=1 tickets=0 reconnect=1" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Event-driven I/O, DTLS: basic handshake" \
            "$P_SRV dtls=1 event=1 tickets=0 auth_mode=none" \
            "$P_CLI dtls=1 event=1 tickets=0" \
            0 \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Event-driven I/O, DTLS: client auth" \
            "$P_SRV dtls=1 event=1 tickets=0 auth_mode=required" \
            "$P_CLI dtls=1 event=1 tickets=0" \
            0 \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O, DTLS: ticket" \
            "$P_SRV dtls=1 event=1 tickets=1 auth_mode=none" \
            "$P_CLI dtls=1 event=1 tickets=1" \
            0 \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O, DTLS: ticket + client auth" \
            "$P_SRV dtls=1 event=1 tickets=1 auth_mode=required" \
            "$P_CLI dtls=1 event=1 tickets=1" \
            0 \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O, DTLS: ticket + client auth + resume" \
            "$P_SRV dtls=1 event=1 tickets=1 auth_mode=required" \
            "$P_CLI dtls=1 event=1 tickets=1 reconnect=1 skip_close_notify=1" \
            0 \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "Event-driven I/O, DTLS: ticket + resume" \
            "$P_SRV dtls=1 event=1 tickets=1 auth_mode=none" \
            "$P_CLI dtls=1 event=1 tickets=1 reconnect=1 skip_close_notify=1" \
            0 \
            -c "Read from server: .* bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Event-driven I/O, DTLS: session-id resume" \
            "$P_SRV dtls=1 event=1 tickets=0 auth_mode=none" \
            "$P_CLI dtls=1 event=1 tickets=0 reconnect=1 skip_close_notify=1" \
            0 \
            -c "Read from server: .* bytes read"

# This test demonstrates the need for the mbedtls_ssl_check_pending function.
# During session resumption, the client will send its ApplicationData record
# within the same datagram as the Finished messages. In this situation, the
# server MUST NOT idle on the underlying transport after handshake completion,
# because the ApplicationData request has already been queued internally.
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Event-driven I/O, DTLS: session-id resume, UDP packing" \
            -p "$P_PXY pack=50" \
            "$P_SRV dtls=1 event=1 tickets=0 auth_mode=required" \
            "$P_CLI dtls=1 event=1 tickets=0 reconnect=1 skip_close_notify=1" \
            0 \
            -c "Read from server: .* bytes read"

# Tests for version negotiation. Some information to ease the understanding
# of the version negotiation test titles below:
# . 1.2/1.3 means that only TLS 1.2/TLS 1.3 is enabled.
# . 1.2+1.3 means that both TLS 1.2 and TLS 1.3 are enabled.
# . 1.2+(1.3)/(1.2)+1.3 means that TLS 1.2/1.3 is enabled and that
#   TLS 1.3/1.2 may be enabled or not.
# . max=1.2 means that both TLS 1.2 and TLS 1.3 are enabled at build time but
#   TLS 1.3 is disabled at runtime (maximum negotiable version is TLS 1.2).
# . min=1.3 means that both TLS 1.2 and TLS 1.3 are enabled at build time but
#   TLS 1.2 is disabled at runtime (minimum negotiable version is TLS 1.3).

# Tests for version negotiation, MbedTLS client and server

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_disabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Version nego m->m: cli 1.2, srv 1.2 -> 1.2" \
            "$P_SRV" \
            "$P_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2" \
            -c "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Version nego m->m: cli max=1.2, srv max=1.2 -> 1.2" \
            "$P_SRV max_version=tls12" \
            "$P_CLI max_version=tls12" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2" \
            -c "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_disabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Version nego m->m: cli 1.3, srv 1.3 -> 1.3" \
            "$P_SRV" \
            "$P_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Version nego m->m: cli min=1.3, srv min=1.3 -> 1.3" \
            "$P_SRV min_version=tls13" \
            "$P_CLI min_version=tls13" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Version nego m->m: cli 1.2+1.3, srv 1.2+1.3 -> 1.3" \
            "$P_SRV" \
            "$P_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Version nego m->m: cli 1.2+1.3, srv min=1.3 -> 1.3" \
            "$P_SRV min_version=tls13" \
            "$P_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Version nego m->m: cli 1.2+1.3, srv max=1.2 -> 1.2" \
            "$P_SRV max_version=tls12" \
            "$P_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2" \
            -c "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Version nego m->m: cli max=1.2, srv 1.2+1.3 -> 1.2" \
            "$P_SRV" \
            "$P_CLI max_version=tls12" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2" \
            -c "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Version nego m->m: cli min=1.3, srv 1.2+1.3 -> 1.3" \
            "$P_SRV" \
            "$P_CLI min_version=tls13" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -C "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Not supported version m->m: cli max=1.2, srv min=1.3" \
            "$P_SRV min_version=tls13" \
            "$P_CLI max_version=tls12" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.2" \
            -C "Protocol is TLSv1.2" \
            -S "Protocol is TLSv1.3" \
            -C "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Not supported version m->m: cli min=1.3, srv max=1.2" \
            "$P_SRV max_version=tls12" \
            "$P_CLI min_version=tls13" \
            1 \
            -s "The handshake negotiation failed" \
            -S "Protocol is TLSv1.2" \
            -C "Protocol is TLSv1.2" \
            -S "Protocol is TLSv1.3" \
            -C "Protocol is TLSv1.3"

# Tests of version negotiation on server side against GnuTLS client

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Server version nego G->m: cli 1.2, srv 1.2+(1.3) -> 1.2" \
            "$P_SRV" \
            "$G_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Server version nego G->m: cli 1.2, srv max=1.2 -> 1.2" \
            "$P_SRV max_version=tls12" \
            "$G_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego G->m: cli 1.3, srv (1.2)+1.3 -> 1.3" \
            "$P_SRV" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego G->m: cli 1.3, srv min=1.3 -> 1.3" \
            "$P_SRV min_version=tls13" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego G->m: cli 1.2+1.3, srv (1.2)+1.3 -> 1.3" \
            "$P_SRV" \
            "$G_NEXT_CLI localhost --priority=NORMAL" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego G->m (no compat): cli 1.2+1.3, srv (1.2)+1.3 -> 1.3" \
            "$P_SRV" \
            "$G_NEXT_CLI localhost --priority=NORMAL:%DISABLE_TLS13_COMPAT_MODE" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

# GnuTLS can be setup to send a ClientHello containing a supported versions
# extension proposing TLS 1.2 (preferred) and then TLS 1.3. In that case,
# a TLS 1.3 and TLS 1.2 capable server is supposed to negotiate TLS 1.2 and
# to indicate in the ServerHello that it downgrades from TLS 1.3. The GnuTLS
# client then detects the downgrade indication and aborts the handshake even
# if TLS 1.2 was its preferred version. Keeping the test even if the
# handshake fails eventually as it exercices parts of the Mbed TLS
# implementation that are otherwise not exercised.
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Server version nego G->m: cli 1.2+1.3 (1.2 preferred!), srv 1.2+1.3 -> 1.2" \
            "$P_SRV" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:+VERS-TLS1.3" \
            1 \
            -c "Detected downgrade to TLS 1.2 from TLS 1.3"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego G->m: cli 1.2+1.3, srv min=1.3 -> 1.3" \
            "$P_SRV min_version=tls13" \
            "$G_NEXT_CLI localhost --priority=NORMAL" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_disabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Server version nego G->m: cli 1.2+1.3, srv 1.2 -> 1.2" \
            "$P_SRV" \
            "$G_NEXT_CLI localhost --priority=NORMAL" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Server version nego G->m: cli 1.2+1.3, max=1.2 -> 1.2" \
            "$P_SRV max_version=tls12" \
            "$G_NEXT_CLI localhost --priority=NORMAL" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
run_test    "Not supported version G->m: cli 1.0, (1.2)+(1.3)" \
            "$P_SRV" \
            "$G_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.0" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.0"

requires_config_enabled MBEDTLS_SSL_SRV_C
run_test    "Not supported version G->m: cli 1.1, (1.2)+(1.3)" \
            "$P_SRV" \
            "$G_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.1" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.1"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_disabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Not supported version G->m: cli 1.2, srv 1.3" \
            "$P_SRV" \
            "$G_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_disabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Not supported version G->m: cli 1.3, srv 1.2" \
            "$P_SRV" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3" \
            1 \
            -S "Handshake protocol not within min/max boundaries" \
            -s "The handshake negotiation failed" \
            -S "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Not supported version G->m: cli 1.2, srv min=1.3" \
            "$P_SRV min_version=tls13" \
            "$G_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Not supported version G->m: cli 1.3, srv max=1.2" \
            "$P_SRV max_version=tls12" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3" \
            1 \
            -S "Handshake protocol not within min/max boundaries" \
            -s "The handshake negotiation failed" \
            -S "Protocol is TLSv1.3"

# Tests of version negotiation on server side against OpenSSL client

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Server version nego O->m: cli 1.2, srv 1.2+(1.3) -> 1.2" \
            "$P_SRV" \
            "$O_NEXT_CLI -tls1_2" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Server version nego O->m: cli 1.2, srv max=1.2 -> 1.2" \
            "$P_SRV max_version=tls12" \
            "$O_NEXT_CLI -tls1_2" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego O->m: cli 1.3, srv (1.2)+1.3 -> 1.3" \
            "$P_SRV" \
            "$O_NEXT_CLI -tls1_3" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego O->m: cli 1.3, srv min=1.3 -> 1.3" \
            "$P_SRV min_version=tls13" \
            "$O_NEXT_CLI -tls1_3" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego O->m: cli 1.2+1.3, srv (1.2)+1.3 -> 1.3" \
            "$P_SRV" \
            "$O_NEXT_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego O->m (no compat): cli 1.2+1.3, srv (1.2)+1.3 -> 1.3" \
            "$P_SRV" \
            "$O_NEXT_CLI -no_middlebox" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Server version nego O->m: cli 1.2+1.3, srv min=1.3 -> 1.3" \
            "$P_SRV min_version=tls13" \
            "$O_NEXT_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_disabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Server version nego O->m: cli 1.2+1.3, srv 1.2 -> 1.2" \
            "$P_SRV" \
            "$O_NEXT_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Server version nego O->m: cli 1.2+1.3, srv max=1.2 -> 1.2" \
            "$P_SRV max_version=tls12" \
            "$O_NEXT_CLI" \
            0 \
            -S "mbedtls_ssl_handshake returned" \
            -s "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
run_test    "Not supported version O->m: cli 1.0, srv (1.2)+(1.3)" \
            "$P_SRV" \
            "$O_CLI -tls1" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.0"

requires_config_enabled MBEDTLS_SSL_SRV_C
run_test    "Not supported version O->m: cli 1.1, srv (1.2)+(1.3)" \
            "$P_SRV" \
            "$O_CLI -tls1_1" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.1"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_disabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Not supported version O->m: cli 1.2, srv 1.3" \
            "$P_SRV" \
            "$O_NEXT_CLI -tls1_2" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_disabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Not supported version O->m: cli 1.3, srv 1.2" \
            "$P_SRV" \
            "$O_NEXT_CLI -tls1_3" \
            1 \
            -S "Handshake protocol not within min/max boundaries" \
            -s "The handshake negotiation failed" \
            -S "Protocol is TLSv1.3"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Not supported version O->m: cli 1.2, srv min=1.3" \
            "$P_SRV min_version=tls13" \
            "$O_NEXT_CLI -tls1_2" \
            1 \
            -s "Handshake protocol not within min/max boundaries" \
            -S "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
run_test    "Not supported version O->m: cli 1.3, srv max=1.2" \
            "$P_SRV max_version=tls12" \
            "$O_NEXT_CLI -tls1_3" \
            1 \
            -S "Handshake protocol not within min/max boundaries" \
            -s "The handshake negotiation failed" \
            -S "Protocol is TLSv1.3"

# Tests of version negotiation on client side against GnuTLS and OpenSSL server

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Not supported version: srv max TLS 1.0" \
            "$G_SRV --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.0" \
            "$P_CLI" \
            1 \
            -s "Error in protocol version" \
            -c "Handshake protocol not within min/max boundaries" \
            -S "Version: TLS1.0" \
            -C "Protocol is TLSv1.0"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "Not supported version: srv max TLS 1.1" \
            "$G_SRV --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.1" \
            "$P_CLI" \
            1 \
            -s "Error in protocol version" \
            -c "Handshake protocol not within min/max boundaries" \
            -S "Version: TLS1.1" \
            -C "Protocol is TLSv1.1"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
skip_handshake_stage_check
requires_gnutls_tls1_3
run_test    "TLS 1.3: Not supported version:gnutls: srv max TLS 1.0" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.0 -d 4" \
            "$P_CLI debug_level=4" \
            1 \
            -s "Client's version: 3.3" \
            -S "Version: TLS1.0" \
            -C "Protocol is TLSv1.0"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
skip_handshake_stage_check
requires_gnutls_tls1_3
run_test    "TLS 1.3: Not supported version:gnutls: srv max TLS 1.1" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.1 -d 4" \
            "$P_CLI debug_level=4" \
            1 \
            -s "Client's version: 3.3" \
            -S "Version: TLS1.1" \
            -C "Protocol is TLSv1.1"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
skip_handshake_stage_check
requires_gnutls_tls1_3
run_test    "TLS 1.3: Not supported version:gnutls: srv max TLS 1.2" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.2 -d 4" \
            "$P_CLI force_version=tls13 debug_level=4" \
            1 \
            -s "Client's version: 3.3" \
            -c "is a fatal alert message (msg 40)" \
            -S "Version: TLS1.2" \
            -C "Protocol is TLSv1.2"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
skip_handshake_stage_check
requires_openssl_next
run_test    "TLS 1.3: Not supported version:openssl: srv max TLS 1.0" \
            "$O_NEXT_SRV -msg -tls1" \
            "$P_CLI debug_level=4" \
            1 \
            -s "fatal protocol_version" \
            -c "is a fatal alert message (msg 70)" \
            -S "Version: TLS1.0" \
            -C "Protocol  : TLSv1.0"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
skip_handshake_stage_check
requires_openssl_next
run_test    "TLS 1.3: Not supported version:openssl: srv max TLS 1.1" \
            "$O_NEXT_SRV -msg -tls1_1" \
            "$P_CLI debug_level=4" \
            1 \
            -s "fatal protocol_version" \
            -c "is a fatal alert message (msg 70)" \
            -S "Version: TLS1.1" \
            -C "Protocol  : TLSv1.1"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
skip_handshake_stage_check
requires_openssl_next
run_test    "TLS 1.3: Not supported version:openssl: srv max TLS 1.2" \
            "$O_NEXT_SRV -msg -tls1_2" \
            "$P_CLI force_version=tls13 debug_level=4" \
            1 \
            -s "fatal protocol_version" \
            -c "is a fatal alert message (msg 70)" \
            -S "Version: TLS1.2" \
            -C "Protocol  : TLSv1.2"

# Tests for ALPN extension

run_test    "ALPN: none" \
            "$P_SRV debug_level=3" \
            "$P_CLI debug_level=3" \
            0 \
            -C "client hello, adding alpn extension" \
            -S "found alpn extension" \
            -C "got an alert message, type: \\[2:120]" \
            -S "server side, adding alpn extension" \
            -C "found alpn extension " \
            -C "Application Layer Protocol is" \
            -S "Application Layer Protocol is"

run_test    "ALPN: client only" \
            "$P_SRV debug_level=3" \
            "$P_CLI debug_level=3 alpn=abc,1234" \
            0 \
            -c "client hello, adding alpn extension" \
            -s "found alpn extension" \
            -C "got an alert message, type: \\[2:120]" \
            -S "server side, adding alpn extension" \
            -C "found alpn extension " \
            -c "Application Layer Protocol is (none)" \
            -S "Application Layer Protocol is"

run_test    "ALPN: server only" \
            "$P_SRV debug_level=3 alpn=abc,1234" \
            "$P_CLI debug_level=3" \
            0 \
            -C "client hello, adding alpn extension" \
            -S "found alpn extension" \
            -C "got an alert message, type: \\[2:120]" \
            -S "server side, adding alpn extension" \
            -C "found alpn extension " \
            -C "Application Layer Protocol is" \
            -s "Application Layer Protocol is (none)"

run_test    "ALPN: both, common cli1-srv1" \
            "$P_SRV debug_level=3 alpn=abc,1234" \
            "$P_CLI debug_level=3 alpn=abc,1234" \
            0 \
            -c "client hello, adding alpn extension" \
            -s "found alpn extension" \
            -C "got an alert message, type: \\[2:120]" \
            -s "server side, adding alpn extension" \
            -c "found alpn extension" \
            -c "Application Layer Protocol is abc" \
            -s "Application Layer Protocol is abc"

run_test    "ALPN: both, common cli2-srv1" \
            "$P_SRV debug_level=3 alpn=abc,1234" \
            "$P_CLI debug_level=3 alpn=1234,abc" \
            0 \
            -c "client hello, adding alpn extension" \
            -s "found alpn extension" \
            -C "got an alert message, type: \\[2:120]" \
            -s "server side, adding alpn extension" \
            -c "found alpn extension" \
            -c "Application Layer Protocol is abc" \
            -s "Application Layer Protocol is abc"

run_test    "ALPN: both, common cli1-srv2" \
            "$P_SRV debug_level=3 alpn=abc,1234" \
            "$P_CLI debug_level=3 alpn=1234,abcde" \
            0 \
            -c "client hello, adding alpn extension" \
            -s "found alpn extension" \
            -C "got an alert message, type: \\[2:120]" \
            -s "server side, adding alpn extension" \
            -c "found alpn extension" \
            -c "Application Layer Protocol is 1234" \
            -s "Application Layer Protocol is 1234"

run_test    "ALPN: both, no common" \
            "$P_SRV debug_level=3 alpn=abc,123" \
            "$P_CLI debug_level=3 alpn=1234,abcde" \
            1 \
            -c "client hello, adding alpn extension" \
            -s "found alpn extension" \
            -c "got an alert message, type: \\[2:120]" \
            -S "server side, adding alpn extension" \
            -C "found alpn extension" \
            -C "Application Layer Protocol is 1234" \
            -S "Application Layer Protocol is 1234"


# Tests for keyUsage in leaf certificates, part 1:
# server-side certificate/suite selection
#
# This is only about 1.2 (for 1.3, all key exchanges use signatures).
# In 4.0 this will probably go away as all TLS 1.2 key exchanges will use
# signatures too, following the removal of RSA #8170 and static ECDH #9201.

run_test    "keyUsage srv 1.2: RSA, digitalSignature -> ECDHE-RSA" \
            "$P_SRV force_version=tls12 key_file=$DATA_FILES_PATH/server2.key \
             crt_file=$DATA_FILES_PATH/server2.ku-ds.crt" \
            "$P_CLI" \
            0 \
            -c "Ciphersuite is TLS-ECDHE-RSA-WITH-"

run_test    "keyUsage srv 1.2: RSA, keyEncipherment -> fail" \
            "$P_SRV force_version=tls12 key_file=$DATA_FILES_PATH/server2.key \
             crt_file=$DATA_FILES_PATH/server2.ku-ke.crt" \
            "$P_CLI" \
            1 \
            -C "Ciphersuite is "

run_test    "keyUsage srv 1.2: RSA, keyAgreement -> fail" \
            "$P_SRV force_version=tls12 key_file=$DATA_FILES_PATH/server2.key \
             crt_file=$DATA_FILES_PATH/server2.ku-ka.crt" \
            "$P_CLI" \
            1 \
            -C "Ciphersuite is "

requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
run_test    "keyUsage srv 1.2: ECC, digitalSignature -> ECDHE-ECDSA" \
            "$P_SRV force_version=tls12 key_file=$DATA_FILES_PATH/server5.key \
             crt_file=$DATA_FILES_PATH/server5.ku-ds.crt" \
            "$P_CLI" \
            0 \
            -c "Ciphersuite is TLS-ECDHE-ECDSA-WITH-"

run_test    "keyUsage srv 1.2: ECC, keyEncipherment -> fail" \
            "$P_SRV force_version=tls12 key_file=$DATA_FILES_PATH/server5.key \
             crt_file=$DATA_FILES_PATH/server5.ku-ke.crt" \
            "$P_CLI" \
            1 \
            -C "Ciphersuite is "

# Tests for keyUsage in leaf certificates, part 2:
# client-side checking of server cert

run_test    "keyUsage cli 1.2: DigitalSignature+KeyEncipherment, ECDHE-RSA: OK" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2.ku-ds_ke.crt" \
            "$P_CLI debug_level=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is TLS-"

run_test    "keyUsage cli 1.2: KeyEncipherment, ECDHE-RSA: fail (hard)" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2.ku-ke.crt" \
            "$P_CLI debug_level=3 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            1 \
            -c "bad certificate (usage extensions)" \
            -c "Processing of the Certificate handshake message failed" \
            -C "Ciphersuite is TLS-" \
            -c "send alert level=2 message=43" \
            -c "! Usage does not match the keyUsage extension"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

run_test    "keyUsage cli 1.2: KeyEncipherment, ECDHE-RSA: fail (soft)" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2.ku-ke.crt" \
            "$P_CLI debug_level=3 auth_mode=optional \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            0 \
            -c "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is TLS-" \
            -C "send alert level=2 message=43" \
            -c "! Usage does not match the keyUsage extension"

run_test    "keyUsage cli 1.2: DigitalSignature, ECDHE-RSA: OK" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2.ku-ds.crt" \
            "$P_CLI debug_level=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is TLS-"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli 1.3: DigitalSignature, RSA: OK" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2-sha256.ku-ds.crt" \
            "$P_CLI debug_level=3" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli 1.3: DigitalSignature+KeyEncipherment, RSA: OK" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2-sha256.ku-ds_ke.crt" \
            "$P_CLI debug_level=3" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli 1.3: KeyEncipherment, RSA: fail (hard)" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2-sha256.ku-ke.crt" \
            "$P_CLI debug_level=3" \
            1 \
            -c "bad certificate (usage extensions)" \
            -c "Processing of the Certificate handshake message failed" \
            -C "Ciphersuite is" \
            -c "send alert level=2 message=43" \
            -c "! Usage does not match the keyUsage extension"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli 1.3: KeyAgreement, RSA: fail (hard)" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2-sha256.ku-ka.crt" \
            "$P_CLI debug_level=3" \
            1 \
            -c "bad certificate (usage extensions)" \
            -c "Processing of the Certificate handshake message failed" \
            -C "Ciphersuite is" \
            -c "send alert level=2 message=43" \
            -c "! Usage does not match the keyUsage extension"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli 1.3: DigitalSignature, ECDSA: OK" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.ku-ds.crt" \
            "$P_CLI debug_level=3" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli 1.3: KeyEncipherment, ECDSA: fail (hard)" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.ku-ke.crt" \
            "$P_CLI debug_level=3" \
            1 \
            -c "bad certificate (usage extensions)" \
            -c "Processing of the Certificate handshake message failed" \
            -C "Ciphersuite is" \
            -c "send alert level=2 message=43" \
            -c "! Usage does not match the keyUsage extension"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli 1.3: KeyAgreement, ECDSA: fail (hard)" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.ku-ka.crt" \
            "$P_CLI debug_level=3" \
            1 \
            -c "bad certificate (usage extensions)" \
            -c "Processing of the Certificate handshake message failed" \
            -C "Ciphersuite is" \
            -c "send alert level=2 message=43" \
            -c "! Usage does not match the keyUsage extension"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

# Tests for keyUsage in leaf certificates, part 3:
# server-side checking of client cert
#
# Here, both 1.2 and 1.3 only use signatures.

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "keyUsage cli-auth 1.2: RSA, DigitalSignature: OK" \
            "$P_SRV debug_level=1 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2.ku-ds.crt" \
            0 \
            -s "Verifying peer X.509 certificate... ok" \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "keyUsage cli-auth 1.2: RSA, DigitalSignature+KeyEncipherment: OK" \
            "$P_SRV debug_level=1 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2.ku-ds_ke.crt" \
            0 \
            -s "Verifying peer X.509 certificate... ok" \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "keyUsage cli-auth 1.2: RSA, KeyEncipherment: fail (soft)" \
            "$P_SRV debug_level=3 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2.ku-ke.crt" \
            0 \
            -s "bad certificate (usage extensions)" \
            -S "send alert level=2 message=43" \
            -s "! Usage does not match the keyUsage extension" \
            -S "Processing of the Certificate handshake message failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "keyUsage cli-auth 1.2: RSA, KeyEncipherment: fail (hard)" \
            "$P_SRV debug_level=3 force_version=tls12 auth_mode=required" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2.ku-ke.crt" \
            1 \
            -s "bad certificate (usage extensions)" \
            -s "send alert level=2 message=43" \
            -s "! Usage does not match the keyUsage extension" \
            -s "Processing of the Certificate handshake message failed"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "keyUsage cli-auth 1.2: ECDSA, DigitalSignature: OK" \
            "$P_SRV debug_level=1 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.ku-ds.crt" \
            0 \
            -s "Verifying peer X.509 certificate... ok" \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "keyUsage cli-auth 1.2: ECDSA, KeyAgreement: fail (soft)" \
            "$P_SRV debug_level=3 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.ku-ka.crt" \
            0 \
            -s "bad certificate (usage extensions)" \
            -S "send alert level=2 message=43" \
            -s "! Usage does not match the keyUsage extension" \
            -S "Processing of the Certificate handshake message failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "keyUsage cli-auth 1.2: ECDSA, KeyAgreement: fail (hard)" \
            "$P_SRV debug_level=3 auth_mode=required" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.ku-ka.crt" \
            1 \
            -s "bad certificate (usage extensions)" \
            -s "send alert level=2 message=43" \
            -s "! Usage does not match the keyUsage extension" \
            -s "Processing of the Certificate handshake message failed"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli-auth 1.3: RSA, DigitalSignature: OK" \
            "$P_SRV debug_level=1 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2-sha256.ku-ds.crt" \
            0 \
            -s "Verifying peer X.509 certificate... ok" \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli-auth 1.3: RSA, DigitalSignature+KeyEncipherment: OK" \
            "$P_SRV debug_level=1 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2-sha256.ku-ds_ke.crt" \
            0 \
            -s "Verifying peer X.509 certificate... ok" \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli-auth 1.3: RSA, KeyEncipherment: fail (soft)" \
            "$P_SRV debug_level=3 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server2.key \
             -cert $DATA_FILES_PATH/server2-sha256.ku-ke.crt" \
            0 \
            -s "bad certificate (usage extensions)" \
            -S "send alert level=2 message=43" \
            -s "! Usage does not match the keyUsage extension" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli-auth 1.3: RSA, KeyEncipherment: fail (hard)" \
            "$P_SRV debug_level=3 force_version=tls13 auth_mode=required" \
            "$P_CLI key_file=$DATA_FILES_PATH/server2.key \
             crt_file=$DATA_FILES_PATH/server2-sha256.ku-ke.crt" \
            1 \
            -s "bad certificate (usage extensions)" \
            -s "Processing of the Certificate handshake message failed" \
            -s "send alert level=2 message=43" \
            -s "! Usage does not match the keyUsage extension" \
            -s "! mbedtls_ssl_handshake returned"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli-auth 1.3: ECDSA, DigitalSignature: OK" \
            "$P_SRV debug_level=1 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.ku-ds.crt" \
            0 \
            -s "Verifying peer X.509 certificate... ok" \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli-auth 1.3: ECDSA, KeyAgreement: fail (soft)" \
            "$P_SRV debug_level=3 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.ku-ka.crt" \
            0 \
            -s "bad certificate (usage extensions)" \
            -s "! Usage does not match the keyUsage extension" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "keyUsage cli-auth 1.3: ECDSA, KeyAgreement: fail (hard)" \
            "$P_SRV debug_level=3 force_version=tls13 auth_mode=required" \
            "$P_CLI key_file=$DATA_FILES_PATH/server5.key \
             crt_file=$DATA_FILES_PATH/server5.ku-ka.crt" \
            1 \
            -s "bad certificate (usage extensions)" \
            -s "Processing of the Certificate handshake message failed" \
            -s "send alert level=2 message=43" \
            -s "! Usage does not match the keyUsage extension" \
            -s "! mbedtls_ssl_handshake returned"
            # MBEDTLS_X509_BADCERT_KEY_USAGE -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

# Tests for extendedKeyUsage, part 1: server-side certificate/suite selection

run_test    "extKeyUsage srv: serverAuth -> OK" \
            "$P_SRV key_file=$DATA_FILES_PATH/server5.key \
             crt_file=$DATA_FILES_PATH/server5.eku-srv.crt" \
            "$P_CLI" \
            0

run_test    "extKeyUsage srv: serverAuth,clientAuth -> OK" \
            "$P_SRV key_file=$DATA_FILES_PATH/server5.key \
             crt_file=$DATA_FILES_PATH/server5.eku-srv.crt" \
            "$P_CLI" \
            0

run_test    "extKeyUsage srv: codeSign,anyEKU -> OK" \
            "$P_SRV key_file=$DATA_FILES_PATH/server5.key \
             crt_file=$DATA_FILES_PATH/server5.eku-cs_any.crt" \
            "$P_CLI" \
            0

run_test    "extKeyUsage srv: codeSign -> fail" \
            "$P_SRV key_file=$DATA_FILES_PATH/server5.key \
             crt_file=$DATA_FILES_PATH/server5.eku-cli.crt" \
            "$P_CLI" \
            1

# Tests for extendedKeyUsage, part 2: client-side checking of server cert

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli 1.2: serverAuth -> OK" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-srv.crt" \
            "$P_CLI debug_level=1" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is TLS-"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli 1.2: serverAuth,clientAuth -> OK" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-srv_cli.crt" \
            "$P_CLI debug_level=1" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is TLS-"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli 1.2: codeSign,anyEKU -> OK" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs_any.crt" \
            "$P_CLI debug_level=1" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is TLS-"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli 1.2: codeSign -> fail (soft)" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs.crt" \
            "$P_CLI debug_level=3 auth_mode=optional" \
            0 \
            -c "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is TLS-" \
            -C "send alert level=2 message=43" \
            -c "! Usage does not match the extendedKeyUsage extension"
            # MBEDTLS_X509_BADCERT_EXT_KEY_USAGE  -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli 1.2: codeSign -> fail (hard)" \
            "$O_SRV -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs.crt" \
            "$P_CLI debug_level=3" \
            1 \
            -c "bad certificate (usage extensions)" \
            -c "Processing of the Certificate handshake message failed" \
            -C "Ciphersuite is TLS-" \
            -c "send alert level=2 message=43" \
            -c "! Usage does not match the extendedKeyUsage extension"
            # MBEDTLS_X509_BADCERT_EXT_KEY_USAGE  -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli 1.3: serverAuth -> OK" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-srv.crt" \
            "$P_CLI debug_level=1" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli 1.3: serverAuth,clientAuth -> OK" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-srv_cli.crt" \
            "$P_CLI debug_level=1" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli 1.3: codeSign,anyEKU -> OK" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs_any.crt" \
            "$P_CLI debug_level=1" \
            0 \
            -C "bad certificate (usage extensions)" \
            -C "Processing of the Certificate handshake message failed" \
            -c "Ciphersuite is"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli 1.3: codeSign -> fail (hard)" \
            "$O_NEXT_SRV_NO_CERT -tls1_3 -num_tickets=0 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs.crt" \
            "$P_CLI debug_level=3" \
            1 \
            -c "bad certificate (usage extensions)" \
            -c "Processing of the Certificate handshake message failed" \
            -C "Ciphersuite is" \
            -c "send alert level=2 message=43" \
            -c "! Usage does not match the extendedKeyUsage extension"
            # MBEDTLS_X509_BADCERT_EXT_KEY_USAGE  -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

# Tests for extendedKeyUsage, part 3: server-side checking of client cert

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli-auth 1.2: clientAuth -> OK" \
            "$P_SRV debug_level=1 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cli.crt" \
            0 \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli-auth 1.2: serverAuth,clientAuth -> OK" \
            "$P_SRV debug_level=1 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-srv_cli.crt" \
            0 \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli-auth 1.2: codeSign,anyEKU -> OK" \
            "$P_SRV debug_level=1 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs_any.crt" \
            0 \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli-auth 1.2: codeSign -> fail (soft)" \
            "$P_SRV debug_level=3 auth_mode=optional" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs.crt" \
            0 \
            -s "bad certificate (usage extensions)" \
            -S "send alert level=2 message=43" \
            -s "! Usage does not match the extendedKeyUsage extension" \
            -S "Processing of the Certificate handshake message failed" \

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "extKeyUsage cli-auth 1.2: codeSign -> fail (hard)" \
            "$P_SRV debug_level=3 auth_mode=required" \
            "$O_CLI -tls1_2 -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs.crt" \
            1 \
            -s "bad certificate (usage extensions)" \
            -s "send alert level=2 message=43" \
            -s "! Usage does not match the extendedKeyUsage extension" \
            -s "Processing of the Certificate handshake message failed"
            # MBEDTLS_X509_BADCERT_EXT_KEY_USAGE  -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli-auth 1.3: clientAuth -> OK" \
            "$P_SRV debug_level=1 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cli.crt" \
            0 \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli-auth 1.3: serverAuth,clientAuth -> OK" \
            "$P_SRV debug_level=1 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-srv_cli.crt" \
            0 \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli-auth 1.3: codeSign,anyEKU -> OK" \
            "$P_SRV debug_level=1 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs_any.crt" \
            0 \
            -S "bad certificate (usage extensions)" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli-auth 1.3: codeSign -> fail (soft)" \
            "$P_SRV debug_level=3 force_version=tls13 auth_mode=optional" \
            "$O_NEXT_CLI_NO_CERT -key $DATA_FILES_PATH/server5.key \
             -cert $DATA_FILES_PATH/server5.eku-cs.crt" \
            0 \
            -s "bad certificate (usage extensions)" \
            -S "send alert level=2 message=43" \
            -s "! Usage does not match the extendedKeyUsage extension" \
            -S "Processing of the Certificate handshake message failed"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "extKeyUsage cli-auth 1.3: codeSign -> fail (hard)" \
            "$P_SRV debug_level=3 force_version=tls13 auth_mode=required" \
            "$P_CLI key_file=$DATA_FILES_PATH/server5.key \
             crt_file=$DATA_FILES_PATH/server5.eku-cs.crt" \
            1 \
            -s "bad certificate (usage extensions)" \
            -s "send alert level=2 message=43" \
            -s "! Usage does not match the extendedKeyUsage extension" \
            -s "Processing of the Certificate handshake message failed"
            # MBEDTLS_X509_BADCERT_EXT_KEY_USAGE  -> MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT

# Tests for PSK callback

run_test    "PSK callback: psk, no callback" \
            "$P_SRV psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368" \
            0 \
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: opaque psk on client, no callback" \
            "$P_SRV extended_ms=0 debug_level=1 psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI extended_ms=0 debug_level=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368 psk_opaque=1" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: opaque psk on client, no callback, SHA-384" \
            "$P_SRV extended_ms=0 debug_level=1 psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI extended_ms=0 debug_level=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=foo psk=73776f726466697368 psk_opaque=1" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: opaque psk on client, no callback, EMS" \
            "$P_SRV extended_ms=1 debug_level=3 psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI extended_ms=1 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368 psk_opaque=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: opaque psk on client, no callback, SHA-384, EMS" \
            "$P_SRV extended_ms=1 debug_level=3 psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI extended_ms=1 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=foo psk=73776f726466697368 psk_opaque=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: opaque ecdhe-psk on client, no callback" \
            "$P_SRV extended_ms=0 debug_level=1 psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI extended_ms=0 debug_level=1 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA256 \
            psk_identity=foo psk=73776f726466697368 psk_opaque=1" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: opaque ecdhe-psk on client, no callback, SHA-384" \
            "$P_SRV extended_ms=0 debug_level=1 psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI extended_ms=0 debug_level=1 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=foo psk=73776f726466697368 psk_opaque=1" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: opaque ecdhe-psk on client, no callback, EMS" \
            "$P_SRV extended_ms=1 debug_level=3 psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI extended_ms=1 debug_level=3 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368 psk_opaque=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: opaque ecdhe-psk on client, no callback, SHA-384, EMS" \
            "$P_SRV extended_ms=1 debug_level=3 psk=73776f726466697368 psk_identity=foo" \
            "$P_CLI extended_ms=1 debug_level=3 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=foo psk=73776f726466697368 psk_opaque=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, static opaque on server, no callback" \
            "$P_SRV extended_ms=0 debug_level=1 psk=73776f726466697368 psk_identity=foo psk_opaque=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, static opaque on server, no callback, SHA-384" \
            "$P_SRV extended_ms=0 debug_level=1 psk=73776f726466697368 psk_identity=foo psk_opaque=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384" \
            "$P_CLI extended_ms=0 debug_level=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=foo psk=73776f726466697368" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, static opaque on server, no callback, EMS" \
            "$P_SRV debug_level=3 psk=73776f726466697368 psk_identity=foo psk_opaque=1 min_version=tls12 \
            force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA extended_ms=1" \
            "$P_CLI debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368 extended_ms=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, static opaque on server, no callback, EMS, SHA384" \
            "$P_SRV debug_level=3 psk=73776f726466697368 psk_identity=foo psk_opaque=1 min_version=tls12 \
            force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384 extended_ms=1" \
            "$P_CLI debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=foo psk=73776f726466697368 extended_ms=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw ecdhe-psk on client, static opaque on server, no callback" \
            "$P_SRV extended_ms=0 debug_level=5 psk=73776f726466697368 psk_identity=foo psk_opaque=1 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=5 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw ecdhe-psk on client, static opaque on server, no callback, SHA-384" \
            "$P_SRV extended_ms=0 debug_level=1 psk=73776f726466697368 psk_identity=foo psk_opaque=1 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384" \
            "$P_CLI extended_ms=0 debug_level=1 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=foo psk=73776f726466697368" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw ecdhe-psk on client, static opaque on server, no callback, EMS" \
            "$P_SRV debug_level=3 psk=73776f726466697368 psk_identity=foo psk_opaque=1 min_version=tls12 \
            force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA extended_ms=1" \
            "$P_CLI debug_level=3 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368 extended_ms=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw ecdhe-psk on client, static opaque on server, no callback, EMS, SHA384" \
            "$P_SRV debug_level=3 psk=73776f726466697368 psk_identity=foo psk_opaque=1 min_version=tls12 \
            force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384 extended_ms=1" \
            "$P_CLI debug_level=3 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=foo psk=73776f726466697368 extended_ms=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, no static PSK on server, opaque PSK from callback" \
            "$P_SRV extended_ms=0 debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=def psk=beef" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, no static PSK on server, opaque PSK from callback, SHA-384" \
            "$P_SRV extended_ms=0 debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=def psk=beef" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, no static PSK on server, opaque PSK from callback, EMS" \
            "$P_SRV debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 \
            force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA extended_ms=1" \
            "$P_CLI debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=abc psk=dead extended_ms=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, no static PSK on server, opaque PSK from callback, EMS, SHA384" \
            "$P_SRV debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 \
            force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384 extended_ms=1" \
            "$P_CLI debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=abc psk=dead extended_ms=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw ecdhe-psk on client, no static ECDHE-PSK on server, opaque ECDHE-PSK from callback" \
            "$P_SRV extended_ms=0 debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=def psk=beef" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw ecdhe-psk on client, no static ECDHE-PSK on server, opaque ECDHE-PSK from callback, SHA-384" \
            "$P_SRV extended_ms=0 debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=def psk=beef" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw ecdhe-psk on client, no static ECDHE-PSK on server, opaque ECDHE-PSK from callback, EMS" \
            "$P_SRV debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 \
            force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA extended_ms=1" \
            "$P_CLI debug_level=3 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=abc psk=dead extended_ms=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw ecdhe-psk on client, no static ECDHE-PSK on server, opaque ECDHE-PSK from callback, EMS, SHA384" \
            "$P_SRV debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 \
            force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384 extended_ms=1" \
            "$P_CLI debug_level=3 min_version=tls12 force_ciphersuite=TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384 \
            psk_identity=abc psk=dead extended_ms=1" \
            0 \
            -c "session hash for extended master secret"\
            -s "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, mismatching static raw PSK on server, opaque PSK from callback" \
            "$P_SRV extended_ms=0 psk_identity=foo psk=73776f726466697368 debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=def psk=beef" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, mismatching static opaque PSK on server, opaque PSK from callback" \
            "$P_SRV extended_ms=0 psk_opaque=1 psk_identity=foo psk=73776f726466697368 debug_level=3 psk_list=abc,dead,def,beef psk_list_opaque=1 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=def psk=beef" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, mismatching static opaque PSK on server, raw PSK from callback" \
            "$P_SRV extended_ms=0 psk_opaque=1 psk_identity=foo psk=73776f726466697368 debug_level=3 psk_list=abc,dead,def,beef min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=def psk=beef" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, id-matching but wrong raw PSK on server, opaque PSK from callback" \
            "$P_SRV extended_ms=0 psk_opaque=1 psk_identity=def psk=73776f726466697368 debug_level=3 psk_list=abc,dead,def,beef min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=def psk=beef" \
            0 \
            -C "session hash for extended master secret"\
            -S "session hash for extended master secret"\
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: raw psk on client, matching opaque PSK on server, wrong opaque PSK from callback" \
            "$P_SRV extended_ms=0 psk_opaque=1 psk_identity=def psk=beef debug_level=3 psk_list=abc,dead,def,73776f726466697368 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA" \
            "$P_CLI extended_ms=0 debug_level=3 min_version=tls12 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=def psk=beef" \
            1 \
            -s "SSL - Verification of the message MAC failed"

run_test    "PSK callback: no psk, no callback" \
            "$P_SRV" \
            "$P_CLI force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368" \
            1 \
            -s "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: callback overrides other settings" \
            "$P_SRV psk=73776f726466697368 psk_identity=foo psk_list=abc,dead,def,beef" \
            "$P_CLI force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=foo psk=73776f726466697368" \
            1 \
            -S "SSL - The handshake negotiation failed" \
            -s "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: first id matches" \
            "$P_SRV psk_list=abc,dead,def,beef" \
            "$P_CLI force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=abc psk=dead" \
            0 \
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: second id matches" \
            "$P_SRV psk_list=abc,dead,def,beef" \
            "$P_CLI force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=def psk=beef" \
            0 \
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: no match" \
            "$P_SRV psk_list=abc,dead,def,beef" \
            "$P_CLI force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=ghi psk=beef" \
            1 \
            -S "SSL - The handshake negotiation failed" \
            -s "SSL - Unknown identity received" \
            -S "SSL - Verification of the message MAC failed"

run_test    "PSK callback: wrong key" \
            "$P_SRV psk_list=abc,dead,def,beef" \
            "$P_CLI force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA \
            psk_identity=abc psk=beef" \
            1 \
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Unknown identity received" \
            -s "SSL - Verification of the message MAC failed"

# Tests for EC J-PAKE

requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "ECJPAKE: client not configured" \
            "$P_SRV debug_level=3" \
            "$P_CLI debug_level=3" \
            0 \
            -C "add ciphersuite: 0xc0ff" \
            -C "adding ecjpake_kkpp extension" \
            -S "found ecjpake kkpp extension" \
            -S "skip ecjpake kkpp extension" \
            -S "ciphersuite mismatch: ecjpake not configured" \
            -S "server hello, ecjpake kkpp extension" \
            -C "found ecjpake_kkpp extension" \
            -S "SSL - The handshake negotiation failed"

requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: server not configured" \
            "$P_SRV debug_level=3" \
            "$P_CLI debug_level=3 ecjpake_pw=bla \
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            1 \
            -c "add ciphersuite: c0ff" \
            -c "adding ecjpake_kkpp extension" \
            -s "found ecjpake kkpp extension" \
            -s "skip ecjpake kkpp extension" \
            -s "ciphersuite mismatch: ecjpake not configured" \
            -S "server hello, ecjpake kkpp extension" \
            -C "found ecjpake_kkpp extension" \
            -s "SSL - The handshake negotiation failed"

# Note: if the name of this test is changed, then please adjust the corresponding
#       filtering label in "test_tls1_2_ecjpake_compatibility" (in "all.sh")
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: working, TLS" \
            "$P_SRV debug_level=3 ecjpake_pw=bla" \
            "$P_CLI debug_level=3 ecjpake_pw=bla \
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            0 \
            -c "add ciphersuite: c0ff" \
            -c "adding ecjpake_kkpp extension" \
            -C "re-using cached ecjpake parameters" \
            -s "found ecjpake kkpp extension" \
            -S "skip ecjpake kkpp extension" \
            -S "ciphersuite mismatch: ecjpake not configured" \
            -s "server hello, ecjpake kkpp extension" \
            -c "found ecjpake_kkpp extension" \
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Verification of the message MAC failed"

requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: opaque password client+server, working, TLS" \
            "$P_SRV debug_level=3 ecjpake_pw=bla ecjpake_pw_opaque=1" \
            "$P_CLI debug_level=3 ecjpake_pw=bla ecjpake_pw_opaque=1\
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            0 \
            -c "add ciphersuite: c0ff" \
            -c "adding ecjpake_kkpp extension" \
            -c "using opaque password" \
            -s "using opaque password" \
            -C "re-using cached ecjpake parameters" \
            -s "found ecjpake kkpp extension" \
            -S "skip ecjpake kkpp extension" \
            -S "ciphersuite mismatch: ecjpake not configured" \
            -s "server hello, ecjpake kkpp extension" \
            -c "found ecjpake_kkpp extension" \
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Verification of the message MAC failed"

# Note: if the name of this test is changed, then please adjust the corresponding
#       filtering label in "test_tls1_2_ecjpake_compatibility" (in "all.sh")
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: opaque password client only, working, TLS" \
            "$P_SRV debug_level=3 ecjpake_pw=bla" \
            "$P_CLI debug_level=3 ecjpake_pw=bla ecjpake_pw_opaque=1\
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            0 \
            -c "add ciphersuite: c0ff" \
            -c "adding ecjpake_kkpp extension" \
            -c "using opaque password" \
            -S "using opaque password" \
            -C "re-using cached ecjpake parameters" \
            -s "found ecjpake kkpp extension" \
            -S "skip ecjpake kkpp extension" \
            -S "ciphersuite mismatch: ecjpake not configured" \
            -s "server hello, ecjpake kkpp extension" \
            -c "found ecjpake_kkpp extension" \
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Verification of the message MAC failed"

# Note: if the name of this test is changed, then please adjust the corresponding
#       filtering label in "test_tls1_2_ecjpake_compatibility" (in "all.sh")
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: opaque password server only, working, TLS" \
            "$P_SRV debug_level=3 ecjpake_pw=bla ecjpake_pw_opaque=1" \
            "$P_CLI debug_level=3 ecjpake_pw=bla\
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            0 \
            -c "add ciphersuite: c0ff" \
            -c "adding ecjpake_kkpp extension" \
            -C "using opaque password" \
            -s "using opaque password" \
            -C "re-using cached ecjpake parameters" \
            -s "found ecjpake kkpp extension" \
            -S "skip ecjpake kkpp extension" \
            -S "ciphersuite mismatch: ecjpake not configured" \
            -s "server hello, ecjpake kkpp extension" \
            -c "found ecjpake_kkpp extension" \
            -S "SSL - The handshake negotiation failed" \
            -S "SSL - Verification of the message MAC failed"

server_needs_more_time 1
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: password mismatch, TLS" \
            "$P_SRV debug_level=3 ecjpake_pw=bla" \
            "$P_CLI debug_level=3 ecjpake_pw=bad \
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            1 \
            -C "re-using cached ecjpake parameters" \
            -s "SSL - Verification of the message MAC failed"

server_needs_more_time 1
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE_OPAQUE_PW: opaque password mismatch, TLS" \
            "$P_SRV debug_level=3 ecjpake_pw=bla ecjpake_pw_opaque=1" \
            "$P_CLI debug_level=3 ecjpake_pw=bad ecjpake_pw_opaque=1 \
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            1 \
            -c "using opaque password" \
            -s "using opaque password" \
            -C "re-using cached ecjpake parameters" \
            -s "SSL - Verification of the message MAC failed"

requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: working, DTLS" \
            "$P_SRV debug_level=3 dtls=1 ecjpake_pw=bla" \
            "$P_CLI debug_level=3 dtls=1 ecjpake_pw=bla \
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            0 \
            -c "re-using cached ecjpake parameters" \
            -S "SSL - Verification of the message MAC failed"

requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: working, DTLS, no cookie" \
            "$P_SRV debug_level=3 dtls=1 ecjpake_pw=bla cookies=0" \
            "$P_CLI debug_level=3 dtls=1 ecjpake_pw=bla \
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            0 \
            -C "re-using cached ecjpake parameters" \
            -S "SSL - Verification of the message MAC failed"

server_needs_more_time 1
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: password mismatch, DTLS" \
            "$P_SRV debug_level=3 dtls=1 ecjpake_pw=bla" \
            "$P_CLI debug_level=3 dtls=1 ecjpake_pw=bad \
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            1 \
            -c "re-using cached ecjpake parameters" \
            -s "SSL - Verification of the message MAC failed"

# for tests with configs/config-thread.h
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
run_test    "ECJPAKE: working, DTLS, nolog" \
            "$P_SRV dtls=1 ecjpake_pw=bla" \
            "$P_CLI dtls=1 ecjpake_pw=bla \
             force_ciphersuite=TLS-ECJPAKE-WITH-AES-128-CCM-8" \
            0

# Test for ClientHello without extensions

requires_config_enabled MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
requires_gnutls
run_test    "ClientHello without extensions: PSK" \
            "$P_SRV force_version=tls12 debug_level=3 psk=73776f726466697368" \
            "$G_CLI --priority=NORMAL:+PSK:-RSA:%NO_EXTENSIONS:%DISABLE_SAFE_RENEGOTIATION --pskusername=Client_identity --pskkey=73776f726466697368 localhost" \
            0 \
            -s "Ciphersuite is .*-PSK-.*" \
            -S "Ciphersuite is .*-EC.*" \
            -s "dumping 'client hello extensions' (0 bytes)"

# Tests for mbedtls_ssl_get_bytes_avail()

# The server first reads buffer_size-1 bytes, then reads the remainder.
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "mbedtls_ssl_get_bytes_avail: no extra data" \
            "$P_SRV buffer_size=100" \
            "$P_CLI request_size=100" \
            0 \
            -s "Read from client: 100 bytes read$"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "mbedtls_ssl_get_bytes_avail: extra data (+1)" \
            "$P_SRV buffer_size=100" \
            "$P_CLI request_size=101" \
            0 \
            -s "Read from client: 101 bytes read (100 + 1)"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_max_content_len 200
run_test    "mbedtls_ssl_get_bytes_avail: extra data (*2)" \
            "$P_SRV buffer_size=100" \
            "$P_CLI request_size=200" \
            0 \
            -s "Read from client: 200 bytes read (100 + 100)"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "mbedtls_ssl_get_bytes_avail: extra data (max)" \
            "$P_SRV buffer_size=100 force_version=tls12" \
            "$P_CLI request_size=$MAX_CONTENT_LEN" \
            0 \
            -s "Read from client: $MAX_CONTENT_LEN bytes read (100 + $((MAX_CONTENT_LEN - 100)))"

# Tests for small client packets

run_test    "Small client packet TLS 1.2 BlockCipher" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -s "Read from client: 1 bytes read"

run_test    "Small client packet TLS 1.2 BlockCipher, without EtM" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA etm=0" \
            0 \
            -s "Read from client: 1 bytes read"

run_test    "Small client packet TLS 1.2 BlockCipher larger MAC" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA384" \
            0 \
            -s "Read from client: 1 bytes read"

run_test    "Small client packet TLS 1.2 AEAD" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=1 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM" \
            0 \
            -s "Read from client: 1 bytes read"

run_test    "Small client packet TLS 1.2 AEAD shorter tag" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=1 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM-8" \
            0 \
            -s "Read from client: 1 bytes read"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Small client packet TLS 1.3 AEAD" \
            "$P_SRV" \
            "$P_CLI request_size=1 \
             force_ciphersuite=TLS1-3-AES-128-CCM-SHA256" \
            0 \
            -s "Read from client: 1 bytes read"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Small client packet TLS 1.3 AEAD shorter tag" \
            "$P_SRV" \
            "$P_CLI request_size=1 \
             force_ciphersuite=TLS1-3-AES-128-CCM-8-SHA256" \
            0 \
            -s "Read from client: 1 bytes read"

# Tests for small client packets in DTLS

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
run_test    "Small client packet DTLS 1.2" \
            "$P_SRV dtls=1 force_version=dtls12" \
            "$P_CLI dtls=1 request_size=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -s "Read from client: 1 bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
run_test    "Small client packet DTLS 1.2, without EtM" \
            "$P_SRV dtls=1 force_version=dtls12 etm=0" \
            "$P_CLI dtls=1 request_size=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -s "Read from client: 1 bytes read"

# Tests for small server packets

run_test    "Small server packet TLS 1.2 BlockCipher" \
            "$P_SRV response_size=1 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -c "Read from server: 1 bytes read"

run_test    "Small server packet TLS 1.2 BlockCipher, without EtM" \
            "$P_SRV response_size=1 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA etm=0" \
            0 \
            -c "Read from server: 1 bytes read"

run_test    "Small server packet TLS 1.2 BlockCipher larger MAC" \
            "$P_SRV response_size=1 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA384" \
            0 \
            -c "Read from server: 1 bytes read"

run_test    "Small server packet TLS 1.2 AEAD" \
            "$P_SRV response_size=1 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM" \
            0 \
            -c "Read from server: 1 bytes read"

run_test    "Small server packet TLS 1.2 AEAD shorter tag" \
            "$P_SRV response_size=1 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM-8" \
            0 \
            -c "Read from server: 1 bytes read"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Small server packet TLS 1.3 AEAD" \
            "$P_SRV response_size=1" \
            "$P_CLI force_ciphersuite=TLS1-3-AES-128-CCM-SHA256" \
            0 \
            -c "Read from server: 1 bytes read"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Small server packet TLS 1.3 AEAD shorter tag" \
            "$P_SRV response_size=1" \
            "$P_CLI force_ciphersuite=TLS1-3-AES-128-CCM-8-SHA256" \
            0 \
            -c "Read from server: 1 bytes read"

# Tests for small server packets in DTLS

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
run_test    "Small server packet DTLS 1.2" \
            "$P_SRV dtls=1 response_size=1 force_version=dtls12" \
            "$P_CLI dtls=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -c "Read from server: 1 bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
run_test    "Small server packet DTLS 1.2, without EtM" \
            "$P_SRV dtls=1 response_size=1 force_version=dtls12 etm=0" \
            "$P_CLI dtls=1 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -c "Read from server: 1 bytes read"

# Test for large client packets

# How many fragments do we expect to write $1 bytes?
fragments_for_write() {
    echo "$(( ( $1 + $MAX_OUT_LEN - 1 ) / $MAX_OUT_LEN ))"
}

run_test    "Large client packet TLS 1.2 BlockCipher" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=16384 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -c "16384 bytes written in $(fragments_for_write 16384) fragments" \
            -s "Read from client: $MAX_CONTENT_LEN bytes read"

run_test    "Large client packet TLS 1.2 BlockCipher, without EtM" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=16384 etm=0 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -s "Read from client: $MAX_CONTENT_LEN bytes read"

run_test    "Large client packet TLS 1.2 BlockCipher larger MAC" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=16384 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA384" \
            0 \
            -c "16384 bytes written in $(fragments_for_write 16384) fragments" \
            -s "Read from client: $MAX_CONTENT_LEN bytes read"

run_test    "Large client packet TLS 1.2 AEAD" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=16384 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM" \
            0 \
            -c "16384 bytes written in $(fragments_for_write 16384) fragments" \
            -s "Read from client: $MAX_CONTENT_LEN bytes read"

run_test    "Large client packet TLS 1.2 AEAD shorter tag" \
            "$P_SRV force_version=tls12" \
            "$P_CLI request_size=16384 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM-8" \
            0 \
            -c "16384 bytes written in $(fragments_for_write 16384) fragments" \
            -s "Read from client: $MAX_CONTENT_LEN bytes read"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Large client packet TLS 1.3 AEAD" \
            "$P_SRV" \
            "$P_CLI request_size=16383 \
             force_ciphersuite=TLS1-3-AES-128-CCM-SHA256" \
            0 \
            -c "16383 bytes written in $(fragments_for_write 16383) fragments" \
            -s "Read from client: 16383 bytes read"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Large client packet TLS 1.3 AEAD shorter tag" \
            "$P_SRV" \
            "$P_CLI request_size=16383 \
             force_ciphersuite=TLS1-3-AES-128-CCM-8-SHA256" \
            0 \
            -c "16383 bytes written in $(fragments_for_write 16383) fragments" \
            -s "Read from client: 16383 bytes read"

# The tests below fail when the server's OUT_CONTENT_LEN is less than 16384.
run_test    "Large server packet TLS 1.2 BlockCipher" \
            "$P_SRV response_size=16384 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -c "Read from server: 16384 bytes read"

run_test    "Large server packet TLS 1.2 BlockCipher, without EtM" \
            "$P_SRV response_size=16384 force_version=tls12" \
            "$P_CLI etm=0 force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA" \
            0 \
            -s "16384 bytes written in 1 fragments" \
            -c "Read from server: 16384 bytes read"

run_test    "Large server packet TLS 1.2 BlockCipher larger MAC" \
            "$P_SRV response_size=16384 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA384" \
            0 \
            -c "Read from server: 16384 bytes read"

run_test    "Large server packet TLS 1.2 BlockCipher, without EtM, truncated MAC" \
            "$P_SRV response_size=16384 trunc_hmac=1 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA trunc_hmac=1 etm=0" \
            0 \
            -s "16384 bytes written in 1 fragments" \
            -c "Read from server: 16384 bytes read"

run_test    "Large server packet TLS 1.2 AEAD" \
            "$P_SRV response_size=16384 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM" \
            0 \
            -c "Read from server: 16384 bytes read"

run_test    "Large server packet TLS 1.2 AEAD shorter tag" \
            "$P_SRV response_size=16384 force_version=tls12" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-256-CCM-8" \
            0 \
            -c "Read from server: 16384 bytes read"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Large server packet TLS 1.3 AEAD" \
            "$P_SRV response_size=16383" \
            "$P_CLI force_ciphersuite=TLS1-3-AES-128-CCM-SHA256" \
            0 \
            -c "Read from server: 16383 bytes read"

requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "Large server packet TLS 1.3 AEAD shorter tag" \
            "$P_SRV response_size=16383" \
            "$P_CLI force_ciphersuite=TLS1-3-AES-128-CCM-8-SHA256" \
            0 \
            -c "Read from server: 16383 bytes read"

# Tests for restartable ECC

# Force the use of a curve that supports restartable ECC (secp256r1).

requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, default" \
            "$P_SRV groups=secp256r1 auth_mode=required" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1" \
            0 \
            -C "x509_verify_cert.*\(4b00\|-248\)" \
            -C "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -C "mbedtls_pk_sign.*\(4b00\|-248\)"

requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, max_ops=0" \
            "$P_SRV groups=secp256r1 auth_mode=required" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=0" \
            0 \
            -C "x509_verify_cert.*\(4b00\|-248\)" \
            -C "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -C "mbedtls_pk_sign.*\(4b00\|-248\)"

requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, max_ops=65535" \
            "$P_SRV groups=secp256r1 auth_mode=required" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=65535" \
            0 \
            -C "x509_verify_cert.*\(4b00\|-248\)" \
            -C "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -C "mbedtls_pk_sign.*\(4b00\|-248\)"

# The following test cases for restartable ECDH come in two variants:
# * The "(USE_PSA)" variant expects the current behavior, which is the behavior
#   from Mbed TLS 3.x when MBEDTLS_USE_PSA_CRYPTO is enabled. This tests
#   the partial implementation where ECDH in TLS is not actually restartable.
# * The "(no USE_PSA)" variant expects the desired behavior. These test
#   cases cannot currently pass because the implementation of restartable ECC
#   in TLS is partial: ECDH is not actually restartable. This is the behavior
#   from Mbed TLS 3.x when MBEDTLS_USE_PSA_CRYPTO is disabled.
#
# As part of resolving https://github.com/Mbed-TLS/mbedtls/issues/7294,
# we will remove the "(USE_PSA)" test cases and run the "(no USE_PSA)" test
# cases.

# With USE_PSA disabled we expect full restartable behaviour.
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
skip_next_test
run_test    "EC restart: TLS, max_ops=1000 (no USE_PSA)" \
            "$P_SRV groups=secp256r1 auth_mode=required" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=1000" \
            0 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -c "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -c "mbedtls_pk_sign.*\(4b00\|-248\)"

# With USE_PSA enabled we expect only partial restartable behaviour:
# everything except ECDH (where TLS calls PSA directly).
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, max_ops=1000 (USE_PSA)" \
            "$P_SRV groups=secp256r1 auth_mode=required" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=1000" \
            0 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -c "mbedtls_pk_sign.*\(4b00\|-248\)"

# This works the same with & without USE_PSA as we never get to ECDH:
# we abort as soon as we determined the cert is bad.
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, max_ops=1000, badsign" \
            "$P_SRV groups=secp256r1 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=1000" \
            1 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -C "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -C "mbedtls_pk_sign.*\(4b00\|-248\)" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -c "! mbedtls_ssl_handshake returned" \
            -c "X509 - Certificate verification failed"

# With USE_PSA disabled we expect full restartable behaviour.
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
skip_next_test
run_test    "EC restart: TLS, max_ops=1000, auth_mode=optional badsign (no USE_PSA)" \
            "$P_SRV groups=secp256r1 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=1000 auth_mode=optional" \
            0 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -c "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -c "mbedtls_pk_sign.*\(4b00\|-248\)" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed"

# With USE_PSA enabled we expect only partial restartable behaviour:
# everything except ECDH (where TLS calls PSA directly).
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, max_ops=1000, auth_mode=optional badsign (USE_PSA)" \
            "$P_SRV groups=secp256r1 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=1000 auth_mode=optional" \
            0 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -c "mbedtls_pk_sign.*\(4b00\|-248\)" \
            -c "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed"

# With USE_PSA disabled we expect full restartable behaviour.
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
skip_next_test
run_test    "EC restart: TLS, max_ops=1000, auth_mode=none badsign (no USE_PSA)" \
            "$P_SRV groups=secp256r1 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=1000 auth_mode=none" \
            0 \
            -C "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -c "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -c "mbedtls_pk_sign.*\(4b00\|-248\)" \
            -C "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed"

# With USE_PSA enabled we expect only partial restartable behaviour:
# everything except ECDH (where TLS calls PSA directly).
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, max_ops=1000, auth_mode=none badsign (USE_PSA)" \
            "$P_SRV groups=secp256r1 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server5-badsign.crt \
             key_file=$DATA_FILES_PATH/server5.key" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=1000 auth_mode=none" \
            0 \
            -C "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -c "mbedtls_pk_sign.*\(4b00\|-248\)" \
            -C "! The certificate is not correctly signed by the trusted CA" \
            -C "! mbedtls_ssl_handshake returned" \
            -C "X509 - Certificate verification failed"

# With USE_PSA disabled we expect full restartable behaviour.
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
skip_next_test
run_test    "EC restart: DTLS, max_ops=1000 (no USE_PSA)" \
            "$P_SRV groups=secp256r1 auth_mode=required dtls=1" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             dtls=1 debug_level=1 ec_max_ops=1000" \
            0 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -c "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -c "mbedtls_pk_sign.*\(4b00\|-248\)"

# With USE_PSA enabled we expect only partial restartable behaviour:
# everything except ECDH (where TLS calls PSA directly).
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: DTLS, max_ops=1000 (USE_PSA)" \
            "$P_SRV groups=secp256r1 auth_mode=required dtls=1" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             dtls=1 debug_level=1 ec_max_ops=1000" \
            0 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -c "mbedtls_pk_sign.*\(4b00\|-248\)"

# With USE_PSA disabled we expect full restartable behaviour.
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
skip_next_test
run_test    "EC restart: TLS, max_ops=1000 no client auth (no USE_PSA)" \
            "$P_SRV groups=secp256r1" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             debug_level=1 ec_max_ops=1000" \
            0 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -c "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -C "mbedtls_pk_sign.*\(4b00\|-248\)"


# With USE_PSA enabled we expect only partial restartable behaviour:
# everything except ECDH (where TLS calls PSA directly).
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, max_ops=1000 no client auth (USE_PSA)" \
            "$P_SRV groups=secp256r1" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             debug_level=1 ec_max_ops=1000" \
            0 \
            -c "x509_verify_cert.*\(4b00\|-248\)" \
            -c "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -C "mbedtls_pk_sign.*\(4b00\|-248\)"

# Restartable is only for ECDHE-ECDSA, with another ciphersuite we expect no
# restartable behaviour at all (not even client auth).
# This is the same as "EC restart: TLS, max_ops=1000" except with ECDHE-RSA,
# and all 4 assertions negated.
requires_config_enabled MBEDTLS_ECP_RESTARTABLE
requires_config_enabled PSA_WANT_ECC_SECP_R1_256
run_test    "EC restart: TLS, max_ops=1000, ECDHE-RSA" \
            "$P_SRV groups=secp256r1 auth_mode=required" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt  \
             debug_level=1 ec_max_ops=1000" \
            0 \
            -C "x509_verify_cert.*\(4b00\|-248\)" \
            -C "mbedtls_pk_verify.*\(4b00\|-248\)" \
            -C "mbedtls_ecdh_make_public.*\(4b00\|-248\)" \
            -C "mbedtls_pk_sign.*\(4b00\|-248\)"

# Tests of asynchronous private key support in SSL

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: sign, delay=0" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=0 async_private_delay2=0" \
            "$P_CLI" \
            0 \
            -s "Async sign callback: using key slot " \
            -s "Async resume (slot [0-9]): sign done, status=0"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: sign, delay=1" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=1 async_private_delay2=1" \
            "$P_CLI" \
            0 \
            -s "Async sign callback: using key slot " \
            -s "Async resume (slot [0-9]): call 0 more times." \
            -s "Async resume (slot [0-9]): sign done, status=0"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: sign, delay=2" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=2 async_private_delay2=2" \
            "$P_CLI" \
            0 \
            -s "Async sign callback: using key slot " \
            -U "Async sign callback: using key slot " \
            -s "Async resume (slot [0-9]): call 1 more times." \
            -s "Async resume (slot [0-9]): call 0 more times." \
            -s "Async resume (slot [0-9]): sign done, status=0"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
requires_config_disabled MBEDTLS_X509_REMOVE_INFO
run_test    "SSL async private: sign, SNI" \
            "$P_SRV force_version=tls12 debug_level=3 \
             async_operations=s async_private_delay1=0 async_private_delay2=0 \
             crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$P_CLI server_name=polarssl.example" \
            0 \
            -s "Async sign callback: using key slot " \
            -s "Async resume (slot [0-9]): sign done, status=0" \
            -s "parse ServerName extension" \
            -c "issuer name *: C=NL, O=PolarSSL, CN=PolarSSL Test CA" \
            -c "subject name *: C=NL, O=PolarSSL, CN=polarssl.example"

# key1: ECDSA, key2: RSA; use key1 from slot 0
requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: slot 0 used with key1" \
            "$P_SRV \
             async_operations=s async_private_delay1=1 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt \
             key_file2=$DATA_FILES_PATH/server2.key crt_file2=$DATA_FILES_PATH/server2.crt" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -s "Async sign callback: using key slot 0," \
            -s "Async resume (slot 0): call 0 more times." \
            -s "Async resume (slot 0): sign done, status=0"

# key1: ECDSA, key2: RSA; use key2 from slot 0
requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: slot 0 used with key2" \
            "$P_SRV \
             async_operations=s async_private_delay2=1 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt \
             key_file2=$DATA_FILES_PATH/server2.key crt_file2=$DATA_FILES_PATH/server2.crt" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -s "Async sign callback: using key slot 0," \
            -s "Async resume (slot 0): call 0 more times." \
            -s "Async resume (slot 0): sign done, status=0"

# key1: ECDSA, key2: RSA; use key2 from slot 1
requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: slot 1 used with key2" \
            "$P_SRV \
             async_operations=s async_private_delay1=1 async_private_delay2=1 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt \
             key_file2=$DATA_FILES_PATH/server2.key crt_file2=$DATA_FILES_PATH/server2.crt" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -s "Async sign callback: using key slot 1," \
            -s "Async resume (slot 1): call 0 more times." \
            -s "Async resume (slot 1): sign done, status=0"

# key1: ECDSA, key2: RSA; use key2 directly
requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: fall back to transparent key" \
            "$P_SRV \
             async_operations=s async_private_delay1=1 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt \
             key_file2=$DATA_FILES_PATH/server2.key crt_file2=$DATA_FILES_PATH/server2.crt " \
            "$P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -s "Async sign callback: no key matches this certificate."

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: sign, error in start" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=1 async_private_delay2=1 \
             async_private_error=1" \
            "$P_CLI" \
            1 \
            -s "Async sign callback: injected error" \
            -S "Async resume" \
            -S "Async cancel" \
            -s "! mbedtls_ssl_handshake returned"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: sign, cancel after start" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=1 async_private_delay2=1 \
             async_private_error=2" \
            "$P_CLI" \
            1 \
            -s "Async sign callback: using key slot " \
            -S "Async resume" \
            -s "Async cancel"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: sign, error in resume" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=1 async_private_delay2=1 \
             async_private_error=3" \
            "$P_CLI" \
            1 \
            -s "Async sign callback: using key slot " \
            -s "Async resume callback: sign done but injected error" \
            -S "Async cancel" \
            -s "! mbedtls_ssl_handshake returned"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: cancel after start then operate correctly" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=1 async_private_delay2=1 \
             async_private_error=-2" \
            "$P_CLI; [ \$? -eq 1 ] && $P_CLI" \
            0 \
            -s "Async cancel" \
            -s "! mbedtls_ssl_handshake returned" \
            -s "Async resume" \
            -s "Successful connection"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
run_test    "SSL async private: error in resume then operate correctly" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=1 async_private_delay2=1 \
             async_private_error=-3" \
            "$P_CLI; [ \$? -eq 1 ] && $P_CLI" \
            0 \
            -s "! mbedtls_ssl_handshake returned" \
            -s "Async resume" \
            -s "Successful connection"

# key1: ECDSA, key2: RSA; use key1 through async, then key2 directly
requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
# Note: the function "detect_required_features()" is not able to detect more than
#       one "force_ciphersuite" per client/server and it only picks the 2nd one.
#       Therefore the 1st one is added explicitly here
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
run_test    "SSL async private: cancel after start then fall back to transparent key" \
            "$P_SRV \
             async_operations=s async_private_delay1=1 async_private_error=-2 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt \
             key_file2=$DATA_FILES_PATH/server2.key crt_file2=$DATA_FILES_PATH/server2.crt" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256;
             [ \$? -eq 1 ] &&
             $P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -s "Async sign callback: using key slot 0" \
            -S "Async resume" \
            -s "Async cancel" \
            -s "! mbedtls_ssl_handshake returned" \
            -s "Async sign callback: no key matches this certificate." \
            -s "Successful connection"

# key1: ECDSA, key2: RSA; use key1 through async, then key2 directly
requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
# Note: the function "detect_required_features()" is not able to detect more than
#       one "force_ciphersuite" per client/server and it only picks the 2nd one.
#       Therefore the 1st one is added explicitly here
requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
run_test    "SSL async private: sign, error in resume then fall back to transparent key" \
            "$P_SRV \
             async_operations=s async_private_delay1=1 async_private_error=-3 \
             key_file=$DATA_FILES_PATH/server5.key crt_file=$DATA_FILES_PATH/server5.crt \
             key_file2=$DATA_FILES_PATH/server2.key crt_file2=$DATA_FILES_PATH/server2.crt" \
            "$P_CLI force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256;
             [ \$? -eq 1 ] &&
             $P_CLI force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -s "Async resume" \
            -s "! mbedtls_ssl_handshake returned" \
            -s "Async sign callback: no key matches this certificate." \
            -s "Successful connection"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "SSL async private: renegotiation: client-initiated, sign" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=1 async_private_delay2=1 \
             exchanges=2 renegotiation=1" \
            "$P_CLI exchanges=2 renegotiation=1 renegotiate=1" \
            0 \
            -s "Async sign callback: using key slot " \
            -s "Async resume (slot [0-9]): sign done, status=0"

requires_config_enabled MBEDTLS_SSL_ASYNC_PRIVATE
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "SSL async private: renegotiation: server-initiated, sign" \
            "$P_SRV force_version=tls12 \
             async_operations=s async_private_delay1=1 async_private_delay2=1 \
             exchanges=2 renegotiation=1 renegotiate=1" \
            "$P_CLI exchanges=2 renegotiation=1" \
            0 \
            -s "Async sign callback: using key slot " \
            -s "Async resume (slot [0-9]): sign done, status=0"

# Tests for ECC extensions (rfc 4492)

requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
run_test    "Force a non ECC ciphersuite in the client side" \
            "$P_SRV debug_level=3 psk=73776f726466697368" \
            "$P_CLI debug_level=3 psk=73776f726466697368 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA256" \
            0 \
            -C "client hello, adding supported_groups extension" \
            -C "client hello, adding supported_point_formats extension" \
            -S "found supported elliptic curves extension" \
            -S "found supported point formats extension"

requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
run_test    "Force a non ECC ciphersuite in the server side" \
            "$P_SRV debug_level=3 psk=73776f726466697368 force_ciphersuite=TLS-PSK-WITH-AES-128-CBC-SHA256" \
            "$P_CLI debug_level=3 psk=73776f726466697368" \
            0 \
            -C "found supported_point_formats extension" \
            -S "server hello, supported_point_formats extension"

requires_hash_alg SHA_256
run_test    "Force an ECC ciphersuite in the client side" \
            "$P_SRV debug_level=3" \
            "$P_CLI debug_level=3 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256" \
            0 \
            -c "client hello, adding supported_groups extension" \
            -c "client hello, adding supported_point_formats extension" \
            -s "found supported elliptic curves extension" \
            -s "found supported point formats extension"

requires_hash_alg SHA_256
run_test    "Force an ECC ciphersuite in the server side" \
            "$P_SRV debug_level=3 force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256" \
            "$P_CLI debug_level=3" \
            0 \
            -c "found supported_point_formats extension" \
            -s "server hello, supported_point_formats extension"

# Tests for DTLS HelloVerifyRequest

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS cookie: enabled" \
            "$P_SRV dtls=1 debug_level=2" \
            "$P_CLI dtls=1 debug_level=2" \
            0 \
            -s "cookie verification failed" \
            -s "cookie verification passed" \
            -S "cookie verification skipped" \
            -c "received hello verify request" \
            -s "hello verification requested" \
            -S "SSL - The requested feature is not available"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS cookie: disabled" \
            "$P_SRV dtls=1 debug_level=2 cookies=0" \
            "$P_CLI dtls=1 debug_level=2" \
            0 \
            -S "cookie verification failed" \
            -S "cookie verification passed" \
            -s "cookie verification skipped" \
            -C "received hello verify request" \
            -S "hello verification requested" \
            -S "SSL - The requested feature is not available"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS cookie: default (failing)" \
            "$P_SRV dtls=1 debug_level=2 cookies=-1" \
            "$P_CLI dtls=1 debug_level=2 hs_timeout=100-400" \
            1 \
            -s "cookie verification failed" \
            -S "cookie verification passed" \
            -S "cookie verification skipped" \
            -C "received hello verify request" \
            -S "hello verification requested" \
            -s "SSL - The requested feature is not available"

requires_ipv6
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS cookie: enabled, IPv6" \
            "$P_SRV dtls=1 debug_level=2 server_addr=::1" \
            "$P_CLI dtls=1 debug_level=2 server_addr=::1" \
            0 \
            -s "cookie verification failed" \
            -s "cookie verification passed" \
            -S "cookie verification skipped" \
            -c "received hello verify request" \
            -s "hello verification requested" \
            -S "SSL - The requested feature is not available"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS cookie: enabled, nbio" \
            "$P_SRV dtls=1 nbio=2 debug_level=2" \
            "$P_CLI dtls=1 nbio=2 debug_level=2" \
            0 \
            -s "cookie verification failed" \
            -s "cookie verification passed" \
            -S "cookie verification skipped" \
            -c "received hello verify request" \
            -s "hello verification requested" \
            -S "SSL - The requested feature is not available"

# Tests for client reconnecting from the same port with DTLS

not_with_valgrind # spurious resend
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client reconnect from same port: reference" \
            "$P_SRV dtls=1 exchanges=2 read_timeout=20000 hs_timeout=10000-20000" \
            "$P_CLI dtls=1 exchanges=2 debug_level=2 hs_timeout=10000-20000" \
            0 \
            -C "resend" \
            -S "The operation timed out" \
            -S "Client initiated reconnection from same port"

not_with_valgrind # spurious resend
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client reconnect from same port: reconnect" \
            "$P_SRV dtls=1 exchanges=2 read_timeout=20000 hs_timeout=10000-20000" \
            "$P_CLI dtls=1 exchanges=2 debug_level=2 hs_timeout=10000-20000 reconnect_hard=1" \
            0 \
            -C "resend" \
            -S "The operation timed out" \
            -s "Client initiated reconnection from same port"

not_with_valgrind # server/client too slow to respond in time (next test has higher timeouts)
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client reconnect from same port: reconnect, nbio, no valgrind" \
            "$P_SRV dtls=1 exchanges=2 read_timeout=1000 nbio=2" \
            "$P_CLI dtls=1 exchanges=2 debug_level=2 hs_timeout=500-1000 reconnect_hard=1" \
            0 \
            -S "The operation timed out" \
            -s "Client initiated reconnection from same port"

only_with_valgrind # Only with valgrind, do previous test but with higher read_timeout and hs_timeout
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client reconnect from same port: reconnect, nbio, valgrind" \
            "$P_SRV dtls=1 exchanges=2 read_timeout=2000 nbio=2 hs_timeout=1500-6000" \
            "$P_CLI dtls=1 exchanges=2 debug_level=2 hs_timeout=1500-3000 reconnect_hard=1" \
            0 \
            -S "The operation timed out" \
            -s "Client initiated reconnection from same port"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client reconnect from same port: no cookies" \
            "$P_SRV dtls=1 exchanges=2 read_timeout=1000 cookies=0" \
            "$P_CLI dtls=1 exchanges=2 debug_level=2 hs_timeout=500-8000 reconnect_hard=1" \
            0 \
            -s "The operation timed out" \
            -S "Client initiated reconnection from same port"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client reconnect from same port: attacker-injected" \
            -p "$P_PXY inject_clihlo=1" \
            "$P_SRV dtls=1 exchanges=2 debug_level=1" \
            "$P_CLI dtls=1 exchanges=2" \
            0 \
            -s "possible client reconnect from the same port" \
            -S "Client initiated reconnection from same port"

# Tests for various cases of client authentication with DTLS
# (focused on handshake flows and message parsing)

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client auth: required" \
            "$P_SRV dtls=1 auth_mode=required" \
            "$P_CLI dtls=1" \
            0 \
            -s "Verifying peer X.509 certificate... ok"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client auth: optional, client has no cert" \
            "$P_SRV dtls=1 auth_mode=optional" \
            "$P_CLI dtls=1 crt_file=none key_file=none" \
            0 \
            -s "! Certificate was missing"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS client auth: none, client has no cert" \
            "$P_SRV dtls=1 auth_mode=none" \
            "$P_CLI dtls=1 crt_file=none key_file=none debug_level=2" \
            0 \
            -c "skip write certificate$" \
            -s "! Certificate verification was skipped"

run_test    "DTLS wrong PSK: badmac alert" \
            "$P_SRV dtls=1 psk=73776f726466697368 force_ciphersuite=TLS-PSK-WITH-AES-128-GCM-SHA256" \
            "$P_CLI dtls=1 psk=73776f726466697374" \
            1 \
            -s "SSL - Verification of the message MAC failed" \
            -c "SSL - A fatal alert message was received from our peer"

# Tests for receiving fragmented handshake messages with DTLS

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: no fragmentation (gnutls server)" \
            "$G_SRV -u --mtu 2048 -a" \
            "$P_CLI dtls=1 debug_level=2" \
            0 \
            -C "found fragmented DTLS handshake message" \
            -C "error"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: some fragmentation (gnutls server)" \
            "$G_SRV -u --mtu 512" \
            "$P_CLI dtls=1 debug_level=2" \
            0 \
            -c "found fragmented DTLS handshake message" \
            -c "Certificate handshake message has been buffered and reassembled" \
            -C "error"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: more fragmentation (gnutls server)" \
            "$G_SRV -u --mtu 128" \
            "$P_CLI dtls=1 debug_level=2" \
            0 \
            -c "found fragmented DTLS handshake message" \
            -c "Certificate handshake message has been buffered and reassembled" \
            -c "ServerKeyExchange handshake message has been buffered and reassembled" \
            -C "error"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: more fragmentation, nbio (gnutls server)" \
            "$G_SRV -u --mtu 109" \
            "$P_CLI dtls=1 nbio=2 debug_level=2" \
            0 \
            -c "found fragmented DTLS handshake message" \
            -c "Certificate handshake message has been buffered and reassembled" \
            -c "ServerKeyExchange handshake message has been buffered and reassembled" \
            -C "error"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: fragmentation, renego (gnutls server)" \
            "$G_SRV -u --mtu 241" \
            "$P_CLI debug_level=3 dtls=1 renegotiation=1 renegotiate=1" \
            0 \
            -c "found fragmented DTLS handshake message" \
            -c "Certificate handshake message has been buffered and reassembled" \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -C "mbedtls_ssl_handshake returned" \
            -C "error" \
            -s "Extra-header:"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: fragmentation, nbio, renego (gnutls server)" \
            "$G_SRV -u --mtu 256" \
            "$P_CLI debug_level=3 nbio=2 dtls=1 renegotiation=1 renegotiate=1" \
            0 \
            -c "found fragmented DTLS handshake message" \
            -c "Certificate handshake message has been buffered and reassembled" \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate" \
            -C "mbedtls_ssl_handshake returned" \
            -C "error" \
            -s "Extra-header:"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: no fragmentation (gnutls client)" \
            "$P_SRV debug_level=2 dtls=1" \
            "$G_NEXT_CLI -u --mtu 2048 --insecure 127.0.0.1" \
            0 \
            -S "found fragmented DTLS handshake message" \
            -s "HTTP/1.0 200 OK"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: some fragmentation (gnutls client)" \
            "$P_SRV debug_level=2 dtls=1 auth_mode=required" \
            "$G_NEXT_CLI -u --mtu 211 --insecure 127.0.0.1 --x509certfile $DATA_FILES_PATH/server5.crt --x509keyfile $DATA_FILES_PATH/server5.key" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -s "Certificate handshake message has been buffered and reassembled" \
            -s "HTTP/1.0 200 OK"

# Set the MTU to 128 bytes. The minimum size of a DTLS 1.2 record
# containing a ClientHello handshake message is 69 bytes, without any cookie,
# ciphersuite, or extension. With an MTU of 128 bytes, the ClientHello handshake
# message is therefore very likely to be fragmented, regardless of the
# GnuTLS client version. For example, the ClientHello sent by the GnuTLS 3.7.2
# client is 206 bytes in this test.
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: more fragmentation (gnutls client)" \
            "$P_SRV debug_level=2 dtls=1" \
            "$G_NEXT_CLI -u --mtu 103 --insecure 127.0.0.1" \
            0 \
            -s "ClientHello handshake message has been buffered and reassembled" \
            -s "HTTP/1.0 200 OK"

requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: more fragmentation, nbio (gnutls client)" \
            "$P_SRV debug_level=2 dtls=1 nbio=2" \
            "$G_NEXT_CLI -u --mtu 103 --insecure 127.0.0.1" \
            0 \
            -s "ClientHello handshake message has been buffered and reassembled" \
            -s "HTTP/1.0 200 OK"

# No fragmentation and renegotiation tests with GnuTLS client as the feature
# does not work properly.

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: no fragmentation (openssl server)" \
            "$O_SRV -dtls -mtu 2048" \
            "$P_CLI dtls=1 debug_level=2" \
            0 \
            -C "found fragmented DTLS handshake message" \
            -C "error"

# Minimum possible MTU for OpenSSL server: 256 bytes.
# We expect the server Certificate handshake to be fragmented and verify that
# this is the case. Depending on the configuration, other handshake messages may
# also be fragmented.
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: fragmentation (openssl server)" \
            "$O_SRV -dtls -mtu 256" \
            "$P_CLI dtls=1 debug_level=2" \
            0 \
            -c "found fragmented DTLS handshake message" \
            -c "Certificate handshake message has been buffered and reassembled" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: fragmentation, nbio (openssl server)" \
            "$O_SRV -dtls -mtu 273" \
            "$P_CLI dtls=1 nbio=2 debug_level=2" \
            0 \
            -c "found fragmented DTLS handshake message" \
            -c "Certificate handshake message has been buffered and reassembled" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: no fragmentation (openssl client)" \
            "$P_SRV debug_level=2 dtls=1 auth_mode=required" \
            "$O_NEXT_CLI -dtls -mtu 2048 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            0 \
            -S "found fragmented DTLS handshake message" \
            -s "HTTP/1.0 200 OK"

# Minimum possible MTU for OpenSSL server: 256 bytes.
# We expect the client Certificate handshake message to be fragmented and
# verify that this is the case. With OpenSSL 3.0.13, the ClientHello handshake
# message is 224 bytes and also fragmented. However, it may not hold across
# OpenSSL version updates. Therefore, we do not verify that the ClientHello is
# reassembled by the server.
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: some fragmentation (openssl client)" \
            "$P_SRV debug_level=2 dtls=1 auth_mode=required" \
            "$O_NEXT_CLI -dtls -mtu 256 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -s "Certificate handshake message has been buffered and reassembled" \
            -s "HTTP/1.0 200 OK"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reassembly: fragmentation, nbio (openssl client)" \
            "$P_SRV debug_level=2 dtls=1 auth_mode=required nbio=2" \
            "$O_NEXT_CLI -dtls -mtu 269 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -s "Certificate handshake message has been buffered and reassembled" \
            -s "HTTP/1.0 200 OK"

# Tests for sending fragmented handshake messages with DTLS
#
# Use client auth when we need the client to send large messages,
# and use large cert chains on both sides too (the long chains we have all use
# both RSA and ECDSA, but ideally we should have long chains with either).
# Sizes reached (UDP payload):
# - 2037B for server certificate
# - 1542B for client certificate
# - 1013B for newsessionticket
# - all others below 512B
# All those tests assume MAX_CONTENT_LEN is at least 2048

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: none (for reference)" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             max_frag_len=4096" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             max_frag_len=4096" \
            0 \
            -S "found fragmented DTLS handshake message" \
            -C "found fragmented DTLS handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: server only (max_frag_len)" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             max_frag_len=1024" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             max_frag_len=2048" \
            0 \
            -S "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# With the MFL extension, the server has no way of forcing
# the client to not exceed a certain MTU; hence, the following
# test can't be replicated with an MTU proxy such as the one
# `client-initiated, server only (max_frag_len)` below.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: server only (more) (max_frag_len)" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             max_frag_len=512" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             max_frag_len=4096" \
            0 \
            -S "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: client-initiated, server only (max_frag_len)" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=none \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             max_frag_len=2048" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             max_frag_len=1024" \
             0 \
            -S "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# While not required by the standard defining the MFL extension
# (according to which it only applies to records, not to datagrams),
# Mbed TLS will never send datagrams larger than MFL + { Max record expansion },
# as otherwise there wouldn't be any means to communicate MTU restrictions
# to the peer.
# The next test checks that no datagrams significantly larger than the
# negotiated MFL are sent.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: client-initiated, server only (max_frag_len), proxy MTU" \
            -p "$P_PXY mtu=1110" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=none \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             max_frag_len=2048" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             max_frag_len=1024" \
            0 \
            -S "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: client-initiated, both (max_frag_len)" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             max_frag_len=2048" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             max_frag_len=1024" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# While not required by the standard defining the MFL extension
# (according to which it only applies to records, not to datagrams),
# Mbed TLS will never send datagrams larger than MFL + { Max record expansion },
# as otherwise there wouldn't be any means to communicate MTU restrictions
# to the peer.
# The next test checks that no datagrams significantly larger than the
# negotiated MFL are sent.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: client-initiated, both (max_frag_len), proxy MTU" \
            -p "$P_PXY mtu=1110" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             max_frag_len=2048" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             max_frag_len=1024" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: none (for reference) (MTU)" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             mtu=4096" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             mtu=4096" \
            0 \
            -S "found fragmented DTLS handshake message" \
            -C "found fragmented DTLS handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 4096
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: client (MTU)" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=3500-60000 \
             mtu=4096" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=3500-60000 \
             mtu=1024" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -C "found fragmented DTLS handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 2048
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: server (MTU)" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             mtu=512" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             mtu=2048" \
            0 \
            -S "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 1038
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: both (MTU=1038)" \
            -p "$P_PXY mtu=1038" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             mtu=1038" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=2500-60000 \
             mtu=1038" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# Forcing ciphersuite for this test to fit the MTU of 512 with full config.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_max_content_len 509
run_test    "DTLS fragmenting: both (MTU=509)" \
            -p "$P_PXY mtu=509" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=2500-60000 \
             mtu=509" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             hs_timeout=2500-60000 \
             mtu=509" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# Depending on the ciphersuite selected to encrypt the application data, the
# maximum application data payload per record may be small with an MTU of 128.
# For example, with TLS-ECDHE-ECDSA-WITH-AES-256-CBC-SHA384, this maximum is
# 35 bytes. We therefore reduce the size of the client request and the server
# response in this test and the two following tests.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_max_content_len 128
run_test    "DTLS fragmenting: both (MTU=128)" \
            -p "$P_PXY mtu=128" \
            "$P_SRV dtls=1 debug_level=5 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             response_size=8 \
             hs_timeout=2500-60000 \
             mtu=128" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             request_size=8 \
             hs_timeout=2500-60000 \
             mtu=128" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -s "fragmenting Certificate handshake message" \
            -s "fragmenting ServerKeyExchange handshake message" \
            -c "found fragmented DTLS handshake message" \
            -c "fragmenting ClientHello handshake message" \
            -c "fragmenting Certificate handshake message" \
            -c "fragmenting CertificateVerify handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_max_content_len 2048
run_test    "DTLS fragmenting: both (MTU=107)" \
            -p "$P_PXY mtu=107" \
            "$P_SRV dtls=1 debug_level=5 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             response_size=8 \
             hs_timeout=2500-60000 \
             mtu=107" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             request_size=8 \
             hs_timeout=2500-60000 \
             mtu=107" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -s "fragmenting Certificate handshake message" \
            -s "fragmenting ServerKeyExchange handshake message" \
            -c "found fragmented DTLS handshake message" \
            -c "fragmenting ClientHello handshake message" \
            -c "fragmenting Certificate handshake message" \
            -c "fragmenting CertificateVerify handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_max_content_len 2048
run_test    "DTLS fragmenting: both (MTU=133)" \
            -p "$P_PXY mtu=133" \
            "$P_SRV dtls=1 debug_level=5 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             response_size=8 \
             hs_timeout=2500-60000 \
             mtu=133" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             request_size=8 \
             hs_timeout=2500-60000 \
             mtu=133" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -s "fragmenting Certificate handshake message" \
            -s "fragmenting ServerKeyExchange handshake message" \
            -c "found fragmented DTLS handshake message" \
            -c "fragmenting ClientHello handshake message" \
            -c "fragmenting Certificate handshake message" \
            -c "fragmenting CertificateVerify handshake message" \
            -C "error"

# Test for automatic MTU reduction on repeated resend.
# Forcing ciphersuite for this test to fit the MTU of 508 with full config.
# The ratio of max/min timeout should ideally equal 4 to accept two
# retransmissions, but in some cases (like both the server and client using
# fragmentation and auto-reduction) an extra retransmission might occur,
# hence the ratio of 8.
not_with_valgrind
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 2048
run_test    "DTLS fragmenting: proxy MTU: auto-reduction (not valgrind)" \
            -p "$P_PXY mtu=508" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=400-3200" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             hs_timeout=400-3200" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# Forcing ciphersuite for this test to fit the MTU of 508 with full config.
only_with_valgrind
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 2048
run_test    "DTLS fragmenting: proxy MTU: auto-reduction (with valgrind)" \
            -p "$P_PXY mtu=508" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=250-10000" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             hs_timeout=250-10000" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# the proxy shouldn't drop or mess up anything, so we shouldn't need to resend
# OTOH the client might resend if the server is to slow to reset after sending
# a HelloVerifyRequest, so only check for no retransmission server-side
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 1024
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: proxy MTU, simple handshake (MTU=1024)" \
            -p "$P_PXY mtu=1024" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=10000-60000 \
             mtu=1024" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=10000-60000 \
             mtu=1024" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# Forcing ciphersuite for this test to fit the MTU of 512 with full config.
# the proxy shouldn't drop or mess up anything, so we shouldn't need to resend
# OTOH the client might resend if the server is to slow to reset after sending
# a HelloVerifyRequest, so only check for no retransmission server-side
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 512
run_test    "DTLS fragmenting: proxy MTU, simple handshake (MTU=512)" \
            -p "$P_PXY mtu=512" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=10000-60000 \
             mtu=512" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             hs_timeout=10000-60000 \
             mtu=512" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 1024
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS fragmenting: proxy MTU, simple handshake, nbio (MTU=1024)" \
            -p "$P_PXY mtu=1024" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=10000-60000 \
             mtu=1024 nbio=2" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=10000-60000 \
             mtu=1024 nbio=2" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# Forcing ciphersuite for this test to fit the MTU of 512 with full config.
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 512
run_test    "DTLS fragmenting: proxy MTU, simple handshake, nbio (MTU=512)" \
            -p "$P_PXY mtu=512" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=10000-60000 \
             mtu=512 nbio=2" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             hs_timeout=10000-60000 \
             mtu=512 nbio=2" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# Forcing ciphersuite for this test to fit the MTU of 1450 with full config.
# This ensures things still work after session_reset().
# It also exercises the "resumed handshake" flow.
# Since we don't support reading fragmented ClientHello yet,
# up the MTU to 1450 (larger than ClientHello with session ticket,
# but still smaller than client's Certificate to ensure fragmentation).
# An autoreduction on the client-side might happen if the server is
# slow to reset, therefore omitting '-C "autoreduction"' below.
# reco_delay avoids races where the client reconnects before the server has
# resumed listening, which would result in a spurious autoreduction.
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 1450
run_test    "DTLS fragmenting: proxy MTU, resumed handshake" \
            -p "$P_PXY mtu=1450" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=10000-60000 \
             mtu=1450" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=10000-60000 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             mtu=1450 reconnect=1 skip_close_notify=1 reco_delay=1000" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# An autoreduction on the client-side might happen if the server is
# slow to reset, therefore omitting '-C "autoreduction"' below.
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_max_content_len 512
run_test    "DTLS fragmenting: proxy MTU, ChachaPoly renego" \
            -p "$P_PXY mtu=512" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             exchanges=2 renegotiation=1 \
             hs_timeout=10000-60000 \
             mtu=512" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             exchanges=2 renegotiation=1 renegotiate=1 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256 \
             hs_timeout=10000-60000 \
             mtu=512" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# An autoreduction on the client-side might happen if the server is
# slow to reset, therefore omitting '-C "autoreduction"' below.
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_max_content_len 512
run_test    "DTLS fragmenting: proxy MTU, AES-GCM renego" \
            -p "$P_PXY mtu=512" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             exchanges=2 renegotiation=1 \
             hs_timeout=10000-60000 \
             mtu=512" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             exchanges=2 renegotiation=1 renegotiate=1 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             hs_timeout=10000-60000 \
             mtu=512" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# An autoreduction on the client-side might happen if the server is
# slow to reset, therefore omitting '-C "autoreduction"' below.
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_max_content_len 1024
run_test    "DTLS fragmenting: proxy MTU, AES-CCM renego" \
            -p "$P_PXY mtu=1024" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             exchanges=2 renegotiation=1 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8 \
             hs_timeout=10000-60000 \
             mtu=1024" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             exchanges=2 renegotiation=1 renegotiate=1 \
             hs_timeout=10000-60000 \
             mtu=1024" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# An autoreduction on the client-side might happen if the server is
# slow to reset, therefore omitting '-C "autoreduction"' below.
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_config_enabled MBEDTLS_SSL_ENCRYPT_THEN_MAC
requires_max_content_len 1024
run_test    "DTLS fragmenting: proxy MTU, AES-CBC EtM renego" \
            -p "$P_PXY mtu=1024" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             exchanges=2 renegotiation=1 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256 \
             hs_timeout=10000-60000 \
             mtu=1024" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             exchanges=2 renegotiation=1 renegotiate=1 \
             hs_timeout=10000-60000 \
             mtu=1024" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# An autoreduction on the client-side might happen if the server is
# slow to reset, therefore omitting '-C "autoreduction"' below.
not_with_valgrind # spurious autoreduction due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_hash_alg SHA_256
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
requires_max_content_len 1024
run_test    "DTLS fragmenting: proxy MTU, AES-CBC non-EtM renego" \
            -p "$P_PXY mtu=1024" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             exchanges=2 renegotiation=1 \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256 etm=0 \
             hs_timeout=10000-60000 \
             mtu=1024" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             exchanges=2 renegotiation=1 renegotiate=1 \
             hs_timeout=10000-60000 \
             mtu=1024" \
            0 \
            -S "autoreduction" \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# Forcing ciphersuite for this test to fit the MTU of 512 with full config.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 2
requires_max_content_len 512
run_test    "DTLS fragmenting: proxy MTU + 3d" \
            -p "$P_PXY mtu=512 drop=8 delay=8 duplicate=8" \
            "$P_SRV dgram_packing=0 dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=250-10000 mtu=512" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             hs_timeout=250-10000 mtu=512" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# Forcing ciphersuite for this test to fit the MTU of 512 with full config.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 2
requires_max_content_len 512
run_test    "DTLS fragmenting: proxy MTU + 3d, nbio" \
            -p "$P_PXY mtu=512 drop=8 delay=8 duplicate=8" \
            "$P_SRV dtls=1 debug_level=2 auth_mode=required \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=250-10000 mtu=512 nbio=2" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             force_ciphersuite=TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256 \
             hs_timeout=250-10000 mtu=512 nbio=2" \
            0 \
            -s "found fragmented DTLS handshake message" \
            -c "found fragmented DTLS handshake message" \
            -C "error"

# interop tests for DTLS fragmentating with reliable connection
#
# here and below we just want to test that the we fragment in a way that
# pleases other implementations, so we don't need the peer to fragment
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_gnutls
requires_max_content_len 2048
run_test    "DTLS fragmenting: MTU=501, gnutls server, DTLS 1.2" \
            "$G_SRV -u" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             mtu=501 force_version=dtls12" \
            0 \
            -c "fragmenting Certificate handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_gnutls
requires_max_content_len 2048
run_test    "DTLS fragmenting: MTU=110, gnutls server, DTLS 1.2" \
            "$G_NEXT_SRV -u" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             request_size=35 \
             mtu=110 force_version=dtls12" \
            0 \
            -c "fragmenting ClientHello handshake message" \
            -c "fragmenting Certificate handshake message" \
            -C "error"

# We use --insecure for the GnuTLS client because it expects
# the hostname / IP it connects to to be the name used in the
# certificate obtained from the server. Here, however, it
# connects to 127.0.0.1 while our test certificates use 'localhost'
# as the server name in the certificate. This will make the
# certificate validation fail, but passing --insecure makes
# GnuTLS continue the connection nonetheless.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_gnutls
requires_not_i686
requires_max_content_len 2048
run_test    "DTLS fragmenting: MTU=536, gnutls client, DTLS 1.2" \
            "$P_SRV dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             mtu=536 force_version=dtls12" \
            "$G_CLI -u --insecure 127.0.0.1" \
            0 \
            -s "fragmenting Certificate handshake message"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_gnutls
requires_not_i686
requires_max_content_len 2048
run_test    "DTLS fragmenting: MTU=149, gnutls client, DTLS 1.2" \
            "$P_SRV dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             mtu=149 force_version=dtls12" \
            "$G_CLI -u --insecure 127.0.0.1" \
            0 \
            -s "fragmenting Certificate handshake message"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 2048
run_test    "DTLS fragmenting: MTU=525, openssl server, DTLS 1.2" \
            "$O_SRV -dtls1_2 -verify 10" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             mtu=525 force_version=dtls12" \
            0 \
            -c "fragmenting Certificate handshake message" \
            -C "error"

# Depending on the ciphersuite selected to encrypt the application data, the
# maximum application data payload per record may be small with an MTU of 128.
# For example, with TLS-ECDHE-ECDSA-WITH-AES-256-CBC-SHA384, this maximum is
# 35 bytes. We therefore reduce the size of the client request in this test.
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 2048
run_test    "DTLS fragmenting: MTU=130, openssl server, DTLS 1.2" \
            "$O_NEXT_SRV -dtls1_2 -verify 10" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             request_size=8 \
             mtu=130 force_version=dtls12" \
            0 \
            -c "fragmenting ClientHello handshake message" \
            -c "fragmenting Certificate handshake message" \
            -C "error"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 2048
run_test    "DTLS fragmenting: MTU=512, openssl client, DTLS 1.2" \
            "$P_SRV dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             mtu=512 force_version=dtls12" \
            "$O_CLI -dtls1_2" \
            0 \
            -s "fragmenting Certificate handshake message"

requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_max_content_len 2048
run_test    "DTLS fragmenting: MTU=131, openssl client, DTLS 1.2" \
            "$P_SRV dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             mtu=131 force_version=dtls12" \
            "$O_CLI -dtls1_2" \
            0 \
            -s "fragmenting Certificate handshake message"

# interop tests for DTLS fragmentating with unreliable connection
#
# again we just want to test that the we fragment in a way that
# pleases other implementations, so we don't need the peer to fragment
requires_gnutls_next
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 4
requires_max_content_len 2048
run_test    "DTLS fragmenting: 3d, MTU=434, gnutls server, DTLS 1.2" \
            -p "$P_PXY drop=8 delay=8 duplicate=8" \
            "$G_NEXT_SRV -u" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=250-60000 mtu=434 force_version=dtls12" \
            0 \
            -c "fragmenting Certificate handshake message" \
            -C "error"

requires_gnutls_next
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 6
requires_max_content_len 2048
run_test    "DTLS fragmenting: 3d, MTU=103, gnutls server, DTLS 1.2" \
            -p "$P_PXY drop=8 delay=8 duplicate=8" \
            "$G_NEXT_SRV -u" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             request_size=35 \
             hs_timeout=250-60000 mtu=103 force_version=dtls12" \
            0 \
            -c "fragmenting ClientHello handshake message" \
            -c "fragmenting Certificate handshake message" \
            -C "error"

requires_gnutls_next
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 4
requires_max_content_len 2048
run_test    "DTLS fragmenting: 3d, MTU=614, gnutls client, DTLS 1.2" \
            -p "$P_PXY drop=8 delay=8 duplicate=8" \
            "$P_SRV dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=250-60000 mtu=614 force_version=dtls12" \
           "$G_NEXT_CLI -u --insecure 127.0.0.1" \
            0 \
            -s "fragmenting Certificate handshake message"

requires_gnutls_next
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 4
requires_max_content_len 2048
run_test    "DTLS fragmenting: 3d, MTU=116, gnutls client, DTLS 1.2" \
            -p "$P_PXY drop=8 delay=8 duplicate=8" \
            "$P_SRV dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=250-60000 mtu=116 force_version=dtls12" \
           "$G_NEXT_CLI -u --insecure 127.0.0.1" \
            0 \
            -s "fragmenting Certificate handshake message"

## The test below requires 1.1.1a or higher version of openssl, otherwise
## it might trigger a bug due to openssl server (https://github.com/openssl/openssl/issues/6902)
requires_openssl_next
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 4
requires_max_content_len 2048
run_test    "DTLS fragmenting: 3d, MTU=541, openssl server, DTLS 1.2" \
            -p "$P_PXY drop=8 delay=8 duplicate=8" \
            "$O_NEXT_SRV -dtls1_2 -verify 10" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             hs_timeout=250-60000 mtu=541 force_version=dtls12" \
            0 \
            -c "fragmenting Certificate handshake message" \
            -C "error"

# Depending on the ciphersuite selected to encrypt the application data, the
# maximum application data payload per record may be small with an MTU of 128.
# For example, with TLS-ECDHE-ECDSA-WITH-AES-256-CBC-SHA384, this maximum is
# 35 bytes. We therefore reduce the size of the client request in this test.
requires_openssl_next
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 4
requires_max_content_len 2048
run_test    "DTLS fragmenting: 3d, MTU=108, openssl server, DTLS 1.2" \
            -p "$P_PXY drop=8 delay=8 duplicate=8" \
            "$O_NEXT_SRV -dtls1_2 -verify 10" \
            "$P_CLI dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server8_int-ca2.crt \
             key_file=$DATA_FILES_PATH/server8.key \
             request_size=8 \
             hs_timeout=250-60000 mtu=108 force_version=dtls12" \
            0 \
            -c "fragmenting ClientHello handshake message" \
            -c "fragmenting Certificate handshake message" \
            -C "error"

## the test below will time out with certain seed.
## The cause is an openssl bug (https://github.com/openssl/openssl/issues/18887)
skip_next_test
requires_config_enabled MBEDTLS_SSL_PROTO_DTLS
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
client_needs_more_time 4
requires_max_content_len 2048
run_test    "DTLS fragmenting: 3d, MTU=512, openssl client, DTLS 1.2" \
            -p "$P_PXY drop=8 delay=8 duplicate=8" \
            "$P_SRV dtls=1 debug_level=2 \
             crt_file=$DATA_FILES_PATH/server7_int-ca.crt \
             key_file=$DATA_FILES_PATH/server7.key \
             hs_timeout=250-60000 mtu=512 force_version=dtls12" \
            "$O_CLI -dtls1_2" \
            0 \
            -s "fragmenting Certificate handshake message"

# Tests for DTLS-SRTP (RFC 5764)
requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported" \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -C "error"


requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports one profile." \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=5 debug_level=3" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile: MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_80" \
          -s "selected srtp profile: MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_80" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_80" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports one profile. Client supports all profiles." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=6 debug_level=3" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile: MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_32" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_32" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one matching profile." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -s "selected srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one different profile." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=6 debug_level=3" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile: MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_32" \
          -S "selected srtp profile" \
          -S "server hello, adding use_srtp extension" \
          -S "DTLS-SRTP key material is"\
          -c "client hello, adding use_srtp extension" \
          -C "found use_srtp extension" \
          -C "found srtp profile" \
          -C "selected srtp profile" \
          -C "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server doesn't support use_srtp extension." \
          "$P_SRV dtls=1 debug_level=3" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -s "found use_srtp extension" \
          -S "server hello, adding use_srtp extension" \
          -S "DTLS-SRTP key material is"\
          -c "client hello, adding use_srtp extension" \
          -C "found use_srtp extension" \
          -C "found srtp profile" \
          -C "selected srtp profile" \
          -C "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported. mki used" \
          "$P_SRV dtls=1 use_srtp=1 support_mki=1 debug_level=3" \
          "$P_CLI dtls=1 use_srtp=1 mki=542310ab34290481 debug_level=3" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "dumping 'using mki' (8 bytes)" \
          -s "DTLS-SRTP key material is"\
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile" \
          -c "dumping 'sending mki' (8 bytes)" \
          -c "dumping 'received mki' (8 bytes)" \
          -c "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -g "find_in_both '^ *DTLS-SRTP mki value: [0-9A-F]*$'"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported. server doesn't support mki." \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$P_CLI dtls=1 use_srtp=1 mki=542310ab34290481 debug_level=3" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -s "DTLS-SRTP no mki value negotiated"\
          -S "dumping 'using mki' (8 bytes)" \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -c "DTLS-SRTP no mki value negotiated"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -c "dumping 'sending mki' (8 bytes)" \
          -C "dumping 'received mki' (8 bytes)" \
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported. openssl client." \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$O_CLI -dtls -use_srtp SRTP_AES128_CM_SHA1_80:SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -c "SRTP Extension negotiated, profile=SRTP_AES128_CM_SHA1_80"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports all profiles, in different order. openssl client." \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$O_CLI -dtls -use_srtp SRTP_AES128_CM_SHA1_32:SRTP_AES128_CM_SHA1_80 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -c "SRTP Extension negotiated, profile=SRTP_AES128_CM_SHA1_32"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports one profile. openssl client." \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$O_CLI -dtls -use_srtp SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -c "SRTP Extension negotiated, profile=SRTP_AES128_CM_SHA1_32"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports one profile. Client supports all profiles. openssl client." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          "$O_CLI -dtls -use_srtp SRTP_AES128_CM_SHA1_80:SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -c "SRTP Extension negotiated, profile=SRTP_AES128_CM_SHA1_32"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one matching profile. openssl client." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          "$O_CLI -dtls -use_srtp SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -g "find_in_both '^ *Keying material: [0-9A-F]*$'"\
          -c "SRTP Extension negotiated, profile=SRTP_AES128_CM_SHA1_32"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one different profile. openssl client." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=1 debug_level=3" \
          "$O_CLI -dtls -use_srtp SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -S "selected srtp profile" \
          -S "server hello, adding use_srtp extension" \
          -S "DTLS-SRTP key material is"\
          -C "SRTP Extension negotiated, profile"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server doesn't support use_srtp extension. openssl client" \
          "$P_SRV dtls=1 debug_level=3" \
          "$O_CLI -dtls -use_srtp SRTP_AES128_CM_SHA1_80:SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          0 \
          -s "found use_srtp extension" \
          -S "server hello, adding use_srtp extension" \
          -S "DTLS-SRTP key material is"\
          -C "SRTP Extension negotiated, profile"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported. openssl server" \
          "$O_SRV -dtls -verify 0 -use_srtp SRTP_AES128_CM_SHA1_80:SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports all profiles, in different order. openssl server." \
          "$O_SRV -dtls -verify 0 -use_srtp SRTP_AES128_CM_SHA1_32:SRTP_AES128_CM_SHA1_80 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports one profile. openssl server." \
          "$O_SRV -dtls -verify 0 -use_srtp SRTP_AES128_CM_SHA1_80:SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports one profile. Client supports all profiles. openssl server." \
          "$O_SRV -dtls -verify 0 -use_srtp SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one matching profile. openssl server." \
          "$O_SRV -dtls -verify 0 -use_srtp SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one different profile. openssl server." \
          "$O_SRV -dtls -verify 0 -use_srtp SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=6 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -C "found use_srtp extension" \
          -C "found srtp profile" \
          -C "selected srtp profile" \
          -C "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server doesn't support use_srtp extension. openssl server" \
          "$O_SRV -dtls" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -C "found use_srtp extension" \
          -C "found srtp profile" \
          -C "selected srtp profile" \
          -C "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported. server doesn't support mki. openssl server." \
          "$O_SRV -dtls -verify 0 -use_srtp SRTP_AES128_CM_SHA1_80:SRTP_AES128_CM_SHA1_32 -keymatexport 'EXTRACTOR-dtls_srtp' -keymatexportlen 60" \
          "$P_CLI dtls=1 use_srtp=1 mki=542310ab34290481 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -c "DTLS-SRTP no mki value negotiated"\
          -c "dumping 'sending mki' (8 bytes)" \
          -C "dumping 'received mki' (8 bytes)" \
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported. gnutls client." \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$G_CLI -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_80:SRTP_AES128_CM_HMAC_SHA1_32:SRTP_NULL_HMAC_SHA1_80:SRTP_NULL_SHA1_32 --insecure 127.0.0.1" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "SRTP profile: SRTP_AES128_CM_HMAC_SHA1_80"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports all profiles, in different order. gnutls client." \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$G_CLI -u --srtp-profiles=SRTP_NULL_HMAC_SHA1_80:SRTP_AES128_CM_HMAC_SHA1_80:SRTP_NULL_SHA1_32:SRTP_AES128_CM_HMAC_SHA1_32 --insecure 127.0.0.1" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "SRTP profile: SRTP_NULL_HMAC_SHA1_80"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports one profile. gnutls client." \
          "$P_SRV dtls=1 use_srtp=1 debug_level=3" \
          "$G_CLI -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_32 --insecure 127.0.0.1" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -s "selected srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "SRTP profile: SRTP_AES128_CM_HMAC_SHA1_32"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports one profile. Client supports all profiles. gnutls client." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=6 debug_level=3" \
          "$G_CLI -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_80:SRTP_AES128_CM_HMAC_SHA1_32:SRTP_NULL_HMAC_SHA1_80:SRTP_NULL_SHA1_32 --insecure 127.0.0.1" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile: MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_32" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "SRTP profile: SRTP_NULL_SHA1_32"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one matching profile. gnutls client." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          "$G_CLI -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_32 --insecure 127.0.0.1" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -s "selected srtp profile" \
          -s "server hello, adding use_srtp extension" \
          -s "DTLS-SRTP key material is"\
          -c "SRTP profile: SRTP_AES128_CM_HMAC_SHA1_32"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one different profile. gnutls client." \
          "$P_SRV dtls=1 use_srtp=1 srtp_force_profile=1 debug_level=3" \
          "$G_CLI -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_32 --insecure 127.0.0.1" \
          0 \
          -s "found use_srtp extension" \
          -s "found srtp profile" \
          -S "selected srtp profile" \
          -S "server hello, adding use_srtp extension" \
          -S "DTLS-SRTP key material is"\
          -C "SRTP profile:"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server doesn't support use_srtp extension. gnutls client" \
          "$P_SRV dtls=1 debug_level=3" \
          "$G_CLI -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_80:SRTP_AES128_CM_HMAC_SHA1_32:SRTP_NULL_HMAC_SHA1_80:SRTP_NULL_SHA1_32 --insecure 127.0.0.1" \
          0 \
          -s "found use_srtp extension" \
          -S "server hello, adding use_srtp extension" \
          -S "DTLS-SRTP key material is"\
          -C "SRTP profile:"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported. gnutls server" \
          "$G_SRV -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_80:SRTP_AES128_CM_HMAC_SHA1_32:SRTP_NULL_HMAC_SHA1_80:SRTP_NULL_SHA1_32" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports all profiles, in different order. gnutls server." \
          "$G_SRV -u --srtp-profiles=SRTP_NULL_SHA1_32:SRTP_AES128_CM_HMAC_SHA1_32:SRTP_AES128_CM_HMAC_SHA1_80:SRTP_NULL_HMAC_SHA1_80:SRTP_NULL_SHA1_32" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports all profiles. Client supports one profile. gnutls server." \
          "$G_SRV -u --srtp-profiles=SRTP_NULL_SHA1_32:SRTP_AES128_CM_HMAC_SHA1_32:SRTP_AES128_CM_HMAC_SHA1_80:SRTP_NULL_HMAC_SHA1_80:SRTP_NULL_SHA1_32" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server supports one profile. Client supports all profiles. gnutls server." \
          "$G_SRV -u --srtp-profiles=SRTP_NULL_HMAC_SHA1_80" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_80" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one matching profile. gnutls server." \
          "$G_SRV -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_32" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=2 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile: MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server and Client support only one different profile. gnutls server." \
          "$G_SRV -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_32" \
          "$P_CLI dtls=1 use_srtp=1 srtp_force_profile=6 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -C "found use_srtp extension" \
          -C "found srtp profile" \
          -C "selected srtp profile" \
          -C "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP server doesn't support use_srtp extension. gnutls server" \
          "$G_SRV -u" \
          "$P_CLI dtls=1 use_srtp=1 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -C "found use_srtp extension" \
          -C "found srtp profile" \
          -C "selected srtp profile" \
          -C "DTLS-SRTP key material is"\
          -C "error"

requires_config_enabled MBEDTLS_SSL_DTLS_SRTP
requires_gnutls
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test  "DTLS-SRTP all profiles supported. mki used. gnutls server." \
          "$G_SRV -u --srtp-profiles=SRTP_AES128_CM_HMAC_SHA1_80:SRTP_AES128_CM_HMAC_SHA1_32:SRTP_NULL_HMAC_SHA1_80:SRTP_NULL_SHA1_32" \
          "$P_CLI dtls=1 use_srtp=1 mki=542310ab34290481 debug_level=3" \
          0 \
          -c "client hello, adding use_srtp extension" \
          -c "found use_srtp extension" \
          -c "found srtp profile" \
          -c "selected srtp profile" \
          -c "DTLS-SRTP key material is"\
          -c "DTLS-SRTP mki value:"\
          -c "dumping 'sending mki' (8 bytes)" \
          -c "dumping 'received mki' (8 bytes)" \
          -C "error"

# Tests for specific things with "unreliable" UDP connection

not_with_valgrind # spurious resend due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: reference" \
            -p "$P_PXY" \
            "$P_SRV dtls=1 debug_level=2 hs_timeout=10000-20000" \
            "$P_CLI dtls=1 debug_level=2 hs_timeout=10000-20000" \
            0 \
            -C "replayed record" \
            -S "replayed record" \
            -C "Buffer record from epoch" \
            -S "Buffer record from epoch" \
            -C "ssl_buffer_message" \
            -S "ssl_buffer_message" \
            -C "discarding invalid record" \
            -S "discarding invalid record" \
            -S "resend" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

not_with_valgrind # spurious resend due to timeout
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: duplicate every packet" \
            -p "$P_PXY duplicate=1" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=2 hs_timeout=10000-20000" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=2 hs_timeout=10000-20000" \
            0 \
            -c "replayed record" \
            -s "replayed record" \
            -c "record from another epoch" \
            -s "record from another epoch" \
            -S "resend" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: duplicate every packet, server anti-replay off" \
            -p "$P_PXY duplicate=1" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=2 anti_replay=0" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=2" \
            0 \
            -c "replayed record" \
            -S "replayed record" \
            -c "record from another epoch" \
            -s "record from another epoch" \
            -c "resend" \
            -s "resend" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: multiple records in same datagram" \
            -p "$P_PXY pack=50" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=2" \
            0 \
            -c "next record in same datagram" \
            -s "next record in same datagram"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: multiple records in same datagram, duplicate every packet" \
            -p "$P_PXY pack=50 duplicate=1" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=2" \
            0 \
            -c "next record in same datagram" \
            -s "next record in same datagram"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: inject invalid AD record, default badmac_limit" \
            -p "$P_PXY bad_ad=1" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=1" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=1 read_timeout=100" \
            0 \
            -c "discarding invalid record (mac)" \
            -s "discarding invalid record (mac)" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK" \
            -S "too many records with bad MAC" \
            -S "Verification of the message MAC failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: inject invalid AD record, badmac_limit 1" \
            -p "$P_PXY bad_ad=1" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=1 badmac_limit=1" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=1 read_timeout=100" \
            1 \
            -C "discarding invalid record (mac)" \
            -S "discarding invalid record (mac)" \
            -S "Extra-header:" \
            -C "HTTP/1.0 200 OK" \
            -s "too many records with bad MAC" \
            -s "Verification of the message MAC failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: inject invalid AD record, badmac_limit 2" \
            -p "$P_PXY bad_ad=1" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=1 badmac_limit=2" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=1 read_timeout=100" \
            0 \
            -c "discarding invalid record (mac)" \
            -s "discarding invalid record (mac)" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK" \
            -S "too many records with bad MAC" \
            -S "Verification of the message MAC failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: inject invalid AD record, badmac_limit 2, exchanges 2"\
            -p "$P_PXY bad_ad=1" \
            "$P_SRV dtls=1 dgram_packing=0 debug_level=1 badmac_limit=2 exchanges=2" \
            "$P_CLI dtls=1 dgram_packing=0 debug_level=1 read_timeout=100 exchanges=2" \
            1 \
            -c "discarding invalid record (mac)" \
            -s "discarding invalid record (mac)" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK" \
            -s "too many records with bad MAC" \
            -s "Verification of the message MAC failed"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: delay ChangeCipherSpec" \
            -p "$P_PXY delay_ccs=1" \
            "$P_SRV dtls=1 debug_level=1 dgram_packing=0" \
            "$P_CLI dtls=1 debug_level=1 dgram_packing=0" \
            0 \
            -c "record from another epoch" \
            -s "record from another epoch" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

# Tests for reordering support with DTLS

requires_certificate_authentication
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reordering: Buffer out-of-order handshake message on client" \
            -p "$P_PXY delay_srv=ServerHello" \
            "$P_SRV dgram_packing=0 cookies=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            0 \
            -c "Buffering HS message" \
            -c "Certificate handshake message has been buffered$"\
            -S "Buffering HS message" \
            -S "handshake message has been buffered"\
            -C "Injecting buffered CCS message" \
            -C "Remember CCS message" \
            -S "Injecting buffered CCS message" \
            -S "Remember CCS message"

requires_certificate_authentication
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reordering: Buffer out-of-order handshake message fragment on client" \
            -p "$P_PXY delay_srv=ServerHello" \
            "$P_SRV mtu=512 dgram_packing=0 cookies=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            0 \
            -c "Buffering HS message" \
            -c "found fragmented DTLS handshake message"\
            -c "Next handshake message 1 not or only partially buffered" \
            -c "Certificate handshake message has been buffered and reassembled"\
            -S "Buffering HS message" \
            -S "handshake message has been buffered" \
            -C "Injecting buffered CCS message" \
            -C "Remember CCS message" \
            -S "Injecting buffered CCS message" \
            -S "Remember CCS message"

# The client buffers the ServerKeyExchange before receiving the fragmented
# Certificate message; at the time of writing, together these are aroudn 1200b
# in size, so that the bound below ensures that the certificate can be reassembled
# while keeping the ServerKeyExchange.
requires_certificate_authentication
requires_config_value_at_least "MBEDTLS_SSL_DTLS_MAX_BUFFERING" 1300
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reordering: Buffer out-of-order hs msg before reassembling next" \
            -p "$P_PXY delay_srv=Certificate delay_srv=Certificate" \
            "$P_SRV mtu=512 dgram_packing=0 cookies=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            0 \
            -c "Buffering HS message" \
            -c "Certificate handshake message has been buffered and reassembled"\
            -c "ServerKeyExchange handshake message has been buffered$"\
            -C "attempt to make space by freeing buffered messages" \
            -S "Buffering HS message" \
            -S "handshake message has been buffered" \
            -C "Injecting buffered CCS message" \
            -C "Remember CCS message" \
            -S "Injecting buffered CCS message" \
            -S "Remember CCS message"

# The size constraints ensure that the delayed certificate message can't
# be reassembled while keeping the ServerKeyExchange message, but it can
# when dropping it first.
requires_certificate_authentication
requires_config_value_at_least "MBEDTLS_SSL_DTLS_MAX_BUFFERING" 900
requires_config_value_at_most "MBEDTLS_SSL_DTLS_MAX_BUFFERING" 1299
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reordering: Buffer out-of-order hs msg before reassembling next, free buffered msg" \
            -p "$P_PXY delay_srv=Certificate delay_srv=Certificate" \
            "$P_SRV mtu=512 dgram_packing=0 cookies=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            0 \
            -c "Buffering HS message" \
            -c "attempt to make space by freeing buffered future messages" \
            -c "Enough space available after freeing buffered HS messages" \
            -S "Buffering HS message" \
            -S "handshake message has been buffered" \
            -C "Injecting buffered CCS message" \
            -C "Remember CCS message" \
            -S "Injecting buffered CCS message" \
            -S "Remember CCS message"

requires_certificate_authentication
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reordering: Buffer out-of-order handshake message on server" \
            -p "$P_PXY delay_cli=Certificate" \
            "$P_SRV dgram_packing=0 auth_mode=required cookies=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            0 \
            -C "Buffering HS message" \
            -C "handshake message has been buffered" \
            -s "Buffering HS message" \
            -s "ClientKeyExchange handshake message has been buffered$" \
            -C "Injecting buffered CCS message" \
            -C "Remember CCS message" \
            -S "Injecting buffered CCS message" \
            -S "Remember CCS message"

requires_certificate_authentication
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "DTLS reordering: Buffer out-of-order CCS message on client"\
            -p "$P_PXY delay_srv=NewSessionTicket" \
            "$P_SRV dgram_packing=0 cookies=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            0 \
            -C "Buffering HS message" \
            -C "handshake message has been buffered" \
            -S "Buffering HS message" \
            -S "handshake message has been buffered" \
            -c "Injecting buffered CCS message" \
            -c "Remember CCS message" \
            -S "Injecting buffered CCS message" \
            -S "Remember CCS message"

requires_certificate_authentication
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reordering: Buffer out-of-order CCS message on server"\
            -p "$P_PXY delay_cli=ClientKeyExchange" \
            "$P_SRV dgram_packing=0 cookies=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            0 \
            -C "Buffering HS message" \
            -C "handshake message has been buffered" \
            -S "Buffering HS message" \
            -S "handshake message has been buffered" \
            -C "Injecting buffered CCS message" \
            -C "Remember CCS message" \
            -s "Injecting buffered CCS message" \
            -s "Remember CCS message"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS reordering: Buffer encrypted Finished message" \
            -p "$P_PXY delay_ccs=1" \
            "$P_SRV dgram_packing=0 cookies=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 \
            hs_timeout=2500-60000" \
            0 \
            -s "Buffer record from epoch 1" \
            -s "Found buffered record from current epoch - load" \
            -c "Buffer record from epoch 1" \
            -c "Found buffered record from current epoch - load"

# In this test, both the fragmented NewSessionTicket and the ChangeCipherSpec
# from the server are delayed, so that the encrypted Finished message
# is received and buffered. When the fragmented NewSessionTicket comes
# in afterwards, the encrypted Finished message must be freed in order
# to make space for the NewSessionTicket to be reassembled.
# This works only in very particular circumstances:
# - MBEDTLS_SSL_DTLS_MAX_BUFFERING must be large enough to allow buffering
#   of the NewSessionTicket, but small enough to also allow buffering of
#   the encrypted Finished message.
# - The MTU setting on the server must be so small that the NewSessionTicket
#   needs to be fragmented.
# - All messages sent by the server must be small enough to be either sent
#   without fragmentation or be reassembled within the bounds of
#   MBEDTLS_SSL_DTLS_MAX_BUFFERING. Achieve this by testing with a PSK-based
#   handshake, omitting CRTs.
requires_config_value_at_least "MBEDTLS_SSL_DTLS_MAX_BUFFERING" 190
requires_config_value_at_most "MBEDTLS_SSL_DTLS_MAX_BUFFERING" 230
run_test    "DTLS reordering: Buffer encrypted Finished message, drop for fragmented NewSessionTicket" \
            -p "$P_PXY delay_srv=NewSessionTicket delay_srv=NewSessionTicket delay_ccs=1" \
            "$P_SRV mtu=140 response_size=90 dgram_packing=0 psk=73776f726466697368 psk_identity=foo cookies=0 dtls=1 debug_level=2" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8 psk=73776f726466697368 psk_identity=foo" \
            0 \
            -s "Buffer record from epoch 1" \
            -s "Found buffered record from current epoch - load" \
            -c "Buffer record from epoch 1" \
            -C "Found buffered record from current epoch - load" \
            -c "Enough space available after freeing future epoch record"

# Tests for "randomly unreliable connection": try a variety of flows and peers

client_needs_more_time 2
run_test    "DTLS proxy: 3d (drop, delay, duplicate), \"short\" PSK handshake" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 \
             psk=73776f726466697368" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 psk=73776f726466697368 \
             force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8" \
            0 \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 2
run_test    "DTLS proxy: 3d, \"short\" ECDHE-RSA handshake" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=none" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 \
             force_ciphersuite=TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA" \
            0 \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, \"short\" (no ticket, no cli_auth) FS handshake" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=none" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0" \
            0 \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, FS, client auth" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=required" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0" \
            0 \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "DTLS proxy: 3d, FS, ticket" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=1 auth_mode=none" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=1" \
            0 \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "DTLS proxy: 3d, max handshake (FS, ticket + client auth)" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=1 auth_mode=required" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=1" \
            0 \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "DTLS proxy: 3d, max handshake, nbio" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 nbio=2 tickets=1 \
             auth_mode=required" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 nbio=2 tickets=1" \
            0 \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 4
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "DTLS proxy: 3d, min handshake, resumption" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=none \
             psk=73776f726466697368 debug_level=3" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 psk=73776f726466697368 \
             debug_level=3 reconnect=1 skip_close_notify=1 read_timeout=1000 max_resend=10 \
             force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8" \
            0 \
            -s "a session has been resumed" \
            -c "a session has been resumed" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 4
requires_config_enabled MBEDTLS_SSL_CACHE_C
run_test    "DTLS proxy: 3d, min handshake, resumption, nbio" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=none \
             psk=73776f726466697368 debug_level=3 nbio=2" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 psk=73776f726466697368 \
             debug_level=3 reconnect=1 skip_close_notify=1 read_timeout=1000 max_resend=10 \
             force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8 nbio=2" \
            0 \
            -s "a session has been resumed" \
            -c "a session has been resumed" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 4
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "DTLS proxy: 3d, min handshake, client-initiated renego" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=none \
             psk=73776f726466697368 renegotiation=1 debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 psk=73776f726466697368 \
             renegotiate=1 debug_level=2 \
             force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8" \
            0 \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 4
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "DTLS proxy: 3d, min handshake, client-initiated renego, nbio" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=none \
             psk=73776f726466697368 renegotiation=1 debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 psk=73776f726466697368 \
             renegotiate=1 debug_level=2 \
             force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8" \
            0 \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 4
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "DTLS proxy: 3d, min handshake, server-initiated renego" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=none \
             psk=73776f726466697368 renegotiate=1 renegotiation=1 exchanges=4 \
             debug_level=2" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 psk=73776f726466697368 \
             renegotiation=1 exchanges=4 debug_level=2 \
             force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8" \
            0 \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

client_needs_more_time 4
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "DTLS proxy: 3d, min handshake, server-initiated renego, nbio" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 auth_mode=none \
             psk=73776f726466697368 renegotiate=1 renegotiation=1 exchanges=4 \
             debug_level=2 nbio=2" \
            "$P_CLI dtls=1 dgram_packing=0 hs_timeout=500-10000 tickets=0 psk=73776f726466697368 \
             renegotiation=1 exchanges=4 debug_level=2 nbio=2 \
             force_ciphersuite=TLS-PSK-WITH-AES-128-CCM-8" \
            0 \
            -c "=> renegotiate" \
            -s "=> renegotiate" \
            -s "Extra-header:" \
            -c "HTTP/1.0 200 OK"

## The three tests below require 1.1.1a or higher version of openssl, otherwise
## it might trigger a bug due to openssl (https://github.com/openssl/openssl/issues/6902)
## Besides, openssl should use dtls1_2 or dtls, otherwise it will cause "SSL alert number 70" error
requires_openssl_next
client_needs_more_time 6
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, openssl server" \
            -p "$P_PXY drop=5 delay=5 duplicate=5 protect_hvr=1" \
            "$O_NEXT_SRV -dtls1_2 -mtu 2048" \
            "$P_CLI dgram_packing=0 dtls=1 hs_timeout=500-60000 tickets=0" \
            0 \
            -c "HTTP/1.0 200 OK"

requires_openssl_next
client_needs_more_time 8
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, openssl server, fragmentation" \
            -p "$P_PXY drop=5 delay=5 duplicate=5 protect_hvr=1" \
            "$O_NEXT_SRV -dtls1_2 -mtu 277" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 hs_timeout=500-60000 tickets=0" \
            0 \
            -c "HTTP/1.0 200 OK" \
            -c "Certificate handshake message has been buffered and reassembled"

requires_openssl_next
client_needs_more_time 8
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, openssl server, fragmentation, nbio" \
            -p "$P_PXY drop=5 delay=5 duplicate=5 protect_hvr=1" \
            "$O_NEXT_SRV -dtls1_2 -mtu 268" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 hs_timeout=500-60000 nbio=2 tickets=0" \
            0 \
            -c "HTTP/1.0 200 OK" \
            -c "Certificate handshake message has been buffered and reassembled"

requires_openssl_next
client_needs_more_time 6
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, openssl client" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dgram_packing=0 dtls=1 hs_timeout=500-60000 tickets=0" \
            "$O_NEXT_CLI -dtls1_2 -mtu 2048" \
            0 \
            -s "HTTP/1.0 200 OK"

# The following test fails intermittently on the CI with a frequency that
# significantly impacts CI throughput. Disable it for the time being.
# See https://github.com/Mbed-TLS/mbedtls/issues/10652 for more information.
skip_next_test
requires_openssl_next
client_needs_more_time 8
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, openssl client, fragmentation" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV debug_level=2 dgram_packing=0 auth_mode=required dtls=1 hs_timeout=500-60000 tickets=0" \
            "$O_NEXT_CLI -dtls1_2 -mtu 260 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            0 \
            -s "HTTP/1.0 200 OK" \
            -s "found fragmented DTLS handshake message" \
            -s "Certificate handshake message has been buffered and reassembled"

# The following test fails intermittently on the CI with a frequency that
# significantly impacts CI throughput. Disable it for the time being.
# See https://github.com/Mbed-TLS/mbedtls/issues/10652 for more information.
skip_next_test
requires_openssl_next
client_needs_more_time 8
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, openssl client, fragmentation, nbio" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV debug_level=2 dgram_packing=0 auth_mode=required dtls=1 hs_timeout=500-60000 nbio=2 tickets=0" \
            "$O_NEXT_CLI -dtls1_2 -mtu 259 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            0 \
            -s "HTTP/1.0 200 OK" \
            -s "found fragmented DTLS handshake message" \
            -s "Certificate handshake message has been buffered and reassembled"

requires_gnutls
client_needs_more_time 6
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, gnutls server" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$G_SRV -u --mtu 2048 -a" \
            "$P_CLI dgram_packing=0 dtls=1 hs_timeout=500-60000" \
            0 \
            -s "Extra-header:" \
            -c "Extra-header:"

requires_gnutls_next
client_needs_more_time 8
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, gnutls server, fragmentation" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$G_NEXT_SRV -u --mtu 499" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 hs_timeout=500-60000" \
            0 \
            -s "Extra-header:" \
            -c "Extra-header:" \
            -c "Certificate handshake message has been buffered and reassembled"

requires_gnutls_next
client_needs_more_time 8
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, gnutls server, fragmentation, nbio" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$G_NEXT_SRV -u --mtu 528" \
            "$P_CLI dgram_packing=0 dtls=1 debug_level=2 hs_timeout=500-60000 nbio=2" \
            0 \
            -s "Extra-header:" \
            -c "Extra-header:" \
            -c "Certificate handshake message has been buffered and reassembled"

requires_gnutls
client_needs_more_time 6
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, gnutls client" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dgram_packing=0 dtls=1" \
            "$G_NEXT_CLI -u --mtu 2048 --insecure 127.0.0.1" \
            0 \
            -s "HTTP/1.0 200 OK"

# Set the MTU to 131 bytes. The ClientHello is not guaranteed to be surely
# fragmented but it is very likely. For example, the ClientHello sent by the
# GnuTLS 3.7.2 client is 206 bytes in this test. We expect ClientHello
# fragmentation to remain the case across GnuTLS version updates. Avoid using a
# smaller MTU, as the smaller the MTU, the more likely the handshake is to fail
# in this very unreliable connection emulation.

# The following test fails intermittently on the CI with a frequency that
# significantly impacts CI throughput. Disable it for the time being.
# See https://github.com/Mbed-TLS/mbedtls/issues/10652 for more information.
skip_next_test
requires_gnutls
client_needs_more_time 8
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, gnutls client, fragmentation" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dgram_packing=0 dtls=1 debug_level=2" \
            "$G_NEXT_CLI -u --mtu 131 --insecure 127.0.0.1" \
            0 \
            -s "HTTP/1.0 200 OK" \
            -s "ClientHello handshake message has been buffered and reassembled"

# The following test fails intermittently on the CI with a frequency that
# significantly impacts CI throughput. Disable it for the time being.
# See https://github.com/Mbed-TLS/mbedtls/issues/10652 for more information.
skip_next_test
requires_gnutls
client_needs_more_time 8
not_with_valgrind # risk of non-mbedtls peer timing out
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "DTLS proxy: 3d, gnutls client, fragmentation, nbio=2" \
            -p "$P_PXY drop=5 delay=5 duplicate=5" \
            "$P_SRV dgram_packing=0 dtls=1  debug_level=2 nbio=2" \
            "$G_NEXT_CLI -u --mtu 135 --insecure 127.0.0.1" \
            0 \
            -s "HTTP/1.0 200 OK" \
            -s "ClientHello handshake message has been buffered and reassembled"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "export keys functionality" \
            "$P_SRV eap_tls=1 debug_level=3" \
            "$P_CLI force_version=tls12 eap_tls=1 debug_level=3" \
            0 \
            -c "EAP-TLS key material is:"\
            -s "EAP-TLS key material is:"\
            -c "EAP-TLS IV is:" \
            -s "EAP-TLS IV is:"

# openssl feature tests: check if tls1.3 exists.
requires_openssl_tls1_3
run_test    "TLS 1.3: Test openssl tls1_3 feature" \
            "$O_NEXT_SRV -tls1_3 -msg" \
            "$O_NEXT_CLI -tls1_3 -msg" \
            0 \
            -c "TLS 1.3" \
            -s "TLS 1.3"

# gnutls feature tests: check if TLS 1.3 is supported as well as the NO_TICKETS and DISABLE_TLS13_COMPAT_MODE options.
requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
run_test    "TLS 1.3: Test gnutls tls1_3 feature" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE --disable-client-cert " \
            "$G_NEXT_CLI localhost --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "Version: TLS1.3" \
            -c "Version: TLS1.3"

# TLS1.3 test cases
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_ciphersuite_enabled TLS1-3-CHACHA20-POLY1305-SHA256
requires_any_configs_enabled "PSA_WANT_ECC_MONTGOMERY_255"
requires_any_configs_enabled "PSA_WANT_ECC_SECP_R1_256"
run_test    "TLS 1.3: Default" \
            "$P_SRV allow_sha1=0 debug_level=3 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key force_version=tls13" \
            "$P_CLI allow_sha1=0" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "Ciphersuite is TLS1-3-CHACHA20-POLY1305-SHA256" \
            -s "ECDH/FFDH group: " \
            -s "selected signature algorithm ecdsa_secp256r1_sha256"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Establish TLS 1.2 then TLS 1.3 session" \
            "$P_SRV" \
            "( $P_CLI force_version=tls12; \
               $P_CLI force_version=tls13 )" \
            0 \
            -s "Protocol is TLSv1.2" \
            -s "Protocol is TLSv1.3" \

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled $TLS1_2_KEY_EXCHANGES_WITH_CERT
run_test    "Establish TLS 1.3 then TLS 1.2 session" \
            "$P_SRV" \
            "( $P_CLI force_version=tls13; \
               $P_CLI force_version=tls12 )" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "Protocol is TLSv1.2" \

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: minimal feature sets - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache" \
            "$P_CLI debug_level=3" \
            0 \
            -c "client state: MBEDTLS_SSL_HELLO_REQUEST" \
            -c "client state: MBEDTLS_SSL_SERVER_HELLO" \
            -c "client state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -c "client state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -c "client state: MBEDTLS_SSL_SERVER_FINISHED" \
            -c "client state: MBEDTLS_SSL_CLIENT_FINISHED" \
            -c "client state: MBEDTLS_SSL_FLUSH_BUFFERS" \
            -c "client state: MBEDTLS_SSL_HANDSHAKE_WRAPUP" \
            -c "<= ssl_tls13_process_server_hello" \
            -c "server hello, chosen ciphersuite: ( 1303 ) - TLS1-3-CHACHA20-POLY1305-SHA256" \
            -c "DHE group name: " \
            -c "=> ssl_tls13_process_server_hello" \
            -c "<= parse encrypted extensions" \
            -c "Certificate verification flags clear" \
            -c "=> parse certificate verify" \
            -c "<= parse certificate verify" \
            -c "mbedtls_ssl_tls13_process_certificate_verify() returned 0" \
            -c "<= parse finished message" \
            -c "Protocol is TLSv1.3" \
            -c "HTTP/1.0 200 ok"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: minimal feature sets - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS --disable-client-cert" \
            "$P_CLI debug_level=3" \
            0 \
            -s "SERVER HELLO was queued" \
            -c "client state: MBEDTLS_SSL_HELLO_REQUEST" \
            -c "client state: MBEDTLS_SSL_SERVER_HELLO" \
            -c "client state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -c "client state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -c "client state: MBEDTLS_SSL_SERVER_FINISHED" \
            -c "client state: MBEDTLS_SSL_CLIENT_FINISHED" \
            -c "client state: MBEDTLS_SSL_FLUSH_BUFFERS" \
            -c "client state: MBEDTLS_SSL_HANDSHAKE_WRAPUP" \
            -c "<= ssl_tls13_process_server_hello" \
            -c "server hello, chosen ciphersuite: ( 1303 ) - TLS1-3-CHACHA20-POLY1305-SHA256" \
            -c "DHE group name: " \
            -c "=> ssl_tls13_process_server_hello" \
            -c "<= parse encrypted extensions" \
            -c "Certificate verification flags clear" \
            -c "=> parse certificate verify" \
            -c "<= parse certificate verify" \
            -c "mbedtls_ssl_tls13_process_certificate_verify() returned 0" \
            -c "<= parse finished message" \
            -c "Protocol is TLSv1.3" \
            -c "HTTP/1.0 200 OK"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_ALPN
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: alpn - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -alpn h2" \
            "$P_CLI debug_level=3 alpn=h2" \
            0 \
            -c "client state: MBEDTLS_SSL_HELLO_REQUEST" \
            -c "client state: MBEDTLS_SSL_SERVER_HELLO" \
            -c "client state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -c "client state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -c "client state: MBEDTLS_SSL_SERVER_FINISHED" \
            -c "client state: MBEDTLS_SSL_CLIENT_FINISHED" \
            -c "client state: MBEDTLS_SSL_FLUSH_BUFFERS" \
            -c "client state: MBEDTLS_SSL_HANDSHAKE_WRAPUP" \
            -c "<= ssl_tls13_process_server_hello" \
            -c "server hello, chosen ciphersuite: ( 1303 ) - TLS1-3-CHACHA20-POLY1305-SHA256" \
            -c "DHE group name: " \
            -c "=> ssl_tls13_process_server_hello" \
            -c "<= parse encrypted extensions" \
            -c "Certificate verification flags clear" \
            -c "=> parse certificate verify" \
            -c "<= parse certificate verify" \
            -c "mbedtls_ssl_tls13_process_certificate_verify() returned 0" \
            -c "<= parse finished message" \
            -c "Protocol is TLSv1.3" \
            -c "HTTP/1.0 200 ok" \
            -c "Application Layer Protocol is h2"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_ALPN
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: alpn - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS --disable-client-cert --alpn=h2" \
            "$P_CLI debug_level=3 alpn=h2" \
            0 \
            -s "SERVER HELLO was queued" \
            -c "client state: MBEDTLS_SSL_HELLO_REQUEST" \
            -c "client state: MBEDTLS_SSL_SERVER_HELLO" \
            -c "client state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -c "client state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -c "client state: MBEDTLS_SSL_SERVER_FINISHED" \
            -c "client state: MBEDTLS_SSL_CLIENT_FINISHED" \
            -c "client state: MBEDTLS_SSL_FLUSH_BUFFERS" \
            -c "client state: MBEDTLS_SSL_HANDSHAKE_WRAPUP" \
            -c "<= ssl_tls13_process_server_hello" \
            -c "server hello, chosen ciphersuite: ( 1303 ) - TLS1-3-CHACHA20-POLY1305-SHA256" \
            -c "DHE group name: " \
            -c "=> ssl_tls13_process_server_hello" \
            -c "<= parse encrypted extensions" \
            -c "Certificate verification flags clear" \
            -c "=> parse certificate verify" \
            -c "<= parse certificate verify" \
            -c "mbedtls_ssl_tls13_process_certificate_verify() returned 0" \
            -c "<= parse finished message" \
            -c "Protocol is TLSv1.3" \
            -c "HTTP/1.0 200 OK" \
            -c "Application Layer Protocol is h2"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_ALPN
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: server alpn - openssl" \
            "$P_SRV debug_level=3 tickets=0 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key alpn=h2" \
            "$O_NEXT_CLI -msg -tls1_3 -no_middlebox -alpn h2" \
            0 \
            -s "found alpn extension" \
            -s "server side, adding alpn extension" \
            -s "Protocol is TLSv1.3" \
            -s "HTTP/1.0 200 OK" \
            -s "Application Layer Protocol is h2"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_ALPN
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: server alpn - gnutls" \
            "$P_SRV debug_level=3 tickets=0 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key alpn=h2" \
            "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V --alpn h2" \
            0 \
            -s "found alpn extension" \
            -s "server side, adding alpn extension" \
            -s "Protocol is TLSv1.3" \
            -s "HTTP/1.0 200 OK" \
            -s "Application Layer Protocol is h2"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, no client certificate - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -verify 10" \
            "$P_CLI debug_level=4 crt_file=none key_file=none" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -s "TLS 1.3" \
            -c "HTTP/1.0 200 ok" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, no client certificate - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS --verify-client-cert" \
            "$P_CLI debug_level=3 crt_file=none key_file=none" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE"\
            -s "Version: TLS1.3" \
            -c "HTTP/1.0 200 OK" \
            -c "Protocol is TLSv1.3"


requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, no server middlebox compat - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10 -no_middlebox" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/cli2.crt key_file=$DATA_FILES_PATH/cli2.key" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, no server middlebox compat - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/cli2.crt \
                    key_file=$DATA_FILES_PATH/cli2.key" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, ecdsa_secp256r1_sha256 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/ecdsa_secp256r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp256r1.key" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, ecdsa_secp256r1_sha256 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp256r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp256r1.key" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, ecdsa_secp384r1_sha384 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/ecdsa_secp384r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp384r1.key" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, ecdsa_secp384r1_sha384 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp384r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp384r1.key" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, ecdsa_secp521r1_sha512 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/ecdsa_secp521r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp521r1.key" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, ecdsa_secp521r1_sha512 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp521r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp521r1.key" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, rsa_pss_rsae_sha256 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/cert_sha256.crt \
                    key_file=$DATA_FILES_PATH/server1.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha256" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, rsa_pss_rsae_sha256 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
                    key_file=$DATA_FILES_PATH/server2.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha256" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, rsa_pss_rsae_sha384 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/cert_sha256.crt \
                    key_file=$DATA_FILES_PATH/server1.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha384" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, rsa_pss_rsae_sha384 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
                    key_file=$DATA_FILES_PATH/server2.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha384" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, rsa_pss_rsae_sha512 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/cert_sha256.crt \
                    key_file=$DATA_FILES_PATH/server1.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha512" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, rsa_pss_rsae_sha512 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
                    key_file=$DATA_FILES_PATH/server2.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha512" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, client alg not in server list - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10
                -sigalgs ecdsa_secp256r1_sha256" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp521r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp521r1.key sig_algs=ecdsa_secp256r1_sha256,ecdsa_secp521r1_sha512" \
            1 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "no suitable signature algorithm"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication, client alg not in server list - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:-SIGN-ALL:+SIGN-ECDSA-SECP256R1-SHA256:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp521r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp521r1.key sig_algs=ecdsa_secp256r1_sha256,ecdsa_secp521r1_sha512" \
            1 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "no suitable signature algorithm"

# Test using an opaque private key for client authentication
requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, no server middlebox compat - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10 -no_middlebox" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/cli2.crt key_file=$DATA_FILES_PATH/cli2.key key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, no server middlebox compat - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/cli2.crt \
                    key_file=$DATA_FILES_PATH/cli2.key key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, ecdsa_secp256r1_sha256 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/ecdsa_secp256r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp256r1.key key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, ecdsa_secp256r1_sha256 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp256r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp256r1.key key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, ecdsa_secp384r1_sha384 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/ecdsa_secp384r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp384r1.key key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, ecdsa_secp384r1_sha384 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp384r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp384r1.key key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, ecdsa_secp521r1_sha512 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/ecdsa_secp521r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp521r1.key key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, ecdsa_secp521r1_sha512 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp521r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp521r1.key key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, rsa_pss_rsae_sha256 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/cert_sha256.crt \
                    key_file=$DATA_FILES_PATH/server1.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha256 key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, rsa_pss_rsae_sha256 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
                    key_file=$DATA_FILES_PATH/server2.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha256 key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, rsa_pss_rsae_sha384 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/cert_sha256.crt \
                    key_file=$DATA_FILES_PATH/server1.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha384 key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, rsa_pss_rsae_sha384 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
                    key_file=$DATA_FILES_PATH/server2.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha384 key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, rsa_pss_rsae_sha512 - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/cert_sha256.crt \
                    key_file=$DATA_FILES_PATH/server1.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha512 key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, rsa_pss_rsae_sha512 - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/server2-sha256.crt \
                    key_file=$DATA_FILES_PATH/server2.key sig_algs=ecdsa_secp256r1_sha256,rsa_pss_rsae_sha512 key_opaque=1" \
            0 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "Protocol is TLSv1.3"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, client alg not in server list - openssl" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache -Verify 10
                -sigalgs ecdsa_secp256r1_sha256" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp521r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp521r1.key sig_algs=ecdsa_secp256r1_sha256,ecdsa_secp521r1_sha512 key_opaque=1" \
            1 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "no suitable signature algorithm"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Client authentication - opaque key, client alg not in server list - gnutls" \
            "$G_NEXT_SRV --debug=4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:-SIGN-ALL:+SIGN-ECDSA-SECP256R1-SHA256:%NO_TICKETS" \
            "$P_CLI debug_level=3 crt_file=$DATA_FILES_PATH/ecdsa_secp521r1.crt \
                    key_file=$DATA_FILES_PATH/ecdsa_secp521r1.key sig_algs=ecdsa_secp256r1_sha256,ecdsa_secp521r1_sha512 key_opaque=1" \
            1 \
            -c "got a certificate request" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE" \
            -c "client state: MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY" \
            -c "no suitable signature algorithm"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: HRR check, ciphersuite TLS_AES_128_GCM_SHA256 - openssl" \
            "$O_NEXT_SRV -ciphersuites TLS_AES_128_GCM_SHA256  -sigalgs ecdsa_secp256r1_sha256 -groups P-256 -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache" \
            "$P_CLI debug_level=4" \
            0 \
            -c "received HelloRetryRequest message" \
            -c "<= ssl_tls13_process_server_hello ( HelloRetryRequest )" \
            -c "client state: MBEDTLS_SSL_CLIENT_HELLO" \
            -c "Protocol is TLSv1.3" \
            -c "HTTP/1.0 200 ok"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: HRR check, ciphersuite TLS_AES_256_GCM_SHA384 - openssl" \
            "$O_NEXT_SRV -ciphersuites TLS_AES_256_GCM_SHA384  -sigalgs ecdsa_secp256r1_sha256 -groups P-256 -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache" \
            "$P_CLI debug_level=4" \
            0 \
            -c "received HelloRetryRequest message" \
            -c "<= ssl_tls13_process_server_hello ( HelloRetryRequest )" \
            -c "client state: MBEDTLS_SSL_CLIENT_HELLO" \
            -c "Protocol is TLSv1.3" \
            -c "HTTP/1.0 200 ok"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_ALG_ECDH
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: HRR check, ciphersuite TLS_AES_128_GCM_SHA256 - gnutls" \
            "$G_NEXT_SRV -d 4 --priority=NONE:+GROUP-SECP256R1:+AES-128-GCM:+SHA256:+AEAD:+SIGN-ECDSA-SECP256R1-SHA256:+VERS-TLS1.3:%NO_TICKETS --disable-client-cert" \
            "$P_CLI debug_level=4" \
            0 \
            -c "received HelloRetryRequest message" \
            -c "<= ssl_tls13_process_server_hello ( HelloRetryRequest )" \
            -c "client state: MBEDTLS_SSL_CLIENT_HELLO" \
            -c "Protocol is TLSv1.3" \
            -c "HTTP/1.0 200 OK"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_ALG_ECDH
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: HRR check, ciphersuite TLS_AES_256_GCM_SHA384 - gnutls" \
            "$G_NEXT_SRV -d 4 --priority=NONE:+GROUP-SECP256R1:+AES-256-GCM:+SHA384:+AEAD:+SIGN-ECDSA-SECP256R1-SHA256:+VERS-TLS1.3:%NO_TICKETS --disable-client-cert" \
            "$P_CLI debug_level=4" \
            0 \
            -c "received HelloRetryRequest message" \
            -c "<= ssl_tls13_process_server_hello ( HelloRetryRequest )" \
            -c "client state: MBEDTLS_SSL_CLIENT_HELLO" \
            -c "Protocol is TLSv1.3" \
            -c "HTTP/1.0 200 OK"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - openssl" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$O_NEXT_CLI -msg -debug -tls1_3 -no_middlebox" \
            0 \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "tls13 server state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_FINISHED" \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_FINISHED" \
            -s "tls13 server state: MBEDTLS_SSL_HANDSHAKE_WRAPUP"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - openssl with client authentication" \
            "$P_SRV debug_level=4 auth_mode=required crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$O_NEXT_CLI -msg -debug -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key -tls1_3 -no_middlebox" \
            0 \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -s "tls13 server state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "tls13 server state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_FINISHED" \
            -s "=> write certificate request" \
            -s "=> parse client hello" \
            -s "<= parse client hello"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - gnutls" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "tls13 server state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_FINISHED" \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_FINISHED" \
            -s "tls13 server state: MBEDTLS_SSL_HANDSHAKE_WRAPUP" \
            -c "HTTP/1.0 200 OK"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - gnutls with client authentication" \
            "$P_SRV debug_level=4 auth_mode=required crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$G_NEXT_CLI localhost -d 4 --x509certfile $DATA_FILES_PATH/server5.crt --x509keyfile $DATA_FILES_PATH/server5.key --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -s "tls13 server state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "tls13 server state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_FINISHED" \
            -s "=> write certificate request" \
            -s "=> parse client hello" \
            -s "<= parse client hello"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - mbedtls" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$P_CLI debug_level=4" \
            0 \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -s "tls13 server state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "tls13 server state: MBEDTLS_SSL_CERTIFICATE_VERIFY" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_FINISHED" \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_FINISHED" \
            -s "tls13 server state: MBEDTLS_SSL_HANDSHAKE_WRAPUP" \
            -c "HTTP/1.0 200 OK"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - mbedtls with client authentication" \
            "$P_SRV debug_level=4 auth_mode=required crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key" \
            0 \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "=> write certificate request" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -s "=> parse client hello" \
            -s "<= parse client hello"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - mbedtls with client empty certificate" \
            "$P_SRV debug_level=4 auth_mode=required crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$P_CLI debug_level=4 crt_file=none key_file=none" \
            1 \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "=> write certificate request" \
            -s "SSL - No client certification received from the client, but required by the authentication mode" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -s "=> parse client hello" \
            -s "<= parse client hello"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - mbedtls with optional client authentication" \
            "$P_SRV debug_level=4 auth_mode=optional crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$P_CLI debug_level=4 crt_file=none key_file=none" \
            0 \
            -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
            -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "=> write certificate request" \
            -c "client state: MBEDTLS_SSL_CERTIFICATE_REQUEST" \
            -s "=> parse client hello" \
            -s "<= parse client hello"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled PSA_WANT_ALG_ECDH
run_test "TLS 1.3: server: HRR check - mbedtls" \
         "$P_SRV debug_level=4 groups=secp384r1" \
         "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
         0 \
        -s "tls13 server state: MBEDTLS_SSL_CLIENT_HELLO" \
        -s "tls13 server state: MBEDTLS_SSL_SERVER_HELLO" \
        -s "tls13 server state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
        -s "tls13 server state: MBEDTLS_SSL_HELLO_RETRY_REQUEST" \
        -c "client state: MBEDTLS_SSL_ENCRYPTED_EXTENSIONS" \
        -s "selected_group: secp384r1" \
        -s "=> write hello retry request" \
        -s "<= write hello retry request"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check, no server certificate available" \
            "$P_SRV debug_level=4 crt_file=none key_file=none" \
            "$P_CLI debug_level=4" \
            1 \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CERTIFICATE" \
            -s "No certificate available."

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - openssl with sni" \
            "$P_SRV debug_level=4 auth_mode=required crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0 \
             sni=localhost,$DATA_FILES_PATH/server5.crt,$DATA_FILES_PATH/server5.key,$DATA_FILES_PATH/test-ca_cat12.crt,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$O_NEXT_CLI -msg -debug -servername localhost -CAfile $DATA_FILES_PATH/test-ca_cat12.crt -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key -tls1_3" \
            0 \
            -s "parse ServerName extension" \
            -s "HTTP/1.0 200 OK"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - gnutls with sni" \
            "$P_SRV debug_level=4 auth_mode=required crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0 \
             sni=localhost,$DATA_FILES_PATH/server5.crt,$DATA_FILES_PATH/server5.key,$DATA_FILES_PATH/test-ca_cat12.crt,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$G_NEXT_CLI localhost -d 4 --sni-hostname=localhost --x509certfile $DATA_FILES_PATH/server5.crt --x509keyfile $DATA_FILES_PATH/server5.key --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:%NO_TICKETS -V" \
            0 \
            -s "parse ServerName extension" \
            -s "HTTP/1.0 200 OK"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Server side check - mbedtls with sni" \
            "$P_SRV debug_level=4 auth_mode=required crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0 \
             sni=localhost,$DATA_FILES_PATH/server2.crt,$DATA_FILES_PATH/server2.key,-,-,-,polarssl.example,$DATA_FILES_PATH/server1-nospace.crt,$DATA_FILES_PATH/server1.key,-,-,-" \
            "$P_CLI debug_level=4 server_name=localhost crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key" \
            0 \
            -s "parse ServerName extension" \
            -s "HTTP/1.0 200 OK"

for i in opt-testcases/*.sh
do
    TEST_SUITE_NAME=${i##*/}
    TEST_SUITE_NAME=${TEST_SUITE_NAME%.*}
    . "$i"
done
unset TEST_SUITE_NAME

# Test 1.3 compatibility mode
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->m both peers do not support middlebox compatibility" \
            "$P_SRV debug_level=4 tickets=0" \
            "$P_CLI debug_level=4" \
            0 \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3" \
            -S "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO" \
            -C "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->m both with middlebox compat support" \
            "$P_SRV debug_level=4 tickets=0" \
            "$P_CLI debug_level=4" \
            0 \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->O both peers do not support middlebox compatibility" \
            "$O_NEXT_SRV -msg -tls1_3 -no_middlebox -num_tickets 0 -no_resume_ephemeral -no_cache" \
            "$P_CLI debug_level=4" \
            0 \
            -c "Protocol is TLSv1.3" \
            -C "ChangeCipherSpec invalid in TLS 1.3 without compatibility mode" \
            -C "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->O server with middlebox compat support, not client" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache" \
            "$P_CLI debug_level=4" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->O both with middlebox compat support" \
            "$O_NEXT_SRV -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache" \
            "$P_CLI debug_level=4" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->G both peers do not support middlebox compatibility" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE --disable-client-cert" \
            "$P_CLI debug_level=4" \
            0 \
            -c "Protocol is TLSv1.3" \
            -C "ChangeCipherSpec invalid in TLS 1.3 without compatibility mode" \
            -C "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->G server with middlebox compat support, not client" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS --disable-client-cert" \
            "$P_CLI debug_level=4" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->G both with middlebox compat support" \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS --disable-client-cert" \
            "$P_CLI debug_level=4" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 O->m both peers do not support middlebox compatibility" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$O_NEXT_CLI -msg -debug -no_middlebox" \
            0 \
            -s "Protocol is TLSv1.3" \
            -S "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO" \
            -C "14 03 03 00 01"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 O->m server with middlebox compat support, not client" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$O_NEXT_CLI -msg -debug -no_middlebox" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 O->m both with middlebox compat support" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$O_NEXT_CLI -msg -debug" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO" \
            -c "14 03 03 00 01"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 G->m both peers do not support middlebox compatibility" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$G_NEXT_CLI localhost --priority=NORMAL:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "Protocol is TLSv1.3" \
            -S "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO" \
            -C "SSL 3.3 ChangeCipherSpec packet received"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 G->m server with middlebox compat support, not client" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$G_NEXT_CLI localhost --debug=10 --priority=NORMAL:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO" \
            -c "SSL 3.3 ChangeCipherSpec packet received" \
            -c "discarding change cipher spec in TLS1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 G->m both with middlebox compat support" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key tickets=0" \
            "$G_NEXT_CLI localhost --debug=10 --priority=NORMAL:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO" \
            -c "SSL 3.3 ChangeCipherSpec packet received"

requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->m HRR both peers do not support middlebox compatibility" \
            "$P_SRV debug_level=4 groups=secp384r1 tickets=0" \
            "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
            0 \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_HELLO_RETRY_REQUEST" \
            -S "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST" \
            -C "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_ALG_ECDH
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->m HRR both with middlebox compat support" \
            "$P_SRV debug_level=4 groups=secp384r1 tickets=0" \
            "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
            0 \
            -s "Protocol is TLSv1.3" \
            -c "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_HELLO_RETRY_REQUEST" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->O HRR both peers do not support middlebox compatibility" \
            "$O_NEXT_SRV -msg -tls1_3 -groups P-384 -no_middlebox -num_tickets 0 -no_cache" \
            "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "received HelloRetryRequest message" \
            -C "ChangeCipherSpec invalid in TLS 1.3 without compatibility mode" \
            -C "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->O HRR server with middlebox compat support, not client" \
            "$O_NEXT_SRV -msg -tls1_3 -groups P-384 -num_tickets 0 -no_cache" \
            "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
            0 \
            -c "received HelloRetryRequest message" \
            -c "Protocol is TLSv1.3" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->O HRR both with middlebox compat support" \
            "$O_NEXT_SRV -msg -tls1_3 -groups P-384 -num_tickets 0 -no_resume_ephemeral -no_cache" \
            "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->G HRR both peers do not support middlebox compatibility" \
            "$G_NEXT_SRV --priority=NORMAL:-GROUP-ALL:+GROUP-SECP384R1:-VERS-ALL:+VERS-TLS1.3:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE --disable-client-cert" \
            "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "received HelloRetryRequest message" \
            -C "ChangeCipherSpec invalid in TLS 1.3 without compatibility mode" \
            -C "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->G HRR server with middlebox compat support, not client" \
            "$G_NEXT_SRV --priority=NORMAL:-GROUP-ALL:+GROUP-SECP384R1:-VERS-ALL:+VERS-TLS1.3:%NO_TICKETS --disable-client-cert" \
            "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
            0 \
            -c "received HelloRetryRequest message" \
            -c "Protocol is TLSv1.3" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled PSA_WANT_ALG_ECDH
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 m->G HRR both with middlebox compat support" \
            "$G_NEXT_SRV --priority=NORMAL:-GROUP-ALL:+GROUP-SECP384R1:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS --disable-client-cert" \
            "$P_CLI debug_level=4 groups=secp256r1,secp384r1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Ignore ChangeCipherSpec in TLS 1.3 compatibility mode"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 O->m HRR both peers do not support middlebox compatibility" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key groups=secp384r1 tickets=0" \
            "$O_NEXT_CLI -msg -debug -groups P-256:P-384 -no_middlebox" \
            0 \
            -s "Protocol is TLSv1.3" \
            -S "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST" \
            -C "14 03 03 00 01"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 O->m HRR server with middlebox compat support, not client" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key groups=secp384r1 tickets=0" \
            "$O_NEXT_CLI -msg -debug -groups P-256:P-384 -no_middlebox" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST" \

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 O->m HRR both with middlebox compat support" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key groups=secp384r1 tickets=0" \
            "$O_NEXT_CLI -msg -debug -groups P-256:P-384" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST" \
            -c "14 03 03 00 01"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_disabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 G->m HRR both peers do not support middlebox compatibility" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key groups=secp384r1 tickets=0" \
            "$G_NEXT_CLI localhost --priority=NORMAL:-GROUP-ALL:+GROUP-SECP256R1:+GROUP-SECP384R1:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "Protocol is TLSv1.3" \
            -S "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST" \
            -C "SSL 3.3 ChangeCipherSpec packet received"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled PSA_WANT_ALG_ECDH
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 G->m HRR server with middlebox compat support, not client" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key groups=secp384r1 tickets=0" \
            "$G_NEXT_CLI localhost --debug=10 --priority=NORMAL:-GROUP-ALL:+GROUP-SECP256R1:+GROUP-SECP384R1:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST" \
            -c "SSL 3.3 ChangeCipherSpec packet received" \
            -c "discarding change cipher spec in TLS1.3"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled PSA_WANT_ALG_ECDH
requires_config_enabled MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3 G->m HRR both with middlebox compat support" \
            "$P_SRV debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key groups=secp384r1 tickets=0" \
            "$G_NEXT_CLI localhost --debug=10 --priority=NORMAL:-GROUP-ALL:+GROUP-SECP256R1:+GROUP-SECP384R1:%NO_TICKETS:%DISABLE_TLS13_COMPAT_MODE -V" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "tls13 server state: MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST" \
            -c "SSL 3.3 ChangeCipherSpec packet received"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check signature algorithm order, m->O" \
            "$O_NEXT_SRV_NO_CERT -cert $DATA_FILES_PATH/server2-sha256.crt -key $DATA_FILES_PATH/server2.key
                                 -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache
                                 -Verify 10 -sigalgs rsa_pkcs1_sha512:rsa_pss_rsae_sha512:rsa_pss_rsae_sha384:ecdsa_secp256r1_sha256" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key \
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "CertificateVerify signature with rsa_pss_rsae_sha512" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check signature algorithm order, m->G" \
            "$G_NEXT_SRV_NO_CERT --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key
                    -d 4
                    --priority=NORMAL:-VERS-ALL:-SIGN-ALL:+SIGN-RSA-SHA512:+SIGN-RSA-PSS-RSAE-SHA512:+SIGN-RSA-PSS-RSAE-SHA384:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS " \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key \
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "CertificateVerify signature with rsa_pss_rsae_sha512" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check signature algorithm order, m->m" \
            "$P_SRV debug_level=4 auth_mode=required
                    crt_file2=$DATA_FILES_PATH/server2-sha256.crt key_file2=$DATA_FILES_PATH/server2.key
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256 " \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key \
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "CertificateVerify signature with rsa_pss_rsae_sha512" \
            -s "CertificateVerify signature with rsa_pss_rsae_sha512" \
            -s "ssl_tls13_pick_key_cert:selected signature algorithm rsa_pss_rsae_sha512" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check signature algorithm order, O->m" \
            "$P_SRV debug_level=4 auth_mode=required
                    crt_file2=$DATA_FILES_PATH/server2-sha256.crt key_file2=$DATA_FILES_PATH/server2.key
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256 " \
            "$O_NEXT_CLI_NO_CERT -msg -CAfile $DATA_FILES_PATH/test-ca_cat12.crt \
                                 -cert $DATA_FILES_PATH/server2-sha256.crt -key $DATA_FILES_PATH/server2.key \
                                 -sigalgs rsa_pkcs1_sha512:rsa_pss_rsae_sha512:rsa_pss_rsae_sha384:ecdsa_secp256r1_sha256"  \
            0 \
            -c "TLSv1.3" \
            -s "CertificateVerify signature with rsa_pss_rsae_sha512" \
            -s "ssl_tls13_pick_key_cert:selected signature algorithm rsa_pss_rsae_sha512"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check signature algorithm order, G->m" \
            "$P_SRV debug_level=4 auth_mode=required
                    crt_file2=$DATA_FILES_PATH/server2-sha256.crt key_file2=$DATA_FILES_PATH/server2.key
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256 " \
            "$G_NEXT_CLI_NO_CERT localhost -d 4 --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt \
                                 --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key \
                                 --priority=NORMAL:-SIGN-ALL:+SIGN-RSA-SHA512:+SIGN-RSA-PSS-RSAE-SHA512:+SIGN-RSA-PSS-RSAE-SHA384"  \
            0 \
            -c "Negotiated version: 3.4" \
            -c "HTTP/1.0 200 [Oo][Kk]" \
            -s "CertificateVerify signature with rsa_pss_rsae_sha512" \
            -s "ssl_tls13_pick_key_cert:selected signature algorithm rsa_pss_rsae_sha512"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check server no suitable signature algorithm, G->m" \
            "$P_SRV debug_level=4 auth_mode=required
                    crt_file2=$DATA_FILES_PATH/server2-sha256.crt key_file2=$DATA_FILES_PATH/server2.key
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key
                    sig_algs=rsa_pkcs1_sha512,ecdsa_secp256r1_sha256 " \
            "$G_NEXT_CLI_NO_CERT localhost -d 4 --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt \
                                 --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key \
                                 --priority=NORMAL:-SIGN-ALL:+SIGN-RSA-SHA512:+SIGN-RSA-PSS-RSAE-SHA512:+SIGN-ECDSA-SECP521R1-SHA512"  \
            1 \
            -S "ssl_tls13_pick_key_cert:check signature algorithm"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check server no suitable signature algorithm, O->m" \
            "$P_SRV debug_level=4 auth_mode=required
                    crt_file2=$DATA_FILES_PATH/server2-sha256.crt key_file2=$DATA_FILES_PATH/server2.key
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key
                    sig_algs=rsa_pkcs1_sha512,ecdsa_secp256r1_sha256" \
            "$O_NEXT_CLI_NO_CERT -msg -CAfile $DATA_FILES_PATH/test-ca_cat12.crt \
                                 -cert $DATA_FILES_PATH/server2-sha256.crt -key $DATA_FILES_PATH/server2.key \
                                 -sigalgs rsa_pkcs1_sha512:rsa_pss_rsae_sha512:ecdsa_secp521r1_sha512"  \
            1 \
            -S "ssl_tls13_pick_key_cert:check signature algorithm"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check server no suitable signature algorithm, m->m" \
            "$P_SRV debug_level=4 auth_mode=required
                    crt_file2=$DATA_FILES_PATH/server2-sha256.crt key_file2=$DATA_FILES_PATH/server2.key
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key
                    sig_algs=rsa_pkcs1_sha512,ecdsa_secp256r1_sha256 " \
            "$P_CLI allow_sha1=0 debug_level=4 crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key \
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,ecdsa_secp521r1_sha512" \
            1 \
            -S "ssl_tls13_pick_key_cert:check signature algorithm"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check server no suitable certificate, G->m" \
            "$P_SRV debug_level=4
                    crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256 " \
            "$G_NEXT_CLI_NO_CERT localhost -d 4 --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt \
                                 --priority=NORMAL:-SIGN-ALL:+SIGN-ECDSA-SECP521R1-SHA512:+SIGN-ECDSA-SECP256R1-SHA256"  \
            1 \
            -s "ssl_tls13_pick_key_cert:no suitable certificate found"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check server no suitable certificate, O->m" \
            "$P_SRV debug_level=4
                    crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256 " \
            "$O_NEXT_CLI_NO_CERT -msg -CAfile $DATA_FILES_PATH/test-ca_cat12.crt \
                                 -sigalgs ecdsa_secp521r1_sha512:ecdsa_secp256r1_sha256"  \
            1 \
            -s "ssl_tls13_pick_key_cert:no suitable certificate found"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check server no suitable certificate, m->m" \
            "$P_SRV debug_level=4
                    crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256 " \
            "$P_CLI allow_sha1=0 debug_level=4 \
                    sig_algs=ecdsa_secp521r1_sha512,ecdsa_secp256r1_sha256" \
            1 \
            -s "ssl_tls13_pick_key_cert:no suitable certificate found"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check client no signature algorithm, m->O" \
            "$O_NEXT_SRV_NO_CERT -cert $DATA_FILES_PATH/server2-sha256.crt -key $DATA_FILES_PATH/server2.key
                                 -msg -tls1_3 -num_tickets 0 -no_resume_ephemeral -no_cache
                                 -Verify 10 -sigalgs rsa_pkcs1_sha512:rsa_pss_rsae_sha512:rsa_pss_rsae_sha384:ecdsa_secp521r1_sha512" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256" \
            1 \
            -c "no suitable signature algorithm"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check client no signature algorithm, m->G" \
            "$G_NEXT_SRV_NO_CERT --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key
                    -d 4
                    --priority=NORMAL:-VERS-ALL:-SIGN-ALL:+SIGN-RSA-SHA512:+SIGN-RSA-PSS-RSAE-SHA512:+SIGN-RSA-PSS-RSAE-SHA384:+VERS-TLS1.3:+CIPHER-ALL:%NO_TICKETS " \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256" \
            1 \
            -c "no suitable signature algorithm"

requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: Check client no signature algorithm, m->m" \
            "$P_SRV debug_level=4 auth_mode=required
                    crt_file2=$DATA_FILES_PATH/server2-sha256.crt key_file2=$DATA_FILES_PATH/server2.key
                    crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp521r1_sha512" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server5.crt key_file=$DATA_FILES_PATH/server5.key \
                    sig_algs=rsa_pkcs1_sha512,rsa_pss_rsae_sha512,rsa_pss_rsae_sha384,ecdsa_secp256r1_sha256" \
            1 \
            -c "no suitable signature algorithm"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
run_test    "TLS 1.2: Check rsa_pss_rsae compatibility issue, m->O" \
            "$O_NEXT_SRV_NO_CERT -cert $DATA_FILES_PATH/server2-sha256.crt -key $DATA_FILES_PATH/server2.key
                                 -msg -tls1_2
                                 -Verify 10 " \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key
                    sig_algs=rsa_pss_rsae_sha512,rsa_pkcs1_sha512
                    min_version=tls12 max_version=tls13 " \
            0 \
            -c "Protocol is TLSv1.2" \
            -c "HTTP/1.0 200 [Oo][Kk]"


requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_CLI_C
run_test    "TLS 1.2: Check rsa_pss_rsae compatibility issue, m->G" \
            "$G_NEXT_SRV_NO_CERT --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key
                    -d 4
                    --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2" \
            "$P_CLI debug_level=4 crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key
                    sig_algs=rsa_pss_rsae_sha512,rsa_pkcs1_sha512
                    min_version=tls12 max_version=tls13 " \
            0 \
            -c "Protocol is TLSv1.2" \
            -c "HTTP/1.0 200 [Oo][Kk]"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT
requires_config_enabled PSA_WANT_ALG_FFDH
requires_config_enabled PSA_WANT_DH_RFC7919_3072
requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
run_test "TLS 1.3 G->m: AES_128_GCM_SHA256,ffdhe3072,rsa_pss_rsae_sha256" \
         "$P_SRV crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key debug_level=4 force_ciphersuite=TLS1-3-AES-128-GCM-SHA256 sig_algs=rsa_pss_rsae_sha256 groups=ffdhe3072 tls13_kex_modes=ephemeral cookies=0 tickets=0" \
         "$G_NEXT_CLI_NO_CERT --debug=4 --single-key-share --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt --priority=NONE:+AES-128-GCM:+SHA256:+AEAD:+SIGN-RSA-PSS-RSAE-SHA256:+GROUP-FFDHE3072:+VERS-TLS1.3:%NO_TICKETS" \
         0 \
         -s "Protocol is TLSv1.3" \
         -s "server hello, chosen ciphersuite: TLS1-3-AES-128-GCM-SHA256 ( id=4865 )" \
         -s "received signature algorithm: 0x804" \
         -s "got named group: ffdhe3072(0101)" \
         -s "Certificate verification was skipped" \
         -C "received HelloRetryRequest message"


requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT
requires_config_enabled PSA_WANT_ALG_FFDH
requires_config_enabled PSA_WANT_DH_RFC7919_3072
run_test "TLS 1.3 m->G: AES_128_GCM_SHA256,ffdhe3072,rsa_pss_rsae_sha256" \
         "$G_NEXT_SRV_NO_CERT --http --disable-client-cert --debug=4 --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key --priority=NONE:+AES-128-GCM:+SHA256:+AEAD:+SIGN-RSA-PSS-RSAE-SHA256:+GROUP-FFDHE3072:+VERS-TLS1.3:%NO_TICKETS" \
         "$P_CLI ca_file=$DATA_FILES_PATH/test-ca_cat12.crt debug_level=4 force_ciphersuite=TLS1-3-AES-128-GCM-SHA256 sig_algs=rsa_pss_rsae_sha256 groups=ffdhe3072" \
         0 \
         -c "HTTP/1.0 200 OK" \
         -c "Protocol is TLSv1.3" \
         -c "server hello, chosen ciphersuite: ( 1301 ) - TLS1-3-AES-128-GCM-SHA256" \
         -c "Certificate Verify: Signature algorithm ( 0804 )" \
         -c "NamedGroup: ffdhe3072 ( 101 )" \
         -c "Verifying peer X.509 certificate... ok" \
         -C "received HelloRetryRequest message"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT
requires_config_enabled PSA_WANT_ALG_FFDH
requires_config_enabled PSA_WANT_DH_RFC7919_4096
requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
run_test "TLS 1.3 G->m: AES_128_GCM_SHA256,ffdhe4096,rsa_pss_rsae_sha256" \
         "$P_SRV crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key debug_level=4 force_ciphersuite=TLS1-3-AES-128-GCM-SHA256 sig_algs=rsa_pss_rsae_sha256 groups=ffdhe4096 tls13_kex_modes=ephemeral cookies=0 tickets=0" \
         "$G_NEXT_CLI_NO_CERT --debug=4 --single-key-share --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt --priority=NONE:+AES-128-GCM:+SHA256:+AEAD:+SIGN-RSA-PSS-RSAE-SHA256:+GROUP-FFDHE4096:+VERS-TLS1.3:%NO_TICKETS" \
         0 \
         -s "Protocol is TLSv1.3" \
         -s "server hello, chosen ciphersuite: TLS1-3-AES-128-GCM-SHA256 ( id=4865 )" \
         -s "received signature algorithm: 0x804" \
         -s "got named group: ffdhe4096(0102)" \
         -s "Certificate verification was skipped" \
         -C "received HelloRetryRequest message"


requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT
requires_config_enabled PSA_WANT_ALG_FFDH
requires_config_enabled PSA_WANT_DH_RFC7919_4096
run_test "TLS 1.3 m->G: AES_128_GCM_SHA256,ffdhe4096,rsa_pss_rsae_sha256" \
         "$G_NEXT_SRV_NO_CERT --http --disable-client-cert --debug=4 --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key --priority=NONE:+AES-128-GCM:+SHA256:+AEAD:+SIGN-RSA-PSS-RSAE-SHA256:+GROUP-FFDHE4096:+VERS-TLS1.3:%NO_TICKETS" \
         "$P_CLI ca_file=$DATA_FILES_PATH/test-ca_cat12.crt debug_level=4 force_ciphersuite=TLS1-3-AES-128-GCM-SHA256 sig_algs=rsa_pss_rsae_sha256 groups=ffdhe4096" \
         0 \
         -c "HTTP/1.0 200 OK" \
         -c "Protocol is TLSv1.3" \
         -c "server hello, chosen ciphersuite: ( 1301 ) - TLS1-3-AES-128-GCM-SHA256" \
         -c "Certificate Verify: Signature algorithm ( 0804 )" \
         -c "NamedGroup: ffdhe4096 ( 102 )" \
         -c "Verifying peer X.509 certificate... ok" \
         -C "received HelloRetryRequest message"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT
requires_config_enabled PSA_WANT_ALG_FFDH
requires_config_enabled PSA_WANT_DH_RFC7919_6144
requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
# Tests using FFDH with a large prime take a long time to run with a memory
# sanitizer. GnuTLS <=3.8.1 has a hard-coded timeout and gives up after
# 30s (since 3.8.1, it can be configured with --timeout). We've observed
# 8192-bit FFDH test cases failing intermittently on heavily loaded CI
# executors (https://github.com/Mbed-TLS/mbedtls/issues/9742),
# when using MSan. As a workaround, skip them.
# Also skip 6144-bit FFDH to have a bit of safety margin.
not_with_msan_or_valgrind
run_test "TLS 1.3 G->m: AES_128_GCM_SHA256,ffdhe6144,rsa_pss_rsae_sha256" \
         "$P_SRV crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key debug_level=4 force_ciphersuite=TLS1-3-AES-128-GCM-SHA256 sig_algs=rsa_pss_rsae_sha256 groups=ffdhe6144 tls13_kex_modes=ephemeral cookies=0 tickets=0" \
         "$G_NEXT_CLI_NO_CERT --debug=4 --single-key-share --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt --priority=NONE:+AES-128-GCM:+SHA256:+AEAD:+SIGN-RSA-PSS-RSAE-SHA256:+GROUP-FFDHE6144:+VERS-TLS1.3:%NO_TICKETS" \
         0 \
         -s "Protocol is TLSv1.3" \
         -s "server hello, chosen ciphersuite: TLS1-3-AES-128-GCM-SHA256 ( id=4865 )" \
         -s "received signature algorithm: 0x804" \
         -s "got named group: ffdhe6144(0103)" \
         -s "Certificate verification was skipped" \
         -C "received HelloRetryRequest message"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT
requires_config_enabled PSA_WANT_ALG_FFDH
requires_config_enabled PSA_WANT_DH_RFC7919_6144
not_with_msan_or_valgrind
run_test "TLS 1.3 m->G: AES_128_GCM_SHA256,ffdhe6144,rsa_pss_rsae_sha256" \
         "$G_NEXT_SRV_NO_CERT --http --disable-client-cert --debug=4 --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key --priority=NONE:+AES-128-GCM:+SHA256:+AEAD:+SIGN-RSA-PSS-RSAE-SHA256:+GROUP-FFDHE6144:+VERS-TLS1.3:%NO_TICKETS" \
         "$P_CLI ca_file=$DATA_FILES_PATH/test-ca_cat12.crt debug_level=4 force_ciphersuite=TLS1-3-AES-128-GCM-SHA256 sig_algs=rsa_pss_rsae_sha256 groups=ffdhe6144" \
         0 \
         -c "HTTP/1.0 200 OK" \
         -c "Protocol is TLSv1.3" \
         -c "server hello, chosen ciphersuite: ( 1301 ) - TLS1-3-AES-128-GCM-SHA256" \
         -c "Certificate Verify: Signature algorithm ( 0804 )" \
         -c "NamedGroup: ffdhe6144 ( 103 )" \
         -c "Verifying peer X.509 certificate... ok" \
         -C "received HelloRetryRequest message"

requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT
requires_config_enabled PSA_WANT_ALG_FFDH
requires_config_enabled PSA_WANT_DH_RFC7919_8192
requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
not_with_msan_or_valgrind
client_needs_more_time 4
run_test "TLS 1.3 G->m: AES_128_GCM_SHA256,ffdhe8192,rsa_pss_rsae_sha256" \
         "$P_SRV crt_file=$DATA_FILES_PATH/server2-sha256.crt key_file=$DATA_FILES_PATH/server2.key debug_level=4 force_ciphersuite=TLS1-3-AES-128-GCM-SHA256 sig_algs=rsa_pss_rsae_sha256 groups=ffdhe8192 tls13_kex_modes=ephemeral cookies=0 tickets=0" \
         "$G_NEXT_CLI_NO_CERT --debug=4 --single-key-share --x509cafile $DATA_FILES_PATH/test-ca_cat12.crt --priority=NONE:+AES-128-GCM:+SHA256:+AEAD:+SIGN-RSA-PSS-RSAE-SHA256:+GROUP-FFDHE8192:+VERS-TLS1.3:%NO_TICKETS" \
         0 \
         -s "Protocol is TLSv1.3" \
         -s "server hello, chosen ciphersuite: TLS1-3-AES-128-GCM-SHA256 ( id=4865 )" \
         -s "received signature algorithm: 0x804" \
         -s "got named group: ffdhe8192(0104)" \
         -s "Certificate verification was skipped" \
         -C "received HelloRetryRequest message"

requires_gnutls_tls1_3
requires_gnutls_next_no_ticket
requires_gnutls_next_disable_tls13_compat
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT
requires_config_enabled PSA_WANT_ALG_FFDH
requires_config_enabled PSA_WANT_DH_RFC7919_8192
not_with_msan_or_valgrind
client_needs_more_time 4
run_test "TLS 1.3 m->G: AES_128_GCM_SHA256,ffdhe8192,rsa_pss_rsae_sha256" \
         "$G_NEXT_SRV_NO_CERT --http --disable-client-cert --debug=4 --x509certfile $DATA_FILES_PATH/server2-sha256.crt --x509keyfile $DATA_FILES_PATH/server2.key --priority=NONE:+AES-128-GCM:+SHA256:+AEAD:+SIGN-RSA-PSS-RSAE-SHA256:+GROUP-FFDHE8192:+VERS-TLS1.3:%NO_TICKETS" \
         "$P_CLI ca_file=$DATA_FILES_PATH/test-ca_cat12.crt debug_level=4 force_ciphersuite=TLS1-3-AES-128-GCM-SHA256 sig_algs=rsa_pss_rsae_sha256 groups=ffdhe8192" \
         0 \
         -c "HTTP/1.0 200 OK" \
         -c "Protocol is TLSv1.3" \
         -c "server hello, chosen ciphersuite: ( 1301 ) - TLS1-3-AES-128-GCM-SHA256" \
         -c "Certificate Verify: Signature algorithm ( 0804 )" \
         -c "NamedGroup: ffdhe8192 ( 104 )" \
         -c "Verifying peer X.509 certificate... ok" \
         -C "received HelloRetryRequest message"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: no HRR in case of PSK key exchange mode" \
            "$P_SRV nbio=2 psk=73776f726466697368 psk_identity=0a0b0c tls13_kex_modes=psk groups=none" \
            "$P_CLI nbio=2 debug_level=3 psk=73776f726466697368 psk_identity=0a0b0c tls13_kex_modes=all" \
            0 \
            -C "received HelloRetryRequest message" \
            -c "Selected key exchange mode: psk$" \
            -c "HTTP/1.0 200 OK"

# Legacy_compression_methods testing

requires_gnutls
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
run_test    "TLS 1.2 ClientHello indicating support for deflate compression method" \
            "$P_SRV debug_level=3" \
            "$G_CLI  --priority=NORMAL:-VERS-ALL:+VERS-TLS1.2:+COMP-DEFLATE localhost" \
            0 \
            -c "Handshake was completed" \
            -s "dumping .client hello, compression. (2 bytes)"

# Handshake defragmentation testing

# Most test cases are in opt-testcases/handshake-generated.sh

# Test server-side buffer resizing with fragmented handshake on TLS1.2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_config_enabled MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
requires_max_content_len 1025
run_test    "Handshake defragmentation on server: len=256, buffer resizing with MFL=1024" \
            "$P_SRV debug_level=4 auth_mode=required" \
            "$O_NEXT_CLI -tls1_2 -split_send_frag 256 -maxfraglen 1024 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            0 \
            -s "Reallocating in_buf" \
            -s "Reallocating out_buf" \
            -s "reassembled record" \
            -s "initial handshake fragment: 256, 0\\.\\.256 of [0-9]\\+" \
            -s "Prepare: waiting for more handshake fragments 256/" \
            -s "Consume: waiting for more handshake fragments 256/"

# Test client-initiated renegotiation with fragmented handshake on TLS1.2
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on server: len=512, client-initiated renegotiation" \
            "$P_SRV debug_level=4 exchanges=2 renegotiation=1 auth_mode=required" \
            "$O_NEXT_CLI_RENEGOTIATE -tls1_2 -split_send_frag 512 -connect 127.0.0.1:+$SRV_PORT" \
            0 \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -s "=> renegotiate" \
            -S "write hello request" \
            -s "reassembled record" \
            -s "initial handshake fragment: 512, 0\\.\\.512 of [0-9]\\+" \
            -s "Prepare: waiting for more handshake fragments 512/" \
            -s "Consume: waiting for more handshake fragments 512/" \

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on server: len=256, client-initiated renegotiation" \
            "$P_SRV debug_level=4 exchanges=2 renegotiation=1 auth_mode=required" \
            "$O_NEXT_CLI_RENEGOTIATE -tls1_2 -split_send_frag 256 -connect 127.0.0.1:+$SRV_PORT" \
            0 \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -s "=> renegotiate" \
            -S "write hello request" \
            -s "reassembled record" \
            -s "initial handshake fragment: 256, 0\\.\\.256 of [0-9]\\+" \
            -s "Prepare: waiting for more handshake fragments 256/" \
            -s "Consume: waiting for more handshake fragments 256/" \

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on server: len=128, client-initiated renegotiation" \
            "$P_SRV debug_level=4 exchanges=2 renegotiation=1 auth_mode=required" \
            "$O_NEXT_CLI_RENEGOTIATE -tls1_2 -split_send_frag 128 -connect 127.0.0.1:+$SRV_PORT" \
            0 \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -s "=> renegotiate" \
            -S "write hello request" \
            -s "reassembled record" \
            -s "initial handshake fragment: 128, 0\\.\\.128 of [0-9]\\+" \
            -s "Prepare: waiting for more handshake fragments 128/" \
            -s "Consume: waiting for more handshake fragments 128/" \

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on server: len=4, client-initiated renegotiation" \
            "$P_SRV debug_level=4 exchanges=2 renegotiation=1 auth_mode=required" \
            "$O_NEXT_CLI_RENEGOTIATE -tls1_2 -split_send_frag 4 -connect 127.0.0.1:+$SRV_PORT" \
            0 \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "found renegotiation extension" \
            -s "server hello, secure renegotiation extension" \
            -s "=> renegotiate" \
            -S "write hello request" \
            -s "reassembled record" \
            -s "initial handshake fragment: 4, 0\\.\\.4 of [0-9]\\+" \
            -s "Prepare: waiting for more handshake fragments 4/" \
            -s "Consume: waiting for more handshake fragments 4/" \

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on server: len=4, client-initiated server-rejected renegotiation" \
            "$P_SRV debug_level=4 exchanges=2 renegotiation=0 auth_mode=required" \
            "$O_NEXT_CLI_RENEGOTIATE -tls1_2 -split_send_frag 4 -connect 127.0.0.1:+$SRV_PORT" \
            1 \
            -s "received TLS_EMPTY_RENEGOTIATION_INFO" \
            -s "refusing renegotiation, sending alert" \
            -s "server hello, secure renegotiation extension" \
            -s "initial handshake fragment: 4, 0\\.\\.4 of [0-9]\\+" \
            -s "Prepare: waiting for more handshake fragments 4/" \
            -s "Consume: waiting for more handshake fragments 4/" \

# Test server-initiated renegotiation with fragmented handshake on TLS1.2

# Note: The /reneg endpoint serves as a directive for OpenSSL's s_server
# to initiate a handshake renegotiation.
# Note: Adjusting the renegotiation delay beyond the library's default
# value of 16 is necessary. This parameter defines the maximum
# number of records received before renegotiation is completed.
# By fragmenting records and thereby increasing their quantity,
# the default threshold can be reached more quickly.
# Setting it to -1 disables that policy's enforment.
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on client: len=512, server-initiated renegotiation" \
            "$O_NEXT_SRV -tls1_2 -split_send_frag 512 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            "$P_CLI debug_level=3 renegotiation=1 request_page=/reneg" \
            0 \
            -c "initial handshake fragment: 512, 0\\.\\.512 of [0-9]\\+" \
            -c "Prepare: waiting for more handshake fragments 512/" \
            -c "Consume: waiting for more handshake fragments 512/" \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on client: len=256, server-initiated renegotiation" \
            "$O_NEXT_SRV -tls1_2 -split_send_frag 256 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            "$P_CLI debug_level=3 renegotiation=1 renego_delay=-1 request_page=/reneg" \
            0 \
            -c "initial handshake fragment: 256, 0\\.\\.256 of [0-9]\\+" \
            -c "Prepare: waiting for more handshake fragments 256/" \
            -c "Consume: waiting for more handshake fragments 256/" \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on client: len=128, server-initiated renegotiation" \
            "$O_NEXT_SRV -tls1_2 -split_send_frag 128 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            "$P_CLI debug_level=3 renegotiation=1 renego_delay=-1 request_page=/reneg" \
            0 \
            -c "initial handshake fragment: 128, 0\\.\\.128 of [0-9]\\+" \
            -c "Prepare: waiting for more handshake fragments 128/" \
            -c "Consume: waiting for more handshake fragments 128/" \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_SSL_RENEGOTIATION
run_test    "Handshake defragmentation on client: len=4, server-initiated renegotiation" \
            "$O_NEXT_SRV -tls1_2 -split_send_frag 4 -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key" \
            "$P_CLI debug_level=3 renegotiation=1 renego_delay=-1 request_page=/reneg" \
            0 \
            -c "initial handshake fragment: 4, 0\\.\\.4 of [0-9]\\+" \
            -c "Prepare: waiting for more handshake fragments 4/" \
            -c "Consume: waiting for more handshake fragments 4/" \
            -c "client hello, adding renegotiation extension" \
            -c "found renegotiation extension" \
            -c "=> renegotiate"

# Test heap memory usage after handshake
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_2
requires_config_enabled MBEDTLS_MEMORY_DEBUG
requires_config_enabled MBEDTLS_MEMORY_BUFFER_ALLOC_C
requires_config_enabled MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
requires_max_content_len 16384
run_tests_memory_after_handshake

if [ "$LIST_TESTS" -eq 0 ]; then

    # Final report

    echo "------------------------------------------------------------------------"

    if [ $FAILS = 0 ]; then
        printf "PASSED"
    else
        printf "FAILED"
    fi
    PASSES=$(( $TESTS - $FAILS ))
    echo " ($PASSES / $TESTS tests ($SKIPS skipped))"

    if [ $((TESTS - SKIPS)) -lt $MIN_TESTS ]; then
        cat <<EOF
Error: Expected to run at least $MIN_TESTS, but only ran $((TESTS - SKIPS)).
Maybe a bad filter ('$FILTER') or a bad configuration?
EOF
        if [ $FAILS -eq 0 ]; then
            FAILS=1
        fi
    fi
fi

if [ $FAILS -gt 255 ]; then
    # Clamp at 255 as caller gets exit code & 0xFF
    # (so 256 would be 0, or success, etc)
    FAILS=255
fi
exit $FAILS
