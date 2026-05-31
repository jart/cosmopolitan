#!/bin/sh

usage () {
    cat <<EOF
Usage: $0 [OPTION]... COMMIT VERSION
Save historical information about config options.

Record information for the git commit COMMIT (tag, sha or any other refspec).
Store the list of config options and internal macros in file names indicating
the version VERSION.

  -C DIR    Top-level directory where the git commit can be found.
            Default: $project_root
  -o DIR    Directory for the output files. It must exist.
            Default: $history_dir
EOF
}

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

set -eu

framework_root=$(cd "$(dirname "$0")"/.. && pwd)

project_root=${framework_root%/*}
history_dir=$framework_root/history

if [ $# -ne 0 ] && [ "$1" = "--help" ]; then
    usage
    exit 0
fi

while getopts C:o: OPTLET; do
    case $OPTLET in
        C) project_root=$OPTARG;;
        o) history_dir=$OPTARG;;
        *) usage >&2; exit 120;;
    esac
done
shift $((OPTIND - 1))

if [ $# -ne 2 ]; then
    echo >&2 "This script requires exactly two non-option arguments: COMMIT VERSION"
    usage >&2
    exit 120
fi
commit=$1
version=$2

# Assert that the git commit exists
git -C "$project_root" cat-file -e "${commit}^{commit}"

if git -C "$project_root" merge-base --is-ancestor 0679e3a841c3317dc1f4a0faacc350bc91662b04 "$commit"; then
    product=tfpsacrypto
else
    product=mbedtls
fi

temp_file=$(mktemp)
trap 'rm -f "$temp_file"' EXIT INT TERM

## collect_macros VARIANT FILENAME...
## Find the macros defined in the given files. Save the result in
## $history_dir/config-VARIANT-*.txt.
collect_macros () {
    output_file="$history_dir/config-$1-$product-$version.txt"
    shift
    : >"$temp_file"
    for header in "$@"; do
        git -C "$project_root" show "$commit:$header" >>"$temp_file"
    done
    sed -n 's![/ ]*# *define  *\([A-Z_a-z][0-9A-Z_a-z]*\).*!\1!p' <"$temp_file" |
        sort -u >"$output_file"
    if [ ! -s "$output_file" ]; then
        echo >&2 "$0: Failed to find any config option"
        exit 1
    fi
}


collect_macros options $(git -C "$project_root" ls-tree -r --name-only "$commit" \
                             include/mbedtls/mbedtls_config.h include/psa/crypto_config.h)
collect_macros adjust $(git -C "$project_root" ls-tree -r --name-only "$commit" |
                        grep '_adjust.*\.h$')
