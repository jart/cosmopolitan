// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<'EOF' > $t/a.ver
{
global:
  *;
  *foo_*;
local:
  *foo*;
};
EOF

cat <<EOF | $CXX -fPIC -c -o $t/b.o -xc -
void xyz() {}
void foo_bar() {}
void foo123() {}
EOF

$CC -B. -shared -Wl,--version-script=$t/a.ver -o $t/c.so $t/b.o

readelf --dyn-syms $t/c.so > $t/log
grep -q ' xyz$' $t/log
grep -q ' foo_bar$' $t/log
! grep -q ' foo$' $t/log || false
