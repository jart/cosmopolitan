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

$CC -B. -o $t/b.so $t/a.o -shared
$CC -B. -o $t/c.so $t/a.o -shared -Wl,-z,defs

readelf --dyn-syms $t/b.so | grep -q 'WEAK   DEFAULT  UND foo'
readelf --dyn-syms $t/c.so | grep -q 'WEAK   DEFAULT  UND foo'
