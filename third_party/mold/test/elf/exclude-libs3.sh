// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -xc -c -o $t/a.o -
void foo();
void bar() { foo(); }
EOF

rm -f $t/b.a
ar crs $t/b.a $t/a.o

cat <<EOF | $CC -fPIC -xc -c -o $t/c.o -
void bar();
void foo() { bar(); }
EOF

$CC -B. -shared -o $t/d.so $t/c.o $t/b.a -Wl,-exclude-libs=ALL
readelf --dyn-syms $t/d.so > $t/log
grep -Fq foo $t/log
