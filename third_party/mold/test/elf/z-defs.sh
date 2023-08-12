// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
void foo();
void bar() { foo(); }
EOF

$CC -B. -shared -o $t/b.so $t/a.o
$CC -B. -shared -o $t/b.so $t/a.o -Wl,-z,undefs

! $CC -B. -shared -o $t/b.so $t/a.o -Wl,-z,defs 2> $t/log || false
grep -q 'undefined symbol:.* foo' $t/log

! $CC -B. -shared -o $t/b.so $t/a.o -Wl,-no-undefined 2> $t/log || false
grep -q 'undefined symbol:.* foo' $t/log

$CC -B. -shared -o $t/c.so $t/a.o -Wl,-z,defs -Wl,--warn-unresolved-symbols 2> $t/log
grep -q 'undefined symbol:.* foo$' $t/log
