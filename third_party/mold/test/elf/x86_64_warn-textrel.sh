// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Skip if libc is musl
ldd --help 2>&1 | grep -q musl && skip

# Skip if target is not x86-64
[ $MACHINE = x86_64 ] || skip

cat <<'EOF' | $CC -c -o $t/a.o -x assembler -
.globl fn
fn:
  movabs main, %rax
  ret
EOF

cat <<EOF | $CC -c -o $t/b.o -fPIC -xc -
void fn();
int main() { fn(); }
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o -pie -Wl,-warn-textrel >& $t/log
grep -q 'relocation against symbol `main'\'' in read-only section' $t/log
grep -q 'creating a DT_TEXTREL in an output file' $t/log
