#!/usr/bin/env python3

"""Test helper for the Mbed TLS configuration file tool

Run config.py with various parameters and write the results to files.

This is a harness to help regression testing, not a functional tester.
Sample usage:

    test_config_script.py -d old
    ## Modify config.py and/or mbedtls_config.h ##
    test_config_script.py -d new
    diff -ru old new
"""

## Copyright The Mbed TLS Contributors
## SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
##

import argparse
import glob
import os
import re
import shutil
import subprocess

OUTPUT_FILE_PREFIX = 'config-'

def output_file_name(directory, stem, extension):
    return os.path.join(directory,
                        '{}{}.{}'.format(OUTPUT_FILE_PREFIX,
                                         stem, extension))

def cleanup_directory(directory):
    """Remove old output files."""
    for extension in []:
        pattern = output_file_name(directory, '*', extension)
        filenames = glob.glob(pattern)
        for filename in filenames:
            os.remove(filename)

def prepare_directory(directory):
    """Create the output directory if it doesn't exist yet.

    If there are old output files, remove them.
    """
    if os.path.exists(directory):
        cleanup_directory(directory)
    else:
        os.makedirs(directory)

def guess_presets_from_help(help_text):
    """Figure out what presets the script supports.

    help_text should be the output from running the script with --help.
    """
    # Try the output format from config.py
    hits = re.findall(r'\{([-\w,]+)\}', help_text)
    for hit in hits:
        words = set(hit.split(','))
        if 'get' in words and 'set' in words and 'unset' in words:
            words.remove('get')
            words.remove('set')
            words.remove('unset')
            return words
    # Try the output format from config.pl
    hits = re.findall(r'\n +([-\w]+) +- ', help_text)
    if hits:
        return hits
    raise Exception("Unable to figure out supported presets. Pass the '-p' option.")

def list_presets(options):
    """Return the list of presets to test.

    The list is taken from the command line if present, otherwise it is
    extracted from running the config script with --help.
    """
    if options.presets:
        return re.split(r'[ ,]+', options.presets)
    else:
        help_text = subprocess.run([options.script, '--help'],
                                   check=False, # config.pl --help returns 255
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.STDOUT).stdout
        return guess_presets_from_help(help_text.decode('ascii'))

def run_one(options, args, stem_prefix='', input_file=None):
    """Run the config script with the given arguments.

    Take the original content from input_file if specified, defaulting
    to options.input_file if input_file is None.

    Write the following files, where xxx contains stem_prefix followed by
    a filename-friendly encoding of args:
    * config-xxx.h: modified file.
    * config-xxx.out: standard output.
    * config-xxx.err: standard output.
    * config-xxx.status: exit code.

    Return ("xxx+", "path/to/config-xxx.h") which can be used as
    stem_prefix and input_file to call this function again with new args.
    """
    if input_file is None:
        input_file = options.input_file
    stem = stem_prefix + '-'.join(args)
    data_filename = output_file_name(options.output_directory, stem, 'h')
    stdout_filename = output_file_name(options.output_directory, stem, 'out')
    stderr_filename = output_file_name(options.output_directory, stem, 'err')
    status_filename = output_file_name(options.output_directory, stem, 'status')
    shutil.copy(input_file, data_filename)
    # Pass only the file basename, not the full path, to avoid getting the
    # directory name in error messages, which would make comparisons
    # between output directories more difficult.
    cmd = [os.path.abspath(options.script),
           '-f', os.path.basename(data_filename)]
    with open(stdout_filename, 'wb') as out:
        with open(stderr_filename, 'wb') as err:
            status = subprocess.call(cmd + args,
                                     cwd=options.output_directory,
                                     stdin=subprocess.DEVNULL,
                                     stdout=out, stderr=err)
    with open(status_filename, 'w') as status_file:
        status_file.write('{}\n'.format(status))
    return stem + "+", data_filename

### A list of symbols to test with.
### This script currently tests what happens when you change a symbol from
### having a value to not having a value or vice versa. This is not
### necessarily useful behavior, and we may not consider it a bug if
### config.py stops handling that case correctly.
TEST_SYMBOLS = [
    'CUSTOM_SYMBOL', # does not exist
    'PSA_WANT_KEY_TYPE_AES', # set, no value
    'MBEDTLS_MPI_MAX_SIZE', # unset, has a value
    'MBEDTLS_NO_UDBL_DIVISION', # unset, in "System support"
    'MBEDTLS_PLATFORM_ZEROIZE_ALT', # unset, in "Customisation configuration options"
]

def run_all(options):
    """Run all the command lines to test."""
    presets = list_presets(options)
    for preset in presets:
        run_one(options, [preset])
    for symbol in TEST_SYMBOLS:
        run_one(options, ['get', symbol])
        (stem, filename) = run_one(options, ['set', symbol])
        run_one(options, ['get', symbol], stem_prefix=stem, input_file=filename)
        run_one(options, ['--force', 'set', symbol])
        (stem, filename) = run_one(options, ['set', symbol, 'value'])
        run_one(options, ['get', symbol], stem_prefix=stem, input_file=filename)
        run_one(options, ['--force', 'set', symbol, 'value'])
        run_one(options, ['unset', symbol])

def main():
    """Command line entry point."""
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-d', metavar='DIR',
                        dest='output_directory', required=True,
                        help="""Output directory.""")
    parser.add_argument('-f', metavar='FILE',
                        dest='input_file', default='include/mbedtls/mbedtls_config.h',
                        help="""Config file (default: %(default)s).""")
    parser.add_argument('-p', metavar='PRESET,...',
                        dest='presets',
                        help="""Presets to test (default: guessed from --help).""")
    parser.add_argument('-s', metavar='FILE',
                        dest='script', default='scripts/config.py',
                        help="""Configuration script (default: %(default)s).""")
    options = parser.parse_args()
    prepare_directory(options.output_directory)
    run_all(options)

if __name__ == '__main__':
    main()
