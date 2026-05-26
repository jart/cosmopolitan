#!/usr/bin/env python3

"""
Check that files with lists of config options are up-to-date, or update them.

This script checks the following file:
scripts/data_files/config-options-current.txt
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import sys

import scripts_path # pylint: disable=unused-import
from mbedtls_framework import config_macros


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    # For now this script only acts on one target file.
    # If we check/update more files, we should add a way to select which
    # file(s) to operate on.
    parser.add_argument('--always-update', '-U',
                        action='store_true',
                        help=('Update target files unconditionally '
                              '(overrides --update)'))
    parser.add_argument('--update', '-u',
                        action='store_true',
                        help='Update target files if needed')
    args = parser.parse_args()
    data = config_macros.Current(shadow_missing_ok=True)
    if args.update or args.always_update:
        data.update_shadow_file(args.always_update)
    else:
        up_to_date = True
        if not data.is_shadow_file_up_to_date():
            print(f'{data.shadow_file_path()} is out of date')
            print(f'After adding or removing a config option, you need to run')
            print(f'{sys.argv[0]} -u and commit the result.')
            up_to_date = False
        sys.exit(0 if up_to_date else 1)

if __name__ == "__main__":
    main()
