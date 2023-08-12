// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -fPIC -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int main() {
  puts("Hello world");
}
EOF

$CC -B. -o $t/exe $t/a.o -Wl,-emit-relocs
$QEMU $t/exe | grep -q 'Hello world'

readelf -S $t/exe | grep -Eq 'rela?\.text'
