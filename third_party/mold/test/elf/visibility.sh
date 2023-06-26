// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -xc -c -o $t/a.o -
__attribute__((visibility("hidden"))) int foo = 3;
EOF

cat <<EOF | $CC -xc -c -o $t/b.o -
__attribute__((visibility("hidden"))) int foo = 3;
int bar = 5;
EOF

rm -f $t/c.a
ar crs $t/c.a $t/a.o $t/b.o

cat <<EOF | $CC -xc -fPIC -c -o $t/d.o -
extern int bar;
int main() { return bar; }
EOF

$CC -B. -shared -o $t/e.so $t/c.a $t/d.o
readelf --dyn-syms $t/e.so > $t/log
! grep -Fq foo $t/log || false
