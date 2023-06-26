// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'VER1 { extern "C++" {}; foo; }; VER2 {};' > $t/a.ver

cat <<EOF > $t/b.s
.globl foo, bar, baz
foo:
  nop
bar:
  nop
baz:
  nop
EOF

$CC -B. -shared -o $t/c.so -Wl,-version-script,$t/a.ver $t/b.s
readelf --dyn-syms $t/c.so > $t/log
grep -q ' foo@@VER1$' $t/log
