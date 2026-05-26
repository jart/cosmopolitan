#!/usr/bin/env python3
"""Run the Mbed TLS demo scripts.
"""
import argparse
import glob
import subprocess
import sys

def run_demo(demo, quiet=False):
    """Run the specified demo script. Return True if it succeeds."""
    args = {}
    if quiet:
        args['stdout'] = subprocess.DEVNULL
        args['stderr'] = subprocess.DEVNULL
    returncode = subprocess.call([demo], **args)
    return returncode == 0

def run_demos(demos, quiet=False):
    """Run the specified demos and print summary information about failures.

    Return True if all demos passed and False if a demo fails.
    """
    failures = []
    for demo in demos:
        if not quiet:
            print('#### {} ####'.format(demo))
        success = run_demo(demo, quiet=quiet)
        if not success:
            failures.append(demo)
            if not quiet:
                print('{}: FAIL'.format(demo))
        if quiet:
            print('{}: {}'.format(demo, 'PASS' if success else 'FAIL'))
        else:
            print('')
    successes = len(demos) - len(failures)
    print('{}/{} demos passed'.format(successes, len(demos)))
    if failures and not quiet:
        print('Failures:', *failures)
    return not failures

def run_all_demos(quiet=False):
    """Run all the available demos.

    Return True if all demos passed and False if a demo fails.
    """
    mbedtls_demos = glob.glob('programs/*/*_demo.sh')
    tf_psa_crypto_demos = glob.glob('tf-psa-crypto/programs/*/*_demo.sh')
    all_demos = mbedtls_demos + tf_psa_crypto_demos
    if not all_demos:
        # Keep the message on one line. pylint: disable=line-too-long
        raise Exception('No demos found. run_demos needs to operate from the Mbed TLS toplevel directory.')
    return run_demos(all_demos, quiet=quiet)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--quiet', '-q',
                        action='store_true',
                        help="suppress the output of demos")
    options = parser.parse_args()
    success = run_all_demos(quiet=options.quiet)
    sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()
