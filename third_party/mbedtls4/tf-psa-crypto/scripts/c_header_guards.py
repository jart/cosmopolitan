#!/usr/bin/env python3
"""
Script to check/fix header guards in C header files.

Usage: scripts/c_header_guards.py [--fix]

The script must be called from the TF-PSA-Crypto root folder.

If called without any additional parameter the script checks current guards
and in case of any mismatching with respect to the expected format it print
the errors that it found and it returns an error code.
If called with '--fix' input parameter, instead, it checks and modifies headers'
guards if needed.

The expected format for the headers' guards is as follows:
    1. "<rel_path>_<filename>_H" for public headers;
    2. "TF_PSA_CRYPTO_<rel_path>_<filename>_H" for private ones;
where:
- 'TF_PSA_CRYPTO_' is an hardcoded prefix;
- 'rel_path' is the path used in C files to include such header file.
"""

import sys
import os
import re
import argparse
from typing import List, Tuple, Match

import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework import build_tree

PREFIX = 'TF_PSA_CRYPTO_'

# Dictionary containing the folders where header files are to be fixed/checked:
# - key: path of header files to check;
# - value: path that must be trimmed from the key value in order to get the
#          final guard.
FOLDERS_TO_CHECK = {
    'drivers/builtin/include/mbedtls/': 'drivers/builtin/include/',
    'drivers/builtin/include/mbedtls/private/': 'drivers/builtin/include/',
    'drivers/builtin/src/': 'drivers/builtin/src/',
    'drivers/everest/include/tf-psa-crypto/private/everest/':
        'drivers/everest/include/tf-psa-crypto/private/everest/',
    'drivers/p256-m/': 'drivers/p256-m/',
    'drivers/p256-m/p256-m/': 'drivers/p256-m/',
    'include/mbedtls/': 'include/',
    'include/psa/': 'include/',
    'include/tf-psa-crypto/': 'include/',
    'core/': 'core/',
    'dispatch/': 'dispatch/',
    'extras/': 'extras/',
    'platform/': 'platform/',
    'utilities/': 'utilities/',
}

def generate_guard_name(file_path: str, trim_path: str) -> str:
    """Generate the new header guard."""
    adjusted_name = file_path
    # Trim prefix
    if adjusted_name.startswith(trim_path):
        adjusted_name = adjusted_name[len(trim_path):]
    # Remove suffix
    if adjusted_name.endswith('.h'):
        adjusted_name = adjusted_name[:-2]

    adjusted_name = re.sub(r'[^a-zA-Z0-9]', '_', adjusted_name).upper()
    guard_elements = []
    # Add prefix only to non public header files
    if not file_path.startswith('include'):
        guard_elements.append('TF_PSA_CRYPTO')
    guard_elements.extend([adjusted_name, 'H'])
    return '_'.join(guard_elements)

def find_line(lines: List[str], search_range, regex: str) -> Tuple[int, Match]:
    """Try to match the regex expression on the range of lines given in input.
    The first successful match is returned; an exception is returned if no
    match is found in all the given lines.

    Input params:
        lines: content of the file given as list of strings;
        search_range: range() to use when going through the list of lines;
        regex: the regex expression to be used for the match.

    Returns: a tuple where
        int: the first line where the match was successful;
        Match: the Match object returned on the matching line.
    """
    for i in search_range:
        stripped = lines[i].strip()
        match = re.match(regex, stripped)
        if match is not None:
            return i, match

    raise ValueError(f'Error: {regex} line not found')

def process_header(file_path, trim_path, fix) -> None:
    """Check/fix header guard in a single file.
    On failure an exception is thrown.
    """
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    correct_guard = generate_guard_name(file_path, trim_path)

    ifndef_line, match = find_line(lines, range(0, len(lines)),
                                   r'#ifndef\s+(\w+)\b')
    expected_line = f'#ifndef {correct_guard}'
    if expected_line not in lines[ifndef_line]:
        if fix:
            lines[ifndef_line] = expected_line
        else:
            raise ValueError(f'Invalid guard format:\n'
                             f'    Expected:{expected_line}'
                             f'    Found:{match.group(0)}')

    define_line, match = find_line(lines, range(ifndef_line, len(lines)),
                                   r'#define\s+(\w+)\b')
    expected_line = f'#define {correct_guard}'
    if expected_line not in lines[define_line]:
        if fix:
            lines[define_line] = expected_line
        else:
            raise ValueError(f'Invalid guard format:\n'
                             f'    Expected:{expected_line}'
                             f'    Found:{match.group(0)}')

    define_line, match = find_line(lines, range(len(lines) - 1, -1, -1),
                                   r'#endif\s*\/\*\s*([\w+.]+)\s*\*\/')
    expected_line = f'#endif /* {correct_guard} */'
    if expected_line not in lines[define_line]:
        if fix:
            lines[define_line] = expected_line
        else:
            raise ValueError(f'Invalid guard format:\n'
                             f'    Expected:{expected_line}'
                             f'    Found:{match.group(0)}')

    # Write back to file
    new_content = ''.join(lines)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(new_content)

def main():
    current_path = os.getcwd()

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('--fix', action='store_true',
                            help="Fix guards in header files. Default behavior is to check only.")
    args = arg_parser.parse_args()

    if not build_tree.looks_like_tf_psa_crypto_root(current_path):
        print("Error: this script must be launched from TF-PSA-Crypto root")
        sys.exit(1)

    had_failure = False
    for folder in FOLDERS_TO_CHECK:
        files = [filename for filename in os.listdir(folder) if re.match(r'.*\.h$', filename)]
        for file in files:
            full_file_path = os.path.join(folder, file)
            try:
                process_header(full_file_path, FOLDERS_TO_CHECK[folder], args.fix)
            except ValueError as e:
                print(f"Error processing {full_file_path}: {e}")
                had_failure = True

    if had_failure:
        sys.exit(1)

if __name__ == "__main__":
    main()
