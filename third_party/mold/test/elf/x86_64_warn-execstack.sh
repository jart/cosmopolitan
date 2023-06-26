// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -o $t/a.o -c -xassembler -
.section .note.GNU-stack, "x"
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -
int main() {}
EOF

$GCC -B. -o $t/exe $t/a.o $t/b.o 2>&1 | grep -q 'may cause a segmentation fault'
