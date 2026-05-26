#!/usr/bin/env python3

"""Sanity checks for test data."""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import re
import sys

from mbedtls_framework import collect_test_cases


class DescriptionChecker(collect_test_cases.TestDescriptionExplorer):
    """Check all test case descriptions.

* Check that each description is valid (length, allowed character set, etc.).
* Check that there is no duplicated description inside of one test suite.
"""

    def __init__(self, results):
        self.results = results

    def new_per_file_state(self):
        """Dictionary mapping descriptions to their line number."""
        return {}

    def process_test_case(self, per_file_state,
                          file_name, line_number, description):
        """Check test case descriptions for errors."""
        results = self.results
        seen = per_file_state
        if description in seen:
            results.error(file_name, line_number,
                          'Duplicate description (also line {})',
                          seen[description])
            return
        if re.search(br'[\t;]', description):
            results.error(file_name, line_number,
                          'Forbidden character \'{}\' in description',
                          re.search(br'[\t;]', description).group(0).decode('ascii'))
        if re.search(br'[^ -~]', description):
            results.error(file_name, line_number,
                          'Non-ASCII character in description')
        if len(description) > 66:
            results.warning(file_name, line_number,
                            'Test description too long ({} > 66)',
                            len(description))
        seen[description] = line_number

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--list-all',
                        action='store_true',
                        help='List all test cases, without doing checks')
    parser.add_argument('--quiet', '-q',
                        action='store_true',
                        help='Hide warnings')
    parser.add_argument('--verbose', '-v',
                        action='store_false', dest='quiet',
                        help='Show warnings (default: on; undoes --quiet)')
    options = parser.parse_args()
    if options.list_all:
        descriptions = collect_test_cases.collect_available_test_cases()
        sys.stdout.write('\n'.join(descriptions + ['']))
        return
    results = collect_test_cases.Results(options)
    checker = DescriptionChecker(results)
    try:
        checker.walk_all()
    except collect_test_cases.ScriptOutputError as e:
        results.error(e.script_name, e.idx,
                      '"{}" should be listed as "<suite_name>;<description>"',
                      e.line)
    if (results.warnings or results.errors) and not options.quiet:
        sys.stderr.write('{}: {} errors, {} warnings\n'
                         .format(sys.argv[0], results.errors, results.warnings))
    sys.exit(1 if results.errors else 0)

if __name__ == '__main__':
    main()
