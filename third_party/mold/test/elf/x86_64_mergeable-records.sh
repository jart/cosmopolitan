// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Skip if target is not x86-64
[ $MACHINE = x86_64 ] || skip

test_cflags -static || skip

cat <<'EOF' | $CC -o $t/a.o -c -x assembler -
  .text
  .globl main
main:
  sub $8, %rsp

  mov $.L.str1, %rdi
  xor %rax, %rax
  call printf

  mov $.L.str1+1, %rdi
  xor %rax, %rax
  call printf

  mov $str2+2, %rdi
  xor %rax, %rax
  call printf

  mov $.L.str3+3, %rdi
  xor %rax, %rax
  call printf

  mov $.rodata.cst8+16, %rdi
  xor %rax, %rax
  call printf

  xor %rax, %rax
  add $8, %rsp
  ret

  .section .rodata.cst8, "aM", @progbits, 8
  .align 8
.L.str1:
  .ascii "abcdef\n\0"
.globl str2
str2:
  .ascii "ghijkl\n\0"
.L.str3:
  .ascii "mnopqr\n\0"
EOF

$CC -B. -static -o $t/exe $t/a.o

$QEMU $t/exe | grep -q '^abcdef$'
$QEMU $t/exe | grep -q '^bcdef$'
$QEMU $t/exe | grep -q '^ijkl$'
$QEMU $t/exe | grep -q '^pqr$'
$QEMU $t/exe | grep -q '^mnopqr$'
