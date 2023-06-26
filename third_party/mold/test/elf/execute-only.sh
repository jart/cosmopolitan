// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# .text is writable on sparc, which is not compatible with --execute-only
[ $MACHINE = sparc64 ] && skip

# GCC emits data to .text for PPC64, so PPC64 is not compatible with -execute-only
[ $MACHINE = ppc64 ] && skip

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

$CC -B. -o $t/exe $t/a.o -Wl,--execute-only
$QEMU $t/exe | grep -q 'Hello world'
readelf -W --segments $t/exe | grep -Eq 'LOAD\s.*[0-9a-f]   E 0x'
