// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -fPIC -
void foo() {}
EOF

$CC -B. -shared -o $t/b.so $t/a.o -Wl,-rpath=/foo
readelf --dynamic $t/b.so | grep -q 'RUNPATH.*/foo'

$CC -B. -shared -o $t/b.so $t/a.o -Wl,-rpath=/foo -Wl,-enable-new-dtags
readelf --dynamic $t/b.so | grep -q 'RUNPATH.*/foo'

$CC -B. -shared -o $t/b.so $t/a.o -Wl,-rpath=/foo -Wl,-disable-new-dtags
readelf --dynamic $t/b.so | grep -q 'RPATH.*/foo'
