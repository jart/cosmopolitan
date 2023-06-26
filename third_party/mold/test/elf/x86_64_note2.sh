// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

# Binutils 2.32 injects their own .note.gnu.property section interfering with the tests
test_cflags -Xassembler -mx86-used-note=no && CFLAGS="-Xassembler -mx86-used-note=no" || CFLAGS=""

cat <<EOF | $CC $CFLAGS -o $t/a.o -c -x assembler -
.section .note.a, "a", @note
.p2align 3
.quad 42
EOF

cat <<EOF | $CC $CFLAGS -o $t/b.o -c -x assembler -
.section .note.b, "a", @note
.p2align 2
.quad 42
EOF

cat <<EOF | $CC $CFLAGS -o $t/c.o -c -x assembler -
.section .note.c, "a", @note
.p2align 3
.quad 42
EOF

cat <<EOF | $CC $CFLAGS -o $t/d.o -c -xc -
int main() {}
EOF

./mold -o $t/exe $t/a.o $t/b.o $t/c.o $t/d.o

readelf --segments $t/exe > $t/log
grep -Fq '01     .note.a .note.c .note.b' $t/log
