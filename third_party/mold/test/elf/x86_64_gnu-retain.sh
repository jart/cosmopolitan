// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

echo '.section foo,"R"' | $CC -o /dev/null -c -xassembler - 2> /dev/null || skip

cat <<EOF | $CC -o $t/a.o -c -xc -
int main() {}
EOF

cat <<EOF | $CC -o $t/b.o -c -xassembler -
.section .text.foo, "aR", @progbits
.globl foo
foo:
  ret
EOF

cat <<EOF | $CC -o $t/c.o -c -xassembler -
.section .text.foo, "a", @progbits
.globl foo
foo:
  ret
EOF

$CC -B. -o $t/exe1 $t/a.o $t/b.o -Wl,-gc-sections
nm $t/exe1 | grep -q foo

$CC -B. -o $t/exe1 $t/a.o $t/c.o -Wl,-gc-sections
nm $t/exe1 > $t/log
! grep -q foo $t/log || false
