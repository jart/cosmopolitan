#! /usr/bin/env sh

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# Purpose: check Python files for potential programming errors or maintenance
# hurdles. Run pylint to detect some potential mistakes and enforce PEP8
# coding standards. Run mypy to perform static type checking.

# We'll keep going on errors and report the status at the end.
ret=0

if type python3 >/dev/null 2>/dev/null; then
    PYTHON=python3
else
    PYTHON=python
fi

check_version () {
    $PYTHON - "$2" <<EOF
import packaging.version
import sys
import $1 as package
actual = package.__version__
wanted = sys.argv[1]
if packaging.version.parse(actual) < packaging.version.parse(wanted):
    sys.stderr.write("$1: version %s is too old (want %s)\n" % (actual, wanted))
    exit(1)
EOF
}

can_pylint () {
    # Pylint 1.5.2 from Ubuntu 16.04 is too old:
    #     E: 34, 0: Unable to import 'mbedtls_framework' (import-error)
    # Pylint 1.8.3 from Ubuntu 18.04 passed on the first commit containing this line.
    check_version pylint 1.8.3
}

can_mypy () {
    # mypy 0.770 is too old:
    #     framework/scripts/test_psa_constant_names.py:34: error: Cannot find implementation or library stub for module named 'mbedtls_framework'
    # mypy 0.780 from pip passed on the first commit containing this line.
    check_version mypy.version 0.780
}

# With just a --can-xxx option, check whether the tool for xxx is available
# with an acceptable version, and exit without running any checks. The exit
# status is true if the tool is available and acceptable and false otherwise.
if [ "$1" = "--can-pylint" ]; then
    can_pylint
    exit
elif [ "$1" = "--can-mypy" ]; then
    can_mypy
    exit
fi

echo 'Running pylint ...'
# When we move Python code between repositories, there is a transition
# period during which code is duplicated between the old repository and
# the new repository.
# Pylint looks for duplicate code inside files that are mentioned in the
# same invocation. When we move some code from A to B, we want to skip
# duplicate-code checks between A and B. So we arrange for two separate
# runs of pylint: one for the A files, and one for the others.
# Remove exceptions below once the A file (or the moved code in the A file)
# has been removed from all consuming branches.
find framework/scripts scripts tests/scripts -name '*.py' \( \
     ! -path scripts/abi_check.py \
     ! -path scripts/code_size_compare.py \
     ! -path scripts/ecp_comb_table.py \
     ! -path tests/scripts/audit-validity-dates.py \
     ! -path tests/scripts/generate_server9_bad_saltlen.py \
     ! -path tests/scripts/psa_collect_statuses.py \
     ! -path tests/scripts/run_demos.py \
     ! -path tests/scripts/test_config_script.py \
     ! -path framework/scripts/make_generated_files.py \
        -exec $PYTHON -m pylint {} + \
     -o -exec $PYTHON -m pylint {} + \) || {
    echo >&2 "pylint reported errors"
    ret=1
}

echo
echo 'Running mypy ...'
$PYTHON -m mypy framework/scripts || {
    echo >&2 "mypy reported errors in the framework"
    ret=1
}

$PYTHON -m mypy scripts tests/scripts || {
    echo >&2 "mypy reported errors in the parent repository"
    ret=1
}

exit $ret
