#!/bin/sh

# This script runs tests before and after a PR and analyzes the results in
# order to highlight any difference in the set of tests skipped.
#
# It can be used to check for unintended consequences when making non-trivial
# changes to compile time guards: the sets of tests skipped in the default
# config and the full config must be the same before and after the PR.
#
# USAGE:
# - First, commit any uncommited changes. (Also, see warning below.)
# - Then launch --> [SKIP_SSL_OPT=1] docs/architecture/psa-migration/outcome-analysis.sh
#     - SKIP_SSL_OPT=1 can optionally be set to skip ssl-opt.sh tests
#
# WARNING: this script checks out a commit other than the head of the current
# branch; it checks out the current branch again when running successfully,
# but while the script is running, or if it terminates early in error, you
# should be aware that you might be at a different commit than expected.
#
# NOTE: you can comment out parts that don't need to be re-done when
# re-running this script (for example "get numbers before this PR").

set -eu

: ${SKIP_SSL_OPT:=0}

cleanup() {
    make clean
    git checkout -- include/mbedtls/mbedtls_config.h tf-psa-crypto/include/psa/crypto_config.h
}

record() {
    export MBEDTLS_TEST_OUTCOME_FILE="$PWD/outcome-$1.csv"
    rm -f $MBEDTLS_TEST_OUTCOME_FILE

    make check

    if [ $SKIP_SSL_OPT -eq 0 ]; then
        make -C programs ssl/ssl_server2 ssl/ssl_client2 \
            test/udp_proxy test/query_compile_time_config
        tests/ssl-opt.sh
    fi
}

# save current HEAD.
# Note: this can optionally be updated to
#   HEAD=$(git branch --show-current)
# when using a Git version above 2.22
HEAD=$(git rev-parse --abbrev-ref HEAD)

# get the numbers before this PR for default and full
cleanup
git checkout $(git merge-base HEAD development)

record "before-default"

cleanup

scripts/config.py full
record "before-full"

# get the numbers now for default and full
cleanup
git checkout $HEAD

record "after-default"

cleanup

scripts/config.py full
record "after-full"

cleanup

# analysis

populate_suites () {
    SUITES=''
    make generated_files >/dev/null
    data_files=$(cd tests/suites && echo *.data)
    for data in $data_files; do
        suite=${data%.data}
        SUITES="$SUITES $suite"
    done
    make neat

    if [ $SKIP_SSL_OPT -eq 0 ]; then
        SUITES="$SUITES ssl-opt"
        extra_files=$(cd tests/opt-testcases && echo *.sh)
        for extra in $extra_files; do
            suite=${extra%.sh}
            SUITES="$SUITES $suite"
        done
    fi
}

compare_suite () {
    ref="outcome-$1.csv"
    new="outcome-$2.csv"
    suite="$3"

    pattern_suite=";$suite;"
    total=$(grep -c "$pattern_suite" "$ref")
    sed_cmd="s/^.*$pattern_suite\(.*\);SKIP.*/\1/p"
    sed -n "$sed_cmd" "$ref" > skipped-ref
    sed -n "$sed_cmd" "$new" > skipped-new
    nb_ref=$(wc -l <skipped-ref)
    nb_new=$(wc -l <skipped-new)

    name=${suite#test_suite_}
    printf "%40s: total %4d; skipped %4d -> %4d\n" \
            $name       $total       $nb_ref $nb_new
    if diff skipped-ref skipped-new | grep '^> '; then
        ret=1
    else
        ret=0
    fi
    rm skipped-ref skipped-new
    return $ret
}

compare_builds () {
    printf "\n*** Comparing $1 -> $2 ***\n"
    failed=''
    for suite in $SUITES; do
        if compare_suite "$1" "$2" "$suite"; then :; else
            failed="$failed $suite"
        fi
    done
    if [ -z "$failed" ]; then
        printf "No coverage gap found.\n"
    else
        printf "Suites with less coverage:%s\n" "$failed"
    fi
}

populate_suites
compare_builds before-default after-default
compare_builds before-full after-full
