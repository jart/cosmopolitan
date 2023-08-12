// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -x assembler -
  .text
  .globl _start
_start:
  nop
EOF

./mold -o $t/b.so $t/a.o -auxiliary foo -f bar -shared

readelf --dynamic $t/b.so > $t/log
grep -Fq 'Auxiliary library: [foo]' $t/log
grep -Fq 'Auxiliary library: [bar]' $t/log
