// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# OneTBB isn't tsan-clean
nm mold | grep -q '__tsan_init' && skip

cat <<EOF | $CC -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
void hello() { printf("Hello world\n"); }
EOF

cat <<EOF | $CC -c -o $t/b.o -xc -
void hello();
int main() { hello(); }
EOF

echo "INPUT($t/a.o $t/b.o)" > $t/c.script

./mold --relocatable -o $t/d.o $t/c.script

$CC -B. -o $t/exe $t/d.o
$QEMU $t/exe | grep -q Hello
