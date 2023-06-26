// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -o $t/a.o -c -x assembler -
  .globl foo
  .data
foo:
  .short foo
EOF

! ./mold -e foo -o $t/exe $t/a.o 2> $t/log || false
grep -Fq 'relocation R_X86_64_16 against foo out of range' $t/log
