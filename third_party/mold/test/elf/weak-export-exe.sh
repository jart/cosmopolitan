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

__attribute__((weak)) int foo();

int main() {
  printf("%d\n", foo ? foo() : 3);
}
EOF

$CC -B. -o $t/exe $t/a.o
! readelf --dyn-syms $t/exe | grep -q 'WEAK   DEFAULT  UND foo' || false
$QEMU $t/exe | grep -q '^3$'
