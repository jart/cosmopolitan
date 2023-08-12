// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'VER1 { foo[12]; }; VER2 {};' > $t/a.ver

cat <<EOF > $t/b.s
.globl foo1, foo2, foo3
foo1:
  nop
foo2:
  nop
foo3:
  nop
EOF

$CC -B. -shared -o $t/c.so -Wl,-version-script,$t/a.ver $t/b.s
readelf --dyn-syms $t/c.so > $t/log
grep -q ' foo1@@VER1$' $t/log
grep -q ' foo2@@VER1$' $t/log
! grep -q ' foo3@@VER1$' $t/log || false
