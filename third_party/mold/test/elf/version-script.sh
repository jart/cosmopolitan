// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'ver_x { global: *; };' > $t/a.ver

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
readelf --version-info $t/c.so > $t/log

grep -Fq 'Rev: 1  Flags: none  Index: 2  Cnt: 1  Name: ver_x' $t/log
