"""Run the PSA Crypto API compliance test suite.
Clone the repo and check out the commit specified by PSA_ARCH_TEST_REPO and PSA_ARCH_TEST_REF,
then compile and run the test suite. The clone is stored at <repository root>/psa-arch-tests.
Known defects in either the test suite or mbedtls / TF-PSA-Crypto - identified by their test
number - are ignored, while unexpected failures AND successes are reported as errors, to help
keep the list of known defects as up to date as possible.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import glob
import os
import re
import shutil
import subprocess
import sys
from typing import List, Optional
from pathlib import Path

from . import build_tree

PSA_ARCH_TESTS_REPO = 'https://github.com/ARM-software/psa-arch-tests.git'

#pylint: disable=too-many-branches,too-many-statements,too-many-locals
def test_compliance(library_build_dir: str,
                    psa_arch_tests_ref: str,
                    patch_files: List[str],
                    expected_failures: List[int]) -> int:
    """Check out and run compliance tests.

    library_build_dir: path where our library will be built.
    psa_arch_tests_ref: tag or sha to use for the arch-tests.
    patch: patch to apply to the arch-tests with ``patch -p1``.
    expected_failures: default list of expected failures.
    """
    root_dir = os.getcwd()
    install_dir = Path(library_build_dir + "/install_dir").resolve()
    tmp_env = os.environ
    tmp_env['CC'] = 'gcc'
    subprocess.check_call(['cmake', '.', '-GUnix Makefiles',
                           '-B' + library_build_dir,
                           '-DCMAKE_INSTALL_PREFIX=' + str(install_dir)],
                          env=tmp_env)
    subprocess.check_call(['cmake', '--build', library_build_dir, '--target', 'install'])

    if build_tree.is_mbedtls_3_6():
        crypto_library_path = install_dir.joinpath("lib/libmbedcrypto.a")
    else:
        crypto_library_path = install_dir.joinpath("lib/libtfpsacrypto.a")

    psa_arch_tests_dir = 'psa-arch-tests'
    os.makedirs(psa_arch_tests_dir, exist_ok=True)
    try:
        os.chdir(psa_arch_tests_dir)

        # Reuse existing local clone
        subprocess.check_call(['git', 'init'])
        subprocess.check_call(['git', 'fetch', PSA_ARCH_TESTS_REPO, psa_arch_tests_ref])
        subprocess.check_call(['git', 'checkout', '--force', 'FETCH_HEAD'])

        if patch_files:
            subprocess.check_call(['git', 'reset', '--hard'])
        for patch_file in patch_files:
            with open(os.path.join(root_dir, patch_file), 'rb') as patch:
                subprocess.check_call(['patch', '-p1'],
                                      stdin=patch)

        build_dir = 'api-tests/build'
        try:
            shutil.rmtree(build_dir)
        except FileNotFoundError:
            pass
        os.mkdir(build_dir)
        os.chdir(build_dir)

        #pylint: disable=bad-continuation
        subprocess.check_call([
            'cmake', '..',
                     '-GUnix Makefiles',
                     '-DTARGET=tgt_dev_apis_stdc',
                     '-DTOOLCHAIN=HOST_GCC',
                     '-DSUITE=CRYPTO',
                     '-DPSA_CRYPTO_LIB_FILENAME={}'.format(str(crypto_library_path)),
                     '-DPSA_INCLUDE_PATHS=' + str(install_dir.joinpath("include"))
        ])

        subprocess.check_call(['cmake', '--build', '.'])

        proc = subprocess.Popen(['./psa-arch-tests-crypto'],
                                bufsize=1, stdout=subprocess.PIPE, universal_newlines=True)

        test_re = re.compile(
            '^TEST: (?P<test_num>[0-9]*)|'
            '^TEST RESULT: (?P<test_result>FAILED|PASSED)'
        )
        test = -1
        unexpected_successes = expected_failures.copy()
        expected_failures.clear()
        unexpected_failures = [] # type: List[int]
        if proc.stdout is None:
            return 1

        for line in proc.stdout:
            print(line, end='')
            match = test_re.match(line)
            if match is not None:
                groupdict = match.groupdict()
                test_num = groupdict['test_num']
                if test_num is not None:
                    test = int(test_num)
                elif groupdict['test_result'] == 'FAILED':
                    try:
                        unexpected_successes.remove(test)
                        expected_failures.append(test)
                        print('Expected failure, ignoring')
                    except KeyError:
                        unexpected_failures.append(test)
                        print('ERROR: Unexpected failure')
                elif test in unexpected_successes:
                    print('ERROR: Unexpected success')
        proc.wait()

        print()
        print('***** test_psa_compliance.py report ******')
        print()
        print('Expected failures:', ', '.join(str(i) for i in expected_failures))
        print('Unexpected failures:', ', '.join(str(i) for i in unexpected_failures))
        print('Unexpected successes:', ', '.join(str(i) for i in sorted(unexpected_successes)))
        print()
        if unexpected_successes or unexpected_failures:
            if unexpected_successes:
                print('Unexpected successes encountered.')
                print('Please remove the corresponding tests from '
                      'EXPECTED_FAILURES in tests/scripts/compliance_test.py')
                print()
            print('FAILED')
            return 1
        else:
            print('SUCCESS')
            return 0
    finally:
        os.chdir(root_dir)

def main(psa_arch_tests_ref: str,
         expected_failures: Optional[List[int]] = None) -> None:
    """Command line entry point.

    psa_arch_tests_ref: tag or sha to use for the arch-tests.
    expected_failures: default list of expected failures.
    """
    build_dir = 'out_of_source_build'
    default_patch_directory = os.path.join(build_tree.guess_project_root(),
                                           'scripts/data_files/psa-arch-tests')

    # pylint: disable=invalid-name
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-dir', nargs=1,
                        help='path to Mbed TLS / TF-PSA-Crypto build directory')
    parser.add_argument('--expected-failures', nargs='+',
                        help='''set the list of test codes which are expected to fail
                                from the command line. If omitted the list given by
                                EXPECTED_FAILURES (inside the script) is used.''')
    parser.add_argument('--patch-directory', nargs=1,
                        default=default_patch_directory,
                        help='Directory containing patches (*.patch) to apply to psa-arch-tests')
    args = parser.parse_args()

    if args.build_dir is not None:
        build_dir = args.build_dir[0]

    if expected_failures is None:
        expected_failures = []
    if args.expected_failures is not None:
        expected_failures_list = [int(i) for i in args.expected_failures]
    else:
        expected_failures_list = expected_failures

    if args.patch_directory:
        patch_file_glob = os.path.join(args.patch_directory, '*.patch')
        patch_files = sorted(glob.glob(patch_file_glob))
    else:
        patch_files = []

    sys.exit(test_compliance(build_dir,
                             psa_arch_tests_ref,
                             patch_files,
                             expected_failures_list))
