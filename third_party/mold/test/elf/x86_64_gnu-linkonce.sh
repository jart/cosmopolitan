// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -o $t/a.o -c -x assembler -
.globl __x86.get_pc_thunk.bx
.section .gnu.linkonce.t.__x86.get_pc_thunk.bx,"ax"
__x86.get_pc_thunk.bx:
  call printf@PLT
EOF

cat <<EOF | $CC -o $t/b.o -c -x assembler -
.globl __x86.get_pc_thunk.bx
.section .text.__x86.get_pc_thunk.bx,"axG",@progbits,foobar,comdat
__x86.get_pc_thunk.bx:
  call puts@PLT
EOF

cat <<EOF | $CC -o $t/c.o -c -xc -
int main() {}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o $t/c.o
$OBJDUMP -d $t/exe | grep -A1 '<__x86.get_pc_thunk.bx>:' | \
  grep -Fq 'puts$plt'
