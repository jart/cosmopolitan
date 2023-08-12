// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

test_cflags -static || skip

# Skip if target is not x86-64
[ $MACHINE = x86_64 ] || skip

cat <<'EOF' | $CC -o $t/a.o -c -x assembler -
  .text
  .globl main
main:
  sub $8, %rsp
  mov $.L.str+3, %rdi
  xor %rax, %rax
  call printf
  mov $.rodata.str1.1+16, %rdi
  xor %rax, %rax
  call printf
  xor %rax, %rax
  add $8, %rsp
  ret

  .section .rodata.str1.1, "aMS", @progbits, 1
  .string "bar"
.L.str:
foo:
  .string "xyzHello"
  .string "foo world\n"
EOF

$CC -B. -static -o $t/exe $t/a.o
$QEMU $t/exe | grep -q 'Hello world'

readelf -sW $t/exe | grep -Eq '[0-9] foo$'
