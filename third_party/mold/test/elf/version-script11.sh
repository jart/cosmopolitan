// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<'EOF' > $t/a.ver
VER_X1 { global: *; local: b*; };
EOF

cat <<EOF | $CXX -fPIC -c -o $t/b.o -xc -
void foo() {}
void bar() {}
void baz() {}
EOF

$CC -B. -shared -Wl,--version-script=$t/a.ver -o $t/c.so $t/b.o

readelf --dyn-syms $t/c.so > $t/log
grep -q 'foo@@VER_X1$' $t/log
! grep -q ' bar$' $t/log || false
! grep -q ' baz$' $t/log || false
