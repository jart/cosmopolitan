#!/usr/bin/env python3
"""Generate wrapper functions for PSA function calls.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
from mbedtls_framework.code_wrapper.psa_test_wrapper import PSATestWrapper, PSALoggingTestWrapper
from mbedtls_framework import build_tree

def main() -> None:
    default_c_output_file_name = 'tests/src/psa_test_wrappers.c'
    default_h_output_file_name = 'tests/include/test/psa_test_wrappers.h'

    project_root = build_tree.guess_project_root()
    if build_tree.looks_like_mbedtls_root(project_root) and \
       not build_tree.is_mbedtls_3_6():
        default_c_output_file_name = 'tf-psa-crypto/' + default_c_output_file_name
        default_h_output_file_name = 'tf-psa-crypto/' + default_h_output_file_name

    parser = argparse.ArgumentParser(description=globals()['__doc__'])
    parser.add_argument('--log',
                        help='Stream to log to (default: no logging code)')
    parser.add_argument('--output-c',
                        metavar='FILENAME',
                        default=default_c_output_file_name,
                        help=('Output .c file path (default: {}; skip .c output if empty)'
                              .format(default_c_output_file_name)))
    parser.add_argument('--output-h',
                        metavar='FILENAME',
                        default=default_h_output_file_name,
                        help=('Output .h file path (default: {}; skip .h output if empty)'
                              .format(default_h_output_file_name)))
    options = parser.parse_args()

    if options.log:
        generator = PSALoggingTestWrapper(default_h_output_file_name,
                                          default_c_output_file_name,
                                          options.log) #type: PSATestWrapper
    else:
        generator = PSATestWrapper(default_h_output_file_name,
                                   default_c_output_file_name)

    if options.output_h:
        generator.write_h_file(options.output_h)
    if options.output_c:
        generator.write_c_file(options.output_c)

if __name__ == '__main__':
    main()
