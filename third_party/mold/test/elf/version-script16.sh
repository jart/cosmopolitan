// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<'EOF' > $t/a.ver
{ local: *; global: extern "C++" { *foo*; }; };
EOF

cat <<EOF | $CC -fPIC -c -o $t/b.o -xc -
void foobar() {}
EOF

$CC -B. -shared -Wl,--version-script=$t/a.ver -o $t/c.so $t/b.o
readelf --dyn-syms $t/c.so | grep -q foobar
