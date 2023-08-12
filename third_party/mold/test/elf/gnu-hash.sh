// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -xc -
void foo() {}
void bar() {}
static void baz() {}
EOF

$CC -B. -o $t/b.so $t/a.o -Wl,-hash-style=gnu -shared
