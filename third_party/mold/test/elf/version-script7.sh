// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<'EOF' > $t/a.ver
VER_X1 { *; };
EOF

cat <<EOF | $CXX -fPIC -c -o $t/b.o -xc -
extern int foo;
int bar() { return foo; }
EOF

$CC -B. -shared -Wl,--version-script=$t/a.ver -o $t/c.so $t/b.o

readelf --dyn-syms $t/c.so > $t/log
grep -q 'foo$' $t/log
grep -q 'bar@@VER_X1' $t/log
