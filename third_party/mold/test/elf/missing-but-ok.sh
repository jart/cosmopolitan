// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -x assembler -
  .text
  .globl _start, foo
_start:
  nop
EOF

./mold -o $t/exe $t/a.o
