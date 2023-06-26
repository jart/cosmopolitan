// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -fPIC -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void foo() {
  printf("Hello world\n");
}
EOF

$CC -B. -shared -o $t/b.so $t/a.o -Wl,-z,now
readelf --dynamic $t/b.so | grep -q 'Flags: NOW'

$CC -B. -shared -o $t/b.so $t/a.o -Wl,-z,now,-z,lazy
readelf --dynamic $t/b.so > $t/log
! grep -q 'Flags: NOW' $t/log || false
