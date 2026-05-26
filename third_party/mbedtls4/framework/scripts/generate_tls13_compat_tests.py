#!/usr/bin/env python3

# generate_tls13_compat_tests.py
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

"""
Generate TLSv1.3 Compat test cases

"""

import sys
import os
import argparse
import itertools
from collections import namedtuple

# define certificates configuration entry
Certificate = namedtuple("Certificate", ['cafile', 'certfile', 'keyfile'])
# define the certificate parameters for signature algorithms
CERTIFICATES = {
    'ecdsa_secp256r1_sha256': Certificate('$DATA_FILES_PATH/test-ca2.crt',
                                          '$DATA_FILES_PATH/ecdsa_secp256r1.crt',
                                          '$DATA_FILES_PATH/ecdsa_secp256r1.key'),
    'ecdsa_secp384r1_sha384': Certificate('$DATA_FILES_PATH/test-ca2.crt',
                                          '$DATA_FILES_PATH/ecdsa_secp384r1.crt',
                                          '$DATA_FILES_PATH/ecdsa_secp384r1.key'),
    'ecdsa_secp521r1_sha512': Certificate('$DATA_FILES_PATH/test-ca2.crt',
                                          '$DATA_FILES_PATH/ecdsa_secp521r1.crt',
                                          '$DATA_FILES_PATH/ecdsa_secp521r1.key'),
    'rsa_pss_rsae_sha256': Certificate('$DATA_FILES_PATH/test-ca_cat12.crt',
                                       '$DATA_FILES_PATH/server2-sha256.crt',
                                       '$DATA_FILES_PATH/server2.key')
}

CIPHER_SUITE_IANA_VALUE = {
    "TLS_AES_128_GCM_SHA256": 0x1301,
    "TLS_AES_256_GCM_SHA384": 0x1302,
    "TLS_CHACHA20_POLY1305_SHA256": 0x1303,
    "TLS_AES_128_CCM_SHA256": 0x1304,
    "TLS_AES_128_CCM_8_SHA256": 0x1305
}

SIG_ALG_IANA_VALUE = {
    "ecdsa_secp256r1_sha256": 0x0403,
    "ecdsa_secp384r1_sha384": 0x0503,
    "ecdsa_secp521r1_sha512": 0x0603,
    'rsa_pss_rsae_sha256': 0x0804,
}

NAMED_GROUP_IANA_VALUE = {
    'secp256r1': 0x17,
    'secp384r1': 0x18,
    'secp521r1': 0x19,
    'x25519': 0x1d,
    'x448': 0x1e,
    # Only one finite field group to keep testing time within reasonable bounds.
    'ffdhe2048': 0x100,
}

class TLSProgram:
    """
    Base class for generate server/client command.
    """

    # pylint: disable=too-many-arguments
    def __init__(self, ciphersuite=None, signature_algorithm=None, named_group=None,
                 cert_sig_alg=None):
        self._ciphers = []
        self._sig_algs = []
        self._named_groups = []
        self._cert_sig_algs = []
        if ciphersuite:
            self.add_ciphersuites(ciphersuite)
        if named_group:
            self.add_named_groups(named_group)
        if signature_algorithm:
            self.add_signature_algorithms(signature_algorithm)
        if cert_sig_alg:
            self.add_cert_signature_algorithms(cert_sig_alg)

    # add_ciphersuites should not override by sub class
    def add_ciphersuites(self, *ciphersuites):
        self._ciphers.extend(
            [cipher for cipher in ciphersuites if cipher not in self._ciphers])

    # add_signature_algorithms should not override by sub class
    def add_signature_algorithms(self, *signature_algorithms):
        self._sig_algs.extend(
            [sig_alg for sig_alg in signature_algorithms if sig_alg not in self._sig_algs])

    # add_named_groups should not override by sub class
    def add_named_groups(self, *named_groups):
        self._named_groups.extend(
            [named_group for named_group in named_groups if named_group not in self._named_groups])

    # add_cert_signature_algorithms should not override by sub class
    def add_cert_signature_algorithms(self, *signature_algorithms):
        self._cert_sig_algs.extend(
            [sig_alg for sig_alg in signature_algorithms if sig_alg not in self._cert_sig_algs])

    # pylint: disable=no-self-use
    def pre_checks(self):
        return []

    # pylint: disable=no-self-use
    def cmd(self):
        if not self._cert_sig_algs:
            self._cert_sig_algs = list(CERTIFICATES.keys())
        return self.pre_cmd()

    # pylint: disable=no-self-use
    def post_checks(self):
        return []

    # pylint: disable=no-self-use
    def pre_cmd(self):
        return ['false']

    # pylint: disable=unused-argument,no-self-use
    def hrr_post_checks(self, named_group):
        return []


