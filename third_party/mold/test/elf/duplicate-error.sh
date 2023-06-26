// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -x assembler -
  .text
  .globl main
main:
  nop
EOF

! ./mold -o $t/exe $t/a.o $t/a.o 2> $t/log || false
grep -q 'duplicate symbol: .*\.o: .*\.o: main' $t/log
