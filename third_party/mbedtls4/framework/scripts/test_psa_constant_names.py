#!/usr/bin/env python3
"""Test the program psa_constant_names.
Gather constant names from header files and test cases. Compile a C program
to print out their numerical values, feed these numerical values to
psa_constant_names, and check that the output is the original name.
Return 0 if all test cases pass, 1 if the output was not always as expected,
or 1 (with a Python backtrace) if there was an operational error.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
from collections import namedtuple
import os
import re
import subprocess
import sys
from typing import Iterable, List, Optional, Tuple

from mbedtls_framework import build_tree
from mbedtls_framework import c_build_helper
from mbedtls_framework.macro_collector import InputsForTest, PSAMacroEnumerator
from mbedtls_framework import typing_util

def gather_inputs(headers: Iterable[str],
                  test_suites: Iterable[str],
                  inputs_class=InputsForTest) -> PSAMacroEnumerator:
    """Read the list of inputs to test psa_constant_names with."""
    inputs = inputs_class()
    for header in headers:
        inputs.parse_header(header)
    for test_cases in test_suites:
        inputs.parse_test_cases(test_cases)
    inputs.add_numerical_values()
    inputs.gather_arguments()
    return inputs

def run_c(type_word: str,
          expressions: Iterable[str],
          include_path: Optional[str] = None,
          keep_c: bool = False) -> List[str]:
    """Generate and run a program to print out numerical values of C expressions."""
    if type_word == 'status':
        cast_to = 'long'
        printf_format = '%ld'
    else:
        cast_to = 'unsigned long'
        printf_format = '0x%08lx'
    return c_build_helper.get_c_expression_values(
        cast_to, printf_format,
        expressions,
        caller='test_psa_constant_names.py for {} values'.format(type_word),
        file_label=type_word,
        header='#include <psa/crypto.h>',
        include_path=include_path,
        keep_c=keep_c
    )

NORMALIZE_STRIP_RE = re.compile(r'\s+')
def normalize(expr: str) -> str:
    """Normalize the C expression so as not to care about trivial differences.

    Currently "trivial differences" means whitespace.
    """
    return re.sub(NORMALIZE_STRIP_RE, '', expr)

ALG_TRUNCATED_TO_SELF_RE = \
    re.compile(r'PSA_ALG_AEAD_WITH_SHORTENED_TAG\('
               r'PSA_ALG_(?:CCM|CHACHA20_POLY1305|GCM)'
               r', *16\)\Z')

def is_simplifiable(expr: str) -> bool:
    """Determine whether an expression is simplifiable.

    Simplifiable expressions can't be output in their input form, since
    the output will be the simple form. Therefore they must be excluded
    from testing.
    """
    if ALG_TRUNCATED_TO_SELF_RE.match(expr):
        return True
    return False

def collect_values(inputs: InputsForTest,
                   type_word: str,
                   include_path: Optional[str] = None,
                   keep_c: bool = False) -> Tuple[List[str], List[str]]:
    """Generate expressions using known macro names and calculate their values.

    Return a list of pairs of (expr, value) where expr is an expression and
    value is a string representation of its integer value.
    """
    names = inputs.get_names(type_word)
    expressions = sorted(expr
                         for expr in inputs.generate_expressions(names)
                         if not is_simplifiable(expr))
    values = run_c(type_word, expressions,
                   include_path=include_path, keep_c=keep_c)
    return expressions, values

class Tests:
    """An object representing tests and their results."""

    Error = namedtuple('Error',
                       ['type', 'expression', 'value', 'output'])

    def __init__(self, options) -> None:
        self.options = options
        self.count = 0
        self.errors = [] #type: List[Tests.Error]

    def run_one(self, inputs: InputsForTest, type_word: str) -> None:
        """Test psa_constant_names for the specified type.

        Run the program on the names for this type.
        Use the inputs to figure out what arguments to pass to macros that
        take arguments.
        """
        expressions, values = collect_values(inputs, type_word,
                                             include_path=self.options.include,
                                             keep_c=self.options.keep_c)
        output_bytes = subprocess.check_output([self.options.program,
                                                type_word] + values)
        output = output_bytes.decode('ascii')
        outputs = output.strip().split('\n')
        self.count += len(expressions)
        for expr, value, output in zip(expressions, values, outputs):
            if self.options.show:
                sys.stdout.write('{} {}\t{}\n'.format(type_word, value, output))
            if normalize(expr) != normalize(output):
                self.errors.append(self.Error(type=type_word,
                                              expression=expr,
                                              value=value,
                                              output=output))

    def run_all(self, inputs: InputsForTest) -> None:
        """Run psa_constant_names on all the gathered inputs."""
        for type_word in ['status', 'algorithm', 'ecc_curve', 'dh_group',
                          'key_type', 'key_usage']:
            self.run_one(inputs, type_word)

    def report(self, out: typing_util.Writable) -> None:
        """Describe each case where the output is not as expected.

        Write the errors to ``out``.
        Also write a total.
        """
        for error in self.errors:
            out.write('For {} "{}", got "{}" (value: {})\n'
                      .format(error.type, error.expression,
                              error.output, error.value))
        out.write('{} test cases'.format(self.count))
        if self.errors:
            out.write(', {} FAIL\n'.format(len(self.errors)))
        else:
            out.write(' PASS\n')

HEADERS = ['psa/crypto.h', 'psa/crypto_extra.h', 'psa/crypto_values.h']

if build_tree.is_mbedtls_3_6():
    TEST_SUITES = ['tests/suites/test_suite_psa_crypto_metadata.data']
else:
    TEST_SUITES = ['tf-psa-crypto/tests/suites/test_suite_psa_crypto_metadata.data']

def main():
    parser = argparse.ArgumentParser(description=globals()['__doc__'])
    if build_tree.is_mbedtls_3_6():
        parser.add_argument('--include', '-I',
                            action='append', default=['include'],
                            help='Directory for header files')
    else:
        parser.add_argument('--include', '-I',
                            action='append', default=['tf-psa-crypto/include',
                                                      'tf-psa-crypto/drivers/builtin/include',
                                                      'tf-psa-crypto/drivers/everest/include',
                                                      'tf-psa-crypto/drivers/everest/include/' +
                                                      'tf-psa-crypto/private',
                                                      'tf-psa-crypto/drivers/pqcp/include',
                                                      'include'],
                            help='Directory for header files')
    parser.add_argument('--keep-c',
                        action='store_true', dest='keep_c', default=False,
                        help='Keep the intermediate C file')
    parser.add_argument('--no-keep-c',
                        action='store_false', dest='keep_c',
                        help='Don\'t keep the intermediate C file (default)')
    if build_tree.is_mbedtls_3_6():
        parser.add_argument('--program',
                            default='programs/psa/psa_constant_names',
                            help='Program to test')
    else:
        parser.add_argument('--program',
                            default='tf-psa-crypto/programs/psa/psa_constant_names',
                            help='Program to test')
    parser.add_argument('--show',
                        action='store_true',
                        help='Show tested values on stdout')
    parser.add_argument('--no-show',
                        action='store_false', dest='show',
                        help='Don\'t show tested values (default)')
    options = parser.parse_args()
    headers = [os.path.join(options.include[0], h) for h in HEADERS]
    inputs = gather_inputs(headers, TEST_SUITES)
    tests = Tests(options)
    tests.run_all(inputs)
    tests.report(sys.stdout)
    if tests.errors:
        sys.exit(1)

if __name__ == '__main__':
    main()
