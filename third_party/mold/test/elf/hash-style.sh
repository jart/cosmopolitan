// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
void foo() {}
EOF

./mold -shared -o $t/b.so $t/a.o

readelf -WS $t/b.so | grep -Fq ' .hash'
readelf -WS $t/b.so | grep -Fq ' .gnu.hash'

./mold -shared -o $t/c.so $t/a.o --hash-style=both --hash-style=none

readelf -WS $t/c.so > $t/log
! grep -Fq ' .hash' $t/log || false
! grep -Fq ' .gnu.hash' $t/log || false
