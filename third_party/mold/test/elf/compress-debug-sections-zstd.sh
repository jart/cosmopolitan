// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# arm-linux-gnueabihf-objcopy crashes on x86-64
[ $MACHINE = arm ] && skip
[ $MACHINE = riscv32 ] && skip

command -v zstdcat >& /dev/null || skip

cat <<EOF | $CC -c -g -o $t/a.o -xc -
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

$CC -B. -o $t/exe $t/a.o -Wl,--compress-debug-sections=zstd
$OBJCOPY --dump-section .debug_info=$t/debug_info $t/exe
dd if=$t/debug_info of=$t/debug_info.zstd bs=24 skip=1 status=none
zstdcat $t/debug_info.zstd > /dev/null
