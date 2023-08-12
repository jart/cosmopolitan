// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -fPIC -xc -o $t/a.o -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int times = -1; /* times collides with clock_t times(struct tms *buffer); */

int
main ()
{
  printf ("times: %d\n", times);
  return 0;
}
EOF

$CC -B. -shared -o $t/a.so $t/a.o >& $t/log

grep -q "warning: symbol type mismatch: times" $t/log
