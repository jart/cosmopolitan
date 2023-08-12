// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/libfoo.so -shared -fPIC -Wl,-soname,libfoo.so -xc -
int fn1() { return 42; }
EOF

cat <<EOF | $CC -o $t/libbar.so -shared -fPIC -Wl,-soname,libbar.so -xc -
int fn1();
int fn2() { return fn1(); }
EOF

cat <<EOF | $CC -o $t/a.o -c -xc -
int fn2();
int main() { fn2(); }
EOF

$CC -B. -o $t/exe1 $t/a.o -L$t -Wl,--as-needed -lbar -Wl,--allow-shlib-undefined
readelf -W --dynamic $t/exe1 > $t/log1
! grep -q libfoo $t/log1 || false

$CC -B. -o $t/exe2 $t/a.o -L$t -Wl,--as-needed -lbar -lfoo
readelf -W --dynamic $t/exe2 > $t/log2
grep -q libfoo $t/log2
