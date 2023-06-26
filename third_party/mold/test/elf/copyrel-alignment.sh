// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = ppc64 ] && skip
[ $MACHINE = ppc64le ] && skip
[ $MACHINE = alpha ] && skip

cat <<EOF | $CC -fPIC -shared -o $t/a.so -xc -
__attribute__((aligned(32))) int foo = 5;
EOF

cat <<EOF | $CC -fPIC -shared -o $t/b.so -xc -
__attribute__((aligned(8))) int foo = 5;
EOF

cat <<EOF | $CC -fPIC -shared -o $t/c.so -xc -
__attribute__((aligned(256))) int foo = 5;
EOF

cat <<EOF | $CC -fno-PIE -o $t/d.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
extern int foo;
int main() { printf("%d %p\n", foo, &foo); }
EOF

$CC -B. -o $t/exe1 $t/d.o $t/a.so -no-pie
$QEMU $t/exe1 > /dev/null
readelf -W --sections $t/exe1 | grep -q '\.copyrel.* 32$'

$CC -B. -o $t/exe2 $t/d.o $t/b.so -no-pie
$QEMU $t/exe2 > /dev/null
readelf -W --sections $t/exe2 | grep -q '\.copyrel.* 8$'

$CC -B. -o $t/exe3 $t/d.o $t/c.so -no-pie
$QEMU $t/exe3 > /dev/null
readelf -W --sections $t/exe3 | grep -q '\.copyrel.* 256$'
