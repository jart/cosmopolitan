// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Skip if target is not x86-64
[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -o $t/a.o -c -x assembler -Wa,-mrelax-relocations=yes -
.globl bar
bar:
  mov foo@GOTPCREL(%rip), %rax
  mov foo@GOTPCREL(%rip), %rcx
  mov foo@GOTPCREL(%rip), %rdx
  mov foo@GOTPCREL(%rip), %rbx
  mov foo@GOTPCREL(%rip), %rbp
  mov foo@GOTPCREL(%rip), %rsi
  mov foo@GOTPCREL(%rip), %rdi
  mov foo@GOTPCREL(%rip), %r8
  mov foo@GOTPCREL(%rip), %r9
  mov foo@GOTPCREL(%rip), %r10
  mov foo@GOTPCREL(%rip), %r11
  mov foo@GOTPCREL(%rip), %r12
  mov foo@GOTPCREL(%rip), %r13
  mov foo@GOTPCREL(%rip), %r14
  mov foo@GOTPCREL(%rip), %r15

  call *foo@GOTPCREL(%rip)
  jmp  *foo@GOTPCREL(%rip)
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -
void foo() {}

int main() {
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o
$OBJDUMP -d $t/exe | grep -A20 '<bar>:' > $t/log

grep -Eq 'lea \s*0x.+\(%rip\),%rax .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%rcx .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%rdx .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%rbx .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%rbp .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%rsi .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%rdi .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%r8  .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%r9  .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%r10 .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%r11 .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%r12 .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%r13 .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%r14 .*<foo>' $t/log
grep -Eq 'lea \s*0x.+\(%rip\),%r15 .*<foo>' $t/log
grep -Eq 'call.*<foo>' $t/log
grep -Eq 'jmp.*<foo>' $t/log
