// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc - -fno-PIE
void _start() {}
EOF

./mold -o $t/exe $t/a.o

readelf -W --sections $t/exe > $t/log
! grep -Fq ' .dynsym ' $t/log || false
! grep -Fq ' .dynstr ' $t/log || false
