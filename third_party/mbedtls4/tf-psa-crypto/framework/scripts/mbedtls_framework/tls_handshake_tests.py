"""
Generate miscellaneous TLS test cases relating to the handshake.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import os
import sys
from typing import Optional

from mbedtls_framework import tls_test_case
from mbedtls_framework import typing_util
from mbedtls_framework.tls_test_case import Side, Version
import translate_ciphers


# Assume that a TLS 1.2 ClientHello used in these tests will be at most
# this many bytes long.
TLS12_CLIENT_HELLO_ASSUMED_MAX_LENGTH = 255

# Minimum handshake fragment length that Mbed TLS supports.
TLS_HANDSHAKE_FRAGMENT_MIN_LENGTH = 4

def write_tls_handshake_defragmentation_test(
        #pylint: disable=too-many-arguments
        out: typing_util.Writable,
        side: Side,
        length: Optional[int],
        version: Optional[Version] = None,
        cipher: Optional[str] = None,
        etm: Optional[bool] = None, #encrypt-then-mac (only relevant for CBC)
        tls12_client_hello_defragmentation: Optional[bool] = True,
        variant: str = ''
) -> None:
    """Generate one TLS handshake defragmentation test.

    :param out: file to write to.
    :param side: which side is Mbed TLS.
    :param length: fragment length, or None to not fragment.
    :param version: protocol version, if forced.
    """
    #pylint: disable=chained-comparison,too-many-branches,too-many-statements

    our_args = ''
    their_args = ''

    if length is None:
        description = 'no fragmentation, for reference'
    else:
        description = 'len=' + str(length)
    if version is not None:
        description += ', TLS 1.' + str(version.value)
    description = f'Handshake defragmentation on {side.name.lower()}: {description}'
    tc = tls_test_case.TestCase(description)

    if version is not None:
        their_args += ' ' + version.openssl_option()
        # Emit a version requirement, because we're forcing the version via
        # OpenSSL, not via Mbed TLS, and the automatic depdendencies in
        # ssl-opt.sh only handle forcing the version via Mbed TLS.
        tc.requirements.append(version.requires_command())
        if side == Side.SERVER and version == Version.TLS12 and \
           length is not None and \
           length <= TLS12_CLIENT_HELLO_ASSUMED_MAX_LENGTH and \
           not tls12_client_hello_defragmentation:
            # If Server-side ClientHello defragmentation is only supported in
            # the TLS 1.3 message parser, not in the TLS 1.2 message parser,
            # a TLS 1.2 fragmented ClientHello is handled properly only if it
            # is first reassembled by the TLS 1.3 parser before to be passed to
            # the TLS 1.2 ClientHello parser in a TLS 1.3 or TLS 1.2 version
            # negotiation scenario.
            # When TLS 1.3 support is disabled in the server (at compile-time
            # or at runtime), the TLS 1.2 ClientHello parser only sees
            # the first fragment of a fragmented ClientHello.
            tc.requirements.append('requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3')
            tc.description += ' with 1.3 support'

    # To guarantee that the handhake messages are large enough and need to be
    # split into fragments, the tests require certificate authentication.
    # The party in control of the fragmentation operations is OpenSSL and
    # will always use server5.crt (548 Bytes).
    if length is not None and \
       length >= TLS_HANDSHAKE_FRAGMENT_MIN_LENGTH:
        tc.requirements.append('requires_certificate_authentication')
        if version == Version.TLS12 and side == Side.CLIENT:
            #The server uses an ECDSA cert, so make sure we have a compatible key exchange
            tc.requirements.append(
                'requires_config_enabled MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED')
    else:
        # This test case may run in a pure-PSK configuration. OpenSSL doesn't
        # allow this by default with TLS 1.3.
        their_args += ' -allow_no_dhe_kex'

    if length is None:
        forbidden_patterns = [
            'waiting for more fragments',
        ]
        wanted_patterns = []
    elif length < TLS_HANDSHAKE_FRAGMENT_MIN_LENGTH:
        their_args += ' -split_send_frag ' + str(length)
        tc.exit_code = 1
        forbidden_patterns = []
        wanted_patterns = [
            'handshake message too short: ' + str(length),
            'SSL - An invalid SSL record was received',
        ]
        if side == Side.SERVER:
            wanted_patterns[0:0] = ['=> parse client hello']
        elif version == Version.TLS13:
            wanted_patterns[0:0] = ['=> ssl_tls13_process_server_hello']
    else:
        their_args += ' -split_send_frag ' + str(length)
        forbidden_patterns = []
        wanted_patterns = [
            'reassembled record',
            fr'initial handshake fragment: {length}, 0\.\.{length} of [0-9]\+',
            fr'subsequent handshake fragment: [0-9]\+, {length}\.\.',
            fr'Prepare: waiting for more handshake fragments {length}/',
            fr'Consume: waiting for more handshake fragments {length}/',
        ]

    if cipher is not None:
        mbedtls_cipher = translate_ciphers.translate_mbedtls(cipher)
        if side == Side.CLIENT:
            our_args += ' force_ciphersuite=' + mbedtls_cipher
            if 'NULL' in cipher:
                their_args += ' -cipher ALL@SECLEVEL=0:COMPLEMENTOFALL@SECLEVEL=0'
        else:
            # For TLS 1.2, when Mbed TLS is the server, we must force the
            # cipher suite on the client side, because passing
            # force_ciphersuite to ssl_server2 would force a TLS-1.2-only
            # server, which does not support a fragmented ClientHello.
            tc.requirements.append('requires_ciphersuite_enabled ' + mbedtls_cipher)
            their_args += ' -cipher ' + translate_ciphers.translate_ossl(cipher)
            if 'NULL' in cipher:
                their_args += '@SECLEVEL=0'

    if etm is not None:
        if etm:
            tc.requirements.append('requires_config_enabled MBEDTLS_SSL_ENCRYPT_THEN_MAC')
        our_args += ' etm=' + str(int(etm))
        (wanted_patterns if etm else forbidden_patterns)[0:0] = [
            'using encrypt then mac',
        ]

    tc.description += variant

    if side == Side.CLIENT:
        tc.client = '$P_CLI debug_level=4' + our_args
        tc.server = '$O_NEXT_SRV' + their_args
        tc.wanted_client_patterns = wanted_patterns
        tc.forbidden_client_patterns = forbidden_patterns
    else:
        their_args += ' -cert $DATA_FILES_PATH/server5.crt -key $DATA_FILES_PATH/server5.key'
        our_args += ' auth_mode=required'
        tc.client = '$O_NEXT_CLI' + their_args
        tc.server = '$P_SRV debug_level=4' + our_args
        tc.wanted_server_patterns = wanted_patterns
        tc.forbidden_server_patterns = forbidden_patterns
    tc.write(out)


CIPHERS_FOR_TLS12_HANDSHAKE_DEFRAGMENTATION = [
    (None, 'default', None),
    ('TLS_ECDHE_ECDSA_WITH_NULL_SHA', 'null', None),
    ('TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256', 'ChachaPoly', None),
    ('TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256', 'GCM', None),
    ('TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256', 'CBC, etm=n', False),
    ('TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256', 'CBC, etm=y', True),
]

def write_tls_handshake_defragmentation_tests(args, out: typing_util.Writable) -> None:
    """Generate TLS handshake defragmentation tests."""
    for side in Side.CLIENT, Side.SERVER:
        write_tls_handshake_defragmentation_test(out, side, None)
        for length in [512, 513, 256, 128, 64, 36, 32, 16, 13, 5, 4, 3]:
            write_tls_handshake_defragmentation_test(out, side, length,
                                                     Version.TLS13)
            if length == 4:
                for (cipher_suite, nickname, etm) in \
                        CIPHERS_FOR_TLS12_HANDSHAKE_DEFRAGMENTATION:
                    write_tls_handshake_defragmentation_test(
                        out, side, length, Version.TLS12,
                        cipher=cipher_suite, etm=etm,
                        variant=', '+nickname,
                        tls12_client_hello_defragmentation= \
                            args.tls12_client_hello_defragmentation)
            else:
                write_tls_handshake_defragmentation_test(
                    out, side, length, Version.TLS12,
                    tls12_client_hello_defragmentation=
                    args.tls12_client_hello_defragmentation)


def write_handshake_tests(args, out: typing_util.Writable) -> None:
    """Generate handshake tests."""
    out.write(f"""\
# Miscellaneous tests related to the TLS handshake layer.
#
# Automatically generated by {os.path.basename(sys.argv[0])}. Do not edit!

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

""")
    write_tls_handshake_defragmentation_tests(args, out)
    out.write("""\
# End of automatically generated file.
""")

def main() -> None:
    """Command line entry point."""
    parser = argparse.ArgumentParser()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-o', '--output',
                        default='tests/opt-testcases/handshake-generated.sh',
                        help='Output file (default: tests/opt-testcases/handshake-generated.sh)')
    parser.add_argument('--no-tls12-client-hello-defragmentation-support',
                        action="store_false",
                        dest="tls12_client_hello_defragmentation",
                        help="Whether the TLS 1.2 ClientHello defragmentation is "
                             "fully supported or not (default: True)")
    args = parser.parse_args()

    with open(args.output, 'w') as out:
        write_handshake_tests(args, out)