class OpenSSLBase(TLSProgram):
    """
    Generate base test commands for OpenSSL.
    """

    NAMED_GROUP = {
        'secp256r1': 'P-256',
        'secp384r1': 'P-384',
        'secp521r1': 'P-521',
        'x25519': 'X25519',
        'x448': 'X448',
        'ffdhe2048': 'ffdhe2048',
    }

    def cmd(self):
        ret = super().cmd()

        if self._ciphers:
            ciphersuites = ':'.join(self._ciphers)
            ret += ["-ciphersuites {ciphersuites}".format(ciphersuites=ciphersuites)]

        if self._sig_algs:
            signature_algorithms = set(self._sig_algs + self._cert_sig_algs)
            signature_algorithms = ':'.join(signature_algorithms)
            ret += ["-sigalgs {signature_algorithms}".format(
                signature_algorithms=signature_algorithms)]

        if self._named_groups:
            named_groups = ':'.join(
                map(lambda named_group: self.NAMED_GROUP[named_group], self._named_groups))
            ret += ["-groups {named_groups}".format(named_groups=named_groups)]

        ret += ['-msg -tls1_3']

        return ret

    def pre_checks(self):
        ret = ["requires_openssl_tls1_3"]

        # ffdh groups require at least openssl 3.0
        ffdh_groups = ['ffdhe2048']

        if any(x in ffdh_groups for x in self._named_groups):
            ret = ["requires_openssl_tls1_3_with_ffdh"]

        return ret


class OpenSSLServ(OpenSSLBase):
    """
    Generate test commands for OpenSSL server.
    """

    def cmd(self):
        ret = super().cmd()
        ret += ['-num_tickets 0 -no_resume_ephemeral -no_cache']
        return ret

    def post_checks(self):
        return ['-c "HTTP/1.0 200 ok"']

    def pre_cmd(self):
        ret = ['$O_NEXT_SRV_NO_CERT']
        for _, cert, key in map(lambda sig_alg: CERTIFICATES[sig_alg], self._cert_sig_algs):
            ret += ['-cert {cert} -key {key}'.format(cert=cert, key=key)]
        return ret


class OpenSSLCli(OpenSSLBase):
    """
    Generate test commands for OpenSSL client.
    """

    def pre_cmd(self):
        return ['$O_NEXT_CLI_NO_CERT',
                '-CAfile {cafile}'.format(cafile=CERTIFICATES[self._cert_sig_algs[0]].cafile)]


