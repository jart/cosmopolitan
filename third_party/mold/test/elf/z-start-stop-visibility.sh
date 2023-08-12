// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

./mold -z start-stop-visibility=hidden --version > /dev/null
! ./mold -z start-stop-visibility=protected --version 2> $t/log
grep -q 'unsupported visibility: protected' $t/log
