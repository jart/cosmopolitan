#!/usr/bin/env python3
"""Describe the test coverage of PSA functions in terms of return statuses.

1. Build Mbed TLS with -DRECORD_PSA_STATUS_COVERAGE_LOG
2. Run psa_collect_statuses.py

The output is a series of line of the form "psa_foo PSA_ERROR_XXX". Each
function/status combination appears only once.

This script must be run from the top of an Mbed TLS source tree.
The build command is "make -DRECORD_PSA_STATUS_COVERAGE_LOG", which is
only supported with make (as opposed to CMake or other build methods).
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import os
import subprocess
import sys

DEFAULT_STATUS_LOG_FILE = 'tests/statuses.log'
DEFAULT_PSA_CONSTANT_NAMES = 'tf-psa-crypto/programs/psa/psa_constant_names'

class Statuses:
    """Information about observed return statues of API functions."""

    def __init__(self):
        self.functions = {}
        self.codes = set()
        self.status_names = {}

    def collect_log(self, log_file_name):
        """Read logs from RECORD_PSA_STATUS_COVERAGE_LOG.

        Read logs produced by running Mbed TLS test suites built with
        -DRECORD_PSA_STATUS_COVERAGE_LOG.
        """
        with open(log_file_name) as log:
            for line in log:
                value, function, tail = line.split(':', 2)
                if function not in self.functions:
                    self.functions[function] = {}
                fdata = self.functions[function]
                if value not in self.functions[function]:
                    fdata[value] = []
                fdata[value].append(tail)
                self.codes.add(int(value))

    def get_constant_names(self, psa_constant_names):
        """Run psa_constant_names to obtain names for observed numerical values."""
        values = [str(value) for value in self.codes]
        cmd = [psa_constant_names, 'status'] + values
        output = subprocess.check_output(cmd).decode('ascii')
        for value, name in zip(values, output.rstrip().split('\n')):
            self.status_names[value] = name

    def report(self):
        """Report observed return values for each function.

        The report is a series of line of the form "psa_foo PSA_ERROR_XXX".
        """
        for function in sorted(self.functions.keys()):
            fdata = self.functions[function]
            names = [self.status_names[value] for value in fdata.keys()]
            for name in sorted(names):
                sys.stdout.write('{} {}\n'.format(function, name))

def collect_status_logs(options):
    """Build and run unit tests and report observed function return statuses.

    Build Mbed TLS with -DRECORD_PSA_STATUS_COVERAGE_LOG, run the
    test suites and display information about observed return statuses.
    """
    rebuilt = False
    if not options.use_existing_log and os.path.exists(options.log_file):
        os.remove(options.log_file)
    if not os.path.exists(options.log_file):
        if options.clean_before:
            subprocess.check_call(['make', '-f', 'scripts/legacy.make', 'clean'],
                                  cwd='tests',
                                  stdout=sys.stderr)
        with open(os.devnull, 'w') as devnull:
            make_q_ret = subprocess.call(['make', '-f', 'scripts/legacy.make',
                                          '-q', 'lib', 'tests'],
                                         stdout=devnull, stderr=devnull)
        if make_q_ret != 0:
            subprocess.check_call(['make', '-f', 'scripts/legacy.make',
                                   'RECORD_PSA_STATUS_COVERAGE_LOG=1'],
                                  stdout=sys.stderr)
            rebuilt = True
        subprocess.check_call(['make', '-f', 'scripts/legacy.make', 'test'],
                              stdout=sys.stderr)
    data = Statuses()
    data.collect_log(options.log_file)
    data.get_constant_names(options.psa_constant_names)
    if rebuilt and options.clean_after:
        subprocess.check_call(['make', '-f', 'scripts/legacy.make', 'clean'],
                              cwd='tests',
                              stdout=sys.stderr)
    return data

def main():
    parser = argparse.ArgumentParser(description=globals()['__doc__'])
    parser.add_argument('--clean-after',
                        action='store_true',
                        help='Run "make clean" after rebuilding')
    parser.add_argument('--clean-before',
                        action='store_true',
                        help='Run "make clean" before regenerating the log file)')
    parser.add_argument('--log-file', metavar='FILE',
                        default=DEFAULT_STATUS_LOG_FILE,
                        help='Log file location (default: {})'.format(
                            DEFAULT_STATUS_LOG_FILE
                        ))
    parser.add_argument('--psa-constant-names', metavar='PROGRAM',
                        default=DEFAULT_PSA_CONSTANT_NAMES,
                        help='Path to psa_constant_names (default: {})'.format(
                            DEFAULT_PSA_CONSTANT_NAMES
                        ))
    parser.add_argument('--use-existing-log', '-e',
                        action='store_true',
                        help='Don\'t regenerate the log file if it exists')
    options = parser.parse_args()
    data = collect_status_logs(options)
    data.report()

if __name__ == '__main__':
    main()
