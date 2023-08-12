// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

test_cflags -static || skip

cat <<EOF | $CC -c -o $t/a.o -xc - -fno-PIC
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

$CC -B. $t/a.o -o $t/exe -static -Wl,--omagic
readelf -W --segments $t/exe | grep -qw RWE
