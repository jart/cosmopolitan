// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

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

$CC -B. -o $t/exe1 $t/a.o -Wl,-z,max-page-size=65536 \
  -Wl,-z,separate-loadable-segments

$QEMU $t/exe1 | grep -q 'Hello world'
readelf -W --segments $t/exe1 | grep -q 'LOAD.*R   0x10000$'

$CC -B. -o $t/exe2 $t/a.o -Wl,-zmax-page-size=$((1024*1024)) \
  -Wl,-z,separate-loadable-segments

$QEMU $t/exe2 | grep -q 'Hello world'
readelf -W --segments $t/exe2 | grep -q 'LOAD.*R   0x100000$'

$CC -B. -o $t/exe3 $t/a.o -Wl,-zmax-page-size=$((1024*1024))

$QEMU $t/exe3 | grep -q 'Hello world'
readelf -W --segments $t/exe3 | grep -q 'LOAD.*R   0x100000$'
