// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -o $t/a.o -c -x assembler -
.globl main
main:
EOF

$CC -B. -o $t/exe $t/a.o
readelf --notes $t/exe > $t/log
! grep -qw SHSTK $t/log

$CC -B. -o $t/exe $t/a.o -Wl,-z,shstk
readelf --notes $t/exe | grep -qw SHSTK
