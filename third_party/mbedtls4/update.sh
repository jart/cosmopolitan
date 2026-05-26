#!/bin/sh
# update.sh — download and unpack a specific version of mbedTLS into this
# directory, replacing all upstream source files while preserving
# Cosmopolitan-specific files (BUILD.mk, README.txt, update.sh, clean.sh).
#
# Usage:
#   ./third_party/mbedtls4/update.sh 4.1.0
#
# The script:
#   1. Runs clean.sh to remove the old upstream files (preserving Cosmo-owned
#      files: BUILD.mk, update.sh, clean.sh, mbedtls_config.h, platform.h,
#      private_access.h).
#   2. Downloads the mbedTLS release tarball (which includes tf-psa-crypto
#      and framework inline — no separate submodule download needed).
#   3. Unpacks the tarball into place.
#   4. Writes include/mbedtls/mbedtls_config.h from the embedded heredoc.
#   5. Regenerates generated/ via build/mkmbedtls4wrappers.py.
#      This must happen before make, since make depend scans includes
#      before any build rules execute.
#
# Requirements: curl, tar, python3

set -e

if [ $# -gt 0 ]; then
    # Only accept versions higher than 4 and strictly lower than 5
    if ! echo "$1" | grep -E '^4\.[0-9]+\.[0-9]+$' >/dev/null; then
        echo "Error: Invalid version format. Please provide a version in the format '4.x.y'." >&2
        exit 1
    fi
    export MBEDTLS_VERSION="${1:-}"
else
    export MBEDTLS_VERSION=4.1.0
fi
MBEDTLS_TAG="mbedtls-${MBEDTLS_VERSION}"


echo "==> Removing old upstream source ..."
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
"${SCRIPT_DIR}/clean.sh"

TMPDIR="$(mktemp -d)"
trap 'rm -rf "${TMPDIR}"' EXIT
MBEDTLS_TAR="${TMPDIR}/mbedtls.tar.bz2"

echo "==> Downloading mbedTLS ${MBEDTLS_TAG} ..."
curl -fL -# \
    "https://github.com/Mbed-TLS/mbedtls/releases/download/${MBEDTLS_TAG}/${MBEDTLS_TAG}.tar.bz2" \
    -o "${MBEDTLS_TAR}"

echo "==> Unpacking mbedTLS ..."
tar -xjf "${MBEDTLS_TAR}" -C "${SCRIPT_DIR}" --strip-components=1 \
    --exclude='*/.github' \
    --exclude='*/.gitmodules' \
    --exclude='*/.gitignore' \
    --exclude='*/.gitattributes'

echo "==> Writing include/mbedtls/mbedtls_config.h ..."
mkdir -p "${SCRIPT_DIR}/include/mbedtls"
mv "${SCRIPT_DIR}/include/mbedtls/mbedtls_config.h" "${SCRIPT_DIR}/include/mbedtls/mbedtls_config.h.upstream"
cat > "${SCRIPT_DIR}/include/mbedtls/mbedtls_config.h" << 'MBEDTLS_CONFIG_END'
#define MBEDTLS_NET_C
#define MBEDTLS_TIMING_C
#define MBEDTLS_ERROR_C
#undef MBEDTLS_ERROR_STRERROR_DUMMY
#undef MBEDTLS_VERSION_FEATURES
#define MBEDTLS_VERSION_C
#define MBEDTLS_DEBUG_C
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

#undef MBEDTLS_SSL_COOKIE_C
#undef MBEDTLS_SSL_PROTO_DTLS
#undef MBEDTLS_SSL_DTLS_ANTI_REPLAY
#undef MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#undef MBEDTLS_SSL_DTLS_CONNECTION_ID
#undef MBEDTLS_SSL_DTLS_HELLO_VERIFY
#undef MBEDTLS_SSL_DTLS_SRTP

#undef MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_ALPN
#define MBEDTLS_SSL_CACHE_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_CONTEXT_SERIALIZATION
#define MBEDTLS_SSL_EARLY_DATA
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
#define MBEDTLS_SSL_RENEGOTIATION
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_SESSION_TICKETS
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TICKET_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_PROTO_TLS1_2

#define MBEDTLS_SSL_PROTO_TLS1_3
#define MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED

#define MBEDTLS_PKCS7_C
#define MBEDTLS_X509_CREATE_C
#define MBEDTLS_X509_CRL_PARSE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRT_WRITE_C
#define MBEDTLS_X509_CSR_PARSE_C
#define MBEDTLS_X509_CSR_WRITE_C
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT
#define MBEDTLS_X509_USE_C
MBEDTLS_CONFIG_END

echo "==> Regenerating header wrappers ..."
python3 "${REPO_ROOT}/build/mkmbedtls4wrappers.py" --generate

echo ""
echo "==> mbedTLS ${MBEDTLS_VERSION} installed in ${SCRIPT_DIR}"
echo ""
echo "Next steps:"
echo "  1. Review include/mbedtls/mbedtls_config.h against include/mbedtls/mbedtls_config.h.upstream"
echo "     config to pick up any new options."
echo "  2. Run a full clean build:"
echo "       cd \${REPO_ROOT} && make clean && make -j\$(nproc)"
