// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int foo() { return 3; }
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -
int bar() { return 3; }
EOF

cat <<EOF | $CC -o $t/c.o -c -xc -
int main() {}
EOF

$CC -B. -o $t/exe -Wl,-start-lib $t/a.o -Wl,-end-lib $t/b.o $t/c.o
nm $t/exe > $t/log
! grep -q ' foo$' $t/log || false
grep -q ' bar$' $t/log
