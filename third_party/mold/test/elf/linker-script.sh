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

cat <<EOF > $t/script
GROUP("$t/a.o")
EOF

$CC -B. -o $t/exe $t/script
$QEMU $t/exe | grep -q 'Hello world'

$CC -B. -o $t/exe -Wl,-T,$t/script
$QEMU $t/exe | grep -q 'Hello world'

$CC -B. -o $t/exe -Wl,--script,$t/script
$QEMU $t/exe | grep -q 'Hello world'
