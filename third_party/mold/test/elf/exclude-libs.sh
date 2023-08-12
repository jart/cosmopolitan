// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -xc -c -o $t/a.o -
int foo() {
  return 3;
}
EOF

cat <<EOF | $CC -fPIC -xc -c -o $t/b.o -
int bar() {
  return 5;
}
EOF

rm -f $t/c.a
ar crs $t/c.a $t/a.o

rm -f $t/d.a
ar crs $t/d.a $t/b.o

cat <<EOF | $CC -fPIC -xc -c -o $t/e.o -
int foo();
int bar();

int baz() {
  foo();
  bar();
  return 0;
}
EOF

$CC -B. -shared -o $t/f.so $t/e.o $t/c.a $t/d.a
readelf --dyn-syms $t/f.so > $t/log
grep -Fq foo $t/log
grep -Fq bar $t/log
grep -Fq baz $t/log

$CC -B. -shared -o $t/f.so $t/e.o $t/c.a $t/d.a -Wl,-exclude-libs=c.a
readelf --dyn-syms $t/f.so > $t/log
! grep -Fq foo $t/log || false
grep -Fq bar $t/log
grep -Fq baz $t/log

$CC -B. -shared -o $t/f.so $t/e.o $t/c.a $t/d.a -Wl,-exclude-libs=c.a -Wl,-exclude-libs=d.a
readelf --dyn-syms $t/f.so > $t/log
! grep -Fq foo $t/log || false
! grep -Fq bar $t/log || false
grep -Fq baz $t/log

$CC -B. -shared -o $t/f.so $t/e.o $t/c.a $t/d.a -Wl,-exclude-libs=ALL
readelf --dyn-syms $t/f.so > $t/log
! grep -Fq foo $t/log || false
! grep -Fq bar $t/log || false
grep -Fq baz $t/log
