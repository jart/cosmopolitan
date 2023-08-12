// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

_Thread_local char x1 = 42;

int main() {
  printf("%d\n", x1);
}
EOF

$CC -B. -o $t/exe1 $t/a.o -pie -Wl,-section-start=.tdata=0x100001 -Wl,-relax
$QEMU $t/exe1 | grep -q '^42$'

$CC -B. -o $t/exe2 $t/a.o -pie -Wl,-section-start=.tdata=0x100001 -Wl,-no-relax
$QEMU $t/exe2 | grep -q '^42$'

$CC -B. -o $t/exe3 $t/a.o -pie -Wl,-section-start=.tdata=0x10000f -Wl,-relax
$QEMU $t/exe3 | grep -q '^42$'

$CC -B. -o $t/exe4 $t/a.o -pie -Wl,-section-start=.tdata=0x10000f -Wl,-no-relax
$QEMU $t/exe4 | grep -q '^42$'
