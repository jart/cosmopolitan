// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -fPIC -xc -o $t/a.o -
extern int foo;
int x() { return foo; }
EOF

cat <<EOF | $CC -c -fPIC -xc -o $t/b.o -
extern int foo;
int y() { return foo; }
int main() {}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o -Wl,--warn-unresolved-symbols,--warn-once >& $t/log

[ "$(grep 'undefined symbol:.* foo$' $t/log | wc -l)" = 1 ]