class GnuTLSBase(TLSProgram):
    """
    Generate base test commands for GnuTLS.
    """

    CIPHER_SUITE = {
        'TLS_AES_256_GCM_SHA384': [
            'AES-256-GCM',
            'SHA384',
            'AEAD'],
        'TLS_AES_128_GCM_SHA256': [
            'AES-128-GCM',
            'SHA256',
            'AEAD'],
        'TLS_CHACHA20_POLY1305_SHA256': [
            'CHACHA20-POLY1305',
            'SHA256',
            'AEAD'],
        'TLS_AES_128_CCM_SHA256': [
            'AES-128-CCM',
            'SHA256',
            'AEAD'],
        'TLS_AES_128_CCM_8_SHA256': [
            'AES-128-CCM-8',
            'SHA256',
            'AEAD']}

    SIGNATURE_ALGORITHM = {
        'ecdsa_secp256r1_sha256': ['SIGN-ECDSA-SECP256R1-SHA256'],
        'ecdsa_secp521r1_sha512': ['SIGN-ECDSA-SECP521R1-SHA512'],
        'ecdsa_secp384r1_sha384': ['SIGN-ECDSA-SECP384R1-SHA384'],
        'rsa_pss_rsae_sha256': ['SIGN-RSA-PSS-RSAE-SHA256']}

    NAMED_GROUP = {
        'secp256r1': ['GROUP-SECP256R1'],
        'secp384r1': ['GROUP-SECP384R1'],
        'secp521r1': ['GROUP-SECP521R1'],
        'x25519': ['GROUP-X25519'],
        'x448': ['GROUP-X448'],
        'ffdhe2048': ['GROUP-FFDHE2048'],
    }

    def pre_checks(self):
        return ["requires_gnutls_tls1_3",
                "requires_gnutls_next_no_ticket"]

    def cmd(self):
        ret = super().cmd()

        priority_string_list = []

        def update_priority_string_list(items, map_table):
            for item in items:
                for i in map_table[item]:
                    if i not in priority_string_list:
                        yield i

        if self._ciphers:
            priority_string_list.extend(update_priority_string_list(
                self._ciphers, self.CIPHER_SUITE))
        else:
            priority_string_list.extend(['CIPHER-ALL', 'MAC-ALL'])

        if self._sig_algs:
            signature_algorithms = set(self._sig_algs + self._cert_sig_algs)
            priority_string_list.extend(update_priority_string_list(
                signature_algorithms, self.SIGNATURE_ALGORITHM))
        else:
            priority_string_list.append('SIGN-ALL')


        if self._named_groups:
            priority_string_list.extend(update_priority_string_list(
                self._named_groups, self.NAMED_GROUP))
        else:
            priority_string_list.append('GROUP-ALL')

        priority_string_list = ['NONE'] + \
            priority_string_list + ['VERS-TLS1.3']

        priority_string = ':+'.join(priority_string_list)
        priority_string += ':%NO_TICKETS'

        ret += ['--priority={priority_string}'.format(
            priority_string=priority_string)]
        return ret

class GnuTLSServ(GnuTLSBase):
    """
    Generate test commands for GnuTLS server.
    """

    def pre_cmd(self):
        ret = ['$G_NEXT_SRV_NO_CERT', '--http', '--disable-client-cert', '--debug=4']

        for _, cert, key in map(lambda sig_alg: CERTIFICATES[sig_alg], self._cert_sig_algs):
            ret += ['--x509certfile {cert} --x509keyfile {key}'.format(
                cert=cert, key=key)]
        return ret

    def post_checks(self):
        return ['-c "HTTP/1.0 200 OK"']


class GnuTLSCli(GnuTLSBase):
    """
    Generate test commands for GnuTLS client.
    """

    def pre_cmd(self):
        return ['$G_NEXT_CLI_NO_CERT', '--debug=4', '--single-key-share',
                '--x509cafile {cafile}'.format(cafile=CERTIFICATES[self._cert_sig_algs[0]].cafile)]


class MbedTLSBase(TLSProgram):
    """
    Generate base test commands for mbedTLS.
    """

    CIPHER_SUITE = {
        'TLS_AES_256_GCM_SHA384': 'TLS1-3-AES-256-GCM-SHA384',
        'TLS_AES_128_GCM_SHA256': 'TLS1-3-AES-128-GCM-SHA256',
        'TLS_CHACHA20_POLY1305_SHA256': 'TLS1-3-CHACHA20-POLY1305-SHA256',
        'TLS_AES_128_CCM_SHA256': 'TLS1-3-AES-128-CCM-SHA256',
        'TLS_AES_128_CCM_8_SHA256': 'TLS1-3-AES-128-CCM-8-SHA256'}

    def cmd(self):
        ret = super().cmd()
        ret += ['debug_level=4']


        if self._ciphers:
            ciphers = ','.join(
                map(lambda cipher: self.CIPHER_SUITE[cipher], self._ciphers))
            ret += ["force_ciphersuite={ciphers}".format(ciphers=ciphers)]

        if self._sig_algs + self._cert_sig_algs:
            ret += ['sig_algs={sig_algs}'.format(
                sig_algs=','.join(set(self._sig_algs + self._cert_sig_algs)))]

        if self._named_groups:
            named_groups = ','.join(self._named_groups)
            ret += ["groups={named_groups}".format(named_groups=named_groups)]
        return ret

    #pylint: disable=missing-function-docstring
    def add_ffdh_group_requirements(self, requirement_list):
        if 'ffdhe2048' in self._named_groups:
            requirement_list.append('requires_config_enabled PSA_WANT_DH_RFC7919_2048')
        if 'ffdhe3072' in self._named_groups:
            requirement_list.append('requires_config_enabled PSA_WANT_DH_RFC7919_2048')
        if 'ffdhe4096' in self._named_groups:
            requirement_list.append('requires_config_enabled PSA_WANT_DH_RFC7919_2048')
        if 'ffdhe6144' in self._named_groups:
            requirement_list.append('requires_config_enabled PSA_WANT_DH_RFC7919_2048')
        if 'ffdhe8192' in self._named_groups:
            requirement_list.append('requires_config_enabled PSA_WANT_DH_RFC7919_2048')

    def pre_checks(self):
        ret = ['requires_config_enabled MBEDTLS_DEBUG_C',
               'requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED']

        if 'rsa_pss_rsae_sha256' in self._sig_algs + self._cert_sig_algs:
            ret.append(
                'requires_config_enabled MBEDTLS_X509_RSASSA_PSS_SUPPORT')

        ec_groups = ['secp256r1', 'secp384r1', 'secp521r1', 'x25519', 'x448']
        ffdh_groups = ['ffdhe2048', 'ffdhe3072', 'ffdhe4096', 'ffdhe6144', 'ffdhe8192']

        if any(x in ec_groups for x in self._named_groups):
            ret.append('requires_config_enabled PSA_WANT_ALG_ECDH')

        if any(x in ffdh_groups for x in self._named_groups):
            ret.append('requires_config_enabled PSA_WANT_ALG_FFDH')
            self.add_ffdh_group_requirements(ret)

        return ret


