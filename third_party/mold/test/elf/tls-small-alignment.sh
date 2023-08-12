// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xassembler -
.text
.byte 0
EOF

cat <<EOF | $CC -fPIC -std=c11 -c -o $t/b.o -xc -
_Thread_local char x = 42;
EOF

cat <<EOF | $CC -fPIC -c -o $t/c.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

extern _Thread_local char x;

int main() {
  printf("%d\n", x);
}
EOF

$CC -B. -shared -o $t/d.so $t/a.o $t/b.o

$CC -B. -o $t/exe1 $t/a.o $t/b.o $t/c.o
$QEMU $t/exe1 | grep -q '^42$'

$CC -B. -o $t/exe2 $t/c.o $t/d.so
$QEMU $t/exe2 | grep -q '^42$'
