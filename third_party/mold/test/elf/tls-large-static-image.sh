// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
_Thread_local int x[] = { 1, 2, 3, [10000] = 5 };
EOF

cat <<EOF | $CC -fPIC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
extern _Thread_local int x[];

int main() {
  printf("%d %d %d %d %d\n", x[0], x[1], x[2], x[3], x[10000]);
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe | grep -q '^1 2 3 0 5$'
