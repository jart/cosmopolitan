// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
void foo1() {}
void foo2() {}

__asm__(".symver foo1, bar1@");
__asm__(".symver foo2, bar2@@");
EOF

$CC -B. -shared -o $t/b.so $t/a.o

readelf --dyn-syms $t/b.so | grep -q 'bar1$'
readelf --dyn-syms $t/b.so | grep -q 'bar2$'
