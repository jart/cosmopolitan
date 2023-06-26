// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -o $t/a.o -c -xc -
void foo() {}
EOF

$CC -B. -o $t/b.so -shared -Wl,-defsym=bar=foo $t/a.o
nm -D $t/b.so | grep -q 'bar' || false
