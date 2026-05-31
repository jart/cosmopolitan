#!/usr/bin/env python3

"""
Generate `tests/src/test_certs.h` which includes certficaties/keys/certificate list for testing.
"""

#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later


import os
import sys
import argparse
import jinja2
from mbedtls_framework.build_tree import guess_project_root

TESTS_DIR = os.path.join(guess_project_root(), 'tests')
FRAMEWORK_DIR = os.path.join(guess_project_root(), 'framework')
DATA_FILES_PATH = os.path.join(FRAMEWORK_DIR, 'data_files')

INPUT_ARGS = [
    ("string", "TEST_CA_CRT_EC_PEM", DATA_FILES_PATH + "/test-ca2.crt"),
    ("binary", "TEST_CA_CRT_EC_DER", DATA_FILES_PATH + "/test-ca2.crt.der"),
    ("string", "TEST_CA_KEY_EC_PEM", DATA_FILES_PATH + "/test-ca2.key.enc"),
    ("password", "TEST_CA_PWD_EC_PEM", "PolarSSLTest"),
    ("binary", "TEST_CA_KEY_EC_DER", DATA_FILES_PATH + "/test-ca2.key.der"),
    ("string", "TEST_CA_CRT_RSA_SHA256_PEM", DATA_FILES_PATH + "/test-ca-sha256.crt"),
    ("binary", "TEST_CA_CRT_RSA_SHA256_DER", DATA_FILES_PATH + "/test-ca-sha256.crt.der"),
    ("string", "TEST_CA_CRT_RSA_SHA1_PEM", DATA_FILES_PATH + "/test-ca-sha1.crt"),
    ("binary", "TEST_CA_CRT_RSA_SHA1_DER", DATA_FILES_PATH + "/test-ca-sha1.crt.der"),
    ("string", "TEST_CA_KEY_RSA_PEM", DATA_FILES_PATH + "/test-ca.key"),
    ("password", "TEST_CA_PWD_RSA_PEM", "PolarSSLTest"),
    ("binary", "TEST_CA_KEY_RSA_DER", DATA_FILES_PATH + "/test-ca.key.der"),
    ("string", "TEST_SRV_CRT_EC_PEM", DATA_FILES_PATH + "/server5.crt"),
    ("binary", "TEST_SRV_CRT_EC_DER", DATA_FILES_PATH + "/server5.crt.der"),
    ("string", "TEST_SRV_KEY_EC_PEM", DATA_FILES_PATH + "/server5.key"),
    ("binary", "TEST_SRV_KEY_EC_DER", DATA_FILES_PATH + "/server5.key.der"),
    ("string", "TEST_SRV_CRT_RSA_SHA256_PEM", DATA_FILES_PATH + "/server2-sha256.crt"),
    ("binary", "TEST_SRV_CRT_RSA_SHA256_DER", DATA_FILES_PATH + "/server2-sha256.crt.der"),
    ("string", "TEST_SRV_CRT_RSA_SHA1_PEM", DATA_FILES_PATH + "/server2.crt"),
    ("binary", "TEST_SRV_CRT_RSA_SHA1_DER", DATA_FILES_PATH + "/server2.crt.der"),
    ("string", "TEST_SRV_KEY_RSA_PEM", DATA_FILES_PATH + "/server2.key"),
    ("binary", "TEST_SRV_KEY_RSA_DER", DATA_FILES_PATH + "/server2.key.der"),
    ("string", "TEST_CLI_CRT_EC_PEM", DATA_FILES_PATH + "/cli2.crt"),
    ("binary", "TEST_CLI_CRT_EC_DER", DATA_FILES_PATH + "/cli2.crt.der"),
    ("string", "TEST_CLI_KEY_EC_PEM", DATA_FILES_PATH + "/cli2.key"),
    ("binary", "TEST_CLI_KEY_EC_DER", DATA_FILES_PATH + "/cli2.key.der"),
    ("string", "TEST_CLI_CRT_RSA_PEM", DATA_FILES_PATH + "/cli-rsa-sha256.crt"),
    ("binary", "TEST_CLI_CRT_RSA_DER", DATA_FILES_PATH + "/cli-rsa-sha256.crt.der"),
    ("string", "TEST_CLI_KEY_RSA_PEM", DATA_FILES_PATH + "/cli-rsa.key"),
    ("binary", "TEST_CLI_KEY_RSA_DER", DATA_FILES_PATH + "/cli-rsa.key.der"),
]

def main():
    parser = argparse.ArgumentParser()
    default_output_path = os.path.join(TESTS_DIR, 'include', 'test', 'test_certs.h')
    parser.add_argument('--output', type=str, default=default_output_path)
    parser.add_argument('--list-dependencies', action='store_true')
    args = parser.parse_args()

    if args.list_dependencies:
        files_list = [arg[2] for arg in INPUT_ARGS
                      if arg[0] != "password"]
        print(" ".join(files_list))
        return

    generate(INPUT_ARGS, output=args.output)

#pylint: disable=dangerous-default-value, unused-argument
def generate(values=[], output=None):
    """Generate C header file.
    """
    template_loader = jinja2.FileSystemLoader(DATA_FILES_PATH)
    template_env = jinja2.Environment(
        loader=template_loader, lstrip_blocks=True, trim_blocks=True,
        keep_trailing_newline=True)

    def read_as_c_array(filename):
        with open(filename, 'rb') as f:
            data = f.read(12)
            while data:
                yield ', '.join(['{:#04x}'.format(b) for b in data])
                data = f.read(12)

    def read_lines(filename):
        with open(filename) as f:
            try:
                for line in f:
                    yield line.strip()
            except:
                print(filename)
                raise

    def put_to_column(value, position=0):
        return ' '*position + value

    template_env.filters['read_as_c_array'] = read_as_c_array
    template_env.filters['read_lines'] = read_lines
    template_env.filters['put_to_column'] = put_to_column

    template = template_env.get_template('test_certs.h.jinja2')

    with open(output, 'w') as f:
        f.write(template.render(macros=values))


if __name__ == '__main__':
    sys.exit(main())
