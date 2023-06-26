// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc - -fPIC
__attribute__((weak)) int foo();
int bar() { return foo ? foo() : 0; }
EOF

$CC -B. -shared -o $t/b.so $t/a.o
$CC -B. -shared -o $t/c.so $t/a.o -Wl,-z,dynamic-undefined-weak
$CC -B. -shared -o $t/d.so $t/a.o -Wl,-z,nodynamic-undefined-weak

cat <<EOF | $CC -o $t/e.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int foo() { return 1; }
int bar();
int main() { printf("%d\n", bar()); }
EOF

$CC -B. -o $t/exe1 $t/e.o $t/b.so
$QEMU $t/exe1 | grep -q 1

$CC -B. -o $t/exe2 $t/e.o $t/c.so
$QEMU $t/exe2 | grep -q 1

$CC -B. -o $t/exe3 $t/e.o $t/d.so
$QEMU $t/exe3 | grep -q 0
