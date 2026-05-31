#!/usr/bin/env python3

"""Analyze the test outcomes from a full CI run.

This script can also run on outcomes from a partial run, but the results are
less likely to be useful.
"""

import re
import typing

import scripts_path # pylint: disable=unused-import
from mbedtls_framework import outcome_analysis


class CoverageTask(outcome_analysis.CoverageTask):
    """Justify test cases that are never executed."""

    @staticmethod
    def _has_word_re(words: typing.Iterable[str],
                     exclude: typing.Optional[str] = None) -> typing.Pattern:
        """Construct a regex that matches if any of the words appears.

        The occurrence must start and end at a word boundary.

        If exclude is specified, strings containing a match for that
        regular expression will not match the returned pattern.
        """
        exclude_clause = r''
        if exclude:
            exclude_clause = r'(?!.*' + exclude + ')'
        return re.compile(exclude_clause +
                          r'.*\b(?:' + r'|'.join(words) + r')\b.*',
                          re.DOTALL)

    IGNORED_TESTS = {
        'ssl-opt': [
            # We don't run ssl-opt.sh with Valgrind on the CI because
            # it's extremely slow. We don't intend to change this.
            'DTLS client reconnect from same port: reconnect, nbio, valgrind',
            # We don't have IPv6 in our CI environment.
            # https://github.com/Mbed-TLS/mbedtls-test/issues/176
            'DTLS cookie: enabled, IPv6',
            # Disabled due to OpenSSL bug.
            # https://github.com/openssl/openssl/issues/18887
            'DTLS fragmenting: 3d, MTU=512, openssl client, DTLS 1.2',
            # We don't run ssl-opt.sh with Valgrind on the CI because
            # it's extremely slow. We don't intend to change this.
            'DTLS fragmenting: proxy MTU: auto-reduction (with valgrind)',
            # TLS doesn't use restartable ECDH yet.
            # https://github.com/Mbed-TLS/mbedtls/issues/7294
            re.compile(r'EC restart:.*no USE_PSA.*'),
            # The following test fails intermittently on the CI with a frequency
            # that significantly impacts CI throughput. They are thus disabled
            # for the time being. See
            # https://github.com/Mbed-TLS/mbedtls/issues/10652 for more
            # information.
            'DTLS proxy: 3d, openssl client, fragmentation',
            'DTLS proxy: 3d, openssl client, fragmentation, nbio',
            'DTLS proxy: 3d, gnutls client, fragmentation',
            'DTLS proxy: 3d, gnutls client, fragmentation, nbio=2',
        ],
        'test_suite_config.mbedtls_boolean': [
            # Missing coverage of test configurations.
            # https://github.com/Mbed-TLS/mbedtls/issues/9585
            'Config: !MBEDTLS_SSL_DTLS_ANTI_REPLAY',
            # Missing coverage of test configurations.
            # https://github.com/Mbed-TLS/mbedtls/issues/9585
            'Config: !MBEDTLS_SSL_DTLS_HELLO_VERIFY',
            # We don't run test_suite_config when we test this.
            # https://github.com/Mbed-TLS/mbedtls/issues/9586
            'Config: !MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED',
        ],
        'test_suite_config.crypto_combinations': [
            # New thing in crypto. Not intended to be tested separately
            # in mbedtls.
            # https://github.com/Mbed-TLS/mbedtls/issues/10300
            'Config: entropy: NV seed only',
        ],
        'test_suite_config.psa_boolean': [
            # We don't test with HMAC disabled.
            # https://github.com/Mbed-TLS/mbedtls/issues/9591
            'Config: !PSA_WANT_ALG_HMAC',
            # The DERIVE key type is always enabled.
            'Config: !PSA_WANT_KEY_TYPE_DERIVE',
            # More granularity of key pair type enablement macros
            # than we care to test.
            # https://github.com/Mbed-TLS/mbedtls/issues/9590
            'Config: !PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT',
            'Config: !PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE',
            'Config: !PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT',
            # More granularity of key pair type enablement macros
            # than we care to test.
            # https://github.com/Mbed-TLS/mbedtls/issues/9590
            'Config: !PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT',
            'Config: !PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT',
            # We don't test with HMAC disabled.
            # https://github.com/Mbed-TLS/mbedtls/issues/9591
            'Config: !PSA_WANT_KEY_TYPE_HMAC',
            # The PASSWORD key type is always enabled.
            'Config: !PSA_WANT_KEY_TYPE_PASSWORD',
            # The PASSWORD_HASH key type is always enabled.
            'Config: !PSA_WANT_KEY_TYPE_PASSWORD_HASH',
            # The RAW_DATA key type is always enabled.
            'Config: !PSA_WANT_KEY_TYPE_RAW_DATA',
            # More granularity of key pair type enablement macros
            # than we care to test.
            # https://github.com/Mbed-TLS/mbedtls/issues/9590
            'Config: !PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT',
            'Config: !PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT',
            # Algorithm declared but not supported.
            'Config: PSA_WANT_ALG_CBC_MAC',
            # Algorithm declared but not supported.
            'Config: PSA_WANT_ALG_XTS',
            # More granularity of key pair type enablement macros
            # than we care to test.
            # https://github.com/Mbed-TLS/mbedtls/issues/9590
            'Config: PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE',
            'Config: PSA_WANT_KEY_TYPE_ECC_KEY_PAIR',
            'Config: PSA_WANT_KEY_TYPE_RSA_KEY_PAIR',
            'Config: PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE',
            # https://github.com/Mbed-TLS/mbedtls/issues/9583
            'Config: !MBEDTLS_ECP_NIST_OPTIM',
            # We never test without the PSA client code. Should we?
            # https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/112
            'Config: !MBEDTLS_PSA_CRYPTO_CLIENT',
                        # We only test multithreading with pthreads.
            # https://github.com/Mbed-TLS/mbedtls/issues/9584
            'Config: !MBEDTLS_THREADING_PTHREAD',
            # Built but not tested.
            # https://github.com/Mbed-TLS/mbedtls/issues/9587
            'Config: MBEDTLS_AES_USE_HARDWARE_ONLY',
            # Untested platform-specific optimizations.
            # https://github.com/Mbed-TLS/mbedtls/issues/9588
            'Config: MBEDTLS_HAVE_SSE2',
            # Untested aspect of the platform interface.
            # https://github.com/Mbed-TLS/mbedtls/issues/9589
            'Config: MBEDTLS_PLATFORM_NO_STD_FUNCTIONS',
            # In a client-server build, test_suite_config runs in the
            # client configuration, so it will never report
            # MBEDTLS_PSA_CRYPTO_SPM as enabled. That's ok.
            'Config: MBEDTLS_PSA_CRYPTO_SPM',
            # We don't test on armv8 yet.
            'Config: MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY',
            'Config: MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY',
            # We don't run test_suite_config when we test this.
            # https://github.com/Mbed-TLS/mbedtls/issues/9586
            'Config: MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND',
        ],
        'test_suite_config.psa_combinations': [
            # We don't test this unusual, but sensible configuration.
            # https://github.com/Mbed-TLS/mbedtls/issues/9592
            'Config: PSA_WANT_ALG_DETERMINSTIC_ECDSA without PSA_WANT_ALG_ECDSA',
        ],
        'test_suite_pkcs12': [
            # We never test with CBC/PKCS5/PKCS12 enabled but
            # PKCS7 padding disabled.
            # https://github.com/Mbed-TLS/mbedtls/issues/9580
            'PBE Decrypt, (Invalid padding & PKCS7 padding disabled)',
            'PBE Encrypt, pad = 8 (PKCS7 padding disabled)',
        ],
        'test_suite_pkcs5': [
            # We never test with CBC/PKCS5/PKCS12 enabled but
            # PKCS7 padding disabled.
            # https://github.com/Mbed-TLS/mbedtls/issues/9580
            'PBES2 Decrypt (Invalid padding & PKCS7 padding disabled)',
            'PBES2 Encrypt, pad=6 (PKCS7 padding disabled)',
            'PBES2 Encrypt, pad=8 (PKCS7 padding disabled)',
        ],
        'test_suite_psa_crypto': [
            # We don't test this unusual, but sensible configuration.
            # https://github.com/Mbed-TLS/mbedtls/issues/9592
            re.compile(r'.*ECDSA.*only deterministic supported'),
        ],
        'test_suite_psa_crypto_metadata': [
            # Algorithms declared but not supported.
            # https://github.com/Mbed-TLS/mbedtls/issues/9579
            'Asymmetric signature: Ed25519ph',
            'Asymmetric signature: Ed448ph',
            'Asymmetric signature: pure EdDSA',
            'Cipher: XTS',
            'MAC: CBC_MAC-3DES',
            'MAC: CBC_MAC-AES-128',
            'MAC: CBC_MAC-AES-192',
            'MAC: CBC_MAC-AES-256',
        ],
        'test_suite_psa_crypto_not_supported.generated': [
            # We never test with DH key support disabled but support
            # for a DH group enabled. The dependencies of these test
            # cases don't really make sense.
            # https://github.com/Mbed-TLS/mbedtls/issues/9574
            re.compile(r'PSA \w+ DH_.*type not supported'),
            # We only test partial support for DH with the 2048-bit group
            # enabled and the other groups disabled.
            # https://github.com/Mbed-TLS/mbedtls/issues/9575
            'PSA generate DH_KEY_PAIR(RFC7919) 2048-bit group not supported',
            'PSA import DH_KEY_PAIR(RFC7919) 2048-bit group not supported',
            'PSA import DH_PUBLIC_KEY(RFC7919) 2048-bit group not supported',
        ],
        'test_suite_psa_crypto_op_fail.generated': [
            # We don't test this unusual, but sensible configuration.
            # https://github.com/Mbed-TLS/mbedtls/issues/9592
            re.compile(r'.*: !ECDSA but DETERMINISTIC_ECDSA with ECC_.*'),
            # We never test with the HMAC algorithm enabled but the HMAC
            # key type disabled. Those dependencies don't really make sense.
            # https://github.com/Mbed-TLS/mbedtls/issues/9573
            re.compile(r'.* !HMAC with HMAC'),
            # We don't test with ECDH disabled but the key type enabled.
            # https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/161
            re.compile(r'PSA key_agreement.* !ECDH with ECC_KEY_PAIR\(.*'),
            # We don't test with FFDH disabled but the key type enabled.
            # https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/160
            re.compile(r'PSA key_agreement.* !FFDH with DH_KEY_PAIR\(.*'),
        ],
        'test_suite_psa_crypto_op_fail.misc': [
            # We don't test this unusual, but sensible configuration.
            # https://github.com/Mbed-TLS/mbedtls/issues/9592
            'PSA sign DETERMINISTIC_ECDSA(SHA_256): !ECDSA but DETERMINISTIC_ECDSA with ECC_KEY_PAIR(SECP_R1)', #pylint: disable=line-too-long
        ],
        'tls13-misc': [
            # Disabled due to OpenSSL bug.
            # https://github.com/openssl/openssl/issues/10714
            'TLS 1.3 O->m: resumption',
            # Disabled due to OpenSSL command line limitation.
            # https://github.com/Mbed-TLS/mbedtls/issues/9582
            'TLS 1.3 m->O: resumption with early data',
        ],
    }

# List of tasks with a function that can handle this task and additional arguments if required
KNOWN_TASKS: typing.Dict[str, typing.Type[outcome_analysis.Task]] = {
    'analyze_coverage': CoverageTask,
}

if __name__ == '__main__':
    outcome_analysis.main(KNOWN_TASKS)
