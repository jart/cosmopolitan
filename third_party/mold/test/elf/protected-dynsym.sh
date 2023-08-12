// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
extern int foo;
EOF

cat <<EOF | $CC -fPIC -c -o $t/b.o -fcommon -xc -
__attribute__((visibility("protected"))) int foo;
EOF

$CC -B. -shared -o $t/c.so $t/a.o $t/b.o -Wl,-strip-all
readelf --symbols $t/c.so | grep -Eq 'PROTECTED\b.*\bfoo\b'

cat <<EOF | $CC -fPIC -c -o $t/d.o -fno-common -xc -
__attribute__((visibility("protected"))) int foo;
EOF

$CC -B. -shared -o $t/e.so $t/a.o $t/d.o -Wl,-strip-all
readelf --symbols $t/e.so | grep -Eq 'PROTECTED\b.*\bfoo\b'
