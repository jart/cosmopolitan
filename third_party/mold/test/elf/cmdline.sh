// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

{ ./mold -zfoo || true; } 2>&1 | grep -q 'unknown command line option: -zfoo'
{ ./mold -z foo || true; } 2>&1 | grep -q 'unknown command line option: -z foo'
{ ./mold -abcdefg || true; } 2>&1 | grep -q 'unknown command line option: -abcdefg'
{ ./mold --abcdefg || true; } 2>&1 | grep -q 'unknown command line option: --abcdefg'
