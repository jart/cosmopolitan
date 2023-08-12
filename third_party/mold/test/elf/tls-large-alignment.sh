// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -std=c11 -c -o $t/a.o -xc -
__attribute__((section(".tdata1")))
_Thread_local int x = 42;
EOF

cat <<EOF | $CC -fPIC -std=c11 -c -o $t/b.o -xc -
__attribute__((section(".tdata2")))
_Alignas(256) _Thread_local int y[] = { 1, 2, 3 };
EOF

cat <<EOF | $CC -fPIC -c -o $t/c.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

extern _Thread_local int x;
extern _Thread_local int y[];

int main() {
  printf("%d %d %d %d\n", x, y[0], y[1], y[2]);
}
EOF

$CC -B. -shared -o $t/d.so $t/a.o $t/b.o

$CC -B. -o $t/exe1 $t/a.o $t/b.o $t/c.o
$QEMU $t/exe1 | grep -q '^42 1 2 3$'

$CC -B. -o $t/exe2 $t/c.o $t/d.so
$QEMU $t/exe2 | grep -q '^42 1 2 3$'
