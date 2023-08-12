// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

test_cflags -static-pie || skip

cat <<EOF | $CC -o $t/a.o -c -xc - -fPIE
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

$CC -B. -o $t/exe1 $t/a.o -static-pie
$QEMU $t/exe1 | grep -q 'Hello world'

$CC -B. -o $t/exe2 $t/a.o -static-pie -Wl,--no-relax
$QEMU $t/exe2 | grep -q 'Hello world'
