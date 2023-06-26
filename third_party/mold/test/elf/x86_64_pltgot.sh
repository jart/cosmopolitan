// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -fPIC -shared -Wl,-z,noexecstack -o $t/a.so -x assembler -
.globl ext1, ext2
ext1:
  nop
ext2:
  nop
EOF

cat <<EOF | $CC -c -o $t/b.o -x assembler -
.globl _start
_start:
  call ext1@PLT
  call ext2@PLT
  mov ext2@GOTPCREL(%rip), %rax
  ret
EOF

./mold -z separate-loadable-segments -pie -o $t/exe $t/b.o $t/a.so

$OBJDUMP -d -j .plt.got $t/exe > $t/log

grep -Eq '1034:.*jmp.* <ext2>' $t/log