class MbedTLSServ(MbedTLSBase):
    """
    Generate test commands for mbedTLS server.
    """

    def cmd(self):
        ret = super().cmd()
        ret += ['tls13_kex_modes=ephemeral cookies=0 tickets=0']
        return ret

    def pre_checks(self):
        return ['requires_config_enabled MBEDTLS_SSL_SRV_C'] + super().pre_checks()

    def post_checks(self):
        check_strings = ["Protocol is TLSv1.3"]
        if self._ciphers:
            check_strings.append(
                "server hello, chosen ciphersuite: {} ( id={:04d} )".format(
                    self.CIPHER_SUITE[self._ciphers[0]],
                    CIPHER_SUITE_IANA_VALUE[self._ciphers[0]]))
        if self._sig_algs:
            check_strings.append(
                "received signature algorithm: 0x{:x}".format(
                    SIG_ALG_IANA_VALUE[self._sig_algs[0]]))

        for named_group in self._named_groups:
            check_strings += ['got named group: {named_group}({iana_value:04x})'.format(
                                named_group=named_group,
                                iana_value=NAMED_GROUP_IANA_VALUE[named_group])]

        check_strings.append("Certificate verification was skipped")
        return ['-s "{}"'.format(i) for i in check_strings]

    def pre_cmd(self):
        ret = ['$P_SRV']
        for _, cert, key in map(lambda sig_alg: CERTIFICATES[sig_alg], self._cert_sig_algs):
            ret += ['crt_file={cert} key_file={key}'.format(cert=cert, key=key)]
        return ret

    def hrr_post_checks(self, named_group):
        return ['-s "HRR selected_group: {:s}"'.format(named_group)]


class MbedTLSCli(MbedTLSBase):
    """
    Generate test commands for mbedTLS client.
    """

    def pre_cmd(self):
        return ['$P_CLI',
                'ca_file={cafile}'.format(cafile=CERTIFICATES[self._cert_sig_algs[0]].cafile)]

    def pre_checks(self):
        return ['requires_config_enabled MBEDTLS_SSL_CLI_C'] + super().pre_checks()

    def hrr_post_checks(self, named_group):
        ret = ['-c "received HelloRetryRequest message"']
        ret += ['-c "selected_group ( {:d} )"'.format(NAMED_GROUP_IANA_VALUE[named_group])]
        return ret

    def post_checks(self):
        check_strings = ["Protocol is TLSv1.3"]
        if self._ciphers:
            check_strings.append(
                "server hello, chosen ciphersuite: ( {:04x} ) - {}".format(
                    CIPHER_SUITE_IANA_VALUE[self._ciphers[0]],
                    self.CIPHER_SUITE[self._ciphers[0]]))
        if self._sig_algs:
            check_strings.append(
                "Certificate Verify: Signature algorithm ( {:04x} )".format(
                    SIG_ALG_IANA_VALUE[self._sig_algs[0]]))

        for named_group in self._named_groups:
            check_strings += ['NamedGroup: {named_group} ( {iana_value:x} )'.format(
                                named_group=named_group,
                                iana_value=NAMED_GROUP_IANA_VALUE[named_group])]

        check_strings.append("Verifying peer X.509 certificate... ok")
        return ['-c "{}"'.format(i) for i in check_strings]


