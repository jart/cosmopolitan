// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<'EOF' | $CC -o $t/a.o -c -x assembler -
  .text
  .globl main
main:
  sub $8, %rsp
  lea msg(%rip), %rdi
  xor %rax, %rax
  call printf@PLT
  xor %rax, %rax
  add $8, %rsp
  ret

  .data
msg:
  .string "Hello world\n"
EOF

$CC -B. -o $t/exe $t/a.o

readelf --sections $t/exe | grep -Fq '.got'
readelf --sections $t/exe | grep -Fq '.got.plt'

$QEMU $t/exe | grep -q 'Hello world'
