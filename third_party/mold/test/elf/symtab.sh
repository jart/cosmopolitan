// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -x assembler -
  .globl foo, bar, this_is_global
local1:
foo:
bar:
  .byte 0
EOF

cat <<EOF | $CC -o $t/b.o -c -x assembler -
  .globl this_is_global
local2:
this_is_global:

  .globl module_local
module_local:
EOF

echo '{ local: module_local; global: *; };' > $t/c.map

./mold -o $t/exe $t/a.o $t/b.o --version-script=$t/c.map

readelf --symbols $t/exe > $t/log

grep -Eq '0 NOTYPE  LOCAL  DEFAULT .* local1' $t/log
grep -Eq '0 NOTYPE  LOCAL  DEFAULT .* local2' $t/log
grep -Eq '0 NOTYPE  LOCAL  DEFAULT .* module_local' $t/log
grep -Eq '0 NOTYPE  GLOBAL DEFAULT .* foo' $t/log
grep -Eq '0 NOTYPE  GLOBAL DEFAULT .* bar' $t/log
grep -Eq '0 NOTYPE  GLOBAL DEFAULT .* this_is_global' $t/log
