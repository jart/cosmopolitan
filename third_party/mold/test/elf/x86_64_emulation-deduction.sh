// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Skip if target is not x86-64
[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -o $t/a.o -c -xc -
void _start() {}
EOF

./mold -o $t/exe $t/a.o
readelf --file-header $t/exe | grep -qi x86-64
