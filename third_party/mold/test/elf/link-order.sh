// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
void foo() {}
EOF

$CC -B. -shared -o $t/libfoo.so $t/a.o
ar crs $t/libfoo.a $t/a.o

cat <<EOF | $CC -c -o $t/b.o -xc -
void foo();
int main() {
  foo();
}
EOF

$CC -B. -o $t/exe $t/b.o -Wl,--as-needed $t/libfoo.so $t/libfoo.a
readelf --dynamic $t/exe | grep -q libfoo

$CC -B. -o $t/exe $t/b.o -Wl,--as-needed $t/libfoo.a $t/libfoo.so
! readelf --dynamic $t/exe | grep -q libfoo || false