SERVER_CLASSES = {'OpenSSL': OpenSSLServ, 'GnuTLS': GnuTLSServ, 'mbedTLS': MbedTLSServ}
CLIENT_CLASSES = {'OpenSSL': OpenSSLCli, 'GnuTLS': GnuTLSCli, 'mbedTLS': MbedTLSCli}


def generate_compat_test(client=None, server=None, cipher=None, named_group=None, sig_alg=None):
    """
    Generate test case with `ssl-opt.sh` format.
    """
    name = 'TLS 1.3 {client[0]}->{server[0]}: {cipher},{named_group},{sig_alg}'.format(
        client=client, server=server, cipher=cipher[4:], sig_alg=sig_alg, named_group=named_group)

    server_object = SERVER_CLASSES[server](ciphersuite=cipher,
                                           named_group=named_group,
                                           signature_algorithm=sig_alg,
                                           cert_sig_alg=sig_alg)
    client_object = CLIENT_CLASSES[client](ciphersuite=cipher,
                                           named_group=named_group,
                                           signature_algorithm=sig_alg,
                                           cert_sig_alg=sig_alg)

    cmd = ['run_test "{}"'.format(name),
           '"{}"'.format(' '.join(server_object.cmd())),
           '"{}"'.format(' '.join(client_object.cmd())),
           '0']
    cmd += server_object.post_checks()
    cmd += client_object.post_checks()
    cmd += ['-C "received HelloRetryRequest message"']
    prefix = ' \\\n' + (' '*9)
    cmd = prefix.join(cmd)
    return '\n'.join(server_object.pre_checks() + client_object.pre_checks() + [cmd])


def generate_hrr_compat_test(client=None, server=None,
                             client_named_group=None, server_named_group=None,
                             cert_sig_alg=None):
    """
    Generate Hello Retry Request test case with `ssl-opt.sh` format.
    """
    name = 'TLS 1.3 {client[0]}->{server[0]}: HRR {c_named_group} -> {s_named_group}'.format(
        client=client, server=server, c_named_group=client_named_group,
        s_named_group=server_named_group)
    server_object = SERVER_CLASSES[server](named_group=server_named_group,
                                           cert_sig_alg=cert_sig_alg)

    client_object = CLIENT_CLASSES[client](named_group=client_named_group,
                                           cert_sig_alg=cert_sig_alg)
    client_object.add_named_groups(server_named_group)

    cmd = ['run_test "{}"'.format(name),
           '"{}"'.format(' '.join(server_object.cmd())),
           '"{}"'.format(' '.join(client_object.cmd())),
           '0']
    cmd += server_object.post_checks()
    cmd += client_object.post_checks()
    cmd += server_object.hrr_post_checks(server_named_group)
    cmd += client_object.hrr_post_checks(server_named_group)
    prefix = ' \\\n' + (' '*9)
    cmd = prefix.join(cmd)
    return '\n'.join(server_object.pre_checks() +
                     client_object.pre_checks() +
                     [cmd])

SSL_OUTPUT_HEADER = '''\
# TLS 1.3 interoperability test cases (equivalent of compat.sh for TLS 1.3).
#
# Automatically generated by {cmd}. Do not edit!

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
'''
DATA_FILES_PATH_VAR = '''
DATA_FILES_PATH=../framework/data_files
'''

