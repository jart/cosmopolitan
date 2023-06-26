// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -xc -o $t/a.o -
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

$CC -B. -o $t/exe1 $t/a.o -Wl,-z,relro,-z,lazy
$QEMU $t/exe1 | grep -q 'Hello world'
readelf --segments -W $t/exe1 > $t/log1
grep -q 'GNU_RELRO ' $t/log1

$CC -B. -o $t/exe2 $t/a.o -Wl,-z,relro,-z,now
$QEMU $t/exe2 | grep -q 'Hello world'
readelf --segments -W $t/exe2 > $t/log2
grep -q 'GNU_RELRO ' $t/log2

$CC -B. -o $t/exe3 $t/a.o -Wl,-z,norelro
$QEMU $t/exe3 | grep -q 'Hello world'
readelf --segments -W $t/exe3 > $t/log3
! grep -q 'GNU_RELRO ' $t/log3 || false
