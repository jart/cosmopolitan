// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
void foo() {}
EOF

$CC -B. -o $t/b.so -shared $t/a.o -Wl,-default-symver
readelf --dyn-syms $t/b.so | grep -q ' foo@@b\.so$'

$CC -B. -o $t/b.so -shared $t/a.o \
  -Wl,--soname=bar -Wl,-default-symver
readelf --dyn-syms $t/b.so | grep -q ' foo@@bar$'