def main():
    """
    Main function of this program
    """
    parser = argparse.ArgumentParser()

    parser.add_argument('-o', '--output',
                        default='tests/opt-testcases/tls13-compat.sh',
                        help='Output file path (not used with -1)')

    parser.add_argument('-1', '--single', action='store_true',
                        help='Print a single test case')
    # Single mode used to be the default.
    parser.add_argument('-a', '--generate-all-tls13-compat-tests',
                        action='store_false', dest='single',
                        help='Generate all test cases (negates -1) (default)')

    parser.add_argument('--list-ciphers', action='store_true',
                        default=False, help='List supported ciphersuites')

    parser.add_argument('--list-sig-algs', action='store_true',
                        default=False, help='List supported signature algorithms')

    parser.add_argument('--list-named-groups', action='store_true',
                        default=False, help='List supported named groups')

    parser.add_argument('--list-servers', action='store_true',
                        default=False, help='List supported TLS servers')

    parser.add_argument('--list-clients', action='store_true',
                        default=False, help='List supported TLS Clients')

    parser.add_argument('server', choices=SERVER_CLASSES.keys(), nargs='?',
                        default=list(SERVER_CLASSES.keys())[0],
                        help='Choose TLS server program for test')
    parser.add_argument('client', choices=CLIENT_CLASSES.keys(), nargs='?',
                        default=list(CLIENT_CLASSES.keys())[0],
                        help='Choose TLS client program for test')
    parser.add_argument('cipher', choices=CIPHER_SUITE_IANA_VALUE.keys(), nargs='?',
                        default=list(CIPHER_SUITE_IANA_VALUE.keys())[0],
                        help='Choose cipher suite for test')
    parser.add_argument('sig_alg', choices=SIG_ALG_IANA_VALUE.keys(), nargs='?',
                        default=list(SIG_ALG_IANA_VALUE.keys())[0],
                        help='Choose cipher suite for test')
    parser.add_argument('named_group', choices=NAMED_GROUP_IANA_VALUE.keys(), nargs='?',
                        default=list(NAMED_GROUP_IANA_VALUE.keys())[0],
                        help='Choose cipher suite for test')

    args = parser.parse_args()

    def get_all_test_cases():
        # Generate normal compat test cases
        for client, server, cipher, named_group, sig_alg in \
            itertools.product(CLIENT_CLASSES.keys(),
                              SERVER_CLASSES.keys(),
                              CIPHER_SUITE_IANA_VALUE.keys(),
                              NAMED_GROUP_IANA_VALUE.keys(),
                              SIG_ALG_IANA_VALUE.keys()):
            if server == 'mbedTLS' or client == 'mbedTLS':
                yield generate_compat_test(client=client, server=server,
                                           cipher=cipher, named_group=named_group,
                                           sig_alg=sig_alg)


        # Generate Hello Retry Request  compat test cases
        for client, server, client_named_group, server_named_group in \
            itertools.product(CLIENT_CLASSES.keys(),
                              SERVER_CLASSES.keys(),
                              NAMED_GROUP_IANA_VALUE.keys(),
                              NAMED_GROUP_IANA_VALUE.keys()):

            if (client == 'mbedTLS' or server == 'mbedTLS') and \
                client_named_group != server_named_group:
                yield generate_hrr_compat_test(client=client, server=server,
                                               client_named_group=client_named_group,
                                               server_named_group=server_named_group,
                                               cert_sig_alg="ecdsa_secp256r1_sha256")

    if not args.single:
        if args.output:
            with open(args.output, 'w', encoding="utf-8") as f:
                f.write(SSL_OUTPUT_HEADER.format(
                    filename=os.path.basename(args.output),
                    cmd=os.path.basename(sys.argv[0])))
                f.write(DATA_FILES_PATH_VAR)
                f.write('\n\n'.join(get_all_test_cases()))
                f.write('\n')
        else:
            print('\n\n'.join(get_all_test_cases()))
        return 0

    if args.list_ciphers or args.list_sig_algs or args.list_named_groups \
            or args.list_servers or args.list_clients:
        if args.list_ciphers:
            print(*CIPHER_SUITE_IANA_VALUE.keys())
        if args.list_sig_algs:
            print(*SIG_ALG_IANA_VALUE.keys())
        if args.list_named_groups:
            print(*NAMED_GROUP_IANA_VALUE.keys())
        if args.list_servers:
            print(*SERVER_CLASSES.keys())
        if args.list_clients:
            print(*CLIENT_CLASSES.keys())
        return 0

    print(generate_compat_test(server=args.server, client=args.client, sig_alg=args.sig_alg,
                               cipher=args.cipher, named_group=args.named_group))
    return 0


if __name__ == "__main__":
    sys.exit(main())
