// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -o $t/a.o -c -xc -
__attribute__((weak)) int fn1();

int main() {
  if (fn1)
    fn1();
}
EOF

cat <<EOF | $CC -o $t/libfoo.so -shared -fPIC -Wl,-soname,libfoo.so -xc -
int fn1() { return 42; }
EOF

cat <<EOF | $CC -o $t/libbar.so -shared -fPIC -Wl,-soname,libbar.so -xc -
int fn2() { return 42; }
EOF

$CC -o $t/exe1 $t/a.o -Wl,-no-as-needed -L$t -lbar -lfoo

readelf --dynamic $t/exe1 > $t/log1
grep -Fq 'Shared library: [libfoo.so]' $t/log1
grep -Fq 'Shared library: [libbar.so]' $t/log1

$CC -o $t/exe2 $t/a.o -Wl,-as-needed -L$t -lbar -lfoo

readelf --dynamic $t/exe2 > $t/log2
! grep -Fq 'Shared library: [libfoo.so]' $t/log2 || false
! grep -Fq 'Shared library: [libbar.so]' $t/log2 || false
