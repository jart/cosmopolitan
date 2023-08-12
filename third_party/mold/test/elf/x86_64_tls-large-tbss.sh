// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || [[ $MACHINE = riscv* ]] || skip

cat <<EOF | $CC -c -o $t/a.o -x assembler -
.globl x, y
.section .tbss,"awT",@nobits
x:
.zero 1024
.section .tcommon,"awT",@nobits
y:
.zero 1024
EOF

cat <<EOF | $CC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

extern _Thread_local char x[1024000];
extern _Thread_local char y[1024000];

int main() {
  x[0] = 3;
  x[1023] = 5;
  printf("%d %d %d %d %d %d\n", x[0], x[1], x[1023], y[0], y[1], y[1023]);
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe | grep -q '^3 0 5 0 0 0$'
