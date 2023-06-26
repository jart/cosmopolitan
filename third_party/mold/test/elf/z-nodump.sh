// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -xc -
void foo() {}
EOF

$CC -B. -shared -o $t/b.so $t/a.o
! readelf --dynamic $t/b.so | grep -Eq 'Flags:.*NODUMP' || false

$CC -B. -shared -o $t/b.so $t/a.o -Wl,-z,nodump
readelf --dynamic $t/b.so | grep -Eq 'Flags:.*NODUMP'
