// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

seq 1 100000 | sed 's/.*/.section .data.\0,"aw"\n.word 0\n/g' | \
  $CC -c -xassembler -o $t/a.o -

cat <<'EOF' | $CC -c -xc -o $t/b.o -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int main() {
  printf("Hello\n");
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe | grep -q Hello
