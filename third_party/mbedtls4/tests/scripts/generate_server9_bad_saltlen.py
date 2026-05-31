#!/usr/bin/env python3
"""Generate server9-bad-saltlen.crt

Generate a certificate signed with RSA-PSS, with an incorrect salt length.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import subprocess
import argparse
from asn1crypto import pem, x509, core #type: ignore #pylint: disable=import-error

OPENSSL_RSA_PSS_CERT_COMMAND = r'''
openssl x509 -req -CA {ca_name}.crt -CAkey {ca_name}.key -set_serial 24 {ca_password} \
    {openssl_extfile} -days 3650 -outform DER -in {csr}  \
    -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:{anounce_saltlen} \
    -sigopt rsa_mgf1_md:sha256
'''
SIG_OPT = \
    r'-sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:{saltlen} -sigopt rsa_mgf1_md:sha256'
OPENSSL_RSA_PSS_DGST_COMMAND = r'''openssl dgst -sign {ca_name}.key {ca_password} \
    -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:{actual_saltlen} \
    -sigopt rsa_mgf1_md:sha256'''


def auto_int(x):
    return int(x, 0)


def build_argparser(parser):
    """Build argument parser"""
    parser.description = __doc__
    parser.add_argument('--ca-name', type=str, required=True,
                        help='Basename of CA files')
    parser.add_argument('--ca-password', type=str,
                        required=True, help='CA key file password')
    parser.add_argument('--csr', type=str, required=True,
                        help='CSR file for generating certificate')
    parser.add_argument('--openssl-extfile', type=str,
                        required=True, help='X905 v3 extension config file')
    parser.add_argument('--anounce_saltlen', type=auto_int,
                        required=True, help='Announced salt length')
    parser.add_argument('--actual_saltlen', type=auto_int,
                        required=True, help='Actual salt length')
    parser.add_argument('--output', type=str, required=True)


def main():
    parser = argparse.ArgumentParser()
    build_argparser(parser)
    args = parser.parse_args()

    return generate(**vars(args))

def generate(**kwargs):
    """Generate different salt length certificate file."""
    ca_password = kwargs.get('ca_password', '')
    if ca_password:
        kwargs['ca_password'] = r'-passin "pass:{ca_password}"'.format(
            **kwargs)
    else:
        kwargs['ca_password'] = ''
    extfile = kwargs.get('openssl_extfile', '')
    if extfile:
        kwargs['openssl_extfile'] = '-extfile {openssl_extfile}'.format(
            **kwargs)
    else:
        kwargs['openssl_extfile'] = ''

    cmd = OPENSSL_RSA_PSS_CERT_COMMAND.format(**kwargs)
    der_bytes = subprocess.check_output(cmd, shell=True)
    target_certificate = x509.Certificate.load(der_bytes)

    cmd = OPENSSL_RSA_PSS_DGST_COMMAND.format(**kwargs)
    #pylint: disable=unexpected-keyword-arg
    der_bytes = subprocess.check_output(cmd,
                                        input=target_certificate['tbs_certificate'].dump(),
                                        shell=True)

    with open(kwargs.get('output'), 'wb') as f:
        target_certificate['signature_value'] = core.OctetBitString(der_bytes)
        f.write(pem.armor('CERTIFICATE', target_certificate.dump()))


if __name__ == '__main__':
    main()
