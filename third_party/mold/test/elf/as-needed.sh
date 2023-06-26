// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
void fn1();
int main() {
  fn1();
}
EOF

cat <<EOF | $CC -o $t/b.so -shared -fPIC -Wl,-soname,libfoo.so -xc -
int fn1() { return 42; }
EOF

cat <<EOF | $CC -o $t/c.so -shared -fPIC -Wl,-soname,libbar.so -xc -
int fn2() { return 42; }
EOF

$CC -B. -o $t/exe $t/a.o -Wl,--no-as-needed $t/b.so $t/c.so

readelf --dynamic $t/exe > $t/readelf
grep -Fq 'Shared library: [libfoo.so]' $t/readelf
grep -Fq 'Shared library: [libbar.so]' $t/readelf

$CC -B. -o $t/exe $t/a.o -Wl,--as-needed $t/b.so $t/c.so

readelf --dynamic $t/exe > $t/readelf
grep -Fq 'Shared library: [libfoo.so]' $t/readelf
! grep -Fq 'Shared library: [libbar.so]' $t/readelf || false
