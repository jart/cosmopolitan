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
  return 0;
}
EOF

$CC -B. -no-pie -o $t/exe1 $t/a.o -Wl,--image-base=0x8000000
$QEMU $t/exe1 | grep -q 'Hello world'
readelf -W --sections $t/exe1 | grep -Eq '.interp\s+PROGBITS\s+0*8000...\b'

cat <<EOF | $CC -o $t/b.o -c -xc -
void _start() {}
EOF

if [ $MACHINE = x86-64 -o $MACHINE = aarch64 ]; then
  $CC -B. -no-pie -o $t/exe2 $t/b.o -nostdlib -Wl,--image-base=0xffffffff80000000
  readelf -W --sections $t/exe2 | grep -Eq '.interp\s+PROGBITS\s+ffffffff80000...\b'
fi
