// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

test_cflags -static || skip

[ $MACHINE = x86_64 ] || skip

# Binutils 2.32 injects their own .note.gnu.property section interfering with the tests
test_cflags -Xassembler -mx86-used-note=no && CFLAGS="-Xassembler -mx86-used-note=no" || CFLAGS=""

cat <<EOF | $CC $CFLAGS -o $t/a.o -c -x assembler -
.text
.globl _start
_start:
  nop

.section .note.foo, "a", @note
.p2align 3
.quad 42

.section .note.bar, "a", @note
.p2align 2
.quad 42

.section .note.baz, "a", @note
.p2align 3
.quad 42

.section .note.nonalloc, "", @note
.p2align 0
.quad 42
EOF

./mold -static -o $t/exe $t/a.o
readelf -W --sections $t/exe > $t/log

grep -Eq '.note.bar\s+NOTE.+000008 00   A  0   0  4' $t/log
grep -Eq '.note.baz\s+NOTE.+000008 00   A  0   0  8' $t/log
grep -Eq '.note.nonalloc\s+NOTE.+000008 00      0   0  1' $t/log

readelf --segments $t/exe > $t/log
grep -Fq '01     .note.baz .note.foo .note.bar' $t/log
! grep -q 'NOTE.*0x0000000000000000 0x0000000000000000' $t/log || false
