// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# musl doesn't work with `-z noseparate-code`
ldd --help 2>&1 | grep -q musl && skip

cat <<EOF | $CC -o $t/a.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int main() {
  printf("Hello world\n");
}
EOF

$CC -B. -o $t/exe1 $t/a.o -Wl,-z,separate-loadable-segments
$QEMU $t/exe1 | grep -q 'Hello world'

$CC -B. -o $t/exe2 $t/a.o -Wl,-z,separate-code -Wl,-z,norelro
$QEMU $t/exe2 | grep -q 'Hello world'

$CC -B. -o $t/exe3 $t/a.o -Wl,-z,noseparate-code -Wl,-z,norelro
$QEMU $t/exe3 | grep -q 'Hello world'
