// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

./mold -z no-such-opt 2>&1 | grep -q 'unknown command line option: -z no-such-opt'
./mold -zno-such-opt 2>&1 | grep -q 'unknown command line option: -zno-such-opt'
