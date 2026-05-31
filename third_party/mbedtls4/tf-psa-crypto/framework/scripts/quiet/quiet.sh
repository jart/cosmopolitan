# -*-mode: sh; sh-shell: bash -*-
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# This swallows the output of the wrapped tool, unless there is an error.
# This helps reduce excess logging in the CI.

# If you are debugging a build / CI issue, you can get complete unsilenced logs
# by un-commenting the following line (or setting VERBOSE_LOGS in your environment):
#
# VERBOSE_LOGS=1
#
# This script provides most of the functionality for the adjacent make and cmake
# wrappers.
#
# It requires two variables to be set:
#
# TOOL       - the name of the tool that is being wrapped (with no path), e.g. "make"
#
# NO_SILENCE - a regex that describes the commandline arguments for which output will not
#              be silenced, e.g. " --version | test ". In this example, "make lib test" will
#              not be silent, but "make lib" will be.

# Identify path to original tool. There is an edge-case here where the quiet wrapper is on the path via
# a symlink or relative path, but "type -ap" yields the wrapper with it's normalised path. We use
# the -ef operator to compare paths, to avoid picking the wrapper in this case (to avoid infinitely
# recursing).
while IFS= read -r ORIGINAL_TOOL; do
    if ! [[ $ORIGINAL_TOOL -ef "$0" ]]; then break; fi
done < <(type -ap -- "$TOOL")

print_quoted_args() {
    # similar to printf '%q' "$@"
    # but produce more human-readable results for common/simple cases like "a b"
    for a in "$@"; do
        # Get bash to quote the string
        printf -v q '%q' "$a"
        simple_pattern="^([-[:alnum:]_+./:@]+=)?([^']*)$"
        if [[ "$a" != "$q" && $a =~ $simple_pattern ]]; then
            # a requires some quoting (a != q), but has no single quotes, so we can
            # simplify the quoted form - e.g.:
            #   a b        -> 'a b'
            #   CFLAGS=a b -> CFLAGS='a b'
            q="${BASH_REMATCH[1]}'${BASH_REMATCH[2]}'"
        fi
        printf " %s" "$q"
    done
}

if [[ ! " $* " =~ " --version " ]]; then
    # Display the command being invoked - if it succeeds, this is all that will
    # be displayed. Don't do this for invocations with --version, because
    # this output is often parsed by scripts, so we don't want to modify it.
    printf %s "${TOOL}"    1>&2
    print_quoted_args "$@" 1>&2
    echo                   1>&2
fi

if [[ " $@ " =~ $NO_SILENCE || -n "${VERBOSE_LOGS}" ]]; then
    # Run original command with no output supression
    exec "${ORIGINAL_TOOL}" "$@"
else
    # Run original command and capture output & exit status
    TMPFILE=$(mktemp "quiet-${TOOL}.XXXXXX")
    "${ORIGINAL_TOOL}" "$@" > "${TMPFILE}" 2>&1
    EXIT_STATUS=$?

    if [[ $EXIT_STATUS -ne 0 ]]; then
        # On error, display the full output
        cat "${TMPFILE}"
    fi

    # Remove tmpfile
    rm "${TMPFILE}"

    # Propagate the exit status
    exit $EXIT_STATUS
fi
