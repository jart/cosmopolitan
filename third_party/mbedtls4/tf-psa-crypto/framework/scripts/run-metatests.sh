#!/bin/sh

help () {
    cat <<EOF
Usage: $0 [OPTION] [PLATFORM]...
Run all the metatests whose platform matches any of the given PLATFORM.
A PLATFORM can contain shell wildcards.

Expected output: a lot of scary-looking error messages, since each
metatest is expected to report a failure. The final line should be
"Ran N metatests, all good."

If something goes wrong: the final line should be
"Ran N metatests, X unexpected successes". Look for "Unexpected success"
in the logs above.

  -l  List the available metatests, don't run them.
EOF
}

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

set -e -u

if [ -d programs ]; then
    METATEST_PROGRAM=programs/test/metatest
elif [ -d ../programs ]; then
    METATEST_PROGRAM=../programs/test/metatest
elif [ -d ../../programs ]; then
    METATEST_PROGRAM=../../programs/test/metatest
else
    echo >&2 "$0: FATAL: programs/test/metatest not found"
    exit 120
fi

LIST_ONLY=
while getopts hl OPTLET; do
    case $OPTLET in
        h) help; exit;;
        l) LIST_ONLY=1;;
        \?) help >&2; exit 120;;
    esac
done
shift $((OPTIND - 1))

list_matches () {
    while read name platform junk; do
        for pattern in "$@"; do
            case $platform in
                $pattern) echo "$name"; break;;
            esac
        done
    done
}

count=0
errors=0
run_metatest () {
    ret=0
    "$METATEST_PROGRAM" "$1" || ret=$?
    if [ $ret -eq 0 ]; then
        echo >&2 "$0: Unexpected success: $1"
        errors=$((errors + 1))
    fi
    count=$((count + 1))
}

# Don't pipe the output of metatest so that if it fails, this script exits
# immediately with a failure status.
full_list=$("$METATEST_PROGRAM" list)
matching_list=$(printf '%s\n' "$full_list" | list_matches "$@")

if [ -n "$LIST_ONLY" ]; then
    printf '%s\n' $matching_list
    exit
fi

for name in $matching_list; do
    run_metatest "$name"
done

if [ $errors -eq 0 ]; then
    echo "Ran $count metatests, all good."
    exit 0
else
    echo "Ran $count metatests, $errors unexpected successes."
    exit 1
fi
