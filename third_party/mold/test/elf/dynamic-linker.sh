// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -x assembler -
.globl _start
_start:
EOF

./mold -o $t/exe $t/a.o

readelf --sections $t/exe > $t/log
! grep -Fq .interp $t/log || false

readelf --dynamic $t/exe > $t/log

./mold -o $t/exe $t/a.o --dynamic-linker=/foo/bar

readelf --sections $t/exe > $t/log
grep -Fq .interp $t/log
